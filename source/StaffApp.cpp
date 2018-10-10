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

#include "StaffApp.h"
#include "StaffLoginTemplate.h"
#include "StaffAppTemplate.h"

const char *staffconninfo = "hostaddr = 127.0.0.1 port = 5432 user = clinicore dbname = clinicore password = Falcon1337";

StaffApplication::StaffApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env),
    session_(staffconninfo)
{
    init();
}

void StaffApplication::init()
{
    session_.login().changed().connect(this, &StaffApplication::authEvent);
    messageResourceBundle().use(appRoot() + "templates");
    messageResourceBundle().use(appRoot() + "composer");
    messageResourceBundle().use(appRoot() + "strings");

    bootstrap_ = new Wt::WBootstrapTheme();
    bootstrap_->setResponsive(true);
    bootstrap_->setVersion(WBootstrapTheme::Version::Version3);
    bootstrap_->setFormControlStyleEnabled(true);
    setTheme(bootstrap_);

    require("js/jquery.js");
    require("js/sketch.js");

    useStyleSheet("css/style.css");
    useStyleSheet("css/composer.css");

    loginStack_ = new WStackedWidget(root());
    loginStack_->setStyleClass("loginStack");
    template_ = new StaffAppTemplate(staffconninfo, &session_, loginStack_);
    login_ = new StaffLoginTemplate(&session_, loginStack_);
    authEvent();
    //setOverflow(OverflowHidden);

    // Wt::WContainerWidget *container = new Wt::WContainerWidget();
    // container->setStyleClass("main-conatiner");

    // Wt::WImage *image = new Wt::WImage(Wt::WLink("images/logo.png"),
    //     container);
    // image->setAlternateText("usc logo");
    // image->setStyleClass("usc-logo");
    // image->show();

    // Wt::WContainerWidget *header = new Wt::WContainerWidget();
    // header->setStyleClass("custom-header");

    // Wt::WContainerWidget *headerin = new Wt::WContainerWidget();
    // headerin->setStyleClass("inside-header");

    // Wt::WText *unitedphone = new Wt::WText("Questions? Call our office at 562 622 1002");
    // unitedphone->setStyleClass("header-unitedphone");


    // headerin->addWidget(unitedphone);

    // header->addWidget(headerin);


    // root()->addWidget(header);

    // root()->addWidget(container);

    // Wt::Auth::AuthWidget *authWidget
    //     = new PracticeAuthWidget(session_, container);

    // authWidget->model()->addPasswordAuth(&PracticeSession::passwordAuth());
    // authWidget->setRegistrationEnabled(true);

    // links_ = new WContainerWidget();

    // links_->setStyleClass("links");
    // root()->addWidget(links_);

    // /*
    // newReferralAnchor_ = new WAnchor("/newreferral", "New Sleep Study Referral", links_);
    // newReferralAnchor_->setLink(WLink(WLink::InternalPath, "/newreferral"));

    // homeAnchor_ = new WAnchor("/home", "Practice Home", links_);
    // homeAnchor_->setLink(WLink(WLink::InternalPath, "/home"));
    // */

    // Wt::WNavigationBar *navigation = new Wt::WNavigationBar(links_);
    // navigation->setTitle("Physician Portal",
    //     "https://statsleep.com");
    // navigation->setResponsive(true);

    // Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();

    // //contentsStack->addStyleClass("contents");

    // // Setup a Left-aligned menu.
    // Wt::WMenu *leftMenu = new Wt::WMenu(Wt::Vertical, links_);
    // //navigation->addMenu(leftMenu);

    // leftMenu->setStyleClass("nav nav-pills nav-stacked");
    // leftMenu->setWidth(200);

    // leftMenu->addItem("Home")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/home"));
    // leftMenu->addItem("Upload Order Forms")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/sendorder"));
    // leftMenu->addItem("New Online Referral")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/newreferral"));
    // leftMenu->addItem("Request a Patient Report")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/report"));
    // leftMenu->addItem("Study Progress Tracking")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/recent"));

    // Wt::WMenu *rightMenu = new Wt::WMenu();
    // navigation->addMenu(rightMenu, Wt::AlignRight);

    // rightMenu->addItem("Settings")
    //     ->setLink(Wt::WLink(Wt::WLink::InternalPath, "/settings"));

    // authWidget->processEnvironment();

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

}

void StaffApplication::authEvent() {

    if (session_.login().loggedIn()){
        Wt::log("notice") << "Staff user logged in.";
        if (session_.strUserEmail() == "cody@statsleep.com" 
            || session_.strUserEmail() == "anna@statsleep.com"
            || session_.strUserEmail() == "vmadueno@statsleep.com" 
            || session_.strUserEmail() == "vanessa.madueno@statsleep.com " 
            || session_.strUserEmail() == "stephanie@statsleep.com" 
            || session_.strUserEmail() == "saquino@statsleep.com"
            || session_.strUserEmail() == "armen@statsleep.com"
            || session_.strUserEmail() == "niko@statsleep.com"
            || session_.strUserEmail() == "jolene@statsleep.com")
        {
            loginStack_->setCurrentWidget(template_);
            Wt::log("notice") << "Staff user logged in.";
        } else {
            loginStack_->setCurrentWidget(login_);
            Wt::log("notice") << "ACCESS DENIED.";
        }
    }
    else {
        loginStack_->setCurrentWidget(login_);
    }
}


WApplication *createStaffApplication(const WEnvironment& env)
{
    return new StaffApplication(env);
}
