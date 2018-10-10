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
#include <Wt/WMessageBox>

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

#include "PatientListContainerWidget.h"
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
#include "InsuranceItem.h"

#include "StudyType.h"
#include "StudyTypeItem.h"

#include "restclient-cpp/restclient.h"

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

PatientListContainerWidget::PatientListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
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
    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<StudyType>("studytype");
    dbsession.mapClass<AttachmentDb>("attachment");


    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
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

void PatientListContainerWidget::showCurrent(int limit, int offset)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients = dbsession.find<Patient>().where("LIMIT ? OFFSET ?").bind(limit).bind(offset);
    std::cerr << patients.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Patient City"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText("Select"));

    int prow = 1;
    for (Patients::const_iterator pi = patients.begin(); pi != patients.end(); ++pi, ++prow)
    {

            new Wt::WText((*pi)->ptFirstName_, showptable->elementAt(prow, 0)),
            new Wt::WText((*pi)->ptLastName_, showptable->elementAt(prow, 1)),
            new Wt::WText((*pi)->ptDob_, showptable->elementAt(prow, 2)),
            new Wt::WText((*pi)->ptCity_, showptable->elementAt(prow, 3)),
            btnSelectPatient = new Wt::WPushButton("New Referral", showptable->elementAt(prow, 4));
    }

    transaction.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");


    container->addWidget(showptable);
    addWidget(container);
}


void PatientListContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients = dbsession.find<Patient>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt);
    std::cerr << patients.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Patient City"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText(""));
    showptable->elementAt(0, 5)->addWidget(new Wt::WText(""));

    int prow = 1;
    for (Patients::const_iterator pi = patients.begin(); pi != patients.end(); ++pi, ++prow)
    {

            new Wt::WText((*pi)->ptFirstName_, showptable->elementAt(prow, 0)),
            new Wt::WText((*pi)->ptLastName_, showptable->elementAt(prow, 1)),
            new Wt::WText((*pi)->ptDob_, showptable->elementAt(prow, 2)),
            new Wt::WText((*pi)->ptCity_, showptable->elementAt(prow, 3)),
            btnSelectPatient = new Wt::WPushButton("New Referral", showptable->elementAt(prow, 4)),
            btnSelectPatient->clicked().connect(boost::bind(&PatientListContainerWidget::createReferral, this, (*pi)->pracId_, (*pi)->ptId_));
            btnShowPatient = new Wt::WPushButton("Patient Details", showptable->elementAt(prow, 5)),
            btnShowPatient->clicked().connect(boost::bind(&PatientListContainerWidget::showPatient, this, (*pi)->ptId_));

    }

    transaction.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");


    container->addWidget(showptable);
    addWidget(container);
}

void PatientListContainerWidget::showRescheduleSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients = dbsession.find<Patient>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt).orderBy("num_reschedule desc").where("num_reschedule != ?").bind(NULL);
    std::cerr << patients.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Studies Rescheduled"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText(""));

    int prow = 1;
    for (Patients::const_iterator pi = patients.begin(); pi != patients.end(); ++pi, ++prow)
    {
            std::stringstream totalstream;
            totalstream << static_cast<int>((*pi)->numReschedule_);
            Wt::WString showtotalout = totalstream.str();

            new Wt::WText((*pi)->ptFirstName_, showptable->elementAt(prow, 0)),
            new Wt::WText((*pi)->ptLastName_, showptable->elementAt(prow, 1)),
            new Wt::WText((*pi)->ptDob_, showptable->elementAt(prow, 2)),
            new Wt::WText(showtotalout, showptable->elementAt(prow, 3)),
            btnShowPatient = new Wt::WPushButton("Patient Details", showptable->elementAt(prow, 4)),
            btnShowPatient->clicked().connect(boost::bind(&PatientListContainerWidget::showPatient, this, (*pi)->ptId_));

    }

    transaction.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");


    container->addWidget(showptable);
    addWidget(container);
}

void PatientListContainerWidget::showReferralSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients = dbsession.find<Patient>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt);
    std::cerr << patients.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Patient City"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText(""));
    showptable->elementAt(0, 5)->addWidget(new Wt::WText(""));

    int prow = 1;
    for (Patients::const_iterator pi = patients.begin(); pi != patients.end(); ++pi, ++prow)
    {

            new Wt::WText((*pi)->ptFirstName_, showptable->elementAt(prow, 0)),
            new Wt::WText((*pi)->ptLastName_, showptable->elementAt(prow, 1)),
            new Wt::WText((*pi)->ptDob_, showptable->elementAt(prow, 2)),
            new Wt::WText((*pi)->ptCity_, showptable->elementAt(prow, 3)),
            btnSelectPatient = new Wt::WPushButton("New Referral", showptable->elementAt(prow, 4)),
            btnSelectPatient->clicked().connect(boost::bind(&PatientListContainerWidget::createReferral, this, (*pi)->pracId_, (*pi)->ptId_));
            btnShowPatient = new Wt::WPushButton("Patient Details", showptable->elementAt(prow, 5)),
            btnShowPatient->clicked().connect(boost::bind(&PatientListContainerWidget::showPatient, this, (*pi)->ptId_));

    }

    transaction.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");


    container->addWidget(showptable);
    addWidget(container);
}

void PatientListContainerWidget::showStudySearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients = dbsession.find<Patient>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchPt).bind(searchPt);
    std::cerr << patients.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Patient Date of Birth"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Patient City"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText(""));
    showptable->elementAt(0, 5)->addWidget(new Wt::WText(""));

    int prow = 1;
    for (Patients::const_iterator pi = patients.begin(); pi != patients.end(); ++pi, ++prow)
    {

            new Wt::WText((*pi)->ptFirstName_, showptable->elementAt(prow, 0)),
            new Wt::WText((*pi)->ptLastName_, showptable->elementAt(prow, 1)),
            new Wt::WText((*pi)->ptDob_, showptable->elementAt(prow, 2)),
            new Wt::WText((*pi)->ptCity_, showptable->elementAt(prow, 3)),
            btnSelectPatient = new Wt::WPushButton("New Referral", showptable->elementAt(prow, 4)),
            btnSelectPatient->clicked().connect(boost::bind(&PatientListContainerWidget::createReferral, this,  (*pi)->pracId_, (*pi)->ptId_));
            btnShowPatient = new Wt::WPushButton("Patient Details", showptable->elementAt(prow, 5)),
            btnShowPatient->clicked().connect(boost::bind(&PatientListContainerWidget::showPatient, this, (*pi)->ptId_));

    }

    transaction.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");


    container->addWidget(showptable);
    addWidget(container);
}

WString PatientListContainerWidget::setPtId()
{
    return uuid();
}
WString PatientListContainerWidget::setCommentId()
{
    return uuid();
}

WString PatientListContainerWidget::setStdId()
{
    return uuid();
}

WString PatientListContainerWidget::setRefId()
{
    return uuid();
}

