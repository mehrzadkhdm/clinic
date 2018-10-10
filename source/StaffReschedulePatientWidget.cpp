/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std stuff
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
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WStackedWidget>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "StaffReschedulePatientWidget.h"
#include "PatientListContainerWidget.h"
#include "Patient.h"
#include "Practice.h"
#include "Backup.h"
#include "UserSession.h"
#include "Insurance.h"
#include "InsuranceItem.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

StaffReschedulePatientWidget::StaffReschedulePatientWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Backup>("backup");
    dbsession.mapClass<Insurance>("insurance");

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

void StaffReschedulePatientWidget::patientList()
{

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(5000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        StaffReschedulePatientWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();


    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for a Patient");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPatient;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        StaffReschedulePatientWidget::search(upper);

        if (edit->text() == "")
        {
            StaffReschedulePatientWidget::startRefreshTimer();
        } else {
            StaffReschedulePatientWidget::stopRefreshTimer();
        }

    }));

    addWidget(container);

    StaffReschedulePatientWidget::search(Wt::WString(""));
    StaffReschedulePatientWidget::startRefreshTimer();

}

void StaffReschedulePatientWidget::preSearch(Wt::WString searchString)
{
    StaffReschedulePatientWidget::search(Wt::WString(searchString));
    StaffReschedulePatientWidget::startRefreshTimer();
}
void StaffReschedulePatientWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }
}
void StaffReschedulePatientWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}
void StaffReschedulePatientWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    ptContainer = new PatientListContainerWidget(conninfo_, session_, resultStack_);
    ptContainer->showRescheduleSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(ptContainer);

    ptContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        StaffReschedulePatientWidget::stopRefreshTimer();
    }));

    ptContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        StaffReschedulePatientWidget::preSearch(searchString);
    }));

}
