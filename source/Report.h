/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef REPORT_H_
#define REPORT_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Report : public CompositeEntity
{
public:
        Wt::WString repPracticeName_;
        Wt::WString repPracticeDrFirst_;
        Wt::WString repPracticeDrLast_;
        Wt::WString repPracticeEmail_;

        Wt::WString patientFirstName_;
        Wt::WString patientLastName_;
        Wt::WString patientDob_;
        Wt::WString repStaffName_;

        Wt::WString reportId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for reperring practice
                dbo::field(a, repPracticeName_, "prac_name");
                dbo::field(a, repPracticeDrFirst_, "prac_drfirst");
                dbo::field(a, repPracticeDrLast_, "prac_drlast");
                dbo::field(a, repPracticeEmail_, "prac_email");

                //class mapping for patient info

                dbo::field(a, patientFirstName_, "pt_firstname");
                dbo::field(a, patientLastName_, "pt_lastname");
                dbo::field(a, patientDob_, "pt_dob");
                dbo::field(a, repStaffName_, "staff_name");
                dbo::field(a, reportId_, "report_id");


        }

};

#endif //REPORT_H_