WString PatientListContainerWidget::setPracId()
{
    return uuid();
}
WString PatientListContainerWidget::setBuId()
{
    return uuid();
}
void PatientListContainerWidget::showPatient(std::string& pt)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    staticComment_ = new Wt::WText();
    staticComment_->setText(setCommentId());

    dbo::Transaction ptrans(dbsession);

    dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);
    WString pfirst = patient->ptFirstName_;
    WString plast = patient->ptLastName_;
    WString pdob = patient->ptDob_;
    WString pgender = patient->ptGender_;
    WString phome = patient->ptHomePhone_;
    WString pcell = patient->ptCellPhone_;
    WString paddr1 = patient->ptAddress1_;
    WString paddr2 = patient->ptAddress2_;
    WString pcity = patient->ptCity_;
    WString ptout = patient->ptId_;
    WString ptinsur = patient->ptInsuranceName_;
    WString ptpracid = patient->pracId_;
    


    WString pemail = patient->ptEmail_;

    ptrans.commit();

    dbo::Transaction ptpractrans(dbsession);

    dbo::ptr<Practice> ptprac = dbsession.find<Practice>().where("prac_id = ?").bind(ptpracid);
    WString pracname = ptprac->refPracticeName_;
    WString pdrfirst = ptprac->refPracticeDrFirst_;
    WString pdrlast = ptprac->refPracticeDrLast_;
    WString pracnpi = ptprac->refPracticeNpi_;

    Wt::WDialog *dialog = new Wt::WDialog("Patient Info for: " + pfirst + " " + plast);
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WContainerWidget *ptcontainer = new Wt::WContainerWidget();

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *hbox2 = new Wt::WHBoxLayout();
    ptcontainer->setLayout(hbox);
    ptcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WContainerWidget *left = new Wt::WContainerWidget();
    left->addStyleClass("refdiagleft");
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<h4>General Info:</h4>"));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("First Name: " + pfirst));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Last Name: " + plast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Home Phone: " + phome));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Cell Phone: " + plast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Address 1: " + paddr1));
    left->addWidget(new Wt::WText("Address 2: " + paddr2));
    left->addWidget(new Wt::WText("City: " + pcity));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Email: " + pemail));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Insurance: " + ptinsur));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<h4>Referring Practice Info</h4>"));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Name: "+pracname));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("MD First Name: "+pdrfirst));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("MD Last Name: "+pdrlast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Phone Number: "+pracnpi));

    Wt::WContainerWidget *right = new Wt::WContainerWidget();

    right->addWidget(new Wt::WText("<h4>Options</h4>"));


    btnAddToBackup_ = new Wt::WPushButton("Add to Backup List");

    right->addWidget(btnAddToBackup_);

    btnAddToBackup_->clicked().connect(boost::bind(&PatientListContainerWidget::addPatientToBackup, this, pt));




    vbox1->addWidget(left);
    vbox2->addWidget(right);

    Wt::WContainerWidget *filecontainer = new Wt::WContainerWidget();
        //put table here
    dbo::Transaction atrans(dbsession);

    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(pt);
    std::cerr << attachments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Download"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Delete"));

    int row = 1;
    for (Attachments::const_iterator i = attachments.begin(); i != attachments.end(); ++i, ++row)
    {
        Wt::WString delid = (*i)->attId_;

        Wt::WResource *attc = new AttachmentResource((*i)->attId_, (*i)->tempRefId_, (*i)->attFileName_, (*i)->attContentDescription_, this);

        new Wt::WText((*i)->attFileName_, table->elementAt(row, 0)),
        downDone = new Wt::WText("Downloaded", table->elementAt(row, 1)),
        downDone->hide(),
        
        btnDownloadAtt = new Wt::WPushButton("Download", table->elementAt(row, 1)),
        btnDeleteAtt = new Wt::WPushButton("Delete", table->elementAt(row, 2));
        btnDeleteAtt->setStyleClass("btn-danger");
        
        btnDeleteAtt->clicked().connect(std::bind([=](){
            dialog->accept();            
            PatientListContainerWidget::deleteAttachment(delid);
        }));
        
        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WHBoxLayout *fileshbox = new Wt::WHBoxLayout();
    filecontainer->setLayout(fileshbox);

    Wt::WVBoxLayout *filesleft = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesleft);
    Wt::WVBoxLayout *filesright = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesright);

    filesleft->addWidget(new Wt::WText("<h4>Patient Files</h4>"));
    filesleft->addWidget(table);

    composer_ = new Composer(conninfo_, pt, this);

    filesright->addWidget(new Wt::WText("<h4>Upload Files</h4>"));
    filesright->addWidget(new Wt::WText("Upload files to this patient chart."));
    filesright->addWidget(composer_);


    Wt::WContainerWidget *notecontainer = new Wt::WContainerWidget();

    Wt::WVBoxLayout *notevbox = new Wt::WVBoxLayout();
    notecontainer->setLayout(notevbox);

    dbo::Transaction cmtrans(dbsession);

    typedef dbo::collection< dbo::ptr<Comment> > Comments;
    Comments comments = dbsession.find<Comment>().where("pt_id = ?").bind(pt);
    std::cerr << comments.size() << std::endl;

    Wt::WTable *notetable = new Wt::WTable();
    notetable->setHeaderCount(1);

    notetable->elementAt(0, 0)->addWidget(new Wt::WText("User"));
    notetable->elementAt(0, 1)->addWidget(new Wt::WText("Comment"));
    notetable->elementAt(0, 2)->addWidget(new Wt::WText("Added"));


    int cmrow = 1;
    for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i, ++cmrow)
    {
            Wt::WDateTime fixdate = (*i)->createdAt_;
            Wt::WDateTime adjustdate = fixdate.addSecs(-25200);
            WString finaldate = adjustdate.toString("MM/dd/yyyy hh:mm a");

            new Wt::WText((*i)->staffEmail_, notetable->elementAt(cmrow, 0)),
            new Wt::WText((*i)->commentText_, notetable->elementAt(cmrow, 1)),
            new Wt::WText(finaldate, notetable->elementAt(cmrow, 2));

    }

    cmtrans.commit();

    notetable->addStyleClass("table form-inline");
    notetable->addStyleClass("table table-striped");
    notetable->addStyleClass("table table-hover");

    Wt::WContainerWidget *noteresult = new Wt::WContainerWidget();
    noteresult->addStyleClass("backup-comments");
    noteresult->addWidget(notetable);

    notevbox->addWidget(new Wt::WText("<h3>Chart Notes</h3>"));
    notevbox->addWidget(noteresult);

    notevbox->addWidget(new Wt::WText("<h4>Add Chart Note</h4>"));
    Wt::WTextEdit *noteedit = new Wt::WTextEdit();
    noteedit->setHeight(100);
    notevbox->addWidget(noteedit);

    Wt::WText *editout = new Wt::WText();

    Wt::WPushButton *notesave = new Wt::WPushButton("Add Chart Note");
    notevbox->addWidget(notesave);
    notesave->clicked().connect(std::bind([=] () {

        editout->setText("<pre>" + Wt::Utils::htmlEncode(noteedit->text()) + "</pre>");
        Wt::WString commentTextOut_ = editout->text().toUTF8();

        Wt::WString staffEmail = session_->strUserEmail();
        Wt::WString patientId = ptout;

        Wt::Dbo::Transaction chtrans(dbsession);

        Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

        c.modify()->staffEmail_ = staffEmail.toUTF8();
        c.modify()->commentText_ = commentTextOut_;
        c.modify()->ptId_ = patientId.toUTF8();

        //h.modify()->studentFirstName_ = studentFirst.toUTF8();
        //h.modify()->studentLastName_ = studentLast.toUTF8();
        //h.modify()->studentId_ = studentId.toUTF8();

        c.modify()->commentId_ = staticComment_->text().toUTF8();

        chtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal


    }));

    Wt::WContainerWidget *modifypt = new Wt::WContainerWidget();

    Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
    modifypt->setLayout(pthbox);

    Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

    pthbox->addLayout(leftbox);
    pthbox->addLayout(rightbox);

    Wt::WPushButton *modpt = new Wt::WPushButton("Save Changes");
    modpt->setDefault(true);

    //input fields for patient demographics
    Wt::WText *ptFirstNameLbl = new Wt::WText("Patient First Name");
    ptFirstNameLbl->setStyleClass("label-left-box");
    patientFirstName_ = new Wt::WLineEdit(this);
    patientFirstName_->setStyleClass("left-box");
    patientFirstName_->setText(pfirst);
    leftbox->addWidget(ptFirstNameLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientFirstName_);

    Wt::WText *ptLastNameLbl = new Wt::WText("Patient Last Name");
    ptLastNameLbl->setStyleClass("label-left-box");
    patientLastName_ = new Wt::WLineEdit(this);
    patientLastName_->setStyleClass("left-box");
    patientLastName_->setText(plast);
    leftbox->addWidget(ptLastNameLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientLastName_);

    Wt::WText *ptCityLbl = new Wt::WText("Patient City");
    ptCityLbl->setStyleClass("label-left-box");
    patientCity_ = new Wt::WLineEdit(this);
    patientCity_->setStyleClass("left-box");
    patientCity_->setText(pcity);
    leftbox->addWidget(ptCityLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientCity_);

    Wt::WText *ptHomeLbl = new Wt::WText("Patient Home Phone");
    ptHomeLbl->setStyleClass("label-right-box");
    patientHomePhone_ = new Wt::WLineEdit(this);
    patientHomePhone_->setStyleClass("right-box");
    patientHomePhone_->setText(phome);

    leftbox->addWidget(ptHomeLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientHomePhone_);

    Wt::WText *ptCellLbl = new Wt::WText("Patient Cell Phone");
    ptCellLbl->setStyleClass("label-right-box");
    patientCellPhone_ = new Wt::WLineEdit(this);
    patientCellPhone_->setStyleClass("right-box");
    patientCellPhone_->setText(pcell);
    leftbox->addWidget(ptCellLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientCellPhone_);

    Wt::WText *ptEmailLbl = new Wt::WText("Patient Email Address");
    ptEmailLbl->setStyleClass("label-right-box");
    patientEmail_ = new Wt::WLineEdit(this);
    patientEmail_->setStyleClass("right-box");
    patientEmail_->setText(pemail);

    leftbox->addWidget(ptEmailLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientEmail_);

    //modify birth date
    Wt::WDate ptdob;
    ptdob = Wt::WDate::fromString(pdob, "MM/dd/yyyy");
    
    Wt::WString dobday;
    Wt::WString dobmonth;
    Wt::WString dobyear;
    
    std::stringstream daystr;
    std::stringstream monthstr;
    std::stringstream yearstr;

    daystr << std::setw(2) << std::setfill('0') << static_cast<int>(ptdob.day());
    monthstr << std::setw(2) << std::setfill('0') << static_cast<int>(ptdob.month());
    yearstr << static_cast<int>(ptdob.year());

    dobday = daystr.str();
    dobmonth = monthstr.str();
    dobyear = yearstr.str();

    Wt::WContainerWidget *dob = new Wt::WContainerWidget();
    dob->setStyleClass("dob-container");

    Wt::WHBoxLayout *dobhbox = new Wt::WHBoxLayout();
    dob->setLayout(dobhbox);

    dobMonth = new Wt::WLineEdit();
    dobDay = new Wt::WLineEdit();
    dobYear = new Wt::WLineEdit();

    dobMonth->setStyleClass("dob-month");
    dobMonth->setPlaceholderText(dobmonth);
    dobDay->setStyleClass("dob-day");
    dobDay->setPlaceholderText(dobday);
    dobYear->setStyleClass("dob-year");
    dobYear->setPlaceholderText(dobyear);

    dobhbox->addWidget(dobMonth);
    dobhbox->addWidget(new Wt::WText("/"));
    dobhbox->addWidget(dobDay);
    dobhbox->addWidget(new Wt::WText("/"));
    dobhbox->addWidget(dobYear);

    leftbox->addWidget(dob);




    Wt::WContainerWidget *ptInsuranceContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblInsurance = new Wt::WLabel("Patient Insurance:");
    lblInsurance->setStyleClass("sig-lbl");
    Wt::WText *out = new Wt::WText();
    out->setText(ptinsur);

    Wt::WHBoxLayout *insurancebox = new Wt::WHBoxLayout();
    insurancebox->addWidget(lblInsurance);
    insurancebox->addWidget(new Wt::WBreak());
    insurancebox->addWidget(out);
    insurancebox->addWidget(new Wt::WBreak());

    dbo::Transaction instrans(dbsession);

    typedef dbo::collection< dbo::ptr<Insurance> > Insurances;
    Insurances insurances = dbsession.find<Insurance>();
    std::cerr << insurances.size() << std::endl;

    std::vector<InsuranceItem> insuranceitems;

    for (Insurances::const_iterator i = insurances.begin(); i != insurances.end(); ++i)
    {

        Wt::WString name = (*i)->insuranceName_;
        Wt::WString id = (*i)->insuranceId_;


        InsuranceItem *e = new InsuranceItem();
        e->insuranceName = name;
        e->insuranceId = id;


        insuranceitems.push_back(*e);

    }
    log("notice") << "Current Number of insurance items "<< insuranceitems.size();

    instrans.commit();



    Wt::WComboBox *cbinsurance = new Wt::WComboBox();
    cbinsurance->setStyleClass("sig-left-box");
    Wt::WText *insuranceout = new Wt::WText();
    cbinsurance->addItem("");

    for (std::vector<InsuranceItem>::const_iterator ev = insuranceitems.begin(); ev != insuranceitems.end(); ++ev)
    {
        cbinsurance->addItem(ev->insuranceName);
    }

    cbinsurance->changed().connect(std::bind([=] () {

          InsuranceItem selected;

          for (std::vector<InsuranceItem>::const_iterator ev = insuranceitems.begin(); ev != insuranceitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cbinsurance->currentText();

            if (ev->insuranceName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.insuranceName != "")
        {
            Wt::WString patientInsuranceId_;
            Wt::WString patientInsuranceName_;

            patientInsuranceId_ = selected.insuranceId;
            patientInsuranceName_ = selected.insuranceName;

            //save insurance change to pt
            dbo::Transaction ptrans(dbsession);

            dbo::ptr<Patient> pat = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

            pat.modify()->ptInsuranceName_ = patientInsuranceName_.toUTF8();
            pat.modify()->ptInsuranceId_ = patientInsuranceId_.toUTF8();

            ptrans.commit();
            out->setText(patientInsuranceName_);

        } else {
            log("notice") << "Selected: No Insurance";
            studyInsuranceName_ = "Not Selected";
            studyInsuranceId_ = "";
        }

    }));

    cbinsurance->setCurrentIndex(0);
    cbinsurance->setMargin(10, Wt::Right);

    insurancebox->addWidget(cbinsurance);
    ptInsuranceContainer->setLayout(insurancebox);

    rightbox->addWidget(ptInsuranceContainer);

    Wt::WPushButton *deletePatient = new Wt::WPushButton("Delete Patient");
    deletePatient->setStyleClass("btn-delete-patient");

    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(deletePatient);

    deletePatient->clicked().connect(std::bind([=] () {
        dialog->accept();

        Wt::WDialog *delPtDialog = new Wt::WDialog("DELETE PATIENT");

        Wt::WContainerWidget *delPtContainer = new Wt::WContainerWidget(delPtDialog->contents());

        Wt::WText *warningDelPt = new Wt::WText("YOU ARE ABOUT TO PERMENANTLY DELETE THE PATIENT RECORD FOR: "+pfirst+" "+plast);
        Wt::WText *areSure = new Wt::WText("Are you sure this is correct?");

        Wt::WPushButton *delcancel = new Wt::WPushButton("NO - CANCEL", delPtDialog->footer());
        delcancel->setStyleClass("del-pt-cancel");
        //find sleep studies with patient id

        //display warning that patient cannot be deleted if studies.size = 0
        warningDelPt->setStyleClass("warning-del-pt");
        areSure->setStyleClass("warning-are-sure");

        delPtContainer->addWidget(warningDelPt);
        delPtContainer->addWidget(new Wt::WBreak());
        delPtContainer->addWidget(new Wt::WBreak());
        delPtContainer->addWidget(areSure);

        dbo::Transaction findsttrans(dbsession);

        typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
        Studies studies = dbsession.find<SleepStudy>().where("pt_id = ?").bind(pt);
        std::cerr << studies.size() << std::endl;

        Wt::WText *studiesFound = new Wt::WText("THERE ARE SLEEP STUDIES ASSOCIATED WITH THIS PATIENT");
        Wt::WText *studiesLower = new Wt::WText("You cannot delete the patient.");

        Wt::WPushButton *deleteNow = new Wt::WPushButton("YES - DELETE NOW", delPtDialog->footer());
        deleteNow->setStyleClass("del-pt-now-btn");
        deleteNow->show();

        if (studies.size() != 0)
        {
            warningDelPt->hide();
            areSure->hide();
            delPtContainer->addWidget(studiesFound);
            delPtContainer->addWidget(new Wt::WBreak());
            delPtContainer->addWidget(new Wt::WBreak());
            delPtContainer->addWidget(studiesLower);
            deleteNow->hide();
        }
        findsttrans.commit();

        deleteNow->clicked().connect(std::bind([=] () {
            dbo::Transaction rmtrans(dbsession);

            dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);
            patient.remove();

            rmtrans.commit();

            delPtDialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal
        }));
        delcancel->clicked().connect(std::bind([=]() {
            delPtDialog->reject();
        }));

        delPtDialog->show();

    }));

    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(modpt);

    modpt->clicked().connect(std::bind([=]() {
        Wt::WString months;
        Wt::WString days;
        Wt::WString years;
        Wt::WString dobout;

        months = dobMonth->text();
        days = dobDay->text();
        years = dobYear->text();

        dobout = months+"/"+days+"/"+years;

        std::string strFirst = patientFirstName_->text().toUTF8();
          std::string ptUpperFirst_ = boost::to_upper_copy<std::string>(strFirst);

          std::string strLast = patientLastName_->text().toUTF8();
          std::string ptUpperLast_ = boost::to_upper_copy<std::string>(strLast);

        std::string strCity = patientCity_->text().toUTF8();
          std::string ptUpperCity_ = boost::to_upper_copy<std::string>(strCity);

          std::string strHome = patientHomePhone_->text().toUTF8();
          std::string ptUpperHome_ = boost::to_upper_copy<std::string>(strHome);

        std::string strCell = patientCellPhone_->text().toUTF8();
          std::string ptUpperCell_ = boost::to_upper_copy<std::string>(strCell);

         std::string strEmail = patientEmail_->text().toUTF8();
          std::string ptUpperEmail_ = boost::to_upper_copy<std::string>(strEmail);


        dbo::Transaction mptrans(dbsession);

        dbo::ptr<Patient> mpt = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

        mpt.modify()->ptFirstName_ = ptUpperFirst_;
        mpt.modify()->ptLastName_ = ptUpperLast_;
        mpt.modify()->ptCity_ = ptUpperCity_;
        mpt.modify()->ptHomePhone_ = ptUpperHome_;
        mpt.modify()->ptCellPhone_ = ptUpperCell_;
        mpt.modify()->ptDob_ = dobout.toUTF8();
        mpt.modify()->ptEmail_ = ptUpperEmail_;
        mptrans.commit();

        dialog->accept();

        done_.emit(42, "Refresh Now"); // emit the signal

    }));

    Wt::WContainerWidget *refcontainer = new Wt::WContainerWidget();


    dbo::Transaction ptreftrans(dbsession);
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("pt_id = ?").bind(pt).orderBy("ref_received desc");
    std::cerr << referrals.size() << std::endl;

    Wt::WTable *ptreftable = new Wt::WTable();
    ptreftable->setHeaderCount(1);
    ptreftable->elementAt(0, 0)->addWidget(new Wt::WText("Referred From"));
    ptreftable->elementAt(0, 1)->addWidget(new Wt::WText("Referral Status"));
    ptreftable->elementAt(0, 2)->addWidget(new Wt::WText("Received"));
    ptreftable->elementAt(0, 3)->addWidget(new Wt::WText("Details"));


    int refrow = 1;
    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i, ++refrow)
    {

            new Wt::WText((*i)->refPracticeName_, ptreftable->elementAt(refrow, 0)),
            new Wt::WText((*i)->refStatus_, ptreftable->elementAt(refrow, 1)),
            new Wt::WText((*i)->refReceivedAt_.toString("MM/dd/yyyy"), ptreftable->elementAt(refrow, 2)),
            btnShowRef = new Wt::WPushButton("View Referral", ptreftable->elementAt(refrow, 3)),
            //click 'PDF' button on any ptreftable refrow, connect downloadReferral with that referral uuid
            btnShowRef->clicked().connect(boost::bind(&PatientListContainerWidget::showReferralDialog, this, (*i)->referralId_));

            btnShowRef->clicked().connect(std::bind([=]() {
                dialog->accept();
            }));
    }


    ptreftrans.commit();

    ptreftable->addStyleClass("table form-inline");
    ptreftable->addStyleClass("table table-striped");
    ptreftable->addStyleClass("table table-hover");

    refcontainer->addWidget(new Wt::WText("<h4>Referrals</h4>"));
    refcontainer->addWidget(new Wt::WBreak());
    refcontainer->addWidget(ptreftable);

    Wt::WContainerWidget *ptstdcontainer = new Wt::WContainerWidget();


    dbo::Transaction ptstdtrans(dbsession);
    typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
    Studies studies = dbsession.find<SleepStudy>().where("pt_id = ?").bind(pt).orderBy("std_date desc");
    std::cerr << studies.size() << std::endl;

    Wt::WTable *ptstdtable = new Wt::WTable();
    ptstdtable->setHeaderCount(1);
    ptstdtable->elementAt(0, 0)->addWidget(new Wt::WText("Study Location"));
    ptstdtable->elementAt(0, 1)->addWidget(new Wt::WText("Study Date"));
    ptstdtable->elementAt(0, 2)->addWidget(new Wt::WText("Study Status"));
    ptstdtable->elementAt(0, 3)->addWidget(new Wt::WText("Details"));


    int strow = 1;
    for (Studies::const_iterator si = studies.begin(); si != studies.end(); ++si, ++strow)
    {

            new Wt::WText((*si)->studyLocation_, ptstdtable->elementAt(strow, 0)),
            new Wt::WText((*si)->studyDate_.toString("MM/dd/yyyy"), ptstdtable->elementAt(strow, 1)),
            new Wt::WText((*si)->studyStatus_, ptstdtable->elementAt(strow, 2)),
            btnShowStd = new Wt::WPushButton("View Study", ptstdtable->elementAt(strow, 3)),
            //click 'PDF' button on any ptreftable refrow, connect downloadReferral with that referral uuid
            btnShowStd->clicked().connect(boost::bind(&PatientListContainerWidget::showSleepStudyDialog, this, (*si)->studyId_));

            btnShowStd->clicked().connect(std::bind([=]() {
                dialog->accept();
            }));
    }


    ptstdtrans.commit();

    ptstdtable->addStyleClass("table form-inline");
    ptstdtable->addStyleClass("table table-striped");
    ptstdtable->addStyleClass("table table-hover");

    ptstdcontainer->addWidget(new Wt::WText("<h4>Sleep Studies</h4>"));
    ptstdcontainer->addWidget(new Wt::WBreak());
    ptstdcontainer->addWidget(ptstdtable);


    //organize dialog into three tab widgets

    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(ptcontainer,
             "Patient Info", Wt::WTabWidget::PreLoading);
    tabW->addTab(filecontainer,
             "Patient Files", Wt::WTabWidget::PreLoading);
    tabW->addTab(notecontainer,
             "Chart Notes", Wt::WTabWidget::PreLoading);
    tabW->addTab(refcontainer,
             "Referrals", Wt::WTabWidget::PreLoading);
    tabW->addTab(ptstdcontainer,
             "Sleep Studies", Wt::WTabWidget::PreLoading);
    tabW->addTab(modifypt,
             "Edit Patient Info", Wt::WTabWidget::PreLoading);

    tabW->setStyleClass("tabwidget");
    // container->addWidget(dob);
    Wt::WPushButton *alright = new Wt::WPushButton("Ok", dialog->footer());

    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    container->show();
    dialog->show();
}

