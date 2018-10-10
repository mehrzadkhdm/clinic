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

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>

//other header files
#include "PracticeReferralWidget.h"
#include "Referral.h"
#include "PracticeSession.h"
#include "Composer.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

PracticeReferralWidget::PracticeReferralWidget(const char *conninfo, Wt::WString prac, PracticeSession *session, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo),
prac_(prac),
conninfo_(conninfo),
session_(session)
{
    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<CompositeEntity>("composite");

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

void PracticeReferralWidget::start()
{
    form_ = new PracticeReferralView(this);
    staticRef_ = new Wt::WText();
    staticRef_->setText(setRefId());
    PracticeReferralWidget::demographics();
}

void PracticeReferralWidget::demographics()
{
    dialog_ = new Wt::WDialog("Review Referral Information");
    dialog_->setMinimumSize(1000, 700);
    dialog_->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog_->contents());
    
    reviewFirstName_ = new Wt::WText(this);
    container->addWidget(reviewFirstName_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewLastName_ = new Wt::WText(this);
    container->addWidget(reviewLastName_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewAddress1_ = new Wt::WText(this);
    container->addWidget(reviewAddress1_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewAddress2_ = new Wt::WText(this);
    container->addWidget(reviewAddress2_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewCity_ = new Wt::WText(this);
    container->addWidget(reviewCity_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewZip_ = new Wt::WText(this);
    container->addWidget(reviewZip_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewHomePhone_ = new Wt::WText(this);
    container->addWidget(reviewHomePhone_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewCellPhone_ = new Wt::WText(this);
    container->addWidget(reviewCellPhone_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewDob_ = new Wt::WText(this);
    container->addWidget(reviewDob_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewGender_ = new Wt::WText(this);
    container->addWidget(reviewGender_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    //vbox1->addWidget(new Wt::WBreak());

    reviewDiagnostic_ = new Wt::WText(this);
    container->addWidget(reviewDiagnostic_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewOvernight_ = new Wt::WText(this);
    container->addWidget(reviewOvernight_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewOtherSleepTest_ = new Wt::WText(this);
    container->addWidget(reviewOtherSleepTest_);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    reviewHomeStudy_ = new Wt::WText(this);
    container->addWidget(reviewHomeStudy_);
    container->addWidget(new Wt::WBreak());

    reviewAuthConsult_ = new Wt::WText(this);
    container->addWidget(reviewAuthConsult_);



    container->show();

    Wt::WPushButton *ok = new Wt::WPushButton("Yes, everything is correct", dialog_->footer());
    ok->setDefault(true);

    Wt::WPushButton *cancel = new Wt::WPushButton("No, I need to change something", dialog_->footer());
    dialog_->rejectWhenEscapePressed();

    ok->clicked().connect(std::bind([=]() {
        dialog_->accept();
        form_->hide();
        // container1->hide();
        // container2->hide();
        // container3->hide();
        // btnComplete->hide();
    }));
    ok->clicked().connect(this, &PracticeReferralWidget::complete);

    cancel->clicked().connect(dialog_, &Wt::WDialog::reject);

    form_->done().connect(this, &PracticeReferralWidget::oldCompleteClickCode);
}

void PracticeReferralWidget::oldCompleteClickCode(int foo)
{
    reviewFirstName_->setText("<strong>Patient First Name:</strong> " + form_->strFirstName());
    reviewLastName_->setText("<strong>Patient Last Name:</strong> " + form_->strLastName());



    reviewAddress1_->setText("<strong>Patient Address Line 1: </strong>" + form_->strAddress1());
    reviewAddress2_->setText("<strong>Patient Address Line 2: </strong>" + form_->strAddress2());

    //reviewCity_->setText("Patient City: " + patientCity_->text());
    reviewCity_->setText("<strong>Patient City:</strong> " + form_->strCity());

    reviewZip_->setText("<strong>Patient Zip Code: </strong>" + form_->strZip());
    reviewHomePhone_->setText("<strong>Patient Home Phone: </strong>" + form_->strHomePhone());
    reviewCellPhone_->setText("<strong>Patient Cell Phone:  </strong>" + form_->strCellPhone());
    reviewDob_->setText("<strong>Patient Date of Birth: </strong> " + form_->strDob());
    //pracDrSigName_->setText("Referring MD, Please sign here and select today's date and time.");

    reviewGender_->setText("<strong>Patient Gender:</strong> " + form_->strGender());


    reviewDiagnostic_->setText("<strong>Diagnosis: </strong> " + form_->strDiagnostic());

    reviewOvernight_->setText("<strong>Overnight Study: </strong> " + form_->strOvernight());

    //set reviewOtherSleepText to otheroutput
    reviewOtherSleepTest_->setText("<strong>Other Study: </strong> " + form_->strOtherSleepTest());

    //check home study
    if (form_->boolHomeStudy()){
        reviewHomeStudy_->setText("<strong>You have authorized a home sleep study.</strong>");
    }

    //check consult authorization
    if (form_->boolAuthConsult()){
        reviewAuthConsult_->setText("<strong>You have authorized our sleep specialists to consult your patient for Sleep Disorder treatment.</strong>");
    }
    dialog_->show();

}

//function to handle sending all to review and displaying

WString PracticeReferralWidget::setRefId()
{
    return uuid();
}

WString PracticeReferralWidget::strRefId()
{
    return staticRef_->text();
}
void PracticeReferralWidget::showDialog()
{
    Wt::WDialog *dialog = new Wt::WDialog("Electronic Signature");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WLabel *label1 = new Wt::WLabel("For security reasons, we need to verify your identity.",
        dialog->contents());
    dialog->contents()->addWidget(new Wt::WBreak());
    Wt::WLabel *label2 = new Wt::WLabel("Please enter your full name, email, and select today's date from the calendar on the right.",
        dialog->contents());

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container->setLayout(hbox);

    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
    hbox->addLayout(vbox1);

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    hbox->addLayout(vbox2);

    Wt::WLabel *lblDrName = new Wt::WLabel("Dr's Full Name:       ");
    lblDrName->setStyleClass("sig-lbl");
    Wt::WHBoxLayout *namehbox = new Wt::WHBoxLayout();
    namehbox->addWidget(lblDrName);
    Wt::WLineEdit *drFullEnteredName_;
    drFullEnteredName_ = new Wt::WLineEdit();
    drFullEnteredName_->setStyleClass("sig-left-box");
    namehbox->addWidget(drFullEnteredName_);
    vbox1->addLayout(namehbox);



    Wt::WLabel *lblDrEmail = new Wt::WLabel("Dr's Email Address: ");
    lblDrEmail->setStyleClass("sig-lbl");
    Wt::WHBoxLayout *emailhbox = new Wt::WHBoxLayout();
    emailhbox->addWidget(lblDrEmail);
    Wt::WLineEdit *drEmailEntered_;
    drEmailEntered_ = new Wt::WLineEdit();
    drEmailEntered_->setStyleClass("sig-left-box");
    emailhbox->addWidget(drEmailEntered_);
    vbox1->addLayout(emailhbox);



    Wt::WCalendar *c2 = new Wt::WCalendar();
    c2->setSelectionMode(Wt::SingleSelection);

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
            ("<p>{1}</p>")
            .arg(selected));
    }));
    vbox2->addWidget(c2);
    vbox2->addWidget(new Wt::WText("Double Click the date to select."));
    vbox2->addWidget(out);

    outSigName_ = new Wt::WText();
    outSigEmail_ = new Wt::WText();
    outSigDate_ = new Wt::WText();

    dbSigName_ = new Wt::WText();
    dbSigEmail_ = new Wt::WText();
    dbSigDate_ = new Wt::WText();


    container->show();

    Wt::WPushButton *ok = new Wt::WPushButton("Sign Now", dialog->footer());
    ok->setDefault(true);

    Wt::WBreak *br;
    br = new Wt::WBreak(this);
    ok->clicked().connect(std::bind([=]() {
        dialog->accept();
        outSigName_->setText(drFullEnteredName_->text());
        br;
        outSigEmail_->setText(drEmailEntered_->text());
        br;
        outSigDate_->setText(out->text());
        dbSigName_->setText(drFullEnteredName_->text());
        dbSigEmail_->setText(drEmailEntered_->text());
        dbSigDate_->setText(out->text());


    }));


    dialog->show();
}

void PracticeReferralWidget::complete()
{

    Wt::log("notice") << "Practice user clicked complete";
    //Wt::WPushButton *btnReview = new Wt::WPushButton("Review Now", this);
    composer_ = new Composer(conninfo_, strRefId(), this);

    Wt::WContainerWidget *container4 = new Wt::WContainerWidget();
    container4->setStyleClass("background-box");

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container4->setLayout(hbox);

    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();

    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    reviewRefId_ = new Wt::WText(this);
    reviewRefId_->hide();
    //vbox1->addWidget(reviewRefId_);

    reviewFirstName_ = new Wt::WText(this);
    vbox1->addWidget(reviewFirstName_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewLastName_ = new Wt::WText(this);
    vbox1->addWidget(reviewLastName_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewAddress1_ = new Wt::WText(this);
    vbox1->addWidget(reviewAddress1_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewAddress2_ = new Wt::WText(this);
    vbox1->addWidget(reviewAddress2_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewCity_ = new Wt::WText(this);
    vbox1->addWidget(reviewCity_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewZip_ = new Wt::WText(this);
    vbox1->addWidget(reviewZip_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewHomePhone_ = new Wt::WText(this);
    vbox1->addWidget(reviewHomePhone_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewCellPhone_ = new Wt::WText(this);
    vbox1->addWidget(reviewCellPhone_);
    //vbox1->addWidget(new Wt::WBreak());


    reviewDob_ = new Wt::WText(this);
    vbox1->addWidget(reviewDob_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewGender_ = new Wt::WText(this);
    vbox1->addWidget(reviewGender_);
    //vbox1->addWidget(new Wt::WBreak());
    //vbox1->addWidget(new Wt::WBreak());

    reviewDiagnostic_ = new Wt::WText(this);
    vbox1->addWidget(reviewDiagnostic_);
    //vbox1->addWidget(new Wt::WBreak());

    reviewOvernight_ = new Wt::WText(this);
    vbox1->addWidget(reviewOvernight_);
    //addWidget(new Wt::WBreak());

    reviewOtherSleepTest_ = new Wt::WText(this);
    vbox1->addWidget(reviewOtherSleepTest_);
    //addWidget(new Wt::WBreak());

    reviewHomeStudy_ = new Wt::WText(this);
    vbox1->addWidget(reviewHomeStudy_);
    //addWidget(new Wt::WBreak());

    reviewAuthConsult_ = new Wt::WText(this);
    vbox1->addWidget(reviewAuthConsult_);
    //addWidget(new Wt::WBreak());
    //addWidget(new Wt::WBreak());

    //disabled esig for app restructure, causing problems
    dbGender_ = new Wt::WText(this);
    dbGender_->hide();
    dbDiagnostic_ = new Wt::WText(this);
    dbDiagnostic_->hide();
    dbOvernight_ = new Wt::WText(this);
    dbOvernight_->hide();
    dbOtherTest_ = new Wt::WText(this);
    dbOtherTest_->hide();
    dbAuthConsult_ = new Wt::WText(this);
    dbAuthConsult_->hide();
    dbHomeStudy_ = new Wt::WText(this);
    dbHomeStudy_->hide();
    dbFirstName_ = new Wt::WText(this);
    dbFirstName_->hide();
    dbLastName_ = new Wt::WText(this);
    dbLastName_->hide();
    dbAddress1_ = new Wt::WText(this);
    dbAddress1_->hide();
    dbAddress2_ = new Wt::WText(this);
    dbAddress2_->hide();
    dbCity_ = new Wt::WText(this);
    dbCity_->hide();
    dbZip_ = new Wt::WText(this);
    dbZip_->hide();
    dbHomePhone_ = new Wt::WText(this);
    dbHomePhone_->hide();
    dbCellPhone_ = new Wt::WText(this);
    dbCellPhone_->hide();
    dbEmail_ = new Wt::WText(this);
    dbEmail_->hide();
    dbDob_ = new Wt::WText(this);
    dbDob_->hide();

    vbox1->addWidget(new Wt::WBreak());
    Wt::WPushButton *btnSendOrder = new Wt::WPushButton("Send Referral", this);
    btnSendOrder->setStyleClass("sig-btn");
    Wt::WPushButton *sigDialogBtn = new Wt::WPushButton("Sign Now", this);
    sigDialogBtn->setStyleClass("sig-btn");
    sigDialogBtn->clicked().connect(std::bind([=]() {
        showDialog();
        sigDialogBtn->hide();
        btnSendOrder->show();
    }));


    vbox2->addWidget(new Wt::WText("<h4>Upload File Attachments</h4>"));
    vbox2->addWidget(new Wt::WText("Upload scanned copies of the patient's insurance card, ID card, or anything you need to send us."));
    vbox2->addWidget(composer_);

    vbox2->addWidget(sigDialogBtn);
    vbox2->addWidget(btnSendOrder);
    btnSendOrder->hide();

    addWidget(container4);

    btnSendOrder->clicked().connect(this, &PracticeReferralWidget::sendOrder);
    btnSendOrder->clicked().connect(std::bind([=]() {
        container4->hide();
        btnSendOrder->hide();
        composer_->hide();
        outSigName_->hide();
        outSigEmail_->hide();
        outSigDate_->hide();
    }));


    reviewRefId_->setText(setRefId());

    reviewFirstName_->setText("<strong>Patient First Name:</strong> " + form_->strFirstName());
    dbFirstName_->setText(form_->strFirstName());
    reviewLastName_->setText("<strong>Patient Last Name:</strong> " + form_->strLastName());
    dbLastName_->setText(form_->strLastName());
    reviewAddress1_->setText("<strong>Patient Address Line 1:</strong> " + form_->strAddress1());
    dbAddress1_->setText(form_->strAddress1());
    reviewAddress2_->setText("<strong>Patient Address Line 2:</strong> " + form_->strAddress2());
    dbAddress2_->setText(form_->strAddress2());
    reviewCity_->setText("<strong>Patient City:</strong> "+ form_->strCity());
    dbCity_->setText(form_->strCity());
    reviewZip_->setText("<strong>Patient Zip Code:</strong> " + form_->strZip());
    dbZip_->setText(form_->strZip());
    reviewHomePhone_->setText("<strong>Patient Home Phone:</strong> " + form_->strHomePhone());
    dbHomePhone_->setText(form_->strHomePhone());
    reviewCellPhone_->setText("<strong>Patient Cell Phone:</strong>  " + form_->strCellPhone());
    dbCellPhone_->setText(form_->strCellPhone());

    reviewDob_->setText("<strong>Patient Date of Birth:</strong> " + form_->strDob());
    dbDob_->setText(form_->strDob());

    //pracDrSigName_->setText("Referring MD, Please sign here and select today's date and time.");

    reviewGender_->setText("<strong>Patient Gender:</strong> " + form_->strGender());
    dbGender_->setText(form_->strGender());

    reviewDiagnostic_->setText("<strong>Diagnosis: </strong> " + form_->strDiagnostic());
    dbDiagnostic_->setText(form_->strDiagnostic());

    reviewOvernight_->setText("<strong>Overnight Study: </strong> " + form_->strOvernight());
    dbOvernight_->setText(form_->strOvernight());

    //set reviewOtherSleepText to otheroutput
    reviewOtherSleepTest_->setText("<strong>Other Study: </strong> " + form_->strOtherSleepTest());
    dbOtherTest_->setText(form_->strOtherSleepTest());

    //check home study
    if (form_->boolHomeStudy()){
        reviewHomeStudy_->setText("<strong>You have authorized a home sleep study.</strong>");
        dbHomeStudy_->setText("Doctor Authorized Home Sleep Study");
    }

    //check consult authorization
    if (form_->boolAuthConsult()){
        reviewAuthConsult_->setText("<strong>You have authorized our board certified sleep specialists to consult your patient for Sleep Disorder treatment.</strong>");
        dbAuthConsult_->setText("Doctor Authorized Consultation");
    }



}

std::string PracticeReferralWidget::strFirstName()
{
    std::string str = dbFirstName_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strLastName()
{
    std::string str = dbLastName_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strAddress1()
{
    std::string str = dbAddress1_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strAddress2()
{
    std::string str = dbAddress2_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strCity()
{
    std::string str = dbCity_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strZip()
{
    std::string str = dbZip_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strHomePhone()
{
    std::string str = dbHomePhone_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strCellPhone()
{
    std::string str = dbCellPhone_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strDob()
{
    std::string str = dbDob_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReferralWidget::strGender()
{
    std::string str = dbGender_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strDiagnostic()
{
    std::string str = dbDiagnostic_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strOvernight()
{
    std::string str = dbOvernight_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strOtherSleepTest()
{
    std::string str = dbOtherTest_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strHomeStudy()
{
    std::string str = dbHomeStudy_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}


std::string PracticeReferralWidget::strAuthConsult()
{
    std::string str = dbAuthConsult_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReferralWidget::strEnteredName()
{
    std::string str = dbSigName_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReferralWidget::strEnteredEmail()
{
    std::string str = dbSigEmail_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

std::string PracticeReferralWidget::strEsigDate()
{
    std::string str = dbSigDate_->text().toUTF8();
      std::string newstr = boost::to_upper_copy<std::string>(str);
      return newstr;
}

void PracticeReferralWidget::sendOrder()
{
    Wt::log("notice") << "Practice user clicked send order";

    PracticeReferralWidget::saveOrder();

    orderSent_ = new Wt::WText(this);
    orderSent_->setText("Thank you for your referral. Please inform your patient that we will contact them shortly.");
    addWidget(new Wt::WBreak());

}

//this should create a new referral record in the database and email the staff
void PracticeReferralWidget::saveOrder()
{
    Wt::log("notice") << "Send order triggered saveOrder";
    dbo::Transaction practrans(dbsession);
    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac_.toUTF8());
    
    Wt::log("info") << "PRACTICE ID: " << prac_.toUTF8();

    Wt::WString pracName;
    Wt::WString pracZip;
    Wt::WString drFirst;
    Wt::WString drLast;
    Wt::WString pracNpi;
    Wt::WString pracEmail;
    Wt::WString pracSpec;
    Wt::WString pracId;
    

    pracName = practice->refPracticeName_;
    pracZip = practice->refPracticeZip_;
    drFirst = practice->refPracticeDrFirst_;
    drLast = practice->refPracticeDrLast_;
    pracNpi = practice->refPracticeNpi_;
    pracEmail = practice->refPracticeEmail_;
    pracSpec = practice->refSpecialty_;
    pracId = practice->practiceId_;

    
    Wt::WString refInbound = "inbound";
    Wt::log("info") << "pracId: " << pracId.toUTF8();

    Transaction transaction(dbsession);

    ptr<Referral> r = dbsession.add(new Referral());

    r.modify()->refPracticeName_ = pracName.toUTF8();
    r.modify()->refPracticeZip_ = pracZip.toUTF8();
    r.modify()->refPracticeDrFirst_ = drFirst.toUTF8();
    r.modify()->refPracticeDrLast_ = drLast.toUTF8();
    r.modify()->refPracticeNpi_ = pracNpi.toUTF8();
    r.modify()->refSpecialty_ = pracSpec.toUTF8();
    r.modify()->refPracticeEmail_ = pracEmail.toUTF8();
    r.modify()->refPracticeId_ = pracId.toUTF8();

    r.modify()->refFirstName_ = strFirstName();
    r.modify()->refLastName_ = strLastName();
    r.modify()->refAddress1_ = strAddress1();
    r.modify()->refAddress2_ = strAddress2();
    r.modify()->refCity_ = strCity();
    r.modify()->refZip_ = strZip();
    r.modify()->refHomePhone_ = strHomePhone();
    r.modify()->refCellPhone_ = strCellPhone();
    r.modify()->refDob_ = strDob();
    r.modify()->refGender_ = strGender();
    r.modify()->refDiagnostic_ = strDiagnostic();
    r.modify()->refOvernight_ = strOvernight();
    r.modify()->refOtherSleepTest_ = strOtherSleepTest();
    r.modify()->refHomeStudy_ = strHomeStudy();
    r.modify()->refInbound_ = refInbound.toUTF8();
    r.modify()->refAuthConsult_ = strAuthConsult();
    r.modify()->referralId_ = strRefId().toUTF8();
    r.modify()->enteredDrFullName_ = strEnteredName();
    r.modify()->enteredDrEmail_ = strEnteredEmail();
    r.modify()->selectedDate_ = strEsigDate();

    transaction.commit();


    Mail::Message message;
    message.setFrom(Mail::Mailbox("clinicore@statsleep.com", "United STAT Sleep"));
    message.addRecipient(Mail::To, Mail::Mailbox("dl-info@statsleep.com", "United STAT Sleep"));
    message.setSubject("An online referral was received");
    message.setBody("A referral was submitted online, please login to the sleep study portal to process the referral.");
    message.addHtmlBody ("<p>A referral has been submitted online, please login to the sleep study tool to process the referral.");
    Mail::Client client;
    client.connect("localhost");
    client.send(message);
}
