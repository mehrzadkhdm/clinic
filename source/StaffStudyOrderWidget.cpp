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

//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "StaffStudyOrderWidget.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InboundRefApp{
    struct InboundReferral {
        Wt::WString pracName;
        Wt::WString createdAt;
        Wt::WString numAttc;

        InboundReferral(const Wt::WString& aPracName,
            const Wt::WString& aCreatedAt,
            const Wt::WString& aNumAttc)
            : pracName(aPracName),
            createdAt(aCreatedAt),
            numAttc(aNumAttc){}
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
StaffStudyOrderWidget::StaffStudyOrderWidget(const char *conninfo, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<Order>("order");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<AttachmentDb>("attachment");


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

void StaffStudyOrderWidget::orderList()
{

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<Order> > Orders;
    Orders orders = dbsession.find<Order>();
    std::cerr << orders.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Order Received"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("View Batch"));


    int row = 1;
    for (Orders::const_iterator i = orders.begin(); i != orders.end(); ++i, ++row)
    {

        new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->repPracticeName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->repCreatedAt_, table->elementAt(row, 2)),
            btnDownloadRef = new Wt::WPushButton("Details", table->elementAt(row, 3));
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnDownloadRef->clicked().connect(boost::bind(&StaffStudyOrderWidget::showOrderDialog, this, (*i)->orderId_));

    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addWidget(table);
    addWidget(new Wt::WText("<h3>Incoming Study Orders</h3>"));
    addWidget(result);

}

WString StaffStudyOrderWidget::setPtId()
{
    return uuid();
}

WString StaffStudyOrderWidget::setStdId()
{
    return uuid();
}

void StaffStudyOrderWidget::showOrderDialog(WString& ord)
{
    //write dialog for staff to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Inbound Study Order Batch");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    dbo::Transaction otrans(dbsession);

    dbo::ptr<Order> order = dbsession.find<Order>().where("report_id = ?").bind(ord);

    WString pname = order->repPracticeName_;
    WString pdrfirst = order->repPracticeDrFirst_;
    WString pdrlast = order->repPracticeDrLast_;
    WString pemail = order->repPracticeEmail_;
    WString crat = order->repCreatedAt_;
    WString oid = order->orderId_;
    otrans.commit();

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container->setLayout(hbox);
    container->addStyleClass("attcont");

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WContainerWidget *left = new Wt::WContainerWidget(dialog->contents());
    left->addStyleClass("refdiagleft");
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("<h4>Practice Info:</h4>"));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Name: " + pname));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("MD First Name: " + pdrfirst));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("MD Last Name: " + pdrlast));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Practice Email: " + pemail));
    left->addWidget(new Wt::WBreak());
    left->addWidget(new Wt::WText("Order Received On: " + crat));
    left->addWidget(new Wt::WBreak());

    Wt::WContainerWidget *right = new Wt::WContainerWidget(dialog->contents());


    //put table here
    dbo::Transaction atrans(dbsession);
    typedef dbo::collection< dbo::ptr<AttachmentDb> > Attachments;

    Attachments attachments = dbsession.find<AttachmentDb>().where("temp_ref = ?").bind(oid);
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
        btnDownloadAtt = new Wt::WPushButton("Download", table->elementAt(row, 1)),
        btnDownloadAtt->setLink(attc);

    }

    atrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WPushButton *alright = new Wt::WPushButton("Ok", dialog->footer());

    Wt::WPushButton *cancel = new Wt::WPushButton("Go Back", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(std::bind([=]() {
        dialog->reject();
    }));

    alright->clicked().connect(std::bind([=]() {
        dialog->accept();
    }));

    right->addWidget(new Wt::WText("<h4>Order Attachments</h4>"));
    right->addWidget(table);

    vbox1->addWidget(left);
    vbox2->addWidget(right);

    container->show();
    dialog->show();
}
