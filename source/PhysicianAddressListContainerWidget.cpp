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
#include "PhysicianAddressListContainerWidget.h"
#include "SleepStudy.h"
#include "Referral.h"
#include "Attachment.h"
#include "Patient.h"
#include "Composer.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "Comment.h"
#include "Address.h"
#include "Backup.h"
#include "Address.h"
#include "Locality.h"
#include "LocalityItem.h"

//rest http library
#include "restclient-cpp/restclient.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

//struct for SleepStudy info
namespace InboundRefApp{
    struct InboundSleepStudy {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptCity;

        InboundSleepStudy(
            const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob,
            const Wt::WString& aPtCity)
            :    ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob),
            ptCity(aPtCity) {}
    };
}

PhysicianAddressListContainerWidget::PhysicianAddressListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Address>("address");
    dbsession.mapClass<Practice>("practice");
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

void PhysicianAddressListContainerWidget::showSearch(Wt::WString& curstr)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Address> > Addresss;
    Addresss addresss = dbsession.find<Address>().where("line1 LIKE ?").bind(searchPt);
    std::cerr << addresss.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Street Address"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Locality"));


    int row = 1;
    for (Addresss::const_iterator i = addresss.begin(); i != addresss.end(); ++i, ++row)
    {

        
        dbo::Transaction loctrans(dbsession);
        dbo::ptr<Locality> local = dbsession.find<Locality>().where("local_id = ?").bind((*i)->localityId_.toUTF8());

        WString locname = local->name_;
        WString locabbv = local->abbrv_;
        Wt::WString disploc = locname + ", "+locabbv;

            new Wt::WText((*i)->line1_, table->elementAt(row, 0)),
            new Wt::WText(disploc, table->elementAt(row, 1)),
            btnSelectAddress = new Wt::WPushButton("Assign Address", table->elementAt(row, 2)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnSelectAddress->clicked().connect(boost::bind(&PhysicianAddressListContainerWidget::selectNewAddress, this, (*i)->addressId_));
    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    WText *nomatches = new WText("No addresses found.");
    nomatches->setStyleClass("no-addresses-found");

    WPushButton *newaddress = new WPushButton("Add New Address");
    newaddress->setStyleClass("add-new-addr-btn");

    if (addresss.size() < 1) {
        container->addWidget(nomatches);
        container->addWidget(new WBreak());
        container->addWidget(new WBreak());
        container->addWidget(newaddress);

    } else {
        container->addWidget(table);
    }
    newaddress->clicked().connect(boost::bind(&PhysicianAddressListContainerWidget::addAddressDialog, this));

    addWidget(container);
}

void PhysicianAddressListContainerWidget::showSearchLocation(Wt::WString& curstr, Wt::WString& locid)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
    std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Address> > Addresss;
    Addresss addresss = dbsession.find<Address>().where("line1 ILIKE ?").bind(searchPt).where("local_id = ?").bind(locid);
    std::cerr << addresss.size() << std::endl;



    int row = 1;
    for (Addresss::const_iterator i = addresss.begin(); i != addresss.end(); ++i, ++row)
    {

        dbo::Transaction loctrans(dbsession);
        dbo::ptr<Locality> local = dbsession.find<Locality>().where("local_id = ?").bind((*i)->localityId_.toUTF8());

        WString locname = local->name_;
        WString locabbv = local->abbrv_;
        Wt::WString disploc = locname + ", "+locabbv;
        
        Wt::WText *addrLine1 = new Wt::WText((*i)->line1_);
        Wt::WText *addrLocality = new Wt::WText(disploc);
        Wt::WText *addrZip = new Wt::WText((*i)->zip_);

        addrLine1->setStyleClass("address-line1");
        addrLocality->setStyleClass("address-locality");
        addrZip->setStyleClass("address-zipcode");

        container->addWidget(new Wt::WBreak());
        container->addWidget(addrLine1);
        container->addWidget(new Wt::WBreak());
        container->addWidget(addrLocality);
        container->addWidget(new Wt::WBreak());
        container->addWidget(addrZip);


        container->addWidget(new Wt::WBreak());


        int row = 1;

        typedef dbo::collection< dbo::ptr<Practice> > Practices;
        Practices practices = dbsession.find<Practice>().where("address_id = ?").bind((*i)->addressId_);
        std::cerr << practices.size() << std::endl;

        Wt::WTable *table = new Wt::WTable();
        table->setHeaderCount(1);

        table->elementAt(0, 0)->addWidget(new Wt::WText("MD First Name"));
        table->elementAt(0, 1)->addWidget(new Wt::WText("MD Last Name"));
        table->elementAt(0, 2)->addWidget(new Wt::WText("Suite Number"));
        table->elementAt(0, 3)->addWidget(new Wt::WText("Phone Number"));
        table->elementAt(0, 4)->addWidget(new Wt::WText("Email Address"));
        
                    
        for (Practices::const_iterator p = practices.begin(); p != practices.end(); ++p, ++row) {

            if ((*p)->dateAdded_.toString() != "" && (*p)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 90)
            {
                table->rowAt(row)->setStyleClass("row-new-provider");
            } else if ((*p)->dateAdded_.toString() != "" && (*p)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) > 90 && (*p)->dateAdded_.daysTo(Wt::WDateTime::currentDateTime()) <= 365) {
                table->rowAt(row)->setStyleClass("row-newyr-provider");
            }
            if ((*p)->refPracticeDrFirst_ == "") {
                new Wt::WText((*p)->refPracticeName_, table->elementAt(row, 0));
            } else {
                new Wt::WText((*p)->refPracticeDrFirst_, table->elementAt(row, 0));
            }
            new Wt::WText((*p)->refPracticeDrLast_, table->elementAt(row, 1)),
            new Wt::WText((*p)->addrLine2_, table->elementAt(row, 2)),
            new Wt::WText((*p)->refPracticeNpi_, table->elementAt(row, 3)),
            new Wt::WText((*p)->refPracticeEmail_, table->elementAt(row, 4));

        }

        table->addStyleClass("table form-inline");
        table->addStyleClass("table table-striped");
        table->addStyleClass("table table-hover");

        container->addWidget(table);
        container->addWidget(new Wt::WBreak());
        container->addWidget(new Wt::WBreak());
    }

    transaction.commit();

    WText *nomatches = new WText("No addresses found.");
    nomatches->setStyleClass("no-addresses-found");

    WPushButton *newaddress = new WPushButton("Add New Address");
    newaddress->setStyleClass("add-new-addr-btn");

    newaddress->clicked().connect(boost::bind(&PhysicianAddressListContainerWidget::addAddressDialog, this));

    addWidget(container);
}
WString PhysicianAddressListContainerWidget::setAddressId()
{
    return uuid();
}
void PhysicianAddressListContainerWidget::addAddressDialog()
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
        PhysicianAddressListContainerWidget::selectNewAddress(staticAddress_->text());

    }));

    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}

void PhysicianAddressListContainerWidget::selectNewAddress(Wt::WString aid) {
    Wt::log("info") << "PhysicianAddressListContainerWidget selectNewAddress before emit";
    selectAddress_.emit(aid);
    Wt::log("info") << "PhysicianAddressListContainerWidget selectNewAddress after emit";
}