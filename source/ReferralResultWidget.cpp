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
#include <Wt/WMessageBox>

#include "ReferralResultWidget.h"
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
namespace {
    /*
     * A standard item for referrals by insurance count chart
     */
    class NumericItem : public Wt::WStandardItem {
        public:
            virtual NumericItem *clone() const {
                return new NumericItem();
            }

            virtual void setData(const boost::any &data, int role = Wt::UserRole) {
                boost::any dt;

                if (role == Wt::EditRole) {
                double d = Wt::asNumber(data);

                if (d != d)
                    dt = data;
                else
                    dt = boost::any(d);
                }

                Wt::WStandardItem::setData(dt, role);
            }
    };
}

ReferralResultWidget::ReferralResultWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
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
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Insurance>("insurance");

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
void ReferralResultWidget::showAllReferralLog(Wt::WDate& startDate, Wt::WDate& endDate)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);


    //define logentries vector here
    std::vector<ReferralLogEntry> entries;
    std::vector<ReferralLogEntry> unscheduled;
    std::vector<ReferralLogEntry> scheduled;
    
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;

    Referrals referrals = dbsession.find<Referral>().where("ref_received BETWEEN ? AND ?").bind(startDate).bind(endDate);

    std::cerr << referrals.size() << std::endl;

    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i)
    {
        //instance new log entry object here

        ReferralLogEntry *ent = new ReferralLogEntry();

        Wt::WString ptName;
        Wt::WString mdName;
        Wt::WString ptInsur;
        Wt::WString stdType = "";
        Wt::WString none = "None";

        Wt::WDate recvDate;

        ptName = (*i)->refFirstName_ + " " + (*i)->refLastName_;
        mdName = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;
        ptInsur = (*i)->refInsuranceName_;
        recvDate = (*i)->refReceivedAt_;

        //add pt name, md name, insurance, status, ref received date info to ReferralLogEntry vector from Referral object
        ent->patientName = ptName;
        ent->physicianName = mdName;
        ent->patientInsurance = ptInsur;
        ent->referralReceivedDate = recvDate;
        ent->referralId = (*i)->referralId_;

        if((*i)->studyTypeName_ != "")
        {
            stdType = (*i)->studyTypeName_;
        }

        typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
        Studies studies = dbsession.find<SleepStudy>().where("ref_id = ?").bind((*i)->referralId_);
        std::cerr << studies.size() << std::endl;
        if (studies.size() == 1)
        {
            for (Studies::const_iterator s = studies.begin(); s != studies.end(); ++s)
            {

                Wt::WDate stdDate;

                if ((*i)->studyTypeName_ == "" && (*s)->studyTypeName_ != "") {
                    Wt::log("info") << "referral does not have studytype but study does";
                    stdType = (*s)->studyTypeName_;
                }

                stdDate = (*s)->studyDate_;

                // //add studytype, studydate info to ReferralLogEntry vector from SleepStudy object
                // if ((*i)->studyTypeName_ == "" && stdType != "") {
                //     Wt::log("info") << "referral does not have studytype but study does";
                //     ent->sleepStudyType = stdType;
                // } else if ((*i)->studyTypeName_ != "") {
                //     Wt::log("info") << "referral has study type, using it.";
                //     ent->sleepStudyType = (*i)->studyTypeName_;
                // } else {
                //     Wt::log("info") << "Neither study nor referral have study type, should be set to: " << none.toUTF8();
                //     ent->sleepStudyType = none;
                // }

                ent->studyDate = stdDate;

            }

        }
        
        if (stdType != "")
        {
            Wt::log("info") << "Study Type is here! setting ent to: " << stdType.toUTF8();
            ent->sleepStudyType = stdType;
        } else {
            Wt::log("info") << "Study Type is EMPTY! setting ent to: " << none.toUTF8();  
            ent->sleepStudyType = none;
        }
        entries.push_back(*ent);

        if ((*i)->refStatus_ == "Study Scheduled") {
            scheduled.push_back(*ent);
        } else {
            unscheduled.push_back(*ent);
        }
    }
    Wt::log("info") << "Query for Referral Count by Date Report";

    //query for Referral Count by Date report

    Wt::WDate currentQueryDate = startDate;
    Wt::log("info") << "set currentQueryDate to startDate";

    std::vector<ReferralCountItem> refcounts;
    Wt::log("info") << "vector for ReferralCountItem";

    Wt::log("info") << "Starting while loop";
    while (currentQueryDate <= endDate)
    {
        ReferralCountItem *rce = new ReferralCountItem();
        Wt::log("info") << "Created new ReferralCountItem";
        int refcountsize;

        typedef dbo::collection< dbo::ptr<Referral> > CountReferrals;
        Wt::log("info") << "typedef for CountReferrals<Referral>";
        CountReferrals countreferrals = dbsession.find<Referral>().where("ref_received = ?").bind(currentQueryDate);
        Wt::log("info") << "countreferrals db find where ref_received = currentQueryDate";
        std::cerr << countreferrals.size() << std::endl;
        Wt::log("info") << "Size of countreferrals";
        refcountsize = countreferrals.size();
        Wt::log("info") << "set refcountsize to countreferrals.size()";


        rce->referralReceivedDate = currentQueryDate;
        Wt::log("info") << "set rce->referralReceivedDate to currentQueryDate";
        rce->numberReferrals = refcountsize;
        Wt::log("info") << "set rce->numberReferrals to refcountsize";

        refcounts.push_back(*rce);
        Wt::log("info") << "push back rce to refcounts vector";
        
        if (currentQueryDate < endDate)
        {
            currentQueryDate = currentQueryDate.addDays(1);
            Wt::log("info") << "currentQueryDate: " << currentQueryDate.toString().toUTF8();
        } else {
            break;
        }

        Wt::log("info") << "reset currentQueryDate to itself, addDfays(1)";

    }

    int nummodelrows;
    nummodelrows = refcounts.size();

    Wt::log("info") << "set nummodelrows to refcounts size";

    Wt::WContainerWidget *chartcontainer = new Wt::WContainerWidget();

    Wt::WStandardItemModel *model = new Wt::WStandardItemModel(chartcontainer);

    // Configure the header.
    model->insertColumns(model->columnCount(), 2);
    model->setHeaderData(0, Wt::WString("Date"));
    model->setHeaderData(1, Wt::WString("Referrals"));
    Wt::log("info") << "configured model header";

    // Set data in the model.
    model->insertRows(model->rowCount(), nummodelrows);
    int crow = 0;

    for (std::vector<ReferralCountItem>::iterator ien = refcounts.begin(); ien != refcounts.end(); ++ien, ++crow)
    {
        model->setData(crow, 0, Wt::WString(ien->referralReceivedDate.toString("dd/MM/yy")));
        model->setData(crow, 1, ien->numberReferrals);
    }
    Wt::log("info") << "set data in the model";

    /*
     * Parses the first column as dates, to be able to use a date scale
     */
    for (int row = 0; row < model->rowCount(); ++row) {
        Wt::WString s = Wt::asString(model->data(row, 0));
        Wt::WDate date = Wt::WDate::fromString(s, "dd/MM/yy");
        model->setData(row, 0, date);
      }
    Wt::log("info") << "parse first column as dates";


    /*
     * Creates the scatter plot.
     */
    Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart(chartcontainer);
    chart->setBackground(Wt::WColor(220, 220, 220));
    chart->setModel(model);
    chart->setXSeriesColumn(0);
    chart->setLegendEnabled(true);
    chart->setType(Wt::Chart::ScatterPlot);
    chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
    chart->axis(Wt::Chart::YAxis).setMinimum(0);
    chart->axis(Wt::Chart::YAxis).setMaximum(20);

    Wt::log("info") << "set up chart";

    /*
     * Provide ample space for the title, the X and Y axis and the legend.
     */
    // chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
    // chart->setPlotAreaPadding(120, Wt::Right);

    /*
     * Add the second and the third column as line series.
     */

    Wt::Chart::WDataSeries s(1, Wt::Chart::LineSeries);
    s.setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
    chart->addSeries(s);

    chart->resize(800, 400);
    chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);

    int totalunscheduled = unscheduled.size();
    std::cout << "Total Unscheduled Entries: " << totalunscheduled;

    std::stringstream unscheduledstream;
    unscheduledstream << static_cast<int>(totalunscheduled);
    Wt::WString showunscheduled = unscheduledstream.str();

    Wt::WText *finalunscheduled = new Wt::WText("<h4>Unscheduled: " + showunscheduled + "</h4>");
    finalunscheduled->setStyleClass("final-total");
    
    int totalscheduled = scheduled.size();
    std::cout << "Total Scheduled Entries: " << totalscheduled;

    std::stringstream scheduledstream;
    scheduledstream << static_cast<int>(totalscheduled);
    Wt::WString showscheduled = scheduledstream.str();

    Wt::WText *finalscheduled = new Wt::WText("<h4>Scheduled: " + showscheduled + "</h4>");
    finalscheduled->setStyleClass("final-total");

    Wt::WContainerWidget *chartinside = new Wt::WContainerWidget();
    Wt::WHBoxLayout *charthbox = new Wt::WHBoxLayout();
    chartinside->setLayout(charthbox);
    Wt::WContainerWidget *counts = new Wt::WContainerWidget();

    counts->addWidget(finalscheduled);
    counts->addWidget(new Wt::WBreak());
    counts->addWidget(finalunscheduled);
    charthbox->addWidget(chart);
    charthbox->addWidget(counts);

    chartcontainer->addWidget(chartinside);

    Wt::log("info") << "set up chart dataseries";
    //display referrals reftable
    Wt::WTable *reftable = new Wt::WTable();

    reftable->setHeaderCount(1);
    reftable->elementAt(0, 0)->addWidget(new Wt::WText("Patient Name"));
    reftable->elementAt(0, 1)->addWidget(new Wt::WText("Physician Name"));
    reftable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Insurance"));
    reftable->elementAt(0, 3)->addWidget(new Wt::WText("Study Type"));
    reftable->elementAt(0, 4)->addWidget(new Wt::WText("Referral Received"));
    reftable->elementAt(0, 5)->addWidget(new Wt::WText("Study Date"));
    reftable->elementAt(0, 6)->addWidget(new Wt::WText("Duplicate"));

    int rrow = 1;
    for (std::vector<ReferralLogEntry>::iterator en = entries.begin(); en != entries.end(); ++en, ++rrow)
    {
            new Wt::WText(en->patientName, reftable->elementAt(rrow, 0)),
            new Wt::WText(en->physicianName, reftable->elementAt(rrow, 1)),
            new Wt::WText(en->patientInsurance, reftable->elementAt(rrow, 2)),
            new Wt::WText(en->sleepStudyType, reftable->elementAt(rrow, 3)),
            new Wt::WText(en->referralReceivedDate.toString("MM/dd/yyyy"), reftable->elementAt(rrow, 4)),
            new Wt::WText(en->studyDate.toString("MM/dd/yyyy"), reftable->elementAt(rrow, 5)),
            markDuplicate = new Wt::WPushButton("Mark Duplicate", reftable->elementAt(rrow, 6));
            markDuplicate->clicked().connect(boost::bind(&ReferralResultWidget::showDuplicate, this, en->referralId));

    }

    int totalentries = entries.size();
    std::cout << "Total Entries: " << totalentries;

    std::stringstream totalstream;
    totalstream << static_cast<int>(totalentries);
    Wt::WString showtotalout = totalstream.str();

    Wt::WText *finaltotal = new Wt::WText("<h4>Total Referrals Between "+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showtotalout + "</h4>");
    finaltotal->setStyleClass("final-total");

    reftable->addStyleClass("table form-inline");
    reftable->addStyleClass("table table-striped");
    reftable->addStyleClass("table table-hover");

    transaction.commit();

    container->addWidget(chartcontainer);
    container->addWidget(new Wt::WBreak());
    container->addWidget(finaltotal);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(reftable);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    addWidget(container);

}

