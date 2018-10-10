/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/Dbo/Dbo>
#include "AdminHomeWidget.h"

AdminHomeWidget::AdminHomeWidget(WContainerWidget *parent) :
    WContainerWidget(parent)
{
        Wt::log("notice") << "Admin user switched routes to /home.";
        addWidget(new Wt::WText("<h3>Welcome to the Administrator Portal for United STAT Sleep</h3>"));
        addWidget(new Wt::WText("<h4>Click a link on the left to get started.</h4>"));

}
AdminHomeWidget::~AdminHomeWidget()
{
}
