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
#include <Wt/WLineEdit>
#include <Wt/WDateEdit>
#include <Wt/WCheckBox>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WPushButton>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>

//other header files
#include "PracticeReferralView.h"
#include "PracticeReferralModel.h"
#include "Referral.h"

PracticeReferralView::PracticeReferralView(Wt::WContainerWidget *parent=0)
    : Wt::WTemplateFormView(parent),
    done_(this)
{
    model_ =  new PracticeReferralModel(this);
    setStyleClass("referral-form");

    setTemplateText(tr("template.PracticeReferral"));
    addFunction("id", &WTemplate::Functions::id);
    addFunction("block", &WTemplate::Functions::block);
    addFunction("tr", &WTemplate::Functions::tr);

    /*
     * first name
     */
    Wt::WLineEdit *lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::FirstNameField, lineEdit);

    /*
     * last name
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::LastNameField, lineEdit);

    /*
     * address 1
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::Address1Field, lineEdit);

    /*
     * address 2
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::Address2Field, lineEdit);

    /*
     * city
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::CityField, lineEdit);

    /*
     * zip
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::ZipField, lineEdit);

    /*
     * home phone
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::HomePhoneField, lineEdit);

    /*
     * cell phone
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::CellPhoneField, lineEdit);

    /*
     * date of birth
     */
    Wt::WDateEdit *dateEdit = new Wt::WDateEdit();
    dateEdit->setFormat("MM/dd/yyyy");
    dateEdit->setBottom(Wt::WDate(1900, 1, 1));
    dateEdit->setTop(Wt::WDate::currentDate());
    // Make the date mandatory. WDateEdit has it's own validator
    dateEdit->validator()->setMandatory(true);
    setFormWidget(PracticeReferralModel::DobField, dateEdit);


    /*
     * Gender radio buttons
     */
    group_ = new Wt::WButtonGroup(this);
    Wt::WRadioButton *radioButton = new Wt::WRadioButton();
    group_->addButton(radioButton, GENDER_MALE);
    setFormWidget(PracticeReferralModel::GenderMaleField, radioButton);

    radioButton = new Wt::WRadioButton();
    group_->addButton(radioButton, GENDER_FEMALE);
    setFormWidget(PracticeReferralModel::GenderFemaleField, radioButton);

    radioButton = new Wt::WRadioButton();
    group_->addButton(radioButton, GENDER_OTHER);
    setFormWidget(PracticeReferralModel::GenderOtherField, radioButton);

    /*
     * diagnosis - apnea
     */
    Wt::WCheckBox *checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::ApneaEventsField, checkBox);

    /*
     * diagnosis - chronic fatigue
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::ChronicFatigueField, checkBox);

    /*
     * diagnosis - seizures
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::SeizuresField, checkBox);

    /*
     * diagnosis - insomnia
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::InsomniaField, checkBox);

    /*
     * diagnosis - narcolepsy
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::NarcolepsyField, checkBox);

    /*
     * diagnosis - restless legs
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::RestlessLegsField, checkBox);

    /*
     * diagnosis - other
     */
    lineEdit = new Wt::WLineEdit();
    setFormWidget(PracticeReferralModel::OtherDiagnosticField, lineEdit);

    /*
     * overnight - complete study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::CompleteStudyField, checkBox);

    /*
     * overnight - psg study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::PsgStudyField, checkBox);

    /*
     * overnight - cpap study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::CpapStudyField, checkBox);

    /*
     * overnight - split study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::SplitStudyField, checkBox);

    /*
     * other - pap nap
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::PapNapField, checkBox);

    /*
     * other - mslt & mwt
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::MsltMwtField, checkBox);

    /*
     * other - mwt
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::MwtField, checkBox);

    /*
     * other - seizure study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::SeizureStudyField, checkBox);

    /*
     * other - home sleep study
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::HomeStudyField, checkBox);

    /*
     * authorize consultation
     */
    checkBox = new Wt::WCheckBox();
    setFormWidget(PracticeReferralModel::AuthConsultField, checkBox);

    Wt::WPushButton *button = new Wt::WPushButton(Wt::WString::tr("patient.referral.submit.button"));
    bindWidget("continue-button", button);
    button->clicked().connect(this, &PracticeReferralView::process);

    updateView(model_);
}

void PracticeReferralView::process()
{
    updateModel(model_);
    Wt::log("info") << "Process";
    bool valid = model_->validate();
    if (valid) {
        done_.emit(0);
    }
    std::string validity = model_->validate() ? "valid" : "not valid";
    Wt::log("info") << validity;

    updateView(model_);
}

std::string PracticeReferralView::strFirstName()
{
    return model_->strFirstName();
}
std::string PracticeReferralView::strLastName()
{
    return model_->strLastName();
}
std::string PracticeReferralView::strAddress1()
{
    return model_->strAddress1();
}
std::string PracticeReferralView::strAddress2()
{
    return model_->strAddress2();
}
std::string PracticeReferralView::strCity()
{
    return model_->strCity();
}
std::string PracticeReferralView::strZip()
{
    return model_->strZip();
}
std::string PracticeReferralView::strHomePhone()
{
    return model_->strHomePhone();
}
std::string PracticeReferralView::strCellPhone()
{
    return model_->strCellPhone();
}
std::string PracticeReferralView::strDob()
{
    return model_->strDob();
}
std::string PracticeReferralView::strGender()
{
    return model_->strGender();
}
std::string PracticeReferralView::strDiagnostic()
{
    return model_->strDiagnostic();
}
std::string PracticeReferralView::strOvernight()
{
    return model_->strOvernight();
}
std::string PracticeReferralView::strOtherSleepTest()
{
    return model_->strOtherSleepTest();
}
bool PracticeReferralView::boolHomeStudy()
{
    return model_->boolHomeStudy();
}
bool PracticeReferralView::boolAuthConsult()
{
    return model_->boolAuthConsult();
}
