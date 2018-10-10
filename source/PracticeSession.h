/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICESESSION_H_
#define PRACTICESESSION_H_

#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"
#include <Wt/WString>
#include "Wt/Auth/Login"
#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Postgres>
#include "Practice.h"

namespace dbo = Wt::Dbo;
using namespace Wt;

typedef Wt::Auth::Dbo::UserDatabase<PracAuthInfo> PracticeDatabase;


class PracticeSession : public dbo::Session
{
public:
    static void configureAuth();
    PracticeSession(const std::string& db);

    dbo::ptr<Practice> practice();
    dbo::weak_ptr<Practice> wprac();
    dbo::ptr<Practice> practice(const Wt::Auth::User& practice);

    Wt::Auth::AbstractUserDatabase& practices();
    Wt::Auth::Login& login() { return login_; }

    static const Wt::Auth::AuthService& auth();
    static const Wt::Auth::PasswordService& passwordAuth();
    static const Wt::Auth::AbstractPasswordService& absPasswordAuth();

    Wt::Auth::User pracUser();

    Wt::WString strPracticeName();
    Wt::WString strPracticeZip();
    Wt::WString strDrFirstName();
    Wt::WString strDrLastName();
    Wt::WString strPracticeNpi();
    Wt::WString strPracticeEmail();
    Wt::WString strSpecialty();
    Wt::WString strEmailOpt();
    Wt::WString strPracticeId();
    Wt::WString strGroupId();
    Wt::WString strIsGroup();

private:
    Wt::Auth::Login login_;
    dbo::backend::Postgres connection_;
    PracticeDatabase *practices_;


};
#endif // PRACTICESESSION_H_
