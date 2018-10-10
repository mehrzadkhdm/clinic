/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PHYSICIAN_ISSUE_H_
#define PHYSICIAN_ISSUE_H_

#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/WTime>
#include <Wt/WDateTime>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class PhysicianIssue : public CompositeEntity
{
public:
        /* You probably want to add other practice information here */

        Wt::WDateTime issueReported_;
        Wt::WDateTime issueResolved_;

        Wt::WString pccName_;
        Wt::WString mdName_;
        Wt::WString mdPhone_;
        Wt::WString issueTitle_;
        Wt::WString issueDescription_;
        Wt::WString issueStatus_;
        Wt::WString actionTaken_;

        Wt::WString issueId_;
        Wt::WString practiceId_;
        Wt::WString visitId_;
        Wt::WString pccId_;


        template<class Action>
        void persist(Action& a)
        {

                CompositeEntity::persist(a);

                dbo::field(a, issueReported_, "date_reported");
                dbo::field(a, issueResolved_, "date_resolved");
                dbo::field(a, pccName_, "pcc_name");
                dbo::field(a, mdName_, "md_name");
                dbo::field(a, mdPhone_, "md_phone");
                dbo::field(a, issueTitle_, "title");
                dbo::field(a, issueDescription_, "description");
                dbo::field(a, issueStatus_, "status");
                dbo::field(a, actionTaken_, "action_taken");

                dbo::field(a, issueId_, "issue_id");
                dbo::field(a, practiceId_, "prac_id");
                dbo::field(a, visitId_, "visit_id");
                dbo::field(a, pccId_, "pcc_id");

        }
};

#endif // PHYSICIAN_ISSUE_H_
