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
#include "InsuranceListContainerWidget.h"
#include "SleepStudy.h"
#include "Referral.h"
#include "Attachment.h"
#include "Patient.h"
#include "Composer.h"
#include "ReferralResource.cpp"
#include "Practice.h"
#include "AttachmentResource.cpp"
#include "UserSession.h"
#include "InsuranceGroup.h"
#include "InsuranceGroupItem.h"
#include "Comment.h"
#include "Backup.h"
#include "Insurance.h"

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

InsuranceListContainerWidget::InsuranceListContainerWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    session_(session),
    conninfo_(conninfo),
    pg_(conninfo)
{
    dbsession.setConnection(pg_);


    pg_.setProperty("show-queries", "true");


    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Insurance>("insurance");
    dbsession.mapClass<InsuranceGroup>("insurance_group");


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

void InsuranceListContainerWidget::showSearch(Wt::WString& curstr, int limit, int offset)
{
    log("info") << "ILC::showSearch";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    dbo::Transaction transaction(dbsession);
    std::string strSearchPtFirst = curstr.toUTF8();
      std::string searchPt = '%' + strSearchPtFirst +'%';

    typedef dbo::collection< dbo::ptr<Insurance> > Insurances;
    Insurances insurances = dbsession.find<Insurance>().where("ins_name LIKE ?").bind(searchPt);
    std::cerr << insurances.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("Insurance Name"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Insurance Group"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Details"));

    int row = 1;
    for (Insurances::const_iterator i = insurances.begin(); i != insurances.end(); ++i, ++row)
    {

            new Wt::WText((*i)->insuranceName_, table->elementAt(row, 0)),
            new Wt::WText((*i)->insuranceGroupName_, table->elementAt(row, 1)),
            btnShowInsurance = new Wt::WPushButton("Details", table->elementAt(row, 2)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnShowInsurance->clicked().connect(boost::bind(&InsuranceListContainerWidget::showDialog, this, (*i)->insuranceId_));

    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    container->addWidget(table);
    addWidget(container);
}

void InsuranceListContainerWidget::showDialog(Wt::WString ins) {
    Wt::WDialog *dialog = new Wt::WDialog("Insurance Details");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());
    
    Wt::WContainerWidget *insuranceGroupContainer = new Wt::WContainerWidget();

    Wt::WLabel *lblGroup = new Wt::WLabel("Insurance Group:");
    lblGroup->setStyleClass("sig-lbl");
    Wt::WText *out = new Wt::WText();

    Wt::WHBoxLayout *insuranceGrouphbox = new Wt::WHBoxLayout();
    insuranceGrouphbox->addWidget(lblGroup);
    insuranceGrouphbox->addWidget(new Wt::WBreak());
    insuranceGrouphbox->addWidget(out);
    insuranceGrouphbox->addWidget(new Wt::WBreak());

    dbo::Transaction instrans(dbsession);

    typedef dbo::collection< dbo::ptr<InsuranceGroup> > Groups;
    Groups groups = dbsession.find<InsuranceGroup>();
    std::cerr << groups.size() << std::endl;

    std::vector<InsuranceGroupItem> groupitems;

    for (Groups::const_iterator i = groups.begin(); i != groups.end(); ++i)
    {

        Wt::WString name = (*i)->groupName_;
        Wt::WString id = (*i)->groupId_;

        InsuranceGroupItem *e = new InsuranceGroupItem();
        e->groupName = name;
        e->groupId = id;

        groupitems.push_back(*e);

    }
    log("notice") << "Current Number of insuranceGroup items "<< groupitems.size();

    instrans.commit();


    Wt::WComboBox *cbinsuranceGroup = new Wt::WComboBox();
    cbinsuranceGroup->setStyleClass("sig-left-box");
    Wt::WText *groupout = new Wt::WText();
    cbinsuranceGroup->addItem("");

    for (std::vector<InsuranceGroupItem>::const_iterator ev = groupitems.begin(); ev != groupitems.end(); ++ev)
    {
        cbinsuranceGroup->addItem(ev->groupName);
    }

    cbinsuranceGroup->changed().connect(std::bind([=] () {

        InsuranceGroupItem selected;

        for (std::vector<InsuranceGroupItem>::const_iterator ev = groupitems.begin(); ev != groupitems.end(); ++ev)
        {
            Wt::WString cbname;
            cbname = cbinsuranceGroup->currentText();

            if (ev->groupName == cbname)
            {
                selected = *ev;
            }
        }
        if (selected.groupName != "")
        {
            Wt::WString groupId_;
            Wt::WString groupName_;

            groupId_ = selected.groupId;
            groupName_ = selected.groupName;

            //save insuranceGroup change to practice
            dbo::Transaction ptrans(dbsession);

            dbo::ptr<Insurance> pat = dbsession.find<Insurance>().where("ins_id = ?").bind(ins.toUTF8());

            pat.modify()->insuranceGroupId_ = groupId_.toUTF8();
            pat.modify()->insuranceGroupName_ = groupName_.toUTF8();

            ptrans.commit();
            out->setText(groupName_);

        } else {
            log("notice") << "Selected: No Group";
        }

    }));

    cbinsuranceGroup->setCurrentIndex(0);
    cbinsuranceGroup->setMargin(10, Wt::Right);

    insuranceGrouphbox->addWidget(cbinsuranceGroup);
    insuranceGroupContainer->setLayout(insuranceGrouphbox);

    container->addWidget(insuranceGroupContainer);

    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    Wt::WPushButton *save = new Wt::WPushButton("Save", dialog->footer());
    save->setDefault(true);

    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);
    save->clicked().connect(dialog, &Wt::WDialog::accept);

    dialog->show();

}