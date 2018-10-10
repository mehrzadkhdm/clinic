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
#include <thread>

//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//json rpc cpp library
#include "stubclient.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

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

#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/Utils>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
#include <Wt/WDate>
//hpdf stuff
#include <hpdf.h>

//rest http library
#include "restclient-cpp/restclient.h"

//other header files
#include "TechSleepStudyWidget.h"
#include "TechStudyListWidget.h"
#include "Referral.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "Comment.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"

using namespace Wt;
namespace dbo = Wt::Dbo;
namespace bpt = boost::posix_time;

using namespace jsonrpc;
using namespace std;


//stuct of the study data for view
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

//struct for practice info
namespace InboundRefApp{
    struct InboundPractice {
        Wt::WString practiceName;
        Wt::WString practiceDrFirst;
        Wt::WString practiceDrLast;
        Wt::WString practiceZip;

        InboundPractice(
            const Wt::WString& aPracName,
            const Wt::WString& aPracDrFirst,
            const Wt::WString& aPracDrLast,
            const Wt::WString& aPracZip)
            :    practiceName(aPracName),
            practiceDrFirst(aPracDrFirst),
            practiceDrLast(aPracDrLast),
            practiceZip(aPracZip){}
    };
}

//struct for study attachment info
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

namespace InboundStudyApp{
    struct InboundComment {
        Wt::WString staffEmail;
        Wt::WString commentText;
        Wt::WString createdAt;

        InboundComment(const Wt::WString& aStaffEmail,
            const Wt::WString& aCommentText,
            const Wt::WString& aCreatedAt)
            : staffEmail(aStaffEmail),
            commentText(aCommentText),
            createdAt(aCreatedAt){}
    };
}


TechSleepStudyWidget::TechSleepStudyWidget(const char *conninfo, Wt::WString location, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    location_(location),
    close_(this),
    conninfo_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
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

WString TechSleepStudyWidget::setPtId()
{
    return uuid();
}

WString TechSleepStudyWidget::setStdId()
{
    return uuid();
}

WString TechSleepStudyWidget::setRefId()
{
    return uuid();
}

WString TechSleepStudyWidget::setPracId()
{
    return uuid();
}

WString TechSleepStudyWidget::setCommentId()
{
    return uuid();
}

void TechSleepStudyWidget::studyList()
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    resultStack_ = new Wt::WStackedWidget(container);

    TechSleepStudyWidget::search();

    addWidget(container);

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(5000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        TechSleepStudyWidget::search();
    }));

    TechSleepStudyWidget::startRefreshTimer();
}

void TechSleepStudyWidget::preSearch()
{
    TechSleepStudyWidget::search();
}

void TechSleepStudyWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }
}

void TechSleepStudyWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}

void TechSleepStudyWidget::search()
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    stContainer = new TechStudyListWidget(conninfo_, session_, resultStack_);
    stContainer->showSearch(location_);

    resultStack_->setCurrentWidget(stContainer);

    stContainer->showStudy().connect(this, &TechSleepStudyWidget::showStudyDialog);
}

