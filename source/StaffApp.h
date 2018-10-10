    /*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFFAPP_H_
#define STAFFAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "StaffHomeWidget.h"
#include "StaffReportWidget.h"
#include "StaffInboundWidget.h"
#include "StaffStudyOrderWidget.h"
#include "StaffSleepStudyWidget.h"
#include "StaffPatientWidget.h"
#include "StaffPracticeWidget.h"
#include "StaffBackupListWidget.h"
#include "StaffInsuranceListWidget.h"
#include "StaffStudyTypeWidget.h"
#include "StaffSleepCalendarWidget.h"
#include "StaffReferralWidget.h"
#include "StaffCancellationWidget.h"

#include "StaffLoginTemplate.h"
#include "StaffAppTemplate.h"
#include "UserSession.h"

using namespace Wt;

class StaffApplication : public Wt::WApplication
{

public:
    StaffApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    void authEvent();
    void init();
    WStackedWidget *loginStack_;
    WBootstrapTheme *bootstrap_;
    StaffAppTemplate *template_;
    StaffLoginTemplate *login_;

    Wt::WContainerWidget *links_;
    UserSession session_;



};

WApplication *createStaffApplication(const WEnvironment& env);

#endif //STAFFAPP_H_
