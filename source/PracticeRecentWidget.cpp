/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <boost/lexical_cast.hpp>
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
#include <Wt/WTabWidget>

#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>

#include "PracticeRecentWidget.h"
#include "PracticeSession.h"
#include "Referral.h"
#include "Practice.h"
#include "Attachment.h"
#include "AttachmentResource.cpp"
#include "SleepStudy.h"
#include "PendingReferralEntry.h"
#include "StudyProgressEntry.h"
#include "CompletedStudyEntry.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

namespace InboundRefApp{
    struct InboundReferral {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptStatus;

        InboundReferral(const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob,
            const Wt::WString& aPtStatus)
            : ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob),
            ptStatus(aPtStatus){}
    };
}
namespace SleepStudyApp{
    struct InboundStudy {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptStDate;
        Wt::WString ptStatus;

        InboundStudy(const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob,
            const Wt::WString& aPtStDate,
            const Wt::WString& aPtStatus)
            : ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob),
            ptStDate(aPtStDate),
            ptStatus(aPtStatus){}
    };
}
PracticeRecentWidget::PracticeRecentWidget(const char *conninfo, Wt::WString *prac, PracticeSession *session, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo),
prac_(prac),
session_(session)

{

    Wt::log("info") << "PracticeRecentWidget instantiation";

    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
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

void PracticeRecentWidget::pracRecent(Wt::WString& curstr)
{
    Wt::log("notice") << "Practice user switched routes to /recent.";
    /*
     *Recent Referrals NOT Scheduled and NOT Completed
    */

    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    Wt::WContainerWidget *container1 = new Wt::WContainerWidget();
    

    dbo::Transaction transaction(dbsession);
    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    typedef dbo::collection< dbo::ptr<SleepStudy> > SleepStudies;
    
    pid = new Wt::WString(prac_->toUTF8());

    Practices practices;
    if (prac_->toUTF8() != "")
    {
        if (session_->strIsGroup() == "Yes")
        {
            practices = dbsession.find<Practice>().where("group_id = ?").bind(pid->toUTF8());
        } else {
            practices = dbsession.find<Practice>().where("prac_id = ?").bind(pid->toUTF8());
        }
    }

    std::cerr << practices.size();

    std::vector<PendingReferralEntry> refentries;
    std::vector<StudyProgressEntry> progentries;
    std::vector<CompletedStudyEntry> compentries;

    for (Practices::const_iterator p = practices.begin(); p != practices.end(); ++p)
    {
        Wt::WString physician = (*p)->refPracticeDrFirst_ + " " + (*p)->refPracticeDrLast_;

        Referrals referrals = dbsession.find<Referral>().where("pt_firstname LIKE ? OR pt_lastname LIKE ?").bind(searchPt).bind(searchPt).where("ref_status != ?").bind("Study Scheduled").where("ref_status != ?").bind("Study Complete").where("prac_id = ?").bind((*p)->practiceId_).orderBy("ref_received desc");
        std::cerr << referrals.size() << std::endl;
        for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i)
        {
            PendingReferralEntry *refent = new PendingReferralEntry();

            refent->patientFirstName = (*i)->refFirstName_;
            refent->patientLastName = (*i)->refLastName_;
            refent->physicianName = physician;
            refent->patientDob = (*i)->refDob_;
            refent->patientCity = (*i)->refCity_;
            refent->status = (*i)->refStatus_;
            
            refentries.push_back(*refent);
        }

        SleepStudies studies = dbsession.find<SleepStudy>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt).where("std_status != ? OR std_status != ?").bind("Study Complete").bind("Checked Out").where("prac_id = ?").bind((*p)->practiceId_).orderBy("std_date desc");
        std::cerr << studies.size() << std::endl;
        for (SleepStudies::const_iterator i = studies.begin(); i != studies.end(); ++i)
        {

            StudyProgressEntry *stdent = new StudyProgressEntry();
            
            stdent->patientFirstName = (*i)->ptFirstName_;
            stdent->patientLastName = (*i)->ptLastName_;
            stdent->physicianName = physician;
            stdent->patientDob = (*i)->ptDob_;
            stdent->studyDate = (*i)->studyDate_;
            stdent->status = (*i)->studyStatus_;
            stdent->studyId = (*i)->studyId_;
            
            progentries.push_back(*stdent); 
        }

        SleepStudies compstudies = dbsession.find<SleepStudy>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt).where("std_status = ? OR std_status = ?").bind("Study Complete").bind("Checked Out").where("prac_id = ?").bind((*p)->practiceId_).orderBy("std_date desc");
        std::cerr << compstudies.size() << std::endl;
        for (SleepStudies::const_iterator i = compstudies.begin(); i != compstudies.end(); ++i)
        {
            
            CompletedStudyEntry *compent = new CompletedStudyEntry();
    
            compent->patientFirstName = (*i)->ptFirstName_;
            compent->patientLastName = (*i)->ptLastName_;
            compent->physicianName = physician;
            compent->patientDob = (*i)->ptDob_;
            compent->studyDate = (*i)->studyDate_;
            compent->status = (*i)->studyStatus_;
            compent->studyId = (*i)->studyId_;
    
            compentries.push_back(*compent); 

        }

    }
    
    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Physician Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Patient Date of Birth"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Patient City"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Status"));

    int row = 1;
    for (std::vector<PendingReferralEntry>::iterator en = refentries.begin(); en != refentries.end(); ++en, ++row)
    {
            new Wt::WText(en->patientFirstName, table->elementAt(row, 0)),
            new Wt::WText(en->patientLastName, table->elementAt(row, 1)),
            new Wt::WText(en->physicianName, table->elementAt(row, 2)),
            new Wt::WText(en->patientDob, table->elementAt(row, 3)),
            new Wt::WText(en->patientCity, table->elementAt(row, 4)),
            new Wt::WText(en->status, table->elementAt(row, 5));
    }


    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    /*
     *Recent Studies Scheduled but NOT Completed
    */

    Wt::WContainerWidget *container2 = new Wt::WContainerWidget();

    Wt::WTable *table2 = new Wt::WTable();
    table2->setHeaderCount(1);

    table2->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table2->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table2->elementAt(0, 2)->addWidget(new Wt::WText("Physician Name"));
    table2->elementAt(0, 3)->addWidget(new Wt::WText("Patient Date of Birth"));
    table2->elementAt(0, 4)->addWidget(new Wt::WText("Study Date"));
    table2->elementAt(0, 5)->addWidget(new Wt::WText("Status"));
    table2->elementAt(0, 6)->addWidget(new Wt::WText("More Info"));


    int srow = 1;
    for (std::vector<StudyProgressEntry>::iterator en = progentries.begin(); en != progentries.end(); ++en, ++srow)
    {
            new Wt::WText(en->patientFirstName, table2->elementAt(srow, 0)),
            new Wt::WText(en->patientLastName, table2->elementAt(srow, 1)),
            new Wt::WText(en->physicianName, table2->elementAt(srow, 2)),
            new Wt::WText(en->patientDob, table2->elementAt(srow, 3)),
            new Wt::WText(en->studyDate.toString("MM/dd/yyyy"), table2->elementAt(srow, 4));
            new Wt::WText(en->status, table2->elementAt(srow, 5)),
            btnDownloadRef = new Wt::WPushButton("Details", table2->elementAt(srow, 6)),
            btnDownloadRef->clicked().connect(boost::bind(&PracticeRecentWidget::showStudyDialog, this, en->studyId));

    }

    table2->addStyleClass("table form-inline");
    table2->addStyleClass("table table-striped");
    table2->addStyleClass("table table-hover");

    /*
     *Recent Studies NOT Scheduled but Completed
    */

    Wt::WContainerWidget *container3 = new Wt::WContainerWidget();

    Wt::WTable *table3 = new Wt::WTable();
    table3->setHeaderCount(1);

    table3->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table3->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table3->elementAt(0, 1)->addWidget(new Wt::WText("Physician Name"));
    table3->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    table3->elementAt(0, 3)->addWidget(new Wt::WText("Study Date"));
    table3->elementAt(0, 4)->addWidget(new Wt::WText("Status"));
    table3->elementAt(0, 5)->addWidget(new Wt::WText("More Info"));


    int crow = 1;
    for (std::vector<CompletedStudyEntry>::iterator en = compentries.begin(); en != compentries.end(); ++en, ++crow)
    {
            new Wt::WText(en->patientFirstName, table3->elementAt(crow, 0)),
            new Wt::WText(en->patientLastName, table3->elementAt(crow, 1)),
            new Wt::WText(en->physicianName, table3->elementAt(crow, 2)),
            new Wt::WText(en->patientDob, table3->elementAt(crow, 3)),
            new Wt::WText(en->studyDate.toString("MM/dd/yyyy"), table3->elementAt(crow, 4));
            new Wt::WText(en->status, table3->elementAt(crow, 5)),
            btnDownloadRef = new Wt::WPushButton("Details", table3->elementAt(crow, 6)),
            btnDownloadRef->clicked().connect(boost::bind(&PracticeRecentWidget::showStudyDialog, this, en->studyId));

    }

    table3->addStyleClass("table form-inline");
    table3->addStyleClass("table table-striped");
    table3->addStyleClass("table table-hover");

    if (prac_->toUTF8() == "") {
        Wt::WText *loading = new Wt::WText("<h3>Loading data, please wait....</h3>");
        loading->setStyleClass("loading-message");

        container1->addWidget(loading);
        container1->addWidget(new Wt::WBreak());
        done_.emit(42, "Refresh Now"); // emit the signal
    }

    std::stringstream numrefss;
    numrefss << static_cast<int>(refentries.size());
    Wt::WString shownumrefss = numrefss.str();

    std::stringstream numstdss;
    numstdss << static_cast<int>(progentries.size());
    Wt::WString shownumstdss = numstdss.str();

    std::stringstream numcompss;
    numcompss << static_cast<int>(compentries.size());
    Wt::WString shownumcompss = numcompss.str();


    Wt::WText *numReferrals = new Wt::WText(shownumrefss);
    numReferrals->setStyleClass("physician-num");

    Wt::WText *numStudies = new Wt::WText(shownumstdss);
    numStudies->setStyleClass("physician-num");

    Wt::WText *numCompleted = new Wt::WText(shownumcompss);
    numCompleted->setStyleClass("physician-num");

    Wt::WContainerWidget *numbercontainer = new Wt::WContainerWidget();
    Wt::WHBoxLayout *numberhbox = new Wt::WHBoxLayout();
    numbercontainer->setLayout(numberhbox);

    Wt::WVBoxLayout *numrefvbox = new Wt::WVBoxLayout();
    numberhbox->addLayout(numrefvbox);

    Wt::WVBoxLayout *numstdvbox = new Wt::WVBoxLayout();
    numberhbox->addLayout(numstdvbox);

    Wt::WVBoxLayout *numcompvbox = new Wt::WVBoxLayout();
    numberhbox->addLayout(numcompvbox);


    Wt::WText *pendingref = new Wt::WText("Pending Referrals");
    pendingref->setStyleClass("pending-ref-header");
    numrefvbox->addWidget(numReferrals);
    numrefvbox->addWidget(pendingref);

    Wt::WText *inprogress = new Wt::WText("Studies Scheduled");
    inprogress->setStyleClass("inprogress-header");
    numstdvbox->addWidget(numStudies);
    numstdvbox->addWidget(inprogress);

    Wt::WText *completedstd = new Wt::WText("Completed Studies");
    completedstd->setStyleClass("completed-std-header");
    numcompvbox->addWidget(numCompleted);
    numcompvbox->addWidget(completedstd);

    container1->addWidget(numbercontainer);
    container1->addWidget(new Wt::WBreak());
    container1->addWidget(new Wt::WText("<h4>Pending Referrals</h4>"));
    container1->addWidget(table);
    container2->addWidget(new Wt::WText("<h4>Sleep Studies in Progress</h4>"));
    container2->addWidget(table2);
    container3->addWidget(new Wt::WText("<h4>Completed Sleep Studies</h4>"));
    container3->addWidget(table3);

    addWidget(container1);
    addWidget(container2);
    addWidget(container3);



}
void PracticeRecentWidget::showStudyDialog(WString& study)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Sleep Study Referral");
    
    dialog->setMaximumSize(800, 600);
    dialog->setMinimumSize(800, 600);
    
    dbo::Transaction strans(dbsession);

    dbo::ptr<SleepStudy> sleepstudy = dbsession.find<SleepStudy>().where("std_id = ?").bind(study);

    WString ptfirst = sleepstudy->ptFirstName_;
    WString ptlast = sleepstudy->ptLastName_;
    WString ptaddr1 = sleepstudy->ptAddress1_;
    WString ptaddr2 = sleepstudy->ptAddress2_;
    WString ptcity = sleepstudy->ptCity_;
    WString ptzip = sleepstudy->ptZip_;
    WString pthome = sleepstudy->ptHomePhone_;
    WString ptcell = sleepstudy->ptCellPhone_;
    WString ptdob = sleepstudy->ptDob_;
    WString ptgender = sleepstudy->ptGender_;
    WString stdiag = sleepstudy->studyDiagnostic_;
    WString stover = sleepstudy->studyOvernight_;
    WString sthome = sleepstudy->studyHomeStudy_;
    WString stother = sleepstudy->studyOtherSleepTest_;
    WString stauth = sleepstudy->studyAuthConsult_;

    WString ststatus = sleepstudy->studyStatus_;
    WString stdate = sleepstudy->studyDate_.toString("MM/dd/yyyy");
    WString sttime = sleepstudy->studyTime_;
    WString stloco = sleepstudy->studyLocation_;

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WContainerWidget *studyinfo = new Wt::WContainerWidget();
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);
    studyinfo->setLayout(hbox);

    Wt::WContainerWidget *left = new Wt::WContainerWidget(studyinfo);
    left->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Address: <br /></strong>" + ptaddr1));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText(ptaddr2));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient City: </strong>" + ptcity));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Zip: </strong>" + ptzip));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Home Phone: </strong>" + pthome));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Cell Phone: </strong>" + ptcell));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    left->addWidget(new Wt::WBreak());

    Wt::WContainerWidget *right = new Wt::WContainerWidget(studyinfo);
    right->addStyleClass("refdiagright");

    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<h4>Referral Diagnosis</h4> "));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Diagnosis: </strong>" + stdiag));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Overnight Study: </strong>" + stover));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Other Study: </strong>" + stother));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Home Study: </strong>" + sthome));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Consultation: </strong><br /> " +stauth));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    right->addWidget(new Wt::WText("Current Status:"+ ststatus));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("Study Date:"+ stdate));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("Study Time:"+ sttime));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("Study Location:"+ stloco));
    right->addWidget(new Wt::WBreak());
   


    Wt::WContainerWidget *filecontainer = new Wt::WContainerWidget();
    //put table here
    dbo::Transaction satrans(dbsession);
    
    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;
    
    Attachments sattachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(study);
    std::cerr << sattachments.size() << std::endl;
    
    if (sattachments.size() > 0) {
        Wt::WTable *filetable = new Wt::WTable();
        filetable->setHeaderCount(1);

        filetable->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
        filetable->elementAt(0, 1)->addWidget(new Wt::WText("Download"));

        int frow = 1;
        for (Attachments::const_iterator ai = sattachments.begin(); ai != sattachments.end(); ++ai, ++frow)
        {

            Wt::WResource *attc = new AttachmentResource((*ai)->attId_, (*ai)->tempRefId_, (*ai)->attFileName_, (*ai)->attContentDescription_, this);

            new Wt::WText((*ai)->attFileName_, filetable->elementAt(frow, 0)),
            downDone = new Wt::WText("Downloaded", filetable->elementAt(frow, 1)),
            downDone->hide(),
            
            btnDownloadAtt = new Wt::WPushButton("Download", filetable->elementAt(frow, 1));
            btnDownloadAtt->setLink(attc);

        }

        satrans.commit();

        filetable->addStyleClass("table form-inline");
        filetable->addStyleClass("table table-striped");
        filetable->addStyleClass("table table-hover");

        filecontainer->addWidget(new Wt::WText("<h4>Study Report Files</h4>"));
        filecontainer->addWidget(filetable);
    } else {
        filecontainer->addWidget(new Wt::WText("<h3>The report for this sleep study is not available yet.</h3><h4>Please contact our office at 562-622-1002 if you have any questions.</h4>"));
    }
    
    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(studyinfo,
             "Study Info", Wt::WTabWidget::PreLoading);
    tabW->addTab(filecontainer,
             "Download Report", Wt::WTabWidget::PreLoading);
    
    tabW->setStyleClass("tabwidget");


    Wt::WPushButton *alright = new Wt::WPushButton("Ok", dialog->footer());

    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    // dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));


    vbox1->addWidget(left);
    vbox2->addWidget(right);

    container->show();
    dialog->show();
    strans.commit();
}
