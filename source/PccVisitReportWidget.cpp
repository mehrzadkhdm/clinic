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
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTableView>
#include <Wt/Chart/WPieChart>
#include <Wt/WPaintedWidget>
#include <Wt/WItemDelegate>
#include <Wt/WShadow>
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
#include <Wt/WAbstractItemModel>
#include <Wt/WAbstractItemView>
#include <Wt/WEnvironment>

#include "PccVisitReportWidget.h"
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
#include "Insurance.h"
#include "ReferralLogEntry.h"
#include "ReferralCountItem.h"
#include "ReferralInsuranceEntry.h"
#include "VisitCountItem.h"
#include "PhysicianVisit.h"

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

PccVisitReportWidget::PccVisitReportWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<PhysicianVisit>("physicianvisit");

    dbsession.mapClass<Patient>("patient");
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

//attept to build a custom vector
void PccVisitReportWidget::showVisitReport(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& pcc)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    //define logentries vector here
    std::vector<VisitCountItem> counts;
    Wt::log("info") << "PCC EMAIL: " << pcc.toUTF8();

    typedef dbo::collection< dbo::ptr<PhysicianVisit> > Visits;
    Visits visits = dbsession.find<PhysicianVisit>().where("pcc_name = ?").bind(pcc.toUTF8()).where("visit_start BETWEEN ? AND ?").bind(startDate).bind(endDate).where("visit_end IS NOT NULL");
    std::cerr << visits.size() << std::endl;
    
    //need to display number of visits in timeframe
    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("MD Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("PCC"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Visit Start"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Visit End"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Duration"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Status"));
    int row = 1;
    for (Visits::const_iterator i = visits.begin(); i != visits.end(); ++i, ++row)
    {
        //instance new log entry object here

        VisitCountItem *ent = new VisitCountItem();

        int duration = (*i)->visitStartTime_.secsTo((*i)->visitEndTime_)/60;
        ent->dur = duration;
        //push_back to counts vector
        counts.push_back(*ent);


        std::stringstream durstream;
        durstream << static_cast<int>(duration);
        Wt::WString visitDuration = Wt::WString(durstream.str() + " Min");

        new Wt::WText((*i)->practiceName_, table->elementAt(row, 0)),
        new Wt::WText((*i)->pccName_, table->elementAt(row, 1)),
        new Wt::WText((*i)->visitStartTime_.toLocalTime(Wt::WLocale::currentLocale()).toString("MM/dd/yyyy h:m a"), table->elementAt(row, 2)),
        new Wt::WText((*i)->visitEndTime_.toLocalTime(Wt::WLocale::currentLocale()).toString("MM/dd/yyyy h:m a"), table->elementAt(row, 3)),
        new Wt::WText(visitDuration, table->elementAt(row, 4)),
        new Wt::WText((*i)->visitIssues_, table->elementAt(row, 5));      
        //click 'PDF' button on any table row, connect downloadReferral with that referral uuid


    }
    
    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    int totalentries = visits.size();
    std::cout << "Total Visits:" << totalentries;

    std::stringstream totalstream;
    totalstream << static_cast<int>(totalentries);
    Wt::WString showtotalout = totalstream.str();

    Wt::WText *finaltotal = new Wt::WText("<h4>Number of Visits Between "+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showtotalout + "</h4>");
    finaltotal->setStyleClass("final-total");

    transaction.commit();

    //calculate average time spent at each visit:
    int totaldur = 0;
    for (std::vector<VisitCountItem>::iterator c = counts.begin(); c != counts.end(); ++c)
    {
        totaldur = totaldur + c->dur;
    }

    int averagedur;
    if (totalentries != 0)
    {
        averagedur = totaldur / totalentries;        
    }
    std::cout << "Average Time Spent:" << averagedur;

    std::stringstream averagestream;
    averagestream << static_cast<int>(averagedur);
    Wt::WString showaverageout = averagestream.str();

    Wt::WText *finalaverage = new Wt::WText("<h4>Average Duration of Each Visit Between"+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showaverageout + " Minutes" +"</h4>");
    finalaverage->setStyleClass("final-total");

    container->addWidget(finaltotal);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(finalaverage);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(table);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    addWidget(container);

}
