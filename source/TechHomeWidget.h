/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef TECH_HOME_WIDGET_H_
#define TECH_HOME_WIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WText>

using namespace Wt;

class TechHomeWidget : public Wt::WContainerWidget
{
public:
    TechHomeWidget(Wt::WContainerWidget *parent = 0);
};

#endif //TECH_HOME_WIDGET_H
