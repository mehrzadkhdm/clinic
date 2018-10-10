/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_APP_TEMPLATE_H_
#define STAFF_APP_TEMPLATE_H_

#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>
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
#include "UserSession.h"

using namespace Wt;

class StaffAppTemplate : public WTemplate
{

public:
    StaffAppTemplate(const char *conninfo, UserSession *session, WContainerWidget *parent);
    void handleInternalPath(const std::string &internalPath);

private:
    void init();
    void authEvent();
    void showHome();
    void showInbound();
    void showOrder();
    void showReport();
    void showSleepStudy();
    void showPatient();
    void showInsuranceList();
    void showStudyTypeList();
    void showBackupList();
    void showSleepCalendar();
    void showReferrals();
    void showProviders();
    void showSleepCancellations();
    
    UserSession *session_;
    WAnchor *brandAnchor_;
    WAnchor *settingsAnchor_;
    Auth::AuthWidget *authWidget_;
    Auth::AuthModel *authModel_;
    WStackedWidget *mainStack_;
    WMenu *sidebar_;

    StaffHomeWidget *home_;
    StaffStudyOrderWidget *order_;
    StaffInboundWidget *inbound_;
    StaffSleepStudyWidget *study_;
    StaffReportWidget *report_;
    StaffPatientWidget *patient_;
    StaffPracticeWidget *practice_;
    StaffInsuranceListWidget *insurance_;
    StaffStudyTypeWidget *studytype_;
    StaffBackupListWidget *backup_;
    StaffSleepCalendarWidget *calendar_;
    StaffCancellationWidget *cancel_;
    StaffReferralWidget *referral_;

    const char *staffconninfo_;


};

#endif //STAFF_APP_TEMPLATE_H_
