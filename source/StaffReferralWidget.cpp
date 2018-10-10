/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
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
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WDate>
#include <Wt/WStackedWidget>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "Composer.h"

#include "InboundContainerWidget.h"
#include "StaffReferralWidget.h"
#include "Referral.h"
#include "Practice.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "InsuranceItem.h"
#include "Insurance.h"
#include "UserSession.h"
using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
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

StaffReferralWidget::StaffReferralWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session),
    conninfo_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
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

void StaffReferralWidget::searchList()
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    Wt::WPushButton *newref = new Wt::WPushButton("New Referral");
    Wt::WLineEdit *edit = new Wt::WLineEdit();
    edit->setStyleClass("search-box");
    edit->setPlaceholderText("Search for a referral by patient name, doctor name, or practice name");

    container->addWidget(newref);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(edit);

    newref->clicked().connect(std::bind([=]() {
        StaffReferralWidget::newReferral();
    }));

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPatient;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
           WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
       StaffReferralWidget::search(upper);

    }));

    addWidget(container);

    StaffReferralWidget::search(Wt::WString(""));


}

void StaffReferralWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    inboundContainer = new InboundContainerWidget(conninfo_, session_, resultStack_);
    inboundContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(inboundContainer);

}
WString StaffReferralWidget::setPtId()
{
    return uuid();
}


WString StaffReferralWidget::setStdId()
{
    return uuid();
}

WString StaffReferralWidget::setRefId()
{
    return uuid();
}

WString StaffReferralWidget::setPracId()
{
    return uuid();
}

WString StaffReferralWidget::setCommentId()
{
    return uuid();
}



void StaffReferralWidget::newReferral()
{
    Wt::WDialog *newstdialog = new Wt::WDialog("New Sleep Study");
    newstdialog->setMinimumSize(1000, 700);
    newstdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newstcontainer = new Wt::WContainerWidget(newstdialog->contents());
    newstcontainer->setStyleClass("new-referral-pt-list");
    Wt::WVBoxLayout *newstvbox = new Wt::WVBoxLayout();
    Wt::WHBoxLayout *newsthbox = new Wt::WHBoxLayout();
    newstcontainer->setLayout(newstvbox);

    Wt::WText *searchPtFirstLbl = new Wt::WText("Search for Patient by their last name: ");
    searchPtFirstLbl->setStyleClass("label-left-box");
    newstvbox->addWidget(searchPtFirstLbl);

    newstvbox->addLayout(newsthbox);

    Wt::WPushButton *addpt = new Wt::WPushButton("New Patient");


    searchPatientFirst_ = new Wt::WLineEdit(this);
    searchPatientFirst_->setStyleClass("left-box");
    //newptcontainer->addWidget(new Wt::WBreak());
    newsthbox->addWidget(searchPatientFirst_);



    Wt::WPushButton *searchpt = new Wt::WPushButton("Search");
    searchpt->setDefault(true);
    newsthbox->addWidget(searchpt);

    Wt::WContainerWidget *newstbottom = new Wt::WContainerWidget(newstdialog->contents());
    newstbottom->addWidget(addpt);

    searchpt->clicked().connect(std::bind([=]() {

        addpt->hide();
        searchpt->hide();
        newstcontainer->clear();

        std::string strSearchPtFirst = searchPatientFirst_->text().toUTF8();
            std::string searchUpperPtFirst = '%' + boost::to_upper_copy<std::string>(strSearchPtFirst) +'%';

        dbo::Transaction showpttrans(dbsession);

        typedef dbo::collection< dbo::ptr<Patient> > Patients;
        Patients patients = dbsession.find<Patient>().where("pt_first LIKE ? OR pt_last LIKE ?").bind(searchUpperPtFirst).bind(searchUpperPtFirst);

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
                btnSelectPatient = new Wt::WPushButton("New Referral", showptable->elementAt(prow, 4)),
                btnSelectPatient->clicked().connect(boost::bind(&StaffReferralWidget::selectPracticeDialog, this, (*pi)->ptId_));

                btnSelectPatient->clicked().connect(std::bind([=]() {
                    newstdialog->accept();
                }));

        }

        showpttrans.commit();

        showptable->addStyleClass("table form-inline");
        showptable->addStyleClass("table table-striped");
        showptable->addStyleClass("table table-hover");

        newstcontainer->addWidget(showptable);
    }));


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newstdialog->footer());
    newstdialog->rejectWhenEscapePressed();

    addpt->clicked().connect(std::bind([=]() {
        newstdialog->accept();
        StaffReferralWidget::newReferralAddPatient();
    }));

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();

}

void StaffReferralWidget::ptsearch(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    ptResultStack_->clear();

    ptContainer = new PatientListContainerWidget(conninfo_, session_, ptResultStack_);
    ptContainer->showReferralSearch(searchString, limit, offset);

    ptResultStack_->setCurrentWidget(ptContainer);

}

