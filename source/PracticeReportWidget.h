/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_REPORT_WIDGET_H_
#define PRACTICE_REPORT_WIDGET_H_

#include <fstream>
#include <iostream>
#include <string>

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

class PracticeReportWidget : public Wt::WContainerWidget
{

public:
    PracticeReportWidget(const char *conninfo, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }

    PracticeSession *session_;

public:

    //Patient demographics info input variables
    Wt::WLineEdit *patientFirstName_;
    Wt::WLineEdit *patientLastName_;
    Wt::WLineEdit *patientDob_;
    void demographics();
    void complete();
    void sendReport();
    void saveReport();

    Wt::WText *staticRef_;
    Wt::WText *outFirstName_;
    Wt::WText *outLastName_;
    Wt::WText *outDob_;

    Wt::WText *outRefId_;

    std::string strFirstName();
    std::string strLastName();
    std::string strDob();

    Wt::WText *reviewFirstName_;
    Wt::WText *reviewLastName_;
    Wt::WText *reviewDob_;
    Wt::WText *reviewRefId_;

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WText *orderSent_;



};

#endif //PRACTICE_REPORT_WIDGET_H
