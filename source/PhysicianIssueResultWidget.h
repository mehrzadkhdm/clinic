/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ISSUE_RESULT_WIDGET_H_
#define ISSUE_RESULT_WIDGET_H_

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

class PhysicianIssueResultWidget : public Wt::WContainerWidget
{

public:
    PhysicianIssueResultWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }

private:

    //done signal to refresh app
    Wt::Signal<int, std::string> done_;

     //stopTimer signal to refresh app
     Wt::Signal<int, std::string> stopTimer_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
    const char *conninfo_;
    UserSession *session_;

public:


    void showPhysicianIssueLog(Wt::WString& curstr, int limit, int offset);
    void showPhysicianIssueDialog(Wt::WString isid);

    Wt::WString setCommentId();

    Wt::WText *staticComment_;


    Wt::WPushButton *btnSelectPhysicianIssue;

};

#endif //ISSUE_RESULT_WIDGET_H
