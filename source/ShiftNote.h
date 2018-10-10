/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef SHIFTNOTE_H_
#define SHIFTNOTE_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WDateTime>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class ShiftNote : public CompositeEntity
{
public:
        Wt::WString employeeEmail_;
        Wt::WString shiftLocation_;
        Wt::WDateTime noteSent_;
        Wt::WString noteBody_;
        Wt::WString noteId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);
                //class mapping for reperring practice
                dbo::field(a, employeeEmail_, "emp_email");
                dbo::field(a, shiftLocation_, "shift_loc");
                dbo::field(a, noteSent_, "note_sent");
                dbo::field(a, noteBody_, "note_body");
                dbo::field(a, noteId_, "note_id");

        }

};

#endif //SHIFTNOTE_H_