void ReferralResultWidget::showReferralLog(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& status)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);


    //define logentries vector here
    std::vector<ReferralLogEntry> entries;
    std::vector<ReferralLogEntry> unscheduled;
    std::vector<ReferralLogEntry> scheduled;
    
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    
    Referrals referrals = dbsession.find<Referral>().where("ref_received BETWEEN ? AND ?").bind(startDate).bind(endDate).where("ref_status = ?").bind(status.toUTF8());

    std::cerr << referrals.size() << std::endl;

    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i)
    {
        //instance new log entry object here

        ReferralLogEntry *ent = new ReferralLogEntry();

        Wt::WString ptName;
        Wt::WString mdName;
        Wt::WString ptInsur;
        Wt::WString stdType = "";
        Wt::WString none = "None";

        Wt::WDate recvDate;

        ptName = (*i)->refFirstName_ + " " + (*i)->refLastName_;
        mdName = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;
        ptInsur = (*i)->refInsuranceName_;
        recvDate = (*i)->refReceivedAt_;

        //add pt name, md name, insurance, status, ref received date info to ReferralLogEntry vector from Referral object
        ent->patientName = ptName;
        ent->physicianName = mdName;
        ent->patientInsurance = ptInsur;
        ent->referralReceivedDate = recvDate;
        ent->referralId = (*i)->referralId_;

        if((*i)->studyTypeName_ != "")
        {
            stdType = (*i)->studyTypeName_;
        }

        typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
        Studies studies = dbsession.find<SleepStudy>().where("ref_id = ?").bind((*i)->referralId_);
        std::cerr << studies.size() << std::endl;
        if (studies.size() == 1)
        {
            for (Studies::const_iterator s = studies.begin(); s != studies.end(); ++s)
            {

                Wt::WDate stdDate;

                if ((*i)->studyTypeName_ == "" && (*s)->studyTypeName_ != "") {
                    Wt::log("info") << "referral does not have studytype but study does";
                    stdType = (*s)->studyTypeName_;
                }

                stdDate = (*s)->studyDate_;

                // //add studytype, studydate info to ReferralLogEntry vector from SleepStudy object
                // if ((*i)->studyTypeName_ == "" && stdType != "") {
                //     Wt::log("info") << "referral does not have studytype but study does";
                //     ent->sleepStudyType = stdType;
                // } else if ((*i)->studyTypeName_ != "") {
                //     Wt::log("info") << "referral has study type, using it.";
                //     ent->sleepStudyType = (*i)->studyTypeName_;
                // } else {
                //     Wt::log("info") << "Neither study nor referral have study type, should be set to: " << none.toUTF8();
                //     ent->sleepStudyType = none;
                // }

                ent->studyDate = stdDate;

            }

        }
        
        if (stdType != "")
        {
            Wt::log("info") << "Study Type is here! setting ent to: " << stdType.toUTF8();
            ent->sleepStudyType = stdType;
        } else {
            Wt::log("info") << "Study Type is EMPTY! setting ent to: " << none.toUTF8();  
            ent->sleepStudyType = none;
        }

        //push_back to logentries vector
        entries.push_back(*ent);
        if ((*i)->refStatus_ == "Study Scheduled") {
            scheduled.push_back(*ent);
        } else {
            unscheduled.push_back(*ent);
        }

    }
    Wt::log("info") << "Query for Referral Count by Date Report";

    //query for Referral Count by Date report

    Wt::WDate currentQueryDate = startDate;
    Wt::log("info") << "set currentQueryDate to startDate";

    std::vector<ReferralCountItem> refcounts;
    Wt::log("info") << "vector for ReferralCountItem";

    Wt::log("info") << "Starting while loop";
    while (currentQueryDate <= endDate)
    {
        ReferralCountItem *rce = new ReferralCountItem();
        Wt::log("info") << "Created new ReferralCountItem";
        int refcountsize;

        typedef dbo::collection< dbo::ptr<Referral> > CountReferrals;
        Wt::log("info") << "typedef for CountReferrals<Referral>";
        CountReferrals countreferrals = dbsession.find<Referral>().where("ref_received = ?").bind(currentQueryDate);
        Wt::log("info") << "countreferrals db find where ref_received = currentQueryDate";
        std::cerr << countreferrals.size() << std::endl;
        Wt::log("info") << "Size of countreferrals";
        refcountsize = countreferrals.size();
        Wt::log("info") << "set refcountsize to countreferrals.size()";


        rce->referralReceivedDate = currentQueryDate;
        Wt::log("info") << "set rce->referralReceivedDate to currentQueryDate";
        rce->numberReferrals = refcountsize;
        Wt::log("info") << "set rce->numberReferrals to refcountsize";

        refcounts.push_back(*rce);
        Wt::log("info") << "push back rce to refcounts vector";
        
        if (currentQueryDate < endDate)
        {
            currentQueryDate = currentQueryDate.addDays(1);
            Wt::log("info") << "currentQueryDate: " << currentQueryDate.toString().toUTF8();
        } else {
            break;
        }

        Wt::log("info") << "reset currentQueryDate to itself, addDfays(1)";

    }

    int nummodelrows;
    nummodelrows = refcounts.size();

    Wt::log("info") << "set nummodelrows to refcounts size";

    Wt::WContainerWidget *chartcontainer = new Wt::WContainerWidget();

    Wt::WStandardItemModel *model = new Wt::WStandardItemModel(chartcontainer);

    // Configure the header.
    model->insertColumns(model->columnCount(), 2);
    model->setHeaderData(0, Wt::WString("Date"));
    model->setHeaderData(1, Wt::WString("Referrals"));
    Wt::log("info") << "configured model header";

    // Set data in the model.
    model->insertRows(model->rowCount(), nummodelrows);
    int crow = 0;

    for (std::vector<ReferralCountItem>::iterator ien = refcounts.begin(); ien != refcounts.end(); ++ien, ++crow)
    {
        model->setData(crow, 0, Wt::WString(ien->referralReceivedDate.toString("dd/MM/yy")));
        model->setData(crow, 1, ien->numberReferrals);
    }
    Wt::log("info") << "set data in the model";

    /*
     * Parses the first column as dates, to be able to use a date scale
     */
    for (int row = 0; row < model->rowCount(); ++row) {
        Wt::WString s = Wt::asString(model->data(row, 0));
        Wt::WDate date = Wt::WDate::fromString(s, "dd/MM/yy");
        model->setData(row, 0, date);
      }
    Wt::log("info") << "parse first column as dates";


    /*
     * Creates the scatter plot.
     */
    Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart(chartcontainer);
    chart->setBackground(Wt::WColor(220, 220, 220));
    chart->setModel(model);
    chart->setXSeriesColumn(0);
    chart->setLegendEnabled(true);
    chart->setType(Wt::Chart::ScatterPlot);
    chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
    chart->axis(Wt::Chart::YAxis).setMinimum(0);
    chart->axis(Wt::Chart::YAxis).setMaximum(20);

    Wt::log("info") << "set up chart";

    /*
     * Provide ample space for the title, the X and Y axis and the legend.
     */
    // chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
    // chart->setPlotAreaPadding(120, Wt::Right);

    /*
     * Add the second and the third column as line series.
     */

    Wt::Chart::WDataSeries s(1, Wt::Chart::LineSeries);
    s.setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
    chart->addSeries(s);

    chart->resize(800, 400);
    chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);

    int totalunscheduled = unscheduled.size();
    std::cout << "Total Unscheduled Entries: " << totalunscheduled;

    std::stringstream unscheduledstream;
    unscheduledstream << static_cast<int>(totalunscheduled);
    Wt::WString showunscheduled = unscheduledstream.str();

    Wt::WText *finalunscheduled = new Wt::WText("<h4>Unscheduled: " + showunscheduled + "</h4>");
    finalunscheduled->setStyleClass("final-total");
    
    int totalscheduled = scheduled.size();
    std::cout << "Total Scheduled Entries: " << totalscheduled;

    std::stringstream scheduledstream;
    scheduledstream << static_cast<int>(totalscheduled);
    Wt::WString showscheduled = scheduledstream.str();

    Wt::WText *finalscheduled = new Wt::WText("<h4>Scheduled: " + showscheduled + "</h4>");
    finalscheduled->setStyleClass("final-total");

    Wt::WContainerWidget *chartinside = new Wt::WContainerWidget();
    Wt::WHBoxLayout *charthbox = new Wt::WHBoxLayout();
    chartinside->setLayout(charthbox);
    Wt::WContainerWidget *counts = new Wt::WContainerWidget();

    counts->addWidget(finalscheduled);
    counts->addWidget(new Wt::WBreak());
    counts->addWidget(finalunscheduled);
    charthbox->addWidget(chart);
    charthbox->addWidget(counts);

    chartcontainer->addWidget(chartinside);

    Wt::log("info") << "set up chart dataseries";
    //display referrals reftable
    Wt::WTable *reftable = new Wt::WTable();

    reftable->setHeaderCount(1);
    reftable->elementAt(0, 0)->addWidget(new Wt::WText("Patient Name"));
    reftable->elementAt(0, 1)->addWidget(new Wt::WText("Physician Name"));
    reftable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Insurance"));
    reftable->elementAt(0, 3)->addWidget(new Wt::WText("Study Type"));
    reftable->elementAt(0, 4)->addWidget(new Wt::WText("Referral Received"));
    reftable->elementAt(0, 5)->addWidget(new Wt::WText("Study Date"));
    reftable->elementAt(0, 6)->addWidget(new Wt::WText("Duplicate"));

    int rrow = 1;
    for (std::vector<ReferralLogEntry>::iterator en = entries.begin(); en != entries.end(); ++en, ++rrow)
    {
            new Wt::WText(en->patientName, reftable->elementAt(rrow, 0)),
            new Wt::WText(en->physicianName, reftable->elementAt(rrow, 1)),
            new Wt::WText(en->patientInsurance, reftable->elementAt(rrow, 2)),
            new Wt::WText(en->sleepStudyType, reftable->elementAt(rrow, 3)),
            new Wt::WText(en->referralReceivedDate.toString("MM/dd/yyyy"), reftable->elementAt(rrow, 4)),
            new Wt::WText(en->studyDate.toString("MM/dd/yyyy"), reftable->elementAt(rrow, 5)),
            markDuplicate = new Wt::WPushButton("Mark Duplicate", reftable->elementAt(rrow, 6));
            markDuplicate->clicked().connect(boost::bind(&ReferralResultWidget::showDuplicate, this, en->referralId));

    }

    int totalentries = entries.size();
    std::cout << "Total Entries: " << totalentries;

    std::stringstream totalstream;
    totalstream << static_cast<int>(totalentries);
    Wt::WString showtotalout = totalstream.str();

    Wt::WText *finaltotal = new Wt::WText("<h4>Total Referrals Between "+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showtotalout + "</h4>");
    finaltotal->setStyleClass("final-total");

    reftable->addStyleClass("table form-inline");
    reftable->addStyleClass("table table-striped");
    reftable->addStyleClass("table table-hover");

    transaction.commit();

    container->addWidget(chartcontainer);
    container->addWidget(new Wt::WBreak());
    container->addWidget(finaltotal);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(reftable);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    addWidget(container);

}

