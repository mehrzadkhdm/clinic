/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef MARKETINGAPP_H_
#define MARKETINGAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "MarketingHomeWidget.h"
#include "UserSession.h"
#include "PccReferralWidget.h"
#include "PccSleepStudyWidget.h"
#include "AdminPracticeCountWidget.h"
#include "MarketingPracticeWidget.h"
#include "MarketingPhysicianVisitWidget.h"
#include "MarketingPhysicianIssueWidget.h"
#include "MarketingLocalityListWidget.h"
#include "MarketingPracticeGroupWidget.h"
#include "PhysicianAddressListWidget.h"
#include "StaffPatientWidget.h"

using namespace Wt;

class MarketingApplication : public Wt::WApplication
{

public:
    MarketingApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    Wt::WStackedWidget *mainStack_;
    void authEvent();
    void showHome();
    void showStudyReport();
    void showReferrals();
    void showStudies();
    void showProviders();
    void showPracticeCount();
    void showPhysicianVisit();
    void showPhysicianIssue();
    void showLocality();
    void showAddress();
    void showGroup();
    void showPatient();

    MarketingHomeWidget *home_;
    PccReferralWidget *referral_;
    PccSleepStudyWidget *study_;
    AdminPracticeCountWidget *pracCount_;
    MarketingPracticeWidget *practice_;
    MarketingPhysicianVisitWidget *physicianVisit_;
    MarketingPhysicianIssueWidget *physicianIssue_;
    MarketingLocalityListWidget *localityList_;
    MarketingPracticeGroupWidget *groupList_;
    PhysicianAddressListWidget *addressList_;
    StaffPatientWidget *patient_;
    Wt::WContainerWidget *links_;
    UserSession session_;

};

WApplication *createMarketingApplication(const WEnvironment& env);

#endif //MARKETINGAPP_H_
