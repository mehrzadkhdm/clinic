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
#include <boost/date_time/posix_time/posix_time.hpp>

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
#include <Wt/WLink>
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
#include <Wt/WServer>
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

#include "StaffSleepStudyWidget.h"
#include "InboundContainerWidget.h"
#include "SleepStudy.h"
#include "Referral.h"
#include "Attachment.h"
#include "Patient.h"
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

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;
namespace bpt = boost::posix_time;

//struct for SleepStudy info
namespace InboundRefApp{
    struct InboundSleepStudy {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptCity;

        InboundSleepStudy(
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

InboundContainerWidget::InboundContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
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
    dbsession.mapClass<AttachmentDb>("attachment");


    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<Comment>("comment");
    dbsession.mapClass<StudyType>("studytype");
    dbsession.mapClass<Insurance>("insurance");

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

void InboundContainerWidget::showSearchInbound(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    dbo::Transaction transaction(dbsession);
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("inbound_status = ?").bind("inbound").where("pt_firstname LIKE ? OR pt_lastname LIKE ? OR prac_name LIKE ? OR dr_first LIKE ? OR dr_last LIKE ?").bind(searchPt).bind(searchPt).bind(searchPt).bind(searchPt).bind(searchPt).orderBy("created_at desc");
    std::cerr << referrals.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Referring MD"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Birth Date"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Receive"));


    int row = 1;
    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i, ++row)
    {

            new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 0)),
            new Wt::WText((*i)->refPracticeDrLast_, table->elementAt(row, 1)),
            new Wt::WText((*i)->refFirstName_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refLastName_, table->elementAt(row, 3)),
            new Wt::WText((*i)->refDob_, table->elementAt(row, 4)),
            btnReceive = new Wt::WPushButton("Receive Referral", table->elementAt(row, 5)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnReceive->clicked().connect(boost::bind(&InboundContainerWidget::receiveReferral, this, (*i)->referralId_));

    }


    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    container->addWidget(table);




    addWidget(container);
}

void InboundContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();


    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';
    std::string studystatus = "Study Scheduled";

    dbo::Transaction transaction(dbsession);
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("inbound_status != ?").bind("inbound").where("pt_firstname LIKE ? OR pt_lastname LIKE ? OR prac_name LIKE ? OR dr_first LIKE ? OR dr_last LIKE ?").bind(searchPt).bind(searchPt).bind(searchPt).bind(searchPt).bind(searchPt).orderBy("ref_received desc");
    std::cerr << referrals.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Birth Date"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Referred From"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Referral Status"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Study Date"));
    table->elementAt(0, 6)->addWidget(new Wt::WText("Received"));
    table->elementAt(0, 7)->addWidget(new Wt::WText("Details"));


    int row = 1;
    for (Referrals::const_iterator i = referrals.begin(); i != referrals.end(); ++i, ++row)
    {
            
                
        WDate stdate;
        dbo::Transaction rpttrans(dbsession);
        
        typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
        Studies studies = dbsession.find<SleepStudy>().where("ref_id = ?").bind((*i)->referralId_);
        std::cerr << studies.size() << std::endl;
        if (studies.size() == 1) {
            for (Studies::const_iterator st = studies.begin(); st != studies.end(); ++st)
            {
                stdate = (*st)->studyDate_;

            }
        } else {
            stdate = Wt::WDate::fromString("00/00/0000");
        }

        // dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("ref_id = ?").bind((*i)->referralId_);
        // if (study.get() != nullptr) {
        //     stdate = study->studyDate_;
        // } else {
        //     stdate = Wt::WDate::fromString("00/00/0000");
        // }
        
        if ((*i)->refPracticeName_ == "")
        {
            Wt::WString fullname = (*i)->refPracticeDrFirst_ + " " + (*i)->refPracticeDrLast_;
            new Wt::WText(fullname, table->elementAt(row, 3));
        } else {
            new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 3));
        }
        new Wt::WText((*i)->refFirstName_, table->elementAt(row, 0)),
        new Wt::WText((*i)->refLastName_, table->elementAt(row, 1)),
        new Wt::WText((*i)->refDob_, table->elementAt(row, 2)),
        new Wt::WText((*i)->refStatus_, table->elementAt(row, 4)),
        new Wt::WText(stdate.toString("MM/dd/yyyy"), table->elementAt(row, 5)),
        new Wt::WText((*i)->refReceivedAt_.toString("MM/dd/yyyy"), table->elementAt(row, 6)),
        btnShowRef = new Wt::WPushButton("View Referral", table->elementAt(row, 7)),
        //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
        btnShowRef->clicked().connect(boost::bind(&InboundContainerWidget::showReferralDialog, this, (*i)->referralId_));

    }


    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    container->addWidget(new Wt::WText("<h4>Referrals</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(table);
    addWidget(container);
}

WString InboundContainerWidget::setStdId()
{
    return uuid();
}

WString InboundContainerWidget::setCommentId()
{
    return uuid();
}

WString InboundContainerWidget::setPtId()
{
    return uuid();
}

void InboundContainerWidget::receiveReferral(WString& ref)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
    Wt::WString recvstatus = "Order Received";
    Wt::WString inbstatus = "staff";

    Wt::WDate createDate = WDate::currentServerDate();

    dbo::Transaction transaction(dbsession);
    dbo::ptr<Referral> refer = dbsession.find<Referral>().where("ref_id = ?").bind(ref);
    refer.modify()->refStatus_ = recvstatus.toUTF8();
    refer.modify()->refInbound_ = inbstatus.toUTF8();
    refer.modify()->refReceivedAt_ = createDate;
    transaction.commit();

    //create new patient here.
    InboundContainerWidget::createPatient(ref);
}

