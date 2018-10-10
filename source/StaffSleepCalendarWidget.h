/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_SLEEPCALENDAR_WIDGET_H_
#define STAFF_SLEEPCALENDAR_WIDGET_H_
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
#include <Wt/WSignal>
#include <Wt/WTimer>

#include "UserSession.h"
#include "Composer.h"
#include "CalendarContainerWidget.h"

using namespace Wt;

class UserSession;

class StaffSleepCalendarWidget : public Wt::WContainerWidget
{
public:
    StaffSleepCalendarWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    void studyList();
    void monthView();
    void hello();
    void triggerCalendar(Wt::WDate calendarDate);
    void preTriggerCalendar(Wt::WDate calendarDate);
    void stopRefreshTimer();
    void preTriggerStartTimer(Wt::WDate calendarDate);
    Wt::WStackedWidget *calStack_;
private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }

    ~StaffSleepCalendarWidget()
    {
        refreshTimer->stop();
    }

    Composer *composer_;
    UserSession *session_;
    const char *conninfo_;
    CalendarContainerWidget *calendarContainer;

    Wt::WTimer *refreshTimer;

private:

    int curSelectedMonth;
    int curSelectedYear;

    Wt::WDate currentlySelectedDate_;
    Wt::WString currentlySelectedLocation_;
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

    Wt::WLineEdit *patientFirstName_;
    Wt::WLineEdit *patientLastName_;
    Wt::WLineEdit *patientCity_;
    Wt::WLineEdit *patientHomePhone_;
    Wt::WLineEdit *patientCellPhone_;
    Wt::WLineEdit *patientEmail_;
    Wt::WLineEdit *patientDob_;

    Wt::WLineEdit *pracName_;
    Wt::WLineEdit *pracDrFirstName_;
    Wt::WLineEdit *pracDrLastName_;
    Wt::WLineEdit *pracZip_;
    Wt::WLineEdit *pracNpi_;
    Wt::WLineEdit *pracSpecialty_;
    Wt::WLineEdit *pracEmail_;

    Wt::WLineEdit *searchPatientFirst_;
    Wt::WLineEdit *searchPatientLast_;
    Wt::WLineEdit *searchPatientDob_;

    Wt::WText *newpatientFirstName_;
    Wt::WText *newpatientLastName_;
    Wt::WText *newpatientCity_;;
    Wt::WText *newpatientHomePhone_;
    Wt::WText *newpatientCellPhone_;
    Wt::WText *newpatientEmail_;
    Wt::WText *newpatientDob_;

    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setPracId();
    Wt::WString strPracId();

    Wt::WString setStdId();
    Wt::WString strStdId();

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString strPracticeName();
    Wt::WString strPracticeZip();
    Wt::WString strPracticeDrName();
    Wt::WString strPracticeNpi();
    Wt::WString strPracticeEmail();
    Wt::WString strSpecialty();
    Wt::WString strFirstName();
    Wt::WString strLastName();
    Wt::WString strAddress1();
    Wt::WString strAddress2();
    Wt::WString strCity();
    Wt::WString strZip();
    Wt::WString strHomePhone();
    Wt::WString strCellPhone();
    Wt::WString strEmail();
    Wt::WString strDob();
    Wt::WString strFour();
    Wt::WString strGender();
    Wt::WString strDiagnostic();
    Wt::WString strOvernight();
    Wt::WString strOtherSleepTest();
    Wt::WString strHomeStudy();
    Wt::WString strAuthConsult();

    Wt::WString strT(WTemplate& tem);

    Wt::WText *staticComment_;
    Wt::WString commentTextOut_;

    WString setCommentId();

    Wt::WText *orderSent_;
    Wt::WPushButton *btnDownloadRef;
    Wt::WPushButton *btnDownloadAtt;
    Wt::WPushButton *btnSelectPatient;
    Wt::WPushButton *btnSelectPractice;
    Wt::WText *downDone;

    void downloadReferral(WString& ref);
    void showStudyDialog(WString& ref);
    void selectPracticeDialog(std::string& pt);
    void newStudyAddPatient();
    void newStudyAddProvider();
    void createReferral(std::string& prac, std::string& pt);
    void addPatientToBackup(std::string &st);
};

#endif //STAFF_SLEEPCALENDAR_WIDGET_H
