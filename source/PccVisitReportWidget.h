/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PCC_VISIT_REPORT_WIDGET_H_
#define PCC_VISIT_REPORT_WIDGET_H_

#include <fstream>
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/WContainerWidget>
#include <Wt/WString>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/backend/Postgres>
#include "UserSession.h"

using namespace Wt;
using namespace Wt::Dbo;
class UserSession;

class PccVisitReportWidget : public Wt::WContainerWidget
{

public:
    PccVisitReportWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
    const char *conninfo_;
    UserSession *session_;

public:

    void showVisitReport(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& pcc);

    Wt::WString selectedDateOut_;
    Wt::WString startTimeOut_;
    Wt::WString endTimeOut_;
    Wt::WString totalTimeOut_;
    Wt::WString descriptionOut_;

    Wt::WText *outHoursId_;

    std::string strDate();
    std::string strStartTime();
    std::string strEndTime();
    std::string strTotalHours();
    std::string strDescription();

    Wt::WString setHoursId();
    Wt::WString strHoursId();

    Wt::WText *staticHours_;
    Wt::WText *hoursSent_;

    Wt::WString *startEntry;
    Wt::WString *endEntry;



};

#endif //PCC_VISIT_REPORT_WIDGET_H