void InboundContainerWidget::createPatient(WString& ref)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal
    staticPt_ = new Wt::WText();
    staticPt_->setText(setPtId());

    dbo::Transaction rpttrans(dbsession);

    dbo::ptr<Referral> referral = dbsession.find<Referral>().where("ref_id = ?").bind(ref);

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
    WString pracid = referral->refPracticeId_;

    rpttrans.commit();

    dbo::Transaction ptrans(dbsession);

    dbo::ptr<Patient> pt = dbsession.add(new Patient());

    pt.modify()->ptFirstName_ = ptfirst.toUTF8();
    pt.modify()->ptLastName_ = ptlast.toUTF8();
    pt.modify()->ptAddress1_ = ptaddr1.toUTF8();
    pt.modify()->ptAddress2_ = ptaddr2.toUTF8();
    pt.modify()->ptCity_ = ptcity.toUTF8();
    pt.modify()->ptHomePhone_ = pthome.toUTF8();
    pt.modify()->ptCellPhone_ = ptcell.toUTF8();
    pt.modify()->ptDob_ = ptdob.toUTF8();
    pt.modify()->refId_ = ref.toUTF8();
    pt.modify()->pracId_ = pracid.toUTF8();
    pt.modify()->ptId_ = staticPt_->text().toUTF8();

    ptrans.commit();

    dbo::Transaction transaction(dbsession);
    dbo::ptr<Referral> refer = dbsession.find<Referral>().where("ref_id = ?").bind(ref);
    refer.modify()->patientId_ = staticPt_->text().toUTF8();
    transaction.commit();

    InboundContainerWidget::showReferralDialog(ref);
}

