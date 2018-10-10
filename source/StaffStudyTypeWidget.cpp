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
#include "StaffStudyTypeWidget.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "StudyType.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "StudyTypeListContainerWidget.h"
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


StaffStudyTypeWidget::StaffStudyTypeWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    conninfo_(conninfo),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at tche backup list.)";

    dbsession.mapClass<StudyType>("studytype");
    dbsession.mapClass<CompositeEntity>("composite");

}

void StaffStudyTypeWidget::studyTypeList()
{
    Wt::log("notice") << "SILW::insuranceList";

    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *addStudyType = new Wt::WPushButton("New Study Type");
    container->addWidget(addStudyType);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for study type");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        StaffStudyTypeWidget::search(upper);

    }));

    addStudyType->clicked().connect(std::bind([=] () {
        StaffStudyTypeWidget::addStudyTypeDialog();
    }));

    addWidget(container);
    Wt::log("notice") << "SILW::insuranceList call search";

    StaffStudyTypeWidget::search(Wt::WString(""));

}

void StaffStudyTypeWidget::search(Wt::WString searchString)
{
    Wt::log("notice") << "SILW::search()";

    int limit = 100;
    int offset = 0;

    resultStack_->clear();

    insContainer = new StudyTypeListContainerWidget(conninfo_, session_, resultStack_);
    insContainer->showSearch(searchString, limit, offset);

    resultStack_->setCurrentWidget(insContainer);

}

WString StaffStudyTypeWidget::setStudyTypeId()
{
    return uuid();
}

void StaffStudyTypeWidget::addStudyTypeDialog()
{
    staticStudyType_ = new Wt::WText();
    staticStudyType_->setText(setStudyTypeId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Add Study Type");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WLabel *typeNameLbl = new Wt::WLabel("Study Type Name: ");
    Wt::WLineEdit *typeName = new Wt::WLineEdit();

    Wt::WLabel *typeCodeLbl = new Wt::WLabel("Study Type Code: ");
    Wt::WLineEdit *typeCode = new Wt::WLineEdit();

    container->addWidget(typeNameLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(typeName);
    container->addWidget(new Wt::WBreak());
    container->addWidget(typeCodeLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(typeCode);



    Wt::WPushButton *save = new Wt::WPushButton("Add Study Type");
    container->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        Wt::WString typeNameOut = typeName->text().toUTF8();
        Wt::WString typeCodeOut = typeCode->text().toUTF8();

        Wt::Dbo::Transaction htrans(dbsession);
        Wt::Dbo::ptr<StudyType> in = dbsession.add(new StudyType());

        in.modify()->studyTypeName_ = typeNameOut.toUTF8();
        in.modify()->studyTypeCode_ = typeCodeOut.toUTF8();
        in.modify()->studyTypeId_ = staticStudyType_->text().toUTF8();

        htrans.commit();

        dialog->accept();
        StaffStudyTypeWidget::search(Wt::WString(""));

    }));


    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
