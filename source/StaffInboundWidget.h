/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_INBOUND_WIDGET_H_
#define STAFF_INBOUND_WIDGET_H_
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
#include <Wt/WTimer>
#include "InboundContainerWidget.h"
#include "UserSession.h"

using namespace Wt;

class UserSession;
class StaffInboundWidget : public Wt::WContainerWidget
{
public:
    StaffInboundWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;


    void referralList();
    void hello();
    void searchList();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }

    InboundContainerWidget *inboundContainer;
    Wt::WStackedWidget *resultStack_;
    UserSession *session_;
    const char *conninfo_;
    
    Wt::WTimer *refreshTimer; 

    ~StaffInboundWidget()
    {
        refreshTimer->stop();
    }

private:
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

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setStdId();
    Wt::WString strStdId();


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
    Wt::WString *studyInsuranceId_;
    void stopRefreshTimer();
    void startRefreshTimer();
    
    void search(Wt::WString searchString);
    void preSearch(Wt::WString search);
    void downloadReferral(WString& ref);
    void showReferralDialog(WString & ref);

};

#endif //STAFF_INBOUND_WIDGET_H
