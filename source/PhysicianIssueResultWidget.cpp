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

#include "PhysicianIssueResultWidget.h"
#include "Practice.h"
#include "Comment.h"
#include "PhysicianIssue.h"
#include "PhysicianVisit.h"
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

PhysicianIssueResultWidget::PhysicianIssueResultWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session),
	conninfo_(conninfo),
	pg_(conninfo)
{
	dbsession.setConnection(pg_);


	pg_.setProperty("show-queries", "true");
	
	dbsession.mapClass<CompositeEntity>("composite");
	dbsession.mapClass<PhysicianIssue>("physicianissue");
	dbsession.mapClass<PhysicianVisit>("physicianvisit");
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

WString PhysicianIssueResultWidget::setCommentId()
{
	return uuid();
}


void PhysicianIssueResultWidget::showPhysicianIssueLog(Wt::WString& curstr, int limit, int offset)
{		
	Wt::WContainerWidget *container = new Wt::WContainerWidget();
	
	dbo::Transaction transaction(dbsession);
	
	std::string strSearchPrac = curstr.toUTF8();
  	std::string searchPrac = '%' + strSearchPrac +'%';
	
	typedef dbo::collection< dbo::ptr<PhysicianIssue> > Issues;
	Issues issues = dbsession.find<PhysicianIssue>().where("md_name LIKE ? OR pcc_name LIKE ?").bind(searchPrac).bind(searchPrac);
	std::cerr << issues.size() << std::endl;
	
	Wt::WTable *table = new Wt::WTable();
	table->setHeaderCount(1);

	table->elementAt(0, 0)->addWidget(new Wt::WText("MD Name"));
	table->elementAt(0, 1)->addWidget(new Wt::WText("PCC"));
	table->elementAt(0, 2)->addWidget(new Wt::WText("Issue Title"));
	table->elementAt(0, 3)->addWidget(new Wt::WText("Reported"));
	table->elementAt(0, 4)->addWidget(new Wt::WText("Status"));
	table->elementAt(0, 5)->addWidget(new Wt::WText("Details"));


	int row = 1;
	for (Issues::const_iterator i = issues.begin(); i != issues.end(); ++i, ++row)
	{
			new Wt::WText((*i)->mdName_, table->elementAt(row, 0)),
			new Wt::WText((*i)->pccName_, table->elementAt(row, 1)),
			new Wt::WText((*i)->issueTitle_, table->elementAt(row, 2)),
			new Wt::WText((*i)->issueReported_.toString("MM/dd/yyyy"), table->elementAt(row, 3)),
			new Wt::WText((*i)->issueStatus_, table->elementAt(row, 4)),
			btnSelectPhysicianIssue = new Wt::WPushButton("Details", table->elementAt(row, 5)),
			//click 'PDF' button on any table row, connect downloadReferral with that referral uuid
			btnSelectPhysicianIssue->clicked().connect(boost::bind(&PhysicianIssueResultWidget::showPhysicianIssueDialog, this, (*i)->issueId_));
	}

	
	transaction.commit();

	table->addStyleClass("table form-inline");
	table->addStyleClass("table table-striped");
	table->addStyleClass("table table-hover");


	container->addWidget(table);
	addWidget(container);

}

void PhysicianIssueResultWidget::showPhysicianIssueDialog(Wt::WString isid)
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
