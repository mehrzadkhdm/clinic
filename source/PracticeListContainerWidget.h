/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_LIST_CONTAINER_WIDGET
#define PRACTICE_LIST_CONTAINER_WIDGET

#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WText>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WPushButton>

#include <Wt/WDate>
#include <Wt/WTime>

#include "UserSession.h"
#include "PracticeSession.h"
#include "PhysicianAddressListContainerWidget.h"
#include "PhysicianRecordListContainerWidget.h"
using namespace Wt;
using namespace Wt::Dbo;

class UserSession;

class PracticeListContainerWidget : public Wt::WContainerWidget
{
public:
    PracticeListContainerWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;


    Wt::WPushButton *btnSelectPractice;
    Wt::WPushButton *btnCreateAccount;
    Wt::WPushButton *btnShowVisit;
    Wt::WPushButton *btnShowIssue;

    //signal accessors
    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }

    void showSearch(Wt::WString& curstr, int limit, int offset);
    void showPracticeDialog(Wt::WString prac);
    void showCreateAccount(Wt::WString prac);
    void createPhysicianLogin(Wt::WString prac);
    void showPhysicianVisitComplete(Wt::WString vid);
    void newPhysicianVisitStart(Wt::WString prac);
    void showCompletedVisit(Wt::WString vid);
    void addPhysicianAddress(Wt::WString aid);
    void mergePhysicianRecords(Wt::WString aid);

    Wt::WText *globalPracticeId;
private:

    //done signal to refresh app
    Wt::Signal<int, std::string> done_;

     //stopTimer signal to refresh app
    Wt::Signal<int, std::string> stopTimer_;

    PhysicianAddressListContainerWidget *insContainer;
    PhysicianRecordListContainerWidget *mergeContainer;

    UserSession *session_;
    PracticeSession psession_;
    PracticeSession *pracsession_;

    const char *conninfo_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }
    void showPhysicianVisitDialog(Wt::WString vid);
    void newPhysicianVisitDialog(Wt::WString prac);
    void showPhysicianIssueDialog(Wt::WString isid);
    void newPhysicianIssueDialog(Wt::WString prac, Wt::WString vid="");


    Wt::WText *reviewPracticeName_;
    Wt::WText *reviewPracticeZip_;
    Wt::WText *reviewPracticeDrName_;
    Wt::WText *reviewPracticeNpi_;
    Wt::WText *reviewPracticeEmail_;
    Wt::WText *reviewSpecialty_;

    Wt::WText *staticPt_;
    Wt::WText *staticStd_;
    Wt::WText *staticRef_;
    Wt::WText *staticPrac_;
    Wt::WText *staticVisit_;

    Wt::WLineEdit *SleepStudyFirstName_;
    Wt::WLineEdit *SleepStudyLastName_;
    Wt::WLineEdit *SleepStudyCity_;
    Wt::WLineEdit *SleepStudyHomePhone_;
    Wt::WLineEdit *SleepStudyCellPhone_;
    Wt::WLineEdit *SleepStudyEmail_;
    Wt::WLineEdit *SleepStudyDob_;

    Wt::WLineEdit *pracName_;
    Wt::WLineEdit *pracDrFirstName_;
    Wt::WLineEdit *pracDrLastName_;
    Wt::WLineEdit *pracZip_;
    Wt::WLineEdit *pracNpi_;
    Wt::WLineEdit *pracSpecialty_;
    Wt::WLineEdit *pracEmail_;
    Wt::WLineEdit *pracFax_;

    Wt::WLineEdit *visitReason_;
    Wt::WLineEdit *visitOutcome_;
    Wt::WLineEdit *visitOfficeManager_;

    Wt::WLineEdit *issueTitle_;
    Wt::WLineEdit *issueDescription_;
    Wt::WLineEdit *issueActionTaken_;

    Wt::WLineEdit *searchSleepStudyFirst_;
    Wt::WLineEdit *searchSleepStudyLast_;
    Wt::WLineEdit *searchSleepStudyDob_;

    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;

    Wt::WDialog *pracdialog;

    Wt::WText *staticComment_;
    Wt::WText *staticIssue_;

    WString setCommentId();

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setPracId();
    Wt::WString strPracId();

    Wt::WString setStdId();
    Wt::WString strStdId();

    Wt::WString setVisitId();
    Wt::WString setIssueId();

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString setBuId();

};

#endif //PRACTICE_LIST_CONTAINER_WIDGET
