/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_HOME_WIDGET_H_
#define PRACTICE_HOME_WIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WText>

using namespace Wt;

class PracticeHomeWidget : public Wt::WContainerWidget
{
public:
    PracticeHomeWidget(Wt::WContainerWidget *parent = 0);
    void hello();
};

#endif //PRACTICE_HOME_WIDGET_H
