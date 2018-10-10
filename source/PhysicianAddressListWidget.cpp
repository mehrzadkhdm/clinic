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
#include "PhysicianAddressListWidget.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Address.h"
#include "Patient.h"
#include "Locality.h"
#include "LocalityItem.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "PhysicianAddressListContainerWidget.h"
#include "UserSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace AddressApp{
    struct AddressItem {
        Wt::WString addressName;

        AddressItem(const Wt::WString& aAddressName)
            : addressName(aAddressName){}
    };
}


PhysicianAddressListWidget::PhysicianAddressListWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    conninfo_(conninfo),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at the backup list.)";

    dbsession.mapClass<Address>("address");
    dbsession.mapClass<Locality>("locality");
    dbsession.mapClass<CompositeEntity>("composite");

}

void PhysicianAddressListWidget::addressList()
{
    Wt::log("notice") << "SILW::addressList";

    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    Wt::WText *localityid = new Wt::WText();

    Wt::WPushButton *addAddress = new Wt::WPushButton("New Street Address");
    container->addWidget(addAddress);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    Wt::WContainerWidget *localityContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblLocality = new Wt::WLabel("Address Locality:");
    lblLocality->setStyleClass("sig-lbl");

    Wt::WHBoxLayout *localityhbox = new Wt::WHBoxLayout();
    localityhbox->addWidget(lblLocality);
    localityhbox->addWidget(new Wt::WBreak());
    localityhbox->addWidget(new Wt::WBreak());

    dbo::Transaction instrans(dbsession);

    typedef dbo::collection< dbo::ptr<Locality> > Locals;
    Locals locals = dbsession.find<Locality>();
    std::cerr << locals.size() << std::endl;

    std::vector<LocalityItem> localitems;

    for (Locals::const_iterator i = locals.begin(); i != locals.end(); ++i)
    {

        Wt::WString name = (*i)->name_;
        Wt::WString abbrv = (*i)->abbrv_;
        Wt::WString id = (*i)->localityId_;

        Wt::WString fullname = name + ", " + abbrv;

        LocalityItem *e = new LocalityItem();
        e->localName = fullname;
        e->localAbbrv = abbrv;
        e->localId = id;


        localitems.push_back(*e);

    }
    log("notice") << "Current Number of locality items "<< localitems.size();

    instrans.commit();


    Wt::WComboBox *cblocality = new Wt::WComboBox();
    cblocality->setStyleClass("sig-left-box");
    Wt::WText *localout = new Wt::WText();
    cblocality->addItem("Select Locality");

    for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
    {
        cblocality->addItem(ev->localName);
    }

    cblocality->changed().connect(std::bind([=] () {

          LocalityItem selected;

          for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cblocality->currentText();

            if (ev->localName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.localName != "")
        {
            Wt::WString localid;
            Wt::WString localname;
            localid = selected.localId;
            localname = selected.localName;
            
            localityid->setText(localid);
            PhysicianAddressListWidget::search(Wt::WString(""), localid);

        } else {
            log("notice") << "Selected: No Locality";
        }

    }));

    cblocality->setCurrentIndex(0);
    cblocality->setMargin(10, Wt::Right);

    localityContainer->addWidget(cblocality);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(localityContainer);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    Wt::WLineEdit *edit = new Wt::WLineEdit(container);
    edit->setPlaceholderText("Search for address");
    edit->setStyleClass("search-box");

    Wt::WText *out = new Wt::WText("", container);
    out->addStyleClass("help-block");
    Wt::WText *curout = new Wt::WText();

    resultStack_ = new Wt::WStackedWidget(container);

    edit->textInput().connect(std::bind([=] () {
        WString upper = boost::to_upper_copy<std::string>(edit->text().toUTF8());
        PhysicianAddressListWidget::search(upper, localityid->text());

    }));

    addAddress->clicked().connect(std::bind([=] () {
        PhysicianAddressListWidget::addAddressDialog();
    }));

    addWidget(container);
    Wt::log("notice") << "SILW::addressList call search";

    PhysicianAddressListWidget::search(Wt::WString(""), localityid->text());

}