void TechSleepStudyWidget::showStudyDialog(WString st)
{
    staticComment_ = new Wt::WText();
    staticComment_->setText(setCommentId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Sleep Study");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WText *hidebutton = new Wt::WText("Loading...", dialog->footer());
    hidebutton->hide();

    Wt::WPushButton *accept = new Wt::WPushButton("Ok", dialog->footer());
    accept->setDefault(true);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());

    dbo::Transaction trans(dbsession);

    dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);

    WString pfname = study->ptFirstName_;
    WString plname = study->ptLastName_;
    WString pdob = study->ptDob_;
    WString phome = study->ptHomePhone_;
    WString pcell = study->ptCellPhone_;
    WString buid = study->studyId_;
    WString ptid = study->patientId_;
    WString stloc = study->studyLocation_;
    WString sttime = study->studyTime_;
    WString bednumber = study->bedNumber_;
    WString stdtype = study->studyTypeName_;
    WString apptstatus = study->appointmentStatus_;
    WString stid = study->studyId_;
    WString pracid = study->practiceId_;
    WString stdate = study->studyDate_.toString("MM/dd/yyyy");

    Wt::WDate ptdobin = Wt::WDate::fromString(pdob, "MM/dd/yyyy");
    Wt::WDate today = Wt::WDate::currentServerDate();

    int dobdiff = ptdobin.daysTo(today);
    float years = dobdiff / 365;
    log("info") << "DOB: " << ptdobin.toString("MM/dd/yyyy");
    log("info") << "TODAY: " << today.toString("MM/dd/yyyy");
    log("info") << "DOB DIFF: " << dobdiff;
    log("info") << "PT AGE: " << years;

    std::stringstream agestream;
    agestream << std::setprecision(1) << std::fixed << static_cast<float>(years);

    Wt::WString ptage = agestream.str();
    log("info") << "PT AGE: " << ptage.toUTF8();

    trans.commit();

    dbo::Transaction practrans(dbsession);
    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);
    WString mdfirst = practice->refPracticeDrFirst_;
    WString mdlast = practice->refPracticeDrLast_;
    practrans.commit();

    WString mdname = mdfirst + " " + mdlast;

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
    Wt::WContainerWidget *initial = new Wt::WContainerWidget();

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *hbox2 = new Wt::WHBoxLayout();
    initial->setLayout(hbox);
    initial->addStyleClass("backuplistcont");

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WContainerWidget *contactInfo = new Wt::WContainerWidget();

    contactInfo->addStyleClass("pt-left-cont");
    contactInfo->addWidget(new Wt::WText("<h4>Patient Contact Info:</h4>"));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("First Name: " + pfname));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Last Name: " + plname));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Home Phone: " + phome));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Cell Phone: " + pcell));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Age: " + ptage));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("<h4>Appointment Info:</h4>"));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Referring Physician: " + mdname));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Appointment Location: " + stloc));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Appointment Time: " + sttime));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Bed Number: " + bednumber));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Appointment Status: " + apptstatus));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("<h4>Study Info:</h4>"));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Study Type: " + stdtype));
    contactInfo->addWidget(new Wt::WBreak());


    Wt::WPushButton *preformdl = new Wt::WPushButton("Download Pre Sleep Packet");
    if (apptstatus == "Checked In")
    {
        preformdl->show();

    } else {

        preformdl->hide();

    }

    //download pre-sleep forms
    std::string presleepformurl;
    presleepformurl = "http://lab.unitedsleep.local/forms/presleep/"+stid.toUTF8();
    preformdl->setLink(presleepformurl);

    contactInfo->addWidget(preformdl);
    vbox1->addWidget(contactInfo);

    Wt::WComboBox *bed = new Wt::WComboBox();
    bed->addItem("Select Bed Number");
    bed->addItem("1");
    bed->addItem("2");
    bed->addItem("3");
    bed->addItem("4");
    bed->addItem("5");
    bed->addItem("6");
    bed->addItem("7");
    bed->addItem("8");
    bed->addItem("9");
    bed->addItem("10");
    bed->addItem("11");
    bed->addItem("12");

    Wt::WText *bedout = new Wt::WText();

    bed->changed().connect(std::bind([=] () {
        bedout->setText(Wt::WString::fromUTF8("{1}")
            .arg(bed->currentText()));

        dbo::Transaction bedtrans(dbsession);
        dbo::ptr<SleepStudy> studyb = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);
        studyb.modify()->bedNumber_ = bedout->text().toUTF8();

        bedtrans.commit();

    }));

    std::string bedn;
    bedn = bednumber.toUTF8();

    std::string pcloc;

    if (stloc == "Long Beach")
    {

        pcloc = "longbeach";

    } else if(stloc == "Downey") {

        pcloc = "downey";
    }

    std::string stu;
    stu = st.toUTF8();

    Wt::WComboBox *cb = new Wt::WComboBox();
    cb->addItem("Select Study Status");
    cb->addItem("Check Patient In");
    cb->addItem("Check Patient Out");
    cb->addItem("Patient Cancelled");
    cb->addItem("Patient Did Not Show");


    Wt::WText *errbox = new Wt::WText();
    errbox->addStyleClass("help-block");

    cb->changed().connect(std::bind([=] () {
        TechSleepStudyWidget::selection(st.toUTF8(), cb->currentText().toUTF8());
    }));

    vbox1->addWidget(new Wt::WBreak());

    vbox2->addWidget(bed);
    vbox2->addWidget(new Wt::WBreak());

    vbox2->addWidget(cb);

    Wt::WContainerWidget *notecontainer = new Wt::WContainerWidget();
    dbo::Transaction ctrans(dbsession);

    typedef dbo::collection< dbo::ptr<Comment> > Comments;
    Comments comments = dbsession.find<Comment>().where("backup_id = ?").bind(st);
    std::cerr << comments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("User"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Note"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Added"));


    int row = 1;
    for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i, ++row)
    {
            new Wt::WText((*i)->staffEmail_, table->elementAt(row, 0)),
            new Wt::WText((*i)->commentText_, table->elementAt(row, 1)),
            new Wt::WText((*i)->createdAt_.toString(), table->elementAt(row, 2));

    }

    ctrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addStyleClass("tech-study-comments");
    result->addWidget(table);

    notecontainer->addWidget(new Wt::WText("<h3>Chart Notes</h3>"));
    notecontainer->addWidget(result);

    notecontainer->addWidget(new Wt::WText("<h4>Add Chart Note</h4>"));
    Wt::WTextArea *edit = new Wt::WTextArea();
    edit->setHeight(100);

    notecontainer->addWidget(new Wt::WBreak());
    notecontainer->addWidget(edit);
    notecontainer->addWidget(new Wt::WBreak());
    Wt::WText *editout = new Wt::WText();


    Wt::WPushButton *save = new Wt::WPushButton("Add Chart Note");
    notecontainer->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        editout->setText("<pre>" + Wt::Utils::htmlEncode(edit->text()) + "</pre>");
        Wt::WString commentTextOut_ = editout->text().toUTF8();

        Wt::WString staffEmail = session_->strUserEmail();
        Wt::WString backupId = st;
        Wt::Dbo::Transaction htrans(dbsession);

        Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

        c.modify()->staffEmail_ = staffEmail.toUTF8();
        c.modify()->commentText_ = commentTextOut_;
        c.modify()->buId_ = backupId.toUTF8();
        c.modify()->ptId_ = ptid.toUTF8();

        c.modify()->commentId_ = staticComment_->text().toUTF8();

        htrans.commit();

        dialog->accept();


    }));
    
    Wt::WContainerWidget *filecontainer = new Wt::WContainerWidget();
        //put table here
    dbo::Transaction atrans(dbsession);

    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(ptid);
    std::cerr << attachments.size() << std::endl;

    Wt::WTable *filetable = new Wt::WTable();
    filetable->setHeaderCount(1);

    filetable->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    filetable->elementAt(0, 1)->addWidget(new Wt::WText("Download"));
    filetable->elementAt(0, 2)->addWidget(new Wt::WText("Delete"));

    int frow = 1;
    for (Attachments::const_iterator i = attachments.begin(); i != attachments.end(); ++i, ++frow)
    {

        Wt::WResource *attc = new AttachmentResource((*i)->attId_, (*i)->tempRefId_, (*i)->attFileName_, (*i)->attContentDescription_, this);

        new Wt::WText((*i)->attFileName_, filetable->elementAt(frow, 0)),
        downDone = new Wt::WText("Downloaded", filetable->elementAt(frow, 1)),
        downDone->hide(),
        
        btnDownloadAtt = new Wt::WPushButton("Download", filetable->elementAt(frow, 1));
        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    filetable->addStyleClass("table form-inline");
    filetable->addStyleClass("table table-striped");
    filetable->addStyleClass("table table-hover");

    Wt::WHBoxLayout *fileshbox = new Wt::WHBoxLayout();
    filecontainer->setLayout(fileshbox);

    Wt::WVBoxLayout *filesleft = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesleft);
    Wt::WVBoxLayout *filesright = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesright);

    filesleft->addWidget(new Wt::WText("<h4>Patient Files</h4>"));
    filesleft->addWidget(filetable);

    composer_ = new Composer(conninfo_, ptid, this);

    filesright->addWidget(new Wt::WText("<h4>Upload Files</h4>"));
    filesright->addWidget(new Wt::WText("Upload files to this patient chart."));
    filesright->addWidget(composer_);

    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(initial,
             "Study Info", Wt::WTabWidget::PreLoading);
    tabW->addTab(notecontainer,
             "Chart Notes", Wt::WTabWidget::PreLoading);
    tabW->addTab(filecontainer,
         "Patient Files", Wt::WTabWidget::PreLoading);

    tabW->setStyleClass("tabwidget");

    accept->clicked().connect(std::bind([=] () {
        accept->hide();
        hidebutton->show();
        dialog->accept();
    }));

    container->show();

    dialog->rejectWhenEscapePressed();
    cancel->clicked().connect(dialog, &Wt::WDialog::reject);
    
    this->close().connect(std::bind([=] () {
        dialog->accept();
    }));
    
    dialog->show();
}