void ReferralResultWidget::showDuplicate(Wt::WString ref)
{
    dbo::Transaction sttrans(dbsession);
    typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
    Studies studies = dbsession.find<SleepStudy>().where("ref_id = ?").bind(ref.toUTF8());
    std::cerr << studies.size() << std::endl;
    sttrans.commit();
    
    if (studies.size() != 0)
    {
        Wt::WMessageBox *messageBox = new Wt::WMessageBox
        ("Cannot Remove Duplicate",
         "<p>There is a Sleep Study Associtated with this referral</p>"
         "<p>Select another duplicate referral.</p>",
         Wt::Information, Wt::Ok);

        messageBox->setModal(false);

        messageBox->buttonClicked().connect(std::bind([=] () {
            delete messageBox;
        }));

        messageBox->show();
    } else {

       Wt::WMessageBox *messageBox = new Wt::WMessageBox
        ("Status",
         "<p>Do you want to remove this referral permenantly?</p>",
         Wt::Information, Wt::Yes | Wt::No);

        messageBox->setModal(false);

        messageBox->buttonClicked().connect(std::bind([=] () {
            if (messageBox->buttonResult() == Wt::Yes)
            {
                //find referral by id and delete it
                dbo::Transaction rmtrans(dbsession);
                dbo::ptr<Referral> referral = dbsession.find<Referral>().where("ref_id = ?").bind(ref.toUTF8());
                referral.remove();
                rmtrans.commit();
            }

            delete messageBox;
        }));

        messageBox->show();
    }
}