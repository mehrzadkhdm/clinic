/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PENDING_REFERRAL_ENTRY_H_
#define PENDING_REFERRAL_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class PendingReferralEntry;

class PendingReferralEntry {
public:


    Wt::WString patientFirstName;
    Wt::WString patientLastName;
    Wt::WString physicianName;
    Wt::WString patientDob;
    Wt::WString patientCity;
    Wt::WString status;
 	Wt::WString referralId;


};

#endif // PENDING_REFERRAL_ENTRY_H_