void StaffReferralWidget::newReferralAddPatient()
{
    //invoke find patient modal

    //create patient id
    staticPt_ = new Wt::WText();
    staticPt_->setText(setPtId());

    Wt::WDialog *newptdialog = new Wt::WDialog("Add a New Patient");
    newptdialog->setMinimumSize(1000, 700);
    newptdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newptcontainer = new Wt::WContainerWidget(newptdialog->contents());
    Wt::WHBoxLayout *pthbox = new Wt::WHBoxLayout();
    newptcontainer->setLayout(pthbox);

    Wt::WVBoxLayout *leftbox = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rightbox = new Wt::WVBoxLayout();

    pthbox->addLayout(leftbox);
    pthbox->addLayout(rightbox);

    Wt::WPushButton *addpt = new Wt::WPushButton("Add Patient", newptdialog->footer());
    addpt->setDefault(true);

    //input fields for patient demographics
    Wt::WText *ptFirstNameLbl = new Wt::WText("Patient First Name");
    ptFirstNameLbl->setStyleClass("label-left-box");
    patientFirstName_ = new Wt::WLineEdit(this);
    patientFirstName_->setStyleClass("left-box");
    leftbox->addWidget(ptFirstNameLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientFirstName_);

    Wt::WText *ptLastNameLbl = new Wt::WText("Patient Last Name");
    ptLastNameLbl->setStyleClass("label-left-box");
    patientLastName_ = new Wt::WLineEdit(this);
    patientLastName_->setStyleClass("left-box");
    leftbox->addWidget(ptLastNameLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientLastName_);

    Wt::WText *ptCityLbl = new Wt::WText("Patient City");
    ptCityLbl->setStyleClass("label-left-box");
    patientCity_ = new Wt::WLineEdit(this);
    patientCity_->setStyleClass("left-box");
    leftbox->addWidget(ptCityLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientCity_);

    Wt::WText *ptHomeLbl = new Wt::WText("Patient Home Phone");
    ptHomeLbl->setStyleClass("label-right-box");
    patientHomePhone_ = new Wt::WLineEdit(this);
    patientHomePhone_->setStyleClass("right-box");
    leftbox->addWidget(ptHomeLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientHomePhone_);

    Wt::WText *ptCellLbl = new Wt::WText("Patient Cell Phone");
    ptCellLbl->setStyleClass("label-right-box");
    patientCellPhone_ = new Wt::WLineEdit(this);
    patientCellPhone_->setStyleClass("right-box");
    leftbox->addWidget(ptCellLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientCellPhone_);

    Wt::WText *ptDobLbl = new Wt::WText("Patient Date of Birth");
    ptDobLbl->setStyleClass("label-left-box");
    leftbox->addWidget(ptDobLbl);
    leftbox->addWidget(new Wt::WBreak());

    Wt::WContainerWidget *dob = new Wt::WContainerWidget();
    dob->setStyleClass("dob-container");
    Wt::WHBoxLayout *dobhbox = new Wt::WHBoxLayout();

    dob->setLayout(dobhbox);

    dobMonth = new Wt::WLineEdit();
    dobDay = new Wt::WLineEdit();
    dobYear = new Wt::WLineEdit();

    dobMonth->setStyleClass("dob-month");
    dobMonth->setPlaceholderText("Month");
    dobDay->setStyleClass("dob-day");
    dobDay->setPlaceholderText("Day");
    dobYear->setStyleClass("dob-year");
    dobYear->setPlaceholderText("Year");

    dobhbox->addWidget(dobMonth);
    dobhbox->addWidget(new Wt::WText("/"));
    dobhbox->addWidget(dobDay);
    dobhbox->addWidget(new Wt::WText("/"));
    dobhbox->addWidget(dobYear);

    leftbox->addWidget(dob);

    Wt::WText *ptEmailLbl = new Wt::WText("Patient Email Address");
    ptEmailLbl->setStyleClass("label-right-box");
    patientEmail_ = new Wt::WLineEdit(this);
    patientEmail_->setStyleClass("right-box");
    leftbox->addWidget(ptEmailLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientEmail_);

    Wt::WContainerWidget *ptInsuranceContainer = new Wt::WContainerWidget();

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

            patInsuranceId_ = selected.insuranceId;
            patInsuranceName_ = selected.insuranceName;


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

    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(new Wt::WBreak());
    rightbox->addWidget(ptInsuranceContainer);

    addpt->clicked().connect(std::bind([=]() {


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

        std::string strDob = dobout.toUTF8();
          std::string ptUpperDob_ = boost::to_upper_copy<std::string>(strDob);

         std::string strEmail = patientEmail_->text().toUTF8();
          std::string ptUpperEmail_ = boost::to_upper_copy<std::string>(strEmail);


        dbo::Transaction ptrans(dbsession);

        dbo::ptr<Patient> pt = dbsession.add(new Patient());

        pt.modify()->ptFirstName_ = ptUpperFirst_;
        pt.modify()->ptLastName_ = ptUpperLast_;
        pt.modify()->ptCity_ = ptUpperCity_;
        pt.modify()->ptHomePhone_ = ptUpperHome_;
        pt.modify()->ptCellPhone_ = ptUpperCell_;
        pt.modify()->ptEmail_ = ptUpperEmail_;
        pt.modify()->ptDob_ = ptUpperDob_;
        pt.modify()->ptId_ = staticPt_->text().toUTF8();
        pt.modify()->ptInsuranceName_ = patInsuranceName_.toUTF8();
        pt.modify()->ptInsuranceId_ = patInsuranceId_.toUTF8();
        ptrans.commit();

        std::string staticptid;
        staticptid = std::string(staticPt_->text().toUTF8());

        newptdialog->accept();
        StaffReferralWidget::selectPracticeDialog(staticptid);
    }));


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newptdialog->footer());
    newptdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newptdialog, &Wt::WDialog::reject);

    newptdialog->show();

}

