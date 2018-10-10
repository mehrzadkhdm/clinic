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
#include <Wt/WTimeEdit>

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
#include "MarketingPhysicianIssueWidget.h"
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
#include "PhysicianIssueResultWidget.h"
#include "PhysicianVisit.h"
#include "PhysicianIssue.h"


using namespace Wt;
namespace dbo = Wt::Dbo;

MarketingPhysicianIssueWidget::MarketingPhysicianIssueWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    conninfo_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<PhysicianVisit>("physicianvisit");
    dbsession.mapClass<PhysicianIssue>("physicianissue");
    dbsession.mapClass<Comment>("comment");

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

void MarketingPhysicianIssueWidget::issueList()
{

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(5000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        MarketingPhysicianIssueWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();


    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit();
    edit->setStyleClass("search-box");
    edit->setPlaceholderText("Search for an Issue by MD name or PCC name");

    container->addWidget(edit);


    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPractice;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {

       MarketingPhysicianIssueWidget::search(edit->text().toUTF8());

       if (edit->text() == "")
       {
            MarketingPhysicianIssueWidget::startRefreshTimer();
       } else {
               MarketingPhysicianIssueWidget::stopRefreshTimer();
       }

    }));

    addWidget(container);

    MarketingPhysicianIssueWidget::search(Wt::WString(""));
    MarketingPhysicianIssueWidget::startRefreshTimer();

}

void MarketingPhysicianIssueWidget::preSearch(Wt::WString searchString)
{
    MarketingPhysicianIssueWidget::search(Wt::WString(searchString));
    MarketingPhysicianIssueWidget::startRefreshTimer();
}
void MarketingPhysicianIssueWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }

}
void MarketingPhysicianIssueWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";
    }

}
void MarketingPhysicianIssueWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    prContainer = new PhysicianIssueResultWidget(conninfo_, session_, resultStack_);
    prContainer->showPhysicianIssueLog(searchString, limit, offset);

    resultStack_->setCurrentWidget(prContainer);

    prContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        MarketingPhysicianIssueWidget::stopRefreshTimer();
    }));

    prContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        MarketingPhysicianIssueWidget::preSearch(searchString);
    }));
}