void PatientListContainerWidget::addPatientToBackup(std::string& pt)
{
    //invoke find patient modal

    staticBu_ = new Wt::WText();
    staticBu_->setText(setBuId());

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

    WString pfname = patient->ptFirstName_;
    WString plname = patient->ptLastName_;
    WString pdob = patient->ptDob_;
    WString phome = patient->ptHomePhone_;
    WString pcell = patient->ptCellPhone_;
    WString pid = patient->ptId_;

    transaction.commit();


    dbo::Transaction butrans(dbsession);

    dbo::ptr<Backup> bu = dbsession.add(new Backup());

    bu.modify()->ptFirstName_ = pfname.toUTF8();
    bu.modify()->ptLastName_ = plname.toUTF8();
    bu.modify()->ptHomePhone_ = phome.toUTF8();
    bu.modify()->ptCellPhone_ = pcell.toUTF8();
    bu.modify()->ptDob_ = pdob.toUTF8();
    bu.modify()->ptId_ = pid.toUTF8();

    bu.modify()->buId_ = staticBu_->text().toUTF8();

    butrans.commit();

}

void PatientListContainerWidget::createReferral(std::string prac, std::string pt)
{
    if (prac != "") {
        stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
        log("info") << "PatientListContainerWidget::createReferral";
        log("info") << "PatientListContainerWidget::createReferral - prac: " << prac << " , pt: " << pt;
        //create referral from information from prac and referral
        log("info") << "PatientListContainerWidget::createReferral find practice details";
        dbo::Transaction practrans(dbsession);
        dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac);

        WString pname = practice->refPracticeName_;
        WString pracid = practice->practiceId_;
        WString pzip = practice->refPracticeZip_;
        WString pdrfirst = practice->refPracticeDrFirst_;
        WString pdrlast = practice->refPracticeDrLast_;
        WString pnpi = practice->refPracticeNpi_;
        WString pemail = practice->refPracticeEmail_;
        WString pspec = practice->refSpecialty_;

        practrans.commit();

        log("info") << "PatientListContainerWidget::createReferral find patient details";
        dbo::Transaction ptrans(dbsession);
        dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

        WString ptfirst = patient->ptFirstName_;
        WString ptlast = patient->ptLastName_;
        WString ptaddr1 = patient->ptAddress1_;
        WString ptaddr2 = patient->ptAddress2_;
        WString ptcity = patient->ptCity_;
        WString ptzip = patient->ptZip_;
        WString pthome = patient->ptHomePhone_;
        WString ptcell = patient->ptCellPhone_;
        WString ptdob = patient->ptDob_;
        WString ptgender = patient->ptGender_;
        WString ptemail = patient->ptEmail_;
        WString ptid = patient->ptId_;
        WString ptinsuranceid = patient->ptInsuranceId_;
        WString ptinsurancename = patient->ptInsuranceName_;

        WString refstatus = "Study Unscheduled";
        WString status = "Study Scheduled";


        ptrans.commit();

        //upload referral attachment PDF
        staticRef_ = new Wt::WText();
        staticRef_->setText(setRefId());

        staticStd_ = new Wt::WText();
        staticStd_->setText(setStdId());

        log("info") << "PatientListContainerWidget::createReferral create dialog";

        Wt::WDialog *newrefdialog = new Wt::WDialog("Sleep Study Info for "+ptfirst+" "+ptlast);
        newrefdialog->setMinimumSize(1000, 700);
        newrefdialog->setMaximumSize(1000, 700);

        Wt::WContainerWidget *newrefcontainer = new Wt::WContainerWidget(newrefdialog->contents());
        Wt::WContainerWidget *firstcontainer = new Wt::WContainerWidget();

        Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
        firstcontainer->setLayout(hbox);
        firstcontainer->addStyleClass("attcont");

        Wt::WVBoxLayout *rvbox2 = new Wt::WVBoxLayout();
        Wt::WVBoxLayout *rvbox1 = new Wt::WVBoxLayout();


        hbox->addLayout(rvbox1);
        hbox->addLayout(rvbox2);

        Wt::WContainerWidget *refleft = new Wt::WContainerWidget(firstcontainer);



        Wt::WContainerWidget *refright = new Wt::WContainerWidget(firstcontainer);


        Wt::WPushButton *schedule = new Wt::WPushButton("Schedule Now", newrefdialog->footer());
        Wt::WPushButton *save = new Wt::WPushButton("Save for Later", newrefdialog->footer());

        Wt::WText* refout = new Wt::WText();
        refout->addStyleClass("help-block");

        Wt::WText* authout = new Wt::WText();
        authout->addStyleClass("help-block");

        if (ptinsurancename == "")
        {
            schedule->hide();
            save->hide();
            refright->addWidget(new Wt::WBreak());
            refright->addWidget(new Wt::WText("<strong>You must select an insurance provider or cash to save the referral or schedule the study.</strong>"));
            refright->addWidget(new Wt::WBreak());
        } else if (refout->text() == "") {
            schedule->hide();
            save->hide();
            refright->addWidget(new Wt::WBreak());
            refright->addWidget(new Wt::WText("<strong>Patient Insurance: </strong>" + ptinsurancename));
            refright->addWidget(new Wt::WBreak());
            refright->addWidget(new Wt::WBreak());
        }    else {
            refright->addWidget(new Wt::WBreak());
            refright->addWidget(new Wt::WText("<strong>Patient Insurance: </strong>" + ptinsurancename));
            refright->addWidget(new Wt::WBreak());
        }
        //referral received at date selection widget.
        Wt::WCalendar *authcal = new Wt::WCalendar();
        authcal->setSelectionMode(Wt::ExtendedSelection);

        authcal->selectionChanged().connect(std::bind([=]() {
            Wt::WString aselected;
            std::set<Wt::WDate> aselection = authcal->selection();

            for (std::set<Wt::WDate>::const_iterator ait = aselection.begin();
                ait != aselection.end(); ++ait) {
                if (!aselected.empty())
                    aselected += ", ";

                const Wt::WDate& ad = *ait;
                aselected = ad.toString("MM/dd/yyyy");
            }

            authout->setText(Wt::WString::fromUTF8
                ("{1}")
                .arg(aselected));
        }));
        Wt::WText *refreceivedwarning = new Wt::WText("<strong>You must select the Referral Received Date before you can Schedule</strong>", newrefdialog->footer());
        refreceivedwarning->setStyleClass("ref-received-warning");

        //referral received at date selection widget.
        Wt::WCalendar *refcal = new Wt::WCalendar();
        refcal->setSelectionMode(Wt::ExtendedSelection);



        refcal->selectionChanged().connect(std::bind([=]() {
            refreceivedwarning->hide();
            Wt::WString selected;
            std::set<Wt::WDate> selection = refcal->selection();

            for (std::set<Wt::WDate>::const_iterator it = selection.begin();
                it != selection.end(); ++it) {
                if (!selected.empty())
                    selected += ", ";

                const Wt::WDate& d = *it;
                selected = d.toString("MM/dd/yyyy");
            }

            refout->setText(Wt::WString::fromUTF8
                ("{1}")
                .arg(selected));
            save->show();
            schedule->show();
        }));



        dbReferralDate_ = new Wt::WText();
        dbAuthorizationDate_ = new Wt::WText();


        Wt::WContainerWidget *insuranceContainer = new Wt::WContainerWidget();

        Wt::WLabel *lblInsurance = new Wt::WLabel("Patient Insurance:");
        lblInsurance->setStyleClass("sig-lbl");

        Wt::WHBoxLayout *insurancebox = new Wt::WHBoxLayout();
        insurancebox->addWidget(lblInsurance);

        dbo::Transaction instrans(dbsession);

        typedef dbo::collection< dbo::ptr<Insurance> > Insurances;
        Insurances insurances = dbsession.find<Insurance>();
        std::cerr << insurances.size() << std::endl;

        std::vector<InsuranceItem> insuranceitems;

        for (Insurances::const_iterator i = insurances.begin(); i != insurances.end(); ++i)
        {

            Wt::WString name = (*i)->insuranceName_;
            Wt::WString id = (*i)->insuranceId_;


            InsuranceItem *e = new InsuranceItem();
            e->insuranceName = name;
            e->insuranceId = id;


            insuranceitems.push_back(*e);

        }
        log("notice") << "Current Number of insurance items "<< insuranceitems.size();

        instrans.commit();



        Wt::WComboBox *cbinsurance = new Wt::WComboBox();
        cbinsurance->setStyleClass("sig-left-box");
        Wt::WText *insuranceout = new Wt::WText();
        cbinsurance->addItem("");

        for (std::vector<InsuranceItem>::const_iterator ev = insuranceitems.begin(); ev != insuranceitems.end(); ++ev)
        {
            cbinsurance->addItem(ev->insuranceName);
        }

        cbinsurance->changed().connect(std::bind([=] () {

              InsuranceItem selected;

              for (std::vector<InsuranceItem>::const_iterator ev = insuranceitems.begin(); ev != insuranceitems.end(); ++ev)
            {
                Wt::WString cbname;
                cbname = cbinsurance->currentText();

                if (ev->insuranceName == cbname)
                {
                    selected = *ev;
                }
            }
            if (selected.insuranceName != "")
            {

                studyInsuranceId_ = selected.insuranceId;
                studyInsuranceName_ = selected.insuranceName;

                //save insurance change to pt
                dbo::Transaction ptrans(dbsession);

                dbo::ptr<Patient> pat = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

                pat.modify()->ptInsuranceName_ = studyInsuranceName_.toUTF8();
                pat.modify()->ptInsuranceId_ = studyInsuranceId_.toUTF8();

                ptrans.commit();

            } else {
                log("notice") << "Selected: No Insurance";
                studyInsuranceName_ = "Not Selected";
                studyInsuranceId_ = "";
            }

        }));

        cbinsurance->setCurrentIndex(0);
        cbinsurance->setMargin(10, Wt::Right);

        insurancebox->addWidget(cbinsurance);
        insuranceContainer->setLayout(insurancebox);

        Wt::WContainerWidget *typeContainer = new Wt::WContainerWidget();

        Wt::WLabel *lblStudyType = new Wt::WLabel("Study Type:");
        lblStudyType->setStyleClass("sig-lbl");

        Wt::WHBoxLayout *typebox = new Wt::WHBoxLayout();
        typebox->addWidget(lblStudyType);

        dbo::Transaction typetrans(dbsession);

        typedef dbo::collection< dbo::ptr<StudyType> > StudyTypes;
        StudyTypes types = dbsession.find<StudyType>();
        std::cerr << types.size() << std::endl;

        std::vector<StudyTypeItem> typeitems;

        for (StudyTypes::const_iterator i = types.begin(); i != types.end(); ++i)
        {

            Wt::WString name = (*i)->studyTypeName_;
            Wt::WString code = (*i)->studyTypeCode_;
            Wt::WString id = (*i)->studyTypeId_;


            StudyTypeItem *e = new StudyTypeItem();
            e->studyTypeName = name;
            e->studyTypeCode = code;
            e->studyTypeId = id;


            typeitems.push_back(*e);

        }
        log("notice") << "Current Number of type items "<< typeitems.size();

        typetrans.commit();



        Wt::WComboBox *cbtype = new Wt::WComboBox();
        cbtype->setStyleClass("sig-left-box");
        Wt::WText *typeout = new Wt::WText();
        cbtype->addItem("");

        for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
        {
            cbtype->addItem(ev->studyTypeName);
        }

        cbtype->changed().connect(std::bind([=] () {

              StudyTypeItem selected;

            for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
            {
                Wt::WString cbname;
                cbname = cbtype->currentText();

                if (ev->studyTypeName == cbname)
                {
                    selected = *ev;
                }
            }
            if (selected.studyTypeName != "")
            {

                newStudyTypeId_ = selected.studyTypeId;
                newStudyTypeName_ = selected.studyTypeName;
                newStudyTypeCode_ = selected.studyTypeCode;


            } else {
                log("notice") << "Selected: No StudyType";
                newStudyTypeId_ = "";
                newStudyTypeName_ = "No Type Selected";
                newStudyTypeCode_ = "No Type Code";
            }

        }));

        cbtype->setCurrentIndex(0);
        cbtype->setMargin(10, Wt::Right);

        typebox->addWidget(cbtype);
        typeContainer->setLayout(typebox);


        refleft->addWidget(new Wt::WBreak());
        refleft->addWidget(typeContainer);
        refleft->addWidget(insuranceContainer);
        refleft->addWidget(new Wt::WBreak());
        refleft->addWidget(new Wt::WText("<h5>Referral Received</h5>"));
        refleft->addWidget(new Wt::WBreak());
        refleft->addWidget(refcal);
        refleft->addWidget(refout);

        refright->addWidget(new Wt::WBreak());
        refright->addWidget(new Wt::WText("<h5>Authorization Expires</h5>"));
        refright->addWidget(new Wt::WBreak());
        refright->addWidget(authcal);
        refright->addWidget(authout);

        rvbox1->addWidget(refleft);
        rvbox2->addWidget(refright);

        newrefcontainer->show();


        Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newrefdialog->footer());
        newrefdialog->rejectWhenEscapePressed();

        cancel->clicked().connect(newrefdialog, &Wt::WDialog::reject);


        save->clicked().connect(std::bind([=]() {

            dbAuthorizationDate_->setText(authout->text());
            Wt::WDate dbAuthDate;
            dbAuthDate = Wt::WDate().fromString(dbAuthorizationDate_->text().toUTF8(), "MM/dd/yyyy");
            
            dbReferralDate_->setText(refout->text());
            Wt::WDate dbReferralDateD;
            dbReferralDateD = Wt::WDate().fromString(dbReferralDate_->text().toUTF8(), "MM/dd/yyyy");

            Wt::WString refinbound = "staff";

            dbo::Transaction reftrans(dbsession);

            dbo::ptr<Referral> r = dbsession.add(new Referral());
            r.modify()->refPracticeName_ = pname.toUTF8();
            r.modify()->refPracticeZip_ = pzip.toUTF8();
            r.modify()->refPracticeDrFirst_ = pdrfirst.toUTF8();
            r.modify()->refPracticeDrLast_ = pdrlast.toUTF8();
            r.modify()->refPracticeNpi_ = pnpi.toUTF8();
            r.modify()->refSpecialty_ = pspec.toUTF8();
            r.modify()->refPracticeEmail_ = pemail.toUTF8();
            r.modify()->refPracticeId_ = prac;

            r.modify()->refFirstName_ = ptfirst.toUTF8();
            r.modify()->refLastName_ = ptlast.toUTF8();
            r.modify()->refAddress1_ = ptaddr1.toUTF8();
            r.modify()->refAddress2_ = ptaddr2.toUTF8();
            r.modify()->refCity_ = ptcity.toUTF8();
            r.modify()->refZip_ = ptzip.toUTF8();
            r.modify()->refHomePhone_ = pthome.toUTF8();
            r.modify()->refCellPhone_ = ptcell.toUTF8();
            r.modify()->refEmail_ = ptemail.toUTF8();
            r.modify()->refDob_ = ptdob.toUTF8();
            r.modify()->refGender_ = ptgender.toUTF8();
            r.modify()->refInbound_ = refinbound.toUTF8();
            r.modify()->refReceivedAt_ = dbReferralDateD;
            r.modify()->authExpiration_ = dbAuthDate;
            r.modify()->patientId_ = pt;
            //check if ptinsurancename == null
            //if it is, use the studyInsuranceName, studyInsuranceId
            //else, use ptinsuranceid
            if (ptinsuranceid == "")
            {
                r.modify()->refInsuranceId_ = studyInsuranceId_.toUTF8();
                r.modify()->refInsuranceName_ = studyInsuranceName_.toUTF8();
            } else {
                r.modify()->refInsuranceId_ = ptinsuranceid.toUTF8();
                r.modify()->refInsuranceName_ = ptinsurancename.toUTF8();
            }
            r.modify()->studyTypeId_ = newStudyTypeId_.toUTF8();
            r.modify()->studyTypeName_ = newStudyTypeName_.toUTF8();
            r.modify()->studyTypeCode_ = newStudyTypeCode_.toUTF8();

            r.modify()->refStatus_ = refstatus.toUTF8();
            r.modify()->referralId_ = staticRef_->text().toUTF8();

            reftrans.commit();


            newrefdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal
        }));

        schedule->clicked().connect(std::bind([=]() {
            dbAuthorizationDate_->setText(authout->text());
            Wt::WDate dbAuthDate;
            dbAuthDate = Wt::WDate().fromString(dbAuthorizationDate_->text().toUTF8(), "MM/dd/yyyy");
            
            dbReferralDate_->setText(refout->text());
            Wt::WDate dbReferralDateD;
            dbReferralDateD = Wt::WDate().fromString(dbReferralDate_->text().toUTF8(), "MM/dd/yyyy");

            dbo::Transaction reftrans(dbsession);

            dbo::ptr<Referral> r = dbsession.add(new Referral());

            r.modify()->refPracticeName_ = pname.toUTF8();
            r.modify()->refPracticeZip_ = pzip.toUTF8();
            r.modify()->refPracticeDrFirst_ = pdrfirst.toUTF8();
            r.modify()->refPracticeDrLast_ = pdrlast.toUTF8();
            r.modify()->refPracticeNpi_ = pnpi.toUTF8();
            r.modify()->refSpecialty_ = pspec.toUTF8();
            r.modify()->refPracticeEmail_ = pemail.toUTF8();
            r.modify()->refPracticeId_ = pracid.toUTF8();

            r.modify()->refFirstName_ = ptfirst.toUTF8();
            r.modify()->refLastName_ = ptlast.toUTF8();
            r.modify()->refAddress1_ = ptaddr1.toUTF8();
            r.modify()->refAddress2_ = ptaddr2.toUTF8();
            r.modify()->refCity_ = ptcity.toUTF8();
            r.modify()->refZip_ = ptzip.toUTF8();
            r.modify()->refHomePhone_ = pthome.toUTF8();
            r.modify()->refCellPhone_ = ptcell.toUTF8();
            r.modify()->refEmail_ = ptemail.toUTF8();
            r.modify()->refDob_ = ptdob.toUTF8();
            r.modify()->refGender_ = ptgender.toUTF8();
            r.modify()->refReceivedAt_ = dbReferralDateD;
            r.modify()->authExpiration_ = dbAuthDate;
            r.modify()->patientId_ = pt;

            //check if ptinsurancename == null
            //if it is, use the studyInsuranceName, studyInsuranceId
            //else, use ptinsuranceid
            if (ptinsuranceid == "")
            {
                r.modify()->refInsuranceId_ = studyInsuranceId_.toUTF8();
                r.modify()->refInsuranceName_ = studyInsuranceName_.toUTF8();
            } else {
                r.modify()->refInsuranceId_ = ptinsuranceid.toUTF8();
                r.modify()->refInsuranceName_ = ptinsurancename.toUTF8();
            }

            r.modify()->studyTypeId_ = referralTypeId_.toUTF8();
            r.modify()->studyTypeName_ = referralTypeName_.toUTF8();
            r.modify()->studyTypeCode_ = referralTypeCode_.toUTF8();
            r.modify()->refStatus_ = refstatus.toUTF8();
            r.modify()->referralId_ = staticRef_->text().toUTF8();

            reftrans.commit();

            newrefdialog->accept();

            Wt::WDialog *stdialog = new Wt::WDialog("Schedule Sleep Study for "+ ptfirst+" "+ptlast);
            stdialog->setMinimumSize(1000, 700);
            stdialog->setMaximumSize(1000, 700);

            Wt::WContainerWidget *cont= new Wt::WContainerWidget(stdialog->contents());

            Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
            cont->setLayout(hbox);

            Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
            hbox->addLayout(vbox1);

            Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
            hbox->addLayout(vbox2);

            Wt::WLabel *lblLocation = new Wt::WLabel("Sleep Study Location:");
            lblLocation->setStyleClass("sig-lbl");
            Wt::WHBoxLayout *locobox = new Wt::WHBoxLayout();
            locobox->addWidget(lblLocation);

            Wt::WComboBox *cbloco = new Wt::WComboBox();
            cbloco->setStyleClass("sig-left-box");

            cbloco->addItem("none selected");
            cbloco->addItem("Long Beach");
            cbloco->addItem("Downey");
            cbloco->addItem("Home Study");
            cbloco->setCurrentIndex(0);
            cbloco->setMargin(10, Wt::Right);

            Wt::WText *locoout = new Wt::WText();
            locoout->addStyleClass("help-block");

            cbloco->changed().connect(std::bind([=] () {
                locoout->setText(Wt::WString::fromUTF8("{1}")
                     .arg(cbloco->currentText()));
            }));

            locobox->addWidget(cbloco);
            vbox1->addLayout(locobox);

            Wt::WLabel *lblTime = new Wt::WLabel("Study Appointment Time:");
            lblTime->setStyleClass("sig-lbl");
            Wt::WHBoxLayout *timebox = new Wt::WHBoxLayout();
            timebox->addWidget(lblTime);

            Wt::WComboBox *cbtime = new Wt::WComboBox();
            cbtime->setStyleClass("sig-left-box");

            cbtime->addItem("none selected");
            cbtime->addItem("7:00 PM");
            cbtime->addItem("7:30 PM");
            cbtime->addItem("8:00 PM");
            cbtime->addItem("8:30 PM");
            cbtime->addItem("9:00 PM");
            cbtime->addItem("9:30 PM");
            cbtime->addItem("10:00 PM");
            cbtime->addItem("10:30 PM");
            cbtime->addItem("11:00 PM");
            cbtime->addItem("11:30 PM");
            cbtime->addItem("12:00 PM");
            cbtime->setCurrentIndex(0);
            cbtime->setMargin(10, Wt::Right);

            Wt::WText *timeout = new Wt::WText();
            timeout->addStyleClass("help-block");

            cbtime->changed().connect(std::bind([=] () {
                timeout->setText(Wt::WString::fromUTF8("{1}")
                     .arg(cbtime->currentText()));
            }));

            timebox->addWidget(cbtime);
            vbox1->addLayout(timebox);
            // Wt::WContainerWidget *typeContainer = new Wt::WContainerWidget();

            // Wt::WLabel *lblStudyType = new Wt::WLabel("Study Type:");
            // lblStudyType->setStyleClass("sig-lbl");

            // Wt::WHBoxLayout *typebox = new Wt::WHBoxLayout();
            // typebox->addWidget(lblStudyType);

            // dbo::Transaction typetrans(dbsession);

            // typedef dbo::collection< dbo::ptr<StudyType> > StudyTypes;
            // StudyTypes types = dbsession.find<StudyType>();
            // std::cerr << types.size() << std::endl;

            // std::vector<StudyTypeItem> typeitems;

            // for (StudyTypes::const_iterator i = types.begin(); i != types.end(); ++i)
            // {

            //     Wt::WString name = (*i)->studyTypeName_;
            //     Wt::WString code = (*i)->studyTypeCode_;
            //     Wt::WString id = (*i)->studyTypeId_;


            //     StudyTypeItem *e = new StudyTypeItem();
            //     e->studyTypeName = name;
            //     e->studyTypeCode = code;
            //     e->studyTypeId = id;


            //     typeitems.push_back(*e);

            // }
            // log("notice") << "Current Number of type items "<< typeitems.size();

            // typetrans.commit();



            // Wt::WComboBox *cbtype = new Wt::WComboBox();
            // cbtype->setStyleClass("sig-left-box");
            // Wt::WText *typeout = new Wt::WText();
            // cbtype->addItem("");

            // for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
            // {
            //     cbtype->addItem(ev->studyTypeName);
            // }

            // cbtype->changed().connect(std::bind([=] () {

            //       StudyTypeItem selected;

            //       for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
            //     {
            //         Wt::WString cbname;
            //         cbname = cbtype->currentText();

            //         if (ev->studyTypeName == cbname)
            //         {
            //             selected = *ev;
            //         }
            //     }
            //     if (selected.studyTypeName != "")
            //     {

            //         newStudyTypeId_ = selected.studyTypeId;
            //         newStudyTypeName_ = selected.studyTypeName;
            //         newStudyTypeCode_ = selected.studyTypeCode;


            //     } else {
            //         log("notice") << "Selected: No StudyType";
            //         newStudyTypeId_ = "";
            //         newStudyTypeName_ = "No Type Selected";
            //         newStudyTypeCode_ = "No Type Code";
            //     }

            // }));

            // cbtype->setCurrentIndex(0);
            // cbtype->setMargin(10, Wt::Right);

            // typebox->addWidget(cbtype);
            // typeContainer->setLayout(typebox);

            // vbox1->addWidget(typeContainer);

            Wt::WCalendar *c2 = new Wt::WCalendar();
            c2->setSelectionMode(Wt::ExtendedSelection);

            Wt::WText* out = new Wt::WText();
            out->addStyleClass("help-block");

            c2->selectionChanged().connect(std::bind([=]() {
                Wt::WString selected;
                std::set<Wt::WDate> selection = c2->selection();

                for (std::set<Wt::WDate>::const_iterator it = selection.begin();
                    it != selection.end(); ++it) {
                    if (!selected.empty())
                        selected += ", ";

                    const Wt::WDate& d = *it;
                    selected = d.toString("MM/dd/yyyy");
                }

                out->setText(Wt::WString::fromUTF8
                    ("{1}")
                    .arg(selected));
            }));
            vbox2->addWidget(c2);
            vbox2->addWidget(out);

            dbStudyLocation_ = new Wt::WText();
            dbStudyTime_ = new Wt::WText();
            dbStudyDate_ = new Wt::WText();


            cont->show();

            Wt::WPushButton *schedulenow = new Wt::WPushButton("Schedule", stdialog->footer());
            schedulenow->setDefault(true);

            Wt::WPushButton *cl = new Wt::WPushButton("Cancel", stdialog->footer());
            stdialog->rejectWhenEscapePressed();
            schedulenow->clicked().connect(std::bind([=]() {
                stdialog->accept();
                dbStudyLocation_->setText(locoout->text());
                dbStudyTime_->setText(timeout->text());
                dbStudyDate_->setText(out->text());

                Wt::WDate dbStudyDateD;
                dbStudyDateD = Wt::WDate().fromString(dbStudyDate_->text().toUTF8(), "MM/dd/yyyy");


                //create new study here
                dbo::Transaction strans(dbsession);
                dbo::ptr<SleepStudy> st = dbsession.add(new SleepStudy());

                st.modify()->ptFirstName_ = ptfirst.toUTF8();
                st.modify()->ptLastName_ = ptlast.toUTF8();
                st.modify()->ptAddress1_ = ptaddr1.toUTF8();
                st.modify()->ptAddress2_ = ptaddr2.toUTF8();
                st.modify()->ptCity_ = ptcity.toUTF8();
                st.modify()->ptZip_ = ptzip.toUTF8();
                st.modify()->ptHomePhone_ = pthome.toUTF8();
                st.modify()->ptCellPhone_ = ptcell.toUTF8();
                st.modify()->ptEmail_ = pemail.toUTF8();
                st.modify()->ptDob_ = ptdob.toUTF8();
                st.modify()->ptGender_ = ptgender.toUTF8();

                st.modify()->studyStatus_ = status.toUTF8();
                st.modify()->studyDate_ = dbStudyDateD;
                st.modify()->studyTime_ = dbStudyTime_->text().toUTF8();
                st.modify()->studyLocation_ = dbStudyLocation_->text().toUTF8();
                st.modify()->studyId_ = staticStd_->text().toUTF8();
                st.modify()->referralId_ = staticRef_->text().toUTF8();
                st.modify()->practiceId_ = pracid.toUTF8();
                st.modify()->patientId_ = pt;
                
                st.modify()->studyTypeId_ = newStudyTypeId_.toUTF8();
                st.modify()->studyTypeName_ = newStudyTypeName_.toUTF8();
                st.modify()->studyTypeCode_ = newStudyTypeCode_.toUTF8();

                strans.commit();

                //referral status update
                dbo::Transaction referraltrans(dbsession);
                dbo::ptr<Referral> refer = dbsession.find<Referral>().where("ref_id = ?").bind(staticRef_->text().toUTF8());
                refer.modify()->refStatus_ = status.toUTF8();
                referraltrans.commit();

                dbo::Transaction pctrans(dbsession);
                dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);

                WString poptmail = pc->pracEmailOpt_;

                //send email to practice
                   pctrans.commit();

                   if (poptmail == "Yes") {
                       Mail::Message message;
                    message.setFrom(Mail::Mailbox("info@statsleep.com", "United STAT Sleep"));
                    message.addRecipient(Mail::To, Mail::Mailbox(pemail.toUTF8(), pname.toUTF8(    )));
                    message.setSubject("A Sleep Study has been Scheduled");
                    message.setBody("You referred a patient to our sleep center, and their sleep study has been scheduled. Please login to your physician account to check details.");
                    message.addHtmlBody ("<p>Hi, "+pname.toUTF8()+"<br>You referred a patient to our sleep center, and their sleep study has been scheduled.<br> Please <a href=\"https://127.0.0.1/providers\">Log In to your Physician Account</a> to check the sleep study details and track progress.</p><br><p>Thanks,</p><p>The United STAT Sleep Team</p>");
                    Mail::Client client;
                    client.connect("localhost");
                    client.send(message);
                   }

                done_.emit(42, "Refresh Now"); // emit the signal
            }));

            cl->clicked().connect(stdialog, &Wt::WDialog::reject);

            stdialog->show();

        }));
        
        Wt::WContainerWidget *uploadcontainer = new Wt::WContainerWidget();

        composer_ = new Composer(conninfo_, staticRef_->text(), this);

        uploadcontainer->addWidget(new Wt::WText("<h4>Upload Referral</h4>"));
        uploadcontainer->addWidget(new Wt::WBreak());
        uploadcontainer->addWidget(new Wt::WText("Upload the Referral or Order PDF or image from the efax system!"));
        uploadcontainer->addWidget(new Wt::WBreak());
        uploadcontainer->addWidget(composer_);



        Wt::WContainerWidget *refinfo = new Wt::WContainerWidget();
        Wt::WHBoxLayout *infohbox = new Wt::WHBoxLayout();
        refinfo->setLayout(infohbox);
        refinfo->addStyleClass("attcont");

        Wt::WVBoxLayout *infovbox1 = new Wt::WVBoxLayout();
        Wt::WVBoxLayout *infovbox2 = new Wt::WVBoxLayout();


        infohbox->addLayout(infovbox1);
        infohbox->addLayout(infovbox2);

        Wt::WContainerWidget *infoleft = new Wt::WContainerWidget(refinfo);
        Wt::WContainerWidget *inforight = new Wt::WContainerWidget(refinfo);

        infoleft->addStyleClass("refdiaginfoleft");
        infoleft->addWidget(new Wt::WText("<h4>Referring Practice Info</h4>"));
        infoleft->addWidget(new Wt::WBreak());
        infoleft->addWidget(new Wt::WText("Practice Name: " + pname));
        infoleft->addWidget(new Wt::WBreak());
        infoleft->addWidget(new Wt::WText("Practice Zip: " + pzip));
        infoleft->addWidget(new Wt::WBreak());
        infoleft->addWidget(new Wt::WText("Dr Full Name: " + pdrfirst + " " + pdrlast));
        // infoleft->addWidget(new Wt::WBreak());
        // infoleft->addWidget(new Wt::WText("Practice NPI: " + pnpi));
        // infoleft->addWidget(new Wt::WBreak());
        // infoleft->addWidget(new Wt::WText("Practice Specialty: " + pspec));
        infoleft->addWidget(new Wt::WBreak());
        infoleft->addWidget(new Wt::WText("Practice Email: " + pemail));
        inforight->addStyleClass("refdiaginforight");
        inforight->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
        inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
        inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
        inforight->addWidget(new Wt::WBreak());
        // inforight->addWidget(new Wt::WText("<strong>Patient Address: <br /></strong>" + ptaddr1));
        // inforight->addWidget(new Wt::WText(ptaddr2));
        // inforight->addWidget(new Wt::WBreak());
        // inforight->addWidget(new Wt::WText("<strong>Patient City: </strong>" + ptcity));
        // inforight->addWidget(new Wt::WBreak());
        // inforight->addWidget(new Wt::WText("<strong>Patient Zip: </strong>" + ptzip));
        // inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient Home Phone: </strong>" + pthome));
        inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient Cell Phone: </strong>" + ptcell));
        inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
        inforight->addWidget(new Wt::WBreak());
        inforight->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));

        infovbox1->addWidget(infoleft);
        infovbox2->addWidget(inforight);
        
        firstcontainer->setStyleClass("height-limit-container");
        refinfo->setStyleClass("height-limit-container");


        Wt::WTabWidget *tabW = new Wt::WTabWidget(newrefdialog->contents());
        tabW->addTab(firstcontainer,
                 "Referral Info", Wt::WTabWidget::PreLoading);
        tabW->addTab(uploadcontainer,
                 "Upload Referral", Wt::WTabWidget::PreLoading);
        tabW->addTab(refinfo,
                 "Patient and Physician Info", Wt::WTabWidget::PreLoading);

        tabW->setStyleClass("tabwidget");


        newrefdialog->show();
        //save referral to database

    } else {
        // Wt::WMessageBox *messageBox = new Wt::WMessageBox
        // ("Cannot Start Referral",
        //  "<p>It seems the referring physician record is missing from this patient.</p>"
        //  "<p>Try deleting the patient and adding them again, this usually fixes the problem.</p>"
        //  "<p>If that does not work, please contact technical support.</p>",
        //  Wt::Information, Wt::Ok);

        // messageBox->setModal(false);

        // messageBox->buttonClicked().connect(std::bind([=] () {
        //     delete messageBox;
        // }));

        // messageBox->show();

        PatientListContainerWidget::selectPracticeDialog(pt);



    }
    
    //start sleep study from referral id
}

