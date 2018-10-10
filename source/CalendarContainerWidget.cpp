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
#include <stdlib.h>
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

#include <Wt/WSignal>
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
#include <Wt/WTimer>


#include "CalendarContainerWidget.h"
#include "Calendar.h"
#include "Composer.h"
#include "CalendarEvent.h"
//other header files
#include "StaffSleepStudyWidget.h"
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
#include "Insurance.h"
#include "InsuranceItem.h"
#include "SleepStudyListContainerWidget.h"
#include "StudyType.h"
#include "StudyTypeItem.h"
//rest http library
#include "restclient-cpp/restclient.h"

CalendarContainerWidget::CalendarContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo),
    done_(this)
{

    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<StudyType>("studytype");
    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<Comment>("comment");
    dbsession.mapClass<Insurance>("insurance");


    dbsession.mapClass<Comment>("sleepstudy");

    dbo::Transaction strans(dbsession);
    try {
        dbsession.createTables();
        log("info") << "Database created";
    }
    catch (...) {
        log("info") << "Using existing database";
    }
    strans.commit();

}

void CalendarContainerWidget::showCalendar(Wt::WString &location, Wt::WDate &calendarDate)
{

            
    Wt::WDate today = Wt::WDate::currentServerDate();

    std::stringstream todayyear;
    todayyear << today.year();


    Wt::WDate startdate;
    startdate.setDate(calendarDate.year(), calendarDate.month(), 1);
    log("notice") << "startdatestr: " << startdate.toString("MM/dd/yyyy");


    Wt::WDate firstOfNextMonth;

    int year = startdate.year();
    int month = startdate.month() + 1;

    if (month > 12)
    {
        month = 1;
        year++;

    }

    firstOfNextMonth.setDate(year, month, 1);



    Wt::WDate enddate;
    enddate = firstOfNextMonth.addDays(-1);


    log("notice") << "SQL START DATE: " << startdate.toString("MM/dd/yyyy");
    log("notice") << "SQL END DATE: " << enddate.toString("MM/dd/yyyy");

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<SleepStudy> > SleepStudies;
    SleepStudies sleepstudies = dbsession.find<SleepStudy>().where("std_loco = ?").bind(location).where("std_date BETWEEN ? AND ?").bind(startdate).bind(enddate);
    std::cerr << sleepstudies.size() << std::endl;

    std::vector<CalendarEvent> events;

    for (SleepStudies::const_iterator i = sleepstudies.begin(); i != sleepstudies.end(); ++i)
    {

        Wt::WString d = (*i)->studyDate_.toString("MM/dd/yyyy");
        Wt::WString t = (*i)->studyTime_;
        Wt::WString loc = (*i)->studyLocation_;
        Wt::WString sid = (*i)->studyId_;
        Wt::WString apptstat = (*i)->appointmentStatus_;
        Wt::WString stdtype = (*i)->studyTypeName_;

        Wt::WDate appointmentDate;
        appointmentDate = Wt::WDate().fromString(d, "MM/dd/yyyy");

        Wt::WTime appointmentTime;
        appointmentTime = Wt::WTime().fromString(t, "h:m a");

        Wt::WString displayName;

        displayName = (*i)->ptFirstName_ + " " + (*i)->ptLastName_;

        CalendarEvent *e = new CalendarEvent();
        e->displayName = displayName;
        e->appointmentDate = appointmentDate;
        e->appointmentTime = appointmentTime;
        e->studyId = sid;
        e->location = loc;
        e->appointmentStatus = apptstat;
        e->studyType = stdtype;

        events.push_back(*e);

    }
    log("notice") << "Current Number of EVENTS from Sleep Studies"<< events.size();

    transaction.commit();

    calendarOutside = new Wt::WContainerWidget();
    calendarOutside->setPadding(-1, All);

    calendar = new Calendar(startdate, &events);
    calendarOutside->setStyleClass("calendar-outside");

    Wt::WVBoxLayout *weeksvbox = new Wt::WVBoxLayout();
    weeksvbox->setSpacing(-1);
    weeksvbox->addSpacing(-9);

    calendarOutside->setLayout(weeksvbox);

    for (int w = 0; w < 6; w++)
    {
        CalendarWeek *curweek = calendar->weeks[w];
        curweek->container = new Wt::WContainerWidget();
        curweek->container->setStyleClass("week-container");
        curweek->container->setPadding(-1, All);

        weeksvbox->addWidget(curweek->container);
        curweek->dayshbox = new Wt::WHBoxLayout();
        curweek->dayshbox->setSpacing(-1);


        curweek->container->setLayout(curweek->dayshbox);

        for (int d = 0; d < 7; d++ )
        {
            CalendarDay *curday = curweek->days[d];
            curday->container = new Wt::WContainerWidget();
            curday->container->setStyleClass("day-container");
            curday->container->setPadding(-1);


            if (curday->shouldBeRendered())
            {
                curday->container->setStyleClass("day-container day-show");
                Wt::log("notice") << "Day should be rendered.";
                std::stringstream ss;
                ss << curday->curDate.day();
                curday->container->addWidget(new Wt::WText(ss.str()));
                curday->evcontainer = new Wt::WContainerWidget();
                curday->evbox = new Wt::WVBoxLayout();
                curday->evcontainer->setLayout(curday->evbox);

                std::vector<CalendarEvent> dayevents;

                for (std::vector<CalendarEvent>::iterator ev = events.begin(); ev != events.end(); ++ev)
                {


                    if (ev->appointmentDate == curday->curDate)
                    {
                        log("notice") << "Event should be rendered: " << ev->displayName;
                        dayevents.push_back(*ev);

                        if (ev->location == "Long Beach" && dayevents.size() < 3)
                        {
                            curday->container->setStyleClass("day-container day-show beds-not-full");

                        } else if (ev->location == "Downey" && dayevents.size() < 6) {
                            curday->container->setStyleClass("day-container day-show beds-not-full");

                        } else if (ev->location == "Long Beach" && dayevents.size() >= 3) {

                            curday->container->setStyleClass("day-container day-show beds-full");

                        } else if (ev->location == "Downey" && dayevents.size() >= 6) {

                            curday->container->setStyleClass("day-container day-show beds-full");
                        }

                        if (ev->appointmentStatus != "Confirmed")
                        {

                            curday->evlb = new Wt::WContainerWidget();

                            curday->evlb->setStyleClass("cal-event-unconfirmed");
                            curday->evlb->addWidget(new Wt::WText(ev->displayName));
                            curday->evlb->addWidget(new Wt::WBreak());
                            curday->evlb->addWidget(new Wt::WText(ev->appointmentTime.toString("h:mm a")));
                            curday->evlb->addWidget(new Wt::WBreak());
                            curday->evlb->addWidget(new Wt::WText(ev->studyType));

                            curday->evlb->clicked().connect(boost::bind(&CalendarContainerWidget::showStudyDialog, this, ev->studyId));

                            curday->evbox->addWidget(curday->evlb);


                        } else if (ev->appointmentStatus == "Confirmed") {

                            curday->evdny = new Wt::WContainerWidget();

                            curday->evdny->setStyleClass("cal-event-confirmed");
                            curday->evdny->addWidget(new Wt::WText(ev->displayName));
                            curday->evdny->addWidget(new Wt::WBreak());
                            curday->evdny->addWidget(new Wt::WText(ev->appointmentTime.toString("h:mm a")));
                            curday->evdny->addWidget(new Wt::WBreak());
                            curday->evdny->addWidget(new Wt::WText(ev->studyType));
                            curday->evdny->clicked().connect(boost::bind(&CalendarContainerWidget::showStudyDialog, this, ev->studyId));



                            curday->evbox->addWidget(curday->evdny);

                        } else {

                            curday->evcontainer->setStyleClass("cal-event-noloc");

                        }
                    }

                    curday->container->addWidget(curday->evcontainer);
                    Wt::log("notice") << "day number" << ss.str();
                }

            }
            curweek->dayshbox->addWidget(curday->container);
        }
    }
    Wt::WContainerWidget *top = new Wt::WContainerWidget();

    Wt::WString thisMonth = Wt::WDate::longMonthName(calendarDate.month());
    //put the year in a string
    std::stringstream yearss;
    yearss << calendarDate.year();

    Wt::WString thisYear = yearss.str();

    Wt::WText *topMonth = new Wt::WText(thisMonth);
    Wt::WText *topYear = new Wt::WText(thisYear);

    topMonth->setStyleClass("cal-top-month");
    topYear->setStyleClass("cal-top-year");
    top->addWidget(topMonth);
    top->addWidget(topYear);

    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());
    addWidget(top);
    
    log("info") << "BEFORE ARRAY";
    std::array<std::string, 7> weekdays = { std::string("Sunday"),"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
    log("info") << "AFTER ARRAY";
    
    Wt::WContainerWidget *weekd = new Wt::WContainerWidget();
    weekd->addStyleClass("weekdaybox");
    Wt::WHBoxLayout *weekdayhbox = new Wt::WHBoxLayout();
    weekd->setLayout(weekdayhbox);
    log("info") << "BEFORE LOOP";

    for(int w=0; w<7; w++) {
        log("info") << "LOOP ITERATION";

        Wt::log("info") << "weekday: " << weekdays[w];
        weekdayhbox->addWidget(new Wt::WText(Wt::WString(weekdays[w])));
        log("info") << "END LOOP ITERATION";

    }
    log("info") << "AFTER LOOP";

    addWidget(weekd);    

    addWidget(new Wt::WBreak());
    addWidget(calendarOutside);
}
WString CalendarContainerWidget::setStdId()
{
    return uuid();
}

