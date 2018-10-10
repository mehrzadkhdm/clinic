/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef REFERRAL_INSURANCE_ENTRY_H_
#define REFERRAL_INSURANCE_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class ReferralInsuranceEntry;

class ReferralInsuranceEntry {
public:

    Wt::WString insuranceName;
    int numReferrals;
    int numRefScheduled;
    int numRefComplete;


};

#endif // REFERRAL_INSURANCE_ENTRY_H_
