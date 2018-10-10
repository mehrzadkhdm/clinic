/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std io stuff
#include <sstream>
#include <fstream>
#include <iostream>
//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>


//wt stuff

#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WText>
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WLabel>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WDialog>
#include <Wt/WTemplate>
#include <Wt/WCalendar>

#include "Wt/Auth/AbstractPasswordService"
#include "Wt/Auth/AuthModel"
#include "Wt/Auth/Login"
#include "Wt/Auth/User"

#include "PracticeUpdatePasswordWidget.h"
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>

//other header files
#include "PhysicianRegistrationModel.h"
#include "PracticeSettingsWidget.h"
#include "Practice.h"
#include "PracticeSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

PracticeSettingsWidget::PracticeSettingsWidget(const char *conninfo, Wt::Auth::AuthModel *authModel, PracticeSession *session, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo),
authModel_(authModel),
session_(session)
{
    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");
    dbsession.mapClass<Practice>("practice");

    dbo::Transaction transaction(dbsession);
    try {
        dbsession.createTables();
        log("info") << "Database created";
    }
    catch (...) {
        log("info") << "Using existing database";
    }

    transaction.commit();

    PracticeSettingsWidget::demographics();

}

void PracticeSettingsWidget::demographics()
{

    Wt::WString practiceId = session_->strPracticeId();
    dbo::Transaction trans(dbsession);
    
    dbo::ptr<Practice> prac = dbsession.find<Practice>().where("prac_id = ?").bind(practiceId);
    Wt::WString current = prac->pracEmailOpt_;

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    container->addWidget(new Wt::WText("<h4>Send Email Notifications</h4>"));
    container->addWidget(new Wt::WText("Would you like us to send you email notifications for your patient's sleep study progress?"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("Current Setting: "+current));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    Wt::WComboBox *cb = new Wt::WComboBox(container);
    cb->addItem("");
    cb->addItem("Yes");
    cb->addItem("No");
    cb->setMargin(10, Wt::Right);
    cb->setCurrentIndex(0);
    Wt::WText *out = new Wt::WText(container);
    Wt::WText *opt = new Wt::WText();
    out->addStyleClass("help-block");


    Wt::WPushButton *ok = new Wt::WPushButton("Update");

    ok->clicked().connect(ok, &Wt::WPushButton::disable);
    ok->clicked().connect(std::bind([=]() {
        opt->setText(cb->currentText());

        Wt::WString pracId = session_->strPracticeId();

        dbo::Transaction trans(dbsession);
        dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracId);
        pc.modify()->pracEmailOpt_ = WString(opt->text()).toUTF8();
        trans.commit();

         out->setText(Wt::WString::fromUTF8("Your account settings have been updated."));

    }));
    container->addWidget(ok);

/*
    Wt::WPushButton *resetPass = new Wt::WPushButton("RESET ACCOUNT PASSWORD");
    resetPass->clicked().connect(ok, &Wt::WPushButton::disable);
    resetPass->clicked().connect(std::bind([=]() {
        resetPassword();
    }));
    container->addWidget(new Wt::WBreak());
    container->addWidget(resetPass);
*/
    addWidget(container);

}
void PracticeSettingsWidget::resetPassword()
{

    Wt::log("info") << "Practice reset password";
    
    Wt::WDialog *dialog = new Wt::WDialog("Reset Password");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);    
    
    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

/*
    dbo::Transaction transaction(dbsession);
   
    Wt::log("info") << "Find practice by uuid";
    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

    Wt::log("info") << "Setting user to AuthUser(practice.id)";
*/
    Wt::Auth::User user = session_->pracUser();

    Wt::log("info") << "declaring WtAuthLogin";
    Wt::Auth::Login login;

    Wt::log("info") << "create the PhysicianRegistrationModel";
    Wt::Auth::PhysicianRegistrationModel *result = new Wt::Auth::PhysicianRegistrationModel(PracticeSession::auth(), session_->practices(), login, user);

    Wt::log("info") << "create the PracticeUpdatePasswordWidget";
    PracticeUpdatePasswordWidget *w = new PracticeUpdatePasswordWidget(user, result, authModel_);

    Wt::log("info") << "Add password reset widget to container";

    container->addWidget(w);


    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    Wt::WPushButton *alright = new Wt::WPushButton("Done", dialog->footer());

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    container->show();
    dialog->show();
}