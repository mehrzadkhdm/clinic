/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_APP_TEMPLATE_H_
#define ADMIN_APP_TEMPLATE_H_

#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>

#include "AdminHomeWidget.h"
#include "AdminReferralWidget.h"
#include "AdminPracticeCountWidget.h"
#include "AdminInsuranceWidget.h"
#include "AdminPhysicianVisitWidget.h"
#include "AdminPhysicianIssueWidget.h"
#include "AdminShiftNoteWidget.h"
#include "AdminPreMapWidget.h"
#include "LocalityReferralCountWidget.h"
#include "StaffSleepStudyWidget.h"
#include "StaffPatientWidget.h"
#include "StaffReschedulePatientWidget.h"
#include "StaffPracticeWidget.h"
#include "StaffBackupListWidget.h"
#include "StaffInsuranceListWidget.h"
#include "StaffStudyTypeWidget.h"
#include "StaffSleepCalendarWidget.h"
#include "StaffReferralWidget.h"
#include "StaffCancellationWidget.h"
#include "MarketingVisitReportWidget.h"
#include "MarketingPhysicianReportWidget.h"
#include "InsuranceGroupListWidget.h"
#include "UserSession.h"

using namespace Wt;

class AdminAppTemplate : public WTemplate
{

public:
    AdminAppTemplate(const char *conninfo, UserSession *session, WContainerWidget *parent);
    void handleInternalPath(const std::string &internalPath);

private:
    void init();
    void authEvent();

    UserSession *session_;
    WAnchor *brandAnchor_;
    WAnchor *settingsAnchor_;
    Auth::AuthWidget *authWidget_;
    Auth::AuthModel *authModel_;
    WStackedWidget *mainStack_;
    WMenu *sidebar_;

    void showHome();
    void showStudyReport();
    void showReferrals();
    void showPracticeCount();
    void showInsurance();
    void showVisits();
    void showIssues();
    void showLocalityCount();
    void showVisitReport();
    void showPhysicianReport();
    void showReferralList();
    void showSleepStudy();
    void showPatient();
    void showReschedulePatient();
    void showProviders();
    void showInsuranceList();
    void showInsuranceGroup();
    void showStudyTypeList();
    void showBackupList();
    void showSleepCalendar();
    void showSleepCancellations();
    void showShiftNote();
    void showMap();

    AdminHomeWidget *home_;
    AdminReferralWidget *referral_;
    AdminPracticeCountWidget *pracCount_;
    AdminInsuranceWidget *insurance_;
    AdminPhysicianVisitWidget *physicianVisit_;
    AdminPhysicianIssueWidget *physicianIssue_;
    AdminShiftNoteWidget *shiftNote_;
    AdminPreMapWidget *map_;
    
    LocalityReferralCountWidget *localCount_;
    
    StaffSleepStudyWidget *study_;
    StaffPatientWidget *patient_;
    StaffReschedulePatientWidget *rpatient_;
    StaffPracticeWidget *practice_;
    StaffInsuranceListWidget *ptinsurance_;
    StaffStudyTypeWidget *studytype_;
    StaffBackupListWidget *backup_;
    StaffSleepCalendarWidget *calendar_;
    StaffCancellationWidget *cancel_;
    StaffReferralWidget *ptreferral_;
    
    InsuranceGroupListWidget *insuranceGroup_;

    MarketingVisitReportWidget *visitReport_;
    MarketingPhysicianReportWidget *physicianReport_;

    const char *adminconninfo_;


};

#endif //ADMIN_APP_TEMPLATE_H_
