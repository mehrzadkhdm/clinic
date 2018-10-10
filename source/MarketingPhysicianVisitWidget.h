/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef MARKETING_PHYSICIANVISIT_WIDGET_H_
#define MARKETING_PHYSICIANVISIT_WIDGET_H_
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
#include "PhysicianVisitResultWidget.h"
using namespace Wt;

class UserSession;

class MarketingPhysicianVisitWidget : public Wt::WContainerWidget
{
public:
	MarketingPhysicianVisitWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

	Wt::Dbo::Session dbsession;

	Wt::Dbo::backend::Postgres pg_;

	void selectPracticeDialog();
	void newVisit(Wt::WString prac);
	void visitList();

private:
	Wt::WString uuid() {
		boost::uuids::uuid uuidx = boost::uuids::random_generator()();

		return boost::lexical_cast<std::string>(uuidx);

	}
	~MarketingPhysicianVisitWidget()
	{
		refreshTimer->stop();
	}

	UserSession *session_;
	const char *conninfo_;

	PhysicianVisitResultWidget *prContainer;
	Wt::WStackedWidget *resultStack_;

	Wt::WTimer *refreshTimer;

private:

	Wt::WLineEdit *visitReason_;
	Wt::WLineEdit *visitOutcome_;
	Wt::WLineEdit *visitOfficeManager_;

	Wt::WLineEdit *issueTitle_;
	Wt::WLineEdit *issueDescription_;
	Wt::WLineEdit *issueActionTaken_;

	Wt::WString setVisitId();
	Wt::WString setIssueId();

	Wt::WText *staticVisit_;
	Wt::WText *staticIssue_;
	Wt::WPushButton *btnSelectPractice;

	void stopRefreshTimer();
	void startRefreshTimer();

	void preSearch(Wt::WString search);
	void search(Wt::WString searchString);
	void ptsearch(Wt::WString searchString);
	void newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid="");

};

#endif //MARKETING_PHYSICIANVISIT_WIDGET_H
