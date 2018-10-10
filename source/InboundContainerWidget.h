/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INBOUND_CONTAINER_WIDGET_H_
#define INBOUND_CONTAINER_WIDGET_H_
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
#include <Wt/WPushButton>

#include "Patient.h"
#include "Composer.h"
#include "UserSession.h"

using namespace Wt;

class UserSession;
class InboundContainerWidget : public Wt::WContainerWidget
{
public:
    InboundContainerWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;


    Wt::WPushButton *btnShowRef;
    Wt::WPushButton *btnShowSchedRef;
    Wt::WPushButton *btnReceive;

    void showSearch(Wt::WString& curstr, int limit, int offset);
    void showSearchInbound(Wt::WString& curstr, int limit, int offset);
    //signal accessors
    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();
        return boost::lexical_cast<std::string>(uuidx);
    }
    UserSession *session_;
    const char *conninfo_;

private:
    
    //done signal to refresh app
    Wt::Signal<int, std::string> done_;

     //stopTimer signal to refresh app
    Wt::Signal<int, std::string> stopTimer_;

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


    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    Wt::WString newStudyTypeName_;
    Wt::WString newStudyTypeId_;
    Wt::WString newStudyTypeCode_;

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setStdId();
    Wt::WString strStdId();
    Wt::WString setCommentId();

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

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString strT(WTemplate& tem);


    Wt::WText *orderSent_;
    Wt::WPushButton *btnDownloadRef;
    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;
    Wt::WString *refInsuranceId_;
    Wt::WString *refInsuranceName_;

    Wt::WString studyInsuranceName_;
    Wt::WString studyInsuranceId_;

    void downloadReferral(WString& ref);
    void showReferralDialog(WString& ref);
    void receiveReferral(WString& ref);
    void createPatient(WString& ref);

};

#endif //INBOUND_CONTAINER_WIDGET_H_