void PhysicianAddressListWidget::search(Wt::WString searchString, Wt::WString locid)
{

    resultStack_->clear();

    insContainer = new PhysicianAddressListContainerWidget(conninfo_, session_, resultStack_);
    insContainer->showSearchLocation(searchString, locid);

    resultStack_->setCurrentWidget(insContainer);

}

WString PhysicianAddressListWidget::setAddressId()
{
    return uuid();
}

void PhysicianAddressListWidget::addAddressDialog()
{
    staticAddress_ = new Wt::WText();
    staticAddress_->setText(setAddressId());

    Wt::WText *localityname = new Wt::WText();
    Wt::WText *localityid = new Wt::WText();
    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Add Address Provider");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WLabel *addressNameLbl = new Wt::WLabel("Street Address");
    Wt::WLineEdit *addressName = new Wt::WLineEdit();

    container->addWidget(addressNameLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(addressName);
    
    Wt::WContainerWidget *localityContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblLocality = new Wt::WLabel("Address Locality:");
    lblLocality->setStyleClass("sig-lbl");

    Wt::WHBoxLayout *localityhbox = new Wt::WHBoxLayout();
    localityhbox->addWidget(lblLocality);
    localityhbox->addWidget(new Wt::WBreak());
    localityhbox->addWidget(new Wt::WBreak());

    dbo::Transaction instrans(dbsession);

    typedef dbo::collection< dbo::ptr<Locality> > Locals;
    Locals locals = dbsession.find<Locality>();
    std::cerr << locals.size() << std::endl;

    std::vector<LocalityItem> localitems;

    for (Locals::const_iterator i = locals.begin(); i != locals.end(); ++i)
    {

        Wt::WString name = (*i)->name_;
        Wt::WString abbrv = (*i)->abbrv_;
        Wt::WString id = (*i)->localityId_;

        Wt::WString fullname = name + ", " + abbrv;

        LocalityItem *e = new LocalityItem();
        e->localName = fullname;
        e->localAbbrv = abbrv;
        e->localId = id;


        localitems.push_back(*e);

    }
    log("notice") << "Current Number of locality items "<< localitems.size();

    instrans.commit();


    Wt::WComboBox *cblocality = new Wt::WComboBox();
    cblocality->setStyleClass("sig-left-box");
    Wt::WText *localout = new Wt::WText();
    cblocality->addItem("");

    for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
    {
        cblocality->addItem(ev->localName);
    }

    cblocality->changed().connect(std::bind([=] () {

          LocalityItem selected;

          for (std::vector<LocalityItem>::const_iterator ev = localitems.begin(); ev != localitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cblocality->currentText();

            if (ev->localName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.localName != "")
        {
            Wt::WString localid;
            Wt::WString localname;
            localid = selected.localId;
            localname = selected.localName;
            
            localityid->setText(localid);
            localityname->setText(localname);

        } else {
            log("notice") << "Selected: No Locality";
        }

    }));

    cblocality->setCurrentIndex(0);
    cblocality->setMargin(10, Wt::Right);

    localityContainer->addWidget(cblocality);

    container->addWidget(new Wt::WBreak());
    container->addWidget(localityContainer);

    Wt::WLabel *zipLbl = new Wt::WLabel("Zip Code");
    Wt::WLineEdit *zipName = new Wt::WLineEdit();

    container->addWidget(zipLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(zipName);



    Wt::WPushButton *save = new Wt::WPushButton("Add Address Group");
    container->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        Wt::WString addressNameOut = addressName->text().toUTF8();
        Wt::WString zipOut = zipName->text().toUTF8();
        Wt::WString localityNameOut = localityname->text().toUTF8();
        Wt::WString localityIdOut = localityid->text().toUTF8();

        Wt::Dbo::Transaction htrans(dbsession);
        Wt::Dbo::ptr<Address> in = dbsession.add(new Address());

        in.modify()->line1_ = addressNameOut.toUTF8();
        in.modify()->zip_ = zipOut.toUTF8();
        in.modify()->localityName_ = localityNameOut.toUTF8();
        in.modify()->localityId_ = localityIdOut.toUTF8();
        in.modify()->addressId_ = staticAddress_->text().toUTF8();

        htrans.commit();

        dialog->accept();
        PhysicianAddressListWidget::search(Wt::WString(""), localityid->text());

    }));

    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
