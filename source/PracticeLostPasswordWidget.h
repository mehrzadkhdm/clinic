// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef PRACTICE_LOST_PASSWORD_WIDGET_H_
#define PRACTICE_LOST_PASSWORD_WIDGET_H_

#include <Wt/WTemplate>

namespace Wt {
  namespace Auth {

class AbstractUserDatabase;
class AuthService;

/*! \class PracticeLostPasswordWidget Auth/PracticeLostPasswordWidget
 *  \brief A widget which initiates a lost-password email.
 *
 * The widget renders the <tt>"Wt.Auth.template.lost-password"</tt>
 * template. It prompts for an email address and then invokes
 * AuthService::lostPassword() with the given email address.
 *
 * \sa AuthWidget::createLostPasswordView()
 *
 * \ingroup auth
 */ 
class PracticeLostPasswordWidget : public WTemplate
{
public:
  /*! \brief Constructor
   */
  PracticeLostPasswordWidget(AbstractUserDatabase& users,
		     const AuthService& auth,
		     WContainerWidget *parent = 0);

protected:
  void send();
  void cancel();

private:
  AbstractUserDatabase& users_;
  const AuthService& baseAuth_;

#ifndef WT_TARGET_JAVA
  static void deleteBox(WMessageBox *box);
#else
  void deleteBox(WMessageBox *box);
 #endif
};

  }
}

#endif // PRACTICE_LOST_PASSWORD_WIDGET_H_