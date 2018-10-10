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

#include "InsuranceResultWidget.h"
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
			:	ptFirstName(aPtFirstName),
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

InsuranceResultWidget::InsuranceResultWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
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
void InsuranceResultWidget::showInsuranceLog(Wt::WDate& startDate, Wt::WDate& endDate)
{		
	Wt::WContainerWidget *container = new Wt::WContainerWidget();
	
	dbo::Transaction transaction(dbsession);
	//first do the insurance list
	Wt::log("notice") << "referrals is being queried for with start and end date";
	std::vector<ReferralInsuranceEntry> ientries;
	
	typedef dbo::collection< dbo::ptr<Insurance> > Insurances;
	Insurances insurances = dbsession.find<Insurance>();
	std::cerr << insurances.size() << std::endl;
	
	int numinsurances = insurances.size();
	int totalstudy = 0;
	for (Insurances::const_iterator i = insurances.begin(); i != insurances.end(); ++i)
	{
		//instance new log entry object here

		ReferralInsuranceEntry *ient = new ReferralInsuranceEntry();

		Wt::WString insurancename;
		int numreferrals;
		
		insurancename = (*i)->insuranceName_;

		//add pt name, md name, insurance, status, ref received date info to ReferralInsuranceEntry vector from Referral object
		ient->insuranceName = insurancename;

		typedef dbo::collection< dbo::ptr<Referral> > Referrals;
		Referrals referrals = dbsession.find<Referral>().where("ref_received BETWEEN ? AND ?").bind(startDate).bind(endDate).where("insurance_id = ?").bind((*i)->insuranceId_);
		std::cerr << referrals.size() << std::endl;

		//push_back to logentries vector
		if(referrals.size() > 0)
		{
			numreferrals = referrals.size();
			ient->numReferrals = numreferrals;
			int numscheduled = 0;
			int numcomplete = 0;
			//get iterate over each referral and find the studies for that referral, filter only study scheduled, study rescheduled, or study complete.
			for (Referrals::const_iterator r = referrals.begin(); r != referrals.end(); ++r)
			{
				//instance new log entry object here

				typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
				Studies studies = dbsession.find<SleepStudy>().where("ref_id = ?").bind((*r)->referralId_).where("std_status != ?").bind("Study Archived");
				std::cerr << studies.size() << std::endl;
				if (studies.size() == 1)
				{
				    for (Studies::const_iterator s = studies.begin(); s != studies.end(); ++s)
				    {

						if ((*s)->studyStatus_ == "Study Complete")
						{					
							numcomplete = numcomplete + 1;
						} else {
							numscheduled = numscheduled + 1;
						}

				    }

				}

			}
			ient->numRefScheduled = numscheduled + numcomplete;
			ient->numRefComplete = numcomplete;

			log("info") << "referrals.size is greater than 0, before push_back";
			ientries.push_back(*ient);
			totalstudy = totalstudy + numreferrals;
			log("info") << "referrals.size is greater than 0, after push_back";

		} else {
			
			log("info") << "referrals.size is 0";
		}

	}
	

	//now setup the data model for the chart
	Wt::WContainerWidget *chartcontainer = new Wt::WContainerWidget();

	Wt::WStandardItemModel *model = new Wt::WStandardItemModel(chartcontainer);
	model->setItemPrototype(new NumericItem());

	// Configure the header.
	model->insertColumns(model->columnCount(), 2);
	model->setHeaderData(0, Wt::WString("Insurance"));
	model->setHeaderData(1, Wt::WString("Referrals"));

	// Set data in the model.
	model->insertRows(model->rowCount(), numinsurances);
	int row = 0;

	for (std::vector<ReferralInsuranceEntry>::iterator ien = ientries.begin(); ien != ientries.end(); ++ien, ++row)
	{
		model->setData(row, 0, Wt::WString(ien->insuranceName));
		model->setData(row, 1, ien->numReferrals);
	}
	
	
	//create the pie chart

	Wt::Chart::WPieChart *chart = new Wt::Chart::WPieChart();
	chart->setModel(model);       // Set the model.
	chart->setLabelsColumn(0);    // Set the column that holds the labels.
	chart->setDataColumn(1);      // Set the column that holds the data.

	// Configure location and type of labels.
	chart->setDisplayLabels(Wt::Chart::Outside |
	                        Wt::Chart::TextLabel |
	                        Wt::Chart::TextPercentage);

	
	/*
	// Enable a 3D and shadow effect.
	chart->setPerspectiveEnabled(true, 0.2);
	chart->setShadowEnabled(true);
	*/

	//chart->setExplode(0, 0.3);  // Explode the first item.
	chart->resize(700, 400);    // WPaintedWidget must be given an explicit size.
	chart->setMargin(30, Wt::Top | Wt::Bottom); // Add margin vertically.
	chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right); // Center horizontally.

	
	//container for tables
	Wt::WContainerWidget *tablecontainer = new Wt::WContainerWidget();
	//hbox of vboxes for total table and table that ajdusts chart
	Wt::WHBoxLayout *tablehbox = new Wt::WHBoxLayout();
	Wt::WVBoxLayout *left = new Wt::WVBoxLayout();
	Wt::WVBoxLayout *right = new Wt::WVBoxLayout();

	tablecontainer->setLayout(tablehbox);
	tablehbox->addLayout(left);
	tablehbox->addLayout(right);

	//display the insurance list table
	Wt::WTableView	*insurancetable = new Wt::WTableView();

	insurancetable->setMargin(10, Wt::Top | Wt::Bottom);
	insurancetable->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
	insurancetable->setSortingEnabled(true);
	insurancetable->setModel(model);
	insurancetable->setColumnWidth(1, 100);
	insurancetable->setRowHeight(28);
	insurancetable->setHeaderHeight(28);
	insurancetable->setWidth(150 + 100 + 14 + 2);

	//display referrals table
	Wt::WTable *table = new Wt::WTable();
	
	table->setHeaderCount(1);
	table->elementAt(0, 0)->addWidget(new Wt::WText("Insurance Name"));
	table->elementAt(0, 1)->addWidget(new Wt::WText("Total Referrals"));
	table->elementAt(0, 2)->addWidget(new Wt::WText("Study Scheduled"));
	table->elementAt(0, 3)->addWidget(new Wt::WText("Study Complete"));

	int rrow = 1;
	for (std::vector<ReferralInsuranceEntry>::iterator en = ientries.begin(); en != ientries.end(); ++en, ++rrow)
	{
			int shownumref = en->numReferrals;
			
			std::stringstream numrefstream;
			numrefstream << static_cast<int>(shownumref);
			Wt::WString insref = numrefstream.str();

			int shownumsch = en->numRefScheduled;
			
			std::stringstream numschstream;
			numschstream << static_cast<int>(shownumsch);
			Wt::WString inssch = numschstream.str();

			int shownumcomp = en->numRefComplete;
			
			std::stringstream numcompstream;
			numcompstream << static_cast<int>(shownumcomp);
			Wt::WString inscomp = numcompstream.str();

			new Wt::WText(en->insuranceName, table->elementAt(rrow, 0)),
			new Wt::WText(insref, table->elementAt(rrow, 1));
			new Wt::WText(inssch, table->elementAt(rrow, 2));
			new Wt::WText(inscomp, table->elementAt(rrow, 3));
	}

	int totalentries = totalstudy;
	std::cout << "Total Entries: " << totalentries;
	
	std::stringstream totalstream;
	totalstream << static_cast<int>(totalentries);
	Wt::WString showtotalout = totalstream.str();

	Wt::WText *finaltotal = new Wt::WText("<h4>Total Referrals Between "+ startDate.toString("MM/dd/yyyy") +" and " + endDate.toString("MM/dd/yyyy") + " : " + showtotalout + "</h4>");
	finaltotal->setStyleClass("final-total");

	table->addStyleClass("table form-inline");
	table->addStyleClass("table table-striped");
	table->addStyleClass("table table-hover");
/*
	left->addWidget(new Wt::WText("<h4> Click the Cell Corner to Sort Chart Data</h4>"));
	left->addWidget(new Wt::WBreak());
	left->addWidget(insurancetable);
*/	
	right->addWidget(finaltotal);
	right->addWidget(new Wt::WBreak());
	right->addWidget(table);

	chartcontainer->addWidget(chart);
	chartcontainer->addWidget(new Wt::WBreak());
	chartcontainer->addWidget(tablecontainer);
	container->addWidget(new Wt::WText("<h4>Referrals Breakdown by Patient Insurance</h4>"));
	container->addWidget(new Wt::WBreak());
	container->addWidget(chartcontainer);
	container->addWidget(new Wt::WBreak());	

	addWidget(container);

}