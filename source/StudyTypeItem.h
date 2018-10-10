/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STUDY_TYPE_ITEM_H_
#define STUDY_TYPE_ITEM_H_
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class StudyTypeItem;

class StudyTypeItem {
public:

    Wt::WString studyTypeName;
    Wt::WString studyTypeCode;
    Wt::WString studyTypeId;

};

#endif // STUDY_TYPE_ITEM_H_