/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Address : public CompositeEntity
{
public:
        Wt::WString line1_;
        Wt::WString zip_;
        Wt::WString localityId_;
        Wt::WString localityName_;
        Wt::WString addressId_;


        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for locality
                dbo::field(a, line1_, "line1");
                dbo::field(a, zip_, "zip");
                dbo::field(a, localityId_, "local_id");
                dbo::field(a, localityName_, "local_name");
                dbo::field(a, addressId_, "address_id");

        }

};

#endif //ADDRESS_H_