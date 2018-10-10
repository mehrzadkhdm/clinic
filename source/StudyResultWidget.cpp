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

#include "StaffSleepStudyWidget.h"
#include "StudyResultWidget.h"
#include "Patient.h"
#include "Referral.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"
#include "Backup.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

//struct for patient info
namespace InboundRefApp{
    struct InboundPatient {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptCity;

        InboundPatient(
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

StudyResultWidget::StudyResultWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Backup>("backup");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Referral>("referral");
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

void StudyResultWidget::showReferralRangeByInsurance(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& insuranceId)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    Wt::log("notice") << "hours is being queried for student hours by id";

    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("insurance_id").bind(insuranceId).where("ref_received BETWEEN ? AND ?").bind(startDate).bind(endDate);
    std::cerr << referrals.size() << std::endl;
    int reftotal;
    reftotal = referrals.size();

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Referred From"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Referral Status"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Study Date"));


    int row = 1;
    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i, ++row)
    {

            new Wt::WText((*i)->refFirstName_, table->elementAt(row, 0)),
            new Wt::WText((*i)->refLastName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->refDob_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 3)),
            new Wt::WText((*i)->refStatus_, table->elementAt(row, 4)),
            new Wt::WText((*i)->refStudyDate_, table->elementAt(row, 5));

    }


    transaction.commit();


    Wt::WText *finaltotal = new Wt::WText("<h4>Total Number of Referrals: " + reftotal + "</h4>");
    finaltotal->setStyleClass("final-total");

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    totaltrans.commit();

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->setStyleClass("print-result");

    std::ostringstream r;
    result->htmlText(r);
    Wt::WString s = r.str();

    Wt::WResource *pdf = new ReportResource(s, this);

    Wt::WPushButton *button = new Wt::WPushButton("Download Report PDF");
    button->setLink(pdf);

    container->addWidget(result);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(button);
    addWidget(container);

}