WString CalendarContainerWidget::setCommentId()
{
    return uuid();
}
void CalendarContainerWidget::showStudyDialog(Wt::WString std)
{
    //show sleep study dialog
    stopTimer_.emit(42, "Stop Timer"); // emit the stop timer signal

    dbo::Transaction getstudy(dbsession);
    dbo::ptr<SleepStudy> stdy = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);

    WString ref = stdy->referralId_;

    staticStd_ = new Wt::WText();
    staticStd_->setText(setStdId());

    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Sleep Study Referral");

    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
    Wt::WContainerWidget *refcontainer = new Wt::WContainerWidget();
    Wt::WContainerWidget *stdcontainer = new Wt::WContainerWidget();

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    refcontainer->setLayout(hbox);
    refcontainer->addStyleClass("attcont");

    dbo::Transaction transaction(dbsession);

    dbo::ptr<Referral> referral = dbsession.find<Referral>().where("ref_id = ?").bind(ref);

    WString pname = referral->refPracticeName_;
    WString pracid = referral->refPracticeId_;
    WString pzip = referral->refPracticeZip_;
    WString pdrfirst = referral->refPracticeDrFirst_;
    WString pdrlast = referral->refPracticeDrLast_;
    WString pnpi = referral->refPracticeNpi_;
    WString pemail = referral->refPracticeEmail_;
    WString pspec = referral->refSpecialty_;

    WString ptfirst = referral->refFirstName_;
    WString ptlast = referral->refLastName_;
    WString ptaddr1 = referral->refAddress1_;
    WString ptaddr2 = referral->refAddress2_;
    WString ptcity = referral->refCity_;
    WString ptzip = referral->refZip_;
    WString pthome = referral->refHomePhone_;
    WString ptcell = referral->refCellPhone_;
    WString ptdob = referral->refDob_;
    WString ptgender = referral->refGender_;
    WString rdiag = referral->refDiagnostic_;
    WString rover = referral->refOvernight_;
    WString rhome = referral->refHomeStudy_;
    WString rother = referral->refOtherSleepTest_;
    WString rauth = referral->refAuthConsult_;
    WString status = referral->refStatus_;

    WString signame = referral->enteredDrFullName_;
    WString sigemail = referral->enteredDrEmail_;
    WString sigdate = referral->selectedDate_;

    Wt::WVBoxLayout *rvbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *rvbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(rvbox1);
    hbox->addLayout(rvbox2);

    Wt::WContainerWidget *refleft = new Wt::WContainerWidget(dialog->contents());
    refleft->addStyleClass("refdiagrefleft");
    refleft->addWidget(new Wt::WText("<h4>Referring Practice Info</h4>"));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Name: " + pname));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Zip: " + pzip));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Dr Full Name: " + pdrfirst + " " + pdrlast));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice NPI: " + pnpi));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Specialty: " + pspec));
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("Practice Email: " + pemail));


    Wt::WContainerWidget *refright = new Wt::WContainerWidget(dialog->contents());
    refright->addStyleClass("refdiagrefright");
    refright->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Address: <br /></strong>" + ptaddr1));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText(ptaddr2));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient City: </strong>" + ptcity));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Zip: </strong>" + ptzip));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Home Phone: </strong>" + pthome));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Cell Phone: </strong>" + ptcell));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<h4>Referral Diagnosis</h4> "));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Diagnosis: </strong>" + rdiag));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Overnight Study: </strong>" + rover));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Other Study: </strong>" + rother));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Home Study: </strong>" + rhome));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<strong>Patient Consultation: </strong><br /> " +rauth));
    refright->addWidget(new Wt::WBreak());
    refright->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    refright->addWidget(new Wt::WText("Current Status:"+ status));


    //put table here
    dbo::Transaction atrans(dbsession);

    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(ref);
    std::cerr << attachments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Download"));

    int row = 1;
    for (Attachments::const_iterator i = attachments.begin(); i != attachments.end(); ++i, ++row)
    {
        Wt::WResource *attc = new AttachmentResource((*i)->attId_, (*i)->tempRefId_, (*i)->attFileName_, (*i)->attContentDescription_, this);

        new Wt::WText((*i)->attFileName_, table->elementAt(row, 0)),
        downDone = new Wt::WText("Downloaded", table->elementAt(row, 1)),
        downDone->hide(),
        btnDownloadAtt = new Wt::WPushButton("Download", table->elementAt(row, 1)),
        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WTemplate *t = new Wt::WTemplate(Wt::WString::tr("template.referral"));

    t->bindString("pname", Wt::WString(pname));
    t->bindString("pzip", Wt::WString(pzip));
    t->bindString("pdrfirst", Wt::WString(pdrfirst));
    t->bindString("pdrlast", Wt::WString(pdrlast));
    t->bindString("pnpi", Wt::WString(pnpi));
    t->bindString("pemail", Wt::WString(pemail));
    t->bindString("pspec", Wt::WString(pspec));
    t->bindString("ptfirst", Wt::WString(ptfirst));
    t->bindString("ptlast", Wt::WString(ptlast));
    t->bindString("ptaddr1", Wt::WString(ptaddr1));
    t->bindString("ptaddr2", Wt::WString(ptaddr2));
    t->bindString("ptcity", Wt::WString(ptcity));
    t->bindString("ptzip", Wt::WString(ptzip));
    t->bindString("pthome", Wt::WString(pthome));

    t->bindString("ptcell", Wt::WString(ptcell));
    t->bindString("ptdob", Wt::WString(ptdob));
    t->bindString("ptgender", Wt::WString(ptgender));
    t->bindString("rdiag", Wt::WString(rdiag));
    t->bindString("rover", Wt::WString(rover));
    t->bindString("rhome", Wt::WString(rhome));
    t->bindString("rother", Wt::WString(rother));
    t->bindString("rauth", Wt::WString(rauth));

    t->bindString("signame", Wt::WString(signame));
    t->bindString("sigemail", Wt::WString(sigemail));
    t->bindString("sigdate", Wt::WString(sigdate));

    std::ostringstream result;
    t->renderTemplate(result);
    Wt::WString s = result.str();

    /*
    WPushButton *pdfBtn = new Wt::WPushButton("Download PDF");
    Wt::WResource *pdf = new ReferralResource(this);
    pdfBtn->setLink(pdf);
    */

    //had to uncomment this because HPDF isnt working at the moment


    Wt::WResource *pdf = new ReferralResource(s, this);

    Wt::WPushButton *button = new Wt::WPushButton("Download Referral PDF");
    button->setLink(pdf);

    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(new Wt::WText("<h4>Attachments</h4>"));
    refleft->addWidget(table);
    refleft->addWidget(new Wt::WBreak());
    refleft->addWidget(button);
    refleft->addWidget(new Wt::WBreak());

    rvbox1->addWidget(refleft);
    rvbox2->addWidget(refright);
    transaction.commit();

    dbo::Transaction strans(dbsession);

    dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);


    WString sptfirst = study->ptFirstName_;
    WString sptlast = study->ptLastName_;
    WString sptaddr1 = study->ptAddress1_;
    WString sptaddr2 = study->ptAddress2_;
    WString sptcity = study->ptCity_;
    WString sptzip = study->ptZip_;
    WString spthome = study->ptHomePhone_;
    WString sptcell = study->ptCellPhone_;
    WString sptdob = study->ptDob_;
    WString sptgender = study->ptGender_;

    WString stdiag = study->studyDiagnostic_;
    WString stover = study->studyOvernight_;
    WString sthome = study->studyHomeStudy_;
    WString stother = study->studyOtherSleepTest_;
    WString stauth = study->studyAuthConsult_;
    WString stid = study->studyId_;
    WString stdtype = study->studyTypeName_;
    WString ptout = study->patientId_;
    std::stringstream ss;
    ss << ptout;

    std::string pt = ss.str();

    WString ststatus = study->studyStatus_;
    WString stdate = study->studyDate_.toString("MM/dd/yyyy");
    WString sttime = study->studyTime_;
    WString stloco = study->studyLocation_;
    WString apptstatus = study->appointmentStatus_;
    WString nststatus = "Re-Scheduled";
    WString compststatus = "Study Complete";
    WString archivestatus = "Study Archived";

    Wt::WHBoxLayout *shbox = new Wt::WHBoxLayout();
    stdcontainer->setLayout(shbox);
    stdcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *svbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *svbox1 = new Wt::WVBoxLayout();


    shbox->addLayout(svbox1);
    shbox->addLayout(svbox2);

    Wt::WContainerWidget *stdleft = new Wt::WContainerWidget(dialog->contents());
    stdleft->addStyleClass("stddiagstdright");
    stdleft->addWidget(new Wt::WText("<h4>Patient Info</h4>"));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient First Name: </strong>" + ptfirst));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Last Name: </strong>" + ptlast));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Date of Birth: </strong>" + ptdob));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<strong>Patient Gender: </strong>" + ptgender));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("<h4>Sleep Study Details</h4> "));
    stdleft->addWidget(new Wt::WText("Current Status:"+ status));
    stdleft->addWidget(new Wt::WBreak());
    stdleft->addWidget(new Wt::WText("Current Study Type:"+ stdtype));
    stdleft->addWidget(new Wt::WBreak());
    //get study type and ability to change study type
    Wt::WContainerWidget *typeContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblStudyType = new Wt::WLabel("Change Study Type:");
    lblStudyType->setStyleClass("sig-lbl");

    Wt::WHBoxLayout *typebox = new Wt::WHBoxLayout();
    typebox->addWidget(lblStudyType);

    dbo::Transaction typetrans(dbsession);

    typedef dbo::collection< dbo::ptr<StudyType> > StudyTypes;
    StudyTypes types = dbsession.find<StudyType>();
    std::cerr << types.size() << std::endl;

    std::vector<StudyTypeItem> typeitems;

    for (StudyTypes::const_iterator i = types.begin(); i != types.end(); ++i)
    {

        Wt::WString name = (*i)->studyTypeName_;
        Wt::WString code = (*i)->studyTypeCode_;
        Wt::WString id = (*i)->studyTypeId_;


        StudyTypeItem *e = new StudyTypeItem();
        e->studyTypeName = name;
        e->studyTypeCode = code;
        e->studyTypeId = id;


        typeitems.push_back(*e);

    }
    log("notice") << "Current Number of type items "<< typeitems.size();

    typetrans.commit();



    Wt::WComboBox *cbtype = new Wt::WComboBox();
    cbtype->setStyleClass("sig-left-box");
    Wt::WText *typeout = new Wt::WText();
    cbtype->addItem("");

    for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
    {
        cbtype->addItem(ev->studyTypeName);
    }

    cbtype->changed().connect(std::bind([=] () {

        StudyTypeItem selected;

        for (std::vector<StudyTypeItem>::const_iterator ev = typeitems.begin(); ev != typeitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cbtype->currentText();

            if (ev->studyTypeName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.studyTypeName != "")
        {

            //update sleep study here
            dbo::Transaction stypetrans(dbsession);
            
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyTypeName_ = selected.studyTypeName.toUTF8();
            st.modify()->studyTypeCode_ = selected.studyTypeCode.toUTF8();
            st.modify()->studyTypeId_ = selected.studyTypeId.toUTF8();
            
            stypetrans.commit();
        }

    }));

    cbtype->setCurrentIndex(0);
    cbtype->setMargin(10, Wt::Right);

    typebox->addWidget(cbtype);
    typeContainer->setLayout(typebox);
    
    stdleft->addWidget(typeContainer);

    Wt::WContainerWidget *stdright = new Wt::WContainerWidget(dialog->contents());

    stdright->addStyleClass("stddiagstdleft");
    stdright->addWidget(new Wt::WText("<h4>Study Info</h4>"));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Sleep Study Date: " + stdate));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Appointment Time: " + sttime));
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WText("Appointment Location: " + stloco));
    stdright->addWidget(new Wt::WBreak());

    Wt::WPushButton *newstudy = new Wt::WPushButton("Re-Schedule Study");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(newstudy);

    Wt::WPushButton *confirmAppt = new Wt::WPushButton("Appointment Confirmed");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(confirmAppt);
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());

    Wt::WPushButton *completestudy = new Wt::WPushButton("Send MD Email - Study Complete");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(completestudy);

    Wt::WPushButton *archivestudy = new Wt::WPushButton("Archive Study");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(archivestudy);

    Wt::WPushButton *patientforms = new Wt::WPushButton("Create Forms Packet");
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(new Wt::WBreak());
    stdright->addWidget(patientforms);

    Wt::WPushButton *preformdl = new Wt::WPushButton("Download Pre Sleep Packet");
    Wt::WPushButton *postformdl = new Wt::WPushButton("Download Post Sleep Packet");
    if (apptstatus != "Checked Out")
    {
        preformdl->hide();
        postformdl->hide();

    } else {

        preformdl->show();
        postformdl->show();

    }

    if (apptstatus == "Confirmed" || apptstatus =="Checked Out")
    {
        confirmAppt->hide();

    } else {

        confirmAppt->show();
    }

    //download pre-sleep forms
    std::string presleepformurl;
    presleepformurl = "http://lab.unitedsleep.local/forms/presleep/"+stid.toUTF8();
    preformdl->setLink(presleepformurl);

    //download post-sleep forms
    std::string postsleepformurl;
    postsleepformurl = "http://lab.unitedsleep.local/forms/postsleep/"+stid.toUTF8();
    postformdl->setLink(postsleepformurl);

    completestudy->clicked().connect(std::bind([=](){
            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyStatus_ = compststatus.toUTF8();
            strans.commit();

            dbo::Transaction pctrans2(dbsession);

            dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);

            WString poptmail = pc->pracEmailOpt_;
            //send email to practice
               pctrans2.commit();


               if (poptmail == "Yes") {
                Mail::Message message;
                message.setFrom(Mail::Mailbox("info@statsleep.com", "United STAT Sleep"));
                message.addRecipient(Mail::To, Mail::Mailbox(pemail.toUTF8(), pname.toUTF8()));
                message.setSubject("A Sleep Study is Complete");
                message.setBody("You referred a patient to our sleep center, and thier sleep study is complete. We are preparing the interpretation report now, you will receive an email with a link to download the report via a secure web-based service (fileon.com). Please login to your physician account to check details.");
                message.addHtmlBody ("<p>Hi, "+pname.toUTF8()+"<br>You referred a patient to our sleep center, and thier sleep study is complete.<br>  We are preparing the interpretation report now, you will receive an email with a link to download the report via <a href=\"https://fileon.com\">a secure web-based service.</a><br><br> Please <a href=\"https://127.0.0.1/providers\">Log In to your Physician Account</a> to check details.</p><br><p>Thanks,</p><p>The United STAT Sleep Team</p>");
                Mail::Client client;
                client.connect("localhost");
                client.send(message);
               }

            dialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

    }));

    confirmAppt->clicked().connect(std::bind([=](){

            Wt::WString confirmStatus = "Confirmed";

            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->appointmentStatus_ = confirmStatus.toUTF8();
            strans.commit();

            dialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

    }));

    archivestudy->clicked().connect(std::bind([=](){
            dbo::Transaction strans(dbsession);
            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);
            st.modify()->studyStatus_ = archivestatus.toUTF8();
            strans.commit();
            dialog->accept();
            this->refresh();

    }));

    patientforms->clicked().connect(std::bind([=]() {

        //create string vars for url, ctype, data

        std::string questionairePatientName;
        std::string questionairePrimaryComplaint;
        std::string questionaireClinicoreId;
        std::string questionairePatientDob;
        std::string questionaireStudyDate;
        std::string questionaireStudyStatus;

        std::string finalPatientName;
        std::string finalPrimaryComplaint;
        std::string finalClinicoreId;
        std::string finalPatientDob;
        std::string finalStudyDate;
        std::string finalStudyStatus;

        questionairePrimaryComplaint = "";
        questionairePatientName = ptfirst.toUTF8()+" "+ptlast.toUTF8();
        questionaireClinicoreId = stid.toUTF8();
        questionairePatientDob = sptdob.toUTF8();
        questionaireStudyDate = stdate.toUTF8();
        questionaireStudyStatus = "incomplete";



        finalPatientName = "\""+questionairePatientName+"\"";
        finalPrimaryComplaint = "\""+questionairePrimaryComplaint+"\"";
        finalClinicoreId = "\""+questionaireClinicoreId+"\"";
        finalPatientDob = "\""+questionairePatientDob+"\"";
        finalStudyDate = "\""+questionaireStudyDate+"\"";
        finalStudyStatus = "\""+questionaireStudyStatus+"\"";

        std::string url;
        std::string ctype;
        std::string data;

        url = "http://lab.statsleep.dev/api/v1/questionaires.json";
        ctype = "application/json";
        data =     "{\"patientName\": "+finalPatientName+","+"\"primaryComplaint\": "+finalPrimaryComplaint+","+"\"patientDob\": "+finalPatientDob+","+"\"patientClinicoreId\": "+finalClinicoreId+","+"\"studyDate\": "+finalStudyDate+","+"\"status\": "+finalStudyStatus+"}";

        //create std string vars with patient info for questionaire
        RestClient::Response res = RestClient::post(url, ctype, data);
        Wt::log("notice") << "Response body from server:" << res.body;

    }));


    newstudy->clicked().connect(std::bind([=]() {
        dialog->accept();

        Wt::WDialog *stdialog = new Wt::WDialog("Re-Schedule Sleep Study");

        Wt::WContainerWidget *cont= new Wt::WContainerWidget(stdialog->contents());

        Wt::WHBoxLayout *resthbox = new Wt::WHBoxLayout();
        cont->setLayout(resthbox);

        Wt::WVBoxLayout *restvbox1 = new Wt::WVBoxLayout();
        resthbox->addLayout(restvbox1);

        Wt::WVBoxLayout *restvbox2 = new Wt::WVBoxLayout();
        resthbox->addLayout(restvbox2);

        Wt::WText *curloco = new Wt::WText("Current Study Location: " + stloco);
        restvbox1->addWidget(curloco);
        restvbox1->addWidget(new Wt::WBreak());
        Wt::WLabel *lblLocation = new Wt::WLabel("New Sleep Study Location:");
        lblLocation->setStyleClass("sig-lbl");
        Wt::WHBoxLayout *locobox = new Wt::WHBoxLayout();
        locobox->addWidget(lblLocation);

        Wt::WComboBox *cbloco = new Wt::WComboBox();
        cbloco->setStyleClass("sig-left-box");

        cbloco->addItem("none selected");
        cbloco->addItem("Long Beach");
        cbloco->addItem("Downey");
        cbloco->addItem("Home Study");
        cbloco->setCurrentIndex(0);
        cbloco->setMargin(10, Wt::Right);

        Wt::WText *locoout = new Wt::WText();
        locoout->addStyleClass("help-block");

        cbloco->changed().connect(std::bind([=] () {
            locoout->setText(Wt::WString::fromUTF8("{1}")
                 .arg(cbloco->currentText()));
        }));

        locobox->addWidget(cbloco);
        restvbox1->addLayout(locobox);

        Wt::WText *curtime = new Wt::WText("Current Scheduled Study Time: " + sttime);
        restvbox1->addWidget(curtime);
        restvbox1->addWidget(new Wt::WBreak());
        Wt::WLabel *lblTime = new Wt::WLabel("New Study Appointment Time:");
        lblTime->setStyleClass("sig-lbl");
        Wt::WHBoxLayout *timebox = new Wt::WHBoxLayout();
        timebox->addWidget(lblTime);

        Wt::WComboBox *cbtime = new Wt::WComboBox();
        cbtime->setStyleClass("sig-left-box");

        cbtime->addItem("none selected");
        cbtime->addItem("7:00 PM");
        cbtime->addItem("7:30 PM");
        cbtime->addItem("8:00 PM");
        cbtime->addItem("8:30 PM");
        cbtime->addItem("9:00 PM");
        cbtime->addItem("9:30 PM");
        cbtime->addItem("10:00 PM");
        cbtime->addItem("10:30 PM");
        cbtime->addItem("11:00 PM");
        cbtime->addItem("11:30 PM");
        cbtime->addItem("12:00 PM");
        cbtime->setCurrentIndex(0);
        cbtime->setMargin(10, Wt::Right);

        Wt::WText *timeout = new Wt::WText();
        timeout->addStyleClass("help-block");

        cbtime->changed().connect(std::bind([=] () {
            timeout->setText(Wt::WString::fromUTF8("{1}")
                 .arg(cbtime->currentText()));
        }));

        timebox->addWidget(cbtime);
        restvbox1->addLayout(timebox);

        Wt::WCalendar *c2 = new Wt::WCalendar();
        c2->setSelectionMode(Wt::ExtendedSelection);

        Wt::WText* out = new Wt::WText();
        out->addStyleClass("help-block");

        Wt::WPushButton *schedule = new Wt::WPushButton("Save", stdialog->footer());
        schedule->setDefault(true);

        if (out->text() == "")
        {
            schedule->hide();
        }
        c2->selectionChanged().connect(std::bind([=]() {
            Wt::WString selected;
            std::set<Wt::WDate> selection = c2->selection();

            for (std::set<Wt::WDate>::const_iterator it = selection.begin();
                it != selection.end(); ++it) {
                if (!selected.empty())
                    selected += ", ";

                const Wt::WDate& d = *it;
                selected = d.toString("MM/dd/yyyy");
            }

            out->setText(Wt::WString::fromUTF8
                ("{1}")
                .arg(selected));
            schedule->show();
        }));


        restvbox2->addWidget(new Wt::WText("Current Study Date: " + stdate));
        restvbox2->addWidget(new Wt::WText("(You must select a new appointment date to Reschedule)"));
        restvbox2->addWidget(c2);
        restvbox2->addWidget(new Wt::WText("New Study Appointment Date"));
        restvbox2->addWidget(out);

        dbStudyLocation_ = new Wt::WText();
        dbStudyTime_ = new Wt::WText();
        dbStudyDate_ = new Wt::WText();


        cont->show();


        Wt::WPushButton *cl = new Wt::WPushButton("Cancel", stdialog->footer());
        stdialog->rejectWhenEscapePressed();

        schedule->clicked().connect(std::bind([=]() {
            if (locoout->text() == "")
            {
                dbStudyLocation_->setText(stloco);
            } else {
                dbStudyLocation_->setText(locoout->text());
            }
            if (timeout->text() == "")
            {
                dbStudyTime_->setText(sttime);

            } else {
                dbStudyTime_->setText(timeout->text());
            }
            if (out->text() == "")
            {
                dbStudyDate_->setText(stdate);
            } else {
                dbStudyDate_->setText(out->text());
            }

            Wt::WDate dbStudyDateD;
            dbStudyDateD = Wt::WDate().fromString(dbStudyDate_->text().toUTF8(), "MM/dd/yyyy");


            //update study details
            dbo::Transaction strans(dbsession);

            dbo::ptr<SleepStudy> st = dbsession.find<SleepStudy>().where("std_id = ?").bind(stid);

            st.modify()->studyStatus_ = nststatus.toUTF8();
            st.modify()->studyDate_ = dbStudyDateD;
            st.modify()->studyTime_ = dbStudyTime_->text().toUTF8();
            st.modify()->studyLocation_ = dbStudyLocation_->text().toUTF8();


            strans.commit();
            
            dbo::Transaction mptrans(dbsession);
            dbo::ptr<Patient> mpt = dbsession.find<Patient>().where("pt_id = ?").bind(pt);
            mpt.modify()->numReschedule_ = mpt->numReschedule_ + 1;
            mptrans.commit();
            
            dbo::Transaction pctrans(dbsession);

            dbo::ptr<Practice> pc = dbsession.find<Practice>().where("prac_id = ?").bind(pracid);

            WString poptmail = pc->pracEmailOpt_;
            //send email to practice
               pctrans.commit();


               if (poptmail == "Yes") {
                Mail::Message message;
                message.setFrom(Mail::Mailbox("info@statsleep.com", "United STAT Sleep"));
                message.addRecipient(Mail::To, Mail::Mailbox(pemail.toUTF8(), pname.toUTF8()));
                message.setSubject("A Sleep Study has been Re-Scheduled");
                message.setBody("You referred a patient to our sleep center, and a previously scheduled sleep study has been re-scheduled. Your patient has been notified, Please login to your physician account to check details.");
                message.addHtmlBody ("<p>Hi, "+pname.toUTF8()+"<br>You referred a patient to our sleep center, and a previously scheduled sleep study has been re-scheduled.<br> Please <a href=\"https://127.0.0.1/providers\">Log In to your Physician Account</a> to check the sleep study details.</p><br><p>Thanks,</p><p>The United STAT Sleep Team</p>");
                Mail::Client client;
                client.connect("localhost");
                client.send(message);
               }


            stdialog->accept();
            done_.emit(42, "Refresh Now"); // emit the signal

        }));

        cl->clicked().connect(stdialog, &Wt::WDialog::reject);

        stdialog->show();
    }));

    svbox1->addWidget(stdleft);
    svbox2->addWidget(stdright);

    strans.commit();


    dbo::Transaction ptrans(dbsession);

    dbo::ptr<Patient> patient = dbsession.find<Patient>().where("pt_id = ?").bind(pt);

    WString patfirst = patient->ptFirstName_;
    WString patlast = patient->ptLastName_;
    WString patdob = patient->ptDob_;
    WString patgender = patient->ptGender_;
    WString pathome = patient->ptHomePhone_;
    WString patcell = patient->ptCellPhone_;
    WString pataddr1 = patient->ptAddress1_;
    WString pataddr2 = patient->ptAddress2_;
    WString patcity = patient->ptCity_;

    WString patemail = patient->ptEmail_;

    ptrans.commit();




    Wt::WContainerWidget *ptcontainer = new Wt::WContainerWidget();

    Wt::WHBoxLayout *patienthbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *patienthbox2 = new Wt::WHBoxLayout();
    ptcontainer->setLayout(patienthbox);
    ptcontainer->addStyleClass("attcont");

    Wt::WVBoxLayout *patientvbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *patientvbox1 = new Wt::WVBoxLayout();


    patienthbox->addLayout(patientvbox1);
    patienthbox->addLayout(patientvbox2);

    Wt::WContainerWidget *patientleft = new Wt::WContainerWidget();
    patientleft->addStyleClass("refdiagleft");
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("<h4>Patient Info:</h4>"));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("First Name: " + patfirst));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Last Name: " + patlast));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Home Phone: " + pathome));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Cell Phone: " + patlast));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Address 1: " + pataddr1));
    patientleft->addWidget(new Wt::WText("Address 2: " + pataddr2));
    patientleft->addWidget(new Wt::WText("City: " + patcity));
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WBreak());
    patientleft->addWidget(new Wt::WText("Email: " + patemail));



    Wt::WContainerWidget *patientright = new Wt::WContainerWidget();

    patientright->addWidget(new Wt::WText("<h4>Options</h4>"));
    patientright->addWidget(new Wt::WBreak());
    patientright->addWidget(new Wt::WBreak());
    patientright->addWidget(preformdl);
    patientright->addWidget(new Wt::WBreak());
    patientright->addWidget(new Wt::WBreak());
    patientright->addWidget(postformdl);

    patientvbox1->addWidget(patientleft);
    patientvbox2->addWidget(patientright);

    staticComment_ = new Wt::WText();
    staticComment_->setText(setCommentId());

    Wt::WContainerWidget *notes = new Wt::WContainerWidget();


    Wt::WVBoxLayout *notevbox = new Wt::WVBoxLayout();
    notes->setLayout(notevbox);

    dbo::Transaction cmtrans(dbsession);

    typedef dbo::collection< dbo::ptr<Comment> > Comments;
    Comments comments = dbsession.find<Comment>().where("backup_id = ?").bind(stid);
    std::cerr << comments.size() << std::endl;

    Wt::WTable *notetable = new Wt::WTable();
    notetable->setHeaderCount(1);

    notetable->elementAt(0, 0)->addWidget(new Wt::WText("User"));
    notetable->elementAt(0, 1)->addWidget(new Wt::WText("Comment"));
    notetable->elementAt(0, 2)->addWidget(new Wt::WText("Added"));


    int cmrow = 1;
    for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i, ++cmrow)
    {
            Wt::WDateTime fixdate = (*i)->createdAt_;
            Wt::WDateTime adjustdate = fixdate.addSecs(-25200);
            WString finaldate = adjustdate.toString("MM/dd/yyyy hh:mm a");

            new Wt::WText((*i)->staffEmail_, notetable->elementAt(cmrow, 0)),
            new Wt::WText((*i)->commentText_, notetable->elementAt(cmrow, 1)),
            new Wt::WText(finaldate, notetable->elementAt(cmrow, 2));

    }

    cmtrans.commit();

    notetable->addStyleClass("table form-inline");
    notetable->addStyleClass("table table-striped");
    notetable->addStyleClass("table table-hover");

    Wt::WContainerWidget *noteresult = new Wt::WContainerWidget();
    noteresult->addStyleClass("backup-comments");
    noteresult->addWidget(notetable);

    notevbox->addWidget(new Wt::WText("<h3>Chart Notes</h3>"));
    notevbox->addWidget(noteresult);

    notevbox->addWidget(new Wt::WText("<h4>Add Chart Note</h4>"));
    Wt::WTextEdit *noteedit = new Wt::WTextEdit();
    noteedit->setHeight(100);
    notevbox->addWidget(noteedit);

    Wt::WText *editout = new Wt::WText();

    Wt::WPushButton *notesave = new Wt::WPushButton("Add Chart Note");
    notevbox->addWidget(notesave);
    notesave->clicked().connect(std::bind([=] () {

        editout->setText("<pre>" + Wt::Utils::htmlEncode(noteedit->text()) + "</pre>");
        Wt::WString commentTextOut_ = editout->text().toUTF8();

        Wt::WString staffEmail = session_->strUserEmail();
        Wt::WString backupId = stid;
        Wt::WString patientId = ptout;

        Wt::Dbo::Transaction chtrans(dbsession);

        Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

        c.modify()->staffEmail_ = staffEmail.toUTF8();
        c.modify()->commentText_ = commentTextOut_;
        c.modify()->buId_ = backupId.toUTF8();
        c.modify()->ptId_ = patientId.toUTF8();

        //h.modify()->studentFirstName_ = studentFirst.toUTF8();
        //h.modify()->studentLastName_ = studentLast.toUTF8();
        //h.modify()->studentId_ = studentId.toUTF8();

        c.modify()->commentId_ = staticComment_->text().toUTF8();

        chtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal


    }));

    Wt::WContainerWidget *deleteStudy = new Wt::WContainerWidget();
    deleteStudy->addWidget(new Wt::WText("<h4>Are you sure you want to delete this sleep study? This cannot be undone.</h4>"));

    Wt::WPushButton *deleteSleepStudy = new Wt::WPushButton("Delete Sleep Study Now");
    deleteSleepStudy->setStyleClass("btn-delete-patient");
    deleteStudy->addWidget(new Wt::WBreak());
    deleteStudy->addWidget(deleteSleepStudy);


    deleteSleepStudy->clicked().connect(std::bind([=] () {
        dbo::Transaction rmtrans(dbsession);

        dbo::ptr<SleepStudy> study = dbsession.find<SleepStudy>().where("std_id = ?").bind(std);
        study.remove();

        rmtrans.commit();

        dialog->accept();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));
    
    Wt::WContainerWidget *filecontainer = new Wt::WContainerWidget();
        //put table here
    dbo::Transaction satrans(dbsession);

    Attachments sattachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(std);
    std::cerr << sattachments.size() << std::endl;

    Wt::WTable *filetable = new Wt::WTable();
    filetable->setHeaderCount(1);

    filetable->elementAt(0, 0)->addWidget(new Wt::WText("File Name"));
    filetable->elementAt(0, 1)->addWidget(new Wt::WText("Download"));
    filetable->elementAt(0, 2)->addWidget(new Wt::WText("Delete"));

    int frow = 1;
    for (Attachments::const_iterator ai = sattachments.begin(); ai != sattachments.end(); ++ai, ++frow)
    {

        Wt::WResource *attc = new AttachmentResource((*ai)->attId_, (*ai)->tempRefId_, (*ai)->attFileName_, (*ai)->attContentDescription_, this);

        new Wt::WText((*ai)->attFileName_, filetable->elementAt(frow, 0)),
        downDone = new Wt::WText("Downloaded", filetable->elementAt(frow, 1)),
        downDone->hide(),
        
        btnDownloadAtt = new Wt::WPushButton("Download", filetable->elementAt(frow, 1));
        btnDownloadAtt->setLink(attc);

    }

    satrans.commit();

    filetable->addStyleClass("table form-inline");
    filetable->addStyleClass("table table-striped");
    filetable->addStyleClass("table table-hover");

    Wt::WHBoxLayout *fileshbox = new Wt::WHBoxLayout();
    filecontainer->setLayout(fileshbox);

    Wt::WVBoxLayout *filesleft = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesleft);
    Wt::WVBoxLayout *filesright = new Wt::WVBoxLayout();
    fileshbox->addLayout(filesright);

    filesleft->addWidget(new Wt::WText("<h4>Study Report Files</h4>"));
    filesleft->addWidget(filetable);

    composer_ = new Composer(conninfo_, std, this);

    filesright->addWidget(new Wt::WText("<h4>Upload Report</h4>"));
    filesright->addWidget(new Wt::WText("Upload FINAL REPORT ONLY to this sleep study. <p style=\"color: #C9302C\"> Warning: Anything you upload here will be accessible by the clinic that referred this patient via their Physician Portal.</p>"));
    filesright->addWidget(composer_);

    Wt::WTabWidget *tabW = new Wt::WTabWidget(container);
    tabW->addTab(stdcontainer,
             "Sleep Study", Wt::WTabWidget::PreLoading);
    tabW->addTab(refcontainer,
             "Referral", Wt::WTabWidget::PreLoading);
    tabW->addTab(ptcontainer,
             "Patient Info", Wt::WTabWidget::PreLoading);
    tabW->addTab(notes,
             "Chart Notes", Wt::WTabWidget::PreLoading);
    tabW->addTab(filecontainer,
             "Study Report", Wt::WTabWidget::PreLoading);
    tabW->addTab(deleteStudy,
             "Delete Study", Wt::WTabWidget::PreLoading);

    tabW->setStyleClass("tabwidget");

    Wt::WPushButton *alright = new Wt::WPushButton("Ok", dialog->footer());

    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
        done_.emit(42, "Refresh Now"); // emit the signal
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
        // provide an accessor for the signal
         done_.emit(42, "Refresh Now"); // emit the signal
    }));

    container->show();
    dialog->show();
}
