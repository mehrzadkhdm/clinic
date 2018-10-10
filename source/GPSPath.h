/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef GPSPATH_H_
#define GPSPATH_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class GPSPath : public CompositeEntity
{
public:
        Wt::WString lat_;
        Wt::WString long_;
        Wt::WString provider_;

        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for locality
                dbo::field(a, lat_, "lat");
                dbo::field(a, long_, "long");
                dbo::field(a, provider_, "provider");
        }

};

#endif //GPSPATH_H_