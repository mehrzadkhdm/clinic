/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_STUDY_ORDER_H_
#define STAFF_STUDY_ORDER_H_

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

using namespace Wt;


class StaffStudyOrderWidget : public Wt::WContainerWidget
{
public:
    StaffStudyOrderWidget(const char *conninfo, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;

    void orderList();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
private:
    Wt::WText *staticPt_;
    Wt::WText *staticStd_;


    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    Wt::WString setPtId();
    Wt::WString strPtId();

    Wt::WString setStdId();
    Wt::WString strStdId();


    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WString strT(WTemplate& tem);


    Wt::WText *orderSent_;
    Wt::WPushButton *btnDownloadRef;
    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;

    void downloadReferral(WString& ref);
    void showOrderDialog(WString & ord);


};

#endif //STAFF_STUDY_ORDER_H
