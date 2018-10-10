/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef SLEEPSTUDY_H_
#define SLEEPSTUDY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WDate>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;
class SleepStudy : public CompositeEntity
{
public:
        std::string ptFirstName_;
        std::string ptLastName_;
        std::string ptAddress1_;
        std::string ptAddress2_;
        std::string ptCity_;
        std::string ptZip_;
        std::string ptHomePhone_;
        std::string ptCellPhone_;
        std::string ptEmail_;
        std::string ptDob_;
        std::string ptGender_;

        Wt::WString studyDiagnostic_;
        Wt::WString studyOvernight_;
        Wt::WString studyOtherSleepTest_;
        Wt::WString studyHomeStudy_;
        Wt::WString studyAuthConsult_;

        Wt::WString studyTypeId_;
        Wt::WString studyTypeName_;
        Wt::WString studyTypeCode_;

        Wt::WString studyStatus_;
        Wt::WString appointmentStatus_;

        Wt::WDate studyDate_;
        Wt::WString studyTime_;
        Wt::WString studyLocation_;
        Wt::WString bedNumber_;

        Wt::WString studyId_;

        Wt::WString referralId_;
        Wt::WString practiceId_;
        Wt::WString insuranceId_;
        Wt::WString patientId_;



        //dbo template for referral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);

                //patient info
                dbo::field(a, ptFirstName_, "pt_first");
                dbo::field(a, ptLastName_, "pt_last");
                dbo::field(a, ptAddress1_, "pt_addr1");
                dbo::field(a, ptAddress2_, "pt_addr2");
                dbo::field(a, ptCity_, "pt_city");
                dbo::field(a, ptZip_, "pt_zip");
                dbo::field(a, ptHomePhone_, "pt_home");
                dbo::field(a, ptCellPhone_, "pt_cell");
                dbo::field(a, ptEmail_, "pt_email");
                dbo::field(a, ptDob_, "pt_dob");
                dbo::field(a, ptGender_, "pt_gender");

                //referral diagnostic
                dbo::field(a, studyDiagnostic_, "std_diag");
                dbo::field(a, studyOvernight_, "std_over");
                dbo::field(a, studyOtherSleepTest_, "std_other");
                dbo::field(a, studyHomeStudy_, "std_home");
                dbo::field(a, studyAuthConsult_, "std_auth");
                //study details
                dbo::field(a, studyStatus_, "std_status");
                dbo::field(a, appointmentStatus_, "appt_status");
                dbo::field(a, studyDate_, "std_date");
                dbo::field(a, studyTime_, "std_time");

                dbo::field(a, studyTypeId_, "std_type_id");
                dbo::field(a, studyTypeName_, "std_type_name");
                dbo::field(a, studyTypeCode_, "std_type_code");

                dbo::field(a, studyLocation_, "std_loco");
                dbo::field(a, bedNumber_, "bed_number");

                //study id
                dbo::field(a, studyId_, "std_id");
                //other ids
                dbo::field(a, referralId_, "ref_id");
                dbo::field(a, practiceId_, "prac_id");
                dbo::field(a, insuranceId_, "in_id");
                dbo::field(a, patientId_, "pt_id");

        }

};

#endif //SLEEPSTUDY_H_
