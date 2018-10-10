/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef BACKUP_H_
#define BACKUP_H_

#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Backup : public CompositeEntity
{
public:
        /* You probably want to add other practice information here */
        std::string ptFirstName_;
        std::string ptLastName_;
        std::string ptHomePhone_;
        std::string ptCellPhone_;
        std::string ptDob_;
        std::string ptId_;

        std::string prefLocation_;
        std::string buId_;


        template<class Action>
        void persist(Action& a)
        {

                CompositeEntity::persist(a);

                dbo::field(a, ptFirstName_, "pt_first");
                dbo::field(a, ptLastName_, "pt_last");
                dbo::field(a, ptHomePhone_, "pt_home");
                dbo::field(a, ptCellPhone_, "pt_cell");
                dbo::field(a, ptDob_, "pt_dob");
                dbo::field(a, ptId_, "pt_id");

                dbo::field(a, prefLocation_, "pref_loco");
                dbo::field(a, buId_, "bu_id");
        }
};

#endif // BACKUP_H_
