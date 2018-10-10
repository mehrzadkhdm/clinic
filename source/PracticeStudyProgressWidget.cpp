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
#include <Wt/WDate>
#include <Wt/WStackedWidget>
#include <Wt/WTimer>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files

#include "PracticeRecentWidget.h"
#include "PracticeStudyProgressWidget.h"
#include "Referral.h"
#include "Practice.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "InsuranceItem.h"
#include "Insurance.h"
#include "PracticeSession.h"


using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InboundRefApp{
    struct InboundReferral {
        Wt::WString pracName;
        Wt::WString pracDrName;
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptCity;

        InboundReferral(const Wt::WString& aPracName,
            const Wt::WString& aPracDrName,
            const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtCity)
            : pracName(aPracName),
            pracDrName(aPracDrName),
            ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptCity(aPtCity) {}
    };
}

namespace InboundRefAttachments{
    struct InboundAttachment {
        Wt::WString fileName;
        Wt::WString fileType;

        InboundAttachment(const Wt::WString& aFileName,
            const Wt::WString& aFileType)
            : fileName(aFileName),
            fileType(aFileType){}
    };
}

PracticeStudyProgressWidget::PracticeStudyProgressWidget(const char *conninfo, Wt::WString *prac, PracticeSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    prac_(prac),
    session_(session),
    conninfo_(conninfo)
{
    Wt::log("info") << "PracticeStudyProgressWidget Instantiation";

    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
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

void PracticeStudyProgressWidget::searchList()
{
    
    Wt::log("info") << "PracticeStudyProgressWidget searchList";

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(5000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        PracticeStudyProgressWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WLineEdit *edit = new Wt::WLineEdit();
    edit->setStyleClass("search-box");
    edit->setPlaceholderText("Search for a referral or sleep study by patient name");
    container->addWidget(new Wt::WText("<h2>Progress Tracking Dashboard</h2>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(edit);



    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPatient;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        PracticeStudyProgressWidget::search(upper);
        
        if (edit->text() == "")
        {
            PracticeStudyProgressWidget::startRefreshTimer();
        } else {
            PracticeStudyProgressWidget::stopRefreshTimer();
        }
        
    }));

    addWidget(container);

    PracticeStudyProgressWidget::search(Wt::WString(""));
    PracticeStudyProgressWidget::startRefreshTimer();

}

void PracticeStudyProgressWidget::preSearch(Wt::WString searchString)
{
    PracticeStudyProgressWidget::search(Wt::WString(searchString));
    PracticeStudyProgressWidget::startRefreshTimer();
}

void PracticeStudyProgressWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }

}

void PracticeStudyProgressWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}

void PracticeStudyProgressWidget::search(Wt::WString searchString)
{

    Wt::log("info") << "PracticeStudyProgressWidget search";
    resultStack_->clear();

    recentContainer = new PracticeRecentWidget(conninfo_, prac_, session_, resultStack_);
    recentContainer->pracRecent(searchString);

    resultStack_->setCurrentWidget(recentContainer);
    
    recentContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        PracticeStudyProgressWidget::stopRefreshTimer();
    }));

    recentContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        PracticeStudyProgressWidget::preSearch(searchString);
    }));
}
