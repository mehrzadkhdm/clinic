/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_PATIENT_WIDGET_H_
#define STAFF_PATIENT_WIDGET_H_

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
#include <Wt/WTimer>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include "PhysicianRecordListContainerWidget.h"
#include "PatientListContainerWidget.h"
#include "UserSession.h"

using namespace Wt;

class UserSession;

class StaffPatientWidget : public Wt::WContainerWidget
{
public:
    StaffPatientWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;

    void patientList();

private:

    UserSession *session_;
    const char *conninfo_;

    PhysicianRecordListContainerWidget *physicianContainer;

    Wt::WTimer *refreshTimer;
    void preSearch(Wt::WString searchString);
    void stopRefreshTimer();
    void startRefreshTimer();
    ~StaffPatientWidget()
    {
        refreshTimer->stop();
    }
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }

    Wt::WLineEdit *dobMonth;
    Wt::WLineEdit *dobDay;
    Wt::WLineEdit *dobYear;

    Wt::WString studyInsuranceId_;
    Wt::WString studyInsuranceName_;
    Wt::WDialog *newstdialog;
    Wt::WText *staticPt_;
    Wt::WText *staticStd_;
    Wt::WText *staticPrac_;

    Wt::WLineEdit *pracName_;
    Wt::WLineEdit *pracDrFirstName_;
    Wt::WLineEdit *pracDrLastName_;
    Wt::WLineEdit *pracZip_;
    Wt::WLineEdit *pracNpi_;
    Wt::WLineEdit *pracFax_;
    Wt::WLineEdit *pracSpecialty_;
    Wt::WLineEdit *pracEmail_;

    Wt::WString setPtId();
    Wt::WString setPracId();
    Wt::WString strPtId();

    Wt::WText *staticBu_;
    Wt::WString setBuId();
    Wt::WString strBuId();

    Wt::WStackedWidget *resultStack_;
    Wt::WStackedWidget *pracResultStack_;
    PatientListContainerWidget *ptContainer;

    Wt::WPushButton *btnViewPt_;
    Wt::WPushButton *btnSelectPractice;
    Wt::WPushButton *btnNewPt_;
    Wt::WPushButton *btnAddToBackup_;

    void search(Wt::WString searchString);
    void showPatientDialog(std::string& pt);
    void addPatientDialog();
    void selectPracticeDialog(std::string pt);
    void newReferralAddProvider(std::string pt);
    void addPatientToBackup(std::string& pt);
    void addPracPt(std::string prac, std::string pt);
    void addPracList(Wt::WString asid);
    void backToPrac(std::string prac, std::string pt);

    Wt::WLineEdit *patientFirstName_;
    Wt::WLineEdit *patientLastName_;
    Wt::WLineEdit *patientCity_;
    Wt::WLineEdit *patientHomePhone_;
    Wt::WLineEdit *patientCellPhone_;
    Wt::WLineEdit *patientEmail_;
    Wt::WLineEdit *patientDob_;

};

#endif //STAFF_PATIENT_WIDGET_H
