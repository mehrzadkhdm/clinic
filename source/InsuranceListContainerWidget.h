/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef INSURANCE_LIST_CONTAINER_WIDGET
#define INSURANCE_LIST_CONTAINER_WIDGET

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WPushButton>

#include <Wt/WDate>
#include <Wt/WTime>

#include "Patient.h"
#include "UserSession.h"

using namespace Wt;
using namespace Wt::Dbo;

class UserSession;
class InsuranceListContainerWidget : public Wt::WContainerWidget
{
public:
    InsuranceListContainerWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    void showSearch(Wt::WString& curstr, int limit, int offset);

private:

    UserSession *session_;
    const char *conninfo_;
    void showDialog(Wt::WString ins);
    
    Wt::WPushButton *btnShowInsurance;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);

    }

};

#endif //INSURANCE_LIST_CONTAINER_WIDGET
