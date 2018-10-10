/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef CALENDAR_WEEK_H_
#define CALENDAR_WEEK_H_

#include <Wt/WDate>
#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>
#include <array>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <vector>
#include "CalendarDay.h"
#include "CalendarEvent.h"

using namespace std;

class CalendarWeek;

//a six week calendar

class CalendarWeek {
public:
    CalendarWeek();
    CalendarWeek(const Wt::WDate displayDate, const Wt::WDate startDate, std::vector<CalendarEvent> *events);

    Wt::WContainerWidget *container;
    Wt::WHBoxLayout *dayshbox;

    Wt::WDate startDate;
    Wt::WDate endDate;
    Wt::WDate displayDate;

    CalendarDay *days[7];
    std::vector<CalendarEvent> *events;

};

#endif // CALENDAR_WEEK_H_
