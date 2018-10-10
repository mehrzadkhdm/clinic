/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef REFERRAL_COUNT_ENTRY_H_
#define REFERRAL_COUNT_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class ReferralCountEntry;

class ReferralCountEntry {
public:


    Wt::WString groupName;
    Wt::WString numThisMonth;
    Wt::WString numScheduledThisMonth;
    Wt::WString numUnscheduledThisMonth;
    Wt::WString numLastMonth;
    Wt::WString numScheduledLastMonth;
    Wt::WString numUnscheduledLastMonth;    
    Wt::WString numTwoMonths;
    Wt::WString numScheduledTwoMonths;
    Wt::WString numUnscheduledTwoMonths;
    Wt::WString numMonthFour;
    Wt::WString numScheduledMonthFour;
    Wt::WString numUnscheduledMonthFour;    
    Wt::WString numMonthFive;
    Wt::WString numScheduledMonthFive;
    Wt::WString numUnscheduledMonthFive;
    Wt::WString numMonthSix;
    Wt::WString numScheduledMonthSix;
    Wt::WString numUnscheduledMonthSix;    
    Wt::WString numYearToDate;
    Wt::WString numScheduledYearToDate;
    Wt::WString numUnscheduledYearToDate;
    
    Wt::WDateTime dateAdded;
    Wt::WDate lastReceivedDate;


};

#endif // REFERRAL_COUNT_ENTRY_H_
