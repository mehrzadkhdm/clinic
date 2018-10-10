    /*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMINAPP_H_
#define ADMINAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>


#include "AdminLoginTemplate.h"
#include "AdminAppTemplate.h"
#include "UserSession.h"

using namespace Wt;

class AdminApplication : public Wt::WApplication
{

public:
    AdminApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    void authEvent();
    void init();
    
    WStackedWidget *loginStack_;
    WBootstrapTheme *bootstrap_;
    AdminAppTemplate *template_;
    AdminLoginTemplate *login_;

    Wt::WContainerWidget *links_;
    UserSession session_;



};

WApplication *createAdminApplication(const WEnvironment& env);

#endif //ADMINAPP_H_
