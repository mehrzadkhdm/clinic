/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef MARKETING_ADMIN_APP_H_
#define MARKETING_ADMIN_APP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "MarketingHomeWidget.h"
#include "UserSession.h"
#include "MarketingReferralWidget.h"
#include "AdminPracticeCountWidget.h"
#include "MarketingPracticeWidget.h"
#include "MarketingVisitReportWidget.h"
#include "MarketingPhysicianIssueWidget.h"
#include "MarketingPhysicianReportWidget.h"
#include "AdminReferralWidget.h"


using namespace Wt;

class MarketingAdminApplication : public Wt::WApplication
{

public:
    MarketingAdminApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    Wt::WStackedWidget *mainStack_;
    void authEvent();
    void showHome();
    void showStudyReport();
    void showReferrals();
    void showProviders();
    void showPracticeCount();
    void showVisitReport();
    void showPhysicianIssue();
    void showPhysicianReport();
    void showReferralReport();

    MarketingHomeWidget *home_;
    MarketingReferralWidget *referral_;
    AdminPracticeCountWidget *pracCount_;
    MarketingPracticeWidget *practice_;
    MarketingVisitReportWidget *visitReport_;
    MarketingPhysicianIssueWidget *physicianIssue_;
    MarketingPhysicianReportWidget *physicianReport_;
    AdminReferralWidget *referralReport_;

    Wt::WContainerWidget *links_;
    UserSession session_;

};

WApplication *createMarketingAdminApplication(const WEnvironment& env);

#endif //MARKETING_ADMIN_APP_H_
