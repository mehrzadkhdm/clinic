/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INSURANCE_ITEM_H_
#define INSURANCE_ITEM_H_
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class InsuranceItem;

class InsuranceItem {
public:

    Wt::WString insuranceName;
    Wt::WString insuranceId;

};

#endif // INSURANCE_ITEM_H_
