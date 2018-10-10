/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_LOGIN_TEMPLATE_H_
#define PRACTICE_LOGIN_TEMPLATE_H_

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
#include "PracticeRecentWidget.h"
#include "PracticeSession.h"
#include "PracticeSettingsWidget.h"
#include "PracticeOrderWidget.h"

using namespace Wt;

class PracticeLoginTemplate : public WTemplate
{

public:
    PracticeLoginTemplate(PracticeSession *session, WContainerWidget *parent);

private:
    void init();

    PracticeSession *session_;
    WAnchor *brandAnchor_;
    Auth::AuthModel *authModel_;
    PracticeAuthWidget *authWidget_;

};

#endif //PRACTICE_LOGIN_TEMPLATE_H_
