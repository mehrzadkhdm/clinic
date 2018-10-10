/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef REFERRAL_H_
#define REFERRAL_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WDateTime>
#include "CompositeEntity.h"


namespace dbo = Wt::Dbo;

class Referral : public CompositeEntity
{
public:

        Wt::WString refPracticeName_;
        Wt::WString refPracticeZip_;
        Wt::WString refPracticeDrFirst_;
        Wt::WString refPracticeDrLast_;
        Wt::WString refPracticeNpi_;
        Wt::WString refPracticeEmail_;
        Wt::WString refSpecialty_;
        Wt::WString refPracticeId_;


        Wt::WString refFirstName_;
        Wt::WString refLastName_;
        Wt::WString refAddress1_;
        Wt::WString refAddress2_;
        Wt::WString refCity_;
        Wt::WString refZip_;
        Wt::WString refHomePhone_;
        Wt::WString refCellPhone_;
        Wt::WString refEmail_;
        Wt::WString refDob_;
        Wt::WString refFour_;
        Wt::WString refGender_;
        Wt::WString refDiagnostic_;
        Wt::WString refOvernight_;
        Wt::WString refOtherSleepTest_;
        Wt::WString refHomeStudy_;
        Wt::WString refAuthConsult_;
        Wt::WString referralId_;
        Wt::WString patientId_;

        Wt::WString enteredDrFullName_;
        Wt::WString enteredDrEmail_;
        Wt::WString selectedDate_;

        Wt::WString refInbound_;

        Wt::WString refInsuranceId_;
        Wt::WDate refReceivedAt_;

        Wt::WString refInsuranceName_;

        Wt::WString refStatus_;
        Wt::WString studyDate_;
        Wt::WString studyTime_;
        Wt::WString studyLocation_;
        Wt::WDate authExpiration_;

        Wt::WString studyTypeId_;
        Wt::WString studyTypeName_;
        Wt::WString studyTypeCode_;

        //dbo template for referral
        template<class Action>
        void persist(Action& a)
        {
                //class mapping for referring practice
                CompositeEntity::persist(a);

                dbo::field(a, refPracticeName_, "prac_name");
                dbo::field(a, refPracticeZip_, "prac_zip");
                dbo::field(a, refPracticeDrFirst_, "dr_first");
                dbo::field(a, refPracticeDrLast_, "dr_last");
                dbo::field(a, refPracticeNpi_, "prac_npi");
                dbo::field(a, refPracticeEmail_, "prac_email");
                dbo::field(a, refSpecialty_, "prac_specialty");
                dbo::field(a, refPracticeId_, "prac_id");
                //class mapping for patient info
                dbo::field(a, refFirstName_, "pt_firstname");
                dbo::field(a, refLastName_, "pt_lastname");
                dbo::field(a, refAddress1_, "pt_addr1");
                dbo::field(a, refAddress2_, "pt_addr2");
                dbo::field(a, refCity_, "pt_city");
                dbo::field(a, refZip_, "pt_zip");
                dbo::field(a, refHomePhone_, "pt_homephone");
                dbo::field(a, refCellPhone_, "pt_cellphone");
                dbo::field(a, refEmail_, "pt_email");
                dbo::field(a, refDob_, "pt_dob");
                dbo::field(a, refFour_, "pt_four");
                dbo::field(a, refGender_, "pt_gender");
                dbo::field(a, refDiagnostic_, "study_diag");
                dbo::field(a, refOvernight_, "study_overnight");
                dbo::field(a, refHomeStudy_, "study_hst");
                dbo::field(a, refOtherSleepTest_, "study_other");
                dbo::field(a, refAuthConsult_, "study_auth");
                dbo::field(a, referralId_, "ref_id");
                dbo::field(a, patientId_, "pt_id");
                dbo::field(a, enteredDrFullName_, "enter_fullname");
                dbo::field(a, enteredDrEmail_, "enter_email");
                dbo::field(a, refInbound_, "inbound_status");
                dbo::field(a, refInsuranceId_, "insurance_id");
                dbo::field(a, refInsuranceName_, "insurance_name");
                dbo::field(a, refReceivedAt_, "ref_received");
                dbo::field(a, authExpiration_, "auth_expires");
                dbo::field(a, refStatus_, "ref_status");
                dbo::field(a, studyDate_, "study_date");
                dbo::field(a, studyTime_, "study_time");
                dbo::field(a, studyTypeId_, "std_type_id");
                dbo::field(a, studyTypeName_, "std_type_name");
                dbo::field(a, studyTypeCode_, "std_type_code");
                dbo::field(a, studyLocation_, "study_loco");

        }

};

#endif //REFERRAL_H_
