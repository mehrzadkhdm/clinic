/*
* Copyrefright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <stdlib.h>
//std io stuff
#include <sstream>
#include <fstream>
#include <iostream>
//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//json rpc cpp library
#include "stubclient.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

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
#include <Wt/WLink>

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

#include "StaffSleepStudyWidget.h"
#include "TechStudyListWidget.h"
#include "SleepStudy.h"
#include "Referral.h"
#include "Attachment.h"
#include "Patient.h"
#include "Composer.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"
#include "Backup.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;
namespace bpt = boost::posix_time;

using namespace jsonrpc;
using namespace std;
//struct for SleepStudy info
namespace InboundRefApp{
    struct InboundSleepStudy {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptCity;

        InboundSleepStudy(
            const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob,
            const Wt::WString& aPtCity)
            :    ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob),
            ptCity(aPtCity) {}
    };
}

TechStudyListWidget::TechStudyListWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo),
    done_(this),
    selectionChanged_(this)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
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

void TechStudyListWidget::showSearch(Wt::WString loc)
{

    Wt::WDate today;
    //get current  time
    Wt::WDate servernow = Wt::WDate::currentServerDate();
    Wt::WTime noontime = Wt::WTime(20,00,00);
    Wt::WDateTime noon = Wt::WDateTime(servernow, noontime);

    log("info") << "Noon: "<< noon.toString("ddd MMM d hh:mm a yyyy");
    Wt::WDateTime rightnow = Wt::WDateTime::currentDateTime();
    log("info") << "Right Now: " << rightnow.toString("ddd MMM d hh:mm a yyyy");

    if (rightnow < noon)
    {
        log("info") << "Current time is before noon, showing yesterday's patients";
        today = Wt::WDate::currentServerDate().addDays(-1);
        log("info") << "today: " << today.toString("MM/dd/yyyy");
    } else {
        log("info") << "Current time is after noon, showing today's patients";
        today = Wt::WDate::currentServerDate();
        log("info") << "today: " << today.toString("MM/dd/yyyy");

    }

    //check to see if current date time is before noon that day
    //if it is, today is yesterday
    //else, today is today
    Wt::WDate queryToday;
    queryToday = today;

    //show studies that are in
    dbo::Transaction strans(dbsession);

    typedef dbo::collection< dbo::ptr<SleepStudy> > SleepStudies;


    if (loc == "Downey") {

        SleepStudies studies = dbsession.find<SleepStudy>().where("std_date = ?").bind(queryToday).where("std_loco = ?").bind("Downey");
        std::cerr << studies.size() << std::endl;



        Wt::WTable *table = new Wt::WTable();
        table->setHeaderCount(1);

        table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
        table->elementAt(0, 1)->addWidget(new Wt::WText("Patient First Name"));
        table->elementAt(0, 2)->addWidget(new Wt::WText("Patient Last Name"));
        table->elementAt(0, 3)->addWidget(new Wt::WText("Patient Date of Birth"));
        table->elementAt(0, 4)->addWidget(new Wt::WText("Patient Age"));
        table->elementAt(0, 5)->addWidget(new Wt::WText("Appointment Time"));
        table->elementAt(0, 6)->addWidget(new Wt::WText("Appointment Status"));
        table->elementAt(0, 7)->addWidget(new Wt::WText("Details"));




        int row = 1;
        for (SleepStudies::const_iterator i = studies.begin(); i != studies.end(); ++i, ++row)
        {

            WString pdob = (*i)->ptDob_;

            Wt::WDate ptdobin = Wt::WDate::fromString(pdob, "MM/dd/yyyy");
            Wt::WDate today = Wt::WDate::currentServerDate();
            Wt::WString dnysid = (*i)->studyId_;

            int dobdiff = ptdobin.daysTo(today);
            float years = dobdiff / 365;
            log("info") << "DOB: " << ptdobin.toString("MM/dd/yyyy");
            log("info") << "TODAY: " << today.toString("MM/dd/yyyy");
            log("info") << "DOB DIFF: " << dobdiff;
            log("info") << "PT AGE: " << years;

            std::stringstream agestream;
            agestream << std::setprecision(0) << std::fixed << static_cast<float>(years);

            Wt::WString ptage = agestream.str();
            log("info") << "PT AGE: " << ptage.toUTF8();


            new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
                table->elementAt(row, 0)),
                new Wt::WText((*i)->ptFirstName_, table->elementAt(row, 1)),
                new Wt::WText((*i)->ptLastName_, table->elementAt(row, 2)),
                new Wt::WText((*i)->ptDob_, table->elementAt(row, 3)),
                new Wt::WText(ptage, table->elementAt(row, 4)),
                new Wt::WText((*i)->studyTime_, table->elementAt(row, 5)),
                new Wt::WText((*i)->appointmentStatus_, table->elementAt(row, 6)),
                btnDownloadDny = new Wt::WPushButton("View Study", table->elementAt(row, 7));
                //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
                btnDownloadDny->clicked().connect(std::bind([=] () {
                    showStudy_.emit(dnysid); // emit the study dialog signal
                }));
        }

        strans.commit();

        table->addStyleClass("table form-inline");
        table->addStyleClass("table table-striped");
        table->addStyleClass("table table-hover");


        Wt::WContainerWidget *result = new Wt::WContainerWidget();

        result->addWidget(table);
        addWidget(new Wt::WText("<h3>Sleep Studies Tonight for Downey</h3>"));
        addWidget(result);
    } else if (loc == "Long Beach") {
        Wt::WContainerWidget *result2 = new Wt::WContainerWidget();

        dbo::Transaction cstrans(dbsession);
        SleepStudies compstudies = dbsession.find<SleepStudy>().where("std_date = ?").bind(queryToday).where("std_loco = ?").bind("Long Beach");
        std::cerr << compstudies.size() << std::endl;

        Wt::WTable *table3 = new Wt::WTable();
        table3->setHeaderCount(1);

        table3->elementAt(0, 0)->addWidget(new Wt::WText("#"));
        table3->elementAt(0, 1)->addWidget(new Wt::WText("Patient First Name"));
        table3->elementAt(0, 2)->addWidget(new Wt::WText("Patient Last Name"));
        table3->elementAt(0, 3)->addWidget(new Wt::WText("Patient Date of Birth"));
        table3->elementAt(0, 4)->addWidget(new Wt::WText("Patient Age"));
        table3->elementAt(0, 5)->addWidget(new Wt::WText("Appointment Time"));
        table3->elementAt(0, 6)->addWidget(new Wt::WText("Appointment Status"));
        table3->elementAt(0, 7)->addWidget(new Wt::WText("More Info"));


        int crow = 1;
        for (SleepStudies::const_iterator s = compstudies.begin(); s != compstudies.end(); ++s, ++crow)
        {


            WString pdob = (*s)->ptDob_;
            Wt::WString lbsid = (*s)->studyId_;

            Wt::WDate ptdobin = Wt::WDate::fromString(pdob, "MM/dd/yyyy");
            Wt::WDate today = Wt::WDate::currentServerDate();

            int dobdiff = ptdobin.daysTo(today);
            float years = dobdiff / 365;
            log("info") << "DOB: " << ptdobin.toString("MM/dd/yyyy");
            log("info") << "TODAY: " << today.toString("MM/dd/yyyy");
            log("info") << "DOB DIFF: " << dobdiff;
            log("info") << "PT AGE: " << years;

            std::stringstream agestream;
            agestream << std::setprecision(0) << std::fixed << static_cast<float>(years);

            Wt::WString ptage = agestream.str();
            log("info") << "PT AGE: " << ptage.toUTF8();


            new Wt::WText(Wt::WString::fromUTF8("{1}").arg(crow),
            table3->elementAt(crow, 0)),
            new Wt::WText((*s)->ptFirstName_, table3->elementAt(crow, 1)),
            new Wt::WText((*s)->ptLastName_, table3->elementAt(crow, 2)),
            new Wt::WText((*s)->ptDob_, table3->elementAt(crow, 3)),
            new Wt::WText(ptage, table3->elementAt(crow, 4)),
            new Wt::WText((*s)->studyTime_, table3->elementAt(crow, 5)),
            new Wt::WText((*s)->appointmentStatus_, table3->elementAt(crow, 6)),
            btnDownloadRef = new Wt::WPushButton("View Study", table3->elementAt(crow, 7));
            btnDownloadRef->clicked().connect(std::bind([=] () {
                showStudy_.emit(lbsid); // emit the study dialog signal
            }));

        }
        cstrans.commit();


        table3->addStyleClass("table form-inline");
        table3->addStyleClass("table table-striped");
        table3->addStyleClass("table table-hover");
        result2->addWidget(table3);

        addWidget(new Wt::WText("<h3>Sleep Studies Tonight for Long Beach</h3>"));
        addWidget(result2);
    }
}
