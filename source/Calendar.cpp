#include <Wt/WDate>
#include <vector>
#include <array>
#include "Calendar.h"
#include <Wt/WApplication>
#include <sstream>

using namespace std;
using namespace Wt;

Calendar::Calendar(Wt::WDate displayDate, std::vector<CalendarEvent> *events) :
    displayDate(displayDate),
    events(events)
{

    //log displayDate
    std::stringstream displayss;
    displayss << displayDate.toString("MM/dd/yyyy");
    Wt::log("notice") << displayss.str();

    //determine what day of the week the first of the month lands on
    Wt::WDate firstOfMonth;
    firstOfMonth = displayDate;

    //log first of the month
    std::stringstream firstss;
    firstss << firstOfMonth.toString("MM/dd/yyyy");
    Wt::log("notice") << "Calendar.cpp firstOfMonth = displayDate" << firstss.str();

    firstOfMonth.setDate(displayDate.year(), displayDate.month(), 1);
    startDate = firstOfMonth;

    //log firstOfMonth after setting date attributes above
    std::stringstream firstsetss;
    firstsetss << firstOfMonth.toString("MM/dd/yyyy");

    std::stringstream startdatess;
    startdatess << startDate.toString("MM/dd/yyyy");
    Wt::log("notice") << "firstOfMonth: " << firstsetss.str() << "startDate = firstOfMonth: " << startdatess.str();

    //calculate how many padding days before the first of the month
    int padding = firstOfMonth.dayOfWeek() - 1;

    //log padding
    std::stringstream paddingss;
    paddingss << padding;
    Wt::log("notice") << "padding = firstOfMonth.dayOfWeek - 1: " << paddingss.str();

    //calculate how many days are in the month
    Wt::WDate firstOfNextMonth;
    firstOfNextMonth.setDate(displayDate.year(), displayDate.month() + 1, 1);

    //log firstofnextmonth
    std::stringstream firstofnextss;
    firstofnextss << firstOfNextMonth.toString("MM/dd/yyyy");
    Wt::log("notice") << "firstOfNextMonth: " << firstofnextss.str();


    int daysInMonth = firstOfMonth.daysTo(firstOfNextMonth);
    //calcuate how many days in the month PLUS the padding days

    //log daysinmonth
    std::stringstream daysinss;
    daysinss << daysInMonth;
    Wt::log("notice") << "daysInMonth: " << daysinss.str();

    //subtract that from total number of days displayed .
    //unsure whether 41 or 42, we'll see
    int endPadding = 42 - padding - daysInMonth;

    //log endpadding
    std::stringstream endpaddingss;
    endpaddingss << endPadding;
    Wt::log("notice") << "endPadding:" << endpaddingss.str();


    endDate = firstOfNextMonth.addDays(-1);

    //log endDate
    std::stringstream enddatess;
    enddatess << endDate.toString("MM/dd/yyyy");
    Wt::log("notice") << "endDate: " << enddatess.str();

    Wt::WDate weekStartDate;
    weekStartDate = firstOfMonth.addDays(-padding-1);

    //log weekstartdate
    std::stringstream weekstartss;
    weekstartss << weekStartDate.toString("MM/dd/yyyy");
    Wt::log("notice") << "weekStartDate: " << weekstartss.str();

    // loop 6 times
    for (int count = 0; count < 6; count ++)
    {
        //   instance week, pass in weekStartDate
        CalendarWeek *week = new CalendarWeek(displayDate, weekStartDate, events);
        weeks[count] = week;
        weekStartDate = weekStartDate.addDays(7);

        //log weekstartdate for each week in the iteration
        std::stringstream weekitss;
        weekitss << weekStartDate.toString("MM/dd/yyyy");
        Wt::log("notice") << "weekstart iterator: " << weekitss.str();
        //   continue loop
    }

}
