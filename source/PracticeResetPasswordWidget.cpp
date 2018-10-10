/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/
#include <Wt/Auth/AbstractUserDatabase>


#include "Wt/Auth/AbstractUserDatabase"
#include "Wt/Auth/AuthWidget"
#include "Wt/Auth/Login"

#include "Wt/WAnchor"
#include "Wt/WApplication"
#include "Wt/WContainerWidget"
#include "Wt/WDialog"
#include "Wt/WImage"
#include "Wt/WLineEdit"
#include "Wt/WPushButton"
#include "Wt/WText"
#include "Wt/WTheme"
#include <Wt/WString>

#include <memory>
#include "PhysicianRegistrationModel.h"
#include "PhysicianRegistrationWidget.h"
#include "PhysicianDetailsModel.h"
#include "PracticeSession.h"
#include "Practice.h"

const char *detailsconninfo = "hostaddr = 127.0.0.1 port = 5432 user = clinicore dbname = clinicore password = Falcon1337";

namespace Wt {

//LOGGER("Auth.PhysicianRegistrationWidget");       

  namespace Auth {

PhysicianRegistrationWidget::PhysicianRegistrationWidget(Wt::WString prac, Wt::WString pcc, PracticeSession *session, AuthWidget *authWidget)
  : WTemplateFormView(tr("Wt.Auth.template.registration")),
    authWidget_(authWidget),
    model_(0),
    session_(session),
    pccEmail_(pcc),
    practiceId_(prac),
    created_(false),
    confirmPasswordLogin_(0)
{
  WApplication *app = WApplication::instance();
  app->theme()->apply(this, this, AuthWidgets);

  detailsModel_ = new PhysicianDetailsModel(detailsconninfo, practiceId_, session_, this);
}

void PhysicianRegistrationWidget::setModel(PhysicianRegistrationModel *model)
{
  if (!model_ && model)
    model->login().changed().connect(this, &PhysicianRegistrationWidget::close);

  delete model_;
  model_ = model;
}

void PhysicianRegistrationWidget::render(WFlags<RenderFlag> flags)
{
  if (!created_) {
    update();
    created_ = true;
  }

  WTemplateFormView::render(flags);
}

WFormWidget *PhysicianRegistrationWidget::createFormWidget(WFormModel::Field field)
{
  WFormWidget *result = 0;

  if (field == PhysicianRegistrationModel::LoginNameField) {
    result = new WLineEdit();
    result->changed().connect
      (boost::bind(&PhysicianRegistrationWidget::checkLoginName, this));
  } else if (field == PhysicianRegistrationModel::EmailField) {
    result = new WLineEdit();
  } else if (field == PhysicianRegistrationModel::ChoosePasswordField) {
    WLineEdit *p = new WLineEdit();
    p->setEchoMode(WLineEdit::Password);
    p->keyWentUp().connect
      (boost::bind(&PhysicianRegistrationWidget::checkPassword, this));
    p->changed().connect
      (boost::bind(&PhysicianRegistrationWidget::checkPassword, this));
    result = p;
  } else if (field == PhysicianRegistrationModel::RepeatPasswordField) {
    WLineEdit *p = new WLineEdit();
    p->setEchoMode(WLineEdit::Password);
    p->changed().connect
      (boost::bind(&PhysicianRegistrationWidget::checkPassword2, this));
    result = p;
  }

  return result;
}

void PhysicianRegistrationWidget::update()
{
  if (model_->passwordAuth())
    bindString("password-description",
         tr("Wt.Auth.password-registration"));
  else
    bindEmpty("password-description");

  updateView(model_);

  if (!created_) {
    WLineEdit *password = resolve<WLineEdit *>
      (PhysicianRegistrationModel::ChoosePasswordField);
    WLineEdit *password2 = resolve<WLineEdit *>
      (PhysicianRegistrationModel::RepeatPasswordField);
    WText *password2Info = resolve<WText *>
      (PhysicianRegistrationModel::RepeatPasswordField + std::string("-info"));

    if (password && password2 && password2Info)
      model_->validatePasswordsMatchJS(password, password2, password2Info);
  }

  WAnchor *isYou = resolve<WAnchor *>("confirm-is-you");
  if (!isYou) {
    isYou = new WAnchor(std::string("#"), tr("Wt.Auth.confirm-is-you"));
    isYou->hide();
    bindWidget("confirm-is-you", isYou);
  }

  if (model_->isConfirmUserButtonVisible()) {
    if (!isYou->clicked().isConnected())
      isYou->clicked().connect(this, &PhysicianRegistrationWidget::confirmIsYou);
    isYou->show();
  } else
    isYou->hide();

  if (model_->isFederatedLoginVisible()) {
    if (!conditionValue("if:oauth")) {
      setCondition("if:oauth", true);
      if (model_->passwordAuth())
  bindString("oauth-description", tr("Wt.Auth.or-oauth-registration"));
      else
  bindString("oauth-description", tr("Wt.Auth.oauth-registration"));

      WContainerWidget *icons = new WContainerWidget();
      icons->addStyleClass("Wt-field");

      for (unsigned i = 0; i < model_->oAuth().size(); ++i) {
  const OAuthService *service = model_->oAuth()[i];

  WImage *w = new WImage("css/oauth-" + service->name() + ".png", icons);
  w->setToolTip(service->description());
  w->setStyleClass("Wt-auth-icon");
  w->setVerticalAlignment(AlignMiddle);
  OAuthProcess *const process
    = service->createProcess(service->authenticationScope());
  w->clicked().connect(process, &OAuthProcess::startAuthenticate);

  process->authenticated().connect
    (boost::bind(&PhysicianRegistrationWidget::oAuthDone, this, process, _1));

  WObject::addChild(process);
      }

      bindWidget("icons", icons);
    }
  } else {
    setCondition("if:oauth", false);
    bindEmpty("icons");
  }

  if (!created_) {
    WPushButton *okButton = new WPushButton(tr("Wt.Auth.register"));
    WPushButton *cancelButton = new WPushButton(tr("Wt.WMessageBox.Cancel"));

    bindWidget("ok-button", okButton);
    bindWidget("cancel-button", cancelButton);

    okButton->clicked().connect(this, &PhysicianRegistrationWidget::doRegister);
    cancelButton->clicked().connect(this, &PhysicianRegistrationWidget::close);

    created_ = true;
  }
}

void PhysicianRegistrationWidget::oAuthDone(OAuthProcess *oauth,
           const Identity& identity)
{
  if (identity.isValid()) {
    //LOG_SECURE(oauth->service().name() << ": identified: as "
    //     << identity.id() << ", " << identity.name() << ", "
    //     << identity.email());

    if (!model_->registerIdentified(identity))
      update();
  } else {
    if (authWidget_)
      authWidget_->displayError(oauth->error());
    //LOG_SECURE(oauth->service().name() << ": error: " << oauth->error());
  }
}

void PhysicianRegistrationWidget::checkLoginName()
{
  updateModelField(model_, PhysicianRegistrationModel::LoginNameField);
  model_->validateField(PhysicianRegistrationModel::LoginNameField);
  model_->setValidated(PhysicianRegistrationModel::LoginNameField, false);
  update();
}

void PhysicianRegistrationWidget::checkPassword()
{
  updateModelField(model_, PhysicianRegistrationModel::LoginNameField);
  updateModelField(model_, PhysicianRegistrationModel::ChoosePasswordField);
  updateModelField(model_, PhysicianRegistrationModel::EmailField);
  model_->validateField(PhysicianRegistrationModel::ChoosePasswordField);
  model_->setValidated(PhysicianRegistrationModel::ChoosePasswordField, false);
  update();
}

void PhysicianRegistrationWidget::checkPassword2()
{
  updateModelField(model_, PhysicianRegistrationModel::ChoosePasswordField);
  updateModelField(model_, PhysicianRegistrationModel::RepeatPasswordField);
  model_->validateField(PhysicianRegistrationModel::RepeatPasswordField);
  model_->setValidated(PhysicianRegistrationModel::RepeatPasswordField, false);
  update();
}

bool PhysicianRegistrationWidget::validate()
{
  return model_->validate();
}

void PhysicianRegistrationWidget::doRegister()
{
  std::auto_ptr<Wt::Auth::AbstractUserDatabase::Transaction>
    t(model_->users().startTransaction());

  updateModel(model_);

  if (validate()) {
    Wt::Auth::User practice = model_->doRegister();
    if (practice.isValid()) {
      registerUserDetails(practice);

      if (authWidget_)
        authWidget_->displayInfo
          (WString::tr("Wt.Auth.confirm-email-first"));

      close();

    } else
      update();
  } else
    update();

  if (t.get())
    t->commit();
}

void PhysicianRegistrationWidget::registerUserDetails(User& user)
{ 
  detailsModel_->save(user, model_->inputEmail(), pccEmail_);
}

void PhysicianRegistrationWidget::close()
{
  delete this;
}

void PhysicianRegistrationWidget::confirmIsYou()
{
  updateModel(model_);

  switch (model_->confirmIsExistingUser()) {
  case PhysicianRegistrationModel::ConfirmWithPassword:
    {
      delete confirmPasswordLogin_;
      confirmPasswordLogin_ = new Login();
      confirmPasswordLogin_->login(model_->existingUser(), WeakLogin);
      confirmPasswordLogin_
  ->changed().connect(this, &PhysicianRegistrationWidget::confirmedIsYou);

      WDialog *dialog =
  authWidget_->createPasswordPromptDialog(*confirmPasswordLogin_);
      dialog->show();
    }

    break;
    //LOG_ERROR("that's gone haywire.");
  }
}

void PhysicianRegistrationWidget::confirmedIsYou()
{
  if (confirmPasswordLogin_->state() == StrongLogin) {
    model_->existingUserConfirmed();
  } else {
    delete confirmPasswordLogin_;
    confirmPasswordLogin_ = 0;
  }
}

  }
}
