/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INSURANCE_GROUP_ITEM_H_
#define INSURANCE_GROUP_ITEM_H_
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class InsuranceGroupItem;

class InsuranceGroupItem {
public:

    Wt::WString groupName;
    Wt::WString groupId;

};

#endif // INSURANCE_GROUP_ITEM_H_
