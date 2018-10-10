/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STUDY_TYPE_H_
#define STUDY_TYPE_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class StudyType : public CompositeEntity
{
public:
    Wt::WString studyTypeName_;
    Wt::WString studyTypeId_;
    Wt::WString studyTypeCode_;


    //dbo template for reperral
    template<class Action>
    void persist(Action& a)
    {
        CompositeEntity::persist(a);
        //class mapping for reperring practice
        dbo::field(a, studyTypeName_, "st_type_name");
        dbo::field(a, studyTypeCode_, "st_type_code");
        dbo::field(a, studyTypeId_, "st_type_id");

    }

};

#endif //STUDY_TYPE_H_
