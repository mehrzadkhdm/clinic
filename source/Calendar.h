/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef CALENDAR_H_
#define CALENDAR_H_
#include <Wt/WDate>
#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDate>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>
#include <array>
#include <vector>
#include "CalendarEvent.h"
#include "CalendarDay.h"
#include "CalendarWeek.h"


class Calendar;

//a six week calendar

class Calendar {
public:
    Calendar();
    Calendar(Wt::WDate displayDate, std::vector<CalendarEvent> *events);

    Wt::WDate displayDate;
    Wt::WDate startDate;
    Wt::WDate endDate;
    CalendarWeek *weeks[6];
    std::vector<CalendarEvent> *events;


private:

};

#endif // CALENDAR_H_
