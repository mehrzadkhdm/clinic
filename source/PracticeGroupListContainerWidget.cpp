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
#include <Wt/WLink>

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

#include "StaffSleepStudyWidget.h"
#include "PracticeGroupListContainerWidget.h"
#include "SleepStudy.h"
#include "Referral.h"
#include "Attachment.h"
#include "Patient.h"
#include "Composer.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Locality.h"
#include "PracticeSession.h"
#include "PhysicianRegistrationModel.h"
#include "PhysicianRegistrationWidget.h"
//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

PracticeGroupListContainerWidget::PracticeGroupListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    psession_(conninfo),
    conninfo_(conninfo),
    pg_(conninfo),
    pracsession_(&psession_)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Practice>("practice");


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

void PracticeGroupListContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);

    std::string strSearchPrac = curstr.toUTF8();
    std::string searchPrac = '%' + strSearchPrac +'%';

    typedef dbo::collection< dbo::ptr<Practice> > Practices;
    Practices practices = dbsession.find<Practice>().where("prac_name ILIKE ?").bind(searchPrac).where("is_group = ?").bind("Yes");
    std::cerr << practices.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Phone Number"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Email Address"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Create Account"));


    int row = 1;
    for (Practices::const_iterator i = practices.begin(); i != practices.end(); ++i, ++row)
    {
            if ((*i)->dateAdded_.toString() != "" && (*i)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 30)
            {
                //Wt::log("info") << "ROW STYLE SHOULD BE CHANGED";
                table->rowAt(row)->setStyleClass("row-new-provider");
            }    
            new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->refPracticeName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->refPracticeNpi_, table->elementAt(row, 2)),
            new Wt::WText((*i)->refPracticeEmail_, table->elementAt(row, 3)),
            btnCreateAccount = new Wt::WPushButton("Clinicore Login", table->elementAt(row, 4)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnCreateAccount->clicked().connect(boost::bind(&PracticeGroupListContainerWidget::showCreateAccount, this, (*i)->practiceId_));
            if ((*i)->clinicoreAccount_ == "Yes")
            {
                btnCreateAccount->hide();
                new Wt::WText("Has Account", table->elementAt(row, 4));
            }
            
    }


    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");


    container->addWidget(table);
    addWidget(container);
}

void PracticeGroupListContainerWidget::showCreateAccount(WString prac)
{



    Wt::log("info") << "createPhysicianLogin";
    
    Wt::WDialog *dialog = new Wt::WDialog("Create Clinicore Login");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);    
    
    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());


    dbo::Transaction transaction(dbsession);
    dbo::ptr<Practice> practice = dbsession.find<Practice>().where("prac_id = ?").bind(prac.toUTF8());

    if (practice->clinicoreAccount_ != "Yes")
    {

        Wt::Auth::User user = Wt::Auth::User(boost::lexical_cast<std::string>(practice.id()), pracsession_->practices());
        Wt::Auth::Login login;
        Wt::WString pccemail = session_->strUserEmail();
        Wt::Auth::PhysicianRegistrationModel *result = new Wt::Auth::PhysicianRegistrationModel(PracticeSession::auth(), pracsession_->practices(), login, user);
        Wt::Auth::PhysicianRegistrationWidget *w = new Wt::Auth::PhysicianRegistrationWidget(prac, pccemail, pracsession_);

        result->addPasswordAuth(&PracticeSession::absPasswordAuth());

        w->setModel(result);

        container->addWidget(w);

    } else {
        container->addWidget(new Wt::WText("<h4>Clinicore Account already exisits for this Physician</h4>"));
    }

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    Wt::WPushButton *alright = new Wt::WPushButton("Done", dialog->footer());

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    container->show();
    dialog->show();
}
