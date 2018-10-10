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
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>

//other header files
#include "PracticeReportWidget.h"
#include "Report.h"
#include "PracticeSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

PracticeReportWidget::PracticeReportWidget(const char *conninfo, PracticeSession *session, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo),
session_(session)
{
    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Report>("report");
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

    PracticeReportWidget::demographics();
    addWidget(new Wt::WBreak());
    staticRef_ = new Wt::WText();
    staticRef_->setText(setRefId());
}

//function to handle sending all to review and displaying
WString PracticeReportWidget::setRefId()
{
    return uuid();
}

WString PracticeReportWidget::strRefId()
{
    return staticRef_->text();
}

void PracticeReportWidget::demographics()
{

    Wt::WContainerWidget *outside = new Wt::WContainerWidget();
    outside->setStyleClass("report-outside");
    Wt::WContainerWidget *container1 = new Wt::WContainerWidget();
    container1->setStyleClass("background-box");
    container1->setStyleClass("report-inside");
    outside->addWidget(new Wt::WText("<h2>Request a Patient Sleep Study Report</h2>"));
    outside->addWidget(new Wt::WBreak());

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container1->setLayout(hbox);

    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WText *welcomeReport = new Wt::WText("Please provide the following information to request a report.");
    vbox1->addWidget(welcomeReport);
    Wt::WText *ptFirstNameLbl = new Wt::WText("Patient First Name");
    ptFirstNameLbl->setStyleClass("report-label-left-box");
    patientFirstName_ = new Wt::WLineEdit(this);
    patientFirstName_->setStyleClass("left-box");
    vbox2->addWidget(ptFirstNameLbl);
    vbox2->addWidget(patientFirstName_);
    vbox2->addWidget(new Wt::WBreak());

    Wt::WText *ptLastNameLbl = new Wt::WText("Patient Last Name");
    ptLastNameLbl->setStyleClass("report-label-left-box");
    patientLastName_ = new Wt::WLineEdit(this);
    patientLastName_->setStyleClass("left-box");
    vbox2->addWidget(ptLastNameLbl);
    vbox2->addWidget(patientLastName_);
    vbox2->addWidget(new Wt::WBreak());

    Wt::WText *ptDobLbl = new Wt::WText("Patient Date of Birth (Please add dashes: e.g. 12-12-1999)");
    ptDobLbl->setStyleClass("report-label-left-box");
    patientDob_ = new Wt::WLineEdit(this);
    patientDob_->setStyleClass("left-box");
    vbox2->addWidget(ptDobLbl);
    vbox2->addWidget(patientDob_);
    vbox2->addWidget(new Wt::WBreak());


    Wt::WPushButton *btnDiagnostics = new Wt::WPushButton("Send Request");
    btnDiagnostics->setStyleClass("btn-report-request");

    vbox2->addWidget(btnDiagnostics);

    btnDiagnostics->clicked().connect(std::bind([=]() {
        container1->hide();
        btnDiagnostics->hide();
        outside->hide();
    }));

    btnDiagnostics->clicked().connect(this, &PracticeReportWidget::complete);
    outside->addWidget(container1);
    addWidget(outside);
}


void PracticeReportWidget::complete()
{

    Wt::log("notice") << "Practice user clicked complete";
    //Wt::WPushButton *btnReview = new Wt::WPushButton("Review Now", this);

    Wt::WContainerWidget *container4 = new Wt::WContainerWidget();
    container4->setStyleClass("background-box");

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container4->setLayout(hbox);

    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();

    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    reviewRefId_ = new Wt::WText(this);
    Wt::WText *reviewMessage = new Wt::WText("Is this correct?");

    vbox1->addWidget(reviewMessage);

    reviewFirstName_ = new Wt::WText(this);
    reviewLastName_ = new Wt::WText(this);
    reviewDob_ = new Wt::WText(this);


    vbox1->addWidget(new Wt::WBreak());
    Wt::WPushButton *btnSendOrder = new Wt::WPushButton("Yes, Send Now", this);
    btnSendOrder->setStyleClass("sig-btn");


    vbox2->addWidget(btnSendOrder);
    addWidget(container4);

    btnSendOrder->clicked().connect(this, &PracticeReportWidget::sendReport);
    btnSendOrder->clicked().connect(std::bind([=]() {
        container4->hide();
        btnSendOrder->hide();
        reviewFirstName_->hide();
        reviewLastName_->hide();
        reviewDob_->hide();
    }));

    reviewFirstName_->setText(patientFirstName_->text());
    reviewLastName_->setText(" " + patientLastName_->text());
    reviewDob_->setText(", " + patientDob_->text());
    //pracDrSigName_->setText("Referring MD, Please sign here and select today's date and time.");

}

std::string PracticeReportWidget::strFirstName()
{

      std::string str = reviewFirstName_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReportWidget::strLastName()
{
    std::string str = reviewLastName_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReportWidget::strDob()
{
    std::string str = patientDob_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

void PracticeReportWidget::sendReport()
{
    Wt::log("notice") << "Practice user clicked send report request";

    PracticeReportWidget::saveReport();

    orderSent_ = new Wt::WText(this);
    orderSent_->setText("Thank you. We will be in contact with your office shortly.");
    addWidget(new Wt::WBreak());

}

//this should create a new Report record in the database and email the staff
void PracticeReportWidget::saveReport()
{
    Wt::log("notice") << "Send order triggered saveOrder";

    //get practice details from practice model
    Wt::WString pracName = session_->strPracticeName();
    Wt::WString drFirst = session_->strDrFirstName();
    Wt::WString drLast = session_->strDrLastName();
    Wt::WString pracEmail = session_->strPracticeEmail();


    Transaction transaction(dbsession);

    ptr<Report> r = dbsession.add(new Report());

    r.modify()->repPracticeName_ = pracName.toUTF8();
    r.modify()->repPracticeDrFirst_ = drFirst.toUTF8();
    r.modify()->repPracticeDrLast_ = drLast.toUTF8();
    r.modify()->repPracticeEmail_ = pracEmail.toUTF8();

    r.modify()->patientFirstName_ = strFirstName();
    r.modify()->patientLastName_ = strLastName();
    r.modify()->patientDob_ = strDob();

    r.modify()->reportId_ = strRefId().toUTF8();

    transaction.commit();

    Mail::Message message;
    message.setFrom(Mail::Mailbox("clinicore@statsleep.com", "United STAT Sleep"));
    message.addRecipient(Mail::To, Mail::Mailbox("dl-info@statsleep.com", "United STAT Sleep"));
    message.setSubject("An online request for a Sleep Study report was received");
    message.setBody("A request for a Sleep Study report was submitted online from one of our MDs, please login to the sleep study portal to process the request.");
    message.addHtmlBody ("<p>A request for a sleep study report has been submitted online, please login to the sleep study tool to process the request.");
    Mail::Client client;
    client.connect("localhost");
    client.send(message);
}
