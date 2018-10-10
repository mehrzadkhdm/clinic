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
#include "StaffInsuranceListWidget.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Insurance.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "InsuranceListContainerWidget.h"
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


StaffInsuranceListWidget::StaffInsuranceListWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    conninfo_(conninfo),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at the backup list.)";

    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<CompositeEntity>("composite");

}

void StaffInsuranceListWidget::insuranceList()
{
    Wt::log("notice") << "SILW::insuranceList";

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *addInsurance = new Wt::WPushButton("New Insurance");
    container->addWidget(addInsurance);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for insurance");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        StaffInsuranceListWidget::search(upper);

    }));

    addInsurance->clicked().connect(std::bind([=] () {
        StaffInsuranceListWidget::addInsuranceDialog();
    }));

    addWidget(container);
    Wt::log("notice") << "SILW::insuranceList call search";

    StaffInsuranceListWidget::search(Wt::WString(""));

}

void StaffInsuranceListWidget::search(Wt::WString searchString)
{
    Wt::log("notice") << "SILW::search()";

    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    insContainer = new InsuranceListContainerWidget(conninfo_, session_, resultStack_);
    insContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(insContainer);

}

WString StaffInsuranceListWidget::setInsuranceId()
{
    return uuid();
}

void StaffInsuranceListWidget::addInsuranceDialog()
{
    staticInsurance_ = new Wt::WText();
    staticInsurance_->setText(setInsuranceId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Add Insurance Provider");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WLabel *insuranceNameLbl = new Wt::WLabel("InsuranceName");
    Wt::WLineEdit *insuranceName = new Wt::WLineEdit();

    container->addWidget(insuranceNameLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(insuranceName);



    Wt::WPushButton *save = new Wt::WPushButton("Add Insurance");
    container->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        Wt::WString insuranceNameOut = insuranceName->text().toUTF8();

        Wt::Dbo::Transaction htrans(dbsession);
        Wt::Dbo::ptr<Insurance> in = dbsession.add(new Insurance());

        in.modify()->insuranceName_ = insuranceNameOut.toUTF8();
        in.modify()->insuranceId_ = staticInsurance_->text().toUTF8();

        htrans.commit();

        dialog->accept();
        StaffInsuranceListWidget::search(Wt::WString(""));

    }));


    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
