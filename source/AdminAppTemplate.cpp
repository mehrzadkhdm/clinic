/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/
#include <Wt/WContainerWidget>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WLink>

#include <Wt/WMenu>
#include <Wt/WNavigationBar>
#include <Wt/WStackedWidget>
#include "AdminAppTemplate.h"
#include "AdminReferralWidget.h"
#include "AdminPracticeCountWidget.h"
#include "AdminInsuranceWidget.h"
#include "AdminInsuranceWidget.h"
#include "AdminPhysicianIssueWidget.h"
#include "AdminShiftNoteWidget.h"
#include "AdminPreMapWidget.h"
#include "LocalityReferralCountWidget.h"
#include "StaffSleepStudyWidget.h"
#include "StaffPatientWidget.h"
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

AdminAppTemplate::AdminAppTemplate(const char *conninfo, UserSession *session, WContainerWidget *parent = 0)
    : WTemplate(parent),
    session_(session),
    adminconninfo_(conninfo)
{
    init();
}

void AdminAppTemplate::init()
{
    session_->login().changed().connect(this, &AdminAppTemplate::authEvent);

    setTemplateText(WString::tr("template.StaffApp"));

    // Brand link
    brandAnchor_ = new WAnchor();
    brandAnchor_->setLink(WLink(WLink::InternalPath, "/home"));
    brandAnchor_->setText("Clinic Portal");
    brandAnchor_->addStyleClass("navbar-brand");
    bindWidget("brand", brandAnchor_);


    // Settings link
    settingsAnchor_ = new WAnchor();
    settingsAnchor_->setLink(Wt::WLink(Wt::WLink::InternalPath, "/settings"));
    settingsAnchor_->setText("Settings");
    bindWidget("settings", settingsAnchor_);

    // Auth model
    authModel_ = new Auth::AuthModel(UserSession::auth(),
                           session_->users(), this);
    authModel_->addPasswordAuth(&UserSession::passwordAuth());

    // Auth widget
    Auth::AuthWidget *authWidget_ = new Auth::AuthWidget(session_->login());
    authWidget_->setModel(authModel_);
    authWidget_->setRegistrationEnabled(false);
    //authWidget_->processEnvironment();

    bindWidget("auth-widget", authWidget_);

    sidebar_ = new WMenu();
    sidebar_->addItem("Home")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/home"));
    sidebar_->addSectionHeader("Admin Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("MD Referrals Report")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/referrals"));
    sidebar_->addItem("PT Insurance Report")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/insurance"));
    sidebar_->addItem("MD Referral Counts")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/mdcounts"));
    sidebar_->addItem("End of Shift Notes")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/shift-notes"));
    sidebar_->addSectionHeader("Staff Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("Provider List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/provider-list"));
    sidebar_->addItem("Patient List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/patient-list"));
    sidebar_->addItem("Referral List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/referral-list"));
    sidebar_->addItem("Study List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/study-list"));
    sidebar_->addItem("Study Calendar")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/study-calendar"));
    sidebar_->addItem("Cancellations")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/cancellations"));
    sidebar_->addItem("Rescheduled Patients")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/rescheduled"));
    sidebar_->addItem("Backup List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/backup-list"));
    sidebar_->addItem("Insurance List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/insurance-list"));
    sidebar_->addItem("Insurance Groups")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/insurance-groups"));
    sidebar_->addItem("Study Type List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/study-type-list"));
    sidebar_->addSectionHeader("Marketing Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("City Referral Counts")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/citycounts"));
    sidebar_->addItem("MD Visits List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/mdvisits"));
    sidebar_->addItem("MD Issues List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/mdissues"));
    // sidebar_->addItem("PCC Visits Report")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/mdvisits-report"));
    // sidebar_->addItem("MDs Added Report")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/new-md-report"));
    sidebar_->addItem("Map")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/map"));

    bindWidget("sidebar", sidebar_);

    mainStack_ = new WStackedWidget();
    mainStack_->setStyleClass("mainStack");
    mainStack_->setStyleClass("practice");
    bindWidget("main", mainStack_);

    // rightMenu->addItem("Settings")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/settings"));


    // mainStack_ = new WStackedWidget();

    // navigation->addWidget(authWidget, Wt::AlignRight);
    // container->addWidget(authWidget);


    // Wt::WContainerWidget *container2 = new Wt::WContainerWidget(links_);
    // Wt::WContainerWidget *footer = new Wt::WContainerWidget();
    // footer->setStyleClass("custom-footer");

    // Wt::WContainerWidget *footerin = new Wt::WContainerWidget();
    // footerin->setStyleClass("inside-footer");

    // Wt::WText *copyright = new Wt::WText("Copyright 2016 United Sleep Centers");
    // copyright->setStyleClass("footer-copyright");

    // Wt::WText *pes7 = new Wt::WText("Powered By <a href='http://pes7.com'>PES7 Healthcare Technology Solutions</a>");
    // pes7->setStyleClass("footer-pes7");


    // footerin->addWidget(copyright);
    // footerin->addWidget(pes7);

    // footer->addWidget(footerin);

    // Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();

    // container2->setLayout(hbox);

    // hbox->addWidget(H);
    // hbox->addWidget(mainStack_);

    // links_->addWidget(container2);

    // links_->hide();

    // root()->addWidget(footer);
    WApplication::instance()->internalPathChanged()
        .connect(this, &AdminAppTemplate::handleInternalPath);
}

