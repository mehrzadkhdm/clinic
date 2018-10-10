/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef MARKETING_LOCALITY_LIST_WIDGET_H_
#define MARKETING_LOCALITY_LIST_WIDGET_H_

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
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WStackedWidget>
#include "UserSession.h"
#include "LocalityListContainerWidget.h"

using namespace Wt;

class UserSession;

class MarketingLocalityListWidget : public Wt::WContainerWidget
{
public:
    MarketingLocalityListWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    void localityList();
    void search(Wt::WString searchString);

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }
private:

    UserSession *session_;
    const char *conninfo_;

    Wt::WText *staticLocality_;

    Wt::WStackedWidget *resultStack_;
    LocalityListContainerWidget *locContainer;

    WString setLocalityId();

    Wt::WPushButton *btnAddLocality_;

    void addLocalityDialog();


};

#endif //MARKETING_LOCALITY_LIST_WIDGET_H_
