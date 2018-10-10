/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_REFERRAL_VIEW_H_
#define PRACTICE_REFERRAL_VIEW_H_

#include <Wt/WTemplateFormView>
#include <Wt/WButtonGroup>
#include "PracticeReferralModel.h"
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

class PracticeSession;
class PracticeReferralModel;

class PracticeReferralView : public Wt::WTemplateFormView
{

public:
    PracticeReferralView(Wt::WContainerWidget *parent);

    enum GenderStates { GENDER_ERROR, GENDER_MALE, GENDER_FEMALE, GENDER_OTHER };

    std::string strFirstName();
    std::string strLastName();
    std::string strAddress1();
    std::string strAddress2();
    std::string strCity();
    std::string strZip();
    std::string strHomePhone();
    std::string strCellPhone();
    std::string strDob();
    std::string strGender();
    std::string strDiagnostic();
    std::string strOvernight();
    std::string strOtherSleepTest();
    bool boolHomeStudy();
    bool boolAuthConsult();

    Wt::Signal<int>& done() { return done_; }

private:
    Wt::Signal<int> done_;
    PracticeReferralModel *model_;
    Wt::WButtonGroup *group_;

    void process();
};

#endif //PRACTICE_REFERRAL_VIEW_H_
