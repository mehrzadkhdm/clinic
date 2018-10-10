/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_PRACTICECOUNT_WIDGET_H_
#define ADMIN_PRACTICECOUNT_WIDGET_H_
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
#include <Wt/WProgressBar>
#include <Wt/WString>
#include <Wt/WStackedWidget>
#include "InboundContainerWidget.h"
#include "PatientListContainerWidget.h"
#include "ReferralResultWidget.h"
#include "UserSession.h"
#include "Composer.h"


using namespace Wt;

class UserSession;
class AdminPracticeCountWidget : public Wt::WContainerWidget
{
public:
    AdminPracticeCountWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;

    void showReferralCount();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
	Wt::WContainerWidget *container;
	Wt::WContainerWidget *initcontainer;

    UserSession *session_;

	Wt::WProgressBar *bar;
	Wt::WPushButton *startButton;
	Wt::WText *running;
	void disableStart();
	void runIt();
	void preRun();
	void allDone();

};

#endif //ADMIN_PRACTICECOUNT_WIDGET_H_
