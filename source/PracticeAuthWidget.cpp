/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <Wt/WDialog>
#include <Wt/WContainerWidget>
#include "PracticeAuthWidget.h"
#include "PracticeRegistrationView.h"
#include "PracticeLostPasswordWidget.h"
#include "PracticeSession.h"

class PracticeLostPasswordWidget;

PracticeAuthWidget::PracticeAuthWidget(PracticeSession& session, Wt::WContainerWidget* parent)
    : Wt::Auth::AuthWidget(PracticeSession::auth(), session.practices(), session.login(), parent),
    session_(session)
{  }

Wt::WWidget *PracticeAuthWidget::createRegistrationView(const Wt::Auth::Identity& id)
{
    PracticeRegistrationView *w = new PracticeRegistrationView(session_, this);
    Wt::Auth::RegistrationModel *model = createRegistrationModel();

    if (id.isValid())
        model->registerIdentified(id);

    w->setModel(model);

    return w;
}

Wt::WDialog *PracticeAuthWidget::showDialog(const Wt::WString& title, Wt::WWidget *contents)
{
	Wt::WDialog *dialog = Wt::Auth::AuthWidget::showDialog(title, contents);
    
    dialog->setMinimumSize(600, 400);
    dialog->setMaximumSize(600, 400);
	
	return dialog;
}

Wt::WWidget *PracticeAuthWidget::createLostPasswordView()
{
  return new Wt::Auth::PracticeLostPasswordWidget(model()->users(), *model()->baseAuth());
}
