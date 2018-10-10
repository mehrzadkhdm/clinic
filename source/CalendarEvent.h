/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef CALENDAR_EVENT_H_
#define CALENDAR_EVENT_H_
#include <Wt/WDate>
#include <Wt/WTime>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class CalendarEvent;

class CalendarEvent {
public:

    Wt::WDate appointmentDate;
    Wt::WTime appointmentTime;

    Wt::WString appointmentStatus;
    Wt::WString studyType;
    Wt::WString studyId;
    Wt::WString displayName;
    Wt::WString    location;


};

#endif // CALENDAR_EVENT_H_
