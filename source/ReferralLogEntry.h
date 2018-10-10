/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef REFERRAL_LOG_ENTRY_H_
#define REFERRAL_LOG_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class ReferralLogEntry;

class ReferralLogEntry {
public:


    Wt::WString patientName;
    Wt::WString physicianName;
    Wt::WString patientInsurance;
    Wt::WString sleepStudyType;
    Wt::WString studyScheduled;
    Wt::WString referralId;

    Wt::WDate referralReceivedDate;
    Wt::WDate studyDate;



};

#endif // REFERRAL_LOG_ENTRY_H_
