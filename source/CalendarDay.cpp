#include <Wt/WDate>
#include <vector>
#include "CalendarDay.h"

using namespace std;
using namespace Wt;

CalendarDay::CalendarDay(const Wt::WDate displayDate, const Wt::WDate curDate, std::vector<CalendarEvent> *events) :
    displayDate(displayDate),
    curDate(curDate),
    events(events)
{
    std::vector<CalendarEvent> myevents;
    //loop over the contents of the events vector
    for (std::vector<CalendarEvent>::iterator i = events->begin(); i != events->end(); i++)
    {
        //compare event date to current date
        if (i->appointmentDate == curDate)
        {
            myevents.push_back(*i);
            log("notice") << "myevents-displayName: " << i->displayName;
        }

    }
    log("notice") << "myevents size: " << myevents.size();

}

//function shouldBeRendered, return true or false
    //is current date same month as display date

bool CalendarDay::shouldBeRendered() {
    if(displayDate.month() != curDate.month())
    {
        return false;
    }

    return true;
}
