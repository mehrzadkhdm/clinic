/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICEAPP_H_
#define PRACTICEAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "PracticeAppTemplate.h"
#include "PracticeLoginTemplate.h"
#include "PracticeHomeWidget.h"
#include "PracticeReferralWidget.h"
#include "PracticeReferralView.h"
#include "PracticeReportWidget.h"
#include "PracticeRecentWidget.h"
#include "PracticeSession.h"
#include "PracticeSettingsWidget.h"
#include "PracticeOrderWidget.h"

using namespace Wt;

class PracticeApplication : public Wt::WApplication
{

public:
    PracticeApplication(const Wt::WEnvironment& env);

    void init();

private:
    Wt::WStackedWidget *mainStack_;
    void authEvent();

    WStackedWidget *loginStack_;
    WBootstrapTheme *bootstrap_;
    PracticeAppTemplate *template_;
    PracticeLoginTemplate *login_;
    Wt::WString prac = "";
    Wt::WContainerWidget *links_;
    PracticeSession session_;
    Wt::WAnchor *reportAnchor_;
    Wt::WAnchor *orderAnchor_;
    Wt::WAnchor *recentAnchor_;
    Wt::WAnchor *newReferralAnchor_;
    Wt::WAnchor *homeAnchor_;

};

WApplication *createPracticeApplication(const WEnvironment& env);

#endif //STAFFAPP_H_
