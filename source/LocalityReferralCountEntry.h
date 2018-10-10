/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef LOCALITY_REFERRAL_COUNT_ENTRY_H_
#define LOCALITY_REFERRAL_COUNT_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class LocalityReferralCountEntry;

class LocalityReferralCountEntry {
public:


    Wt::WString localityName;
    Wt::WString numLastWeek;
    Wt::WString numLastMonth;
    Wt::WString numLastYear;
    Wt::WDate lastReceivedDate;


};

#endif // LOCALITY_REFERRAL_COUNT_ENTRY_H_
