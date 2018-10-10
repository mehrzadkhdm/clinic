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
#include <boost/date_time/posix_time/posix_time.hpp>

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
#include <Wt/WLink>
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
#include <Wt/WStackedWidget>
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

#include "Referral.h"
#include "InboundContainerWidget.h"
#include "StaffHomeWidget.h"

StaffHomeWidget::StaffHomeWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Referral>("referral");


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
void StaffHomeWidget::showHomeWidget()
{
	refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(30000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        StaffHomeWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    
    Wt::WContainerWidget *top = new Wt::WContainerWidget();
    Wt::WContainerWidget *bottom = new Wt::WContainerWidget();
    
    Wt::log("notice") << "Staff user switched routes to /home.";
    
    top->addWidget(new Wt::WText("<h3>Welcome to the Clinic Portal for United STAT Sleep</h3>"));

    Wt::WContainerWidget *authexpire = new Wt::WContainerWidget();

    Wt::WDate today = Wt::WDate::currentServerDate();
    Wt::WDate onemonth = Wt::WDate::currentServerDate().addDays(30);
    
    dbo::Transaction transaction(dbsession);
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("ref_status != ?").bind("Study Scheduled").where("inbound_status != ?").bind("inbound").where("auth_expires BETWEEN ? AND ?").bind(today).bind(onemonth).orderBy("auth_expires asc");
    std::cerr << referrals.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Birth Date"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Referred From"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Referral Status"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Received"));
    table->elementAt(0, 6)->addWidget(new Wt::WText("Expires"));


    int row = 1;
    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i, ++row)
    {
            
            if (today.daysTo((*i)->authExpiration_) <= 10)
            {
                table->rowAt(row)->setStyleClass("row-auth-ten");
            } else if (today.daysTo((*i)->authExpiration_) > 10  && today.daysTo((*i)->authExpiration_) < 30) {
                table->rowAt(row)->setStyleClass("row-auth-thirty");
            }

            if ((*i)->refPracticeName_ == "")
            {
                Wt::WString fullname = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;
                new Wt::WText(fullname, table->elementAt(row, 3));
            } else {
                new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 3));
            }

            new Wt::WText((*i)->refFirstName_, table->elementAt(row, 0)),
            new Wt::WText((*i)->refLastName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->refDob_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refStatus_, table->elementAt(row, 4)),
            new Wt::WText((*i)->refReceivedAt_.toString("MM/dd/yyyy"), table->elementAt(row, 5)),
            new Wt::WText((*i)->authExpiration_.toString("MM/dd/yyyy"), table->elementAt(row, 6));
    }


    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    authexpire->addWidget(new Wt::WText("<h4 style=\"color: #C9302C\">Unscheduled Authorizations Expiring</h4>"));
    authexpire->addWidget(new Wt::WBreak());
    authexpire->addWidget(table);
    bottom->addWidget(authexpire);
    
    resultStack_ = new Wt::WStackedWidget();
    container->addWidget(top);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("<h4 style=\"color: #000000\">Inbound Referrals</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(resultStack_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(bottom);
    addWidget(container);
    
    StaffHomeWidget::search(Wt::WString(""));
	StaffHomeWidget::startRefreshTimer();
}

void StaffHomeWidget::preSearch(Wt::WString searchString)
{
    StaffHomeWidget::search(Wt::WString(searchString));
    StaffHomeWidget::startRefreshTimer();
}

void StaffHomeWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }

}

void StaffHomeWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}

void StaffHomeWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    inboundContainer = new InboundContainerWidget(conninfo_, session_, resultStack_);
    inboundContainer->showSearchInbound(searchString, limit, offset);

    resultStack_->setCurrentWidget(inboundContainer);
    
    inboundContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        StaffHomeWidget::stopRefreshTimer();
    }));

    inboundContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        StaffHomeWidget::preSearch(searchString);
    }));
}
