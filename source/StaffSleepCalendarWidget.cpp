/*
* Copyrefright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
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
#include <Wt/WMenuItem>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WDate>
#include <Wt/WTime>
#include <Wt/WTimer>

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/Utils>

//other header files
#include "StaffSleepCalendarWidget.h"
#include "Referral.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Composer.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"

#include "Calendar.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

StaffSleepCalendarWidget::StaffSleepCalendarWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo)
{

}

void StaffSleepCalendarWidget::monthView()
{

    curSelectedMonth = 0;
    curSelectedYear = 0;
    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    container->setStyleClass("cal-main-container");

    Wt::WDate today = Wt::WDate::currentServerDate();
    log("info") << "TODAYS DATE:" << today.toString("MM/dd/yyyy");

    std::stringstream monthss;
    monthss << today.month();
    log("info") << monthss.str();

    std::stringstream dayss;
    dayss << today.day();
    log("info") << "TODAYS DAY: " << dayss.str();

    std::stringstream yearss;
    yearss << today.year();
    log("info") << "TODAYS YEAR: "<< yearss.str();

    Wt::WText *downeyColor = new Wt::WText("Downey Sleep Center  ");
    Wt::WText *lbColor = new Wt::WText("Long Beach Sleep Center  ");

    Wt::WText *apptConfirmed = new Wt::WText("Appointment Confirmed");
    Wt::WText *apptUnconfirmed = new Wt::WText("Appointment Unconfirmed");

    apptConfirmed->setStyleClass("appt-confirmed-heading");
    apptUnconfirmed->setStyleClass("appt-unconfirmed-heading");

    lbColor->setStyleClass("lb-cal-color");

    currentlySelectedLocation_ = "Downey";
    downeyColor->setStyleClass("downey-cal-selected");
    downeyColor->clicked().connect(std::bind([=]() {
        currentlySelectedLocation_ = "Downey";
        downeyColor->setStyleClass("downey-cal-selected");
        lbColor->setStyleClass("lb-cal-color");
        StaffSleepCalendarWidget::triggerCalendar(currentlySelectedDate_);
    }));

    lbColor->clicked().connect(std::bind([=]() {
        currentlySelectedLocation_ = "Long Beach";
        lbColor->setStyleClass("lb-cal-selected");
        downeyColor->setStyleClass("downey-cal-color");
        StaffSleepCalendarWidget::triggerCalendar(currentlySelectedDate_);
    }));

    //show this month's calendar by default

    Wt::WComboBox *cb = new Wt::WComboBox(container);
    cb->setStyleClass("cal-month-selection");
    cb->addItem("January");
    cb->addItem("February");
    cb->addItem("March");
    cb->addItem("April");
    cb->addItem("May");
    cb->addItem("June");
    cb->addItem("July");
    cb->addItem("August");
    cb->addItem("September");
    cb->addItem("October");
    cb->addItem("November");
    cb->addItem("December");

    Wt::WComboBox *yearcb = new Wt::WComboBox(container);
    yearcb->setStyleClass("cal-year-selection");
    yearcb->addItem("2015");
    yearcb->addItem("2016");
    yearcb->addItem("2017");
    yearcb->addItem("2018");
    yearcb->addItem("2019");
    yearcb->addItem("2020");

    Wt::WText *out = new Wt::WText(container);
    out->addStyleClass("help-block");

    container->addWidget(apptUnconfirmed);
    container->addWidget(apptConfirmed);
    container->addWidget(downeyColor);
    container->addWidget(lbColor);

    container->addWidget(new Wt::WBreak());
    log("info") << "create new calStack";
    int curindex = today.month() - 1;
    cb->setCurrentIndex(curindex); // Show 'Today's Month Number index initially.
    cb->setMargin(10, Wt::Right);


    calStack_ = new Wt::WStackedWidget(container);

    currentlySelectedDate_.setDate(today.year(), today.month(), today.day());
    StaffSleepCalendarWidget::triggerCalendar(currentlySelectedDate_);

    cb->changed().connect(std::bind([=] () {

        int newIndex;
        int first = 1;
        newIndex = cb->currentIndex() + 1;
        std::stringstream newss;
        newss << newIndex;

        log("notice") << "newIndex: " << newss.str();
           log("info") << "setting calendar date to current index";
        Wt::WDate calendarDate;
           calendarDate.setDate(today.year(), newIndex, first);
         log("info") << "StaffCalendarWidget calendarDate: " << calendarDate.toString("MM/dd/yyyy");
         log("info") << "call trigger calendar, pass calendarDate";

        curSelectedMonth = newIndex;

        StaffSleepCalendarWidget::triggerCalendar(calendarDate);

        int whichyear;

        if (curSelectedYear == 0) {
            whichyear = today.year();
        } else if(curSelectedYear == today.year()){
            whichyear = today.year();
        } else {
            whichyear = curSelectedYear;
        }

        currentlySelectedDate_.setDate(whichyear, newIndex, today.day());

    }));

    yearcb->changed().connect(std::bind([=] () {

        int newIndex;
        newIndex = cb->currentIndex() + 1;
        std::stringstream newss;
        newss << newIndex;

        std::string str_year = yearcb->currentText().toUTF8();
        log("notice") << "str_year: " << str_year;
        std::string::size_type sz;

        int year_dec = std::stoi(str_year, &sz);
        std::stringstream syear;
        syear << year_dec;
        log("notice") << "year_dec" << syear.str();

        log("notice") << "newIndex: " << newss.str();
           log("info") << "setting calendar date to current index";
        Wt::WDate calendarDate;
           calendarDate.setDate(year_dec, newIndex, today.day());
         log("info") << "StaffCalendarWidget calendarDate: " << calendarDate.toString("MM/dd/yyyy");
         log("info") << "call trigger calendar, pass calendarDate";

        curSelectedYear = year_dec;

        StaffSleepCalendarWidget::triggerCalendar(calendarDate);

        int whichmonth;

        if (curSelectedMonth == 0) {
            whichmonth = today.month();
        } else if(curSelectedMonth == today.month()){
            whichmonth = today.month();
        } else {
            whichmonth = curSelectedMonth;
        }

        currentlySelectedDate_.setDate(year_dec, whichmonth, today.day());

    }));

    addWidget(container);

    refreshTimer = new Wt::WTimer();
    refreshTimer->setInterval(30000);
    refreshTimer->timeout().connect(std::bind([=] () {
        log("info") << "Timer Refresh Signal Sent";
        StaffSleepCalendarWidget::triggerCalendar(currentlySelectedDate_);
    }));

    refreshTimer->start();


}
void StaffSleepCalendarWidget::preTriggerCalendar(Wt::WDate calendarDate)
{
    StaffSleepCalendarWidget::triggerCalendar(calendarDate);
}

void StaffSleepCalendarWidget::preTriggerStartTimer(Wt::WDate calendarDate)
{
    StaffSleepCalendarWidget::triggerCalendar(calendarDate);
    refreshTimer->start();
}
void StaffSleepCalendarWidget::stopRefreshTimer()
{
    refreshTimer->stop();

}
void StaffSleepCalendarWidget::triggerCalendar(Wt::WDate calendarDate)
{
        calStack_->clear();
           log("info") << "create new CalendarContainer, pass calStack_ as parent";
           calendarContainer = new CalendarContainerWidget(conninfo_, session_, calStack_);
           log("info") << "run showCalendar() and pass calendarDate";
           calendarContainer->showCalendar(currentlySelectedLocation_, calendarDate);
           log("info") << "set current widget to CalendarContainer";
        calStack_->setCurrentWidget(calendarContainer);

        calendarContainer->stopTimer().connect(std::bind([=] () {
            log("info") << "Caught stopTimer Signal";
            StaffSleepCalendarWidget::stopRefreshTimer();
        }));

        calendarContainer->done().connect(std::bind([=] () {
            log("info") << "Caught done Signal";
            StaffSleepCalendarWidget::preTriggerCalendar(currentlySelectedDate_);
        }));
}
