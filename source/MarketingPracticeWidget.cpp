/*
* Copyrefright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std io stuff
#include <sstream>
#include <fstream>
#include <iostream>
//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

//wt stuff
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
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
#include <Wt/WMenuItem>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WDate>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/Utils>
#include <Wt/WTimer>

//other header files
#include "MarketingPracticeWidget.h"
#include "Referral.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"
#include "Insurance.h"
#include "InsuranceItem.h"
#include "StudyType.h"
#include "StudyTypeItem.h"
#include "PracticeListContainerWidget.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

MarketingPracticeWidget::MarketingPracticeWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    conninfo_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Practice>("practice");

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

WString MarketingPracticeWidget::setPracId()
{
    return uuid();
}

void MarketingPracticeWidget::newProvider()
{
    staticPrac_ = new Wt::WText();
    staticPrac_->setText(setPracId());

    Wt::WDialog *newpracdialog = new Wt::WDialog("Add a New Provider");
    newpracdialog->setMinimumSize(1000, 700);
    newpracdialog->setMaximumSize(1000, 700);
    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newpracdialog->contents());
    newprcontainer->setStyleClass("newpr-container");
    Wt::WPushButton *addprac = new Wt::WPushButton("Continue", newpracdialog->contents());
    addprac->setDefault(true);

    //input fields for patient demographics
    Wt::WText *pracNameLbl = new Wt::WText("Practice Business Name");
    pracNameLbl->setStyleClass("label-left-box");
    pracName_ = new Wt::WLineEdit(this);
    pracName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracDrFirstNameLbl = new Wt::WText("Doctor First Name");
    pracDrFirstNameLbl->setStyleClass("label-left-box");
    pracDrFirstName_ = new Wt::WLineEdit(this);
    pracDrFirstName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracDrFirstNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracDrFirstName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracDrLastNameLbl = new Wt::WText("Doctor Last Name");
    pracDrLastNameLbl->setStyleClass("label-left-box");
    pracDrLastName_ = new Wt::WLineEdit(this);
    pracDrLastName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracDrLastNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracDrLastName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracZipLbl = new Wt::WText("Practice Zip");
    pracZipLbl->setStyleClass("label-left-box");
    pracZip_ = new Wt::WLineEdit(this);
    pracZip_->setStyleClass("left-box");
    newprcontainer->addWidget(pracZipLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracZip_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracNpiLbl = new Wt::WText("Practice Phone Number");
    pracNpiLbl->setStyleClass("label-right-box");
    pracNpi_ = new Wt::WLineEdit(this);
    pracNpi_->setStyleClass("right-box");
    newprcontainer->addWidget(pracNpiLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracNpi_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracSpecialtyLbl = new Wt::WText("Practice Specialty");
    pracSpecialtyLbl->setStyleClass("label-right-box");
    pracSpecialty_ = new Wt::WLineEdit(this);
    pracSpecialty_->setStyleClass("right-box");
    newprcontainer->addWidget(pracSpecialtyLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracSpecialty_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracEmailLbl = new Wt::WText("Practice Email Address");
    pracEmailLbl->setStyleClass("label-right-box");
    pracEmail_ = new Wt::WLineEdit(this);
    pracEmail_->setStyleClass("right-box");
    newprcontainer->addWidget(pracEmailLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracEmail_);
    newprcontainer->addWidget(new Wt::WBreak());

    addprac->clicked().connect(std::bind([=]() {

        Wt::WDateTime pracadded = Wt::WDateTime::currentDateTime();
        dbo::Transaction ptrans(dbsession);

        dbo::ptr<Practice> pr = dbsession.add(new Practice());

        pr.modify()->refPracticeName_ = pracName_->text().toUTF8();
        pr.modify()->refPracticeDrFirst_ = pracDrFirstName_->text().toUTF8();
        pr.modify()->refPracticeDrLast_ = pracDrLastName_->text().toUTF8();
        pr.modify()->refPracticeZip_ = pracZip_->text().toUTF8();
        pr.modify()->refPracticeNpi_ = pracNpi_->text().toUTF8();
        pr.modify()->refSpecialty_ = pracSpecialty_->text().toUTF8();
        pr.modify()->refPracticeEmail_ = pracEmail_->text().toUTF8();
        pr.modify()->practiceId_ = staticPrac_->text().toUTF8();
        pr.modify()->pccEmail_ = session_->strUserEmail().toUTF8();
        pr.modify()->dateAdded_ = pracadded;

        ptrans.commit();

        newpracdialog->accept();

    }));

    newprcontainer->addWidget(addprac);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newpracdialog->footer());
    newpracdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newpracdialog, &Wt::WDialog::reject);

    newpracdialog->show();

}


void MarketingPracticeWidget::practiceList()
{

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(30000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        MarketingPracticeWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *newProviderBtn = new Wt::WPushButton("New Provider");

    container->addWidget(newProviderBtn);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit();
    edit->setStyleClass("search-box");
    edit->setPlaceholderText("Search for a Provider by practice name, MD first or last name");

    container->addWidget(edit);


    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPractice;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {

       MarketingPracticeWidget::search(edit->text().toUTF8());

       if (edit->text() == "")
       {
            MarketingPracticeWidget::startRefreshTimer();
       } else {
               MarketingPracticeWidget::stopRefreshTimer();
       }

    }));

    addWidget(container);

    newProviderBtn->clicked().connect(std::bind([=]() {
        MarketingPracticeWidget::newProvider();
    }));

    MarketingPracticeWidget::search(Wt::WString(""));
    MarketingPracticeWidget::startRefreshTimer();

}

void MarketingPracticeWidget::preSearch(Wt::WString searchString)
{
    MarketingPracticeWidget::search(Wt::WString(searchString));
    MarketingPracticeWidget::startRefreshTimer();
}
void MarketingPracticeWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }

}
void MarketingPracticeWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}
void MarketingPracticeWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    prContainer = new PracticeListContainerWidget(conninfo_, session_, resultStack_);
    prContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(prContainer);

    prContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        MarketingPracticeWidget::stopRefreshTimer();
    }));

    prContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        MarketingPracticeWidget::preSearch(searchString);
    }));
}