void PatientListContainerWidget::selectPracticeDialog(Wt::WString pt)
{

    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    Wt::WDialog *newstdialog = new Wt::WDialog("PROVIDER RECORD NOT FOUND");

    newstdialog->setMinimumSize(1000, 700);
    newstdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newstcontainer = new Wt::WContainerWidget(newstdialog->contents());
    newstcontainer->setStyleClass("new-pt-provider-list");
    newstcontainer->addWidget(new Wt::WText("<h3 style=\"color: #C9302C;\">The provider record was not found for this patient.</h3>"));
    newstcontainer->addWidget(new Wt::WText("<h4 style=\"color: #C9302C;\">Please select the provider that referred this patient from the list below to continue.</h4>"));
    newstcontainer->addWidget(new Wt::WText("<h5>If you continue to receive this message for this patient, please contact technical support.</h5>"));
    newstcontainer->addWidget(new Wt::WBreak());

    //display list of Providers from providers database
    dbo::Transaction showprtrans(dbsession);

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>();

    std::cerr << practices.size() << std::endl;

    Wt::WTable *showptable = new Wt::WTable();
    showptable->setHeaderCount(1);

    showptable->elementAt(0, 0)->addWidget(new Wt::WText("Practice Name"));
    showptable->elementAt(0, 1)->addWidget(new Wt::WText("Dr First Name"));
    showptable->elementAt(0, 2)->addWidget(new Wt::WText("Dr Last Name"));
    showptable->elementAt(0, 3)->addWidget(new Wt::WText("Zip Code"));
    showptable->elementAt(0, 4)->addWidget(new Wt::WText(""));

    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {
        std::string pracid = (*i)->practiceId_;

        new Wt::WText((*i)->refPracticeName_, showptable->elementAt(row, 0)),
        new Wt::WText((*i)->refPracticeDrFirst_, showptable->elementAt(row, 1)),
        new Wt::WText((*i)->refPracticeDrLast_, showptable->elementAt(row, 2)),
        new Wt::WText((*i)->refPracticeZip_, showptable->elementAt(row, 3)),

        btnSelectPractice = new Wt::WPushButton("Select", showptable->elementAt(row, 6)),
        //select Practice id, trigger newReferral function
        // btnSelectPractice->clicked().connect(boost::bind(&PatientListContainerWidget::createReferral, this, (*i)->practiceId_, pt.toUTF8()));
        btnSelectPractice->clicked().connect(std::bind([=]() {
            dbo::Transaction mptrans(dbsession);

            dbo::ptr<Patient> mpt = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

            mpt.modify()->pracId_ = pracid;
            mptrans.commit();

            newstdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

        }));
    }

    showprtrans.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");

    newstcontainer->addWidget(showptable);

    //allow user to select a provider with select button

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newstdialog->footer());
    newstdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();


}

