/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ADMIN_HOME_WIDGET_H_
#define ADMIN_HOME_WIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WText>

using namespace Wt;

class AdminHomeWidget : public Wt::WContainerWidget
{
public:
    AdminHomeWidget(Wt::WContainerWidget *parent = 0);
    ~AdminHomeWidget();
};

#endif //ADMIN_HOME_WIDGET_H
