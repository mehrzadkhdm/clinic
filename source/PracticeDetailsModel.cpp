/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <Wt/WText>
#include <Wt/WApplication>
#include "PracticeDetailsModel.h"
#include "Practice.h"
#include "PracticeSession.h"
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

const Wt::WFormModel::Field PracticeDetailsModel::PracticeNameField = "practice-name";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeZipField = "practice-zip";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeDrFirstName = "practice-drfirstname";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeDrLastName = "practice-drlastname";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeDrNpi = "practice-npi";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeEmailAddress = "practice-email";
const Wt::WFormModel::Field PracticeDetailsModel::PracticeSpecialty = "practice-specialty";

namespace dbo = Wt::Dbo;

PracticeDetailsModel::PracticeDetailsModel(const char *conninfo, PracticeSession& session, Wt::WObject *parent)
    : Wt::WFormModel(parent),
    pg_(conninfo),
    session_(session)
{
    addField(PracticeNameField, Wt::WString::tr("practice-name-info"));
    addField(PracticeZipField, Wt::WString::tr("practice-zip-info"));
    addField(PracticeDrFirstName, Wt::WString::tr("practice-drfirstname-info"));
    addField(PracticeDrLastName, Wt::WString::tr("practice-drlastname-info"));
    addField(PracticeDrNpi, Wt::WString::tr("practice-npi-info"));
    addField(PracticeEmailAddress, Wt::WString::tr("practice-email-info"));
    addField(PracticeSpecialty, Wt::WString::tr("practice-specialty-info"));

    staticPrac_ = new Wt::WText();
    emailOpt_ = new Wt::WText();
    staticPrac_->setText(setPracId());
    emailOpt_->setText("Yes");


}

WString PracticeDetailsModel::setPracId()
{
    return uuid();
}

WString PracticeDetailsModel::strPracId()
{
    return staticPrac_->text();
}
WString PracticeDetailsModel::strEmailOpt()
{
    return emailOpt_->text();
}

void PracticeDetailsModel::save(const Wt::Auth::User& practice)
{

    Wt::Dbo::ptr<Practice> prac = session_.practice(practice);
    prac.modify()->refPracticeName_ = valueText(PracticeNameField).toUTF8();
    prac.modify()->refPracticeZip_ = valueText(PracticeZipField).toUTF8();
    prac.modify()->refPracticeDrFirst_ = valueText(PracticeDrFirstName).toUTF8();
    prac.modify()->refPracticeDrLast_ = valueText(PracticeDrLastName).toUTF8();
    prac.modify()->refPracticeNpi_ = valueText(PracticeDrNpi).toUTF8();
    prac.modify()->refPracticeEmail_ = valueText(PracticeEmailAddress).toUTF8();
    prac.modify()->refSpecialty_ = valueText(PracticeSpecialty).toUTF8();
    prac.modify()->pracEmailOpt_ = strEmailOpt().toUTF8();
    prac.modify()->practiceId_ = strPracId().toUTF8();


}
