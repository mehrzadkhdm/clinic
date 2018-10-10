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
#include <Wt/WTimeEdit>
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
#include <Wt/WLink>
#include <Wt/WLocale>
#include <Wt/WStackedWidget>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

#include <Wt/Auth/RegistrationWidget>
#include <Wt/Auth/RegistrationModel>
#include <Wt/Auth/Login>
#include <Wt/Auth/AbstractUserDatabase>

#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/Utils>

#include "PracticeListContainerWidget.h"
#include "PhysicianVisit.h"
#include "PhysicianIssue.h"
#include "Referral.h"
#include "Patient.h"
#include "SleepStudy.h"
#include "Practice.h"
#include "Comment.h"
#include "Locality.h"
#include "LocalityItem.h"
#include "PracticeGroupItem.h"
#include "UserSession.h"
#include "PracticeSession.h"
#include "Address.h"
#include "PhysicianRegistrationModel.h"
#include "PhysicianRegistrationWidget.h"
#include "PhysicianAddressListContainerWidget.h"
#include "PhysicianRecordListContainerWidget.h"


using namespace Wt;
namespace dbo = Wt::Dbo;

PracticeListContainerWidget::PracticeListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    psession_(conninfo),
    conninfo_(conninfo),
    pg_(conninfo),
    done_(this),
    pracsession_(&psession_)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Locality>("locality");
    dbsession.mapClass<Address>("address");
    dbsession.mapClass<PhysicianVisit>("physicianvisit");
    dbsession.mapClass<PhysicianIssue>("physicianissue");
    dbsession.mapClass<CompositeEntity>("composite");
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

void PracticeListContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    std::string strSearchPrac = curstr.toUTF8();
    std::string searchPrac = '%' + strSearchPrac +'%';

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("prac_drfirst ILIKE ? OR prac_drlast ILIKE ?").bind(searchPrac).bind(searchPrac).where("is_group != ?").bind("Yes").orderBy("prac_drlast asc");
    std::cerr << practices.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("MD First Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("MD Last Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Phone Number"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Fax Number"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Email Address"));
    table->elementAt(0, 6)->addWidget(new Wt::WText("Details"));
    table->elementAt(0, 7)->addWidget(new Wt::WText("Create Account"));


    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {
            if ((*i)->dateAdded_.toString() != "" && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 90)
            {
                table->rowAt(row)->setStyleClass("row-new-provider");
            } else if ((*i)->dateAdded_.toString() != "" && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) > 90 && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 365) {
                table->rowAt(row)->setStyleClass("row-newyr-provider");
            }
            if ((*i)->refPracticeDrFirst_ == "") {
                new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 1));
            } else {
                new Wt::WText((*i)->refPracticeDrFirst_, table->elementAt(row, 1));
            }
            new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),

            new Wt::WText((*i)->refPracticeDrLast_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refPracticeNpi_, table->elementAt(row, 3)),
            new Wt::WText((*i)->pracFax_, table->elementAt(row, 4)),
            new Wt::WText((*i)->refPracticeEmail_, table->elementAt(row, 5)),
            btnSelectPractice = new Wt::WPushButton("Details", table->elementAt(row, 6)),
            btnCreateAccount = new Wt::WPushButton("Clinicore Login", table->elementAt(row, 7)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnSelectPractice->clicked().connect(boost::bind(&PracticeListContainerWidget::showPracticeDialog, this, (*i)->practiceId_));
            btnCreateAccount->clicked().connect(boost::bind(&PracticeListContainerWidget::showCreateAccount, this, (*i)->practiceId_));
            if ((*i)->clinicoreAccount_ == "Yes")
            {
                btnCreateAccount->hide();
                new Wt::WText("Has Account", table->elementAt(row, 7));
            }
            
    }


    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");


    container->addWidget(table);
    addWidget(container);
}

WString PracticeListContainerWidget::setStdId()
{
    return uuid();
}

WString PracticeListContainerWidget::setCommentId()
{
    return uuid();
}


