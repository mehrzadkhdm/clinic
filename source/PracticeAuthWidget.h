/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_AUTH_WIDGET_H_
#define PRACTICE_AUTH_WIDGET_H_

#include <Wt/WDialog>
#include <Wt/Auth/AuthWidget>
#include <Wt/WContainerWidget>
#include "PracticeLostPasswordWidget.h"

class PracticeSession;

class PracticeAuthWidget : public Wt::Auth::AuthWidget
{
public:
    PracticeAuthWidget(PracticeSession& session, Wt::WContainerWidget*parent = 0);

    /* We will use a custom registration view */
    virtual Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);

	virtual Wt::WDialog *showDialog(const Wt::WString& title, Wt::WWidget *contents);
	virtual Wt::WWidget *createLostPasswordView();
private:
    PracticeSession& session_;

};

#endif // PRACTICE_AUTH_WIDGET_H_