void TechSleepStudyWidget::selection(std::string st, std::string changedText)
{

    dbo::Transaction trans(dbsession);

    dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);

    WString pfname = study->ptFirstName_;
    WString plname = study->ptLastName_;
    WString pdob = study->ptDob_;
    WString phome = study->ptHomePhone_;
    WString pcell = study->ptCellPhone_;
    WString buid = study->studyId_;
    WString ptid = study->patientId_;
    WString stloc = study->studyLocation_;
    WString sttime = study->studyTime_;
    WString bednumber = study->bedNumber_;
    WString stdtype = study->studyTypeName_;
    WString apptstatus = study->appointmentStatus_;
    WString stid = study->studyId_;
    WString stdate = study->studyDate_.toString("MM/dd/yyyy");

    Wt::WDate ptdobin = Wt::WDate::fromString(pdob, "MM/dd/yyyy");
    Wt::WDate today = Wt::WDate::currentServerDate();

    int dobdiff = ptdobin.daysTo(today);
    float years = dobdiff / 365;
    log("info") << "DOB: " << ptdobin.toString("MM/dd/yyyy");
    log("info") << "TODAY: " << today.toString("MM/dd/yyyy");
    log("info") << "DOB DIFF: " << dobdiff;
    log("info") << "PT AGE: " << years;

    std::stringstream agestream;
    agestream << std::setprecision(1) << std::fixed << static_cast<float>(years);

    Wt::WString ptage = agestream.str();
    log("info") << "PT AGE: " << ptage.toUTF8();

    trans.commit();

    std::string pcloc;

    std::string bedn;
    bedn = bednumber.toUTF8();

    if (stloc == "Long Beach")
    {

        pcloc = "longbeach";

    } else if(stloc == "Downey") {

        pcloc = "downey";
    }

    std::string stu;
    stu = st;

    Wt::WText *apptout = new Wt::WText();
    apptout->setText(Wt::WString::fromUTF8("{1}")
        .arg(changedText));

    if (apptout->text() == "Check Patient In")
    {

        log("notice") << "CHECK PATIENT IN";

        std::string checkedin = "Checked In";
        //update appointment status
        dbo::Transaction chkintrans(dbsession);
        dbo::ptr<SleepStudy> chkinrefer = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);
        chkinrefer.modify()->appointmentStatus_ = checkedin;
        chkintrans.commit();
        std::string empemail = session_->strUserEmail().toUTF8();
        TechSleepStudyWidget::startFormThread(pfname.toUTF8(), empemail, plname.toUTF8(), st, pdob.toUTF8(), stdate.toUTF8());

    } else if (apptout->text() == "Check Patient Out") {
        if (apptstatus == "Checked In")
        {
            Wt::WDialog *dialog = new Wt::WDialog("Return ID");
            dialog->setMinimumSize(1000, 700);
            dialog->setMaximumSize(1000, 700);
            
            Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
            container->addWidget(new Wt::WText("<h1 style=\"color: red\">Did you return the Patient's ID and Insurance Card?</h1>"));
            container->addWidget(new Wt::WText("<h5>Make Sure to check in the printer/scanner and around the Tech Room, possibly in your pockets, etc to make sure you did not accidently forget to return them.</h5>"));
            
            Wt::WPushButton *btnNo = new Wt::WPushButton("No I forgot! I'll Return Them", dialog->footer());
            Wt::WPushButton *btnYes = new Wt::WPushButton("Yes I Returned Them, Check Out", dialog->footer());

            btnNo->setStyleClass("btn-danger");
            btnYes->setStyleClass("btn-success");

            log("notice") << "CHECK PATIENT OUT";
            
            dialog->show();

            btnYes->clicked().connect(std::bind([=] () {
                std::string checkedout = "Checked Out";
                std::string studycpt = "Study Complete";
                //update appointment status
                dbo::Transaction chkouttrans(dbsession);
                dbo::ptr<SleepStudy> chkoutrefer = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);
                chkoutrefer.modify()->appointmentStatus_ = checkedout;
                chkoutrefer.modify()->studyStatus_ = studycpt;
                chkouttrans.commit();

                TechSleepStudyWidget::startTransferThread(bedn, st, pcloc);

                dialog->accept();
                close_.emit(42, "close dialog");               
            }));
            
            btnNo->clicked().connect(std::bind([=] () {
                dialog->reject();
                close_.emit(42, "close dialog");               

            }));


        } else {
            log("notice") << "PATIENT WASNT CHECKED IN - CANT CHECK OUT";
        }


    } else if (apptout->text() == "Patient Cancelled") {


        if (apptstatus == "Checked In" || apptstatus == "Checked Out")
        {
            log("notice") << "CANNOT CANCEL - ALREADY CHECKED IN OR OUT!";
        } else {
            log("notice") << "PATIENT CANCELLED";
            std::string cancelled = "Cancelled";
            //update appointment status
            dbo::Transaction canceltrans(dbsession);
            dbo::ptr<SleepStudy> cancrefer = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);
            cancrefer.modify()->appointmentStatus_ = cancelled;
            canceltrans.commit();
        }

    } else if (apptout->text() == "Patient Did Not Show") {

        if (apptstatus == "Checked In" || apptstatus == "Checked Out")
        {
            log("notice") << "CANNOT NO-SHOW - ALREADY CHECKED IN OR OUT!";
        } else {
            log("notice") << "PATIENT NO SHOW";
            std::string noshow = "No Show";
            //update appointment status
            dbo::Transaction noshtrans(dbsession);
            dbo::ptr<SleepStudy> noshrefer = dbsession.find<SleepStudy>().where("std_id = ?").bind(st);
            noshrefer.modify()->appointmentStatus_ = noshow;
            noshtrans.commit();
        }

    }

    TechSleepStudyWidget::preSearch();

}
void startBedTransfer(std::string bedn, std::string stu, std::string pcloc)
{
    //rpc request to start bedpost transfer
    HttpClient httpclient("http://10.0.3.112:5050");
    StubClient c(httpclient);

    try
    {
        cout << c.startTransfer(bedn, stu, pcloc) << endl;
        c.notifyServer();

    }
    catch (JsonRpcException e)
    {
        cerr << e.what() << endl;

    }
}

