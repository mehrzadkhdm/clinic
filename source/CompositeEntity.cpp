/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WDateTime>
#include <string>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

Wt::WDateTime CompositeEntity::createdAt()
{
    return Wt::WDateTime::currentDateTime();

}

Wt::WDateTime CompositeEntity::updatedAt()
{
    return Wt::WDateTime::currentDateTime();

}

