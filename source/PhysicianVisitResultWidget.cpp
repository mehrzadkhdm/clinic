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

#include "PhysicianVisitResultWidget.h"
#include "Practice.h"
#include "Comment.h"
#include "PhysicianVisit.h"
#include "PhysicianIssue.h"
#include "UserSession.h"


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

PhysicianVisitResultWidget::PhysicianVisitResultWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session),
	conninfo_(conninfo),
	pg_(conninfo)
{
	dbsession.setConnection(pg_);


	pg_.setProperty("show-queries", "true");
	
	dbsession.mapClass<CompositeEntity>("composite");
	dbsession.mapClass<PhysicianVisit>("physicianvisit");
	dbsession.mapClass<PhysicianIssue>("physicianissue");
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

WString PhysicianVisitResultWidget::setCommentId()
{
	return uuid();
}


void PhysicianVisitResultWidget::showPhysicianVisitLog(Wt::WString& curstr, int limit, int offset)
{		
	Wt::WContainerWidget *outside = new Wt::WContainerWidget();	
	Wt::WContainerWidget *container = new Wt::WContainerWidget();
	
	std::string strSearchPrac = curstr.toUTF8();
    std::string searchPrac = '%' + strSearchPrac +'%';

    dbo::Transaction transaction(dbsession);
    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("prac_drfirst ILIKE ? OR prac_drlast ILIKE ?").bind(searchPrac).bind(searchPrac).where("is_group != ?").bind("Yes");
    std::cerr << practices.size() << std::endl;
    
    
    int prow = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++prow)
    {

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

            if ((*i)->pccEmail_ == "staff@statsleep.com") {
                whoadded->setText("Added By: Mayte Lopez");
            } else if ((*i)->pccEmail_ == "anunez@statsleep.com") {
                whoadded->setText("Added By: Adriana Nunez");
            } else if ((*i)->pccEmail_ == "jolene@statsleep.com") {
                whoadded->setText("Added By: Jolene Antolin");
            } else {
                whoadded->setText("Added By: "+(*i)->pccEmail_);
            }
            container->addWidget(dateadd);
            container->addWidget(new Wt::WBreak());
            container->addWidget(whoadded);
            container->addWidget(new Wt::WBreak());
        }
	    dbo::Transaction vtrans(dbsession);
        typedef dbo::collection< dbo::ptr<PhysicianVisit> > Visits;
        Visits visits = dbsession.find<PhysicianVisit>().where("prac_id = ?").bind((*i)->practiceId_)	.orderBy("visit_start desc");
        std::cerr << visits.size() << std::endl;
        
        Wt::WTable *table = new Wt::WTable();
        table->setHeaderCount(1);

        table->elementAt(0, 0)->addWidget(new Wt::WText("PCC"));
        table->elementAt(0, 1)->addWidget(new Wt::WText("Visit Start"));
        table->elementAt(0, 2)->addWidget(new Wt::WText("Visit End"));
        table->elementAt(0, 3)->addWidget(new Wt::WText("Reason"));
        table->elementAt(0, 4)->addWidget(new Wt::WText("Outcome"));
        table->elementAt(0, 5)->addWidget(new Wt::WText("Status"));
        table->elementAt(0, 6)->addWidget(new Wt::WText("Details"));


        int row = 1;
        for (Visits::const_iterator v = visits.begin(); v != visits.end(); ++v, ++row)
        {

            Wt::WString numIssues;
            //get number of actions taken
            dbo::Transaction ctrans(dbsession);

            typedef dbo::collection< dbo::ptr<PhysicianIssue> > Issues;
            Issues issues = dbsession.find<PhysicianIssue>().where("visit_id = ?").bind((*v)->visitId_);
            std::cerr << issues.size() << std::endl;
            
            std::stringstream actionsstream;
            actionsstream << static_cast<int>(issues.size());
            numIssues = actionsstream.str();
            
            ctrans.commit();

            int duration = (*v)->visitStartTime_.secsTo((*v)->visitEndTime_)/60;
            
            std::stringstream durstream;
            durstream << static_cast<int>(duration);
            Wt::WString visitDuration = Wt::WString(durstream.str() + " Min");
            
            Wt::WDateTime fixstart = (*v)->visitStartTime_;
            Wt::WDateTime fixend = (*v)->visitEndTime_;
            
            Wt::WDateTime adjuststart = fixstart.addSecs(-25200);
            Wt::WDateTime adjustend = fixend.addSecs(-25200);
            
            WString finalstart = adjuststart.toString("MM/dd/yyyy hh:mm a");
            WString finalend = adjustend.toString("MM/dd/yyyy hh:mm a");

            new Wt::WText((*v)->pccName_, table->elementAt(row, 0)),
            new Wt::WText(finalstart, table->elementAt(row, 1)),
            new Wt::WText(finalend, table->elementAt(row, 2)),
            new Wt::WText((*v)->reasonForVisit_, table->elementAt(row, 3)),
            new Wt::WText((*v)->outcomeOfVisit_, table->elementAt(row, 4)),
            new Wt::WText((*v)->visitIssues_, table->elementAt(row, 5)),            
            btnSelectPhysicianVisit = new Wt::WPushButton("Details", table->elementAt(row, 6)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnSelectPhysicianVisit->clicked().connect(boost::bind(&PhysicianVisitResultWidget::showPhysicianVisitDialog, this, (*v)->visitId_));
        }

        table->addStyleClass("table form-inline");
        table->addStyleClass("table table-striped");
        table->addStyleClass("table table-hover");
        
        container->addWidget(table);
        container->addWidget(new Wt::WBreak());
        container->addWidget(new Wt::WBreak());

    }

    Wt::WContainerWidget *recentcontainer = new Wt::WContainerWidget();
	dbo::Transaction rectransaction(dbsession);
	
	typedef dbo::collection< dbo::ptr<PhysicianVisit> > RecentVisits;
	RecentVisits rvisits = dbsession.find<PhysicianVisit>().orderBy("visit_start desc");
	std::cerr << rvisits.size() << std::endl;
	
	Wt::WTable *recenttable = new Wt::WTable();
	recenttable->setHeaderCount(1);

	recenttable->elementAt(0, 0)->addWidget(new Wt::WText("MD Name"));
	recenttable->elementAt(0, 1)->addWidget(new Wt::WText("PCC"));
	recenttable->elementAt(0, 2)->addWidget(new Wt::WText("Visit Start"));
	recenttable->elementAt(0, 3)->addWidget(new Wt::WText("Visit End"));
	recenttable->elementAt(0, 4)->addWidget(new Wt::WText("Reason"));
	recenttable->elementAt(0, 5)->addWidget(new Wt::WText("Outcome"));
	recenttable->elementAt(0, 6)->addWidget(new Wt::WText("Status"));
	recenttable->elementAt(0, 7)->addWidget(new Wt::WText("Details"));


	int recrow = 1;
	for (RecentVisits::const_iterator rv = rvisits.begin(); rv != rvisits.end(); ++rv, ++recrow)
	{

		Wt::WString numIssues;
		//get number of actions taken
		dbo::Transaction ctrans(dbsession);

		typedef dbo::collection< dbo::ptr<PhysicianIssue> > Issues;
		Issues issues = dbsession.find<PhysicianIssue>().where("visit_id = ?").bind((*rv)->visitId_);
		std::cerr << issues.size() << std::endl;
		
		std::stringstream actionsstream;
		actionsstream << static_cast<int>(issues.size());
		numIssues = actionsstream.str();
		
		ctrans.commit();

		int duration = (*rv)->visitStartTime_.secsTo((*rv)->visitEndTime_)/60;
	    
	    std::stringstream durstream;
	    durstream << static_cast<int>(duration);
	    Wt::WString visitDuration = Wt::WString(durstream.str() + " Min");
        
        Wt::WDateTime fixstart = (*rv)->visitStartTime_;
        Wt::WDateTime fixend = (*rv)->visitEndTime_;
        
        Wt::WDateTime adjuststart = fixstart.addSecs(-25200);
        Wt::WDateTime adjustend = fixend.addSecs(-25200);
        
        WString finalstart = adjuststart.toString("MM/dd/yyyy hh:mm a");
        WString finalend = adjustend.toString("MM/dd/yyyy hh:mm a");

		new Wt::WText((*rv)->practiceName_, recenttable->elementAt(recrow, 0)),
		new Wt::WText((*rv)->pccName_, recenttable->elementAt(recrow, 1)),
		new Wt::WText(finalstart, recenttable->elementAt(recrow, 2)),
		new Wt::WText(finalend, recenttable->elementAt(recrow, 3)),
		new Wt::WText((*rv)->reasonForVisit_, recenttable->elementAt(recrow, 4)),
		new Wt::WText((*rv)->outcomeOfVisit_, recenttable->elementAt(recrow, 5)),
		new Wt::WText((*rv)->visitIssues_, recenttable->elementAt(recrow, 6)),			
		btnSelectPhysicianVisit = new Wt::WPushButton("Details", recenttable->elementAt(recrow, 7)),
		//click 'PDF' button on any recenttable recrow, connect downloadReferral with that referral uuid
		btnSelectPhysicianVisit->clicked().connect(boost::bind(&PhysicianVisitResultWidget::showPhysicianVisitDialog, this, (*rv)->visitId_));
	}

	transaction.commit();

	recenttable->addStyleClass("table form-inline");
	recenttable->addStyleClass("table table-striped");
	recenttable->addStyleClass("table table-hover");

	recentcontainer->addWidget(recenttable);

 	Wt::WTabWidget *tabW = new Wt::WTabWidget(outside);
    tabW->addTab(container,
             "Search by MD", Wt::WTabWidget::PreLoading);
    tabW->addTab(recentcontainer,
             "Sort by Most Recent", Wt::WTabWidget::PreLoading);
    
    tabW->setStyleClass("tabwidget");
    
    tabW->currentChanged().connect(std::bind([=] () {
		stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
	}));
    
    addWidget(outside);

}


