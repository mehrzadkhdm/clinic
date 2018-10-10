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
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
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
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WDate>
#include <Wt/WDateEdit>
#include <Wt/WStackedWidget>
#include <Wt/WGoogleMap>
#include <Wt/WStringListModel>
#include <Wt/WTemplate>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "GPSPath.h"
#include "GPSPathItem.h"
#include "AdminMapWidget.h"
#include "UserSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

AdminMapWidget::AdminMapWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session),
    conninfo_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<GPSPath>("gpspath");
    dbsession.mapClass<CompositeEntity>("composite");

    dbo::Transaction transaction(dbsession);
    try {
        dbsession.createTables();
        log("info") << "Database created";
    }
    catch (...) {
        log("info") << "Using existing database";
    }
    transaction.commit();

}

void AdminMapWidget::showMap(Wt::WDateTime start, Wt::WDateTime end)
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();
    container->setLayout(layout);
    container->setHeight(650);
                                                                                                   

    map_ = new Wt::WGoogleMap(Wt::WGoogleMap::Version3);
    layout->addWidget(map_, 1);

    map_->setMapTypeControl(Wt::WGoogleMap::DefaultControl);
    map_->enableScrollWheelZoom();

    Wt::WTemplate *controls =
        new Wt::WTemplate(tr("graphics-GoogleMap-controls"));
    layout->addWidget(controls);

    Wt::WPushButton *zoomIn = new Wt::WPushButton("+");
    zoomIn->addStyleClass("zoom");
    controls->bindWidget("zoom-in", zoomIn);

    zoomIn->clicked().connect(std::bind([=] () {
        map_->zoomIn();
    }));

    Wt::WPushButton *zoomOut = new Wt::WPushButton("-");
    zoomOut->addStyleClass("zoom");
    controls->bindWidget("zoom-out", zoomOut);

    zoomOut->clicked().connect(std::bind([=] () {
            map_->zoomOut();
    }));

    std::string cityNames[] = { "Downey", "LongBeach", "Irvine" };
    Wt::WGoogleMap::Coordinate cityCoords[] = {
        Wt::WGoogleMap::Coordinate(33.9401,-118.1332),
        Wt::WGoogleMap::Coordinate(33.7701,-118.1937),
        Wt::WGoogleMap::Coordinate(33.6839, -117.7947)
    };
        
    for (unsigned i = 0; i < 3; ++i) {
        Wt::WPushButton *city = new Wt::WPushButton(cityNames[i]);
        controls->bindWidget(cityNames[i], city);

            Wt::WGoogleMap::Coordinate coord = cityCoords[i];
        city->clicked().connect(std::bind([=] () {
        map_->panTo(coord);
        }));
    }

    Wt::WPushButton *reset = new Wt::WPushButton("Reset");
    controls->bindWidget("emweb", reset);

    reset->clicked().connect(std::bind([=] () {
            this->panToDowney();
        }));

    Wt::WPushButton *savePosition =
        new Wt::WPushButton("Save current position");
    controls->bindWidget("save-position", savePosition);

    savePosition->clicked().connect(std::bind([=] () {
            this->savePosition();
        }));

    returnToPosition_ = new Wt::WPushButton("Return to saved position");
    controls->bindWidget("return-to-saved-position", returnToPosition_);
    returnToPosition_->setEnabled(false);

    returnToPosition_->clicked().connect(std::bind([=] () {
            map_->returnToSavedPosition();
        }));

    mapTypeModel_ = new Wt::WStringListModel(this);
    addMapTypeControl("No control", Wt::WGoogleMap::NoControl);
    addMapTypeControl("Default", Wt::WGoogleMap::DefaultControl);
    addMapTypeControl("Menu", Wt::WGoogleMap::MenuControl);
    if (map_->apiVersion() == Wt::WGoogleMap::Version2)
        addMapTypeControl("Hierarchical",
                  Wt::WGoogleMap::HierarchicalControl);
    if (map_->apiVersion() == Wt::WGoogleMap::Version3)
        addMapTypeControl("Horizontal bar",
                  Wt::WGoogleMap::HorizontalBarControl);

    Wt::WComboBox* menuControls = new Wt::WComboBox();
    menuControls->setModel(mapTypeModel_);
    menuControls->setCurrentIndex(1);
    controls->bindWidget("control-menu-combo", menuControls);

    menuControls->activated().connect(std::bind([=] (int mapType) {
            this->setMapTypeControl(mapType);
        }, std::placeholders::_1));

    Wt::WCheckBox *draggingCB = new Wt::WCheckBox("Enable dragging");
    controls->bindWidget("dragging-cb", draggingCB);
    draggingCB->setChecked(true);
    map_->enableDragging();

    draggingCB->checked().connect(std::bind([=] () {
            map_->enableDragging();
        }));

    draggingCB->unChecked().connect(std::bind([=] () {
            map_->disableDragging();
        }));

    Wt::WCheckBox *enableDoubleClickZoomCB =
            new Wt::WCheckBox("Enable double click zoom");
    controls->bindWidget("double-click-zoom-cb", enableDoubleClickZoomCB);
    enableDoubleClickZoomCB->setChecked(false);
    map_->disableDoubleClickZoom();

    enableDoubleClickZoomCB->checked().connect(std::bind([=] () {
            map_->enableDoubleClickZoom();
    }));

    enableDoubleClickZoomCB->unChecked().connect(std::bind([=] () {
            map_->disableDoubleClickZoom();
        }));

    Wt::WCheckBox *enableScrollWheelZoomCB =
            new Wt::WCheckBox("Enable scroll wheel zoom");
    controls->bindWidget("scroll-wheel-zoom-cb", enableScrollWheelZoomCB);
    enableScrollWheelZoomCB->setChecked(true);
    map_->enableScrollWheelZoom();

    enableScrollWheelZoomCB->checked().connect(std::bind([=] () {
            map_->enableScrollWheelZoom();
        }));

    enableScrollWheelZoomCB->unChecked().connect(std::bind([=] () {
            map_->disableScrollWheelZoom();
        }));


    map_->clicked().connect(std::bind([=] (Wt::WGoogleMap::Coordinate c) {
            this->googleMapClicked(c);
        }, std::placeholders::_1));

    map_->doubleClicked().connect
        (std::bind([=] (Wt::WGoogleMap::Coordinate c) {
            this->googleMapDoubleClicked(c);
        }, std::placeholders::_1));

    //get the gpspaths from the database, populate vector and addMarker for each gps path.
    dbo::Transaction trans(dbsession);
    std::vector<GPSPathItem> cooritems;
    
    typedef dbo::collection< dbo::ptr<GPSPath> > RecentCoordinates;
    RecentCoordinates coordinates = dbsession.find<GPSPath>().where("created_at BETWEEN ? and ?").bind(start).bind(end);

    std::cerr << coordinates.size() << std::endl;

    for (RecentCoordinates::const_iterator i = coordinates.begin(); i != coordinates.end(); ++i)
    {
        //instance new log entry object here

        GPSPathItem *ent = new GPSPathItem();

        double lat = std::stod ((*i)->lat_.toUTF8());
        double lng = std::stod ((*i)->long_.toUTF8());
        
        Wt::WDateTime fixt = (*i)->createdAt_;
        Wt::WDateTime adjust = fixt.addSecs(-25200);
        WString t = adjust.toString("MM/dd/yyyy hh:mm a");
        WString prov = (*i)->provider_;

        ent->lat_ = lat;
        ent->lng_ = lng;
        ent->time_ = t;
        ent->provider = prov;

        cooritems.push_back(*ent);
    }

    for (std::vector<GPSPathItem>::iterator ien = cooritems.begin(); ien != cooritems.end(); ++ien)
    {
        Wt::WString showdt = ien->time_;
        map_->openInfoWindow(Wt::WGoogleMap::Coordinate(ien->lat_, ien->lng_), showdt);
    }
    
    container->addWidget(map_);
    map_->panTo(Wt::WGoogleMap::Coordinate(33.9401,-118.1332));
    addWidget(container);
    
}
