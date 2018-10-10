/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_HOME_WIDGET_H_
#define STAFF_HOME_WIDGET_H_

#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WTimer>
#include <Wt/WStackedWidget>

#include "InboundContainerWidget.h"
#include "Referral.h"
#include "Composer.h"
#include "UserSession.h"

using namespace Wt;

class UserSession;
class StaffHomeWidget : public Wt::WContainerWidget
{
public:
    StaffHomeWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);
    void showHomeWidget();
    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
private:
	UserSession *session_;
    const char *conninfo_;
    
    Wt::WTimer *refreshTimer; 
    
    ~StaffHomeWidget()
    {
        refreshTimer->stop();
    }
    
    Wt::WStackedWidget *resultStack_;
    InboundContainerWidget *inboundContainer;

    void stopRefreshTimer();
    void startRefreshTimer();
    
    void search(Wt::WString searchString);
    void preSearch(Wt::WString search);
};

#endif //STAFF_HOME_WIDGET_H
