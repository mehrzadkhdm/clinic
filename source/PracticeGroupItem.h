/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_GROUP_ITEM_H_
#define PRACTICE_GROUP_ITEM_H_
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class PracticeGroupItem;

class PracticeGroupItem {
public:

    Wt::WString groupName;
    Wt::WString groupId;

};

#endif // PRACTICE_GROUP_ITEM_H_
