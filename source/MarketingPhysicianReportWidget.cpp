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
#include <Wt/WDate>
#include <Wt/WDateEdit>
#include <Wt/WStackedWidget>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "Composer.h"

#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Auth/Dbo/UserDatabase>

#include "InboundContainerWidget.h"
#include "MarketingPhysicianReportWidget.h"
#include "ReferralResultWidget.h"
#include "Referral.h"
#include "Practice.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "InsuranceItem.h"
#include "Insurance.h"
#include "UserSession.h"
#include "PccItem.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InboundRefApp{
    struct InboundReferral {
        Wt::WString pracName;
        Wt::WString pracDrName;
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptCity;

        InboundReferral(const Wt::WString& aPracName,
            const Wt::WString& aPracDrName,
            const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtCity)
            : pracName(aPracName),
            pracDrName(aPracDrName),
            ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptCity(aPtCity) {}
    };
}

namespace InboundRefAttachments{
    struct InboundAttachment {
        Wt::WString fileName;
        Wt::WString fileType;

        InboundAttachment(const Wt::WString& aFileName,
            const Wt::WString& aFileType)
            : fileName(aFileName),
            fileType(aFileType){}
    };
}

MarketingPhysicianReportWidget::MarketingPhysicianReportWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session),
    conninfo_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Referral>("referral");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");
    dbsession.mapClass<Patient>("patient");
    dbsession.mapClass<SleepStudy>("sleepstudy");
    dbsession.mapClass<Practice>("practice");
    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<User>("user");
    dbsession.mapClass<AuthInfo>("auth_info");


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

void MarketingPhysicianReportWidget::newVisitReport()
{

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    container->setStyleClass("print-referrals-container");
    
    Wt::WContainerWidget *container2 = new Wt::WContainerWidget();
    container2->setStyleClass("print-referrals-container");
    
    Wt::WHBoxLayout *reporthbox = new Wt::WHBoxLayout();
    container2->setLayout(reporthbox);

    Wt::WVBoxLayout *startvbox = new Wt::WVBoxLayout();
    reporthbox->addLayout(startvbox);

    Wt::WVBoxLayout *endvbox = new Wt::WVBoxLayout();
    reporthbox->addLayout(endvbox);

    Wt::WVBoxLayout *pccvbox = new Wt::WVBoxLayout();
    reporthbox->addLayout(pccvbox);

    Wt::WTemplate *form = new Wt::WTemplate(Wt::WString::tr("dateEdit-template"));
    form->addFunction("id", &Wt::WTemplate::Functions::id);

    Wt::WText *delabel = new Wt::WText("Please select the date range for the new MDs report.");
    container->addWidget(delabel);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    
    Wt::WLabel *lblStart = new Wt::WLabel("From:");
    lblStart->setStyleClass("sig-lbl");

    Wt::WDateEdit *de1 = new Wt::WDateEdit();
    form->bindWidget("from", de1);
    de1->setFormat("MM/dd/yyyy"); // Apply a different date format.
    de1->setPlaceholderText("Start Date");
    de1->setStyleClass("print-de1");
    startvbox->addWidget(lblStart);
    startvbox->addWidget(de1);

    Wt::WLabel *lblEnd = new Wt::WLabel("To:");
    lblEnd->setStyleClass("sig-lbl");

    Wt::WDateEdit *de2 = new Wt::WDateEdit();
    form->bindWidget("to", de2);
    de2->setFormat("MM/dd/yyyy"); // Apply a different date format.
    de2->setPlaceholderText("End Date");
    de2->setStyleClass("print-de2");
    endvbox->addWidget(lblEnd);
    endvbox->addWidget(de2);

    Wt::WLabel *lblPcc = new Wt::WLabel("Select Employee:");
    lblPcc->setStyleClass("sig-lbl");

    Wt::WHBoxLayout *pccbox = new Wt::WHBoxLayout();
    pccvbox->addWidget(lblPcc);

    dbo::Transaction infotrans(dbsession);

    typedef dbo::collection< dbo::ptr<AuthInfo> > AuthInfos;
    AuthInfos infos = dbsession.find<AuthInfo>();
    std::cerr << infos.size() << std::endl;

    std::vector<PccItem> pccitems;

    for (AuthInfos::const_iterator i = infos.begin(); i != infos.end(); ++i)
    {
        PccItem *e = new PccItem();
        Wt::WString name;
        if((*i)->email() != "")
        {
            name = (*i)->email();
        } else {
            name = (*i)->unverifiedEmail();
        }


        e->pccEmail = name;

        pccitems.push_back(*e);

    }
    log("notice") << "Current Number of pcc items "<< pccitems.size();

    infotrans.commit();

    Wt::WComboBox *cbpcc = new Wt::WComboBox();
    cbpcc->setStyleClass("sig-left-box");
    Wt::WText *pccout = new Wt::WText();
    cbpcc->addItem("");

    for (std::vector<PccItem>::const_iterator ev = pccitems.begin(); ev != pccitems.end(); ++ev)
    {
        cbpcc->addItem(ev->pccEmail);
    }

    Wt::WText *pccReportText = new Wt::WText(container);
    pccReportText->hide();

    cbpcc->changed().connect(std::bind([=] () {
        pccReportText->setText(cbpcc->currentText());

    }));

    cbpcc->setCurrentIndex(0);
    cbpcc->setMargin(10, Wt::Right);

    pccvbox->addWidget(cbpcc);

    Wt::WPushButton *button = new Wt::WPushButton("Run Report");
    form->bindWidget("save", button);
    button->setStyleClass("print-button");

    Wt::WText *out = new Wt::WText();
    form->bindWidget("out", out);

    reporthbox->addWidget(button);
    container->addWidget(container2);

    container->addWidget(new Wt::WBreak());
    resultStack_ = new Wt::WStackedWidget(container);

    button->clicked().connect(std::bind([=] () {
        Wt::WDate start;
        Wt::WDate end;
        Wt::WString pcc;

        start = de1->date();
        end = de2->date();
        pcc = pccReportText->text();

        MarketingPhysicianReportWidget::showResult(start, end, pcc);
    }));

    addWidget(container);

}

void MarketingPhysicianReportWidget::showResult(Wt::WDate& startDate, Wt::WDate& endDate, Wt::WString& pcc)
{

    resultStack_->clear();

    physicianReport = new PccPhysicianReportWidget(conninfo_, session_, resultStack_);
    physicianReport->showPhysicianReport(startDate, endDate, pcc);
    resultStack_->setCurrentWidget(physicianReport);

}
