/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_LOGIN_TEMPLATE_H_
#define ADMIN_LOGIN_TEMPLATE_H_

#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "UserSession.h"

using namespace Wt;

class AdminLoginTemplate : public WTemplate
{

public:
    AdminLoginTemplate(UserSession *session, WContainerWidget *parent);

private:
    void init();
    
    UserSession *session_;
    WAnchor *brandAnchor_;
    Auth::AuthWidget *authWidget_;
    Auth::AuthModel *authModel_;

};

#endif //ADMIN_LOGIN_TEMPLATE_H_