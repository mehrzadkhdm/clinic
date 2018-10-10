/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std io stuff
#include <sstream>
#include <fstream>
#include <iostream>
#include <locale>
//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//wt stuff

#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WText>
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WLabel>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WDialog>
#include <Wt/WTemplate>
#include <Wt/WCalendar>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>

//other header files
#include "PracticeOrderWidget.h"
#include "Order.h"
#include "PracticeSession.h"
#include "Composer.h"

using namespace Wt;
namespace dbo = Wt::Dbo;
namespace bpt = boost::posix_time;

PracticeOrderWidget::PracticeOrderWidget(const char *conninfo, PracticeSession *session, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo),
conninfo_(conninfo),
session_(session)
{
    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Order>("order");
    dbsession.mapClass<CompositeEntity>("composite");

    dbo::Transaction transaction(dbsession);
    try {
        dbsession.createTables();
        log("info") << "Database created";
    }
    catch (...) {
        log("info") << "Using existing database";
    }

    transaction.commit();

}

void PracticeOrderWidget::start()
{
    addWidget(new Wt::WBreak());
    staticRef_ = new Wt::WText();
    staticRef_->setText(setRefId());
    PracticeOrderWidget::demographics();
}

//function to handle sending all to review and displaying
WString PracticeOrderWidget::setRefId()
{
    return uuid();
}

WString PracticeOrderWidget::strRefId()
{
    return staticRef_->text();
}

void PracticeOrderWidget::demographics()
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    container->addWidget(new Wt::WText("<h2>Send Order Forms</h2>"));
    container->addWidget(new Wt::WBreak());

    Wt::WContainerWidget *container1 = new Wt::WContainerWidget();
    container1->setStyleClass("background-box");

    composer_ = new Composer(conninfo_, strRefId(), this);

    container1->addWidget(new Wt::WText("<h4>Upload Study Order Attachments</h4>"));
    container1->addWidget(new Wt::WText("Upload however many orders that you need to send us."));
    container1->addWidget(composer_);

    container->addWidget(container1);
    addWidget(container);
    
    Wt::WPushButton *btnSendOrder = new Wt::WPushButton("Send Order(s)", this);
    btnSendOrder->clicked().connect(std::bind([=]() {
        btnSendOrder->hide();
        container1->hide();
    }));
    btnSendOrder->clicked().connect(this, &PracticeOrderWidget::sendOrder);

}

void PracticeOrderWidget::sendOrder()
{
    Wt::log("notice") << "Practice user clicked send Order request";

    PracticeOrderWidget::saveOrder();

    orderSent_ = new Wt::WText(this);
    orderSent_->setText("Thank you. We will be in contact with your office shortly.");
    addWidget(new Wt::WBreak());

}

//this should create a new Order record in the database and email the staff
void PracticeOrderWidget::saveOrder()
{
    Wt::log("notice") << "Send order triggered saveOrder";
    std::string createTime;

    bpt::ptime now = bpt::second_clock::local_time();
    std::stringstream ss;
    ss << static_cast<int>(now.date().month()) << "/" << now.date().day()
        << "/" << now.date().year();
    createTime = ss.str();

    //get practice details from practice model
    Wt::WString pracName = session_->strPracticeName();
    Wt::WString drFirst = session_->strDrFirstName();
    Wt::WString drLast = session_->strDrLastName();
    Wt::WString pracEmail = session_->strPracticeEmail();


    Transaction transaction(dbsession);

    ptr<Order> r = dbsession.add(new Order());

    r.modify()->repPracticeName_ = pracName.toUTF8();
    r.modify()->repPracticeDrFirst_ = drFirst.toUTF8();
    r.modify()->repPracticeDrLast_ = drLast.toUTF8();
    r.modify()->repPracticeEmail_ = pracEmail.toUTF8();
    r.modify()->repCreatedAt_ = createTime;

    r.modify()->orderId_ = strRefId().toUTF8();

    transaction.commit();

    Mail::Message message;
    message.setFrom(Mail::Mailbox("dl-info@statsleep.com", "United STAT Sleep"));
    message.addRecipient(Mail::To, Mail::Mailbox("dl-info@statsleep.com", "United STAT Sleep"));
    message.setSubject("An online referral was received");
    message.setBody("A referral was submitted online, please login to the sleep study portal to process the referral.");
    message.addHtmlBody ("<p>A referral has been submitted online, please login to the sleep study tool to process the referral.");
    Mail::Client client;
    client.connect("localhost");
    client.send(message);
}
