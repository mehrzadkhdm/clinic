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

#include "StaffAppTemplate.h"
#include "StaffHomeWidget.h"
#include "StaffStudyOrderWidget.h"
#include "StaffInboundWidget.h"
#include "StaffSleepStudyWidget.h"
#include "StaffReportWidget.h"
#include "StaffInsuranceListWidget.h"
#include "StaffStudyTypeWidget.h"
#include "StaffPatientWidget.h"
#include "StaffPracticeWidget.h"
#include "StaffBackupListWidget.h"
#include "StaffReferralWidget.h"
#include "StaffSleepCalendarWidget.h"
#include "StaffCancellationWidget.h"
#include "UserSession.h"

using namespace Wt;

StaffAppTemplate::StaffAppTemplate(const char *conninfo, UserSession *session, WContainerWidget *parent = 0)
    : WTemplate(parent),
    session_(session),
    staffconninfo_(conninfo)
{
    init();
}

void StaffAppTemplate::init()
{
    session_->login().changed().connect(this, &StaffAppTemplate::authEvent);

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
    sidebar_->setStyleClass("nav nav-sidebar");
    sidebar_->addItem("Dashboard")
        ->setLink(WLink(WLink::InternalPath, "/home"));
    sidebar_->addSectionHeader("MD Portal Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("Online Referrals")
        ->setLink(WLink(WLink::InternalPath, "/inbound"));
    sidebar_->addItem("Inbound Study Orders")
        ->setLink(WLink(WLink::InternalPath, "/orders"));
    sidebar_->addItem("Study Report Requests")
        ->setLink(WLink(WLink::InternalPath, "/requests"));
    sidebar_->addSectionHeader("Clinical Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("Providers")
        ->setLink(WLink(WLink::InternalPath, "/providers"));
    sidebar_->addItem("Patients")
        ->setLink(WLink(WLink::InternalPath, "/patients"));
    sidebar_->addItem("Referrals")
        ->setLink(WLink(WLink::InternalPath, "/referrals"));
    sidebar_->addItem("Appointment Calendar")
        ->setLink(WLink(WLink::InternalPath, "/calendar"));
    sidebar_->addItem("No Shows / Cancellations")
        ->setLink(WLink(WLink::InternalPath, "/no-show"));
    sidebar_->addSectionHeader("Other Resources");
    sidebar_->addSeparator();
    sidebar_->addItem("Insurance List")
        ->setLink(WLink(WLink::InternalPath, "/insurance-list"));
    sidebar_->addItem("Study Type List")
        ->setLink(WLink(WLink::InternalPath, "/study-type-list"));


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

    // hbox->addWidget(leftMenu);
    // hbox->addWidget(mainStack_);

    // links_->addWidget(container2);

    // links_->hide();

    // root()->addWidget(footer);
    WApplication::instance()->internalPathChanged()
        .connect(this, &StaffAppTemplate::handleInternalPath);
}

void StaffAppTemplate::authEvent() {

    if (session_->login().loggedIn()){
        handleInternalPath(WApplication::instance()->internalPath());
    }
}

void StaffAppTemplate::handleInternalPath(const std::string &internalPath)
{
    if (session_->login().loggedIn()) {
        if (internalPath == "/home")
            showHome();
        else if (internalPath == "/inbound")
            showInbound();
        else if (internalPath == "/referrals")
            showReferrals();
        else if (internalPath == "/orders")
            showOrder();
        else if (internalPath == "/studies")
            showSleepStudy();
        else if (internalPath == "/requests")
            showReport();
        else if (internalPath == "/providers")
            showProviders();
        else if (internalPath == "/patients")
            showPatient();
        else if (internalPath == "/insurance-list")
            showInsuranceList();
        else if (internalPath == "/study-type-list")
            showStudyTypeList();
        else if (internalPath == "/no-show")
            showBackupList();
        else if (internalPath == "/calendar")
            showSleepCalendar();
        else if (internalPath == "/cancellations")
            showSleepCancellations();
        else
            WApplication::instance()->setInternalPath("/home", true);
    }
}

void StaffAppTemplate::showHome()
{
    mainStack_->clear();
    home_ = new StaffHomeWidget(staffconninfo_, session_, mainStack_);
    home_->showHomeWidget();
    mainStack_->setCurrentWidget(home_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void StaffAppTemplate::showInbound()
{

    mainStack_->clear();
    inbound_ = new StaffInboundWidget(staffconninfo_, session_, mainStack_);
    inbound_->searchList();
    mainStack_->setCurrentWidget(inbound_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showReferrals()
{

    mainStack_->clear();
    referral_ = new StaffReferralWidget(staffconninfo_, session_, mainStack_);
    referral_->searchList();
    mainStack_->setCurrentWidget(referral_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showOrder()
{

    mainStack_->clear();
    order_ = new StaffStudyOrderWidget(staffconninfo_, mainStack_);
    order_->orderList();
    mainStack_->setCurrentWidget(order_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showSleepStudy()
{

    mainStack_->clear();
    study_ = new StaffSleepStudyWidget(staffconninfo_, session_, mainStack_);
    study_->sleepStudyList();
    mainStack_->setCurrentWidget(study_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showReport()
{
    mainStack_->clear();
    report_ = new StaffReportWidget(staffconninfo_, mainStack_);
    report_->reportList();
    mainStack_->setCurrentWidget(report_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showPatient()
{
    mainStack_->clear();
    patient_ = new StaffPatientWidget(staffconninfo_, session_, mainStack_);
    patient_->patientList();
    mainStack_->setCurrentWidget(patient_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showProviders()
{
    mainStack_->clear();
    practice_ = new StaffPracticeWidget(staffconninfo_, session_, mainStack_);
    practice_->practiceList();
    mainStack_->setCurrentWidget(practice_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showInsuranceList()
{
    mainStack_->clear();
    insurance_ = new StaffInsuranceListWidget(staffconninfo_, session_, mainStack_);
    insurance_->insuranceList();
    mainStack_->setCurrentWidget(insurance_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showStudyTypeList()
{
    mainStack_->clear();
    studytype_ = new StaffStudyTypeWidget(staffconninfo_, session_, mainStack_);
    studytype_->studyTypeList();
    mainStack_->setCurrentWidget(studytype_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showBackupList()
{
    mainStack_->clear();
    backup_ = new StaffBackupListWidget(staffconninfo_, session_, mainStack_);
    backup_->backupList();
    mainStack_->setCurrentWidget(backup_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showSleepCalendar()
{

    mainStack_->clear();
    calendar_ = new StaffSleepCalendarWidget(staffconninfo_, session_, mainStack_);
    calendar_->monthView();
    mainStack_->setCurrentWidget(calendar_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void StaffAppTemplate::showSleepCancellations()
{

    mainStack_->clear();
    cancel_ = new StaffCancellationWidget(staffconninfo_, session_, mainStack_);
    cancel_->studyList();
    mainStack_->setCurrentWidget(cancel_);

    /*
    inboundAnchor_->addStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

