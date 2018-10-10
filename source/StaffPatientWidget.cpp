/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std stuff
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
#include <Wt/WStackedWidget>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

#include <Wt/WTemplate>
#include <Wt/WDate>
#include <Wt/WDateValidator>
#include <Wt/WLabel>
#include <Wt/WDateEdit>
#include <Wt/WPushButton>
#include <Wt/WValidator>


//other header files
#include "StaffPatientWidget.h"
#include "PatientListContainerWidget.h"
#include "PhysicianRecordListContainerWidget.h"
#include "Patient.h"
#include "Practice.h"
#include "Backup.h"
#include "UserSession.h"
#include "Insurance.h"
#include "InsuranceItem.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

StaffPatientWidget::StaffPatientWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Backup>("backup");
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

void StaffPatientWidget::patientList()
{

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(30000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        StaffPatientWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *addPatient = new Wt::WPushButton("New Patient");
    container->addWidget(addPatient);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for a Patient");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPatient;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        StaffPatientWidget::search(upper);

        if (edit->text() == "")
        {
            StaffPatientWidget::startRefreshTimer();
        } else {
            StaffPatientWidget::stopRefreshTimer();
        }

    }));

    addPatient->clicked().connect(std::bind([=] () {
        StaffPatientWidget::addPatientDialog();
    }));

    addWidget(container);

    StaffPatientWidget::search(Wt::WString(""));
    StaffPatientWidget::startRefreshTimer();

}

void StaffPatientWidget::preSearch(Wt::WString searchString)
{
    StaffPatientWidget::search(Wt::WString(searchString));
    StaffPatientWidget::startRefreshTimer();
}
void StaffPatientWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }
}
void StaffPatientWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}
void StaffPatientWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    ptContainer = new PatientListContainerWidget(conninfo_, session_, resultStack_);
    ptContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(ptContainer);

    ptContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        StaffPatientWidget::stopRefreshTimer();
    }));

    ptContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        StaffPatientWidget::preSearch(searchString);
    }));

}

WString StaffPatientWidget::setPtId()
{
    return uuid();
}

WString StaffPatientWidget::setPracId()
{
    return uuid();
}