void AdminAppTemplate::authEvent() {

    if (session_->login().loggedIn()){
        handleInternalPath(WApplication::instance()->internalPath());
    }
}

void AdminAppTemplate::handleInternalPath(const std::string &internalPath)
{
    if (session_->login().loggedIn()) {
        if (internalPath == "/home")
            showHome();
        else if (internalPath == "/referrals")
            showReferrals();
        else if (internalPath == "/insurance")
            showInsurance();
        else if (internalPath == "/mdcounts")
            showPracticeCount();
        else if (internalPath == "/citycounts")
            showLocalityCount();
        else if (internalPath == "/mdvisits")
            showVisits();
        else if (internalPath == "/mdissues")
            showIssues();
        else if (internalPath == "/mdvisits-report")
            showVisitReport();
        else if (internalPath == "/new-md-report")
            showPhysicianReport();
        else if (internalPath == "/shift-notes")
            showShiftNote();
        else if (internalPath == "/provider-list")
            showProviders();
        else if (internalPath == "/patient-list")
            showPatient();
        else if (internalPath == "/rescheduled")
            showReschedulePatient();
        else if (internalPath == "/referral-list")
            showReferralList();
        else if (internalPath == "/study-list")
            showSleepStudy();
        else if (internalPath == "/study-calendar")
            showSleepCalendar();
        else if (internalPath == "/cancellations")
            showSleepCancellations();
        else if (internalPath == "/backup-list")
            showBackupList();
        else if (internalPath == "/insurance-list")
            showInsuranceList();
        else if (internalPath == "/insurance-groups")
            showInsuranceGroup();
        else if (internalPath == "/study-type-list")
            showStudyTypeList();
        else if (internalPath == "/map")
            showMap();
        else
            WApplication::instance()->setInternalPath("/home", true);
    }
}

