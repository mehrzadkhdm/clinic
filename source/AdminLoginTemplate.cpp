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

#include "AdminLoginTemplate.h"
#include "UserSession.h"

using namespace Wt;

AdminLoginTemplate::AdminLoginTemplate(UserSession *session, WContainerWidget *parent = 0)
    : WTemplate(parent),
    session_(session)
{
    init();
}

void AdminLoginTemplate::init()
{
    setTemplateText(WString::tr("template.PracticeLogin"));

    brandAnchor_ = new WAnchor();
    brandAnchor_->setLink(WLink(WLink::InternalPath, "/"));
    brandAnchor_->setText("Admin Portal");
    brandAnchor_->addStyleClass("navbar-brand");
    bindWidget("brand", brandAnchor_);

    // Auth widget
    authWidget_ = new Wt::Auth::AuthWidget(UserSession::auth(), session_->users(), session_->login());
    authWidget_->model()->addPasswordAuth(&UserSession::passwordAuth());

    authWidget_->setRegistrationEnabled(false);

    authWidget_->processEnvironment();

    bindWidget("auth-widget", authWidget_);

}
