/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_PREMAP_WIDGET_H_
#define ADMIN_PREMAP_WIDGET_H_
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WGoogleMap>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WAbstractItemModel>
#include <Wt/WStackedWidget>
#include "InboundContainerWidget.h"
#include "PatientListContainerWidget.h"
#include "ReferralResultWidget.h"
#include "UserSession.h"
#include "Composer.h"
#include "AdminMapWidget.h"

using namespace Wt;

class UserSession;
class AdminPreMapWidget : public Wt::WContainerWidget
{
public:
    AdminPreMapWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
    void showMap();


private:
    UserSession *session_;
    Composer *composer_;
    const char *conninfo_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
    Wt::WStackedWidget *resultStack_;
    AdminMapWidget *adminMap;
    void showResult(Wt::WDateTime& startDate, Wt::WDateTime& endDate);
};

#endif //ADMIN_PREMAP_WIDGET_H
