/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_SETTINGS_WIDGET_H_
#define PRACTICE_SETTINGS_WIDGET_H_

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
#include <Wt/WString>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include "PracticeSession.h"

using namespace Wt;
using namespace Wt::Dbo;
class PracticeSession;

class PracticeSettingsWidget : public Wt::WContainerWidget
{

public:
    PracticeSettingsWidget(const char *conninfo, Wt::Auth::AuthModel *authModel, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
private:


    PracticeSession *session_;
    Wt::Auth::AuthModel *authModel_;

public:

    void demographics();
    void resetPassword();

};

#endif //PRACTICE_SETTINGS_WIDGET_H
