/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STAFF_REPORT_WIDGET_H_
#define STAFF_REPORT_WIDGET_H_
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

using namespace Wt;


class StaffReportWidget : public Wt::WContainerWidget
{
public:
    StaffReportWidget(const char *conninfo, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    Wt::WLineEdit *repStaffNameEdit_;
    Wt::WPushButton *btnStaffComplete;
    Wt::WPushButton *btnStaffComplete1;
    Wt::WPushButton *btnRemoveReport;
    Wt::WText *staffDone;
    WString *strStaffComplete();
    void reportList();
    void saveStaffName(WText staffName, WString repId);
private:
    Wt::WString strT(WTemplate& tem);



};

#endif //STAFF_REPORT_WIDGET_H
