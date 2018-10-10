/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PHYSICIAN_VISIT_H_
#define PHYSICIAN_VISIT_H_

#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/WTime>
#include <Wt/WDateTime>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class PhysicianVisit : public CompositeEntity
{
public:
        /* You probably want to add other practice information here */
        Wt::WDateTime visitStartTime_;
        Wt::WDateTime visitEndTime_;

        Wt::WDateTime visitDateTime_;
        Wt::WDateTime nextVisitDateTime_;

        Wt::WString pccName_;

        Wt::WString visitIssues_;
        Wt::WString practiceName_;
        Wt::WString reasonForVisit_;
        Wt::WString outcomeOfVisit_;
        Wt::WString officeManagerName_;
        Wt::WString visitType_;

        Wt::WString visitId_;
        Wt::WString practiceId_;
        Wt::WString pccId_;


        template<class Action>
        void persist(Action& a)
        {

                CompositeEntity::persist(a);

                dbo::field(a, visitStartTime_, "visit_start");
                dbo::field(a, visitEndTime_, "visit_end");
                dbo::field(a, visitDateTime_, "this_visit_dt");
                dbo::field(a, nextVisitDateTime_, "next_visit_dt");
                dbo::field(a, pccName_, "pcc_name");
                dbo::field(a, visitIssues_, "visit_issues");
                dbo::field(a, practiceName_, "prac_name");
                dbo::field(a, reasonForVisit_, "visit_reason");
                dbo::field(a, outcomeOfVisit_, "visit_outcome");
                dbo::field(a, officeManagerName_, "prac_manager");
                dbo::field(a, visitType_, "visit_type");

                dbo::field(a, visitId_, "visit_id");
                dbo::field(a, practiceId_, "prac_id");
                dbo::field(a, pccId_, "pcc_id");

        }
};

#endif // PHYSICIAN_VISIT_H_
