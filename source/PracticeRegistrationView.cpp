/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/
#include "PracticeRegistrationView.h"
#include "PracticeDetailsModel.h"

#include <Wt/WLineEdit>

using namespace Wt;

const char *regconninfo = "hostaddr = 127.0.0.1 port = 5432 user = clinicore dbname = clinicore password = Falcon1337";

PracticeRegistrationView::PracticeRegistrationView(PracticeSession& session,
    Wt::Auth::AuthWidget *authWidget)
    : Wt::Auth::RegistrationWidget(authWidget),
    session_(session)
{
    setTemplateText(tr("template.registration"));
    detailsModel_ = new PracticeDetailsModel(regconninfo, session_, this);

    updateView(detailsModel_);
}

Wt::WFormWidget *PracticeRegistrationView::createFormWidget(Wt::WFormModel::Field field)
{
    if (field == PracticeDetailsModel::PracticeNameField)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeZipField)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeDrFirstName)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeDrLastName)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeDrNpi)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeEmailAddress)
        return new Wt::WLineEdit();
    else if (field == PracticeDetailsModel::PracticeSpecialty)
        return new Wt::WLineEdit();
    // else
    //     return Wt::Auth::RegistrationWidget::createFormWidget(field);
}

bool PracticeRegistrationView::validate()
{
    bool result = Wt::Auth::RegistrationWidget::validate();

    updateModel(detailsModel_);
    if (!detailsModel_->validate())
        result = false;
    updateView(detailsModel_);

    return result;
}

void PracticeRegistrationView::registerUserDetails(Wt::Auth::User& practice)
{
    Wt::log("notice") << "registerPracticeDetails has been called";
    detailsModel_->save(practice);
}
