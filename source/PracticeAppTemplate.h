/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_APP_TEMPLATE_H_
#define PRACTICE_APP_TEMPLATE_H_

#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "PracticeAuthWidget.h"
#include "PracticeHomeWidget.h"
#include "PracticeReferralWidget.h"
#include "PracticeReferralView.h"
#include "PracticeReportWidget.h"
#include "PracticeStudyProgressWidget.h"
#include "PracticeSession.h"
#include "PracticeSettingsWidget.h"
#include "PracticeOrderWidget.h"
#include "PracticePreReferralWidget.h"

using namespace Wt;

class PracticeAppTemplate : public WTemplate
{

public:
    PracticeAppTemplate(const char *conninfo, Wt::WString *prac, PracticeSession *session, WContainerWidget *parent);
    void handleInternalPath(const std::string &internalPath);

private:
    void init();
    void authEvent();
    void showHome();
    void showReferral();
    void showReport();
    void showOrder();
    void showSettings();
    void showRecent();

    PracticeSession *session_;
    WAnchor *brandAnchor_;
    WAnchor *settingsAnchor_;
    PracticeAuthWidget *authWidget_;
    Auth::AuthModel *authModel_;
    WStackedWidget *mainStack_;
    WMenu *sidebar_;
    Wt::WString *prac_;
    PracticeHomeWidget *home_;
    PracticePreReferralWidget *referral_;
    PracticeReportWidget *report_;
    PracticeOrderWidget *order_;
    PracticeStudyProgressWidget *recent_;
    PracticeSettingsWidget *settings_;
    const char *pracconninfo_;


};

#endif //PRACTICE_APP_TEMPLATE_H_
