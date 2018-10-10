/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_H_
#define PRACTICE_H_

#include <Wt/Auth/User>
#include <Wt/Dbo/Types>
#include <Wt/WString>
#include <Wt/WDateTime>
#include <Wt/WTime>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

class Practice;
typedef Wt::Auth::Dbo::AuthInfo<Practice> PracAuthInfo;

class Practice : public Wt::Auth::User {
public:
    /* You probably want to add other practice information here */
    std::string refPracticeName_;
    std::string refPracticeZip_;
    std::string refPracticeDrFirst_;
    std::string refPracticeDrLast_;
    std::string refPracticeNpi_;
    std::string refPracticeEmail_;
    std::string refSpecialty_;
    std::string pracEmailOpt_;
    std::string clinicoreAccount_;
    std::string addressId_;
    std::string pccEmail_;
    std::string localityName_;
    std::string localityId_;
    std::string practiceId_;
    std::string isGroup_;
    std::string groupId_;
    std::string addrLine2_;
    std::string officeContact_;
    Wt::WTime officeOpen_;
    Wt::WTime officeClose_;
    Wt::WTime officeLunchStart_;
    Wt::WTime officeLunchEnd_;
    Wt::WDateTime dateAdded_;
    Wt::WString pracFax_;
    Wt::WString kaiser_;

    template<class Action>
    void persist(Action& a)
    {
        dbo::field(a, refPracticeName_, "prac_name");
        dbo::field(a, refPracticeZip_, "prac_zip");
        dbo::field(a, refPracticeDrFirst_, "prac_drfirst");
        dbo::field(a, refPracticeDrLast_, "prac_drlast");
        dbo::field(a, refPracticeNpi_, "prac_npi");
        dbo::field(a, refPracticeEmail_, "prac_email");
        dbo::field(a, refSpecialty_, "prac_specialty");
        dbo::field(a, pracEmailOpt_, "prac_opt");
        dbo::field(a, clinicoreAccount_, "clinicore_account");
        dbo::field(a, addressId_, "address_id");
        dbo::field(a, pccEmail_, "pcc_email");
        dbo::field(a, localityName_, "local_name");
        dbo::field(a, localityId_, "local_id");
        dbo::field(a, officeContact_, "office_contact");
        dbo::field(a, officeOpen_, "office_open");
        dbo::field(a, officeClose_, "office_close");
        dbo::field(a, officeLunchStart_, "lunch_start");
        dbo::field(a, officeLunchEnd_, "lunch_end");
        dbo::field(a, dateAdded_, "date_added");
        dbo::field(a, isGroup_, "is_group");
        dbo::field(a, kaiser_, "kaiser");
        dbo::field(a, addrLine2_, "addr_line2");
        dbo::field(a, groupId_, "group_id");
        dbo::field(a, pracFax_, "prac_fax");
        dbo::field(a, practiceId_, "prac_id");
    }
};

#endif // PRACTICE_H_