void InboundContainerWidget::showReferralDialog(WString& ref)
{

    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    staticStd_ = new Wt::WText();
    staticStd_->setText(setStdId());

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
    WString rtypename = referral->studyTypeName_;
    WString rtypeid = referral->studyTypeId_;
    WString rtypecode = referral->studyTypeCode_;

    WString signame = referral->enteredDrFullName_;
    WString sigemail = referral->enteredDrEmail_;
    WString sigdate = referral->selectedDate_;

    //taking out and using the one from patient record instead. It makes more sense.
    //WString ptinsur = referral->refInsuranceName_;
    WString refdate = referral->refReceivedAt_.toString("MM/dd/yyyy");

    WString status = "";


    dbo::Transaction ptrans(dbsession);

    dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(rptid);
    WString ptinsur = patient->ptInsuranceName_;

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
    
    if (rtypeid == "") {
        
        left->addWidget(typeContainer);
    }

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

    Wt::WPushButton *newstudy = new Wt::WPushButton("Schedule Sleep Study", dialog->footer());
    newstudy->setDefault(true);
    
    Wt::WPushButton *save = new Wt::WPushButton("Save Referral", dialog->footer());
    newstudy->hide();

    //do not show "Schedule Study" unless there is an insurance selected
    if(ptinsur != "" || rtypeid != "")
    {
        newstudy->show();
        newStudyTypeId_  = rtypeid;
        newStudyTypeName_ = rtypename;
        newStudyTypeCode_ = rtypecode;
    } else if(rtypeid == "") {
        left->addWidget(new Wt::WText("<strong>You must choose the study type in order to schedule a study.</strong>"));
        left->addWidget(new Wt::WBreak());
    } else if (ptinsur == "") {
        newstudy->hide();
        right->addWidget(new Wt::WText("<strong>You must add patient insurance in order to schedule a study.</strong>"));
        right->addWidget(new Wt::WBreak());
    }

    //insurance selection to change the insurance or add new.

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
            dbo::ptr<Patient> pat = dbsession.find<Patient>().where("pt_id = ?").bind(rptid);

            pat.modify()->ptInsuranceName_ = studyInsuranceName_.toUTF8();
            pat.modify()->ptInsuranceId_ = studyInsuranceId_.toUTF8();

            ptrans.commit();
            newstudy->show();


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
    right->addWidget(new Wt::WBreak());
    right->addWidget(insuranceContainer);
    right->addWidget(new Wt::WBreak());

    save->clicked().connect(std::bind([=](){
        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    newstudy->clicked().connect(std::bind([=]() {
        dialog->accept();

        Wt::WDialog *stdialog = new Wt::WDialog("Schedule Sleep Study");
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

        Wt::WPushButton *schedule = new Wt::WPushButton("Schedule", stdialog->footer());
        schedule->setDefault(true);

        Wt::WPushButton *cl = new Wt::WPushButton("Cancel", stdialog->footer());
        stdialog->rejectWhenEscapePressed();
        schedule->clicked().connect(std::bind([=]() {

            dbStudyLocation_->setText(locoout->text());
            dbStudyTime_->setText(timeout->text());
            dbStudyDate_->setText(out->text());
            WString stdstatus_ = "Study Scheduled";
            Wt::WDate dbStudyDateD;
            dbStudyDateD = Wt::WDate().fromString(dbStudyDate_->text().toUTF8(), "MM/dd/yyyy");

            dbo::Transaction pchecktrans(dbsession);

            typedef dbo::collection< dbo::ptr<Patient> > Patients;
            Patients patients = dbsession.find<Patient>().where("pt_id = ?").bind(rptid);

            std::cerr << patients.size() << std::endl;

            if (patients.size() == 1)
            {
                for (Patients::const_iterator i = patients.begin(); i != patients.end(); ++i)
                {
                        dbo::Transaction strans(dbsession);
                        dbo::ptr<SleepStudy> st = dbsession.add(new SleepStudy());

                        st.modify()->ptFirstName_ = (*i)->ptFirstName_;
                        st.modify()->ptLastName_ = (*i)->ptLastName_;
                        st.modify()->ptAddress1_ = (*i)->ptAddress1_;
                        st.modify()->ptAddress2_ = (*i)->ptAddress2_;
                        st.modify()->ptCity_ = (*i)->ptCity_;
                        st.modify()->ptZip_ = (*i)->ptZip_;
                        st.modify()->ptHomePhone_ = (*i)->ptHomePhone_;
                        st.modify()->ptCellPhone_ = (*i)->ptCellPhone_;
                        st.modify()->ptEmail_ = (*i)->ptEmail_;
                        st.modify()->ptDob_ = (*i)->ptDob_;
                        st.modify()->ptGender_ = (*i)->ptGender_;
                        st.modify()->studyDiagnostic_ = rdiag.toUTF8();
                        st.modify()->studyOvernight_ = rover.toUTF8();
                        st.modify()->studyOtherSleepTest_ = rother.toUTF8();
                        st.modify()->studyHomeStudy_ = rhome.toUTF8();
                        st.modify()->studyAuthConsult_ = rauth.toUTF8();
                        st.modify()->studyStatus_ = stdstatus_.toUTF8();
                        st.modify()->studyDate_ = dbStudyDateD;
                        st.modify()->studyTime_ = dbStudyTime_->text().toUTF8();
                        st.modify()->studyLocation_ = dbStudyLocation_->text().toUTF8();
                        st.modify()->studyId_ = staticStd_->text().toUTF8();
                        st.modify()->referralId_ = ref.toUTF8();
                        st.modify()->practiceId_ = pracid.toUTF8();
                        st.modify()->patientId_ = (*i)->ptId_;

                        st.modify()->studyTypeId_ = newStudyTypeId_.toUTF8();
                        st.modify()->studyTypeName_ = newStudyTypeName_.toUTF8();
                        st.modify()->studyTypeCode_ = newStudyTypeCode_.toUTF8();

                    
                        strans.commit();

                }

            } else {

                log("notice") << "Cannot schedule! No patient was found!";
            }

            //referral status update
            dbo::Transaction transaction(dbsession);
            dbo::ptr<Referral> refer = dbsession.find<Referral>().where("ref_id = ?").bind(ref);
            refer.modify()->refStatus_ = stdstatus_.toUTF8();
            transaction.commit();

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

            stdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal
        }));

        cl->clicked().connect(stdialog, &Wt::WDialog::reject);

        stdialog->show();
    }));

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
        done_.emit(42, "Refresh Now"); // emit the signal
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

    Wt::WComboBox *cb = new Wt::WComboBox();
    cb->addItem("Pending");
    cb->addItem("Order Received");
    cb->addItem("Study Scheduled");
    cb->addItem("Patient Refused Study");
    cb->addItem("Insurance Declined Study");
    cb->addItem("Study Complete");
    cb->setCurrentIndex(0); // Show 'Pending' initially.
    cb->setMargin(10, Wt::Right);

    Wt::WText *statusout = new Wt::WText();
    statusout->addStyleClass("help-block");

    cb->changed().connect(std::bind([=] () {
        statusout->setText(Wt::WString::fromUTF8("{1}")
             .arg(cb->currentText()));
        dbo::Transaction transaction(dbsession);

        dbo::ptr<Referral> refer = dbsession.find<Referral>().where("ref_id = ?").bind(ref);
        refer.modify()->refStatus_ = statusout->text().toUTF8();
        transaction.commit();
    }));


    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<h4>Attachments</h4>"));
    left->addWidget(table);
    left->addWidget(new Wt::WBreak());
    left->addWidget(button);
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<br />Status Changed To: " + statusout->text()));
    left->addWidget(new Wt::WBreak());
    left->addWidget(cb);

    vbox1->addWidget(left);
    vbox2->addWidget(right);

    container->show();
    dialog->show();
}