void StaffPatientWidget::addPatientDialog()
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


    Wt::WPushButton *addpt = new Wt::WPushButton("Continue", newptdialog->footer());
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
    ptHomeLbl->setStyleClass("label-left-box");
    patientHomePhone_ = new Wt::WLineEdit(this);
    patientHomePhone_->setStyleClass("left-box");
    leftbox->addWidget(ptHomeLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientHomePhone_);

    Wt::WText *ptCellLbl = new Wt::WText("Patient Cell Phone");
    ptCellLbl->setStyleClass("label-left-box");
    patientCellPhone_ = new Wt::WLineEdit(this);
    patientCellPhone_->setStyleClass("left-box");
    leftbox->addWidget(ptCellLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientCellPhone_);

    Wt::WContainerWidget *dob = new Wt::WContainerWidget();

    Wt::WText *ptDobLbl = new Wt::WText("Patient Date of Birth");
    ptDobLbl->setStyleClass("label-left-box");
    leftbox->addWidget(ptDobLbl);
    leftbox->addWidget(new Wt::WBreak());
    
    Wt::WTemplate *t = new Wt::WTemplate(Wt::WString::tr("date-template"), dob);
    t->addFunction("id", &Wt::WTemplate::Functions::id);

    Wt::WDateEdit *dateEdit = new Wt::WDateEdit(dob);
    t->bindWidget("birth-date", dateEdit);

    Wt::WDateValidator *dv = new Wt::WDateValidator();
    dv->setBottom(Wt::WDate(1900, 1, 1));
    dv->setTop(Wt::WDate::currentDate());
    dv->setFormat("MM/dd/yyyy");
    dv->setMandatory(true);
    dv->setInvalidBlankText("A birthdate is mandatory!");
    dv->setInvalidNotADateText("You should enter a date in the format "
                   "\"MM/dd/yyyy\"!");
    dv->setInvalidTooEarlyText
        (Wt::WString("That's too early... The date must be {1} or later!"
             "").arg(dv->bottom().toString("MM/dd/yyyy")));
    dv->setInvalidTooLateText
        (Wt::WString("That's too late... The date must be {1} or earlier!"
             "").arg(dv->top().toString("MM/dd/yyyy")));

    dateEdit->setValidator(dv);

    leftbox->addWidget(dob);


    Wt::WText *ptEmailLbl = new Wt::WText("Patient Email Address");
    ptEmailLbl->setStyleClass("label-right-box");
    patientEmail_ = new Wt::WLineEdit(this);
    patientEmail_->setStyleClass("right-box");
    leftbox->addWidget(ptEmailLbl);
    leftbox->addWidget(new Wt::WBreak());
    leftbox->addWidget(patientEmail_);

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
            log("notice") << "Selected Insurance ID: "<< selected.insuranceId;
            studyInsuranceId_ = selected.insuranceId;
            studyInsuranceName_ = selected.insuranceName;

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
    leftbox->addWidget(insuranceContainer);
    
    if (!dateEdit->date().isValid()) {
        addpt->hide();
    } else {
        addpt->show();
    }

    dateEdit->changed().connect(std::bind([=]() {
        if (dateEdit->date().isValid()) {
            addpt->show();
        } else {
            addpt->hide();
        }
    }));
    
    addpt->clicked().connect(std::bind([=]() {

        
        Wt::WString dobout;
        dobout = dateEdit->date().toString("MM/dd/yyyy");

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

        pt.modify()->ptInsuranceId_ = studyInsuranceId_.toUTF8();
        pt.modify()->ptInsuranceName_ = studyInsuranceName_.toUTF8();

        ptrans.commit();

        std::string staticptid;
        staticptid = std::string(staticPt_->text().toUTF8());

        newptdialog->accept();
        StaffPatientWidget::selectPracticeDialog(staticPt_->text().toUTF8());

    }));


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newptdialog->footer());
    newptdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newptdialog, &Wt::WDialog::reject);

    newptdialog->show();

}
void StaffPatientWidget::selectPracticeDialog(std::string pt)
{
    newstdialog = new Wt::WDialog("Select Provider");

    newstdialog->setMinimumSize(1000, 700);
    newstdialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *newstcontainer = new Wt::WContainerWidget(newstdialog->contents());
    
    Wt::WPushButton *addprovider = new Wt::WPushButton("New Provider", newstdialog->footer());
    addprovider->setStyleClass("new-pt-add-provider-btn");
    addprovider->setDefault(true);
    
    Wt::WContainerWidget *listphysician = new Wt::WContainerWidget();

    WText *listheader = new Wt::WText("<h4>Search for Physician</h4>", listphysician);
    Wt::WLineEdit *pracEdit = new Wt::WLineEdit(listphysician);
    pracEdit->setPlaceholderText("Search for physician by name");
    pracEdit->setStyleClass("search-box");

    Wt::WStackedWidget *pracResultStack_;

    pracResultStack_ = new Wt::WStackedWidget(listphysician);


    pracEdit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(pracEdit->text().toUTF8());
        Wt::log("notice") << "SILW::search()";

        pracResultStack_->clear();

        physicianContainer = new PhysicianRecordListContainerWidget(conninfo_, session_, pracResultStack_);
        physicianContainer->showSearchOnly(upper);
        pracResultStack_->setCurrentWidget(physicianContainer);
        physicianContainer->selectPhysicianRecord().connect(this, &StaffPatientWidget::addPracList);

    }));

    newstcontainer->addWidget(listphysician);

    //allow user to select a provider with select button

    addprovider->clicked().connect(std::bind([=]() {
        StaffPatientWidget::newReferralAddProvider(pt);
        newstdialog->accept();
    }));

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newstdialog->footer());
    newstdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();

}
void StaffPatientWidget::addPracList(Wt::WString asid) {
    StaffPatientWidget::addPracPt(asid.toUTF8(), staticPt_->text().toUTF8());
    newstdialog->accept();
}
void StaffPatientWidget::newReferralAddProvider(std::string pt)
{
    staticPrac_ = new Wt::WText();
    staticPrac_->setText(setPracId());

    Wt::WDialog *newpracdialog = new Wt::WDialog("Add a New Provider");
    newpracdialog->setMinimumSize(1000, 700);
    newpracdialog->setMaximumSize(1000, 700);
    Wt::WContainerWidget *newprcontainer = new Wt::WContainerWidget(newpracdialog->contents());
    newprcontainer->setStyleClass("newpr-container");
    Wt::WPushButton *addprac = new Wt::WPushButton("Continue", newpracdialog->footer());
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

    Wt::WText *pracNpiLbl = new Wt::WText("Practice Phone Number");
    pracNpiLbl->setStyleClass("label-right-box");
    pracNpi_ = new Wt::WLineEdit(this);
    pracNpi_->setStyleClass("right-box");
    newprcontainer->addWidget(pracNpiLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracNpi_);
    newprcontainer->addWidget(new Wt::WBreak());
    
    Wt::WText *pracFaxLbl = new Wt::WText("Practice Fax Number");
    pracFaxLbl->setStyleClass("label-right-box");
    pracFax_ = new Wt::WLineEdit(this);
    pracFax_->setStyleClass("right-box");
    newprcontainer->addWidget(pracFaxLbl);
    newprcontainer->addWidget(new Wt::WBreak());
    newprcontainer->addWidget(pracFax_);
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
        pr.modify()->refPracticeNpi_ = pracNpi_->text().toUTF8();
        pr.modify()->pracFax_ = pracFax_->text().toUTF8();
        pr.modify()->refSpecialty_ = pracSpecialty_->text().toUTF8();
        pr.modify()->refPracticeEmail_ = pracEmail_->text().toUTF8();
        pr.modify()->practiceId_ = staticPrac_->text().toUTF8();

        ptrans.commit();



        newpracdialog->accept();
        StaffPatientWidget::backToPrac(staticPrac_->text().toUTF8(), pt);
    }));

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newpracdialog->footer());
    newpracdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newpracdialog, &Wt::WDialog::reject);

    newpracdialog->show();

}

void StaffPatientWidget::backToPrac(std::string prac, std::string pt)
{
        StaffPatientWidget::selectPracticeDialog(pt);
}

void StaffPatientWidget::addPracPt(std::string prac, std::string pt)
{
        dbo::Transaction ptrans(dbsession);

        dbo::ptr<Patient> pat = dbsession.find<Patient>().where("pt_id = ?").bind(pt);
        pat.modify()->pracId_ = prac;

        ptrans.commit();

        StaffPatientWidget::search(Wt::WString(""));
}
