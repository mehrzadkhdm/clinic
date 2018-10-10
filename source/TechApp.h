/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef TECHAPP_H_
#define TECHAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "TechHomeWidget.h"
#include "TechSleepStudyWidget.h"
#include "TechBackupListWidget.h"
#include "TechShiftNoteWidget.h"
#include "StaffPatientWidget.h"
#include "UserSession.h"

using namespace Wt;

class TechApplication : public Wt::WApplication
{

public:
    TechApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    Wt::WStackedWidget *mainStack_;
    void authEvent();
    void showHome();
    void showTonight();
    void showBackup();
    void showShiftNote();
    void showPatient();

    TechHomeWidget *home_;
    TechSleepStudyWidget *tonight_;
    TechBackupListWidget *backup_;
    TechShiftNoteWidget *shiftnote_;
    StaffPatientWidget *patient_;

    Wt::WContainerWidget *links_;
    UserSession session_;

};

WApplication *createTechApplication(const WEnvironment& env);

#endif //TECHAPP_H_
