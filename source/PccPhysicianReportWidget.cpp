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

#include "PccPhysicianReportWidget.h"
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

PccPhysicianReportWidget::PccPhysicianReportWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
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
void PccPhysicianReportWidget::showPhysicianReport(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& pcc)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    //define logentries vector here
    Wt::log("info") << "PCC EMAIL: " << pcc.toUTF8();

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("pcc_email = ?").bind(pcc.toUTF8()).where("date_added BETWEEN ? AND ?").bind(startDate).bind(endDate);
    std::cerr << practices.size() << std::endl;
    
    //need to display number of practices in timeframe
    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("MD First Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("MD Last Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Phone Number"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Email Address"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Date Added"));


    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {
            if ((*i)->dateAdded_.toString() != "" && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 90)
            {
                table->rowAt(row)->setStyleClass("row-new-provider");
            } else if ((*i)->dateAdded_.toString() != "" && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) > 90 && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 365) {
                table->rowAt(row)->setStyleClass("row-newyr-provider");
            }
            new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->refPracticeDrFirst_, table->elementAt(row, 1)),
            new Wt::WText((*i)->refPracticeDrLast_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refPracticeNpi_, table->elementAt(row, 3)),
            new Wt::WText((*i)->refPracticeEmail_, table->elementAt(row, 4)),
            new Wt::WText((*i)->dateAdded_.date().toString("MM/dd/yyyy"), table->elementAt(row, 5));
            
    }

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    int totalentries = practices.size();
    std::cout << "Total Practices:" << totalentries;

    std::stringstream totalstream;
    totalstream << static_cast<int>(totalentries);
    Wt::WString showtotalout = totalstream.str();

    Wt::WText *finaltotal = new Wt::WText("<h4>Number of Physicians Added Between "+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showtotalout + "</h4>");
    finaltotal->setStyleClass("final-total");

    transaction.commit();

    container->addWidget(finaltotal);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(table);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    addWidget(container);

}
