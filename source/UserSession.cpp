/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include "UserSession.h"
#include "User.h"

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Postgres>

#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"

#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"

#include "Patient.h"
#include "Practice.h"
#include "Report.h"
#include "Insurance.h"
#include "InsuranceGroup.h"
#include "ShiftNote.h"
#include "Locality.h"
#include "Order.h"
#include "Attachment.h"
#include "Address.h"
#include "SleepStudy.h"
#include "StudyType.h"
#include "Backup.h"
#include "Comment.h"
#include "Referral.h"
#include "PhysicianVisit.h"
#include "PhysicianIssue.h"
#include "GPSPath.h"

using namespace Wt;
using namespace Wt::Dbo;

namespace {
    Wt::Auth::AuthService myAuthService;
    Wt::Auth::PasswordService myPasswordService(myAuthService);
}

void UserSession::configureAuth()
{
    myAuthService.setAuthTokensEnabled(true, "logincookie");
    myAuthService.setEmailVerificationEnabled(true);

    Wt::Auth::PasswordVerifier *verifier = new Wt::Auth::PasswordVerifier();
    verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));
    myPasswordService.setVerifier(verifier);
    myPasswordService.setAttemptThrottlingEnabled(true);
    myPasswordService.setStrengthValidator(new Wt::Auth::PasswordStrengthValidator());
}

UserSession::UserSession(const std::string& db)
    : connection_(db)
{
    setConnection(connection_);

    mapClass<Practice>("practice");
    mapClass<PracAuthInfo>("prac_auth_info");
    mapClass<PracAuthInfo::AuthIdentityType>("prac_auth_identity");
    mapClass<PracAuthInfo::AuthTokenType>("prac_auth_token");

    mapClass<User>("user");
    mapClass<AuthInfo>("auth_info");
    mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    mapClass<AuthInfo::AuthTokenType>("auth_token");


    mapClass<Patient>("patient");
    mapClass<GPSPath>("gpspath");
    mapClass<Address>("address");
    mapClass<Insurance>("insurance");
    mapClass<InsuranceGroup>("insurance_group");
    mapClass<ShiftNote>("shiftnote");
    mapClass<StudyType>("studytype");
    mapClass<Locality>("locality");

    mapClass<Backup>("backup");
    mapClass<Comment>("comment");
    mapClass<PhysicianVisit>("physicianvisit");
    mapClass<PhysicianIssue>("physicianissue");

    mapClass<Referral>("referral");
    mapClass<SleepStudy>("sleepstudy");
    mapClass<Order>("order");
    mapClass<Report>("report");
    mapClass<AttachmentDb>("attachment");
    mapClass<CompositeEntity>("composite");

    try {
        createTables();
        std::cerr << "Created database." << std::endl;
    }
    catch (Wt::Dbo::Exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database";
    }

    users_ = new UserDatabase(*this);
}

Wt::Auth::AbstractUserDatabase& UserSession::users()
{
    return *users_;
}

dbo::ptr<User> UserSession::user() const
{
    if (login_.loggedIn()) {
        dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
        return authInfo->user();
    }
    else
        return dbo::ptr<User>();
}
Wt::WString UserSession::strUserEmail()
{
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    if (authInfo->unverifiedEmail() == "")
    {
        return authInfo->email();
    } else {
        return authInfo->unverifiedEmail();
    }
}

const Wt::Auth::AuthService& UserSession::auth()
{
    return myAuthService;
}

const Wt::Auth::PasswordService& UserSession::passwordAuth()
{
    return myPasswordService;
}

