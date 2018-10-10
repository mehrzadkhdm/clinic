/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_REFERRAL_MODEL_H_
#define PRACTICE_REFERRAL_MODEL_H_

#include <Wt/WFormModel>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WButtonGroup>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include "PracticeSession.h"
#include "Composer.h"

class PracticeSession;
class PracticeReferralView;

class PracticeReferralModel : public Wt::WFormModel
{

public:
    PracticeReferralModel(Wt::WObject *parent);

    static constexpr Field FirstNameField = "patient.referral.first_name";
    static constexpr Field LastNameField = "patient.referral.last_name";
    static constexpr Field Address1Field = "patient.referral.address1";
    static constexpr Field Address2Field = "patient.referral.address2";
    static constexpr Field CityField = "patient.referral.city";
    static constexpr Field ZipField = "patient.referral.zipcode";
    static constexpr Field HomePhoneField = "patient.referral.home_phone";
    static constexpr Field CellPhoneField = "patient.referral.cell_phone";
    static constexpr Field GenderMaleField = "patient.referral.gender.male";
    static constexpr Field GenderFemaleField = "patient.referral.gender.female";
    static constexpr Field GenderOtherField = "patient.referral.gender.other";

    static constexpr Field DobField = "patient.referral.dob";

    static constexpr Field ApneaEventsField = "patient.referral.diagnosis.apnea";
    static constexpr Field ChronicFatigueField = "patient.referral.diagnosis.fatigue";
    static constexpr Field SeizuresField = "patient.referral.diagnosis.seizures";
    static constexpr Field InsomniaField = "patient.referral.diagnosis.insomnia";
    static constexpr Field NarcolepsyField = "patient.referral.diagnosis.narcolepsy";
    static constexpr Field RestlessLegsField = "patient.referral.diagnosis.restless_legs";
    static constexpr Field OtherDiagnosticField = "patient.referral.diagnosis.other";

    static constexpr Field CompleteStudyField = "patient.referral.procedure.overnight.complete";
    static constexpr Field PsgStudyField = "patient.referral.procedure.overnight.psg";
    static constexpr Field CpapStudyField = "patient.referral.procedure.overnight.cpap";
    static constexpr Field SplitStudyField = "patient.referral.procedure.overnight.split";

    static constexpr Field PapNapField = "patient.referral.procedure.other.papnap";
    static constexpr Field MsltMwtField = "patient.referral.procedure.other.mslt_mwt";
    static constexpr Field MwtField = "patient.referral.procedure.other.mwt";
    static constexpr Field SeizureStudyField = "patient.referral.procedure.other.siezure_study";
    static constexpr Field HomeStudyField = "patient.referral.procedure.other.hst";

    static constexpr Field AuthConsultField = "patient.referral.authorize";

    /**
     * Accessors
     */
    std::string strFirstName();
    std::string strLastName();
    std::string strAddress1();
    std::string strAddress2();
    std::string strCity();
    std::string strZip();
    std::string strHomePhone();
    std::string strCellPhone();
    std::string strDob();
    std::string strGender();
    std::string strDiagnostic();
    std::string strOvernight();
    std::string strOtherSleepTest();
    bool boolHomeStudy();
    bool boolAuthConsult();

private:
    static const int MAX_LENGTH = 25;
    Wt::WButtonGroup *genderButtonGroup_;

    Wt::WValidator *createNameValidator(const std::string& field);
    Wt::WValidator *createAddressValidator(const std::string& field);
    Wt::WValidator *createCityValidator();
    Wt::WValidator *createZipValidator();
    Wt::WValidator *createPhoneValidator(const std::string& field);
    Wt::WValidator *createAuthConsultValidator();

};

#endif //PRACTICE_REFERRAL_MODEL_H_
