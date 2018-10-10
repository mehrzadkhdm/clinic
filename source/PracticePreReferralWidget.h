/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_PRE_REFERRAL_WIDGET
#define PRACTICE_PRE_REFERRAL_WIDGET

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WPushButton>

#include <Wt/WDate>
#include <Wt/WTime>

#include "Patient.h"
#include "PracticeReferralWidget.h"
#include "PracticeSession.h"
using namespace Wt;
using namespace Wt::Dbo;

class PracticeSession;
class PracticePreReferralWidget : public Wt::WContainerWidget
{
public:
    PracticePreReferralWidget(const char *conninfo, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    void physicianList();

private:

    PracticeSession *session_;
    const char *conninfo_;
    Wt::WPushButton *selectMd;
    void startReferral(Wt::WString prac);
    WStackedWidget *mainStack_;
    PracticeReferralWidget *referral_;

};

#endif //PRACTICE_PRE_REFERRAL_WIDGET
