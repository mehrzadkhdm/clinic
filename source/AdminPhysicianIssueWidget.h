/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_PHYSICIANISSUE_WIDGET_H_
#define ADMIN_PHYSICIANISSUE_WIDGET_H_
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
#include <Wt/WStackedWidget>
#include <Wt/WTimer>

#include "UserSession.h"
#include "PhysicianIssueResultWidget.h"
using namespace Wt;

class UserSession;

class AdminPhysicianIssueWidget : public Wt::WContainerWidget
{
public:
    AdminPhysicianIssueWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;

    void issueList();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }
    ~AdminPhysicianIssueWidget()
    {
        refreshTimer->stop();
    }

    UserSession *session_;
    const char *conninfo_;

    PhysicianIssueResultWidget *prContainer;
    Wt::WStackedWidget *resultStack_;

    Wt::WTimer *refreshTimer;

private:

    void stopRefreshTimer();
    void startRefreshTimer();

    void preSearch(Wt::WString search);
    void search(Wt::WString searchString);
    void ptsearch(Wt::WString searchString);
    void newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid="");

};

#endif //ADMIN_PHYSICIANVISSUE_WIDGET_H
