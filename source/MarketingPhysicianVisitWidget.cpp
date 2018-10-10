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
#include <Wt/WTimeEdit>

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
#include <Wt/WTimer>

//other header files
#include "MarketingPhysicianVisitWidget.h"
#include "Referral.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"
#include "Insurance.h"
#include "InsuranceItem.h"
#include "StudyType.h"
#include "StudyTypeItem.h"
#include "PhysicianVisitResultWidget.h"
#include "PhysicianVisit.h"
#include "PhysicianIssue.h"


using namespace Wt;
namespace dbo = Wt::Dbo;

MarketingPhysicianVisitWidget::MarketingPhysicianVisitWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	pg_(conninfo),
	conninfo_(conninfo),
	session_(session)
{
	dbsession.setConnection(pg_);

	pg_.setProperty("show-queries", "true");

	dbsession.mapClass<CompositeEntity>("composite");
	dbsession.mapClass<Practice>("practice");	
	dbsession.mapClass<PhysicianVisit>("physicianvisit");
	dbsession.mapClass<PhysicianIssue>("physicianissue");
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


void MarketingPhysicianVisitWidget::visitList()
{

	refreshTimer = new Wt::WTimer();
	refreshTimer->setInterval(5000);
	refreshTimer->timeout().connect(std::bind([=] () {
		log("info") << "Timer Refresh Signal Sent";
		MarketingPhysicianVisitWidget::search("");
	}));

	Wt::WContainerWidget *container = new Wt::WContainerWidget();
	
	container->addWidget(new Wt::WBreak());
	container->addWidget(new Wt::WBreak());
	
	Wt::WLineEdit *edit = new Wt::WLineEdit();
	edit->setStyleClass("search-box");
	edit->setPlaceholderText("Search for a Visit by  MD name, PCC name or reason for visit keyword");
	
	container->addWidget(edit);


	Wt::WText *out = new Wt::WText("", container);
	out->addStyleClass("help-block");
	Wt::WText *curout = new Wt::WText();
	Wt::WString searchPractice;
	
	resultStack_ = new Wt::WStackedWidget(container);

	edit->textInput().connect(std::bind([=] () {
	
	   MarketingPhysicianVisitWidget::search(edit->text().toUTF8());
	   
	   if (edit->text() == "")
	   {
			MarketingPhysicianVisitWidget::startRefreshTimer();
	   } else {
	   		MarketingPhysicianVisitWidget::stopRefreshTimer();
	   }

	}));

	addWidget(container);

	MarketingPhysicianVisitWidget::search(Wt::WString(""));
	MarketingPhysicianVisitWidget::startRefreshTimer();

}

void MarketingPhysicianVisitWidget::preSearch(Wt::WString searchString)
{
	MarketingPhysicianVisitWidget::search(Wt::WString(searchString));
	MarketingPhysicianVisitWidget::startRefreshTimer();
}
void MarketingPhysicianVisitWidget::stopRefreshTimer()
{
	if (refreshTimer->isActive())
	{
		log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
		refreshTimer->stop();		
	} else {
		log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
	}

}
void MarketingPhysicianVisitWidget::startRefreshTimer()
{
	if (refreshTimer->isActive() == false)
	{
		log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
		refreshTimer->start();
	} else {
		log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

	}

}
void MarketingPhysicianVisitWidget::search(Wt::WString searchString)
{
	int limit = 100;
	int offset = 0;

	resultStack_->clear();

	prContainer = new PhysicianVisitResultWidget(conninfo_, session_, resultStack_);
	prContainer->showPhysicianVisitLog(searchString, limit, offset);
	
	resultStack_->setCurrentWidget(prContainer);

    prContainer->stopTimer().connect(std::bind([=] () {
		log("info") << "Caught stopTimer Signal";
		MarketingPhysicianVisitWidget::stopRefreshTimer();
	}));

    prContainer->done().connect(std::bind([=] () {
		log("info") << "Caught done Signal";
		MarketingPhysicianVisitWidget::preSearch(searchString);
	}));
}

WString MarketingPhysicianVisitWidget::setIssueId()
{
	return uuid();
}

void MarketingPhysicianVisitWidget::newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid)
{
	
	staticIssue_ = new Wt::WText();
	staticIssue_->setText(setIssueId());
	
	dbo::Transaction transaction(dbsession);

	dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());
	
	WString pname = practice->refPracticeName_;
	WString pzip = practice->refPracticeZip_;
	WString pdrfirst = practice->refPracticeDrFirst_;
	WString pdrlast = practice->refPracticeDrLast_;
	WString pnpi = practice->refPracticeNpi_;
	WString pemail = practice->refPracticeEmail_;
	WString pspec = practice->refSpecialty_;

	transaction.commit();

	Wt::WDialog *newissuedialog = new Wt::WDialog("Add a New Visit Log Entry");
	newissuedialog->setMinimumSize(1000, 700);
	newissuedialog->setMaximumSize(1000, 700);
	
	Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newissuedialog->contents());

	Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
	newprcontainer->setLayout(pthbox);

	Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
	Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

	pthbox->addLayout(leftbox);
	pthbox->addLayout(rightbox);

	Wt::WPushButton *addissue = new Wt::WPushButton("Save", newissuedialog->footer());
	addissue->setDefault(true);

	Wt::WText *issueTitleLbl = new Wt::WText("Issue Title (Short Description)");
	issueTitleLbl->setStyleClass("label-left-box");
	issueTitle_ = new Wt::WLineEdit(this);
	issueTitle_->setStyleClass("left-box");
	leftbox->addWidget(issueTitleLbl);
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(issueTitle_);
	leftbox->addWidget(new Wt::WBreak());

	Wt::WText *issueDescriptionLbl = new Wt::WText("Issue Description (Details go here)");
	issueDescriptionLbl->setStyleClass("label-left-box");
	issueDescription_ = new Wt::WLineEdit(this);
	issueDescription_->setStyleClass("left-box");
	leftbox->addWidget(issueDescriptionLbl);
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(issueDescription_);
	leftbox->addWidget(new Wt::WBreak());
	
	Wt::WHBoxLayout *calTimeLayout = new Wt::WHBoxLayout();
	leftbox->addLayout(calTimeLayout);

	Wt::WVBoxLayout *calTimeLeft = new Wt::WVBoxLayout();
	Wt::WVBoxLayout *calTimeRight = new Wt::WVBoxLayout();

	calTimeLayout->addLayout(calTimeLeft);
	calTimeLayout->addLayout(calTimeRight);

	//add calendar and time selection here
	//date first
	Wt::WCalendar *c1 = new Wt::WCalendar();
	c1->setSelectionMode(Wt::ExtendedSelection);

	Wt::WText* calout = new Wt::WText();
	calout->addStyleClass("help-block");

	Wt::WDate *dbVisitDateD = new Wt::WDate();
	Wt::WText *c1SelectedDate = new Wt::WText();

	c1->selectionChanged().connect(std::bind([=]() {
		Wt::WString selected;
		std::set<Wt::WDate> selection = c1->selection();

		for (std::set<Wt::WDate>::const_iterator it = selection.begin();
			it != selection.end(); ++it) {
			if (!selected.empty())
				selected += ", ";

			const Wt::WDate& d = *it;
			selected = d.toString("MM/dd/yyyy");
			c1SelectedDate->setText(d.toString("MM/dd/yyyy"));
			
		}

		calout->setText(Wt::WString::fromUTF8
			("{1}")
			.arg(selected));
		Wt::log("info") << "calout inside: " << calout->text().toUTF8();	
		
	}));

	Wt::log("info") << "calout outside: " << calout->text().toUTF8();	
	Wt::log("info") << "c1SelectedDate: " << c1SelectedDate->text().toUTF8();

	//time selection
	calTimeLeft->addWidget(new Wt::WText("Select Date Issue was Reported"));
	calTimeLeft->addWidget(new Wt::WBreak());
	calTimeLeft->addWidget(c1);
	calTimeLeft->addWidget(calout);

	addissue->clicked().connect(std::bind([=]() {

		Wt::WTime issuestart = Wt::WTime::currentServerTime();
		Wt::WDate issuedate = Wt::WDate().fromString(c1SelectedDate->text().toUTF8(), "MM/dd/yyyy");

		Wt::WDateTime issuedatetime = Wt::WDateTime(issuedate, issuestart);
		
		Wt::WString staffEmail = session_->strUserEmail();
		
		Wt::WString pccname = staffEmail;
		Wt::WString pracname = pdrfirst + " " + pdrlast;
		Wt::WString issuedesc = issueDescription_->text(); 
		Wt::WString issuetitle = issueTitle_->text();
		Wt::WString issuestatus = "Unresolved";
		Wt::WString visitid;
		if (vid != "")
		{
			visitid = vid;
		}
		dbo::Transaction ptrans(dbsession);

		dbo::ptr<PhysicianIssue> pi = dbsession.add(new PhysicianIssue());

		pi.modify()->issueReported_ = issuedatetime;
		pi.modify()->pccName_ = pccname;

		pi.modify()->mdName_ = pracname;
		pi.modify()->issueTitle_ = issuetitle;
		pi.modify()->issueDescription_ = issuedesc;
		pi.modify()->issueStatus_ = issuestatus;

		pi.modify()->issueId_ = staticIssue_->text().toUTF8();
		pi.modify()->practiceId_ = prac;
		pi.modify()->visitId_ = visitid;
		
		ptrans.commit();
		
		newissuedialog->accept();


	}));
	
	Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newissuedialog->footer());
	newissuedialog->rejectWhenEscapePressed();

	cancel->clicked().connect(newissuedialog, &Wt::WDialog::reject);
	newprcontainer->show();	
	newissuedialog->show();
}