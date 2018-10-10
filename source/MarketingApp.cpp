/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBootstrapTheme>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>
#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>

#include "MarketingHomeWidget.h"
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

#include "MarketingApp.h"

const char *pccconninfo = "hostaddr = 127.0.0.1 port = 5432 user = clinicore dbname = clinicore password = Falcon1337";

MarketingApplication::MarketingApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env),
    session_(pccconninfo)
{
    session_.login().changed().connect(this, &MarketingApplication::authEvent);

    root()->addStyleClass("container");
    setTheme(new Wt::WBootstrapTheme());
    

    Wt::WLocale applocale;
    applocale.setTimeZone("PST-8PDT,M4.1.0,M10.5.0");
    setLocale(applocale);

    messageResourceBundle().use(appRoot() + "templates");
    messageResourceBundle().use(appRoot() + "strings");
    useStyleSheet("css/style.css");
    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    container->setStyleClass("main-conatiner");

    Wt::WImage *image = new Wt::WImage(Wt::WLink("images/logo.png"),
        container);
    image->setAlternateText("usc logo");
    image->setStyleClass("usc-logo");
    image->show();


    Wt::WContainerWidget *header = new Wt::WContainerWidget();
    header->setStyleClass("custom-header");

    Wt::WContainerWidget *headerin = new Wt::WContainerWidget();
    headerin->setStyleClass("inside-header");

    Wt::WText *unitedphone = new Wt::WText("Questions? Call our office at 562 622 1002");
    unitedphone->setStyleClass("header-unitedphone");


    headerin->addWidget(unitedphone);

    header->addWidget(headerin);


    root()->addWidget(header);
    root()->addWidget(container);

    Wt::WMenu *rightMenu = new Wt::WMenu();

    Wt::Auth::AuthWidget *authWidget
        = new Wt::Auth::AuthWidget(UserSession::auth(), session_.users(),
        session_.login());

    authWidget->model()->addPasswordAuth(&UserSession::passwordAuth());
    authWidget->setRegistrationEnabled(true);

    links_ = new WContainerWidget();
    links_->setStyleClass("links");
    root()->addWidget(links_);

    //nav
    Wt::WNavigationBar *navigation = new Wt::WNavigationBar(links_);
    navigation->setTitle("Patient Care Coordinator Portal",
        "https://127.0.0.1/marketing");
    navigation->setResponsive(true);

    Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
    contentsStack->addStyleClass("contents");

    // Setup a Left-aligned menu.
    Wt::WMenu *leftMenu = new Wt::WMenu(Wt::Vertical, links_);
    //navigation->addMenu(leftMenu);

    leftMenu->setStyleClass("nav nav-pills nav-stacked left-menu");

    leftMenu->addItem("Home")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/home"));
    leftMenu->addItem("Provider List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/providers"));
    leftMenu->addItem("Patient List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/patients"));
    leftMenu->addItem("Patient Referrals")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/referrals"));
    leftMenu->addItem("Patient Studies")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/studies"));
    leftMenu->addItem("MD Referral Counts")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/mdcounts"));
    leftMenu->addItem("MD Visits List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/visits"));
    leftMenu->addItem("MD Issues List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/issues"));
    leftMenu->addItem("Cities List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/localities"));
    leftMenu->addItem("Addresses List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/addresses"));
    leftMenu->addItem("Group List")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/groups"));
    navigation->addMenu(rightMenu, Wt::AlignRight);
    //end nav


    authWidget->processEnvironment();

    mainStack_ = new WStackedWidget();

    navigation->addWidget(authWidget, Wt::AlignRight);
    container->addWidget(authWidget);


    Wt::WContainerWidget *container2 = new Wt::WContainerWidget(links_);
    Wt::WContainerWidget *footer = new Wt::WContainerWidget();
    footer->setStyleClass("custom-footer");

    Wt::WContainerWidget *footerin = new Wt::WContainerWidget();
    footerin->setStyleClass("inside-footer");

    Wt::WText *copyright = new Wt::WText("Copyright 2016 United Sleep Centers");
    copyright->setStyleClass("footer-copyright pull-left");

    Wt::WText *pes7 = new Wt::WText("Powered By <a href='http://pes7.com'>PES7 Healthcare Technology Solutions</a>");
    pes7->setStyleClass("footer-pes7 pull-right");


    footerin->addWidget(copyright);
    footerin->addWidget(pes7);

    footer->addWidget(footerin);

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();

    container2->setLayout(hbox);

    hbox->addWidget(leftMenu);
    hbox->addWidget(mainStack_);

    links_->addWidget(container2);

    links_->hide();

    root()->addWidget(footer);

    WApplication::instance()->internalPathChanged()
        .connect(this, &MarketingApplication::handleInternalPath);

}
void MarketingApplication::authEvent() {

    if (session_.login().loggedIn()){
        if (session_.strUserEmail() == "cody@statsleep.com" 
            || session_.strUserEmail() == "anna@statsleep.com"
            || session_.strUserEmail() == "armen@statsleep.com"
            || session_.strUserEmail() == "niko@statsleep.com"
            || session_.strUserEmail() == "jolene@statsleep.com")
        {
            links_->show();
            handleInternalPath(WApplication::instance()->internalPath());
            Wt::log("notice") << "PCC user logged in.";
        } else {
            mainStack_->clear();
            links_->hide();
            Wt::log("notice") << "ACCESS DENIED";
            this->refresh();
            this->redirect("http://127.0.0.1:9091/pcc");
        }

    }
    else {
        mainStack_->clear();
        links_->hide();
        Wt::log("notice") << "PCC user logged out.";
        this->refresh();
        this->redirect("http://127.0.0.1:9091/pcc");
    }
}