void PatientListContainerWidget::showReferralDialog(Wt::WString& ref)
{

    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Sleep Study Referral");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Referral> referral = dbsession.find<Referral>().where("ref_id = ?").bind(ref);

    WString pname = referral->refPracticeName_;
    WString pracid = referral->refPracticeId_;
    WString pzip = referral->refPracticeZip_;
    WString pdrfirst = referral->refPracticeDrFirst_;
    WString pdrlast = referral->refPracticeDrLast_;
    WString pnpi = referral->refPracticeNpi_;
    WString pemail = referral->refPracticeEmail_;
    WString pspec = referral->refSpecialty_;

    WString ptfirst = referral->refFirstName_;
    WString ptlast = referral->refLastName_;
    WString ptaddr1 = referral->refAddress1_;
    WString ptaddr2 = referral->refAddress2_;
    WString ptcity = referral->refCity_;
    WString ptzip = referral->refZip_;
    WString pthome = referral->refHomePhone_;
    WString ptcell = referral->refCellPhone_;
    WString ptdob = referral->refDob_;
    WString ptgender = referral->refGender_;
    WString rdiag = referral->refDiagnostic_;
    WString rover = referral->refOvernight_;
    WString rhome = referral->refHomeStudy_;
    WString rother = referral->refOtherSleepTest_;
    WString rauth = referral->refAuthConsult_;
    WString rptid = referral->patientId_;

    WString signame = referral->enteredDrFullName_;
    WString sigemail = referral->enteredDrEmail_;
    WString sigdate = referral->selectedDate_;
    WString ptinsur = referral->refInsuranceName_;
    WString refdate = referral->refReceivedAt_.toString("MM/dd/yyyy");

    WString status = "";

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *hbox2 = new Wt::WHBoxLayout();
    container->setLayout(hbox);
    container->addStyleClass("attcont");

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WContainerWidget *left = new Wt::WContainerWidget(dialog->contents());
    left->addStyleClass("refdiagleft");
    left->addWidget(new Wt::WText("<h4>Referring Practice Info</h4>"));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Name: " + pname));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Zip: " + pzip));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Dr Full Name: " + pdrfirst + " " + pdrlast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice NPI: " + pnpi));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Specialty: " + pspec));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Email: " + pemail));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Referred On: " + refdate));


    Wt::WContainerWidget *right = new Wt::WContainerWidget(dialog->contents());
    right->addStyleClass("refdiagright");
    right->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Address: <br /></strong>" + ptaddr1));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText(ptaddr2));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient City: </strong>" + ptcity));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Zip: </strong>" + ptzip));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Home Phone: </strong>" + pthome));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Cell Phone: </strong>" + ptcell));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<h4>Referral Diagnosis</h4> "));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Diagnosis: </strong>" + rdiag));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Overnight Study: </strong>" + rover));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Other Study: </strong>" + rother));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Home Study: </strong>" + rhome));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<strong>Patient Consultation: </strong><br /> " +rauth));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("Current Status:"+ status));
    right->addWidget(new Wt::WBreak());
    right->addWidget(new Wt::WText("Patient Insurance:"+ ptinsur));
    right->addWidget(new Wt::WBreak());

    right->addWidget(new Wt::WBreak());

    //put table here
    dbo::Transaction atrans(dbsession);

    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(ref);
    std::cerr << attachments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Download"));

    int row = 1;
    for (Attachments::const_iterator i = attachments.begin(); i != attachments.end(); ++i, ++row)
    {

        Wt::WResource *attc = new AttachmentResource((*i)->attId_, (*i)->tempRefId_, (*i)->attFileName_, (*i)->attContentDescription_, this);

        new Wt::WText((*i)->attFileName_, table->elementAt(row, 0)),
        downDone = new Wt::WText("Downloaded", table->elementAt(row, 1)),
        downDone->hide(),
        btnDownloadAtt = new Wt::WPushButton("Download", table->elementAt(row, 1)),

        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");


    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    Wt::WTemplate *t = new Wt::WTemplate(Wt::WString::tr("template.referral"));

    t->bindString("pname", Wt::WString(pname));
    t->bindString("pzip", Wt::WString(pzip));
    t->bindString("pdrfirst", Wt::WString(pdrfirst));
    t->bindString("pdrlast", Wt::WString(pdrlast));
    t->bindString("pnpi", Wt::WString(pnpi));
    t->bindString("pemail", Wt::WString(pemail));
    t->bindString("pspec", Wt::WString(pspec));
    t->bindString("ptfirst", Wt::WString(ptfirst));
    t->bindString("ptlast", Wt::WString(ptlast));
    t->bindString("ptaddr1", Wt::WString(ptaddr1));
    t->bindString("ptaddr2", Wt::WString(ptaddr2));
    t->bindString("ptcity", Wt::WString(ptcity));
    t->bindString("ptzip", Wt::WString(ptzip));
    t->bindString("pthome", Wt::WString(pthome));

    t->bindString("ptcell", Wt::WString(ptcell));
    t->bindString("ptdob", Wt::WString(ptdob));
    t->bindString("ptgender", Wt::WString(ptgender));
    t->bindString("rdiag", Wt::WString(rdiag));
    t->bindString("rover", Wt::WString(rover));
    t->bindString("rhome", Wt::WString(rhome));
    t->bindString("rother", Wt::WString(rother));
    t->bindString("rauth", Wt::WString(rauth));

    t->bindString("signame", Wt::WString(signame));
    t->bindString("sigemail", Wt::WString(sigemail));
    t->bindString("sigdate", Wt::WString(sigdate));

    std::ostringstream result;
    t->renderTemplate(result);
    Wt::WString s = result.str();

    /*
    WPushButton *pdfBtn = new Wt::WPushButton("Download PDF");
    Wt::WResource *pdf = new ReferralResource(this);
    pdfBtn->setLink(pdf);
    */

    //had to uncomment this because HPDF isnt working at the moment


    Wt::WResource *pdf = new ReferralResource(s, this);

    Wt::WPushButton *button = new Wt::WPushButton("Download Referral PDF");
    button->setLink(pdf);


    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<h4>Attachments</h4>"));
    left->addWidget(table);
    left->addWidget(new Wt::WBreak());
    left->addWidget(button);

    vbox1->addWidget(left);
    vbox2->addWidget(right);

    container->show();
    dialog->show();
}