void PhysicianVisitResultWidget::showPhysicianIssueDialog(Wt::WString isid)
{
	stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
	
	staticComment_ = new Wt::WText();
	staticComment_->setText(setCommentId());
	
	dbo::Transaction transaction(dbsession);

	dbo::ptr<PhysicianIssue> issue = dbsession.find<PhysicianIssue>().where("issue_id = ?").bind(isid.toUTF8());
	
	Wt::WDateTime issuereported = issue->issueReported_;
	Wt::WDateTime issueresolved = issue->issueResolved_;
	
	Wt::WString pccname = issue->pccName_;
	Wt::WString pracname = issue->mdName_;
	Wt::WString issuetitle = issue->issueTitle_; 
	Wt::WString issuedescription = issue->issueDescription_;
	Wt::WString issuestatus = issue->issueStatus_;
	Wt::WString numActionsTaken;
	Wt::WString prac = issue->practiceId_;

	transaction.commit();
	
	dbo::Transaction ctrans(dbsession);

	typedef dbo::collection< dbo::ptr<Comment> > Comments;
	Comments comments = dbsession.find<Comment>().where("pt_id = ?").bind(isid);
	std::cerr << comments.size() << std::endl;
	
	std::stringstream actionsstream;
	actionsstream << static_cast<int>(comments.size());
	numActionsTaken = actionsstream.str();


	Wt::WDialog *issuedialog = new Wt::WDialog("Add a New Visit Log Entry");
	issuedialog->setMinimumSize(1000, 700);
	issuedialog->setMaximumSize(1000, 700);
	
	Wt::WContainerWidget *container = new Wt::WContainerWidget(issuedialog->contents());
	
	Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget();

	Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
	newprcontainer->setLayout(pthbox);

	Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
	Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

	pthbox->addLayout(leftbox);
	pthbox->addLayout(rightbox);

	Wt::WLabel *lblStatus = new Wt::WLabel("Change Issue Status:");
	lblStatus->setStyleClass("sig-lbl");
	
	Wt::WHBoxLayout *statusbox = new Wt::WHBoxLayout();
	statusbox->addWidget(lblStatus);

	Wt::WComboBox *cbstatus = new Wt::WComboBox();
	cbstatus->setStyleClass("sig-left-box");

	cbstatus->addItem("none selected");
	cbstatus->addItem("Unresolved");
	cbstatus->addItem("Resolved");

	cbstatus->setCurrentIndex(0);
	cbstatus->setMargin(10, Wt::Right);

	Wt::WText *statusout = new Wt::WText();
	statusout->addStyleClass("help-block");

	cbstatus->changed().connect(std::bind([=] () {
	    statusout->setText(Wt::WString::fromUTF8("{1}")
			 .arg(cbstatus->currentText()));
	}));

	statusbox->addWidget(cbstatus);
	
	
	leftbox->addWidget(new Wt::WText("Physician Name: "));
	leftbox->addWidget(new Wt::WText(pracname));		
	leftbox->addWidget(new Wt::WText("PCC Name (Issue Reporter): "));
	leftbox->addWidget(new Wt::WText(pccname));	
	leftbox->addWidget(new Wt::WText("Issue Title: "));
	leftbox->addWidget(new Wt::WText(issuetitle));	
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(new Wt::WText("Issue Description: "));
	leftbox->addWidget(new Wt::WText(issuedescription));	
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(new Wt::WText("Actions Taken: "));
	leftbox->addWidget(new Wt::WText(numActionsTaken));
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(new Wt::WText("Reported:"));
	leftbox->addWidget(new Wt::WText(issuereported.toString()));
	leftbox->addWidget(new Wt::WBreak());
	leftbox->addWidget(new Wt::WText("Resolved"));	
	leftbox->addWidget(new Wt::WText(issueresolved.toString()));
	leftbox->addWidget(new Wt::WBreak());

	rightbox->addLayout(statusbox);

	Wt::WContainerWidget *notes = new Wt::WContainerWidget();

	Wt::WVBoxLayout *notevbox = new Wt::WVBoxLayout();
	notes->setLayout(notevbox);


	Wt::WTable *notetable = new Wt::WTable();
	notetable->setHeaderCount(1);

	notetable->elementAt(0, 0)->addWidget(new Wt::WText("User"));
	notetable->elementAt(0, 1)->addWidget(new Wt::WText("Comment"));
	notetable->elementAt(0, 2)->addWidget(new Wt::WText("Added"));


	int cmrow = 1;
	for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i, ++cmrow)
	{
			new Wt::WText((*i)->staffEmail_, notetable->elementAt(cmrow, 0)),
			new Wt::WText((*i)->commentText_, notetable->elementAt(cmrow, 1)),
			new Wt::WText((*i)->createdAt_.toString(), notetable->elementAt(cmrow, 2));

	}

	ctrans.commit();

	notetable->addStyleClass("table form-inline");
	notetable->addStyleClass("table table-striped");
	notetable->addStyleClass("table table-hover");

	Wt::WContainerWidget *noteresult = new Wt::WContainerWidget();
	noteresult->addStyleClass("backup-comments");
	noteresult->addWidget(notetable);

	notevbox->addWidget(new Wt::WText("<h3>Actions Taken</h3>"));
	notevbox->addWidget(noteresult);

	notevbox->addWidget(new Wt::WText("<h4>Add Action Taken</h4>"));
	Wt::WTextEdit *noteedit = new Wt::WTextEdit();
	noteedit->setHeight(100);
	notevbox->addWidget(noteedit);

	Wt::WText *editout = new Wt::WText();

	Wt::WPushButton *notesave = new Wt::WPushButton("Add Action Taken Note");
	notevbox->addWidget(notesave);
	notesave->clicked().connect(std::bind([=] () {
		
	    editout->setText("<pre>" + Wt::Utils::htmlEncode(noteedit->text()) + "</pre>");
		Wt::WString commentTextOut_ = editout->text().toUTF8();

		Wt::WString staffEmail = session_->strUserEmail();
		Wt::WString parentId = isid;

		Wt::Dbo::Transaction chtrans(dbsession);

		Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

		c.modify()->staffEmail_ = staffEmail.toUTF8();
		c.modify()->commentText_ = commentTextOut_;
		c.modify()->ptId_ = parentId.toUTF8();
		c.modify()->commentId_ = staticComment_->text().toUTF8();

		chtrans.commit();
		
		issuedialog->accept();	

		done_.emit(42, "Refresh Now"); // emit the signal
	}));
	


	Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
	tabW->addTab(newprcontainer,
		     "Details", Wt::WTabWidget::PreLoading);
	tabW->addTab(notes,
		     "Actions Taken", Wt::WTabWidget::PreLoading);

	tabW->setStyleClass("tabwidget");


	Wt::WPushButton *alright = new Wt::WPushButton("Save", issuedialog->footer());
	alright->setDefault(true);

	Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", issuedialog->footer());
	issuedialog->rejectWhenEscapePressed();

	cancel->clicked().connect(issuedialog, &Wt::WDialog::reject);
	
	alright->clicked().connect(std::bind([=]() {

		if (statusout->text() == "Resolved")
		{
			std::string issuestatus = "Resolved";
			Wt::WDateTime issueresolved = Wt::WDateTime::currentDateTime();
			
			dbo::Transaction mptrans(dbsession);

			dbo::ptr<PhysicianIssue> piss = dbsession.find<PhysicianIssue>().where("issue_id = ?").bind(isid);

			piss.modify()->issueStatus_ = issuestatus;
			piss.modify()->issueResolved_ = issueresolved;
			
			mptrans.commit();
		}


		issuedialog->accept();
		done_.emit(42, "Refresh Now"); // emit the signal
	}));
	
	newprcontainer->show();
	issuedialog->show();
}

