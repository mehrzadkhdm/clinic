/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/
#ifndef COMPOSITE_ENTITY_H_
#define COMPOSITE_ENTITY_H_

#include <Wt/WDateTime>
#include <Wt/WApplication>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

using namespace Wt;
namespace dbo = Wt::Dbo;

class CompositeEntity
{
public:

    Wt::WDateTime createdAt();
    Wt::WDateTime updatedAt();

    Wt::WDateTime createdAt_;
    Wt::WDateTime updatedAt_;

    template<class Action>
    void persist(Action& a)
    {
        if (createdAt_.isNull())
        {
            Wt::log("notice") << "CompositeEntity Created At is null";
            createdAt_ = createdAt();

        }

        dbo::field(a, createdAt_, "created_at");

        Wt::WDateTime updatedAt_ = updatedAt();
        dbo::field(a, updatedAt_, "updated_at");
    }

};

#endif //COMPOSITE_ENTITY_H_
