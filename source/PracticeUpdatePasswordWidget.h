// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef PRACTICE_UPDATE_PASSWORD_WIDGET_H_
#define PRACTICE_UPDATE_PASSWORD_WIDGET_H_

#include <Wt/WTemplateFormView>
#include "PhysicianRegistrationModel.h"
#include <Wt/Auth/User>

#include "Practice.h"

using namespace Wt;

class AuthModel;
class PhysicianRegistrationModel;

class PracticeUpdatePasswordWidget : public Wt::WTemplateFormView
{
public:
  /*! \brief Constructor.
   *
   * If \p authModel is not \c null, the user also has to authenticate
   * first using his current password.
   */
  PracticeUpdatePasswordWidget(const Wt::Auth::User& user, Wt::Auth::PhysicianRegistrationModel *registrationModel,
		       Wt::Auth::AuthModel *authModel, Wt::WContainerWidget *parent = 0);

protected:
  virtual Wt::WFormWidget *createFormWidget(WFormModel::Field field);

private:
  Wt::Auth::User user_;

  Wt::Auth::PhysicianRegistrationModel *registrationModel_;
  Wt::Auth::AuthModel *authModel_;

  void checkPassword();
  void checkPassword2();
  bool validate();
  void doUpdate();
  void close();
};


#endif // PRACTICE_UPDATE_PASSWORD_WIDGET_H_
