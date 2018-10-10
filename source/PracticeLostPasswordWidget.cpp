/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include "PracticeLostPasswordWidget.h"
#include "Wt/Auth/AuthService"

#include "Wt/WLineEdit"
#include "Wt/WMessageBox"
#include "Wt/WPushButton"

namespace Wt {
  namespace Auth {

PracticeLostPasswordWidget::PracticeLostPasswordWidget(AbstractUserDatabase& users,
				       const AuthService& auth,
				       WContainerWidget *parent)
  : WTemplate(tr("Wt.Auth.template.lost-password"), parent),
    users_(users),
    baseAuth_(auth)
{
  addFunction("id", &Functions::id);
  addFunction("tr", &Functions::tr);
  addFunction("block", &Functions::block);

  WLineEdit *email = new WLineEdit();
  email->setFocus(true);

  WPushButton *okButton = new WPushButton(tr("Wt.Auth.send"));
  WPushButton *cancelButton = new WPushButton(tr("Wt.WMessageBox.Cancel"));

  okButton->clicked().connect(this, &PracticeLostPasswordWidget::send);
  cancelButton->clicked().connect(this, &PracticeLostPasswordWidget::cancel);

  bindWidget("email", email);
  bindWidget("send-button", okButton);
  bindWidget("cancel-button", cancelButton);
}

void PracticeLostPasswordWidget::send()
{
  WFormWidget *email = resolve<WFormWidget *>("email");

  baseAuth_.lostPassword(email->valueText().toUTF8(), users_);

  cancel();

  WMessageBox *const box = new WMessageBox(tr("Wt.Auth.lost-password"),
				     tr("Wt.Auth.mail-sent"),
				     NoIcon, Ok);
  box->setMinimumSize(600, 400);
  box->setMaximumSize(600, 400);
#ifndef WT_TARGET_JAVA
  box->buttonClicked().connect
    (boost::bind(&PracticeLostPasswordWidget::deleteBox, box));
#else
  box->buttonClicked().connect
    (boost::bind(&PracticeLostPasswordWidget::deleteBox, this, box));
#endif
  box->show();
}

void PracticeLostPasswordWidget::deleteBox(WMessageBox *box)
{
  delete box;
}

void PracticeLostPasswordWidget::cancel()
{
  delete this;
}

  }
}
