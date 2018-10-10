/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef TECH_STUDY_LIST_WIDGET
#define TECH_STUDY_LIST_WIDGET

#include <Wt/WContainerWidget>
#include <Wt/WText>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WSignal>

#include <Wt/WDate>
#include <Wt/WTime>

#include "StaffSleepStudyWidget.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "UserSession.h"

using namespace Wt;
using namespace Wt::Dbo;

class UserSession;

class TechStudyListWidget : public Wt::WContainerWidget
{
public:
    TechStudyListWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;


    Wt::WPushButton *btnSelectSleepStudy;
    Wt::WPushButton *btnDownloadRef;
    Wt::WPushButton *btnDownloadDny;
    Wt::WPushButton *btnShowSleepStudy;
    Composer *composer_;

    //refresh signal accessors
    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& close() { return close_; }

    Wt::Signal<std::string, std::string>& selectionChanged() { return selectionChanged_; }
    Wt::Signal<Wt::WString>& showStudy() { return showStudy_; }

    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }

    void showSearch(Wt::WString loc);
    void createReferral(std::string& prac, std::string& pt);
    void showSleepStudy(std::string& pt);
    void showStudyDialog(Wt::WString& st);
    void addSleepStudyToBackup(std::string& pt);
    void closeDialog();

private:
    //done signal to refresh app
    Wt::Signal<int, std::string> done_;
    Wt::Signal<int, std::string> close_;
     Wt::Signal<std::string, std::string> selectionChanged_;
     Wt::Signal<Wt::WString> showStudy_;

     //stopTimer signal to refresh app
     Wt::Signal<int, std::string> stopTimer_;
     Wt::WDialog *dialog;
     Wt::WPushButton *accept;
     Wt::WPushButton *cancel;
    UserSession *session_;
    const char *conninfo_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }

    Wt::WText *reviewPracticeName_;
    Wt::WText *reviewPracticeZip_;
    Wt::WText *reviewPracticeDrName_;
    Wt::WText *reviewPracticeNpi_;
    Wt::WText *reviewPracticeEmail_;
    Wt::WText *reviewSpecialty_;

    Wt::WText *reviewFirstName_;
    Wt::WText *reviewLastName_;
    Wt::WText *reviewAddress1_;
    Wt::WText *reviewAddress2_;
    Wt::WText *reviewCity_;
    Wt::WText *reviewZip_;
    Wt::WText *reviewHomePhone_;
    Wt::WText *reviewCellPhone_;
    Wt::WText *reviewEmail_;
    Wt::WText *reviewDob_;
    Wt::WText *reviewFour_;
    Wt::WText *reviewGender_;
    Wt::WText *reviewDiagnostic_;
    Wt::WText *reviewOvernight_;
    Wt::WText *reviewOtherSleepTest_;
    Wt::WText *reviewHomeStudy_;
    Wt::WText *reviewAuthConsult_;
    Wt::WText *pracDrSigName_;
    Wt::WText *reviewRefId_;

    Wt::WText *staticPt_;
    Wt::WText *staticStd_;
    Wt::WText *staticRef_;
    Wt::WText *staticPrac_;

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

    Wt::WLineEdit *searchSleepStudyFirst_;
    Wt::WLineEdit *searchSleepStudyLast_;
    Wt::WLineEdit *searchSleepStudyDob_;

    Wt::WText *newSleepStudyFirstName_;
    Wt::WText *newSleepStudyLastName_;
    Wt::WText *newSleepStudyCity_;;
    Wt::WText *newSleepStudyHomePhone_;
    Wt::WText *newSleepStudyCellPhone_;
    Wt::WText *newSleepStudyEmail_;
    Wt::WText *newSleepStudyDob_;

    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;

    Wt::WText *staticComment_;

    WString setCommentId();

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setPracId();
    Wt::WString strPracId();

    Wt::WString setStdId();
    Wt::WString strStdId();

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString setBuId();


    Wt::WText *staticBu_;
    Wt::WString strBuId();
    Wt::WPushButton *btnAddToBackup_;
};

#endif //TECH_STUDY_LIST_WIDGET
