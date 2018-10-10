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

#include "PracticeAppTemplate.h"
#include "PracticePreReferralWidget.h"
#include "PracticeSession.h"

using namespace Wt;

PracticeAppTemplate::PracticeAppTemplate(const char *conninfo, Wt::WString *prac, PracticeSession *session, WContainerWidget *parent = 0)
    : WTemplate(parent),
    session_(session),
    prac_(prac),
    pracconninfo_(conninfo)
{
    init();
}

void PracticeAppTemplate::init()
{
    session_->login().changed().connect(this, &PracticeAppTemplate::authEvent);

    setTemplateText(WString::tr("template.PracticeApp"));

    // Brand link
    brandAnchor_ = new WAnchor();
    brandAnchor_->setLink(WLink(WLink::InternalPath, "/home"));
    brandAnchor_->setText("Physician Portal");
    brandAnchor_->addStyleClass("navbar-brand");
    bindWidget("brand", brandAnchor_);


    // Settings link
    settingsAnchor_ = new WAnchor();
    settingsAnchor_->setLink(Wt::WLink(Wt::WLink::InternalPath, "/settings"));
    settingsAnchor_->setText("Settings");
    bindWidget("settings", settingsAnchor_);

    // Auth model
    authModel_ = new Auth::AuthModel(PracticeSession::auth(),
                           session_->practices(), this);
    authModel_->addPasswordAuth(&PracticeSession::passwordAuth());

    // Auth widget
    Auth::AuthWidget *authWidget_ = new Auth::AuthWidget(session_->login());
    authWidget_->setModel(authModel_);
    authWidget_->setRegistrationEnabled(false);
    //authWidget_->processEnvironment();

    bindWidget("auth-widget", authWidget_);

    sidebar_ = new WMenu();
    sidebar_->setStyleClass("nav nav-sidebar");
    sidebar_->addItem("Progress Tracking")
        ->setLink(WLink(WLink::InternalPath, "/dashboard"));
    sidebar_->addItem("New Online Referral")
        ->setLink(WLink(WLink::InternalPath, "/newreferral"));
    sidebar_->addItem("Upload Order Forms")
        ->setLink(WLink(WLink::InternalPath, "/sendorder"));
    sidebar_->addItem("Request a Report")
        ->setLink(WLink(WLink::InternalPath, "/report"));
    sidebar_->addItem("Sleep Resources")
        ->setLink(WLink(WLink::InternalPath, "/resources"));
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
        .connect(this, &PracticeAppTemplate::handleInternalPath);
}

void PracticeAppTemplate::authEvent() {

    if (session_->login().loggedIn()){
        handleInternalPath(WApplication::instance()->internalPath());
    }
}

void PracticeAppTemplate::handleInternalPath(const std::string &internalPath)
{
    if (session_->login().loggedIn()) {
        if (internalPath == "/dashboard")
            showRecent();
        else if (internalPath == "/sendorder")
            showOrder();
        else if (internalPath == "/newreferral")
            showReferral();
        else if (internalPath == "/report")
            showReport();
        else if (internalPath == "/resources")
            showHome();
        else if (internalPath == "/settings")
            showSettings();
        else
            WApplication::instance()->setInternalPath("/dashboard", true);
    }
}

void PracticeAppTemplate::showHome()
{
    mainStack_->clear();
    home_ = new PracticeHomeWidget(mainStack_);
    mainStack_->setCurrentWidget(home_);
    /*
    newReferralAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void PracticeAppTemplate::showReferral()
{
    mainStack_->clear();
    referral_ = new PracticePreReferralWidget(pracconninfo_, session_, mainStack_);
    mainStack_->setCurrentWidget(referral_);
    referral_->physicianList();
    /*
    newReferralAnchor_->addStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void PracticeAppTemplate::showOrder()
{
    mainStack_->clear();
    order_ = new PracticeOrderWidget(pracconninfo_, session_, mainStack_);
    mainStack_->setCurrentWidget(order_);
    order_->start();

    // newReferralAnchor_->addStyleClass("selected-link");
    // homeAnchor_->removeStyleClass("selected-link");

}

void PracticeAppTemplate::showReport()
{
    mainStack_->clear();
    report_ = new PracticeReportWidget(pracconninfo_, session_, mainStack_);
    mainStack_->setCurrentWidget(report_);
    /*
    newReferralAnchor_->addStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

void PracticeAppTemplate::showSettings()
{
    mainStack_->clear();
    settings_ = new PracticeSettingsWidget(pracconninfo_, authModel_, session_, mainStack_);
    mainStack_->setCurrentWidget(settings_);
}

void PracticeAppTemplate::showRecent()
{
    Wt::log("info") << "PracticeAppTemplate showRecent";

    mainStack_->clear();
    recent_ = new PracticeStudyProgressWidget(pracconninfo_, prac_, session_, mainStack_);
    mainStack_->setCurrentWidget(recent_);
    recent_->searchList();
    /*
    newReferralAnchor_->addStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}

