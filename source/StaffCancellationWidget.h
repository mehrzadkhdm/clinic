/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_CANCELLATION_WIDGET_H_
#define STAFF_CANCELLATION_WIDGET_H_

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

class UserSession;

class StaffCancellationWidget : public Wt::WContainerWidget
{
public:
    StaffCancellationWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    void studyList();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
private:

    UserSession *session_;

    Wt::WText *staticPt_;
    Wt::WText *staticStd_;
    Wt::WText *staticRef_;
    Wt::WText *staticPrac_;

    Wt::WText *staticComment_;
    Wt::WString commentTextOut_;

    WString setCommentId();
    WString setStdId();

    Wt::WPushButton *btnViewBackup;
    Wt::WPushButton *btnSelectSleepStudy;
    Wt::WPushButton *btnSelectSleepStudyNo;
    Wt::WText *dbStudyLocation_;
    Wt::WText *dbStudyTime_;
    Wt::WText *dbStudyDate_;

    void showStudy(Wt::WString& std);

    Wt::WPushButton *btnDownloadAtt;
    Wt::WText *downDone;


};

#endif //STAFF_CANCELLATION_WIDGET_H_
