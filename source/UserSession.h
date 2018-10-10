/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef USERSESSION_H_
#define USERSESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Postgres>
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"


#include "Wt/Auth/Login"


#include "User.h"

namespace dbo = Wt::Dbo;
using namespace Wt;

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;


class UserSession : public dbo::Session
{
public:
    static void configureAuth();
    UserSession(const std::string& db);
    dbo::ptr<User> user() const;

    Wt::Auth::AbstractUserDatabase& users();
    Wt::Auth::Login& login() { return login_; }

    static const Wt::Auth::AuthService& auth();
    static const Wt::Auth::PasswordService& passwordAuth();

    Wt::WString strUserEmail();
private:
    dbo::backend::Postgres connection_;
    UserDatabase *users_;
    Wt::Auth::Login login_;


};
#endif // USERSESSION_H_
