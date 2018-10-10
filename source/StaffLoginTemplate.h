/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_LOGIN_TEMPLATE_H_
#define STAFF_LOGIN_TEMPLATE_H_

#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WString>
#include <Wt/Auth/AuthWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "UserSession.h"

using namespace Wt;

class StaffLoginTemplate : public WTemplate
{

public:
    StaffLoginTemplate(UserSession *session, WContainerWidget *parent);

private:
    void init();
    
    UserSession *session_;
    WAnchor *brandAnchor_;
    Auth::AuthWidget *authWidget_;
    Auth::AuthModel *authModel_;

};

#endif //STAFF_LOGIN_TEMPLATE_H_