void MarketingApplication::handleInternalPath(const std::string &internalPath)
{
    if (session_.login().loggedIn()) {
        if (internalPath == "/home")
            showHome();
        else if (internalPath == "/referrals")
            showReferrals();
        else if (internalPath == "/studies")
            showStudies();
        else if (internalPath == "/mdcounts")
            showPracticeCount();
        else if (internalPath == "/providers")
            showProviders();
        else if (internalPath == "/patients")
            showPatient();
        else if (internalPath == "/visits")
            showPhysicianVisit();
        else if (internalPath == "/issues")
            showPhysicianIssue();
        else if (internalPath == "/localities")
            showLocality();
        else if (internalPath == "/addresses")
            showAddress();
        else if (internalPath == "/groups")
            showGroup();
        else
            WApplication::instance()->setInternalPath("/home", true);

    }
}

void MarketingApplication::showHome()
{
    mainStack_->clear();
    home_ = new MarketingHomeWidget(mainStack_);
    mainStack_->setCurrentWidget(home_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showReferrals()
{
    mainStack_->clear();
    referral_ = new PccReferralWidget(pccconninfo, &session_, mainStack_);
    referral_->searchList();
    mainStack_->setCurrentWidget(referral_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showStudies()
{
    mainStack_->clear();
    study_ = new PccSleepStudyWidget(pccconninfo, &session_, mainStack_);
    study_->sleepStudyList();
    mainStack_->setCurrentWidget(study_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showProviders()
{
    mainStack_->clear();
    practice_ = new MarketingPracticeWidget(pccconninfo, &session_, mainStack_);
    practice_->practiceList();
    mainStack_->setCurrentWidget(practice_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showPatient()
{
    mainStack_->clear();
    patient_ = new StaffPatientWidget(pccconninfo, &session_, mainStack_);
    patient_->patientList();
    mainStack_->setCurrentWidget(patient_);

    /*
    reportAnchor_->addStyleClass("selected-link");
    inboundAnchor_->removeStyleClass("selected-link");
    homeAnchor_->removeStyleClass("selected-link");
    */
}
void MarketingApplication::showPracticeCount()
{
    mainStack_->clear();
    pracCount_ = new AdminPracticeCountWidget(pccconninfo, &session_, mainStack_);
    pracCount_->showReferralCount();
    mainStack_->setCurrentWidget(pracCount_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showPhysicianVisit()
{
    mainStack_->clear();
    physicianVisit_ = new MarketingPhysicianVisitWidget(pccconninfo, &session_, mainStack_);
    physicianVisit_->visitList();
    mainStack_->setCurrentWidget(physicianVisit_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showPhysicianIssue()
{
    mainStack_->clear();
    physicianIssue_ = new MarketingPhysicianIssueWidget(pccconninfo, &session_, mainStack_);
    physicianIssue_->issueList();
    mainStack_->setCurrentWidget(physicianIssue_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showLocality()
{
    mainStack_->clear();
    localityList_ = new MarketingLocalityListWidget(pccconninfo, &session_, mainStack_);
    localityList_->localityList();
    mainStack_->setCurrentWidget(localityList_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingApplication::showAddress()
{
    mainStack_->clear();
    addressList_ = new PhysicianAddressListWidget(pccconninfo, &session_, mainStack_);
    addressList_->addressList();
    mainStack_->setCurrentWidget(addressList_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}


void MarketingApplication::showGroup()
{
    mainStack_->clear();
    groupList_ = new MarketingPracticeGroupWidget(pccconninfo, &session_, mainStack_);
    groupList_->groupList();
    mainStack_->setCurrentWidget(groupList_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

WApplication *createMarketingApplication(const WEnvironment& env)
{
    return new MarketingApplication(env);
}
