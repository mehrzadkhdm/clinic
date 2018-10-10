/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_REFERRAL_WIDGET_H_
#define PRACTICE_REFERRAL_WIDGET_H_

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
#include "PracticeReferralView.h"
#include "Composer.h"

using namespace Wt;
using namespace Wt::Dbo;
class PracticeSession;

class PracticeReferralWidget : public Wt::WContainerWidget
{

public:
    PracticeReferralWidget(const char *conninfo, Wt::WString prac, PracticeSession *session, Wt::WContainerWidget *parent = 0);


    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
    //Wt::Dbo::backend::Postgres postgres_;

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
    PracticeReferralView *form_;
    Wt::WDialog *dialog_;
    PracticeSession *session_;
    const char *conninfo_;
    Composer *composer_;
    Wt::WString prac_;
    Wt::WString groupPracticeId_;

public:
    void start();
    void demographics();
    void diagnosis();
    void study();
    void complete();
    void review();
    void sendOrder();
    void saveOrder();
    void showDialog();
    void showVerifyDialog();

    Wt::WText *staticRef_;
    Wt::WText *reviewFirstName_;
    Wt::WText *dbFirstName_;
    Wt::WText *reviewLastName_;
    Wt::WText *dbLastName_;
    Wt::WText *reviewAddress1_;
    Wt::WText *dbAddress1_;
    Wt::WText *reviewAddress2_;
    Wt::WText *dbAddress2_;
    Wt::WText *reviewCity_;
    Wt::WText *dbCity_;
    Wt::WText *reviewZip_;
    Wt::WText *dbZip_;
    Wt::WText *reviewHomePhone_;
    Wt::WText *dbHomePhone_;
    Wt::WText *reviewCellPhone_;
    Wt::WText *dbCellPhone_;
    Wt::WText *reviewEmail_;
    Wt::WText *dbEmail_;
    Wt::WText *reviewDob_;
    Wt::WText *dbDob_;

    Wt::WText *reviewGender_;
    Wt::WText *dbGender_;
    Wt::WText *reviewDiagnostic_;
    Wt::WText *dbDiagnostic_;
    Wt::WText *reviewOvernight_;
    Wt::WText *dbOvernight_;
    Wt::WText *reviewOtherSleepTest_;
    Wt::WText *dbOtherTest_;
    Wt::WText *reviewHomeStudy_;
    Wt::WText *dbHomeStudy_;
    Wt::WText *reviewAuthConsult_;
    Wt::WText *dbAuthConsult_;
    Wt::WText *pracDrSigName_;
    Wt::WText *reviewRefId_;

    Wt::WText *outFirstName_;
    Wt::WText *outLastName_;
    Wt::WText *outAddress1_;
    Wt::WText *outAddress2_;
    Wt::WText *outCity_;
    Wt::WText *outZip_;
    Wt::WText *outHomePhone_;
    Wt::WText *outCellPhone_;
    Wt::WText *outEmail_;
    Wt::WText *outDob_;
    Wt::WText *outFour_;
    Wt::WText *outGender_;
    Wt::WText *outDiagnostic_;
    Wt::WText *outOvernight_;
    Wt::WText *outOtherSleepTest_;
    Wt::WText *outHomeStudy_;
    Wt::WText *outAuthConsult_;
    Wt::WText *outRefId_;

    Wt::WText *outSigName_;
    Wt::WText *outSigEmail_;
    Wt::WText *outSigDate_;
    Wt::WText *dbSigName_;
    Wt::WText *dbSigEmail_;
    Wt::WText *dbSigDate_;

    std::string strFirstName();
    std::string strLastName();
    std::string strAddress1();
    std::string strAddress2();
    std::string strCity();
    std::string strZip();
    std::string strHomePhone();
    std::string strCellPhone();
    std::string strEmail();
    std::string strDob();
    std::string strFour();
    std::string strGender();
    std::string strDiagnostic();
    std::string strOvernight();
    std::string strOtherSleepTest();
    std::string strHomeStudy();
    std::string strAuthConsult();

    std::string strEnteredName();
    std::string strEnteredEmail();
    std::string strEsigDate();

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WText *orderSent_;
    void oldCompleteClickCode(int foo);

};

#endif //PRACTICE_REFERRAL_WIDGET_H_
