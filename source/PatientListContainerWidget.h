/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PATIENT_LIST_CONTAINER_WIDGET
#define PATIENT_LIST_CONTAINER_WIDGET

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
#include "Composer.h"
#include "UserSession.h"

using namespace Wt;
using namespace Wt::Dbo;

class UserSession;
class PatientListContainerWidget : public Wt::WContainerWidget
{
public:
    PatientListContainerWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;


    Wt::WPushButton *btnSelectPatient;
    Wt::WPushButton *btnShowPatient;
    Composer *composer_;

    void showCurrent(int limit, int offset);
    void showSearch(Wt::WString& curstr, int limit, int offset);
    void showRescheduleSearch(Wt::WString& curstr, int limit, int offset);
    void showReferralSearch(Wt::WString& curstr, int limit, int offset);
    void showStudySearch(Wt::WString& curstr, int limit, int offset);


    void createReferral(std::string prac, std::string pt);
    void showPatient(std::string& pt);
    void addPatientToBackup(std::string& pt);

    //refresh and stop timer signals
    Wt::Signal<int, std::string>& done() { return done_; }
    Wt::Signal<int, std::string>& stopTimer() { return stopTimer_; }

private:

    Wt::Signal<int, std::string> done_;
     Wt::Signal<int, std::string> stopTimer_;

    UserSession *session_;
    const char *conninfo_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }
    void selectPracticeDialog(Wt::WString pt);
    void showReferralDialog(Wt::WString& ref);
    void showSleepStudyDialog(Wt::WString& std);
    void deleteAttachment(Wt::WString att);

    Wt::WPushButton *btnShowRef;
    Wt::WPushButton *btnShowStd;
    Wt::WPushButton *btnSelectPractice;

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

    Wt::WString newStudyTypeName_;
    Wt::WString newStudyTypeId_;
    Wt::WString newStudyTypeCode_;

    Wt::WLineEdit *patientFirstName_;
    Wt::WLineEdit *patientLastName_;
    Wt::WLineEdit *patientCity_;
    Wt::WLineEdit *patientHomePhone_;
    Wt::WLineEdit *patientCellPhone_;
    Wt::WLineEdit *patientEmail_;
    Wt::WLineEdit *patientDob_;
    Wt::WLineEdit *dobMonth;
    Wt::WLineEdit *dobDay;
    Wt::WLineEdit *dobYear;

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
    Wt::WText *dbReferralDate_;
    Wt::WText *dbAuthorizationDate_;

    Wt::WPushButton *btnDownloadAtt;
    Wt::WPushButton *btnDeleteAtt;
    
    Wt::WText *downDone;

    Wt::WText *staticComment_;

    WString setCommentId();

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setPracId();
    Wt::WString strPracId();

    Wt::WString setStdId();
    Wt::WString strStdId();

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString setBuId();

    Wt::WString studyInsuranceId_;
    Wt::WString studyInsuranceName_;
    Wt::WString referralTypeId_;
    Wt::WString referralTypeName_;
    Wt::WString referralTypeCode_;

    Wt::WText *staticBu_;
    Wt::WString strBuId();
    Wt::WPushButton *btnAddToBackup_;
};

#endif //PATIENT_LIST_CONTAINER_WIDGET
