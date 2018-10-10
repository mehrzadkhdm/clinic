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
#include "LocalityListContainerWidget.h"
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

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

LocalityListContainerWidget::LocalityListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Locality>("locality");


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

void LocalityListContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Locality> > Localities;
    Localities locals = dbsession.find<Locality>().where("local_name LIKE ?").bind(searchPt);
    std::cerr << locals.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Locality"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("State"));


    int row = 1;
    for (Localities::const_iterator i = locals.begin(); i != locals.end(); ++i, ++row)
    {

        new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->name_, table->elementAt(row, 1)),
            new Wt::WText((*i)->abbrv_, table->elementAt(row, 2));
    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    container->addWidget(table);
    addWidget(container);
}
