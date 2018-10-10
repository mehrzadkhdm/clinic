/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include "PracticeSession.h"
#include "Practice.h"

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
#include "Referral.h"
#include "SleepStudy.h"
#include "Report.h"
#include "Order.h"
#include "Attachment.h"


using namespace Wt;


namespace {
    Wt::Auth::AuthService myAuthService;
    Wt::Auth::PasswordService myPasswordService(myAuthService);
}

void PracticeSession::configureAuth()
{
    myAuthService.setAuthTokensEnabled(true, "logincookie");
    myAuthService.setEmailVerificationEnabled(true);

    Wt::Auth::PasswordVerifier *verifier = new Wt::Auth::PasswordVerifier();
    verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));
    myPasswordService.setVerifier(verifier);
    myPasswordService.setAttemptThrottlingEnabled(true);
    myPasswordService.setStrengthValidator(new Wt::Auth::PasswordStrengthValidator());

}

PracticeSession::PracticeSession(const std::string& db)
    : connection_(db)
{
    setConnection(connection_);

    connection_.setProperty("show-queries", "true");

    mapClass<Practice>("practice");
    mapClass<PracAuthInfo>("prac_auth_info");
    mapClass<PracAuthInfo::AuthIdentityType>("prac_auth_identity");
    mapClass<PracAuthInfo::AuthTokenType>("prac_auth_token");

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

    practices_ = new PracticeDatabase(*this);
}

Wt::Auth::AbstractUserDatabase& PracticeSession::practices()
{
    return *practices_;
}

Wt::Auth::User PracticeSession::pracUser()
{
    return login_.user();
}

dbo::ptr<Practice> PracticeSession::practice()
{
    if (login_.loggedIn()) {
        return practice(login_.user());
    }
    else
        return dbo::ptr<Practice>();
}

dbo::ptr<Practice> PracticeSession::practice(const Wt::Auth::User& authUser)
{
    dbo::ptr<PracAuthInfo> authInfo = practices_->find(authUser);

    dbo::ptr<Practice> practice = authInfo->user();

    if (!practice) {
        practice = add(new Practice());
        authInfo.modify()->setUser(practice);
    }

    return practice;
}

Wt::WString PracticeSession::strPracticeName()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeName_;
}

Wt::WString PracticeSession::strPracticeZip()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeZip_;
}

Wt::WString PracticeSession::strDrFirstName()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeDrFirst_;
}

Wt::WString PracticeSession::strDrLastName()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeDrLast_;
}

Wt::WString PracticeSession::strPracticeNpi()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeNpi_;
}

Wt::WString PracticeSession::strPracticeEmail()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refPracticeEmail_;
}

Wt::WString PracticeSession::strSpecialty()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->refSpecialty_;
}

Wt::WString PracticeSession::strEmailOpt()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->pracEmailOpt_;
}

Wt::WString PracticeSession::strPracticeId()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->practiceId_;
}   

Wt::WString PracticeSession::strIsGroup()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->isGroup_;
}   

Wt::WString PracticeSession::strGroupId()
{
    dbo::Transaction transaction(PracticeSession);
    dbo::ptr<Practice> p = practice();
    return p->groupId_;
}   

const Wt::Auth::AuthService& PracticeSession::auth()
{
    return myAuthService;
}

const Wt::Auth::PasswordService& PracticeSession::passwordAuth()
{
    return myPasswordService;
}

const Wt::Auth::AbstractPasswordService& PracticeSession::absPasswordAuth()
{
    return myPasswordService;
}