void PracticeListContainerWidget::showPracticeDialog(WString prac)
{
    globalPracticeId = new Wt::WText(prac);
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    pracdialog = new Wt::WDialog("Provider Details");
    pracdialog->setMinimumSize(1000, 700);
    pracdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(pracdialog->contents());
    Wt::WContainerWidget *practiceDetails = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

    WString pname = practice->refPracticeName_;
    WString pzip = practice->refPracticeZip_;
    WString pdrfirst = practice->refPracticeDrFirst_;
    WString pdrlast = practice->refPracticeDrLast_;
    WString pnpi = practice->refPracticeNpi_;
    WString pfax = practice->pracFax_;
    WString pemail = practice->refPracticeEmail_;
    WString pspec = practice->refSpecialty_;
    WString praclocal = practice->localityName_;

    practiceDetails->addWidget(new Wt::WText("<h4>Practice Name: </h4>" + pname));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Practice Zip Code: </h4>"+ pzip));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Doctor First Name: </h4>" + pdrfirst));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Doctor Last Name: </h4>" + pdrlast));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Phone Number: </h4>" + pnpi));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Email: </h4>" + pemail));
    practiceDetails->addWidget(new Wt::WBreak());
    practiceDetails->addWidget(new Wt::WText("<h4>Specialty: </h4>" + pspec));
    practiceDetails->addWidget(new Wt::WBreak());


    Wt::WContainerWidget *editPractice = new Wt::WContainerWidget();

    Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
    editPractice->setLayout(pthbox);

    Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

    pthbox->addLayout(leftbox);
    pthbox->addLayout(rightbox);
    
    Wt::WPushButton *deletePractice = new Wt::WPushButton("Delete Practice");
    deletePractice->setStyleClass("btn-delete-patient");

    deletePractice->clicked().connect(std::bind([=] () {
        pracdialog->accept();

        Wt::WDialog *delPtDialog = new Wt::WDialog("DELETE PROVIDER");

        Wt::WContainerWidget *delPtContainer = new Wt::WContainerWidget(delPtDialog->contents());

        Wt::WText *warningDelPt = new Wt::WText("YOU ARE ABOUT TO PERMENANTLY DELETE THE PROVIDER RECORD FOR: "+pdrfirst+" "+pdrlast);
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

        typedef dbo::collection< dbo::ptr<Referral> > Referrals;
        Referrals referrals = dbsession.find<Referral>().where("prac_id = ?").bind(prac);
        std::cerr << referrals.size() << std::endl;

        Wt::WText *studiesFound = new Wt::WText("THERE ARE ORDERS ASSOCIATED WITH THIS PROVIDER");
        Wt::WText *studiesLower = new Wt::WText("You cannot delete the provider.");

        Wt::WPushButton *deleteNow = new Wt::WPushButton("YES - DELETE NOW", delPtDialog->footer());
        deleteNow->setStyleClass("del-pt-now-btn");
        deleteNow->show();

        if (referrals.size() != 0)
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

            dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac);
            practice.remove();

            rmtrans.commit();

            delPtDialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal
        }));
        delcancel->clicked().connect(std::bind([=]() {
            delPtDialog->reject();
        }));

        delPtDialog->show();

    }));

    Wt::WPushButton *modprac = new Wt::WPushButton("Save Changes");
    modprac->setDefault(true);
    modprac->setStyleClass("mod-practice-btn");

    //input fields for patient demographics
    Wt::WText *pracNameLbl = new Wt::WText("Practice Name");
    pracNameLbl->setStyleClass("label-left-box");
    pracName_ = new Wt::WLineEdit(this);
    pracName_->setStyleClass("left-box");
    pracName_->setText(pname);
    leftbox->addWidget(pracNameLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(pracName_);

    Wt::WText *pracDrFirstLbl = new Wt::WText("Doctor First Name");
    pracDrFirstLbl->setStyleClass("label-left-box");
    pracDrFirstName_ = new Wt::WLineEdit(this);
    pracDrFirstName_->setStyleClass("left-box");
    pracDrFirstName_->setText(pdrfirst);
    leftbox->addWidget(pracDrFirstLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(pracDrFirstName_);

    Wt::WText *pracDrLastLbl = new Wt::WText("Doctor Last Name");
    pracDrLastLbl->setStyleClass("label-left-box");
    pracDrLastName_ = new Wt::WLineEdit(this);
    pracDrLastName_->setStyleClass("left-box");
    pracDrLastName_->setText(pdrlast);
    leftbox->addWidget(pracDrLastLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(pracDrLastName_);

    Wt::WText *pracZipLbl = new Wt::WText("Practice Zip");
    pracZipLbl->setStyleClass("label-left-box");
    pracZip_ = new Wt::WLineEdit(this);
    pracZip_->setStyleClass("left-box");
    pracZip_->setText(pzip);
    leftbox->addWidget(pracZipLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(pracZip_);

    Wt::WText *pracNpiLbl_ = new Wt::WText("Practice Phone Number");
    pracNpiLbl_->setStyleClass("label-left-box");
    pracNpi_ = new Wt::WLineEdit(this);
    pracNpi_->setStyleClass("left-box");
    pracNpi_->setText(pnpi);
    leftbox->addWidget(pracNpiLbl_);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(pracNpi_);

    Wt::WText *pracSpecialtyLbl_ = new Wt::WText("Practice Specialty");
    pracSpecialtyLbl_->setStyleClass("label-left-box");
    pracSpecialty_ = new Wt::WLineEdit(this);
    pracSpecialty_->setStyleClass("left-box");
    pracSpecialty_->setText(pspec);
    rightbox->addWidget(pracSpecialtyLbl_);
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(pracSpecialty_);

    Wt::WText *pracEmailLbl_ = new Wt::WText("Practice Email Address");
    pracEmailLbl_->setStyleClass("label-left-box");
    pracEmail_ = new Wt::WLineEdit(this);
    pracEmail_->setStyleClass("left-box");
    pracEmail_->setText(pemail);
    rightbox->addWidget(pracEmailLbl_);
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(pracEmail_);

    Wt::WText *pracFaxLbl_ = new Wt::WText("Practice Fax Number");
    pracFaxLbl_->setStyleClass("label-left-box");
    pracFax_ = new Wt::WLineEdit(this);
    pracFax_->setStyleClass("left-box");
    pracFax_->setText(pfax);
    rightbox->addWidget(pracFaxLbl_);
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(pracFax_);    

    modprac->clicked().connect(std::bind([=]() {

        std::string strPracName = pracName_->text().toUTF8();
        std::string strPracDrFirst = pracDrFirstName_->text().toUTF8();
        std::string strPracDrLast = pracDrLastName_->text().toUTF8();
        std::string strPracZip = pracZip_->text().toUTF8();
        std::string strPracNpi = pracNpi_->text().toUTF8();
        std::string strPracSpec = pracSpecialty_->text().toUTF8();
        std::string strPracEmail = pracEmail_->text().toUTF8();
        std::string strPracFax = pracFax_->text().toUTF8();

        dbo::Transaction mptrans(dbsession);

        dbo::ptr<Practice> mprac = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

        mprac.modify()->refPracticeDrFirst_ = strPracDrFirst;
        mprac.modify()->refPracticeDrLast_ = strPracDrLast;
        mprac.modify()->refPracticeZip_ = strPracZip;
        mprac.modify()->refPracticeNpi_ = strPracNpi;
        mprac.modify()->refSpecialty_ = strPracSpec;
        mprac.modify()->refPracticeEmail_ = strPracEmail;
        mprac.modify()->pracFax_ = strPracFax;

        mptrans.commit();

        pracdialog->accept();

        done_.emit(42, "Refresh Now"); // emit the signal

    }));
    
    Wt::WContainerWidget *localityContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblLocality = new Wt::WLabel("Physician Locality:");
    lblLocality->setStyleClass("sig-lbl");
    Wt::WText *out = new Wt::WText();
    out->setText(praclocal);

    Wt::WHBoxLayout *localityhbox = new Wt::WHBoxLayout();
    localityhbox->addWidget(lblLocality);
    localityhbox->addWidget(new Wt::WBreak());
    localityhbox->addWidget(out);
    localityhbox->addWidget(new Wt::WBreak());

    dbo::Transaction instrans(dbsession);

    typedef dbo::collection< dbo::ptr<Locality> > Locals;
    Locals locals = dbsession.find<Locality>();
    std::cerr << locals.size() << std::endl;

    std::vector<LocalityItem> localitems;

    for (Locals::const_iterator i = locals.begin(); i != locals.end(); ++i)
    {

        Wt::WString name = (*i)->name_;
        Wt::WString abbrv = (*i)->abbrv_;
        Wt::WString id = (*i)->localityId_;

        Wt::WString fullname = name + ", " + abbrv;

        LocalityItem *e = new LocalityItem();
        e->localName = fullname;
        e->localAbbrv = abbrv;
        e->localId = id;


        localitems.push_back(*e);

    }
    log("notice") << "Current Number of locality items "<< localitems.size();

    instrans.commit();


    Wt::WComboBox *cblocality = new Wt::WComboBox();
    cblocality->setStyleClass("sig-left-box");
    Wt::WText *localout = new Wt::WText();
    cblocality->addItem("");

    for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
    {
        cblocality->addItem(ev->localName);
    }

    cblocality->changed().connect(std::bind([=] () {

          LocalityItem selected;

          for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cblocality->currentText();

            if (ev->localName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.localName != "")
        {
            Wt::WString physLocalityId_;
            Wt::WString physLocalityName_;

            physLocalityId_ = selected.localId;
            physLocalityName_ = selected.localName;

            //save locality change to practice
            dbo::Transaction ptrans(dbsession);

            dbo::ptr<Practice> pat = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

            pat.modify()->localityName_ = physLocalityName_.toUTF8();
            pat.modify()->localityId_ = physLocalityId_.toUTF8();

            ptrans.commit();
            out->setText(physLocalityName_);

        } else {
            log("notice") << "Selected: No Locality";
        }

    }));

    cblocality->setCurrentIndex(0);
    cblocality->setMargin(10, Wt::Right);

    localityhbox->addWidget(cblocality);
    localityContainer->setLayout(localityhbox);

    rightbox->addWidget(localityContainer);
    Wt::WContainerWidget *groupContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblGroup = new Wt::WLabel("Physician Group:");
    lblGroup->setStyleClass("sig-lbl");
    Wt::WText *gout = new Wt::WText();
    gout->setText(praclocal);

    Wt::WHBoxLayout *grouphbox = new Wt::WHBoxLayout();
    grouphbox->addWidget(lblGroup);
    grouphbox->addWidget(new Wt::WBreak());

    dbo::Transaction gtrans(dbsession);

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("is_group = ?").bind("Yes");
    std::cerr << practices.size() << std::endl;

    std::vector<PracticeGroupItem> groupitems;

    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i)
    {

        Wt::WString name = (*i)->refPracticeName_;
        Wt::WString id = (*i)->practiceId_;

        PracticeGroupItem *e = new PracticeGroupItem();
        e->groupName = name;
        e->groupId = id;


        groupitems.push_back(*e);

    }
    log("notice") << "Current Number of group items "<< groupitems.size();

    gtrans.commit();


    Wt::WComboBox *cbgroup = new Wt::WComboBox();
    cbgroup->setStyleClass("sig-left-box");
    Wt::WText *groupout = new Wt::WText();
    cbgroup->addItem("");

    for (std::vector<PracticeGroupItem>::const_iterator ev = groupitems.begin(); ev != groupitems.end(); ++ev)
    {
        cbgroup->addItem(ev->groupName);
    }

    cbgroup->changed().connect(std::bind([=] () {

          PracticeGroupItem selected;

          for (std::vector<PracticeGroupItem>::const_iterator ev = groupitems.begin(); ev != groupitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cbgroup->currentText();

            if (ev->groupName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.groupName != "")
        {
            Wt::WString groupPracticeId_;
            Wt::WString groupPracticeName_;

            groupPracticeId_ = selected.groupId;
            groupPracticeName_ = selected.groupName;

            //save group change to practice
            dbo::Transaction ptrans(dbsession);

            dbo::ptr<Practice> pat = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

            pat.modify()->refPracticeName_ = groupPracticeName_.toUTF8();
            pat.modify()->groupId_ = groupPracticeId_.toUTF8();

            ptrans.commit();

        } else {
            log("notice") << "Selected: No Practice";
        }

    }));

    cbgroup->setCurrentIndex(0);
    cbgroup->setMargin(10, Wt::Right);

    grouphbox->addWidget(cbgroup);
    groupContainer->setLayout(grouphbox);

    rightbox->addWidget(groupContainer);

    rightbox->addWidget(modprac);
    rightbox->addWidget(deletePractice);

    Wt::WContainerWidget *visitation = new Wt::WContainerWidget();
    Wt::WContainerWidget *visittable = new Wt::WContainerWidget();
    visittable->setStyleClass("physician-pracdialog-visit-table");
    visittable->addStyleClass("visit-comments");

    dbo::Transaction vtrans(dbsession);
    typedef dbo::collection< dbo::ptr<PhysicianVisit> > Visits;
    Visits visits = dbsession.find<PhysicianVisit>().where("prac_id = ?").bind(prac.toUTF8()).orderBy("this_visit_dt desc");
    //std::cerr << visits.size() << std::endl;

    Wt::WTable *vtable = new Wt::WTable();
    vtable->setHeaderCount(1);
    vtable->elementAt(0, 0)->addWidget(new Wt::WText("PCC Name"));
    vtable->elementAt(0, 1)->addWidget(new Wt::WText("Reason for Visit"));
    vtable->elementAt(0, 2)->addWidget(new Wt::WText("Date"));
    vtable->elementAt(0, 3)->addWidget(new Wt::WText("Type"));
    vtable->elementAt(0, 4)->addWidget(new Wt::WText("Details"));
    Wt::WString timezone;

    int strow = 1;
    for (Visits::const_iterator v = visits.begin(); v != visits.end(); ++v, ++strow)
    {
            Wt::WDateTime fixstart = (*v)->visitStartTime_;
            Wt::WDateTime adjuststart = fixstart.addSecs(-25200);
            WString finalstart = adjuststart.toString("MM/dd/yyyy hh:mm a");

            new Wt::WText((*v)->pccName_, vtable->elementAt(strow, 0)),
            new Wt::WText((*v)->reasonForVisit_, vtable->elementAt(strow, 1)),
            new Wt::WText(finalstart, vtable->elementAt(strow, 2)),
            new Wt::WText((*v)->visitType_, vtable->elementAt(strow, 3)),
            btnShowVisit = new Wt::WPushButton("Details", vtable->elementAt(strow, 4)),
            //click 'PDF' button on any ptreftable refrow, connect downloadReferral with that referral uuid
            btnShowVisit->clicked().connect(boost::bind(&PracticeListContainerWidget::showPhysicianVisitDialog, this, (*v)->visitId_));

            btnShowVisit->clicked().connect(std::bind([=]() {
                pracdialog->accept();
            }));
    }

    vtrans.commit();

    vtable->addStyleClass("table form-inline");
    vtable->addStyleClass("table table-striped");
    vtable->addStyleClass("table table-hover");


    Wt::WPushButton *newVisit = new Wt::WPushButton("New Visit Log Entry");
    newVisit->setDefault(true);

    newVisit->clicked().connect(std::bind([=]() {
        pracdialog->accept();
        PracticeListContainerWidget::newPhysicianVisitStart(prac);
    }));

    visitation->addWidget(new Wt::WText("<h4>PCC Office Visits</h4>"));
    visitation->addWidget(new Wt::WBreak());
    visitation->addWidget(newVisit);
    visitation->addWidget(new Wt::WBreak());
    visittable->addWidget(vtable);
    visitation->addWidget(visittable);

    Wt::WContainerWidget *physissues = new Wt::WContainerWidget();
    Wt::WContainerWidget *issuestable = new Wt::WContainerWidget();
    issuestable->setStyleClass("physician-pracdialog-issues-table");

    dbo::Transaction itrans(dbsession);
    typedef dbo::collection< dbo::ptr<PhysicianIssue> > Issues;
    Issues issues = dbsession.find<PhysicianIssue>().where("prac_id = ?").bind(prac.toUTF8()).orderBy("date_reported desc");
    //std::cerr << issues.size() << std::endl;

    Wt::WTable *itable = new Wt::WTable();
    itable->setHeaderCount(1);
    itable->elementAt(0, 0)->addWidget(new Wt::WText("PCC Name"));
    itable->elementAt(0, 1)->addWidget(new Wt::WText("Issue"));
    itable->elementAt(0, 2)->addWidget(new Wt::WText("Status"));
    itable->elementAt(0, 3)->addWidget(new Wt::WText("Action Taken"));
    itable->elementAt(0, 4)->addWidget(new Wt::WText("Reported"));
    itable->elementAt(0, 5)->addWidget(new Wt::WText("Details"));

    int irow = 1;
    for (Issues::const_iterator is = issues.begin(); is != issues.end(); ++is, ++irow)
    {

            new Wt::WText((*is)->pccName_, itable->elementAt(irow, 0)),
            new Wt::WText((*is)->issueTitle_, itable->elementAt(irow, 1)),
            new Wt::WText((*is)->issueStatus_, itable->elementAt(irow, 2)),
            new Wt::WText((*is)->actionTaken_, itable->elementAt(irow, 3)),
            new Wt::WText((*is)->issueReported_.toString("MM/dd/yyyy"), itable->elementAt(irow, 4)),
            btnShowIssue = new Wt::WPushButton("Details", itable->elementAt(irow, 5)),
            //click 'PDF' button on any ptreftable refrow, connect downloadReferral with that referral uuid
            btnShowIssue->clicked().connect(boost::bind(&PracticeListContainerWidget::showPhysicianIssueDialog, this, (*is)->issueId_));

            btnShowIssue->clicked().connect(std::bind([=]() {
                pracdialog->accept();
            }));
    }

    itrans.commit();

    itable->addStyleClass("table form-inline");
    itable->addStyleClass("table table-striped");
    itable->addStyleClass("table table-hover");

    Wt::WPushButton *newIssue = new Wt::WPushButton("New Issue");
    newIssue->setDefault(true);

    newIssue->clicked().connect(std::bind([=]() {
        pracdialog->accept();
        PracticeListContainerWidget::newPhysicianIssueDialog(prac);
    }));

    physissues->addWidget(new Wt::WText("<h4>Physician Issues</h4>"));
    physissues->addWidget(new Wt::WBreak());
    physissues->addWidget(newIssue);
    physissues->addWidget(new Wt::WBreak());
    issuestable->addWidget(itable);
    physissues->addWidget(issuestable);

    Wt::WContainerWidget *physaddress = new Wt::WContainerWidget();
    Wt::WContainerWidget *currentaddress = new Wt::WContainerWidget(physaddress);
    Wt::WContainerWidget *newaddress = new Wt::WContainerWidget(physaddress);
    
    WPushButton *changeAddress = new WPushButton("Update Address");

    if (practice->addressId_ != "") {
        
        newaddress->hide();

        dbo::Transaction trans(dbsession);
        dbo::ptr<Practice> curprac = dbsession.find<Practice>().where("prac_id = ?").bind(practice->practiceId_);
        dbo::ptr<Address> addr = dbsession.find<Address>().where("address_id = ?").bind(curprac->addressId_);
        dbo::ptr<Locality> loc = dbsession.find<Locality>().where("local_id = ?").bind(addr->localityId_);
        WString fullloc = loc->name_ + ", " + loc->abbrv_;

        WText *addrline1 = new WText(addr->line1_);
        WText *addrloc = new WText(fullloc);
        WText *addrzip = new WText(addr->zip_);
        
        addrline1->setStyleClass("phys-addr-lien1");
        addrloc->setStyleClass("phys-addr-loc");
        addrzip->setStyleClass("phys-addr-zip");

        currentaddress->addWidget(addrline1);
        currentaddress->addWidget(new WBreak());

        currentaddress->addWidget(addrline1);
        currentaddress->addWidget(new WBreak());
    
        WPushButton *saveline2 = new Wt::WPushButton("Save");
        saveline2->setStyleClass("save-prac-line2");
        
        Wt::WLineEdit *pracLine2_ = new Wt::WLineEdit();
        pracLine2_->setStyleClass("left-box");
        if (practice->addrLine2_ != "") {
            pracLine2_->setText(practice->addrLine2_);
        }
        pracLine2_->setPlaceholderText("Unit, suite number, etc");
        currentaddress->addWidget(pracLine2_);
        currentaddress->addWidget(saveline2);
        
        saveline2->clicked().connect(std::bind([=]() {

            std::string strLine2 = pracLine2_->text().toUTF8();
            dbo::Transaction mptrans(dbsession);

            dbo::ptr<Practice> mprac = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());
            mprac.modify()->addrLine2_ = strLine2;
        
            mptrans.commit();
        }));

        currentaddress->addWidget(new Wt::WBreak());
        currentaddress->addWidget(addrloc);
        currentaddress->addWidget(new WBreak());
        currentaddress->addWidget(addrzip);
        currentaddress->addWidget(new WBreak());
        currentaddress->addWidget(new WBreak());
        currentaddress->addWidget(changeAddress);
        currentaddress->show();
    }

    WText *updateaddress = new Wt::WText("<h4>Update Physician Address</h4>", newaddress);
    WText *searchaddress = new Wt::WText("<h5>Search for exisiting addresses before adding a new one</h5>", newaddress);

    Wt::WLineEdit *edit = new Wt::WLineEdit(newaddress);
    edit->setPlaceholderText("Search for address");
    edit->setStyleClass("search-box");

    Wt::WStackedWidget *resultStack_;

    resultStack_ = new Wt::WStackedWidget(newaddress);


    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        Wt::log("notice") << "SILW::search()";

        resultStack_->clear();

        insContainer = new PhysicianAddressListContainerWidget(conninfo_, session_, resultStack_);
        insContainer->showSearch(upper);
        resultStack_->setCurrentWidget(insContainer);
        insContainer->selectAddress().connect(this, &PracticeListContainerWidget::addPhysicianAddress);

    }));
    
    changeAddress->clicked().connect(std::bind([=]() {
        currentaddress->hide();
        newaddress->show();
    }));

    //wcontainer for merge tool

    Wt::WContainerWidget *mergephysician = new Wt::WContainerWidget();

    WText *mergeheader = new Wt::WText("<h4>Merge Physician Records</h4>", mergephysician);
    WText *searchpractice = new Wt::WText("<h5>Search for duplicate physicians to add merge this physician information with.</h5>", mergephysician);

    Wt::WLineEdit *pracEdit = new Wt::WLineEdit(mergephysician);
    pracEdit->setPlaceholderText("Search for physician by name");
    pracEdit->setStyleClass("search-box");

    Wt::WStackedWidget *pracResultStack_;

    pracResultStack_ = new Wt::WStackedWidget(mergephysician);


    pracEdit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(pracEdit->text().toUTF8());
        Wt::log("notice") << "SILW::search()";

        pracResultStack_->clear();

        mergeContainer = new PhysicianRecordListContainerWidget(conninfo_, session_, pracResultStack_);
        mergeContainer->showSearch(upper, prac);
        pracResultStack_->setCurrentWidget(mergeContainer);
        mergeContainer->selectPhysicianRecord().connect(this, &PracticeListContainerWidget::mergePhysicianRecords);

    }));


    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(practiceDetails,
             "Provider Details", Wt::WTabWidget::PreLoading);
    tabW->addTab(editPractice,
             "Edit Provider Details", Wt::WTabWidget::PreLoading);
    tabW->addTab(visitation,
             "PCC Visits", Wt::WTabWidget::PreLoading);
    tabW->addTab(physissues,
             "Issues", Wt::WTabWidget::PreLoading);
    tabW->addTab(physaddress,
             "Address", Wt::WTabWidget::PreLoading);
    tabW->addTab(mergephysician,
             "Merge Tool", Wt::WTabWidget::PreLoading);

    tabW->setStyleClass("tabwidget");

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", pracdialog->footer());
    pracdialog->rejectWhenEscapePressed();

    Wt::WPushButton *alright = new Wt::WPushButton("Done", pracdialog->footer());

    cancel->clicked().connect(std::bind([=]() {
        pracdialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    alright->clicked().connect(std::bind([=]() {
        pracdialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    container->show();
    pracdialog->show();
}
void PracticeListContainerWidget::addPhysicianAddress(Wt::WString  aid) {
    
   
    Wt::log("info") << "############################";
    Wt::log("info") << "ADDRESS ID: " << aid;
    Wt::log("info") << "############################";

    Wt::WDialog *addrdialog = new Wt::WDialog();
    Wt::WContainerWidget *container = new Wt::WContainerWidget(addrdialog->contents());
    container->addWidget(new Wt::WText("<h4>Enter the Suite Number</h4>"));
    container->addWidget(new Wt::WText("<h5>You can leave this blank if there is no suite number, just click Save</h5>"));
    WPushButton *saveline2 = new Wt::WPushButton("Save", addrdialog->footer());
    saveline2->setDefault(true);
    saveline2->setStyleClass("save-prac-line2");
    
    Wt::WLineEdit *pracLine2_ = new Wt::WLineEdit();
    pracLine2_->setStyleClass("left-box");
    pracLine2_->setPlaceholderText("Unit, suite number, etc");
    container->addWidget(pracLine2_);
    
    saveline2->clicked().connect(std::bind([=]() {

        std::string strLine2 = pracLine2_->text().toUTF8();
        dbo::Transaction mptrans(dbsession);
        
        Wt::WString address = aid;

        dbo::ptr<Practice> mprac = dbsession.find<Practice>().where("prac_id = ?").bind(globalPracticeId->text().toUTF8());
        mprac.modify()->addrLine2_ = strLine2;
        mprac.modify()->addressId_ = address.toUTF8();
        mptrans.commit();
        
        addrdialog->accept();
        pracdialog->accept();

    }));

    addrdialog->show();

}

void PracticeListContainerWidget::mergePhysicianRecords(WString aid) {
    //set current physician id local
    Wt::log("info") << "mergePhysicianRecords Start";
    WString currentid = globalPracticeId->text();
    Wt::log("info") << "set globalPracticeId";
        
    //copy missing information from current prac to new prac
    dbo::Transaction modpractrans(dbsession);
    dbo::ptr<Practice> curprac = dbsession.find<Practice>().where("prac_id = ?").bind(currentid.toUTF8());    
    dbo::ptr<Practice> modprac = dbsession.find<Practice>().where("prac_id = ?").bind(aid.toUTF8());
    Wt::log("info") << "get current and new practice for mods";
    
    if (modprac->refPracticeName_ == "" && curprac->refPracticeName_ != "") {
        modprac.modify()->refPracticeName_ = curprac->refPracticeName_;
        Wt::log("info") << "Modify first name";

    }
    if (modprac->refPracticeNpi_ == "" && curprac->refPracticeNpi_ != "") {
        modprac.modify()->refPracticeNpi_ = curprac->refPracticeNpi_;
    }
    if (modprac->refPracticeEmail_ == "" && curprac->refPracticeEmail_ != "") {
        modprac.modify()->refPracticeEmail_ = curprac->refPracticeEmail_;
    }
    if (modprac->refSpecialty_ == "" && curprac->refSpecialty_ != "") {
        modprac.modify()->refSpecialty_ = curprac->refSpecialty_;
    }
    if (modprac->pracEmailOpt_ == "" && curprac->pracEmailOpt_ != "") {
        modprac.modify()->pracEmailOpt_ = curprac->pracEmailOpt_;
    }
    if (modprac->pccEmail_ == "" && curprac->pccEmail_ != "") {
        modprac.modify()->pccEmail_ = curprac->pccEmail_;
    }
    if (modprac->addressId_ == "" && curprac->addressId_ != "") {
        modprac.modify()->addressId_ = curprac->addressId_;
    }
    if (modprac->localityName_ == "" && curprac->localityName_ != "") {
        modprac.modify()->localityName_ = curprac->localityName_;
    }
    if (modprac->localityId_ == "" && curprac->localityId_ != "") {
        modprac.modify()->localityId_ = curprac->localityId_;
    }
    if (modprac->groupId_ == "" && curprac->groupId_ != "") {
        modprac.modify()->groupId_ = curprac->groupId_;
    }   
    if (modprac->officeContact_ == "" && curprac->officeContact_ != "") {
        modprac.modify()->officeContact_ = curprac->officeContact_;
    }
    if (modprac->addrLine2_ == "" && curprac->addrLine2_ != "") {
        modprac.modify()->addrLine2_ = curprac->addrLine2_;
    }
    
    modpractrans.commit();
    Wt::log("info") << "mods done, start updating patients, referrals, studies";

    //find all patients with current physician id
    dbo::Transaction vtrans(dbsession);
    typedef dbo::collection< dbo::ptr<Patient> > Patients;
    Patients patients= dbsession.find<Patient>().where("prac_id = ?").bind(currentid.toUTF8());
    //for each patient, modify the prac_id to new physician id

    for (Patients::const_iterator pt = patients.begin(); pt != patients.end(); ++pt)
    {

        dbo::Transaction modptrans(dbsession);

        dbo::ptr<Patient> pat = dbsession.find<Patient>().where("pt_id = ?").bind((*pt)->ptId_);
        pat.modify()->pracId_ = aid.toUTF8();

        modptrans.commit();
    }
    
    typedef dbo::collection< dbo::ptr<Referral> > Referrals;
    Referrals referrals = dbsession.find<Referral>().where("prac_id = ?").bind(currentid.toUTF8());
    //for each patient, modify the prac_id to new physician id

    for (Referrals::const_iterator ref = referrals.begin(); ref != referrals.end(); ++ref)
    {

        dbo::Transaction modrtrans(dbsession);

        dbo::ptr<Referral> r = dbsession.find<Referral>().where("ref_id = ?").bind((*ref)->referralId_.toUTF8());
        r.modify()->refPracticeId_ = aid.toUTF8();

        modrtrans.commit();
    }
    
    typedef dbo::collection< dbo::ptr<SleepStudy> > Studies;
    Studies studies = dbsession.find<SleepStudy>().where("prac_id = ?").bind(currentid.toUTF8());
    //for each patient, modify the prac_id to new physician id

    for (Studies::const_iterator st = studies.begin(); st != studies.end(); ++st)
    {

        dbo::Transaction modstrans(dbsession);

        dbo::ptr<SleepStudy> stud = dbsession.find<SleepStudy>().where("std_id = ?").bind((*st)->studyId_.toUTF8());
        stud.modify()->practiceId_ = aid.toUTF8();

        modstrans.commit();
    }
    
    typedef dbo::collection< dbo::ptr<PhysicianVisit> > Visits;
    Visits visits = dbsession.find<PhysicianVisit>().where("prac_id = ?").bind(currentid.toUTF8());
    //for each patient, modify the prac_id to new physician id

    for (Visits::const_iterator vt = visits.begin(); vt != visits.end(); ++vt)
    {

        dbo::Transaction modvtrans(dbsession);

        dbo::ptr<PhysicianVisit> vis = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind((*vt)->visitId_.toUTF8());
        vis.modify()->practiceId_ = aid.toUTF8();
        modvtrans.commit();
    }

    typedef dbo::collection< dbo::ptr<PhysicianIssue> > Issues;
    Issues issues = dbsession.find<PhysicianIssue>().where("prac_id = ?").bind(currentid.toUTF8());
    //for each patient, modify the prac_id to new physician id

    for (Issues::const_iterator is = issues.begin(); is != issues.end(); ++is)
    {

        dbo::Transaction modistrans(dbsession);

        dbo::ptr<PhysicianIssue> iss = dbsession.find<PhysicianIssue>().where("issue_id = ?").bind((*is)->issueId_.toUTF8());
        iss.modify()->practiceId_ = aid.toUTF8();
        modistrans.commit();
    }



    Wt::log("info") << "done updating patients, referrals, studies, visits, issues";

    Wt::log("info") << "start removal of duplicate practice";

    dbo::Transaction rmtrans(dbsession);
    dbo::ptr<Practice> rmprac = dbsession.find<Practice>().where("prac_id = ?").bind(currentid.toUTF8());
    rmprac.remove();
    rmtrans.commit();

    Wt::log("info") << "done removing duplicate practice";

    pracdialog->accept();

}

void PracticeListContainerWidget::showPhysicianVisitDialog(Wt::WString vid)
{

    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction transaction(dbsession);

    dbo::ptr<PhysicianVisit> visit = dbsession.find<PhysicianVisit>().where("visit_id = ?").bind(vid.toUTF8());

    Wt::WString status = visit->visitIssues_;
    
    transaction.commit();

    if (status == "Complete") {
        PracticeListContainerWidget::showCompletedVisit(vid);
    } else {
        PracticeListContainerWidget::showPhysicianVisitComplete(vid);
    }

}
void PracticeListContainerWidget::showCompletedVisit(Wt::WString vid)
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

        PracticeListContainerWidget::newPhysicianIssueDialog(pid, vid);
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

WString PracticeListContainerWidget::setVisitId()
{
    return uuid();
}

void PracticeListContainerWidget::newPhysicianVisitStart(Wt::WString prac)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    staticVisit_ = new Wt::WText();
    staticVisit_->setText(setVisitId());

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

    WString pname = practice->refPracticeName_;
    WString pzip = practice->refPracticeZip_;
    WString pdrfirst = practice->refPracticeDrFirst_;
    WString pdrlast = practice->refPracticeDrLast_;
    WString pnpi = practice->refPracticeNpi_;
    WString pemail = practice->refPracticeEmail_;
    WString pfax = practice->refPracticeEmail_;
    WString pspec = practice->refSpecialty_;

    transaction.commit();

    Wt::WDialog *newvisitdialog = new Wt::WDialog("Add a New Visit Log Entry");
    newvisitdialog->setMinimumSize(1000, 700);
    newvisitdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newvisitdialog->contents());

    Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
    newprcontainer->setLayout(pthbox);

    Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

    pthbox->addLayout(leftbox);
    pthbox->addLayout(rightbox);

    Wt::WPushButton *addvisit = new Wt::WPushButton("Submit", newvisitdialog->footer());
    addvisit->setDefault(true);

    Wt::WComboBox *visittypecb = new Wt::WComboBox();
    visittypecb->setStyleClass("cal-month-selection");
    visittypecb->addItem("Over the Phone");
    visittypecb->addItem("In Person");
    
    Wt::WText *visitCbLbl = new Wt::WText("Visit Type");
    visitCbLbl->setStyleClass("label-left-box");
    leftbox->addWidget(visitCbLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(visittypecb);
    leftbox->addWidget(new Wt::WBreak());

    Wt::WText *visitReasonLbl = new Wt::WText("Reason for Visit");
    visitReasonLbl->setStyleClass("label-left-box");
    visitReason_ = new Wt::WLineEdit(this);
    visitReason_->setStyleClass("left-box");
    leftbox->addWidget(visitReasonLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(visitReason_);
    leftbox->addWidget(new Wt::WBreak());

    addvisit->clicked().connect(std::bind([=]() {

        Wt::WDateTime visitstart = Wt::WDateTime::currentDateTime();
        Wt::WString visitstatus = "In Progress";
        Wt::WString staffEmail = session_->strUserEmail();

        Wt::WString pccname = staffEmail;
        Wt::WString pracname = pdrfirst + " " + pdrlast;
        Wt::WString visitreason = visitReason_->text();

        dbo::Transaction ptrans(dbsession);

        dbo::ptr<PhysicianVisit> pv = dbsession.add(new PhysicianVisit());

        pv.modify()->visitStartTime_ = visitstart;
        pv.modify()->visitIssues_ = visitstatus;
        pv.modify()->pccName_ = pccname;
        pv.modify()->visitType_ = visittypecb->currentText();
        pv.modify()->practiceName_ = pracname;
        pv.modify()->reasonForVisit_ = visitreason;
        pv.modify()->visitId_ = staticVisit_->text().toUTF8();
        pv.modify()->practiceId_ = prac;

        ptrans.commit();

        newvisitdialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal

    }));

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newvisitdialog->footer());
    newvisitdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newvisitdialog, &Wt::WDialog::reject);
    newprcontainer->show();
    newvisitdialog->show();
}

void PracticeListContainerWidget::showPhysicianVisitComplete(Wt::WString vid)
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
            PracticeListContainerWidget::newPhysicianIssueDialog(pid, vid);
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

void PracticeListContainerWidget::showPhysicianIssueDialog(Wt::WString isid)
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

WString PracticeListContainerWidget::setIssueId()
{
    return uuid();
}

void PracticeListContainerWidget::newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid)
{
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    staticIssue_ = new Wt::WText();
    staticIssue_->setText(setVisitId());

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

void PracticeListContainerWidget::showCreateAccount(WString prac)
{

    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    Wt::log("info") << "createPhysicianLogin";
    
    Wt::WDialog *dialog = new Wt::WDialog("Create Clinicore Login");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);    
    
    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());


    dbo::Transaction transaction(dbsession);
    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

    if (practice->clinicoreAccount_ != "Yes")
    {

        Wt::Auth::User user = Wt::Auth::User(boost::lexical_cast<std::string>(practice.id()), pracsession_->practices());
        Wt::Auth::Login login;
        Wt::WString pccemail = session_->strUserEmail();
        Wt::Auth::PhysicianRegistrationModel *result = new Wt::Auth::PhysicianRegistrationModel(PracticeSession::auth(), pracsession_->practices(), login, user);
        Wt::Auth::PhysicianRegistrationWidget *w = new Wt::Auth::PhysicianRegistrationWidget(prac, pccemail, pracsession_);

        result->addPasswordAuth(&PracticeSession::absPasswordAuth());

        w->setModel(result);

        container->addWidget(w);

    } else {
        container->addWidget(new Wt::WText("<h4>Clinicore Account already exisits for this Physician</h4>"));
    }

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    Wt::WPushButton *alright = new Wt::WPushButton("Done", dialog->footer());

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    container->show();
    dialog->show();
}
