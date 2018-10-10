/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PATIENT_H_
#define PATIENT_H_

#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Patient;
typedef Wt::Auth::Dbo::AuthInfo<Patient> PatientAuthInfo;

class Patient : public CompositeEntity {
public:
        /* You probably want to add other practice information here */
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
        std::string ptId_;
        std::string ptInsuranceId_;
        std::string ptInsuranceName_;

        std::string refId_;
        std::string pracId_;
        int numReschedule_;


        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);

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
                dbo::field(a, ptId_, "pt_id");
                dbo::field(a, numReschedule_, "num_reschedule");
                dbo::field(a, ptInsuranceId_, "pt_insid");
                dbo::field(a, ptInsuranceName_, "pt_insname");

                dbo::field(a, refId_, "ref_id");
                dbo::field(a, pracId_, "prac_id");
        }
};

#endif // PATIENT_H_
