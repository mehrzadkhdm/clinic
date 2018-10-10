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
#include <Wt/WDateEdit>
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
#include "Composer.h"

#include "ReferralCountEntry.h"
#include "Referral.h"
#include "Practice.h"
#include "UserSession.h"
#include "MarketingPracticeCountWidget.h"

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

MarketingPracticeCountWidget::MarketingPracticeCountWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
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

//attept to build a custom vector
void MarketingPracticeCountWidget::showReferralCount()
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    Wt::log("notice") << "referrals is being queried for with start and end date";

    //define logentries vector here
    std::vector<ReferralCountEntry> entries;

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>();
    std::cerr << practices.size() << std::endl;

    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i)
    {
        ReferralCountEntry *ent = new ReferralCountEntry();

        Wt::WString refLastWeek;
        Wt::WString refLastMonth;
        Wt::WString refLastYear;
        Wt::WString mdName;

        Wt::WDate recvDate;
        Wt::WDate lastRefDate;
        Wt::WString lastReceivedDate;



        Wt::WDate servernow = Wt::WDate::currentServerDate();
        Wt::WDate weekFromNow;
        Wt::WDate monthFromNow;
        Wt::WDate yearFromNow;

        weekFromNow = servernow.addDays(-7);
        Wt::log("info") << weekFromNow.toString("MM/dd/yyyy");
        int weekCount = 0;

        monthFromNow = servernow.addMonths(-1);
        Wt::log("info") << monthFromNow.toString("MM/dd/yyyy");
        int monthCount = 0;

        yearFromNow = servernow.addYears(-1);
        Wt::log("info") << yearFromNow.toString("MM/dd/yyyy");
        int yearCount = 0;

        typedef dbo::collection< dbo::ptr<Referral> > PracReferrals;
        PracReferrals pracreferrals = dbsession.find<Referral>().where("prac_id= ?").bind((*i)->practiceId_);
        std::cerr << pracreferrals.size() << std::endl;

        for (PracReferrals::const_iterator p = pracreferrals.begin(); p != pracreferrals.end(); ++p)
        {

            recvDate = (*p)->refReceivedAt_;
            Wt::log("info") << recvDate.toString("MM/dd/yyyy");
            //instance new log entry object here

            if(weekFromNow <= recvDate)
            {
                weekCount++;
            }

            if(monthFromNow <= recvDate)
            {
                monthCount++;
            }

            if(yearFromNow <= recvDate);
            {
                yearCount++;
            }

            if( lastRefDate.isNull() || recvDate > lastRefDate )
            {
                lastRefDate = recvDate;
            }

        }

        std::cout << "Week Count:" << weekCount;
        std::stringstream weekstream;
        weekstream << static_cast<int>(weekCount);
        refLastWeek = weekstream.str();

        std::cout << "Month Count:" << monthCount;
        std::stringstream monthstream;
        monthstream << static_cast<int>(monthCount);
        refLastMonth = monthstream.str();

        std::cout << "Year Count:" << yearCount;
        std::stringstream yearstream;
        yearstream << static_cast<int>(yearCount);
        refLastYear = yearstream.str();

        mdName = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;

        ent->numLastWeek = refLastWeek;
        ent->numLastMonth = refLastMonth;
        ent->numLastYear = refLastYear;
        ent->lastReceivedDate = lastRefDate;
        ent->physicianName = mdName;
        ent->dateAdded = (*i)->dateAdded_;
        Wt::log("info") << "numLastWeek: " << ent->numLastWeek.toUTF8();
        Wt::log("info") << "numLastMonth: " << ent->numLastMonth.toUTF8();
        Wt::log("info") << "numLastYear: " << ent->numLastYear.toUTF8();

        entries.push_back(*ent);
    }

    Wt::WTable *table = new Wt::WTable();

    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Physician Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Referrals Last Week"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Referrals Last Month"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Referrals Last Year"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Last Referral Received"));

    int row = 1;
    for (std::vector<ReferralCountEntry>::iterator en = entries.begin(); en != entries.end(); ++en, ++row)
    {
            if (en->dateAdded.toString() != "" && en->dateAdded.daysTo(Wt::WDateTime::currentDateTime()) <= 90)
            {
                table->rowAt(row)->setStyleClass("row-new-provider");
            } else if (en->dateAdded.toString() != "" && en->dateAdded.daysTo(Wt::WDateTime::currentDateTime()) > 90 && en->dateAdded.daysTo(Wt::WDateTime::currentDateTime()) <= 365) {
                table->rowAt(row)->setStyleClass("row-newyr-provider");
            }
            //log output of each en->count to console
            Wt::log("info") << "table en->numLastWeek: " << en->numLastWeek.toUTF8();
            Wt::log("info") << "table en->numLastMonth: " << en->numLastMonth.toUTF8();
            Wt::log("info") << "table en->numLastYear: " << en->numLastYear.toUTF8();

            new Wt::WText(en->physicianName, table->elementAt(row, 0)),
            new Wt::WText(en->numLastWeek, table->elementAt(row, 1)),
            new Wt::WText(en->numLastMonth, table->elementAt(row, 2)),
            new Wt::WText(en->numLastYear, table->elementAt(row, 3)),
            new Wt::WText(en->lastReceivedDate.toString("MM/dd/yyyy"), table->elementAt(row, 4));
    }

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    transaction.commit();

    container->addWidget(table);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    addWidget(container);

}
