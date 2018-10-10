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
#include "PhysicianRecordListContainerWidget.h"
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
#include "Address.h"
#include "Locality.h"
#include "LocalityItem.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

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

PhysicianRecordListContainerWidget::PhysicianRecordListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<Locality>("locality");


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

void PhysicianRecordListContainerWidget::showSearch(Wt::WString& curstr, Wt::WString curprac)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("prac_name ILIKE ? OR prac_drlast ILIKE ? OR prac_drfirst ILIKE ?").bind(searchPt).bind(searchPt).bind(searchPt);
    std::cerr << practices.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("MD First"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("MD Last"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Merge"));


    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {

            if ((*i)->practiceId_ != curprac) {
                new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 0)),
                new Wt::WText((*i)->refPracticeDrFirst_, table->elementAt(row, 1)),
                new Wt::WText((*i)->refPracticeDrLast_, table->elementAt(row, 2)),
                btnSelectPhysician = new Wt::WPushButton("Merge Records", table->elementAt(row, 3)),
                //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
                btnSelectPhysician->clicked().connect(boost::bind(&PhysicianRecordListContainerWidget::selectNewRecord, this, (*i)->practiceId_));


            }
    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    WText *nomatches = new WText("No physician records found.");
    nomatches->setStyleClass("no-addresses-found");

    if (practices.size() < 1) {
        container->addWidget(nomatches);
        container->addWidget(new WBreak());

    } else {
        container->addWidget(table);
    }

    addWidget(container);
}

void PhysicianRecordListContainerWidget::showSearchOnly(Wt::WString& curstr)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("prac_name ILIKE ? OR prac_drlast ILIKE ? OR prac_drfirst ILIKE ?").bind(searchPt).bind(searchPt).bind(searchPt);
    std::cerr << practices.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("MD First"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("MD Last"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Select"));


    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {


        new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 0)),
        new Wt::WText((*i)->refPracticeDrFirst_, table->elementAt(row, 1)),
        new Wt::WText((*i)->refPracticeDrLast_, table->elementAt(row, 2)),
        btnSelectPhysician = new Wt::WPushButton("Select Practice", table->elementAt(row, 3)),
        //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
        btnSelectPhysician->clicked().connect(boost::bind(&PhysicianRecordListContainerWidget::selectNewRecord, this, (*i)->practiceId_));


    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    WText *nomatches = new WText("No physician records found.");
    nomatches->setStyleClass("no-addresses-found");

    if (practices.size() < 1) {
        container->addWidget(nomatches);
        container->addWidget(new WBreak());

    } else {
        container->addWidget(table);
    }

    addWidget(container);
}
void PhysicianRecordListContainerWidget::selectNewRecord(Wt::WString aid) {
    Wt::log("info") << "PhysicianRecordListContainerWidget selectNewAddress before emit";
    selectPhysicianRecord_.emit(aid);
    Wt::log("info") << "PhysicianRecordListContainerWidget selectNewAddress after emit";
}