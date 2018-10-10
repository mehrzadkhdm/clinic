#include <Wt/WDate>
#include <array>
#include <vector>
#include <sstream>
#include "CalendarWeek.h"


using namespace std;
using namespace Wt;

CalendarWeek::CalendarWeek(const Wt::WDate displayDate, const Wt::WDate startDate, std::vector<CalendarEvent> *events) :
    displayDate(displayDate),
    startDate(startDate),
    events(events)
{
    //set the end date
    endDate = startDate.addDays(6);

    Wt::WDate curDate;
    curDate = startDate;

    // loop 6 times

    for (int count = 0; count < 7; count ++)
    {
        //   instance week, pass in curDate
        CalendarDay *day = new CalendarDay(displayDate, curDate, events);
        days[count] = day;
        curDate = curDate.addDays(1);
        Wt::log("notice") << "CalendarWeek curDate is" << curDate.day() << "/" << curDate.month() << "/" << curDate.year();
        //   continue loop
    }
}
