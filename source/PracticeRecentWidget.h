/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_RECENT_WIDGET_H_
#define PRACTICE_RECENT_WIDGET_H_
#include "PracticeSession.h"
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
using namespace Wt;
using namespace Wt::Dbo;

class PracticeSession;

class PracticeRecentWidget : public Wt::WContainerWidget
{
public:
    PracticeRecentWidget(const char *conninfo, Wt::WString *prac, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
    Wt::WString *prac_;
    Wt::WString *pid;

    void pracRecent(Wt::WString& curstr);
    void showStudyDialog(WString& study);

    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }


private:
    //done signal to refresh app
    Wt::Signal<int, std::string> done_;

     //stopTimer signal to refresh app
    Wt::Signal<int, std::string> stopTimer_;
    
    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;

    PracticeSession *session_;
    Wt::WPushButton *btnDownloadRef;
};

#endif //PRACTICE_RECENT_WIDGET_H
