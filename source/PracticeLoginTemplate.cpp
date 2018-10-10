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

#include "PracticeLoginTemplate.h"
#include "PracticeSession.h"
#include "PracticeAuthWidget.h"

using namespace Wt;

PracticeLoginTemplate::PracticeLoginTemplate(PracticeSession *session, WContainerWidget *parent = 0)
    : WTemplate(parent),
    session_(session)
{
    init();
}

void PracticeLoginTemplate::init()
{
    setTemplateText(WString::tr("template.PracticeLogin"));

    brandAnchor_ = new WAnchor();
    brandAnchor_->setLink(WLink(WLink::InternalPath, "/"));
    brandAnchor_->setText("Physician Portal");
    brandAnchor_->addStyleClass("navbar-brand");
    bindWidget("brand", brandAnchor_);

    // Auth model
    authModel_ = new Auth::AuthModel(PracticeSession::auth(),
                           session_->practices(), this);
    authModel_->addPasswordAuth(&PracticeSession::passwordAuth());

    // Auth widget
    authWidget_ = new PracticeAuthWidget(*session_);
    authWidget_->setModel(authModel_);
    authWidget_->setRegistrationEnabled(false);
    authWidget_->processEnvironment();

    bindWidget("auth-widget", authWidget_);

}