void StaffReferralWidget::selectPracticeDialog(std::string pt)
{

    Wt::WDialog *newstdialog = new Wt::WDialog("Select Provider");

    newstdialog->setMinimumSize(1000, 700);
    newstdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newstcontainer = new Wt::WContainerWidget(newstdialog->contents());
    newstcontainer->setStyleClass("new-pt-provider-list");

    Wt::WPushButton *addprovider = new Wt::WPushButton("New Provider", newstdialog->footer());
    addprovider->setStyleClass("new-pt-add-provider-btn");
    addprovider->setDefault(true);


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

        new Wt::WText((*i)->refPracticeName_, showptable->elementAt(row, 0)),
        new Wt::WText((*i)->refPracticeDrFirst_, showptable->elementAt(row, 1)),
        new Wt::WText((*i)->refPracticeDrLast_, showptable->elementAt(row, 2)),
        new Wt::WText((*i)->refPracticeZip_, showptable->elementAt(row, 3)),
        btnSelectPractice = new Wt::WPushButton("Select", showptable->elementAt(row, 6)),
        //select Practice id, trigger newReferral function
        btnSelectPractice->clicked().connect(boost::bind(&StaffReferralWidget::createReferral, this, (*i)->practiceId_, pt));
        btnSelectPractice->clicked().connect(std::bind([=]() {
            newstdialog->accept();
        }));
    }

    showprtrans.commit();

    showptable->addStyleClass("table form-inline");
    showptable->addStyleClass("table table-striped");
    showptable->addStyleClass("table table-hover");

    newstcontainer->addWidget(showptable);

    //allow user to select a provider with select button

    addprovider->clicked().connect(std::bind([=]() {
        StaffReferralWidget::newReferralAddProvider(pt);
    }));

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newstdialog->footer());
    newstdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();


}

