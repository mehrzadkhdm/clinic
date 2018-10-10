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
#include <Wt/WProgressBar>
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
#include "Composer.h"

#include "ReferralCountEntry.h"
#include "Referral.h"
#include "Practice.h"
#include "UserSession.h"
#include "InsuranceGroup.h"
#include "Insurance.h"
#include "AdminPracticeCountWidget.h"

#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

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

AdminPracticeCountWidget::AdminPracticeCountWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<InsuranceGroup>("insurance_group");
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
void AdminPracticeCountWidget::showReferralCount()
{
    container = new Wt::WContainerWidget();

    initcontainer = new Wt::WContainerWidget();
    initcontainer->setStyleClass("inline-buttons");
    
    addWidget(initcontainer);
    addWidget(container);
    
    container->hide();

    startButton = new Wt::WPushButton("Run Report", initcontainer);
    startButton->clicked().connect(boost::bind(&AdminPracticeCountWidget::disableStart, this));

}

void AdminPracticeCountWidget::disableStart() {

    startButton->hide();
    preRun();
}
void  AdminPracticeCountWidget::preRun() {
    runIt();    
}
void AdminPracticeCountWidget::runIt() 
{

    dbo::Transaction transaction(dbsession);

    Wt::log("notice") << "referrals is being queried for with start and end date";
    Wt::WTable *table = new Wt::WTable();

    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Physician Name"));

    int row = 1;

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>();
    std::cerr << practices.size() << std::endl;
    
    //Total
    int staffThisMonthPPO = 0;
    int staffLastMonthPPO = 0;
    int staffTwoMonthsPPO = 0;
    int staffMonthFourPPO = 0;
    int staffMonthFivePPO = 0;
    int staffMonthSixPPO = 0;
    int staffYearToDatePPO = 0;
    
    int staffThisMonthHMO = 0;
    int staffLastMonthHMO = 0;
    int staffTwoMonthsHMO = 0;
    int staffMonthFourHMO = 0;
    int staffMonthFiveHMO = 0;
    int staffMonthSixHMO = 0;
    int staffYearToDateHMO = 0;
    
    int staffThisMonthIPA = 0;
    int staffLastMonthIPA = 0;
    int staffTwoMonthsIPA = 0;
    int staffMonthFourIPA = 0;
    int staffMonthFiveIPA = 0;
    int staffMonthSixIPA = 0;
    int staffYearToDateIPA = 0;
    
    int staffThisMonthOther = 0;
    int staffLastMonthOther = 0;
    int staffTwoMonthsOther = 0;
    int staffMonthFourOther = 0;
    int staffMonthFiveOther = 0;
    int staffMonthSixOther = 0;
    int staffYearToDateOther = 0;

    //Scheduled
    int scheduledThisMonthPPO = 0;
    int scheduledLastMonthPPO = 0;
    int scheduledTwoMonthsPPO = 0;
    int scheduledMonthFourPPO = 0;
    int scheduledMonthFivePPO = 0;
    int scheduledMonthSixPPO = 0;
    int scheduledYearToDatePPO = 0;
    
    int scheduledThisMonthHMO = 0;
    int scheduledLastMonthHMO = 0;
    int scheduledTwoMonthsHMO = 0;
    int scheduledMonthFourHMO = 0;
    int scheduledMonthFiveHMO = 0;
    int scheduledMonthSixHMO = 0;
    int scheduledYearToDateHMO = 0;
    
    int scheduledThisMonthIPA = 0;
    int scheduledLastMonthIPA = 0;
    int scheduledTwoMonthsIPA = 0;
    int scheduledMonthFourIPA = 0;
    int scheduledMonthFiveIPA = 0;
    int scheduledMonthSixIPA = 0;
    int scheduledYearToDateIPA = 0;
    
    int scheduledThisMonthOther = 0;
    int scheduledLastMonthOther = 0;
    int scheduledTwoMonthsOther = 0;
    int scheduledMonthFourOther = 0;
    int scheduledMonthFiveOther = 0;
    int scheduledMonthSixOther = 0;
    int scheduledYearToDateOther = 0;

    //Unscheduled
    int unscheduledThisMonthPPO = 0;
    int unscheduledLastMonthPPO = 0;
    int unscheduledTwoMonthsPPO = 0;
    int unscheduledMonthFourPPO = 0;
    int unscheduledMonthFivePPO = 0;
    int unscheduledMonthSixPPO = 0;
    int unscheduledYearToDatePPO = 0;
    
    int unscheduledThisMonthHMO = 0;
    int unscheduledLastMonthHMO = 0;
    int unscheduledTwoMonthsHMO = 0;
    int unscheduledMonthFourHMO = 0;
    int unscheduledMonthFiveHMO = 0;
    int unscheduledMonthSixHMO = 0;
    int unscheduledYearToDateHMO = 0;
    
    int unscheduledThisMonthIPA = 0;
    int unscheduledLastMonthIPA = 0;
    int unscheduledTwoMonthsIPA = 0;
    int unscheduledMonthFourIPA = 0;
    int unscheduledMonthFiveIPA = 0;
    int unscheduledMonthSixIPA = 0;
    int unscheduledYearToDateIPA = 0;
    
    int unscheduledThisMonthOther = 0;
    int unscheduledLastMonthOther = 0;
    int unscheduledTwoMonthsOther = 0;
    int unscheduledMonthFourOther = 0;
    int unscheduledMonthFiveOther = 0;
    int unscheduledMonthSixOther = 0;
    int unscheduledYearToDateOther = 0;


    //kaiser
    int kaiserThisMonth = 0;
    int kaiserScheduledThisMonth = 0;
    int kaiserUnscheduledThisMonth = 0;
    int kaiserLastMonth = 0;
    int kaiserScheduledLastMonth = 0;
    int kaiserUnscheduledLastMonth = 0;
    int kaiserTwoMonths = 0;
    int kaiserScheduledTwoMonths = 0;
    int kaiserUnscheduledTwoMonths = 0;    
    int kaiserMonthFour = 0;
    int kaiserScheduledMonthFour = 0;
    int kaiserUnscheduledMonthFour = 0;
    int kaiserMonthFive = 0;
    int kaiserScheduledMonthFive = 0;
    int kaiserUnscheduledMonthFive = 0;
    int kaiserMonthSix = 0;
    int kaiserScheduledMonthSix = 0;
    int kaiserUnscheduledMonthSix = 0;
    int kaiserYearToDate = 0;
    int kaiserScheduledYear = 0;
    int kaiserUnscheduledYear = 0;    
    
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {
        typedef dbo::collection< dbo::ptr<Referral> > NumReferrals;
        NumReferrals nreferrals = dbsession.find<Referral>().where("prac_id= ?").bind((*i)->practiceId_);
        std::cerr << nreferrals.size() << std::endl;
        if (nreferrals.size() != 0) {
            Wt::WString physicianName;
            Wt::WString practiceName;
            Wt::WString pracSpec;
            
            physicianName = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;
            practiceName = (*i)->refPracticeName_;
            pracSpec = (*i)->refSpecialty_;

            Wt::WText *docname = new Wt::WText(physicianName);
            Wt::WText *pracname = new Wt::WText(practiceName);
            Wt::WText *pracspec = new Wt::WText(pracSpec);

            docname->setStyleClass("ref-report-docname");
            pracname->setStyleClass("ref-report-pracname");
            pracspec->setStyleClass("ref-report-pracspec");

            container->addWidget(docname);
            container->addWidget(new Wt::WBreak());
            if (practiceName != "") {
                container->addWidget(pracname);
                container->addWidget(new Wt::WBreak());
            }
            if (pracSpec != "") {
                container->addWidget(pracspec);
                container->addWidget(new Wt::WBreak());            
            }

            Wt::WText *kaiserdoc = new Wt::WText("Kaiser Permanente");
            kaiserdoc->setStyleClass("ref-report-kaiserdoc");
            container->addWidget(kaiserdoc);
            kaiserdoc->hide();
            
            container->addWidget(new Wt::WBreak());
            if ((*i)->dateAdded_.isNull()) {
                Wt::WText *frontdesk = new Wt::WText("Added By: Front Desk Staff");
                frontdesk->setStyleClass("ref-report-frontdesk");
                container->addWidget(frontdesk);
            } else {
                Wt::WText *dateadd = new Wt::WText();
                dateadd->setStyleClass("ref-report-dateadd");
                dateadd->setText("Added On: "+(*i)->dateAdded_.toString("MM/dd/yyyy"));

                container->addWidget(new Wt::WBreak());
                
                Wt::WText *whoadded = new Wt::WText();
                whoadded->setStyleClass("ref-report-whoadded");
                whoadded->setText("Added By: Staff");

                container->addWidget(dateadd);
                container->addWidget(new Wt::WBreak());
                container->addWidget(whoadded);
                container->addWidget(new Wt::WBreak());
            }

            

            Wt::WString thisMonthTotal;
            Wt::WString scheduledThisMonthTotal;
            Wt::WString unscheduledThisMonthTotal;

            Wt::WString lastMonthTotal;
            Wt::WString scheduledLastMonthTotal;
            Wt::WString unscheduledLastMonthTotal;

            Wt::WString twoMonthTotal;
            Wt::WString scheduledTwoMonthsTotal;
            Wt::WString unscheduledTwoMonthsTotal;

            Wt::WString monthFourTotal;
            Wt::WString scheduledFourTotal;
            Wt::WString unscheduledFourTotal;

            Wt::WString monthFiveTotal;
            Wt::WString scheduledFiveTotal;
            Wt::WString unscheduledFiveTotal;

            Wt::WString monthSixTotal;
            Wt::WString scheduledSixTotal;
            Wt::WString unscheduledSixTotal;

            Wt::WString thisYearTotal;
            Wt::WString scheduledYearTotal;
            Wt::WString unscheduledYearTotal;

            Wt::WString groupName;

            Wt::WDate recvDate;
            Wt::WDate lastRefDate;
            Wt::WString lastReceivedDate;

            Wt::WDate servernow = Wt::WDate::currentServerDate();
            Wt::WDate thisMonth;
            Wt::WDate lastMonth;
            Wt::WDate twoMonths;
            Wt::WDate monthFour;
            Wt::WDate monthFive;
            Wt::WDate monthSix;
            Wt::WDate yearStart;
            
            Wt::WString month1;
            Wt::WString month2;
            Wt::WString month3;
            Wt::WString month4;
            Wt::WString month5;
            Wt::WString month6;

            thisMonth.setDate(servernow.year(), servernow.month(), 1);
            Wt::log("info") << thisMonth.toString("MM/dd/yyyy");

            lastMonth = thisMonth.addMonths(-1);
            Wt::log("info") << lastMonth.toString("MM/dd/yyyy");

            twoMonths = lastMonth.addMonths(-1);
            Wt::log("info") << twoMonths.toString("MM/dd/yyyy");

            monthFour = twoMonths.addMonths(-1);
            monthFive = monthFour.addMonths(-1);
            monthSix = monthFive.addMonths(-1);

            yearStart.setDate(servernow.year(), 1, 1);
            Wt::log("info") << yearStart.toString("MM/dd/yyyy");
            month1 = Wt::WDate::shortMonthName(thisMonth.month());
            month2 = Wt::WDate::shortMonthName(lastMonth.month());
            month3 = Wt::WDate::shortMonthName(twoMonths.month());
            month4 = Wt::WDate::shortMonthName(monthFour.month());
            month5 = Wt::WDate::shortMonthName(monthFive.month());
            month6 = Wt::WDate::shortMonthName(monthSix.month());

            std::vector<ReferralCountEntry> entries;
            
            typedef dbo::collection< dbo::ptr<InsuranceGroup> > Groups;
            Groups groups = dbsession.find<InsuranceGroup>();
            std::cerr << groups.size() << std::endl;
            
            for (Groups::const_iterator g = groups.begin(); g != groups.end(); ++g) {
                ReferralCountEntry *ent = new ReferralCountEntry();   
                
                int refThisMonth = 0;
                int refScheduledThisMonth = 0;
                int refUnscheduledThisMonth = 0;
                int refLastMonth = 0;
                int refScheduledLastMonth = 0;
                int refUnscheduledLastMonth = 0;
                int refTwoMonths = 0;
                int refScheduledTwoMonths = 0;
                int refUnscheduledTwoMonths = 0;
                int refFour = 0;
                int refScheduledFour = 0;
                int refUnscheduledFour = 0;
                int refFive = 0;
                int refScheduledFive = 0;
                int refUnscheduledFive = 0;
                int refSix = 0;
                int refScheduledSix = 0;
                int refUnscheduledSix = 0;

                int refYearToDate = 0;
                int refScheduledYear = 0;
                int refUnscheduledThisYear = 0;            
                
                typedef dbo::collection< dbo::ptr<Insurance> > Insurances;
                Insurances insurances = dbsession.find<Insurance>().where("ins_group_id = ?").bind((*g)->groupId_);
                std::cerr << insurances.size() << std::endl;
                
                for (Insurances::const_iterator in = insurances.begin(); in != insurances.end(); ++in) {
                    
                    typedef dbo::collection< dbo::ptr<Referral> > PracReferrals;
                    PracReferrals pracreferrals = dbsession.find<Referral>().where("prac_id= ?").bind((*i)->practiceId_).where("insurance_id = ?").bind((*in)->insuranceId_);
                    std::cerr << pracreferrals.size() << std::endl;
                    
                    if ((*i)->kaiser_ == "yes") {
                        kaiserdoc->show();
                    }

                    if (pracreferrals.size() != 0) {
                        for (PracReferrals::const_iterator p = pracreferrals.begin(); p != pracreferrals.end(); ++p) {
                            recvDate = (*p)->refReceivedAt_;
                            Wt::log("info") << recvDate.toString("MM/dd/yyyy");
                            //instance new log entry object here

                            if(thisMonth <= recvDate)
                            {
                                refThisMonth++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserThisMonth++;
                                }
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledThisMonth++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledThisMonth++;
                                    }
                                } else {
                                    refUnscheduledThisMonth++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledThisMonth++;
                                    }
                                }

                                if((*g)->groupName_ == "PPO") {
                                    staffThisMonthPPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledThisMonthPPO++;
                                    } else {
                                        unscheduledThisMonthPPO++;
                                    }
                                } else if((*g)->groupName_ == "IPA") {
                                    staffThisMonthIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledThisMonthIPA++;
                                    } else {
                                        unscheduledThisMonthIPA++;
                                    }
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffThisMonthHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledThisMonthHMO++;
                                    } else {
                                        unscheduledThisMonthHMO++;
                                    }
                                } else {
                                    staffThisMonthOther++;
                                    if((*p)->studyDate_ == "") { 
                                        scheduledThisMonthOther++;
                                    } else {
                                        unscheduledThisMonthOther++;
                                    }
                                }

                            } else if (lastMonth <= recvDate && thisMonth > recvDate) {
                                refLastMonth++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserLastMonth++;                             
                                }                                
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledLastMonth++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledLastMonth++;
                                    }                                    
                                } else {
                                    refUnscheduledLastMonth++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledLastMonth++;
                                    }                                    
                                }
                                if((*g)->groupName_ == "PPO") {
                                    staffLastMonthPPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledLastMonthPPO++;
                                    } else {
                                        unscheduledLastMonthPPO++;
                                    }
                                } else if((*g)->groupName_ == "IPA") {
                                    staffLastMonthIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledLastMonthIPA++;
                                    } else {
                                        unscheduledLastMonthIPA++;
                                    }                                
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffLastMonthHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledLastMonthHMO++;
                                    } else {
                                        unscheduledLastMonthHMO++;
                                    }
                                } else {
                                    staffLastMonthOther++;
                                }
                            } else if(twoMonths <= recvDate && lastMonth > recvDate) {
                                refTwoMonths++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserTwoMonths++;
                                }                                
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledTwoMonths++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledTwoMonths++;
                                    }
                                } else {
                                    refUnscheduledTwoMonths++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledTwoMonths++;
                                    }                                    
                                }
                                
                                if((*g)->groupName_ == "PPO") {
                                    staffTwoMonthsPPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledTwoMonthsPPO++;
                                    } else {
                                        unscheduledTwoMonthsPPO++;
                                    }
                                } else if((*g)->groupName_ == "IPA") {
                                    staffTwoMonthsIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledTwoMonthsIPA++;
                                    } else {
                                        unscheduledTwoMonthsIPA++;
                                    }
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffTwoMonthsHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledTwoMonthsHMO++;
                                    } else {
                                        unscheduledTwoMonthsHMO++;
                                    }
                                } else {
                                    staffTwoMonthsOther++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledTwoMonthsOther++;
                                    } else {
                                        unscheduledTwoMonthsOther++;
                                    }
                                }

                            } else if(monthFour <= recvDate && twoMonths > recvDate) {
                                refFour++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserMonthFour++;
                                }                                
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledFour++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledMonthFour++;
                                    }                                    
                                } else {
                                    refUnscheduledFour++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledMonthFour++;
                                    }                                    
                                }
                                
                                if((*g)->groupName_ == "PPO") {
                                    staffMonthFourPPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFourPPO++;
                                    } else {
                                        unscheduledMonthFourPPO++;
                                    }                                    
                                } else if((*g)->groupName_ == "IPA") {
                                    staffMonthFourIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFourIPA++;
                                    } else {
                                        unscheduledMonthFourIPA++;
                                    }                                    

                                } else if ((*g)->groupName_ == "HMO") {
                                    staffMonthFourHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFourHMO++;
                                    } else {
                                        unscheduledMonthFourHMO++;
                                    }                                    
                                } else {
                                    staffMonthFourOther++;

                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFourOther++;
                                    } else {
                                        unscheduledMonthFourOther++;
                                    }

                                }

                            } else if (monthFive <= recvDate && monthFour > recvDate) {
                                refFive++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserMonthFive++;
                                }                                                                
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledFive++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledMonthFive++;
                                    }
                                } else {
                                    refUnscheduledFive++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledMonthFive++;
                                    }                                    
                                }
                                
                                if((*g)->groupName_ == "PPO") {
                                    staffMonthFivePPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFivePPO++;
                                    } else {
                                        unscheduledMonthFivePPO++;
                                    }
                                } else if((*g)->groupName_ == "IPA") {
                                    staffMonthFiveIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFiveIPA++;
                                    } else {
                                        unscheduledMonthFiveIPA++;
                                    }
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffMonthFiveHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFiveHMO++;
                                    } else {
                                        unscheduledMonthFiveHMO++;
                                    }                                    
                                } else {
                                    staffMonthFiveOther++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthFiveOther++;
                                    } else {
                                        unscheduledMonthFiveOther++;
                                    }                                    
                                }                        

                            } else if (monthSix <= recvDate && monthFive > recvDate) {
                                refSix++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserMonthSix++;
                                }                                                                
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    refScheduledSix++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledMonthSix++;
                                    }                                    
                                } else {
                                    refUnscheduledSix++;
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledMonthSix++;
                                    }                                    
                                }

                                if((*g)->groupName_ == "PPO") {
                                    staffMonthSixPPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthSixPPO++;
                                    } else {
                                        unscheduledMonthSixPPO++;
                                    }                                    
                                } else if((*g)->groupName_ == "IPA") {
                                    staffMonthSixIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthSixIPA++;
                                    } else {
                                        unscheduledMonthSixIPA++;
                                    }                                    
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffMonthSixHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthSixHMO++;
                                    } else {
                                        unscheduledMonthSixHMO++;
                                    }                                    
                                } else {
                                    staffMonthSixOther++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledMonthSixOther++;
                                    } else {
                                        unscheduledMonthSixOther++;
                                    }                                    
                                }                        
                            }

                            if(yearStart <= recvDate) {
                                refYearToDate++;
                                if ((*i)->kaiser_ == "yes") {
                                    kaiserYearToDate++;
                                }              
                                if((*p)->refStatus_ == "Study Scheduled") { 
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserScheduledYear++;
                                    }
                                } else {
                                    if ((*i)->kaiser_ == "yes") {
                                        kaiserUnscheduledYear++;
                                    }
                                }
                                if((*g)->groupName_ == "PPO") {
                                    staffYearToDatePPO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledYearToDatePPO++;
                                    } else {
                                        unscheduledYearToDatePPO++;
                                    }                                    
                                } else if((*g)->groupName_ == "IPA") {
                                    staffYearToDateIPA++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledYearToDateIPA++;
                                    } else {
                                        unscheduledYearToDateIPA++;
                                    }
                                } else if ((*g)->groupName_ == "HMO") {
                                    staffYearToDateHMO++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledYearToDateHMO++;
                                    } else {
                                        unscheduledYearToDateHMO++;
                                    }                                    
                                } else {
                                    staffYearToDateOther++;
                                    if((*p)->refStatus_ == "Study Scheduled") { 
                                        scheduledYearToDateOther++;
                                    } else {
                                        unscheduledYearToDateOther++;                                    }                                    
                                }
                            }

                            if( lastRefDate.isNull() || recvDate > lastRefDate )
                            {
                                lastRefDate = recvDate;
                            }

                        }
                    }

                }
                std::cout << "ThisMonth Count:" << refThisMonth;
                std::stringstream thismonthstream;
                thismonthstream << static_cast<int>(refThisMonth);
                thisMonthTotal = thismonthstream.str();
                
                std::cout << "Scheduledthismonth Count:" << refScheduledThisMonth;
                std::stringstream scheduledthismonthstream;
                scheduledthismonthstream << static_cast<int>(refScheduledThisMonth);
                scheduledThisMonthTotal = scheduledthismonthstream.str();

                std::cout << "Unscheduledthismonth Count:" << refUnscheduledThisMonth;
                std::stringstream unscheduledthismonthstream;
                unscheduledthismonthstream << static_cast<int>(refUnscheduledThisMonth);
                unscheduledThisMonthTotal = unscheduledthismonthstream.str();

                
                std::cout << "LastMonth Count:" << refLastMonth;
                std::stringstream lastmonthstream;
                lastmonthstream << static_cast<int>(refLastMonth);
                lastMonthTotal = lastmonthstream.str();
                
                std::cout << "Scheduledlastmonth Count:" << refScheduledLastMonth;
                std::stringstream scheduledlastmonthstream;
                scheduledlastmonthstream << static_cast<int>(refScheduledLastMonth);
                scheduledLastMonthTotal = scheduledlastmonthstream.str();

                std::cout << "Unscheduledlastmonth Count:" << refUnscheduledLastMonth;
                std::stringstream unscheduledlastmonthstream;
                unscheduledlastmonthstream << static_cast<int>(refUnscheduledLastMonth);
                unscheduledLastMonthTotal = unscheduledlastmonthstream.str();
                

                std::cout << "TwoMonths Count:" << refTwoMonths;
                std::stringstream twomonthstream;
                twomonthstream << static_cast<int>(refTwoMonths);
                twoMonthTotal = twomonthstream.str();

                std::cout << "Scheduledtwomonths Count:" << refScheduledTwoMonths;
                std::stringstream scheduledtwomonthsstream;
                scheduledtwomonthsstream << static_cast<int>(refScheduledTwoMonths);
                scheduledTwoMonthsTotal = scheduledtwomonthsstream.str();

                std::cout << "Unscheduledtwomonths Count:" << refUnscheduledTwoMonths;
                std::stringstream unscheduledtwomonthsstream;
                unscheduledtwomonthsstream << static_cast<int>(refUnscheduledTwoMonths);
                unscheduledTwoMonthsTotal = unscheduledtwomonthsstream.str();

                std::cout << "Month4 Count:" << refFour;
                std::stringstream monthfourstream;
                monthfourstream << static_cast<int>(refFour);
                monthFourTotal = monthfourstream.str();

                std::cout << "Scheduledfour Count:" << refScheduledFour;
                std::stringstream scheduledfourstream;
                scheduledfourstream << static_cast<int>(refScheduledFour);
                scheduledFourTotal = scheduledfourstream.str();

                std::cout << "Unscheduledfour Count:" << refUnscheduledFour;
                std::stringstream unscheduledfourstream;
                unscheduledfourstream << static_cast<int>(refUnscheduledFour);
                unscheduledFourTotal = unscheduledfourstream.str();

                std::cout << "Month5 Count:" << refFive;
                std::stringstream monthfivestream;
                monthfivestream << static_cast<int>(refFive);
                monthFiveTotal = monthfivestream.str();
                
                std::cout << "Scheduledfive Count:" << refScheduledFive;
                std::stringstream scheduledfivestream;
                scheduledfivestream << static_cast<int>(refScheduledFive);
                scheduledFiveTotal = scheduledfivestream.str();

                std::cout << "Unscheduledfive Count:" << refUnscheduledFive;
                std::stringstream unscheduledfivestream;
                unscheduledfivestream << static_cast<int>(refUnscheduledFive);
                unscheduledFiveTotal = unscheduledfivestream.str();

                std::cout << "Month6 Count:" << refSix;
                std::stringstream monthsixstream;
                monthsixstream << static_cast<int>(refSix);
                monthSixTotal = monthsixstream.str();
                
                std::cout << "Scheduledsix Count:" << refScheduledSix;
                std::stringstream scheduledsixstream;
                scheduledsixstream << static_cast<int>(refScheduledSix);
                scheduledSixTotal = scheduledsixstream.str();

                std::cout << "Unscheduledsix Count:" << refUnscheduledSix;
                std::stringstream unscheduledsixstream;
                unscheduledsixstream << static_cast<int>(refUnscheduledSix);
                unscheduledSixTotal = unscheduledsixstream.str();

                std::cout << "YearToDate Count:" << refYearToDate;
                std::stringstream yearstream;
                yearstream << static_cast<int>(refYearToDate);
                thisYearTotal = yearstream.str();

                std::cout << "Scheduledyear Count:" << refScheduledYear;
                std::stringstream scheduledyearstream;
                scheduledyearstream << static_cast<int>(refScheduledYear);
                scheduledYearTotal = scheduledyearstream.str();

                std::cout << "Unscheduledyear Count:" << refUnscheduledThisYear;
                std::stringstream unscheduledyearstream;
                unscheduledyearstream << static_cast<int>(refUnscheduledThisYear);
                unscheduledYearTotal = unscheduledyearstream.str();

                ent->groupName = (*g)->groupName_;
                ent->numThisMonth = thisMonthTotal;
                ent->numScheduledThisMonth = scheduledThisMonthTotal;
                ent->numUnscheduledThisMonth = unscheduledThisMonthTotal;
                ent->numLastMonth = lastMonthTotal;
                ent->numScheduledLastMonth = scheduledLastMonthTotal;
                ent->numUnscheduledLastMonth = unscheduledLastMonthTotal;
                ent->numTwoMonths = twoMonthTotal;
                ent->numScheduledTwoMonths = scheduledTwoMonthsTotal;
                ent->numUnscheduledTwoMonths = unscheduledTwoMonthsTotal;                
                ent->numMonthFour = monthFourTotal;
                ent->numScheduledMonthFour = scheduledFourTotal;
                ent->numUnscheduledMonthFour = unscheduledFourTotal;                  
                ent->numMonthFive = monthFiveTotal;
                ent->numScheduledMonthFive = scheduledFiveTotal;
                ent->numUnscheduledMonthFive = unscheduledFiveTotal;                
                ent->numMonthSix = monthSixTotal;
                ent->numScheduledMonthSix = scheduledSixTotal;
                ent->numUnscheduledMonthSix = unscheduledSixTotal;
                ent->numScheduledYearToDate = scheduledYearTotal;
                ent->numUnscheduledYearToDate = unscheduledYearTotal;                
                ent->numYearToDate = thisYearTotal;
                ent->lastReceivedDate = lastRefDate;

                entries.push_back(*ent);

                refThisMonth = 0;
                refScheduledThisMonth = 0;
                refUnscheduledThisMonth = 0;
                refLastMonth = 0;
                refScheduledLastMonth = 0;
                refUnscheduledLastMonth = 0;
                refTwoMonths = 0;
                refScheduledTwoMonths = 0;
                refUnscheduledTwoMonths = 0;
                refFour = 0;
                refScheduledFour = 0;
                refUnscheduledFour = 0;
                refFive = 0;
                refScheduledFive = 0;
                refUnscheduledFive = 0;
                refSix = 0;
                refScheduledSix = 0;
                refUnscheduledSix = 0;
                refYearToDate = 0;
                refScheduledYear = 0;
                refUnscheduledThisYear = 0;

                lastRefDate.setDate(0000, 00, 00);
                

            }
     
            Wt::WTable *mdtable = new Wt::WTable();

            mdtable->setHeaderCount(1);
            mdtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
            mdtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
            mdtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
            mdtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
            mdtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
            mdtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
            mdtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
            mdtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));
            mdtable->elementAt(0, 8)->addWidget(new Wt::WText("Last Received"));

            int mdrow = 1;
            
            for (std::vector<ReferralCountEntry>::iterator en = entries.begin(); en != entries.end(); ++en, ++mdrow)
            {
                new Wt::WText(en->groupName, mdtable->elementAt(mdrow, 0)),
                new Wt::WText(en->numThisMonth, mdtable->elementAt(mdrow, 1)),
                new Wt::WText(en->numLastMonth, mdtable->elementAt(mdrow, 2)),
                new Wt::WText(en->numTwoMonths, mdtable->elementAt(mdrow, 3)),
                new Wt::WText(en->numMonthFour, mdtable->elementAt(mdrow, 4)),
                new Wt::WText(en->numMonthFive, mdtable->elementAt(mdrow, 5)),
                new Wt::WText(en->numMonthSix, mdtable->elementAt(mdrow, 6)),
                new Wt::WText(en->numYearToDate, mdtable->elementAt(mdrow, 7)),
                new Wt::WText(en->lastReceivedDate.toString("MM/dd/yyyy"), mdtable->elementAt(mdrow, 8));
            }
            mdtable->addStyleClass("table form-inline");
            mdtable->addStyleClass("table table-striped");
            mdtable->addStyleClass("table table-hover");
            container->addWidget(new Wt::WText("<h4>Total Referrals</h4>"));
            container->addWidget(mdtable);

            Wt::WTable *mdscheduledtable = new Wt::WTable();

            mdscheduledtable->setHeaderCount(1);
            mdscheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
            mdscheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
            mdscheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
            mdscheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
            mdscheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
            mdscheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
            mdscheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
            mdscheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));
            mdscheduledtable->elementAt(0, 8)->addWidget(new Wt::WText("Last Received"));

            int mdscheduledrow = 1;
            
            for (std::vector<ReferralCountEntry>::iterator ensch = entries.begin(); ensch != entries.end(); ++ensch, ++mdscheduledrow)
            {
                new Wt::WText(ensch->groupName, mdscheduledtable->elementAt(mdscheduledrow, 0)),
                new Wt::WText(ensch->numScheduledThisMonth, mdscheduledtable->elementAt(mdscheduledrow, 1)),
                new Wt::WText(ensch->numScheduledLastMonth, mdscheduledtable->elementAt(mdscheduledrow, 2)),
                new Wt::WText(ensch->numScheduledTwoMonths, mdscheduledtable->elementAt(mdscheduledrow, 3)),
                new Wt::WText(ensch->numScheduledMonthFour, mdscheduledtable->elementAt(mdscheduledrow, 4)),
                new Wt::WText(ensch->numScheduledMonthFive, mdscheduledtable->elementAt(mdscheduledrow, 5)),
                new Wt::WText(ensch->numScheduledMonthSix, mdscheduledtable->elementAt(mdscheduledrow, 6)),
                new Wt::WText(ensch->numScheduledYearToDate, mdscheduledtable->elementAt(mdscheduledrow, 7)),
                new Wt::WText(ensch->lastReceivedDate.toString("MM/dd/yyyy"), mdscheduledtable->elementAt(mdscheduledrow, 8));
            }
            mdscheduledtable->addStyleClass("table form-inline");
            mdscheduledtable->addStyleClass("table table-striped");
            mdscheduledtable->addStyleClass("table table-hover");
            container->addWidget(new Wt::WText("<h4>Scheduled Referrals</h4>"));
            container->addWidget(mdscheduledtable);


            Wt::WTable *mdunscheduledtable = new Wt::WTable();

            mdunscheduledtable->setHeaderCount(1);
            mdunscheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
            mdunscheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
            mdunscheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
            mdunscheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
            mdunscheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
            mdunscheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
            mdunscheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
            mdunscheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));
            mdunscheduledtable->elementAt(0, 8)->addWidget(new Wt::WText("Last Received"));

            int mdunscheduledrow = 1;
            
            for (std::vector<ReferralCountEntry>::iterator enun = entries.begin(); enun != entries.end(); ++enun, ++mdunscheduledrow)
            {
                new Wt::WText(enun->groupName, mdunscheduledtable->elementAt(mdunscheduledrow, 0)),
                new Wt::WText(enun->numUnscheduledThisMonth, mdunscheduledtable->elementAt(mdunscheduledrow, 1)),
                new Wt::WText(enun->numUnscheduledLastMonth, mdunscheduledtable->elementAt(mdunscheduledrow, 2)),
                new Wt::WText(enun->numUnscheduledTwoMonths, mdunscheduledtable->elementAt(mdunscheduledrow, 3)),
                new Wt::WText(enun->numUnscheduledMonthFour, mdunscheduledtable->elementAt(mdunscheduledrow, 4)),
                new Wt::WText(enun->numUnscheduledMonthFive, mdunscheduledtable->elementAt(mdunscheduledrow, 5)),
                new Wt::WText(enun->numUnscheduledMonthSix, mdunscheduledtable->elementAt(mdunscheduledrow, 6)),
                new Wt::WText(enun->numUnscheduledYearToDate, mdunscheduledtable->elementAt(mdunscheduledrow, 7)),
                new Wt::WText(enun->lastReceivedDate.toString("MM/dd/yyyy"), mdunscheduledtable->elementAt(mdunscheduledrow, 8));
            }
            mdunscheduledtable->addStyleClass("table form-inline");
            mdunscheduledtable->addStyleClass("table table-striped");
            mdunscheduledtable->addStyleClass("table table-hover");
            container->addWidget(new Wt::WText("<h4>Unscheduled Referrals</h4>"));
            container->addWidget(mdunscheduledtable);
            container->addWidget(new Wt::WBreak());
            container->addWidget(new Wt::WBreak());
        }

    }    

    Wt::WString thisMonthStaffTotalPPO;
    Wt::WString thisMonthStaffTotalScheduledPPO;
    Wt::WString thisMonthStaffTotalUnscheduledPPO;
    Wt::WString lastMonthStaffTotalPPO;
    Wt::WString lastMonthStaffTotalScheduledPPO;
    Wt::WString lastMonthStaffTotalUnscheduledPPO;
    Wt::WString twoMonthStaffTotalPPO;
    Wt::WString twoMonthsStaffTotalScheduledPPO;
    Wt::WString twoMonthsStaffTotalUnscheduledPPO;
    Wt::WString monthFourStaffTotalPPO;
    Wt::WString monthFourStaffTotalScheduledPPO;
    Wt::WString monthFourStaffTotalUnscheduledPPO;    
    Wt::WString monthFiveStaffTotalPPO;
    Wt::WString monthFiveStaffTotalScheduledPPO;
    Wt::WString monthFiveStaffTotalUnscheduledPPO;
    Wt::WString monthSixStaffTotalPPO;
    Wt::WString monthSixStaffTotalScheduledPPO;
    Wt::WString monthSixStaffTotalUnscheduledPPO;
    Wt::WString thisYearStaffTotalPPO;
    Wt::WString thisYearStaffTotalScheduledPPO;
    Wt::WString thisYearStaffTotalUnscheduledPPO;
    
    Wt::WString thisMonthStaffTotalIPA;
    Wt::WString thisMonthStaffTotalScheduledIPA;
    Wt::WString thisMonthStaffTotalUnscheduledIPA;
    Wt::WString lastMonthStaffTotalIPA;
    Wt::WString lastMonthStaffTotalScheduledIPA;
    Wt::WString lastMonthStaffTotalUnscheduledIPA;
    Wt::WString twoMonthStaffTotalIPA;
    Wt::WString twoMonthsStaffTotalScheduledIPA;
    Wt::WString twoMonthsStaffTotalUnscheduledIPA;
    Wt::WString monthFourStaffTotalIPA;
    Wt::WString monthFourStaffTotalScheduledIPA;
    Wt::WString monthFourStaffTotalUnscheduledIPA;    
    Wt::WString monthFiveStaffTotalIPA;
    Wt::WString monthFiveStaffTotalScheduledIPA;
    Wt::WString monthFiveStaffTotalUnscheduledIPA;
    Wt::WString monthSixStaffTotalIPA;
    Wt::WString monthSixStaffTotalScheduledIPA;
    Wt::WString monthSixStaffTotalUnscheduledIPA;
    Wt::WString thisYearStaffTotalIPA;
    Wt::WString thisYearStaffTotalScheduledIPA;
    Wt::WString thisYearStaffTotalUnscheduledIPA;
    
    Wt::WString thisMonthStaffTotalHMO;
    Wt::WString thisMonthStaffTotalScheduledHMO;
    Wt::WString thisMonthStaffTotalUnscheduledHMO;
    Wt::WString lastMonthStaffTotalHMO;
    Wt::WString lastMonthStaffTotalScheduledHMO;
    Wt::WString lastMonthStaffTotalUnscheduledHMO;
    Wt::WString twoMonthStaffTotalHMO;
    Wt::WString twoMonthsStaffTotalScheduledHMO;
    Wt::WString twoMonthsStaffTotalUnscheduledHMO;
    Wt::WString monthFourStaffTotalHMO;
    Wt::WString monthFourStaffTotalScheduledHMO;
    Wt::WString monthFourStaffTotalUnscheduledHMO;    
    Wt::WString monthFiveStaffTotalHMO;
    Wt::WString monthFiveStaffTotalScheduledHMO;
    Wt::WString monthFiveStaffTotalUnscheduledHMO;
    Wt::WString monthSixStaffTotalHMO;
    Wt::WString monthSixStaffTotalScheduledHMO;
    Wt::WString monthSixStaffTotalUnscheduledHMO;
    Wt::WString thisYearStaffTotalHMO;
    Wt::WString thisYearStaffTotalScheduledHMO;
    Wt::WString thisYearStaffTotalUnscheduledHMO;
    


    Wt::WString thisMonthStaffTotalOther;
    Wt::WString thisMonthStaffTotalScheduledOther;
    Wt::WString thisMonthStaffTotalUnscheduledOther;
    Wt::WString lastMonthStaffTotalOther;
    Wt::WString lastMonthStaffTotalScheduledOther;
    Wt::WString lastMonthStaffTotalUnscheduledOther;
    Wt::WString twoMonthStaffTotalOther;
    Wt::WString twoMonthsStaffTotalScheduledOther;
    Wt::WString twoMonthsStaffTotalUnscheduledOther;
    Wt::WString monthFourStaffTotalOther;
    Wt::WString monthFourStaffTotalScheduledOther;
    Wt::WString monthFourStaffTotalUnscheduledOther;    
    Wt::WString monthFiveStaffTotalOther;
    Wt::WString monthFiveStaffTotalScheduledOther;
    Wt::WString monthFiveStaffTotalUnscheduledOther;
    Wt::WString monthSixStaffTotalOther;
    Wt::WString monthSixStaffTotalScheduledOther;
    Wt::WString monthSixStaffTotalUnscheduledOther;
    Wt::WString thisYearStaffTotalOther;
    Wt::WString thisYearStaffTotalScheduledOther;
    Wt::WString thisYearStaffTotalUnscheduledOther;

    Wt::WString kaiserThisMonthTotal;
    Wt::WString kaiserScheduledThisMonthTotal;
    Wt::WString kaiserUnscheduledThisMonthTotal;
    Wt::WString kaiserLastMonthTotal;
    Wt::WString kaiserScheduledLastMonthTotal;
    Wt::WString kaiserUnscheduledLastMonthTotal;
    Wt::WString kaiserTwoMonthsTotal;
    Wt::WString kaiserScheduledTwoMonthsTotal;
    Wt::WString kaiserUnscheduledTwoMonthsTotal;    
    Wt::WString kaiserMonthFourTotal;
    Wt::WString kaiserScheduledMonthFourTotal;
    Wt::WString kaiserUnscheduledMonthFourTotal;    
    Wt::WString kaiserMonthFiveTotal;
    Wt::WString kaiserScheduledMonthFiveTotal;
    Wt::WString kaiserUnscheduledMonthFiveTotal;
    Wt::WString kaiserMonthSixTotal;
    Wt::WString kaiserScheduledMonthSixTotal;
    Wt::WString kaiserUnscheduledMonthSixTotal;

    Wt::WString kaiserYearTotal;
    Wt::WString kaiserScheduledYearTotal;
    Wt::WString kaiserUnscheduledYearTotal;
    std::stringstream thismonthstaffppos;
    thismonthstaffppos << static_cast<int>(staffThisMonthPPO);
    thisMonthStaffTotalPPO = thismonthstaffppos.str();

    std::stringstream thismonthscheduledppos;
    thismonthscheduledppos << static_cast<int>(scheduledThisMonthPPO);
    thisMonthStaffTotalScheduledPPO = thismonthscheduledppos.str();

    std::stringstream thismonthunscheduledppos;
    thismonthunscheduledppos << static_cast<int>(unscheduledThisMonthPPO);
    thisMonthStaffTotalUnscheduledPPO = thismonthunscheduledppos.str();

    std::stringstream lastmonthstaffppos;
    lastmonthstaffppos << static_cast<int>(staffLastMonthPPO);
    lastMonthStaffTotalPPO = lastmonthstaffppos.str();

    std::stringstream lastmonthscheduledppos;
    lastmonthscheduledppos << static_cast<int>(scheduledLastMonthPPO);
    lastMonthStaffTotalScheduledPPO = lastmonthscheduledppos.str();

    std::stringstream lastmonthunscheduledppos;
    lastmonthunscheduledppos << static_cast<int>(unscheduledLastMonthPPO);
    lastMonthStaffTotalUnscheduledPPO = lastmonthunscheduledppos.str();


    std::stringstream twomonthstaffppos;
    twomonthstaffppos << static_cast<int>(staffTwoMonthsPPO);
    twoMonthStaffTotalPPO = twomonthstaffppos.str();

    std::stringstream twomonthsscheduledppos;
    twomonthsscheduledppos << static_cast<int>(scheduledTwoMonthsPPO);
    twoMonthsStaffTotalScheduledPPO = twomonthsscheduledppos.str();

    std::stringstream twomonthsunscheduledppos;
    twomonthsunscheduledppos << static_cast<int>(unscheduledTwoMonthsPPO);
    twoMonthsStaffTotalUnscheduledPPO = twomonthsunscheduledppos.str();

    std::stringstream monthfourstaffppos;
    monthfourstaffppos << static_cast<int>(staffMonthFourPPO);
    monthFourStaffTotalPPO = monthfourstaffppos.str();

    std::stringstream monthfourscheduledppos;
    monthfourscheduledppos << static_cast<int>(scheduledMonthFourPPO);
    monthFourStaffTotalScheduledPPO = monthfourscheduledppos.str();

    std::stringstream monthfourunscheduledppos;
    monthfourunscheduledppos << static_cast<int>(unscheduledMonthFourPPO);
    monthFourStaffTotalUnscheduledPPO = monthfourunscheduledppos.str();


    std::stringstream monthfivestaffppos;
    monthfivestaffppos << static_cast<int>(staffMonthFivePPO);
    monthFiveStaffTotalPPO = monthfivestaffppos.str();

    std::stringstream monthfivescheduledppos;
    monthfivescheduledppos << static_cast<int>(scheduledMonthFivePPO);
    monthFiveStaffTotalScheduledPPO = monthfivescheduledppos.str();

    std::stringstream monthfiveunscheduledppos;
    monthfiveunscheduledppos << static_cast<int>(unscheduledMonthFivePPO);
    monthFiveStaffTotalUnscheduledPPO = monthfiveunscheduledppos.str();


    std::stringstream monthsixstaffppos;
    monthsixstaffppos << static_cast<int>(staffMonthSixPPO);
    monthSixStaffTotalPPO = monthsixstaffppos.str();

    std::stringstream monthsixscheduledppos;
    monthsixscheduledppos << static_cast<int>(scheduledMonthSixPPO);
    monthSixStaffTotalScheduledPPO = monthsixscheduledppos.str();

    std::stringstream monthsixunscheduledppos;
    monthsixunscheduledppos << static_cast<int>(unscheduledMonthSixPPO);
    monthSixStaffTotalUnscheduledPPO = monthsixunscheduledppos.str();


    std::cout << "YearToDate Count:" << staffYearToDatePPO;
    std::stringstream yearstaffppos;
    yearstaffppos << static_cast<int>(staffYearToDatePPO);
    thisYearStaffTotalPPO = yearstaffppos.str();

    std::stringstream thisyearscheduledppos;
    thisyearscheduledppos << static_cast<int>(scheduledYearToDatePPO);
    thisYearStaffTotalScheduledPPO = thisyearscheduledppos.str();

    std::stringstream thisyearunscheduledppos;
    thisyearunscheduledppos << static_cast<int>(unscheduledYearToDatePPO);
    thisYearStaffTotalUnscheduledPPO = thisyearunscheduledppos.str();


    std::stringstream thismonthstaffipas;
    thismonthstaffipas << static_cast<int>(staffThisMonthIPA);
    thisMonthStaffTotalIPA = thismonthstaffipas.str();

    std::stringstream thismonthscheduledipas;
    thismonthscheduledipas << static_cast<int>(scheduledThisMonthIPA);
    thisMonthStaffTotalScheduledIPA = thismonthscheduledipas.str();

    std::stringstream thismonthunscheduledipas;
    thismonthunscheduledipas << static_cast<int>(unscheduledThisMonthIPA);
    thisMonthStaffTotalUnscheduledIPA = thismonthunscheduledipas.str();

    std::stringstream lastmonthstaffipas;
    lastmonthstaffipas << static_cast<int>(staffLastMonthIPA);
    lastMonthStaffTotalIPA = lastmonthstaffipas.str();


    std::stringstream lastmonthscheduledipas;
    lastmonthscheduledipas << static_cast<int>(scheduledLastMonthIPA);
    lastMonthStaffTotalScheduledIPA = lastmonthscheduledipas.str();

    std::stringstream lastmonthunscheduledipas;
    lastmonthunscheduledipas << static_cast<int>(unscheduledLastMonthIPA);
    lastMonthStaffTotalUnscheduledIPA = lastmonthunscheduledipas.str();

    std::stringstream twomonthstaffipas;
    twomonthstaffipas << static_cast<int>(staffTwoMonthsIPA);
    twoMonthStaffTotalIPA = twomonthstaffipas.str();    

    std::stringstream twomonthsscheduledipas;
    twomonthsscheduledipas << static_cast<int>(scheduledTwoMonthsIPA);
    twoMonthsStaffTotalScheduledIPA = twomonthsscheduledipas.str();

    std::stringstream twomonthsunscheduledipas;
    twomonthsunscheduledipas << static_cast<int>(unscheduledTwoMonthsIPA);
    twoMonthsStaffTotalUnscheduledIPA = twomonthsunscheduledipas.str();
    
    std::stringstream monthfourstaffipas;
    monthfourstaffipas << static_cast<int>(staffMonthFourIPA);
    monthFourStaffTotalIPA = monthfourstaffipas.str();
    
    std::stringstream monthfourscheduledipas;
    monthfourscheduledipas << static_cast<int>(scheduledMonthFourIPA);
    monthFourStaffTotalScheduledIPA = monthfourscheduledipas.str();

    std::stringstream monthfourunscheduledipas;
    monthfourunscheduledipas << static_cast<int>(unscheduledMonthFourIPA);
    monthFourStaffTotalUnscheduledIPA = monthfourunscheduledipas.str();

    std::stringstream monthfivestaffipas;
    monthfivestaffipas << static_cast<int>(staffMonthFiveIPA);
    monthFiveStaffTotalIPA = monthfivestaffipas.str();

    std::stringstream monthfivescheduledipas;
    monthfivescheduledipas << static_cast<int>(scheduledMonthFiveIPA);
    monthFiveStaffTotalScheduledIPA = monthfivescheduledipas.str();

    std::stringstream monthfiveunscheduledipas;
    monthfiveunscheduledipas << static_cast<int>(unscheduledMonthFiveIPA);
    monthFiveStaffTotalUnscheduledIPA = monthfiveunscheduledipas.str();

    std::stringstream monthsixstaffipas;
    monthsixstaffipas << static_cast<int>(staffMonthSixIPA);
    monthSixStaffTotalIPA = monthsixstaffipas.str();
    
    std::stringstream monthsixscheduledipas;
    monthsixscheduledipas << static_cast<int>(scheduledMonthSixIPA);
    monthSixStaffTotalScheduledIPA = monthsixscheduledipas.str();

    std::stringstream monthsixunscheduledipas;
    monthsixunscheduledipas << static_cast<int>(unscheduledMonthSixIPA);
    monthSixStaffTotalUnscheduledIPA = monthsixunscheduledipas.str();

    std::cout << "YearToDate Count:" << staffYearToDateIPA;
    std::stringstream yearstaffipas;
    yearstaffipas << static_cast<int>(staffYearToDateIPA);
    thisYearStaffTotalIPA = yearstaffipas.str();

    std::stringstream thisyearscheduledipas;
    thisyearscheduledipas << static_cast<int>(scheduledYearToDateIPA);
    thisYearStaffTotalScheduledIPA = thisyearscheduledipas.str();

    std::stringstream thisyearunscheduledipas;
    thisyearunscheduledipas << static_cast<int>(unscheduledYearToDateIPA);
    thisYearStaffTotalUnscheduledIPA = thisyearunscheduledipas.str();

    std::stringstream thismonthstaffhmos;
    thismonthstaffhmos << static_cast<int>(staffThisMonthHMO);
    thisMonthStaffTotalHMO = thismonthstaffhmos.str();
    
    std::stringstream thismonthscheduledhmos;
    thismonthscheduledhmos << static_cast<int>(scheduledThisMonthHMO);
    thisMonthStaffTotalScheduledHMO = thismonthscheduledhmos.str();

    std::stringstream thismonthunscheduledhmos;
    thismonthunscheduledhmos << static_cast<int>(unscheduledThisMonthHMO);
    thisMonthStaffTotalUnscheduledHMO = thismonthunscheduledhmos.str();

    std::stringstream lastmonthstaffhmos;
    lastmonthstaffhmos << static_cast<int>(staffLastMonthHMO);
    lastMonthStaffTotalHMO = lastmonthstaffhmos.str();

    std::stringstream lastmonthscheduledhmos;
    lastmonthscheduledhmos << static_cast<int>(scheduledLastMonthHMO);
    lastMonthStaffTotalScheduledHMO = lastmonthscheduledhmos.str();

    std::stringstream lastmonthunscheduledhmos;
    lastmonthunscheduledhmos << static_cast<int>(unscheduledLastMonthHMO);
    lastMonthStaffTotalUnscheduledHMO = lastmonthunscheduledhmos.str();

    std::stringstream twomonthstaffhmos;
    twomonthstaffhmos << static_cast<int>(staffTwoMonthsHMO);
    twoMonthStaffTotalHMO = twomonthstaffhmos.str();

    std::stringstream twomonthsscheduledhmos;
    twomonthsscheduledhmos << static_cast<int>(scheduledTwoMonthsHMO);
    twoMonthsStaffTotalScheduledHMO = twomonthsscheduledhmos.str();

    std::stringstream twomonthsunscheduledhmos;
    twomonthsunscheduledhmos << static_cast<int>(unscheduledTwoMonthsHMO);
    twoMonthsStaffTotalUnscheduledHMO = twomonthsunscheduledhmos.str();

    std::stringstream monthfourstaffhmos;
    monthfourstaffhmos << static_cast<int>(staffMonthFourHMO);
    monthFourStaffTotalHMO = monthfourstaffhmos.str();

    std::stringstream monthfourscheduledhmos;
    monthfourscheduledhmos << static_cast<int>(scheduledMonthFourHMO);
    monthFourStaffTotalScheduledHMO = monthfourscheduledhmos.str();

    std::stringstream monthfourunscheduledhmos;
    monthfourunscheduledhmos << static_cast<int>(unscheduledMonthFourHMO);
    monthFourStaffTotalUnscheduledHMO = monthfourunscheduledhmos.str();

    std::stringstream monthfivestaffhmos;
    monthfivestaffhmos << static_cast<int>(staffMonthFiveHMO);
    monthFiveStaffTotalHMO = monthfivestaffhmos.str();
    
    std::stringstream monthfivescheduledhmos;
    monthfivescheduledhmos << static_cast<int>(scheduledMonthFiveHMO);
    monthFiveStaffTotalScheduledHMO = monthfivescheduledhmos.str();

    std::stringstream monthfiveunscheduledhmos;
    monthfiveunscheduledhmos << static_cast<int>(unscheduledMonthFiveHMO);
    monthFiveStaffTotalUnscheduledHMO = monthfiveunscheduledhmos.str();
    
    std::stringstream monthsixstaffhmos;
    monthsixstaffhmos << static_cast<int>(staffMonthSixHMO);
    monthSixStaffTotalHMO = monthsixstaffhmos.str();

    std::stringstream monthsixscheduledhmos;
    monthsixscheduledhmos << static_cast<int>(scheduledMonthSixHMO);
    monthSixStaffTotalScheduledHMO = monthsixscheduledhmos.str();

    std::stringstream monthsixunscheduledhmos;
    monthsixunscheduledhmos << static_cast<int>(unscheduledMonthSixHMO);
    monthSixStaffTotalUnscheduledHMO = monthsixunscheduledhmos.str();

    std::cout << "YearToDate Count:" << staffYearToDateHMO;
    std::stringstream yearstaffhmos;
    yearstaffhmos << static_cast<int>(staffYearToDateHMO);
    thisYearStaffTotalHMO = yearstaffhmos.str();

    std::stringstream thisyearscheduledhmos;
    thisyearscheduledhmos << static_cast<int>(scheduledYearToDateHMO);
    thisYearStaffTotalScheduledHMO = thisyearscheduledhmos.str();

    std::stringstream thisyearunscheduledhmos;
    thisyearunscheduledhmos << static_cast<int>(unscheduledYearToDateHMO);
    thisYearStaffTotalUnscheduledHMO = thisyearunscheduledhmos.str();

    std::stringstream thismonthstaffothers;
    thismonthstaffothers << static_cast<int>(staffThisMonthOther);
    thisMonthStaffTotalOther = thismonthstaffothers.str();

    std::stringstream thismonthscheduledothers;
    thismonthscheduledothers << static_cast<int>(scheduledThisMonthOther);
    thisMonthStaffTotalScheduledOther = thismonthscheduledothers.str();

    std::stringstream thismonthunscheduledothers;
    thismonthunscheduledothers << static_cast<int>(unscheduledThisMonthOther);
    thisMonthStaffTotalUnscheduledOther = thismonthunscheduledothers.str();


    std::stringstream lastmonthstaffothers;
    lastmonthstaffothers << static_cast<int>(staffLastMonthOther);
    lastMonthStaffTotalOther = lastmonthstaffothers.str();

    std::stringstream lastmonthscheduledothers;
    lastmonthscheduledothers << static_cast<int>(scheduledLastMonthOther);
    lastMonthStaffTotalScheduledOther = lastmonthscheduledothers.str();

    std::stringstream lastmonthunscheduledothers;
    lastmonthunscheduledothers << static_cast<int>(unscheduledLastMonthOther);
    lastMonthStaffTotalUnscheduledOther = lastmonthunscheduledothers.str();


    std::stringstream twomonthstaffothers;
    twomonthstaffothers << static_cast<int>(staffTwoMonthsOther);
    twoMonthStaffTotalOther = twomonthstaffothers.str();

    std::stringstream twomonthsscheduledothers;
    twomonthsscheduledothers << static_cast<int>(scheduledTwoMonthsOther);
    twoMonthsStaffTotalScheduledOther = twomonthsscheduledothers.str();

    std::stringstream twomonthsunscheduledothers;
    twomonthsunscheduledothers << static_cast<int>(unscheduledTwoMonthsOther);
    twoMonthsStaffTotalUnscheduledOther = twomonthsunscheduledothers.str();

    std::stringstream monthfourstaffothers;
    monthfourstaffothers << static_cast<int>(staffMonthFourOther);
    monthFourStaffTotalOther = monthfourstaffothers.str();

    std::stringstream monthfourscheduledothers;
    monthfourscheduledothers << static_cast<int>(scheduledMonthFourOther);
    monthFourStaffTotalScheduledOther = monthfourscheduledothers.str();

    std::stringstream monthfourunscheduledothers;
    monthfourunscheduledothers << static_cast<int>(unscheduledMonthFourOther);
    monthFourStaffTotalUnscheduledOther = monthfourunscheduledothers.str();

    std::stringstream monthfivestaffothers;
    monthfivestaffothers << static_cast<int>(staffMonthFiveOther);
    monthFiveStaffTotalOther = monthfivestaffothers.str();
    
    std::stringstream monthfivescheduledothers;
    monthfivescheduledothers << static_cast<int>(scheduledMonthFiveOther);
    monthFiveStaffTotalScheduledOther = monthfivescheduledothers.str();

    std::stringstream monthfiveunscheduledothers;
    monthfiveunscheduledothers << static_cast<int>(unscheduledMonthFiveOther);
    monthFiveStaffTotalUnscheduledOther = monthfiveunscheduledothers.str();

    std::stringstream monthsixstaffothers;
    monthsixstaffothers << static_cast<int>(staffMonthSixOther);
    monthSixStaffTotalOther = monthsixstaffothers.str();
    
    std::stringstream monthsixscheduledothers;
    monthsixscheduledothers << static_cast<int>(scheduledMonthSixOther);
    monthSixStaffTotalScheduledOther = monthsixscheduledothers.str();

    std::stringstream monthsixunscheduledothers;
    monthsixunscheduledothers << static_cast<int>(unscheduledMonthSixOther);
    monthSixStaffTotalUnscheduledOther = monthsixunscheduledothers.str();
    
    std::cout << "YearToDate Count:" << staffYearToDateOther;
    std::stringstream yearstaffothers;
    yearstaffothers << static_cast<int>(staffYearToDateOther);
    thisYearStaffTotalOther = yearstaffothers.str();

    std::stringstream thisyearscheduledothers;
    thisyearscheduledothers << static_cast<int>(scheduledYearToDateOther);
    thisYearStaffTotalScheduledOther = thisyearscheduledothers.str();

    std::stringstream thisyearunscheduledothers;
    thisyearunscheduledothers << static_cast<int>(unscheduledYearToDateOther);
    thisYearStaffTotalUnscheduledOther = thisyearunscheduledothers.str();


    std::stringstream kaiserthismonth;
    kaiserthismonth << static_cast<int>(kaiserThisMonth);
    kaiserThisMonthTotal = kaiserthismonth.str();
    
    std::stringstream kaiserthismonthscheduled;
    kaiserthismonthscheduled << static_cast<int>(kaiserScheduledThisMonth);
    kaiserScheduledThisMonthTotal = kaiserthismonthscheduled.str();    

    std::stringstream kaiserthismonthunscheduled;
    kaiserthismonthunscheduled << static_cast<int>(kaiserUnscheduledThisMonth);
    kaiserUnscheduledThisMonthTotal = kaiserthismonthunscheduled.str();    

    std::stringstream kaiserlastmonth;
    kaiserlastmonth << static_cast<int>(kaiserLastMonth);
    kaiserLastMonthTotal = kaiserlastmonth.str();

    std::stringstream kaiserlastmonthscheduled;
    kaiserlastmonthscheduled << static_cast<int>(kaiserScheduledLastMonth);
    kaiserScheduledLastMonthTotal = kaiserlastmonthscheduled.str();    

    std::stringstream kaiserlastmonthunscheduled;
    kaiserlastmonthunscheduled << static_cast<int>(kaiserUnscheduledLastMonth);
    kaiserUnscheduledLastMonthTotal = kaiserlastmonthunscheduled.str();    

    std::stringstream kaisertwomonths;
    kaisertwomonths << static_cast<int>(kaiserTwoMonths);
    kaiserTwoMonthsTotal = kaisertwomonths.str();

    std::stringstream kaisertwomonthsscheduled;
    kaisertwomonthsscheduled << static_cast<int>(kaiserScheduledTwoMonths);
    kaiserScheduledTwoMonthsTotal = kaisertwomonthsscheduled.str();    

    std::stringstream kaisertwomonthsunscheduled;
    kaisertwomonthsunscheduled << static_cast<int>(kaiserUnscheduledTwoMonths);
    kaiserUnscheduledTwoMonthsTotal = kaisertwomonthsunscheduled.str();    

    std::stringstream kaisermonthfour;
    kaisermonthfour << static_cast<int>(kaiserMonthFour);
    kaiserMonthFourTotal = kaisermonthfour.str();
    
    std::stringstream kaisermonthfourscheduled;
    kaisermonthfourscheduled << static_cast<int>(kaiserScheduledMonthFour);
    kaiserScheduledMonthFourTotal = kaisermonthfourscheduled.str();    

    std::stringstream kaisermonthfourunscheduled;
    kaisermonthfourunscheduled << static_cast<int>(kaiserUnscheduledMonthFour);
    kaiserUnscheduledMonthFourTotal = kaisermonthfourunscheduled.str();    

    std::stringstream kaisermonthfive;
    kaisermonthfive << static_cast<int>(kaiserMonthFive);
    kaiserMonthFiveTotal = kaisermonthfive.str();
    
    std::stringstream kaisermonthfivescheduled;
    kaisermonthfivescheduled << static_cast<int>(kaiserScheduledMonthFive);
    kaiserScheduledMonthFiveTotal = kaisermonthfivescheduled.str();    

    std::stringstream kaisermonthfiveunscheduled;
    kaisermonthfiveunscheduled << static_cast<int>(kaiserUnscheduledMonthFive);
    kaiserUnscheduledMonthFiveTotal = kaisermonthfiveunscheduled.str();  

    std::stringstream kaisermonthsix;
    kaisermonthsix << static_cast<int>(kaiserMonthSix);
    kaiserMonthSixTotal = kaisermonthsix.str();

    std::stringstream kaisermonthsixscheduled;
    kaisermonthsixscheduled << static_cast<int>(kaiserScheduledMonthSix);
    kaiserScheduledMonthSixTotal = kaisermonthsixscheduled.str();    

    std::stringstream kaisermonthsixunscheduled;
    kaisermonthsixunscheduled << static_cast<int>(kaiserUnscheduledMonthSix);
    kaiserUnscheduledMonthSixTotal = kaisermonthsixunscheduled.str();  

    std::stringstream kaiseryear;
    kaiseryear << static_cast<int>(kaiserYearToDate);
    kaiserYearTotal = kaiseryear.str();

    std::stringstream kaiseryearscheduled;
    kaiseryearscheduled << static_cast<int>(kaiserScheduledYear);
    kaiserScheduledYearTotal = kaiseryearscheduled.str(); 

    std::stringstream kaiseryearunscheduled;
    kaiseryearunscheduled << static_cast<int>(kaiserUnscheduledYear);
    kaiserUnscheduledYearTotal = kaiseryearunscheduled.str(); 



    std::vector<ReferralCountEntry> entries;
    
    typedef dbo::collection< dbo::ptr<InsuranceGroup> > PGroups;
    PGroups pgroups = dbsession.find<InsuranceGroup>();
    std::cerr << pgroups.size() << std::endl;
    
    for (PGroups::const_iterator pg = pgroups.begin(); pg != pgroups.end(); ++pg) {
        ReferralCountEntry *rcent = new ReferralCountEntry();   
        rcent->groupName = (*pg)->groupName_;
        entries.push_back(*rcent);
    }
    Wt::WDate servernow = Wt::WDate::currentServerDate();
    Wt::WDate thisMonth;
    Wt::WDate lastMonth;
    Wt::WDate twoMonths;
    Wt::WDate monthFour;
    Wt::WDate monthFive;
    Wt::WDate monthSix;
    Wt::WDate yearStart;
    
    Wt::WString month1;
    Wt::WString month2;
    Wt::WString month3;
    Wt::WString month4;
    Wt::WString month5;
    Wt::WString month6;

    thisMonth.setDate(servernow.year(), servernow.month(), 1);
    Wt::log("info") << thisMonth.toString("MM/dd/yyyy");

    lastMonth = thisMonth.addMonths(-1);
    Wt::log("info") << lastMonth.toString("MM/dd/yyyy");

    twoMonths = lastMonth.addMonths(-1);
    Wt::log("info") << twoMonths.toString("MM/dd/yyyy");

    monthFour = twoMonths.addMonths(-1);
    monthFive = monthFour.addMonths(-1);
    monthSix = monthFive.addMonths(-1);

    yearStart.setDate(servernow.year(), 1, 1);
    Wt::log("info") << yearStart.toString("MM/dd/yyyy");
    month1 = Wt::WDate::shortMonthName(thisMonth.month());
    month2 = Wt::WDate::shortMonthName(lastMonth.month());
    month3 = Wt::WDate::shortMonthName(twoMonths.month());
    month4 = Wt::WDate::shortMonthName(monthFour.month());
    month5 = Wt::WDate::shortMonthName(monthFive.month());
    month6 = Wt::WDate::shortMonthName(monthSix.month());
    
    Wt::WTable *kaisertable = new Wt::WTable();

    kaisertable->setHeaderCount(1);
    kaisertable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    kaisertable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    kaisertable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    kaisertable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    kaisertable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    kaisertable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    kaisertable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    kaisertable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    int kaiserrow = 1;

    new Wt::WText("Total Referrals", kaisertable->elementAt(kaiserrow, 0));
    new Wt::WText(kaiserThisMonthTotal, kaisertable->elementAt(kaiserrow, 1)),
    new Wt::WText(kaiserLastMonthTotal, kaisertable->elementAt(kaiserrow, 2)),
    new Wt::WText(kaiserTwoMonthsTotal, kaisertable->elementAt(kaiserrow, 3)),
    new Wt::WText(kaiserMonthFourTotal, kaisertable->elementAt(kaiserrow, 4)),
    new Wt::WText(kaiserMonthFiveTotal, kaisertable->elementAt(kaiserrow, 5)),
    new Wt::WText(kaiserMonthSixTotal, kaisertable->elementAt(kaiserrow, 6)),
    new Wt::WText(kaiserYearTotal, kaisertable->elementAt(kaiserrow, 7));
    kaiserrow = 2;
    new Wt::WText("Scheduled Referrals", kaisertable->elementAt(kaiserrow, 0));
    new Wt::WText(kaiserScheduledThisMonthTotal, kaisertable->elementAt(kaiserrow, 1)),
    new Wt::WText(kaiserScheduledLastMonthTotal, kaisertable->elementAt(kaiserrow, 2)),
    new Wt::WText(kaiserScheduledTwoMonthsTotal, kaisertable->elementAt(kaiserrow, 3)),
    new Wt::WText(kaiserScheduledMonthFourTotal, kaisertable->elementAt(kaiserrow, 4)),
    new Wt::WText(kaiserScheduledMonthFiveTotal, kaisertable->elementAt(kaiserrow, 5)),
    new Wt::WText(kaiserScheduledMonthSixTotal, kaisertable->elementAt(kaiserrow, 6)),
    new Wt::WText(kaiserScheduledYearTotal, kaisertable->elementAt(kaiserrow, 7));
    kaiserrow = 3;
    new Wt::WText("Unscheduled Referrals", kaisertable->elementAt(kaiserrow, 0));
    new Wt::WText(kaiserUnscheduledThisMonthTotal, kaisertable->elementAt(kaiserrow, 1)),
    new Wt::WText(kaiserUnscheduledLastMonthTotal, kaisertable->elementAt(kaiserrow, 2)),
    new Wt::WText(kaiserUnscheduledTwoMonthsTotal, kaisertable->elementAt(kaiserrow, 3)),
    new Wt::WText(kaiserUnscheduledMonthFourTotal, kaisertable->elementAt(kaiserrow, 4)),
    new Wt::WText(kaiserUnscheduledMonthFiveTotal, kaisertable->elementAt(kaiserrow, 5)),
    new Wt::WText(kaiserUnscheduledMonthSixTotal, kaisertable->elementAt(kaiserrow, 6)),
    new Wt::WText(kaiserUnscheduledYearTotal, kaisertable->elementAt(kaiserrow, 7));
    
    kaisertable->addStyleClass("table form-inline");
    kaisertable->addStyleClass("table table-striped");
    kaisertable->addStyleClass("table table-hover");
    container->addWidget(new Wt::WText("<h4>Kaiser Referrals</h4>"));

    container->addWidget(kaisertable);

    // Wt::WTable *kaiserscheduledtable = new Wt::WTable();

    // kaiserscheduledtable->setHeaderCount(1);
    // kaiserscheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    // kaiserscheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    // kaiserscheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    // kaiserscheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    // kaiserscheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    // kaiserscheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    // kaiserscheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    // kaiserscheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    // int kaiserscheduledrow = 1;
    
    // new Wt::WText("KaiserScheduled", kaiserscheduledtable->elementAt(kaiserscheduledrow, 0));
    // new Wt::WText(kaiserScheduledThisMonthTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 1)),
    // new Wt::WText(kaiserScheduledLastMonthTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 2)),
    // new Wt::WText(kaiserScheduledTwoMonthsTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 3)),
    // new Wt::WText(kaiserScheduledMonthFourTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 4)),
    // new Wt::WText(kaiserScheduledMonthFiveTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 5)),
    // new Wt::WText(kaiserScheduledMonthSixTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 6)),
    // new Wt::WText(kaiserScheduledYearTotal, kaiserscheduledtable->elementAt(kaiserscheduledrow, 7));
  
    
    // kaiserscheduledtable->addStyleClass("table form-inline");
    // kaiserscheduledtable->addStyleClass("table table-striped");
    // kaiserscheduledtable->addStyleClass("table table-hover");
    // container->addWidget(new Wt::WText("<h4>Total Scheduled Kaiser Referrals</h4>"));

    // container->addWidget(kaiserscheduledtable);

    // Wt::WTable *kaiserunscheduledtable = new Wt::WTable();

    // kaiserunscheduledtable->setHeaderCount(1);
    // kaiserunscheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    // kaiserunscheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    // kaiserunscheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    // kaiserunscheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    // kaiserunscheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    // kaiserunscheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    // kaiserunscheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    // kaiserunscheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    // int kaiserunscheduledrow = 1;
    
    // new Wt::WText("KaiserUnscheduled", kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 0));
    // new Wt::WText(kaiserUnscheduledThisMonthTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 1)),
    // new Wt::WText(kaiserUnscheduledLastMonthTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 2)),
    // new Wt::WText(kaiserUnscheduledTwoMonthsTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 3)),
    // new Wt::WText(kaiserUnscheduledMonthFourTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 4)),
    // new Wt::WText(kaiserUnscheduledMonthFiveTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 5)),
    // new Wt::WText(kaiserUnscheduledMonthSixTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 6)),
    // new Wt::WText(kaiserUnscheduledYearTotal, kaiserunscheduledtable->elementAt(kaiserunscheduledrow, 7));

    // kaiserunscheduledtable->addStyleClass("table form-inline");
    // kaiserunscheduledtable->addStyleClass("table table-striped");
    // kaiserunscheduledtable->addStyleClass("table table-hover");
    // container->addWidget(new Wt::WText("<h4>Total Unscheduled Kaiser Referrals</h4>"));

    // container->addWidget(kaiserunscheduledtable);



    Wt::WTable *stafftable = new Wt::WTable();

    stafftable->setHeaderCount(1);
    stafftable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    stafftable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    stafftable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    stafftable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    stafftable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    stafftable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    stafftable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    stafftable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    int staffrow = 1;
    
    for (std::vector<ReferralCountEntry>::iterator en = entries.begin(); en != entries.end(); ++en, ++staffrow)
    {
        new Wt::WText(en->groupName, stafftable->elementAt(staffrow, 0));
        if (en->groupName == "PPO") {
            new Wt::WText(thisMonthStaffTotalPPO, stafftable->elementAt(staffrow, 1)),
            new Wt::WText(lastMonthStaffTotalPPO, stafftable->elementAt(staffrow, 2)),
            new Wt::WText(twoMonthStaffTotalPPO, stafftable->elementAt(staffrow, 3)),
            new Wt::WText(monthFourStaffTotalPPO, stafftable->elementAt(staffrow, 4)),
            new Wt::WText(monthFiveStaffTotalPPO, stafftable->elementAt(staffrow, 5)),
            new Wt::WText(monthSixStaffTotalPPO, stafftable->elementAt(staffrow, 6)),
            new Wt::WText(thisYearStaffTotalPPO, stafftable->elementAt(staffrow, 7));
        } else if(en->groupName == "IPA") {
            new Wt::WText(thisMonthStaffTotalIPA, stafftable->elementAt(staffrow, 1)),
            new Wt::WText(lastMonthStaffTotalIPA, stafftable->elementAt(staffrow, 2)),
            new Wt::WText(twoMonthStaffTotalIPA, stafftable->elementAt(staffrow, 3)),
            new Wt::WText(monthFourStaffTotalIPA, stafftable->elementAt(staffrow, 4)),
            new Wt::WText(monthFiveStaffTotalIPA, stafftable->elementAt(staffrow, 5)),
            new Wt::WText(monthSixStaffTotalIPA, stafftable->elementAt(staffrow, 6)),
            new Wt::WText(thisYearStaffTotalIPA, stafftable->elementAt(staffrow, 7));
        } else if (en->groupName == "HMO") {
            new Wt::WText(thisMonthStaffTotalHMO, stafftable->elementAt(staffrow, 1)),
            new Wt::WText(lastMonthStaffTotalHMO, stafftable->elementAt(staffrow, 2)),
            new Wt::WText(twoMonthStaffTotalHMO, stafftable->elementAt(staffrow, 3)),
            new Wt::WText(monthFourStaffTotalHMO, stafftable->elementAt(staffrow, 4)),
            new Wt::WText(monthFiveStaffTotalHMO, stafftable->elementAt(staffrow, 5)),
            new Wt::WText(monthSixStaffTotalHMO, stafftable->elementAt(staffrow, 6)),
            new Wt::WText(thisYearStaffTotalHMO, stafftable->elementAt(staffrow, 7));
        } else {
            new Wt::WText(thisMonthStaffTotalOther, stafftable->elementAt(staffrow, 1)),
            new Wt::WText(lastMonthStaffTotalOther, stafftable->elementAt(staffrow, 2)),
            new Wt::WText(twoMonthStaffTotalOther, stafftable->elementAt(staffrow, 3)),
            new Wt::WText(monthFourStaffTotalOther, stafftable->elementAt(staffrow, 4)),
            new Wt::WText(monthFiveStaffTotalOther, stafftable->elementAt(staffrow, 5)),
            new Wt::WText(monthSixStaffTotalOther, stafftable->elementAt(staffrow, 6)),
            new Wt::WText(thisYearStaffTotalOther, stafftable->elementAt(staffrow, 7));
        }        
        new Wt::WText(en->lastReceivedDate.toString("MM/dd/yyyy"), stafftable->elementAt(staffrow, 8));
    }
    stafftable->addStyleClass("table form-inline");
    stafftable->addStyleClass("table table-striped");
    stafftable->addStyleClass("table table-hover");
    container->addWidget(new Wt::WText("<h4>Total Referrals</h4>"));

    container->addWidget(stafftable);

    Wt::WTable *scheduledtable = new Wt::WTable();

    scheduledtable->setHeaderCount(1);
    scheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    scheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    scheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    scheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    scheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    scheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    scheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    scheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    int scheduledrow = 1;
    
    for (std::vector<ReferralCountEntry>::iterator ensch = entries.begin(); ensch != entries.end(); ++ensch, ++scheduledrow)
    {
        new Wt::WText(ensch->groupName, scheduledtable->elementAt(scheduledrow, 0));
        if (ensch->groupName == "PPO") {
            new Wt::WText(thisMonthStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalScheduledPPO, scheduledtable->elementAt(scheduledrow, 7));
        } else if(ensch->groupName == "IPA") {
            new Wt::WText(thisMonthStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalScheduledIPA, scheduledtable->elementAt(scheduledrow, 7));
        } else if (ensch->groupName == "HMO") {
            new Wt::WText(thisMonthStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalScheduledHMO, scheduledtable->elementAt(scheduledrow, 7));
        } else {
            new Wt::WText(thisMonthStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalScheduledOther, scheduledtable->elementAt(scheduledrow, 7));
        }        
        new Wt::WText(ensch->lastReceivedDate.toString("MM/dd/yyyy"), scheduledtable->elementAt(scheduledrow, 8));
    }
    scheduledtable->addStyleClass("table form-inline");
    scheduledtable->addStyleClass("table table-striped");
    scheduledtable->addStyleClass("table table-hover");
    container->addWidget(new Wt::WText("<h4>Total Scheduled Referrals</h4>"));

    container->addWidget(scheduledtable);

    Wt::WTable *unscheduledtable = new Wt::WTable();

    unscheduledtable->setHeaderCount(1);
    unscheduledtable->elementAt(0, 0)->addWidget(new Wt::WText(""));
    unscheduledtable->elementAt(0, 1)->addWidget(new Wt::WText(month1));
    unscheduledtable->elementAt(0, 2)->addWidget(new Wt::WText(month2));
    unscheduledtable->elementAt(0, 3)->addWidget(new Wt::WText(month3));
    unscheduledtable->elementAt(0, 4)->addWidget(new Wt::WText(month4));
    unscheduledtable->elementAt(0, 5)->addWidget(new Wt::WText(month5));
    unscheduledtable->elementAt(0, 6)->addWidget(new Wt::WText(month6));
    unscheduledtable->elementAt(0, 7)->addWidget(new Wt::WText("YTD"));

    int unscheduledrow = 1;
    
    for (std::vector<ReferralCountEntry>::iterator enun = entries.begin(); enun != entries.end(); ++enun, ++unscheduledrow)
    {
        new Wt::WText(enun->groupName, unscheduledtable->elementAt(unscheduledrow, 0));
        if (enun->groupName == "PPO") {
            new Wt::WText(thisMonthStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalUnscheduledPPO, unscheduledtable->elementAt(unscheduledrow, 7));
        } else if(enun->groupName == "IPA") {
            new Wt::WText(thisMonthStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalUnscheduledIPA, unscheduledtable->elementAt(unscheduledrow, 7));
        } else if (enun->groupName == "HMO") {
            new Wt::WText(thisMonthStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalUnscheduledHMO, unscheduledtable->elementAt(unscheduledrow, 7));
        } else {
            new Wt::WText(thisMonthStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 1)),
            new Wt::WText(lastMonthStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 2)),
            new Wt::WText(twoMonthsStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 3)),
            new Wt::WText(monthFourStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 4)),
            new Wt::WText(monthFiveStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 5)),
            new Wt::WText(monthSixStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 6)),
            new Wt::WText(thisYearStaffTotalUnscheduledOther, unscheduledtable->elementAt(unscheduledrow, 7));
        }        
        new Wt::WText(enun->lastReceivedDate.toString("MM/dd/yyyy"), unscheduledtable->elementAt(unscheduledrow, 8));
    }

    unscheduledtable->addStyleClass("table form-inline");
    unscheduledtable->addStyleClass("table table-striped");
    unscheduledtable->addStyleClass("table table-hover");
    container->addWidget(new Wt::WText("<h4>Total Unscheduled Referrals</h4>"));

    container->addWidget(unscheduledtable);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());



    allDone();
    transaction.commit();

}
void AdminPracticeCountWidget::allDone()
{
    startButton->disable();
    startButton->setText("Complete");
    startButton->hide();

    container->show();
}