WString PhysicianVisitResultWidget::setIssueId()
{
	return uuid();
}

void PhysicianVisitResultWidget::newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid)
{
	stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
	
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

		done_.emit(42, "Refresh Now"); // emit the signal

	}));
	
	Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newissuedialog->footer());
	newissuedialog->rejectWhenEscapePressed();

	cancel->clicked().connect(newissuedialog, &Wt::WDialog::reject);
	newprcontainer->show();	
	newissuedialog->show();
}

void PhysicianVisitResultWidget::showPhysicianVisitDialog(Wt::WString vid)
{

    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction transaction(dbsession);

    dbo::ptr<PhysicianVisit> visit = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind(vid.toUTF8());

    Wt::WString status = visit->visitIssues_;
    
    transaction.commit();

    if (status == "Complete") {
        PhysicianVisitResultWidget::showCompletedVisit(vid);
    } else {
        PhysicianVisitResultWidget::showPhysicianVisitComplete(vid);
    }

}
void PhysicianVisitResultWidget::showCompletedVisit(Wt::WString vid)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction transaction(dbsession);

    dbo::ptr<PhysicianVisit> visit = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind(vid.toUTF8());

    Wt::WString visitstart = visit->visitStartTime_.toLocalTime(Wt::WLocale::currentLocale()).toString("MM/dd/yyyy h:m a");
    Wt::WString visitend = visit->visitEndTime_.toLocalTime(Wt::WLocale::currentLocale()).toString("MM/dd/yyyy h:m a");

    Wt::WDateTime visitdatetime = visit->visitDateTime_;
    Wt::WDateTime nextvisitdatetime = visit->nextVisitDateTime_;

    Wt::WString pccname = visit->pccName_;
    Wt::WString pracname = visit->practiceName_;
    Wt::WString visitreason = visit->reasonForVisit_;
    Wt::WString visitoutcome = visit->outcomeOfVisit_;
    Wt::WString visitofficemgr = visit->officeManagerName_;
    Wt::WString pid = visit->practiceId_;

    transaction.commit();

    Wt::WDialog *visitdialog = new Wt::WDialog("Add a New Visit Log Entry");
    visitdialog->setMinimumSize(1000, 700);
    visitdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(visitdialog->contents());

    Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
    newprcontainer->setLayout(pthbox);

    Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

    pthbox->addLayout(leftbox);
    pthbox->addLayout(rightbox);

    leftbox->addWidget(new Wt::WText("MD Name: "));
    leftbox->addWidget(new Wt::WText(pracname));    
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(new Wt::WText("PCC Name: "));
    leftbox->addWidget(new Wt::WText(pccname)); 
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(new Wt::WText("Visit Start Time"));  
    leftbox->addWidget(new Wt::WText(visitstart));
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(new Wt::WText("Visit End Time"));    
    leftbox->addWidget(new Wt::WText(visitend));

    rightbox->addWidget(new Wt::WText("Visit Reason:"));
    rightbox->addWidget(new Wt::WText(visitreason));
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(new Wt::WText("Visit Outcome:"));
    rightbox->addWidget(new Wt::WText(visitoutcome));
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(new Wt::WText("Next Visit Date / Time:"));
    rightbox->addWidget(new Wt::WText(nextvisitdatetime.toString()));

    Wt::WPushButton *newIssue = new Wt::WPushButton("Add an Issue from this Visit");
    newIssue->setDefault(true);

    rightbox->addWidget(newIssue);

    newIssue->clicked().connect(std::bind([=]() {
        visitdialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal

        PhysicianVisitResultWidget::newPhysicianIssueDialog(pid, vid);
    }));

    Wt::WPushButton *alright = new Wt::WPushButton("Done", visitdialog->footer());
    alright->setDefault(true);


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", visitdialog->footer());
    visitdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(visitdialog, &Wt::WDialog::reject);

    alright->clicked().connect(std::bind([=]() {
        visitdialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    newprcontainer->show();
    visitdialog->show();
}

void PhysicianVisitResultWidget::showPhysicianVisitComplete(Wt::WString vid)
{

    
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction transaction(dbsession);

    dbo::ptr<PhysicianVisit> visit = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind(vid.toUTF8());

    Wt::WDateTime visitstart = visit->visitStartTime_;

    Wt::WString pccname = visit->pccName_;
    Wt::WString pracname = visit->practiceName_;
    Wt::WString visitreason = visit->reasonForVisit_;
    Wt::WString pid = visit->practiceId_;
    transaction.commit();

    Wt::WDialog *newvisitdialog = new Wt::WDialog("Complete Visit Log Entry");
    newvisitdialog->setMinimumSize(1000, 700);
    newvisitdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newvisitdialog->contents());

    Wt::WText *visitOutcomeLbl = new Wt::WText("Visit Outcome");
    visitOutcomeLbl->setStyleClass("label-left-box");
    visitOutcome_ = new Wt::WLineEdit(this);
    visitOutcome_->setStyleClass("left-box");
    newprcontainer->addWidget(visitOutcomeLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(visitOutcome_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *visitOfficeManagerLbl = new Wt::WText("Office Manager");
    visitOfficeManagerLbl->setStyleClass("label-left-box");
    visitOfficeManager_ = new Wt::WLineEdit(this);
    visitOfficeManager_->setStyleClass("left-box");
    newprcontainer->addWidget(visitOfficeManagerLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(visitOfficeManager_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WCheckBox *addIssue = new Wt::WCheckBox("Include an issue determined during this visit.");
    newprcontainer->addWidget(addIssue);

    Wt::WPushButton *punchout = new Wt::WPushButton("Punch Out", newvisitdialog->footer());
    punchout->setDefault(true);
    
    punchout->clicked().connect(std::bind([=]() {


        Wt::WDateTime visitend = Wt::WDateTime::currentDateTime();
        Wt::WString visitstatus = "Complete";
        Wt::WString visitoutcome = visitOutcome_->text();
        Wt::WString visitofficemgr = visitOfficeManager_->text();

        dbo::Transaction ptrans(dbsession);

        dbo::ptr<PhysicianVisit> pv = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind(vid.toUTF8());

        pv.modify()->visitEndTime_ = visitend;
        pv.modify()->visitIssues_ = visitstatus;
        pv.modify()->outcomeOfVisit_ = visitoutcome;
        pv.modify()->officeManagerName_ = visitofficemgr;

        ptrans.commit();

        if (addIssue->isChecked())
        {
            newvisitdialog->accept();
            PhysicianVisitResultWidget::newPhysicianIssueDialog(pid, vid);
        } else {
            newvisitdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal
        }

    }));
    
    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newvisitdialog->footer());
    newvisitdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newvisitdialog, &Wt::WDialog::reject);

    newprcontainer->show();
    newvisitdialog->show();
}