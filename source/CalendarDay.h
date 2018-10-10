/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef CALENDAR_DAY_H_
#define CALENDAR_DAY_H_

#include <Wt/WDate>
#include <Wt/WContainerWidget>
#include <array>
#include <vector>
#include "CalendarEvent.h"


class CalendarDay;

class CalendarDay {
public:
    CalendarDay();
    CalendarDay(const Wt::WDate displayDate, const Wt::WDate curdate, std::vector<CalendarEvent> *events);
    /* You probably want to add other practice information here */
    Wt::WContainerWidget *container;

    Wt::WDate curDate;
    Wt::WDate displayDate;
    int number;
    std::vector<CalendarEvent> *events;
    std::vector<CalendarEvent> myevents;

    Wt::WContainerWidget *evcontainer;
    Wt::WContainerWidget *evlb;
    Wt::WContainerWidget *evdny;

    Wt::WVBoxLayout *evbox;

    bool shouldBeRendered();

};

#endif // CALENDAR_DAY_H_
