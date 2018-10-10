/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//wt stuff
#include <Wt/WApplication>
#include <Wt/WTemplateFormView>
#include <Wt/WFormModel>
#include <Wt/WLengthValidator>
#include <Wt/WString>

#include "PracticeReferralModel.h"

PracticeReferralModel::PracticeReferralModel(Wt::WObject *parent) {
    addField(FirstNameField);
    addField(LastNameField);
    addField(Address1Field);
    addField(Address2Field);
    addField(CityField);
    addField(ZipField);
    addField(HomePhoneField);
    addField(CellPhoneField);
    addField(GenderMaleField);
    addField(GenderFemaleField);
    addField(GenderOtherField);

    addField(DobField);

    addField(ApneaEventsField);
    addField(ChronicFatigueField);
    addField(SeizuresField);
    addField(InsomniaField);
    addField(NarcolepsyField);
    addField(RestlessLegsField);
    addField(OtherDiagnosticField);

    addField(CompleteStudyField);
    addField(PsgStudyField);
    addField(CpapStudyField);
    addField(SplitStudyField);

    addField(PapNapField);
    addField(MsltMwtField);
    addField(MwtField);
    addField(SeizureStudyField);

    addField(HomeStudyField);
    addField(AuthConsultField);

    setValidator(FirstNameField, createNameValidator(FirstNameField));
    setValidator(LastNameField, createNameValidator(LastNameField));
    setValidator(Address1Field, createAddressValidator(Address1Field));
    setValidator(CityField, createCityValidator());
    setValidator(ZipField, createZipValidator());
    setValidator(HomePhoneField, createPhoneValidator(HomePhoneField));
    setValidator(AuthConsultField, createAuthConsultValidator());
};

Wt::WValidator* PracticeReferralModel::createNameValidator(const std::string& field) {
    Wt::WLengthValidator *v = new Wt::WLengthValidator();
    v->setMandatory(true);
    v->setMinimumLength(1);
    v->setMaximumLength(MAX_LENGTH);
    return v;
}

Wt::WValidator* PracticeReferralModel::createAddressValidator(const std::string& field) {
    Wt::WLengthValidator *v = new Wt::WLengthValidator();
    v->setMandatory(true);
    return v;
}

Wt::WValidator* PracticeReferralModel::createCityValidator() {
    Wt::WLengthValidator *v = new Wt::WLengthValidator();
    v->setMandatory(true);
    return v;
}

Wt::WValidator* PracticeReferralModel::createZipValidator() {
    Wt::WLengthValidator *v = new Wt::WLengthValidator();
    v->setMandatory(true);
    return v;
}

Wt::WValidator* PracticeReferralModel::createPhoneValidator(const std::string& field) {
    Wt::WLengthValidator *v = new Wt::WLengthValidator();
    v->setMandatory(true);
    return v;
}

Wt::WValidator* PracticeReferralModel::createAuthConsultValidator() {
    Wt::WValidator *v = new Wt::WValidator();
    v->setMandatory(true);
    return v;
}

/**
 * Accessors
 */