void startFormCreation(std::string pfname, std::string emp, std::string plname, std::string st, std::string pdob, std::string stdate)
{

    std::string employee = emp;
    Wt::log("notice") << "(Tech Email: " << employee;

    std::string questionairePatientName;
    std::string questionairePrimaryComplaint;
    std::string questionaireClinicoreId;
    std::string questionairePatientDob;
    std::string questionaireStudyDate;
    std::string questionaireStudyStatus;
    std::string questionaireEmployee;

    std::string finalPatientName;
    std::string finalPrimaryComplaint;
    std::string finalClinicoreId;
    std::string finalPatientDob;
    std::string finalStudyDate;
    std::string finalStudyStatus;
    std::string finalEmployee;

    questionairePrimaryComplaint = "";
    questionairePatientName = pfname+" "+plname;
    questionaireClinicoreId = st;
    questionairePatientDob = pdob;
    questionaireStudyDate = stdate;
    questionaireEmployee = employee;
    questionaireStudyStatus = "incomplete";

    finalPatientName = "\""+questionairePatientName+"\"";
    finalPrimaryComplaint = "\""+questionairePrimaryComplaint+"\"";
    finalClinicoreId = "\""+questionaireClinicoreId+"\"";
    finalPatientDob = "\""+questionairePatientDob+"\"";
    finalStudyDate = "\""+questionaireStudyDate+"\"";
    finalStudyStatus = "\""+questionaireStudyStatus+"\"";
    finalEmployee = "\""+questionaireEmployee+"\"";

    std::string url;
    std::string ctype;
    std::string data;

    url = "http://lab.statsleep.dev/api/v1/questionaires.json";
    ctype = "application/json";
    data =     "{\"patientName\": "+finalPatientName+","+"\"primaryComplaint\": "+finalPrimaryComplaint+","+"\"employeeEmail\": "+finalEmployee+","+"\"patientDob\": "+finalPatientDob+","+"\"patientClinicoreId\": "+finalClinicoreId+","+"\"studyDate\": "+finalStudyDate+","+"\"status\": "+finalStudyStatus+"}";

    //create std string vars with patient info for questionaire

    RestClient::Response res = RestClient::post(url, ctype, data);
    Wt::log("notice") << "Response body from server:" << res.body;
}

void TechSleepStudyWidget::startTransferThread(std::string bedn, std::string stu, std::string pcloc)
{
    //start thread to call transfer function
    boost::thread thr(startBedTransfer, bedn, stu, pcloc);
}

void TechSleepStudyWidget::startFormThread(std::string pfname, std::string emp, std::string plname, std::string st, std::string pdob, std::string stdate)
{
    //start thread to call form creation function
    boost::thread thr(startFormCreation, pfname, emp, plname, st, pdob, stdate);
}

