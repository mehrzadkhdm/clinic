/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INSURANCE_H_
#define INSURANCE_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Insurance : public CompositeEntity
{
public:
        Wt::WString insuranceName_;
        Wt::WString insuranceGroupId_;
        Wt::WString insuranceGroupName_;
        Wt::WString insuranceId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for reperring practice
                dbo::field(a, insuranceName_, "ins_name");
                dbo::field(a, insuranceGroupId_, "ins_group_id");
                dbo::field(a, insuranceGroupName_, "ins_group_name");
                dbo::field(a, insuranceId_, "ins_id");

        }

};

#endif //INSURANCE_H_
