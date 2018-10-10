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
#include "MarketingReferralWidget.h"
#include "AdminPracticeCountWidget.h"
#include "MarketingPracticeWidget.h"
#include "MarketingVisitReportWidget.h"
#include "MarketingPhysicianIssueWidget.h"
#include "AdminReferralWidget.h"
#include "MarketingAdminApp.h"

const char *pccadminconninfo = "hostaddr = 127.0.0.1 port = 5432 user = clinicore dbname = clinicore password = Falcon1337";

MarketingAdminApplication::MarketingAdminApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env),
    session_(pccadminconninfo)
{
    session_.login().changed().connect(this, &MarketingAdminApplication::authEvent);

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
    leftMenu->addItem("Referral Report")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/referral-report"));
    leftMenu->addItem("PCC Visits Report")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/visit-report"));
    leftMenu->addItem("MDs Added Report")
        ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/physician-report"));

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
        .connect(this, &MarketingAdminApplication::handleInternalPath);

}
void MarketingAdminApplication::authEvent() {

    if (session_.login().loggedIn()){
        if (session_.strUserEmail() == "cody@statsleep.com" 
            || session_.strUserEmail() == "anna@statsleep.com"
            || session_.strUserEmail() == "niko@statsleep.com"
            || session_.strUserEmail() == "jolene@statsleep.com")
        {
            links_->show();
            handleInternalPath(WApplication::instance()->internalPath());
            Wt::log("notice") << "PCC Admin user logged in.";
        } else {
            mainStack_->clear();
            links_->hide();
            Wt::log("notice") << "ACCESS DENIED.";
            this->refresh();
            this->redirect("http://127.0.0.1:9091/pcc-admin");

        }


    }
    else {
        mainStack_->clear();
        links_->hide();
        Wt::log("notice") << "PCC Admin user logged out.";
        this->refresh();
        this->redirect("http://127.0.0.1:9091/pcc-admin");
    }
}

void MarketingAdminApplication::handleInternalPath(const std::string &internalPath)
{
    if (session_.login().loggedIn()) {
        if (internalPath == "/home")
            showHome();
        else if (internalPath == "/referral-report")
            showReferralReport();
        else if (internalPath == "/visit-report")
            showVisitReport();
        else if (internalPath == "/physician-report")
            showPhysicianReport();

        else
            WApplication::instance()->setInternalPath("/home", true);

    }
}

void MarketingAdminApplication::showHome()
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


void MarketingAdminApplication::showReferralReport()
{
    mainStack_->clear();
    referralReport_ = new AdminReferralWidget(pccadminconninfo, &session_, mainStack_);
    referralReport_->searchList();
    mainStack_->setCurrentWidget(referralReport_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingAdminApplication::showVisitReport()
{
    mainStack_->clear();
    visitReport_ = new MarketingVisitReportWidget(pccadminconninfo, &session_, mainStack_);
    visitReport_->newVisitReport();
    mainStack_->setCurrentWidget(visitReport_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}

void MarketingAdminApplication::showPhysicianReport()
{
    mainStack_->clear();
    physicianReport_ = new MarketingPhysicianReportWidget(pccadminconninfo, &session_, mainStack_);
    physicianReport_->newVisitReport();
    mainStack_->setCurrentWidget(physicianReport_);
    /*
    inboundAnchor_->removeStyleClass("selected-link");
    reportAnchor_->removeStyleClass("selected-link");
    homeAnchor_->addStyleClass("selected-link");
    */
}


WApplication *createMarketingAdminApplication(const WEnvironment& env)
{
    return new MarketingAdminApplication(env);
}
