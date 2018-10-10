/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/Auth/AbstractPasswordService"
#include "Wt/Auth/AuthModel"
#include "Wt/Auth/Login"
#include "Wt/Auth/User"

#include "PracticeUpdatePasswordWidget.h"
#include "PhysicianRegistrationModel.h"
#include "Practice.h"

#include "Wt/WLineEdit"
#include "Wt/WPushButton"
#include "Wt/WText"

using namespace Wt;

PracticeUpdatePasswordWidget::PracticeUpdatePasswordWidget(const Wt::Auth::User& user,
					   Wt::Auth::PhysicianRegistrationModel *registrationModel,
					   Wt::Auth::AuthModel *authModel,
					   Wt::WContainerWidget *parent)
  : Wt::WTemplateFormView(tr("Wt.Auth.template.update-password"), parent),
    user_(user),
    registrationModel_(registrationModel),
    authModel_(authModel)
{
  registrationModel_->setValue(Wt::Auth::PhysicianRegistrationModel::LoginNameField,
			       user.identity(Wt::Auth::Identity::LoginName));
  registrationModel_->setReadOnly(Wt::Auth::PhysicianRegistrationModel::LoginNameField, true);

  if (user.password().empty())
    authModel_ = 0;

  if (authModel_ && authModel_->baseAuth()->emailVerificationEnabled()) {
    /*
     * This is set in the model so that the password checker can take
     * into account whether the password is derived from the email
     * address.
     */
    registrationModel_->setValue
      (Wt::Auth::PhysicianRegistrationModel::EmailField,
       WT_USTRING::fromUTF8(user.email() + " " + user.unverifiedEmail()));
  }

  // Make sure it does not block validation
  registrationModel_->setVisible(Wt::Auth::PhysicianRegistrationModel::EmailField, false);

  Wt::WPushButton *okButton = new WPushButton(tr("Wt.WMessageBox.Ok"));
  Wt::WPushButton *cancelButton = new WPushButton(tr("Wt.WMessageBox.Cancel"));

  if (authModel_) {
    authModel_->setValue(Wt::Auth::AuthModel::LoginNameField,
			 user.identity(Wt::Auth::Identity::LoginName));

    updateViewField(authModel_, Wt::Auth::AuthModel::PasswordField);

    authModel_->configureThrottling(okButton);

    WLineEdit *password = resolve<WLineEdit *>(Wt::Auth::AuthModel::PasswordField);
    password->setFocus(true);
  }

  updateView(registrationModel_);

  WLineEdit *password = resolve<WLineEdit *>
    (Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField);
  WLineEdit *password2 = resolve<WLineEdit *>
    (Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField);
  WText *password2Info = resolve<WText *>
    (Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField + std::string("-info"));

  registrationModel_->validatePasswordsMatchJS(password,
					       password2, password2Info);

  if (!authModel_)
    password->setFocus(true);

  okButton->clicked().connect(this, &PracticeUpdatePasswordWidget::doUpdate);
  cancelButton->clicked().connect(this, &PracticeUpdatePasswordWidget::close);

  bindWidget("ok-button", okButton);
  bindWidget("cancel-button", cancelButton);

}

Wt::WFormWidget *PracticeUpdatePasswordWidget::createFormWidget(Wt::WFormModel::Field field)
{
  WFormWidget *result = 0;

  if (field == Wt::Auth::PhysicianRegistrationModel::LoginNameField) {
    result = new WLineEdit();
  } else if (field == Wt::Auth::AuthModel::PasswordField) {
    WLineEdit *p = new WLineEdit();
    p->setEchoMode(WLineEdit::Password);
    result = p;
  } else if (field == Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField) {
    WLineEdit *p = new WLineEdit();
    p->setEchoMode(WLineEdit::Password);
    p->keyWentUp().connect
      (boost::bind(&PracticeUpdatePasswordWidget::checkPassword, this));
    p->changed().connect
      (boost::bind(&PracticeUpdatePasswordWidget::checkPassword, this));
    result = p;
  } else if (field == Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField) {
    WLineEdit *p = new WLineEdit();
    p->setEchoMode(WLineEdit::Password);
    p->changed().connect
      (boost::bind(&PracticeUpdatePasswordWidget::checkPassword2, this));
    result = p;
  }

  return result;
}

void PracticeUpdatePasswordWidget::checkPassword()
{
  updateModelField(registrationModel_, Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField);
  registrationModel_->validateField(Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField);
  updateViewField(registrationModel_, Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField);
}

void PracticeUpdatePasswordWidget::checkPassword2()
{
  updateModelField(registrationModel_, Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField);
  registrationModel_->validateField(Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField);
  updateViewField(registrationModel_, Wt::Auth::PhysicianRegistrationModel::RepeatPasswordField);
}

bool PracticeUpdatePasswordWidget::validate()
{
  bool valid = true;

  if (authModel_) {
    updateModelField(authModel_, Wt::Auth::AuthModel::PasswordField);

    if (!authModel_->validate()) {
      updateViewField(authModel_, Wt::Auth::AuthModel::PasswordField);
      valid = false;
    }
  }

  registrationModel_->validateField(Wt::Auth::PhysicianRegistrationModel::LoginNameField);
  checkPassword();
  checkPassword2();

  registrationModel_->validateField(Wt::Auth::PhysicianRegistrationModel::EmailField);

  if (!registrationModel_->valid()) 
    valid = false;

  return valid;
}

void PracticeUpdatePasswordWidget::doUpdate()
{
  if (validate()) {
    WT_USTRING password
      = registrationModel_->valueText(Wt::Auth::PhysicianRegistrationModel::ChoosePasswordField);
    registrationModel_->passwordAuth()->updatePassword(user_, password);
    registrationModel_->login().login(user_);

    close();
  }
}

void PracticeUpdatePasswordWidget::close()
{
  delete this;
}
