/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef MARKETING_HOME_WIDGET_H_
#define MARKETING_HOME_WIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WText>

using namespace Wt;

class MarketingHomeWidget : public Wt::WContainerWidget
{
public:
    MarketingHomeWidget(Wt::WContainerWidget *parent = 0);
    ~MarketingHomeWidget();
};

#endif //MARKETING_HOME_WIDGET_H_
