/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_MAP_WIDGET_H_
#define ADMIN_MAP_WIDGET_H_
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


using namespace Wt;

class UserSession;
class AdminMapWidget : public Wt::WContainerWidget
{
public:
    AdminMapWidget(const char *conninfo, UserSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;
    void showMap(Wt::WDateTime start, Wt::WDateTime end);


private:
    UserSession *session_;
    Composer *composer_;
    const char *conninfo_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }


private:
    Wt::WGoogleMap *map_;
    Wt::WAbstractItemModel *mapTypeModel_;
    
    void addMapTypeControl(const Wt::WString &description,
               Wt::WGoogleMap::MapTypeControl value) {
    int r = mapTypeModel_->rowCount();
    mapTypeModel_->insertRow(r);
    mapTypeModel_->setData(r, 0, description);
    mapTypeModel_->setData(r, 0, value, Wt::UserRole);
    }

    void setMapTypeControl(int row) {
    boost::any mtc = mapTypeModel_->data(row, 0, Wt::UserRole);
    map_->setMapTypeControl(boost::any_cast<Wt::WGoogleMap::MapTypeControl>
                (mtc));
    }
    
    Wt::WPushButton *returnToPosition_;
    
    void panToDowney() {
        map_->panTo(Wt::WGoogleMap::Coordinate(33.9401, -118.1332));
    }

    void savePosition() {
        returnToPosition_->setEnabled(true);
        map_->savePosition();
    }


    void googleMapDoubleClicked(Wt::WGoogleMap::Coordinate c) {
    std::cerr << "Double clicked at coordinate ("
          << c.latitude() << "," << c.longitude() << ")";
    }

    void googleMapClicked(Wt::WGoogleMap::Coordinate c) {
    std::cerr << "Clicked at coordinate ("
          << c.latitude() << "," << c.longitude() << ")";
    }
};

#endif //ADMIN_MAP_WIDGET_H