//show sleep study dialog
void PatientListContainerWidget::showSleepStudyDialog(WString& std)
{

    //show sleep study dialog
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction getstudy(dbsession);
    dbo::ptr<SleepStudy> stdy = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);

    WString ref = stdy->referralId_;

    staticStd_ = new Wt::WText();
    staticStd_->setText(setStdId());

    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Sleep Study Referral");

    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
    Wt::WContainerWidget *refcontainer = new Wt::WContainerWidget();
    Wt::WContainerWidget *stdcontainer = new Wt::WContainerWidget();

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    refcontainer->setLayout(hbox);
    refcontainer->addStyleClass("attcont");

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Referral> referral = dbsession.find<Referral>().where("ref_id = ?").bind(ref);

    WString pname = referral->refPracticeName_;
    WString pracid = referral->refPracticeId_;
    WString pzip = referral->refPracticeZip_;
    WString pdrfirst = referral->refPracticeDrFirst_;
    WString pdrlast = referral->refPracticeDrLast_;
    WString pnpi = referral->refPracticeNpi_;
    WString pemail = referral->refPracticeEmail_;
    WString pspec = referral->refSpecialty_;

    WString ptfirst = referral->refFirstName_;
    WString ptlast = referral->refLastName_;
    WString ptaddr1 = referral->refAddress1_;
    WString ptaddr2 = referral->refAddress2_;
    WString ptcity = referral->refCity_;
    WString ptzip = referral->refZip_;
    WString pthome = referral->refHomePhone_;
    WString ptcell = referral->refCellPhone_;
    WString ptdob = referral->refDob_;
    WString ptgender = referral->refGender_;
    WString rdiag = referral->refDiagnostic_;
    WString rover = referral->refOvernight_;
    WString rhome = referral->refHomeStudy_;
    WString rother = referral->refOtherSleepTest_;
    WString rauth = referral->refAuthConsult_;
    WString status = referral->refStatus_;

    WString signame = referral->enteredDrFullName_;
    WString sigemail = referral->enteredDrEmail_;
    WString sigdate = referral->selectedDate_;

    Wt::WVBoxLayout *rvbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rvbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(rvbox1);
    hbox->addLayout(rvbox2);

    Wt::WContainerWidget *refleft = new Wt::WContainerWidget(dialog->contents());
    refleft->addStyleClass("refdiagrefleft");
    refleft->addWidget(new Wt::WText("<h4>Referring Practice Info</h4>"));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Name: " + pname));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Zip: " + pzip));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Dr Full Name: " + pdrfirst + " " + pdrlast));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice NPI: " + pnpi));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Specialty: " + pspec));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Email: " + pemail));


    Wt::WContainerWidget *refright = new Wt::WContainerWidget(dialog->contents());
    refright->addStyleClass("refdiagrefright");
    refright->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Address: <br /></strong>" + ptaddr1));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText(ptaddr2));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient City: </strong>" + ptcity));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Zip: </strong>" + ptzip));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Home Phone: </strong>" + pthome));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Cell Phone: </strong>" + ptcell));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<h4>Referral Diagnosis</h4> "));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Diagnosis: </strong>" + rdiag));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Overnight Study: </strong>" + rover));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Other Study: </strong>" + rother));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Home Study: </strong>" + rhome));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Consultation: </strong><br /> " +rauth));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    refright->addWidget(new Wt::WText("Current Status:"+ status));


    //put table here
    dbo::Transaction atrans(dbsession);

    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(ref);
    std::cerr << attachments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Download"));

    int row = 1;
    for (Attachments::const_iterator i = attachments.begin(); i != attachments.end(); ++i, ++row)
    {
        Wt::WResource *attc = new AttachmentResource((*i)->attId_, (*i)->tempRefId_, (*i)->attFileName_, (*i)->attContentDescription_, this);

        new Wt::WText((*i)->attFileName_, table->elementAt(row, 0)),
        downDone = new Wt::WText("Downloaded", table->elementAt(row, 1)),
        downDone->hide(),
        btnDownloadAtt = new Wt::WPushButton("Download", table->elementAt(row, 1)),
        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WTemplate *t = new Wt::WTemplate(Wt::WString::tr("template.referral"));

    t->bindString("pname", Wt::WString(pname));
    t->bindString("pzip", Wt::WString(pzip));
    t->bindString("pdrfirst", Wt::WString(pdrfirst));
    t->bindString("pdrlast", Wt::WString(pdrlast));
    t->bindString("pnpi", Wt::WString(pnpi));
    t->bindString("pemail", Wt::WString(pemail));
    t->bindString("pspec", Wt::WString(pspec));
    t->bindString("ptfirst", Wt::WString(ptfirst));
    t->bindString("ptlast", Wt::WString(ptlast));
    t->bindString("ptaddr1", Wt::WString(ptaddr1));
    t->bindString("ptaddr2", Wt::WString(ptaddr2));
    t->bindString("ptcity", Wt::WString(ptcity));
    t->bindString("ptzip", Wt::WString(ptzip));
    t->bindString("pthome", Wt::WString(pthome));

    t->bindString("ptcell", Wt::WString(ptcell));
    t->bindString("ptdob", Wt::WString(ptdob));
    t->bindString("ptgender", Wt::WString(ptgender));
    t->bindString("rdiag", Wt::WString(rdiag));
    t->bindString("rover", Wt::WString(rover));
    t->bindString("rhome", Wt::WString(rhome));
    t->bindString("rother", Wt::WString(rother));
    t->bindString("rauth", Wt::WString(rauth));

    t->bindString("signame", Wt::WString(signame));
    t->bindString("sigemail", Wt::WString(sigemail));
    t->bindString("sigdate", Wt::WString(sigdate));

    std::ostringstream result;
    t->renderTemplate(result);
    Wt::WString s = result.str();

    /*
    WPushButton *pdfBtn = new Wt::WPushButton("Download PDF");
    Wt::WResource *pdf = new ReferralResource(this);
    pdfBtn->setLink(pdf);
    */

    //had to uncomment this because HPDF isnt working at the moment


    Wt::WResource *pdf = new ReferralResource(s, this);

    Wt::WPushButton *button = new Wt::WPushButton("Download Referral PDF");
    button->setLink(pdf);

    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("<h4>Attachments</h4>"));
    refleft->addWidget(table);
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(button);
    refleft->addWidget(new Wt::WBreak());

    rvbox1->addWidget(refleft);
    rvbox2->addWidget(refright);
    transaction.commit();

    dbo::Transaction strans(dbsession);

    dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);


    WString sptfirst = study->ptFirstName_;
    WString sptlast = study->ptLastName_;
    WString sptaddr1 = study->ptAddress1_;
    WString sptaddr2 = study->ptAddress2_;
    WString sptcity = study->ptCity_;
    WString sptzip = study->ptZip_;
    WString spthome = study->ptHomePhone_;
    WString sptcell = study->ptCellPhone_;
    WString sptdob = study->ptDob_;
    WString sptgender = study->ptGender_;

    WString stdiag = study->studyDiagnostic_;
    WString stover = study->studyOvernight_;
    WString sthome = study->studyHomeStudy_;
    WString stother = study->studyOtherSleepTest_;
    WString stauth = study->studyAuthConsult_;
    WString stid = study->studyId_;
    WString stdtype = study->studyTypeName_;
    WString ptout = study->patientId_;
    std::stringstream ss;
    ss << ptout;

    std::string pt = ss.str();

    WString ststatus = study->studyStatus_;
    WString stdate = study->studyDate_.toString("MM/dd/yyyy");
    WString sttime = study->studyTime_;
    WString stloco = study->studyLocation_;
    WString apptstatus = study->appointmentStatus_;
    WString nststatus = "Re-Scheduled";
    WString compststatus = "Study Complete";
    WString archivestatus = "Study Archived";

    Wt::WHBoxLayout *shbox = new Wt::WHBoxLayout();
    stdcontainer->setLayout(shbox);
    stdcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *svbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *svbox1 = new Wt::WVBoxLayout();


    shbox->addLayout(svbox1);
    shbox->addLayout(svbox2);

    Wt::WContainerWidget *stdleft = new Wt::WContainerWidget(dialog->contents());
    stdleft->addStyleClass("stddiagstdright");
    stdleft->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    stdleft->addWidget(new Wt::WText("Current Status:"+ status));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("Current Study Type:"+ stdtype));
    stdleft->addWidget(new Wt::WBreak());
    //get study type and ability to change study type
    Wt::WContainerWidget *typeContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblStudyType = new Wt::WLabel("Change Study Type:");
    lblStudyType->setStyleClass("sig-lbl");

    Wt::WHBoxLayout *typebox = new Wt::WHBoxLayout();
    typebox->addWidget(lblStudyType);

    dbo::Transaction typetrans(dbsession);

    typedef dbo::collection< dbo::ptr<StudyType> > StudyTypes;
    StudyTypes types = dbsession.find<StudyType>();
    std::cerr << types.size() << std::endl;

    std::vector<StudyTypeItem> typeitems;

    for (StudyTypes::const_iterator i = types.begin(); i != types.end(); ++i)
    {

        Wt::WString name = (*i)->studyTypeName_;
        Wt::WString code = (*i)->studyTypeCode_;
        Wt::WString id = (*i)->studyTypeId_;


        StudyTypeItem *e = new StudyTypeItem();
        e->studyTypeName = name;
        e->studyTypeCode = code;
        e->studyTypeId = id;


        typeitems.push_back(*e);

    }
    log("notice") << "Current Number of type items "<< typeitems.size();

    typetrans.commit();



    Wt::WComboBox *cbtype = new Wt::WComboBox();
    cbtype->setStyleClass("sig-left-box");
    Wt::WText *typeout = new Wt::WText();
    cbtype->addItem("");

    for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
    {
        cbtype->addItem(ev->studyTypeName);
    }

    cbtype->changed().connect(std::bind([=] () {

        StudyTypeItem selected;

        for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cbtype->currentText();

            if (ev->studyTypeName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.studyTypeName != "")
        {

            //update sleep study here
            dbo::Transaction stypetrans(dbsession);
            
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyTypeName_ = selected.studyTypeName.toUTF8();
            st.modify()->studyTypeCode_ = selected.studyTypeCode.toUTF8();
            st.modify()->studyTypeId_ = selected.studyTypeId.toUTF8();
            
            stypetrans.commit();
        }

    }));

    cbtype->setCurrentIndex(0);
    cbtype->setMargin(10, Wt::Right);

    typebox->addWidget(cbtype);
    typeContainer->setLayout(typebox);
    
    stdleft->addWidget(typeContainer);

    Wt::WContainerWidget *stdright = new Wt::WContainerWidget(dialog->contents());

    stdright->addStyleClass("stddiagstdleft");
    stdright->addWidget(new Wt::WText("<h4>Study Info</h4>"));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Sleep Study Date: " + stdate));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Appointment Time: " + sttime));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Appointment Location: " + stloco));
    stdright->addWidget(new Wt::WBreak());

    Wt::WPushButton *newstudy = new Wt::WPushButton("Re-Schedule Study");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(newstudy);

    Wt::WPushButton *confirmAppt = new Wt::WPushButton("Appointment Confirmed");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(confirmAppt);
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());

    Wt::WPushButton *completestudy = new Wt::WPushButton("Send MD Email - Study Complete");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(completestudy);

    Wt::WPushButton *archivestudy = new Wt::WPushButton("Archive Study");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(archivestudy);

    Wt::WPushButton *patientforms = new Wt::WPushButton("Create Forms Packet");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(patientforms);

    Wt::WPushButton *preformdl = new Wt::WPushButton("Download Pre Sleep Packet");
    Wt::WPushButton *postformdl = new Wt::WPushButton("Download Post Sleep Packet");
    if (apptstatus != "Checked Out")
    {
        preformdl->hide();
        postformdl->hide();

    } else {

        preformdl->show();
        postformdl->show();

    }

    if (apptstatus == "Confirmed" || apptstatus =="Checked Out")
    {
        confirmAppt->hide();

    } else {

        confirmAppt->show();
    }

    //download pre-sleep forms
    std::string presleepformurl;
    presleepformurl = "http://lab.unitedsleep.local/forms/presleep/"+stid.toUTF8();
    preformdl->setLink(presleepformurl);

    //download post-sleep forms
    std::string postsleepformurl;
    postsleepformurl = "http://lab.unitedsleep.local/forms/postsleep/"+stid.toUTF8();
    postformdl->setLink(postsleepformurl);


    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(preformdl);
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(postformdl);

    completestudy->clicked().connect(std::bind([=](){
            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyStatus_ = compststatus.toUTF8();
            strans.commit();

            dbo::Transaction pctrans2(dbsession);

            dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);

            WString poptmail = pc->pracEmailOpt_;
            //send email to practice
               pctrans2.commit();


               if (poptmail == "Yes") {
                Mail::Message message;
                message.setFrom(Mail::Mailbox("info@statsleep.com", "United STAT Sleep"));
                message.addRecipient(Mail::To, Mail::Mailbox(pemail.toUTF8(), pname.toUTF8()));
                message.setSubject("A Sleep Study is Complete");
                message.setBody("You referred a patient to our sleep center, and thier sleep study is complete. We are preparing the interpretation report now, you will receive an email with a link to download the report via a secure web-based service (fileon.com). Please login to your physician account to check details.");
                message.addHtmlBody ("<p>Hi, "+pname.toUTF8()+"<br>You referred a patient to our sleep center, and thier sleep study is complete.<br>  We are preparing the interpretation report now, you will receive an email with a link to download the report via <a href=\"https://fileon.com\">a secure web-based service.</a><br><br> Please <a href=\"https://127.0.0.1/providers\">Log In to your Physician Account</a> to check details.</p><br><p>Thanks,</p><p>The United STAT Sleep Team</p>");
                Mail::Client client;
                client.connect("localhost");
                client.send(message);
               }

            dialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

    }));

    confirmAppt->clicked().connect(std::bind([=](){

            Wt::WString confirmStatus = "Confirmed";

            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->appointmentStatus_ = confirmStatus.toUTF8();
            strans.commit();

            dialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

    }));

    archivestudy->clicked().connect(std::bind([=](){
            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyStatus_ = archivestatus.toUTF8();
            strans.commit();
            dialog->accept();
            this->refresh();

    }));

    patientforms->clicked().connect(std::bind([=]() {

        //create string vars for url, ctype, data

        std::string questionairePatientName;
        std::string questionairePrimaryComplaint;
        std::string questionaireClinicoreId;
        std::string questionairePatientDob;
        std::string questionaireStudyDate;
        std::string questionaireStudyStatus;

        std::string finalPatientName;
        std::string finalPrimaryComplaint;
        std::string finalClinicoreId;
        std::string finalPatientDob;
        std::string finalStudyDate;
        std::string finalStudyStatus;

        questionairePrimaryComplaint = "";
        questionairePatientName = ptfirst.toUTF8()+" "+ptlast.toUTF8();
        questionaireClinicoreId = stid.toUTF8();
        questionairePatientDob = sptdob.toUTF8();
        questionaireStudyDate = stdate.toUTF8();
        questionaireStudyStatus = "incomplete";



        finalPatientName = "\""+questionairePatientName+"\"";
        finalPrimaryComplaint = "\""+questionairePrimaryComplaint+"\"";
        finalClinicoreId = "\""+questionaireClinicoreId+"\"";
        finalPatientDob = "\""+questionairePatientDob+"\"";
        finalStudyDate = "\""+questionaireStudyDate+"\"";
        finalStudyStatus = "\""+questionaireStudyStatus+"\"";

        std::string url;
        std::string ctype;
        std::string data;

        url = "http://lab.statsleep.dev/api/v1/questionaires.json";
        ctype = "application/json";
        data =     "{\"patientName\": "+finalPatientName+","+"\"primaryComplaint\": "+finalPrimaryComplaint+","+"\"patientDob\": "+finalPatientDob+","+"\"patientClinicoreId\": "+finalClinicoreId+","+"\"studyDate\": "+finalStudyDate+","+"\"status\": "+finalStudyStatus+"}";

        //create std string vars with patient info for questionaire
        RestClient::Response res = RestClient::post(url, ctype, data);
        Wt::log("notice") << "Response body from server:" << res.body;

    }));


    newstudy->clicked().connect(std::bind([=]() {
        dialog->accept();

        Wt::WDialog *stdialog = new Wt::WDialog("Re-Schedule Sleep Study");

        Wt::WContainerWidget *cont= new Wt::WContainerWidget(stdialog->contents());

        Wt::WHBoxLayout *resthbox = new Wt::WHBoxLayout();
        cont->setLayout(resthbox);

        Wt::WVBoxLayout *restvbox1 = new Wt::WVBoxLayout();
        resthbox->addLayout(restvbox1);

        Wt::WVBoxLayout *restvbox2 = new Wt::WVBoxLayout();
        resthbox->addLayout(restvbox2);

        Wt::WText *curloco = new Wt::WText("Current Study Location: " + stloco);
        restvbox1->addWidget(curloco);
        restvbox1->addWidget(new Wt::WBreak());
        Wt::WLabel *lblLocation = new Wt::WLabel("New Sleep Study Location:");
        lblLocation->setStyleClass("sig-lbl");
        Wt::WHBoxLayout *locobox = new Wt::WHBoxLayout();
        locobox->addWidget(lblLocation);

        Wt::WComboBox *cbloco = new Wt::WComboBox();
        cbloco->setStyleClass("sig-left-box");

        cbloco->addItem("none selected");
        cbloco->addItem("Long Beach");
        cbloco->addItem("Downey");
        cbloco->addItem("Home Study");
        cbloco->setCurrentIndex(0);
        cbloco->setMargin(10, Wt::Right);

        Wt::WText *locoout = new Wt::WText();
        locoout->addStyleClass("help-block");

        cbloco->changed().connect(std::bind([=] () {
            locoout->setText(Wt::WString::fromUTF8("{1}")
                 .arg(cbloco->currentText()));
        }));

        locobox->addWidget(cbloco);
        restvbox1->addLayout(locobox);

        Wt::WText *curtime = new Wt::WText("Current Scheduled Study Time: " + sttime);
        restvbox1->addWidget(curtime);
        restvbox1->addWidget(new Wt::WBreak());
        Wt::WLabel *lblTime = new Wt::WLabel("New Study Appointment Time:");
        lblTime->setStyleClass("sig-lbl");
        Wt::WHBoxLayout *timebox = new Wt::WHBoxLayout();
        timebox->addWidget(lblTime);

        Wt::WComboBox *cbtime = new Wt::WComboBox();
        cbtime->setStyleClass("sig-left-box");

        cbtime->addItem("none selected");
        cbtime->addItem("7:00 PM");
        cbtime->addItem("7:30 PM");
        cbtime->addItem("8:00 PM");
        cbtime->addItem("8:30 PM");
        cbtime->addItem("9:00 PM");
        cbtime->addItem("9:30 PM");
        cbtime->addItem("10:00 PM");
        cbtime->addItem("10:30 PM");
        cbtime->addItem("11:00 PM");
        cbtime->addItem("11:30 PM");
        cbtime->addItem("12:00 PM");
        cbtime->setCurrentIndex(0);
        cbtime->setMargin(10, Wt::Right);

        Wt::WText *timeout = new Wt::WText();
        timeout->addStyleClass("help-block");

        cbtime->changed().connect(std::bind([=] () {
            timeout->setText(Wt::WString::fromUTF8("{1}")
                 .arg(cbtime->currentText()));
        }));

        timebox->addWidget(cbtime);
        restvbox1->addLayout(timebox);

        Wt::WCalendar *c2 = new Wt::WCalendar();
        c2->setSelectionMode(Wt::ExtendedSelection);

        Wt::WText* out = new Wt::WText();
        out->addStyleClass("help-block");

        Wt::WPushButton *schedule = new Wt::WPushButton("Save", stdialog->footer());
        schedule->setDefault(true);

        if (out->text() == "")
        {
            schedule->hide();
        }
        c2->selectionChanged().connect(std::bind([=]() {
            Wt::WString selected;
            std::set<Wt::WDate> selection = c2->selection();

            for (std::set<Wt::WDate>::const_iterator it = selection.begin();
                it != selection.end(); ++it) {
                if (!selected.empty())
                    selected += ", ";

                const Wt::WDate& d = *it;
                selected = d.toString("MM/dd/yyyy");
            }

            out->setText(Wt::WString::fromUTF8
                ("{1}")
                .arg(selected));
            schedule->show();
        }));


        restvbox2->addWidget(new Wt::WText("Current Study Date: " + stdate));
        restvbox2->addWidget(new Wt::WText("(You must select a new appointment date to Reschedule)"));
        restvbox2->addWidget(c2);
        restvbox2->addWidget(new Wt::WText("New Study Appointment Date"));
        restvbox2->addWidget(out);

        dbStudyLocation_ = new Wt::WText();
        dbStudyTime_ = new Wt::WText();
        dbStudyDate_ = new Wt::WText();


        cont->show();


        Wt::WPushButton *cl = new Wt::WPushButton("Cancel", stdialog->footer());
        stdialog->rejectWhenEscapePressed();

        schedule->clicked().connect(std::bind([=]() {
            if (locoout->text() == "")
            {
                dbStudyLocation_->setText(stloco);
            } else {
                dbStudyLocation_->setText(locoout->text());
            }
            if (timeout->text() == "")
            {
                dbStudyTime_->setText(sttime);

            } else {
                dbStudyTime_->setText(timeout->text());
            }
            if (out->text() == "")
            {
                dbStudyDate_->setText(stdate);
            } else {
                dbStudyDate_->setText(out->text());
            }

            Wt::WDate dbStudyDateD;
            dbStudyDateD = Wt::WDate().fromString(dbStudyDate_->text().toUTF8(), "MM/dd/yyyy");


            //update study details
            dbo::Transaction strans(dbsession);

            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);

            st.modify()->studyStatus_ = nststatus.toUTF8();
            st.modify()->studyDate_ = dbStudyDateD;
            st.modify()->studyTime_ = dbStudyTime_->text().toUTF8();
            st.modify()->studyLocation_ = dbStudyLocation_->text().toUTF8();


            strans.commit();
            
            dbo::Transaction mptrans(dbsession);
            dbo::ptr<Patient> mpt = dbsession.find<Patient>().where("pt_id = ?").bind(pt);
            mpt.modify()->numReschedule_ = mpt->numReschedule_ + 1;
            mptrans.commit();
            
            dbo::Transaction pctrans(dbsession);

            dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);

            WString poptmail = pc->pracEmailOpt_;
            //send email to practice
               pctrans.commit();


               if (poptmail == "Yes") {
                Mail::Message message;
                message.setFrom(Mail::Mailbox("info@statsleep.com", "United STAT Sleep"));
                message.addRecipient(Mail::To, Mail::Mailbox(pemail.toUTF8(), pname.toUTF8()));
                message.setSubject("A Sleep Study has been Re-Scheduled");
                message.setBody("You referred a patient to our sleep center, and a previously scheduled sleep study has been re-scheduled. Your patient has been notified, Please login to your physician account to check details.");
                message.addHtmlBody ("<p>Hi, "+pname.toUTF8()+"<br>You referred a patient to our sleep center, and a previously scheduled sleep study has been re-scheduled.<br> Please <a href=\"https://127.0.0.1/providers\">Log In to your Physician Account</a> to check the sleep study details.</p><br><p>Thanks,</p><p>The United STAT Sleep Team</p>");
                Mail::Client client;
                client.connect("localhost");
                client.send(message);
               }


            stdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

        }));

        cl->clicked().connect(stdialog, &Wt::WDialog::reject);

        stdialog->show();
    }));

    svbox1->addWidget(stdleft);
    svbox2->addWidget(stdright);

    strans.commit();


    dbo::Transaction ptrans(dbsession);

    dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

    WString patfirst = patient->ptFirstName_;
    WString patlast = patient->ptLastName_;
    WString patdob = patient->ptDob_;
    WString patgender = patient->ptGender_;
    WString pathome = patient->ptHomePhone_;
    WString patcell = patient->ptCellPhone_;
    WString pataddr1 = patient->ptAddress1_;
    WString pataddr2 = patient->ptAddress2_;
    WString patcity = patient->ptCity_;

    WString patemail = patient->ptEmail_;

    ptrans.commit();




    Wt::WContainerWidget *ptcontainer = new Wt::WContainerWidget();

    Wt::WHBoxLayout *patienthbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *patienthbox2 = new Wt::WHBoxLayout();
    ptcontainer->setLayout(patienthbox);
    ptcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *patientvbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *patientvbox1 = new Wt::WVBoxLayout();


    patienthbox->addLayout(patientvbox1);
    patienthbox->addLayout(patientvbox2);

    Wt::WContainerWidget *patientleft = new Wt::WContainerWidget();
    patientleft->addStyleClass("refdiagleft");
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("<h4>Patient Info:</h4>"));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("First Name: " + patfirst));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Last Name: " + patlast));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Home Phone: " + pathome));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Cell Phone: " + patlast));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Address 1: " + pataddr1));
    patientleft->addWidget(new Wt::WText("Address 2: " + pataddr2));
    patientleft->addWidget(new Wt::WText("City: " + patcity));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Email: " + patemail));



    Wt::WContainerWidget *patientright = new Wt::WContainerWidget();

    patientright->addWidget(new Wt::WText("<h4>Options</h4>"));


    patientvbox1->addWidget(patientleft);
    patientvbox2->addWidget(patientright);

    staticComment_ = new Wt::WText();
    staticComment_->setText(setCommentId());

    Wt::WContainerWidget *notes = new Wt::WContainerWidget();


    Wt::WVBoxLayout *notevbox = new Wt::WVBoxLayout();
    notes->setLayout(notevbox);

    dbo::Transaction cmtrans(dbsession);

    typedef dbo::collection< dbo::ptr<Comment> > Comments;
    Comments comments = dbsession.find<Comment>().where("backup_id = ?").bind(stid);
    std::cerr << comments.size() << std::endl;

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

    cmtrans.commit();

    notetable->addStyleClass("table form-inline");
    notetable->addStyleClass("table table-striped");
    notetable->addStyleClass("table table-hover");

    Wt::WContainerWidget *noteresult = new Wt::WContainerWidget();
    noteresult->addStyleClass("backup-comments");
    noteresult->addWidget(notetable);

    notevbox->addWidget(new Wt::WText("<h3>Chart Notes</h3>"));
    notevbox->addWidget(noteresult);

    notevbox->addWidget(new Wt::WText("<h4>Add Chart Note</h4>"));
    Wt::WTextEdit *noteedit = new Wt::WTextEdit();
    noteedit->setHeight(100);
    notevbox->addWidget(noteedit);

    Wt::WText *editout = new Wt::WText();

    Wt::WPushButton *notesave = new Wt::WPushButton("Add Chart Note");
    notevbox->addWidget(notesave);
    notesave->clicked().connect(std::bind([=] () {

        editout->setText("<pre>" + Wt::Utils::htmlEncode(noteedit->text()) + "</pre>");
        Wt::WString commentTextOut_ = editout->text().toUTF8();

        Wt::WString staffEmail = session_->strUserEmail();
        Wt::WString backupId = stid;
        Wt::WString patientId = ptout;

        Wt::Dbo::Transaction chtrans(dbsession);

        Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

        c.modify()->staffEmail_ = staffEmail.toUTF8();
        c.modify()->commentText_ = commentTextOut_;
        c.modify()->buId_ = backupId.toUTF8();
        c.modify()->ptId_ = patientId.toUTF8();

        //h.modify()->studentFirstName_ = studentFirst.toUTF8();
        //h.modify()->studentLastName_ = studentLast.toUTF8();
        //h.modify()->studentId_ = studentId.toUTF8();

        c.modify()->commentId_ = staticComment_->text().toUTF8();

        chtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal


    }));

    Wt::WContainerWidget *deleteStudy = new Wt::WContainerWidget();
    deleteStudy->addWidget(new Wt::WText("<h4>Are you sure you want to delete this sleep study? This cannot be undone.</h4>"));

    Wt::WPushButton *deleteSleepStudy = new Wt::WPushButton("Delete Sleep Study Now");
    deleteSleepStudy->setStyleClass("btn-delete-patient");
    deleteStudy->addWidget(new Wt::WBreak());
    deleteStudy->addWidget(deleteSleepStudy);


    deleteSleepStudy->clicked().connect(std::bind([=] () {
        dbo::Transaction rmtrans(dbsession);

        dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);
        study.remove();

        rmtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));
    
    Wt::WContainerWidget *filecontainer = new Wt::WContainerWidget();
        //put table here
    dbo::Transaction satrans(dbsession);

    Attachments sattachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(std);
    std::cerr << sattachments.size() << std::endl;

    Wt::WTable *filetable = new Wt::WTable();
    filetable->setHeaderCount(1);

    filetable->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    filetable->elementAt(0, 1)->addWidget(new Wt::WText("Download"));
    filetable->elementAt(0, 2)->addWidget(new Wt::WText("Delete"));

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

    Wt::WHBoxLayout *fileshbox = new Wt::WHBoxLayout();
    filecontainer->setLayout(fileshbox);

    Wt::WVBoxLayout *filesleft = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesleft);
    Wt::WVBoxLayout *filesright = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesright);

    filesleft->addWidget(new Wt::WText("<h4>Study Report Files</h4>"));
    filesleft->addWidget(filetable);

    composer_ = new Composer(conninfo_, std, this);

    filesright->addWidget(new Wt::WText("<h4>Upload Report</h4>"));
    filesright->addWidget(new Wt::WText("Upload FINAL REPORT ONLY to this sleep study. <p style=\"color: #C9302C\"> Warning: Anything you upload here will be accessible by the clinic that referred this patient via their Physician Portal.</p>"));
    filesright->addWidget(composer_);

    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(stdcontainer,
             "Sleep Study", Wt::WTabWidget::PreLoading);
    tabW->addTab(refcontainer,
             "Referral", Wt::WTabWidget::PreLoading);
    tabW->addTab(ptcontainer,
             "Patient Info", Wt::WTabWidget::PreLoading);
    tabW->addTab(notes,
             "Chart Notes", Wt::WTabWidget::PreLoading);
    tabW->addTab(filecontainer,
             "Study Report", Wt::WTabWidget::PreLoading);
    tabW->addTab(deleteStudy,
             "Delete Study", Wt::WTabWidget::PreLoading);

    tabW->setStyleClass("tabwidget");

    Wt::WPushButton *alright = new Wt::WPushButton("Ok", dialog->footer());

    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
        // provide an accessor for the signal
         done_.emit(42, "Refresh Now"); // emit the signal
    }));

    container->show();
    dialog->show();
}

void PatientListContainerWidget::deleteAttachment(Wt::WString att)
{
    Wt::WDialog *dialog = new Wt::WDialog();
    dbo::Transaction atttrans(dbsession);
    dbo::ptr<AttachmentDb> attachment = dbsession.find<AttachmentDb>().where("att_id = ?").bind(att);
    
    Wt::WString filename = attachment->attFileName_;
    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
    container->addWidget(new Wt::WText("<h3 style=\"color: red\">Are you sure you wish to delete: </h3>"));
    container->addWidget(new Wt::WText("<h4>" + filename + "?</h4> "));
    
    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    Wt::WPushButton *deletenow = new Wt::WPushButton("Delete Now", dialog->footer());
    
    deletenow->setStyleClass("btn-danger");
    
    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    deletenow->clicked().connect(std::bind([=]() {
        
        dbo::Transaction rmtrans(dbsession);    
        dbo::ptr<AttachmentDb> attr = dbsession.find<AttachmentDb>().where("att_id = ?").bind(att);
        attr.remove();
        rmtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    container->show();
    dialog->show();

}