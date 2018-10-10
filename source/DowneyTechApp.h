/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef DNYTECHAPP_H_
#define DNYTECHAPP_H_

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include "TechHomeWidget.h"
#include "TechSleepStudyWidget.h"
#include "TechBackupListWidget.h"
#include "TechShiftNoteWidget.h"
#include "UserSession.h"

using namespace Wt;

class DowneyTechApplication : public Wt::WApplication
{

public:
    DowneyTechApplication(const Wt::WEnvironment& env);
    void handleInternalPath(const std::string &internalPath);


private:
    Wt::WStackedWidget *mainStack_;
    void authEvent();
    void showHome();
    void showTonight();
    void showBackup();
    void showShiftNote();

    TechHomeWidget *home_;
    TechSleepStudyWidget *tonight_;
    TechBackupListWidget *backup_;
    TechShiftNoteWidget *shiftnote_;

    Wt::WContainerWidget *links_;
    UserSession session_;

};

WApplication *createDowneyTechApplication(const WEnvironment& env);

#endif //DNYTECHAPP_H_
