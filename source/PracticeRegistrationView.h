/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_REGISTRATION_VIEW_H_
#define PRACTICE_REGISTRATION_VIEW_H_

#include <Wt/Auth/RegistrationWidget>

class PracticeSession;
class PracticeDetailsModel;

class PracticeRegistrationView : public Wt::Auth::RegistrationWidget
{
public:
    PracticeRegistrationView(PracticeSession& session, Wt::Auth::AuthWidget *authWidget = 0);

    /* specialize to create user details fields */
    virtual Wt::WFormWidget *createFormWidget(Wt::WFormModel::Field field);

    /* specialize to also validate the user details */
    virtual bool validate();

    /* specialize to register user details */
    virtual void registerUserDetails(Wt::Auth::User& practice);

private:
    PracticeSession& session_;

    PracticeDetailsModel *detailsModel_;
};

#endif // PRACTICE_REGISTRATION_VIEW_H_