void AdminAppTemplate::showHome()
{
    mainStack_->clear();
    home_ = new AdminHomeWidget(mainStack_);
    mainStack_->setCurrentWidget(home_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showReferrals()
{
    mainStack_->clear();
    referral_ = new AdminReferralWidget(adminconninfo_, session_, mainStack_);
    referral_->searchList();
    mainStack_->setCurrentWidget(referral_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showPracticeCount()
{
    mainStack_->clear();
    pracCount_ = new AdminPracticeCountWidget(adminconninfo_, session_, mainStack_);
    pracCount_->showReferralCount();
    mainStack_->setCurrentWidget(pracCount_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showLocalityCount()
{
    mainStack_->clear();
    localCount_ = new LocalityReferralCountWidget(adminconninfo_, session_, mainStack_);
    localCount_->showReferralCount();
    mainStack_->setCurrentWidget(localCount_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showInsurance()
{
    mainStack_->clear();
    insurance_ = new AdminInsuranceWidget(adminconninfo_, session_, mainStack_);
    insurance_->searchList();
    mainStack_->setCurrentWidget(insurance_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showInsuranceGroup()
{
    mainStack_->clear();
    insuranceGroup_ = new InsuranceGroupListWidget(adminconninfo_, session_, mainStack_);
    insuranceGroup_->insuranceList();
    mainStack_->setCurrentWidget(insuranceGroup_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showVisits()
{
    mainStack_->clear();
    physicianVisit_ = new AdminPhysicianVisitWidget(adminconninfo_, session_, mainStack_);
    physicianVisit_->visitList();
    mainStack_->setCurrentWidget(physicianVisit_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showIssues()
{
    mainStack_->clear();
    physicianIssue_ = new AdminPhysicianIssueWidget(adminconninfo_, session_, mainStack_);
    physicianIssue_->issueList();
    mainStack_->setCurrentWidget(physicianIssue_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}
void AdminAppTemplate::showVisitReport()
{
    mainStack_->clear();
    visitReport_ = new MarketingVisitReportWidget(adminconninfo_, session_, mainStack_);
    visitReport_->newVisitReport();
    mainStack_->setCurrentWidget(visitReport_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void AdminAppTemplate::showPhysicianReport()
{
    mainStack_->clear();
    physicianReport_ = new MarketingPhysicianReportWidget(adminconninfo_, session_, mainStack_);
    physicianReport_->newVisitReport();
    mainStack_->setCurrentWidget(physicianReport_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}




void AdminAppTemplate::showReferralList()
{

    mainStack_->clear();
    ptreferral_ = new StaffReferralWidget(adminconninfo_, session_, mainStack_);
    ptreferral_->searchList();
    mainStack_->setCurrentWidget(ptreferral_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}
void AdminAppTemplate::showSleepStudy()
{

    mainStack_->clear();
    study_ = new StaffSleepStudyWidget(adminconninfo_, session_, mainStack_);
    study_->sleepStudyList();
    mainStack_->setCurrentWidget(study_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}
void AdminAppTemplate::showPatient()
{
    mainStack_->clear();
    patient_ = new StaffPatientWidget(adminconninfo_, session_, mainStack_);
    patient_->patientList();
    mainStack_->setCurrentWidget(patient_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showReschedulePatient()
{
    mainStack_->clear();
    rpatient_ = new StaffReschedulePatientWidget(adminconninfo_, session_, mainStack_);
    rpatient_->patientList();
    mainStack_->setCurrentWidget(rpatient_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}


void AdminAppTemplate::showProviders()
{
    mainStack_->clear();
    practice_ = new StaffPracticeWidget(adminconninfo_, session_, mainStack_);
    practice_->practiceList();
    mainStack_->setCurrentWidget(practice_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showInsuranceList()
{
    mainStack_->clear();
    ptinsurance_ = new StaffInsuranceListWidget(adminconninfo_, session_, mainStack_);
    ptinsurance_->insuranceList();
    mainStack_->setCurrentWidget(ptinsurance_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showStudyTypeList()
{
    mainStack_->clear();
    studytype_ = new StaffStudyTypeWidget(adminconninfo_, session_, mainStack_);
    studytype_->studyTypeList();
    mainStack_->setCurrentWidget(studytype_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showBackupList()
{
    mainStack_->clear();
    backup_ = new StaffBackupListWidget(adminconninfo_, session_, mainStack_);
    backup_->backupList();
    mainStack_->setCurrentWidget(backup_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showSleepCalendar()
{

    mainStack_->clear();
    calendar_ = new StaffSleepCalendarWidget(adminconninfo_, session_, mainStack_);
    calendar_->monthView();
    mainStack_->setCurrentWidget(calendar_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showSleepCancellations()
{

    mainStack_->clear();
    cancel_ = new StaffCancellationWidget(adminconninfo_, session_, mainStack_);
    cancel_->studyList();
    mainStack_->setCurrentWidget(cancel_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showShiftNote()
{

    mainStack_->clear();
    shiftNote_ = new AdminShiftNoteWidget(adminconninfo_, session_, mainStack_);
    shiftNote_->showNotes();
    mainStack_->setCurrentWidget(shiftNote_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void AdminAppTemplate::showMap()
{

    mainStack_->clear();
    map_ = new AdminPreMapWidget(adminconninfo_, session_, mainStack_);
    map_->showMap();
    mainStack_->setCurrentWidget(map_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}



