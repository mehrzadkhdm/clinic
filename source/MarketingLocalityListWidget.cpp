/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std stuff
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
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WStackedWidget>
#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/Utils>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "MarketingLocalityListWidget.h"
#include "LocalityListContainerWidget.h"
#include "Locality.h"
#include "UserSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InsuranceApp{
    struct InsuranceItem {
        Wt::WString insuranceName;

        InsuranceItem(const Wt::WString& aInsuranceName)
            : insuranceName(aInsuranceName){}
    };
}


MarketingLocalityListWidget::MarketingLocalityListWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    conninfo_(conninfo),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at the backup list.)";

    dbsession.mapClass<Locality>("locality");
    dbsession.mapClass<CompositeEntity>("composite");

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

void MarketingLocalityListWidget::localityList()
{
    Wt::log("notice") << "localityList";

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *addLocality = new Wt::WPushButton("New Locality");
    container->addWidget(addLocality);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for locality");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        MarketingLocalityListWidget::search(upper);

    }));

    addLocality->clicked().connect(std::bind([=] () {
        MarketingLocalityListWidget::addLocalityDialog();
    }));

    addWidget(container);
    Wt::log("notice") << "localityList call search";

    MarketingLocalityListWidget::search(Wt::WString(""));

}

void MarketingLocalityListWidget::search(Wt::WString searchString)
{
    Wt::log("notice") << "localitylist::search()";

    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    locContainer = new LocalityListContainerWidget(conninfo_, session_, resultStack_);
    locContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(locContainer);

}

WString MarketingLocalityListWidget::setLocalityId()
{
    return uuid();
}

void MarketingLocalityListWidget::addLocalityDialog()
{
    staticLocality_ = new Wt::WText();
    staticLocality_->setText(setLocalityId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Add New Locality");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WLabel *localityNameLbl = new Wt::WLabel("Locality Name");
    Wt::WLineEdit *localityName = new Wt::WLineEdit();
    Wt::WLabel *localityAbbrvLbl = new Wt::WLabel("Locality State");

    container->addWidget(localityNameLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(localityName);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(localityAbbrvLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("CA"));

    Wt::WPushButton *save = new Wt::WPushButton("Add Locality");
    container->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        Wt::WString localityNameOut = localityName->text().toUTF8();
        Wt::WString localityAbbrvOut = "CA";

        Wt::Dbo::Transaction htrans(dbsession);
        Wt::Dbo::ptr<Locality> loc = dbsession.add(new Locality());

        loc.modify()->name_ = localityNameOut.toUTF8();
        loc.modify()->abbrv_ = localityAbbrvOut.toUTF8();
        loc.modify()->localityId_ = staticLocality_->text().toUTF8();

        htrans.commit();

        dialog->accept();
        MarketingLocalityListWidget::search(Wt::WString(""));

    }));


    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
