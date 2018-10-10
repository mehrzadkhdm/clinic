/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef LOCALITY_H_
#define LOCALITY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Locality : public CompositeEntity
{
public:
        Wt::WString name_;
        Wt::WString abbrv_;
        Wt::WString localityId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for locality
                dbo::field(a, name_, "local_name");
                dbo::field(a, abbrv_, "local_state");
                dbo::field(a, localityId_, "local_id");

        }

};

#endif //LOCALITY_H_