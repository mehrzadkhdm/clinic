/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PHYSICIAN_REGISTRATION_WIDGET_H_
#define PHYSICIAN_REGISTRATION_WIDGET_H_

#include <Wt/WString>
#include <Wt/WTemplateFormView>
#include "PhysicianRegistrationModel.h"
#include "PhysicianDetailsModel.h"
#include "PracticeSession.h"

namespace Wt {
  namespace Auth {

class AuthWidget;
class Login;
class OAuthProcess;

/*! \class PhysicianRegistrationWidget Wt/Auth/PhysicianRegistrationWidget
 *  \brief A registration widget.
 *
 * This implements a widget which allows a new user to register.  The
 * widget renders the <tt>"Wt.Auth.template.registration"</tt>
 * template. and uses a PhysicianRegistrationModel for the actual registration
 * logic.
 *
 * Typically, you may want to specialize this widget to ask for other
 * information.
 *
 * \ingroup auth
 */
class PhysicianRegistrationWidget : public WTemplateFormView
{
public:
  /*! \brief Constructor
   *
   * Creates a new authentication.
   */
  PhysicianRegistrationWidget(Wt::WString prac, Wt::WString pcc, PracticeSession *session, AuthWidget *authWidget = 0);

  /*! \brief Sets the registration model.
   */
  void setModel(PhysicianRegistrationModel *model);

  /*! \brief Returns the registration model.
   *
   * This returns the model that is used by the widget to do the actual
   * registration.
   */
 PhysicianRegistrationModel *model() const { return model_; }

  /*! \brief Updates the user-interface.
   *
   * This updates the user-interface to reflect the current state of the
   * model.
   */
  virtual void update();

protected:
  /*! \brief Validates the current information.
   *
   * The default implementation simply calls
   * PhysicianRegistrationModel::validate() on the model.
   *
   * You may want to reimplement this method if you've added other
   * information to the registration form that need validation.
   */
  virtual bool validate();

  /*! \brief Performs the registration.
   *
   * The default implementation checks if the information is valid
   * with validate(), and then calls
   * PhysicianRegistrationModel::doRegister(). If registration was successful,
   * it calls registerUserDetails() and subsequently logs the user in.
   */
  virtual void doRegister();

  /*! \brief Closes the registration widget.
   *
   * The default implementation simply deletes the widget.
   */
  virtual void close();

  /*! \brief Registers more user information.
   *
   * This method is called when a new user has been successfully
   * registered.
   *
   * You may want to reimplement this method if you've added other
   * information to the registration form which needs to be annotated
   * to the user.
   */
  virtual void registerUserDetails(User& user);

  virtual void render(WFlags<RenderFlag> flags);

protected:
  virtual WFormWidget *createFormWidget(PhysicianRegistrationModel::Field field);

private:
  AuthWidget *authWidget_;
  PhysicianRegistrationModel *model_;

  bool created_;
  Login *confirmPasswordLogin_;
  
  Wt::WString pccEmail_;
  PracticeSession *session_;
  Wt::WString practiceId_;
  
  PhysicianDetailsModel *detailsModel_;
  
  void checkLoginName();
  void checkPassword();
  void checkPassword2();
  void confirmIsYou();
  void confirmedIsYou();
  void oAuthDone(OAuthProcess *oauth, const Identity& identity);
};

  }
}

#endif // PHYSICIAN_REGISTRATION_WIDGET_H_
