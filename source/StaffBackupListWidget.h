/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_BACKUP_LIST_WIDGET_H_
#define STAFF_BACKUP_LIST_WIDGET_H_

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

class StaffBackupListWidget : public Wt::WContainerWidget
{
public:
    StaffBackupListWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    void backupList();

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
private:

    UserSession *session_;

    Wt::WText *staticComment_;
    Wt::WString commentTextOut_;

    WString setCommentId();

    Wt::WPushButton *btnViewBackup;

    void showBackupDialog(std::string& bu);


};

#endif //STAFF_BACKUP_LIST_WIDGET_H_
