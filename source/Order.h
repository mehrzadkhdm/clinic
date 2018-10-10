/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ORDER_H_
#define ORDER_H_

#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Order : public CompositeEntity
{
public:
        Wt::WString repPracticeName_;
        Wt::WString repPracticeDrFirst_;
        Wt::WString repPracticeDrLast_;
        Wt::WString repPracticeEmail_;
        Wt::WString repCreatedAt_;

        Wt::WString orderId_;



        //dbo template for referral
        template<class Action>
        void persist(Action& a)
        {

                CompositeEntity::persist(a);

                //class mapping for reperring practice
                dbo::field(a, repPracticeName_, "prac_name");
                dbo::field(a, repPracticeDrFirst_, "prac_drfirst");
                dbo::field(a, repPracticeDrLast_, "prac_drlast");
                dbo::field(a, repPracticeEmail_, "prac_email");
                dbo::field(a, repCreatedAt_, "rep_at");

                //class mapping for patient info
                dbo::field(a, orderId_, "report_id");


        }

};

#endif //ORDER_H_