std::string PracticeReferralModel::strFirstName()
{
    std::string str = valueText(FirstNameField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strLastName()
{
    std::string str = valueText(LastNameField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strAddress1()
{
    std::string str = valueText(Address1Field).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strAddress2()
{
    std::string str = valueText(Address2Field).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strCity()
{
    std::string str = valueText(CityField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strZip()
{
    std::string str = valueText(ZipField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strHomePhone()
{
    std::string str = valueText(HomePhoneField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strCellPhone()
{
    std::string str = valueText(CellPhoneField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strDob()
{
    std::string str = valueText(DobField).toUTF8();
    return boost::to_upper_copy<std::string>(str);
}

std::string PracticeReferralModel::strGender()
{
    std::string str;
    bool male = valueText(GenderMaleField) == "true";
    bool female = valueText(GenderFemaleField) == "true";
    bool other = valueText(GenderOtherField) == "true";
    if (male) {
        str = Wt::WString::tr("patient.referral.gender.male").toUTF8();
    }
    else if (female) {
        str = Wt::WString::tr("patient.referral.gender.female").toUTF8();
    }
    else if (other) {
        str = Wt::WString::tr("patient.referral.gender.other").toUTF8();
    }
    else {
        str = Wt::WString::tr("patient.referral.gender.unspecified").toUTF8();
    }
    return str;
}

std::string PracticeReferralModel::strDiagnostic()
{
    std::string str = "";
    bool apnea = valueText(ApneaEventsField) == "true";
    bool fatigue = valueText(ChronicFatigueField) == "true";
    bool seizures = valueText(SeizuresField) == "true";
    bool insomnia = valueText(InsomniaField) == "true";
    bool narcolepsy = valueText(NarcolepsyField) == "true";
    bool restless_legs = valueText(RestlessLegsField) == "true";
    std::string other = valueText(OtherDiagnosticField).toUTF8();
    if (apnea) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.apnea").toUTF8();
    }
    if (fatigue) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.fatigue").toUTF8();
    }
    if (seizures) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.seizures").toUTF8();
    }
    if (insomnia) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.insomnia").toUTF8();
    }
    if (narcolepsy) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.narcolepsy").toUTF8();
    }
    if (restless_legs) {
        str += Wt::WString::tr("patient.referral.review.diagnosis.restless_legs").toUTF8();
    }
    if (other != "") {
        str += Wt::WString::tr("patient.referral.review.diagnosis.other").toUTF8() + other;
    }
    if (boost::algorithm::ends_with(str, ", ")) {
        str = boost::algorithm::erase_tail_copy(str, 2);
    }
    return str;
}

std::string PracticeReferralModel::strOvernight()
{
    std::string str = "";
    bool complete = valueText(CompleteStudyField) == "true";
    bool psg = valueText(PsgStudyField) == "true";
    bool cpap = valueText(CpapStudyField) == "true";
    bool split = valueText(SplitStudyField) == "true";
    if (complete) {
        str += Wt::WString::tr("patient.referral.review.overnight.complete").toUTF8();
    }
    if (psg) {
        str += Wt::WString::tr("patient.referral.review.overnight.psg").toUTF8();
    }
    if (cpap) {
        str += Wt::WString::tr("patient.referral.review.overnight.cpap").toUTF8();
    }
    if (split) {
        str += Wt::WString::tr("patient.referral.review.overnight.split").toUTF8();
    }
    if (boost::algorithm::ends_with(str, ", ")) {
        str = boost::algorithm::erase_tail_copy(str, 2);
    }
    return str;
}

std::string PracticeReferralModel::strOtherSleepTest()
{
    std::string str = "";
    bool papnap = valueText(PapNapField) == "true";
    bool mslt_mwt = valueText(MsltMwtField) == "true";
    bool mwt = valueText(MwtField) == "true";
    bool siezure_study = valueText(SeizureStudyField) == "true";
    if (papnap) {
        str += Wt::WString::tr("patient.referral.review.other.papnap").toUTF8();
    }
    if (mslt_mwt) {
        str += Wt::WString::tr("patient.referral.review.other.mslt_mwt").toUTF8();
    }
    if (mwt) {
        str += Wt::WString::tr("patient.referral.review.other.mwt").toUTF8();
    }
    if (siezure_study) {
        str += Wt::WString::tr("patient.referral.review.other.seizure_study").toUTF8();
    }
    if (boost::algorithm::ends_with(str, ", ")) {
        str = boost::algorithm::erase_tail_copy(str, 2);
    }
    return str;
}

bool PracticeReferralModel::boolHomeStudy()
{
    return valueText(HomeStudyField) == "true";
}

bool PracticeReferralModel::boolAuthConsult()
{
    return valueText(AuthConsultField) == "true";
}