void StaffReferralWidget::newReferralAddProvider(std::string pt)
{
    staticPrac_ = new Wt::WText();
    staticPrac_->setText(setPracId());

    Wt::WDialog *newpracdialog = new Wt::WDialog("Add a New Provider");
    newpracdialog->setMinimumSize(1000, 700);
    newpracdialog->setMaximumSize(1000, 700);
    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newpracdialog->contents());
    newprcontainer->setStyleClass("newpr-container");
    Wt::WPushButton *addprac = new Wt::WPushButton("Continue", newpracdialog->contents());
    addprac->setDefault(true);

    //input fields for patient demographics
    Wt::WText *pracNameLbl = new Wt::WText("Practice Business Name");
    pracNameLbl->setStyleClass("label-left-box");
    pracName_ = new Wt::WLineEdit(this);
    pracName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracDrFirstNameLbl = new Wt::WText("Doctor First Name");
    pracDrFirstNameLbl->setStyleClass("label-left-box");
    pracDrFirstName_ = new Wt::WLineEdit(this);
    pracDrFirstName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracDrFirstNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracDrFirstName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracDrLastNameLbl = new Wt::WText("Doctor Last Name");
    pracDrLastNameLbl->setStyleClass("label-left-box");
    pracDrLastName_ = new Wt::WLineEdit(this);
    pracDrLastName_->setStyleClass("left-box");
    newprcontainer->addWidget(pracDrLastNameLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracDrLastName_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracZipLbl = new Wt::WText("Practice Zip");
    pracZipLbl->setStyleClass("label-left-box");
    pracZip_ = new Wt::WLineEdit(this);
    pracZip_->setStyleClass("left-box");
    newprcontainer->addWidget(pracZipLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracZip_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracNpiLbl = new Wt::WText("Practice Phone Number");
    pracNpiLbl->setStyleClass("label-right-box");
    pracNpi_ = new Wt::WLineEdit(this);
    pracNpi_->setStyleClass("right-box");
    newprcontainer->addWidget(pracNpiLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracNpi_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracSpecialtyLbl = new Wt::WText("Practice Specialty");
    pracSpecialtyLbl->setStyleClass("label-right-box");
    pracSpecialty_ = new Wt::WLineEdit(this);
    pracSpecialty_->setStyleClass("right-box");
    newprcontainer->addWidget(pracSpecialtyLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracSpecialty_);
    newprcontainer->addWidget(new Wt::WBreak());

    Wt::WText *pracEmailLbl = new Wt::WText("Practice Email Address");
    pracEmailLbl->setStyleClass("label-right-box");
    pracEmail_ = new Wt::WLineEdit(this);
    pracEmail_->setStyleClass("right-box");
    newprcontainer->addWidget(pracEmailLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracEmail_);
    newprcontainer->addWidget(new Wt::WBreak());

    addprac->clicked().connect(std::bind([=]() {

        dbo::Transaction ptrans(dbsession);

        dbo::ptr<Practice> pr = dbsession.add(new Practice());

        pr.modify()->refPracticeName_ = pracName_->text().toUTF8();
        pr.modify()->refPracticeDrFirst_ = pracDrFirstName_->text().toUTF8();
        pr.modify()->refPracticeDrLast_ = pracDrLastName_->text().toUTF8();
        pr.modify()->refPracticeZip_ = pracZip_->text().toUTF8();
        pr.modify()->refPracticeNpi_ = pracNpi_->text().toUTF8();
        pr.modify()->refSpecialty_ = pracSpecialty_->text().toUTF8();
        pr.modify()->refPracticeEmail_ = pracEmail_->text().toUTF8();
        pr.modify()->practiceId_ = staticPrac_->text().toUTF8();

        ptrans.commit();


        newpracdialog->accept();
        StaffReferralWidget::selectPracticeDialog(pt);
    }));

    newprcontainer->addWidget(addprac);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newpracdialog->footer());
    newpracdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newpracdialog, &Wt::WDialog::reject);

    newpracdialog->show();

}

void StaffReferralWidget::createReferral(std::string prac, std::string pt)
{
    log("info") << "PatientListContainerWidget::createReferral";
    log("info") << "PatientListContainerWidget::createReferral - prac" << prac << " , pt: " << pt;
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


    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    newrefcontainer->setLayout(hbox);
    newrefcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *rvbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rvbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(rvbox1);
    hbox->addLayout(rvbox2);

    Wt::WContainerWidget *refleft = new Wt::WContainerWidget(newrefdialog->contents());
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


    Wt::WContainerWidget *refright = new Wt::WContainerWidget(newrefdialog->contents());
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


    Wt::WPushButton *schedule = new Wt::WPushButton("Schedule Now", newrefdialog->footer());
    Wt::WPushButton *save = new Wt::WPushButton("Save for Later", newrefdialog->footer());

    Wt::WText* refout = new Wt::WText();
    refout->addStyleClass("help-block");

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
        refright->addWidget(new Wt::WText("<strong>You must select the Referral Received Date</strong>"));
        refright->addWidget(new Wt::WBreak());
    }    else {
        refright->addWidget(new Wt::WBreak());
        refright->addWidget(new Wt::WText("<strong>Patient Insurance: </strong>" + ptinsurancename));
        refright->addWidget(new Wt::WBreak());
    }

    //referral received at date selection widget.
    Wt::WCalendar *refcal = new Wt::WCalendar();
    refcal->setSelectionMode(Wt::ExtendedSelection);



    refcal->selectionChanged().connect(std::bind([=]() {
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

    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("<h5>Referral Received</h5>"));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(refcal);
    refleft->addWidget(refout);

    dbReferralDate_ = new Wt::WText();

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
    refright->addWidget(insuranceContainer);


    composer_ = new Composer(conninfo_, staticRef_->text(), this);

    rvbox1->addWidget(new Wt::WText("<h4>Upload Referral</h4>"));
    rvbox1->addWidget(new Wt::WText("Upload the Referral or Order PDF or image from the efax system!"));
    rvbox1->addWidget(composer_);


    rvbox1->addWidget(refleft);
    rvbox2->addWidget(refright);

    newrefcontainer->show();


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newrefdialog->footer());
    newrefdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newrefdialog, &Wt::WDialog::reject);


    save->clicked().connect(std::bind([=]() {

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
        r.modify()->patientId_ = pt;

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

        r.modify()->refStatus_ = refstatus.toUTF8();
        r.modify()->referralId_ = staticRef_->text().toUTF8();

        reftrans.commit();

        newrefdialog->accept();
        StaffReferralWidget::search(Wt::WString(""));
    }));
    newrefdialog->show();
}
