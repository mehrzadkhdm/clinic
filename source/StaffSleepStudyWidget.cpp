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
#include <Wt/WTimer>

//other header files
#include "StaffSleepStudyWidget.h"
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
#include "SleepStudyListContainerWidget.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

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


StaffSleepStudyWidget::StaffSleepStudyWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
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
    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<StudyType>("studytype");

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

WString StaffSleepStudyWidget::setPtId()
{
    return uuid();
}


WString StaffSleepStudyWidget::setStdId()
{
    return uuid();
}

WString StaffSleepStudyWidget::setRefId()
{
    return uuid();
}

WString StaffSleepStudyWidget::setPracId()
{
    return uuid();
}

WString StaffSleepStudyWidget::setCommentId()
{
    return uuid();
}

void StaffSleepStudyWidget::newStudy()
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
                btnSelectPatient->clicked().connect(boost::bind(&StaffSleepStudyWidget::selectPracticeDialog, this, (*pi)->ptId_));

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
        StaffSleepStudyWidget::newStudyAddPatient();
    }));

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();

}

void StaffSleepStudyWidget::ptsearch(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    ptResultStack_->clear();

    ptContainer = new PatientListContainerWidget(conninfo_, session_, ptResultStack_);
    ptContainer->showStudySearch(searchString, limit, offset);

    ptResultStack_->addWidget(ptContainer);

}

void StaffSleepStudyWidget::newStudyAddPatient()
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
        StaffSleepStudyWidget::selectPracticeDialog(staticptid);
    }));


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newptdialog->footer());
    newptdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newptdialog, &Wt::WDialog::reject);

    newptdialog->show();

}

void StaffSleepStudyWidget::selectPracticeDialog(std::string pt)
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
        btnSelectPractice->clicked().connect(boost::bind(&StaffSleepStudyWidget::createReferral, this, (*i)->practiceId_, pt));
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
        StaffSleepStudyWidget::newStudyAddProvider(pt);
    }));

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newstdialog->footer());
    newstdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newstdialog, &Wt::WDialog::reject);

    newstdialog->show();


}

void StaffSleepStudyWidget::newStudyAddProvider(std::string pt)
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
        StaffSleepStudyWidget::selectPracticeDialog(pt);
    }));

    newprcontainer->addWidget(addprac);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", newpracdialog->footer());
    newpracdialog->rejectWhenEscapePressed();

    cancel->clicked().connect(newpracdialog, &Wt::WDialog::reject);

    newpracdialog->show();

}

void StaffSleepStudyWidget::createReferral(std::string prac, std::string pt)
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
    }));

    schedule->clicked().connect(std::bind([=]() {

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

        Wt::WContainerWidget *typeContainer = new Wt::WContainerWidget();

        Wt::WLabel *lblStudyType = new Wt::WLabel("Study Type:");
        lblStudyType->setStyleClass("sig-lbl");

        Wt::WHBoxLayout *typebox = new Wt::WHBoxLayout();
        typebox->addWidget(lblStudyType);

        dbo::Transaction instrans(dbsession);

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

        instrans.commit();



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

        vbox1->addWidget(typeContainer);

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


        }));

        cl->clicked().connect(stdialog, &Wt::WDialog::reject);

        stdialog->show();

    }));
    newrefdialog->show();
    //save referral to database
}


void StaffSleepStudyWidget::sleepStudyList()
{

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(30000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        StaffSleepStudyWidget::search("");
    }));

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *newSleepStudyBtn = new Wt::WPushButton("New Sleep Study");

    container->addWidget(newSleepStudyBtn);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit();
    edit->setStyleClass("search-box");
    edit->setPlaceholderText("Search for a Study by patient name");

    container->addWidget(edit);



    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();
    Wt::WString searchPatient;

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {

       WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
       StaffSleepStudyWidget::search(upper);

       if (edit->text() == "")
       {
            StaffSleepStudyWidget::startRefreshTimer();
       } else {
               StaffSleepStudyWidget::stopRefreshTimer();
       }

    }));

    addWidget(container);
    WString outerUpper = boost::to_upper_copy<std::string>(edit->text().toUTF8());

    newSleepStudyBtn->clicked().connect(std::bind([=]() {
        StaffSleepStudyWidget::newStudy();
    }));


    StaffSleepStudyWidget::search(Wt::WString(""));
    StaffSleepStudyWidget::startRefreshTimer();

}
void StaffSleepStudyWidget::preSearch(Wt::WString searchString)
{
    StaffSleepStudyWidget::search(Wt::WString(searchString));
    StaffSleepStudyWidget::startRefreshTimer();
}
void StaffSleepStudyWidget::stopRefreshTimer()
{
    if (refreshTimer->isActive())
    {
        log("notice") << "REFRESH TIMER IS ACTIVE, STOPPING...";
        refreshTimer->stop();
    } else {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, SKIPPING...";
    }

}
void StaffSleepStudyWidget::startRefreshTimer()
{
    if (refreshTimer->isActive() == false)
    {
        log("notice") << "REFRESH TIMER IS NOT ACTIVE, STARTING...";
        refreshTimer->start();
    } else {
        log("notice") << "REFRESH TIMER IS ALREADY ACTIVE, SKIPPING...";

    }

}
void StaffSleepStudyWidget::search(Wt::WString searchString)
{
    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    stContainer = new SleepStudyListContainerWidget(conninfo_, session_, resultStack_);
    stContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(stContainer);

    stContainer->stopTimer().connect(std::bind([=] () {
        log("info") << "Caught stopTimer Signal";
        StaffSleepStudyWidget::stopRefreshTimer();
    }));

    stContainer->done().connect(std::bind([=] () {
        log("info") << "Caught done Signal";
        StaffSleepStudyWidget::preSearch(searchString);
    }));
}
