/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef COMMENT_H_
#define COMMENT_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include "CompositeEntity.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

class Comment : public CompositeEntity
{
public:
        Wt::WString staffEmail_;

        Wt::WString commentText_;
        Wt::WString buId_;
        Wt::WString ptId_;

        Wt::WString commentId_;



        //dbo template for reperral
        template<class Action>
        void persist(Action& a)
        {

                CompositeEntity::persist(a);

                //class mapping for reperring practice
                dbo::field(a, staffEmail_, "staff_email");
                dbo::field(a, buId_, "backup_id");
                dbo::field(a, ptId_, "pt_id");

                dbo::field(a, commentText_, "comment");
                dbo::field(a, commentId_, "pt_firstname");


        }

};

#endif // COMMENT_H_
