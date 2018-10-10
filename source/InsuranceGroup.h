/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INSURANCE_GROUP_H_
#define INSURANCE_GROUP_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class InsuranceGroup : public CompositeEntity
{
public:
        Wt::WString groupName_;
        Wt::WString groupId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for reperring practice
                dbo::field(a, groupName_, "group_name");
                dbo::field(a, groupId_, "group_id");

        }

};

#endif //INSURANCE_GROUP_H_
