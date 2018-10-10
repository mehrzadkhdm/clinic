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

#include <Wt/WDate>
#include <Wt/WTime>

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
#include "TechBackupListWidget.h"
#include "UserSession.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Backup.h"
#include "Comment.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InboundBackupApp{
    struct InboundBackup {
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;
        Wt::WString ptHomePhone;

        InboundBackup(const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob,
            const Wt::WString& aPtHomePhone)
            : ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob),
            ptHomePhone(aPtHomePhone){}
    };
}
namespace InboundBackupApp{
    struct InboundComment {
        Wt::WString staffEmail;
        Wt::WString commentText;
        Wt::WString createdAt;

        InboundComment(const Wt::WString& aStaffEmail,
            const Wt::WString& aCommentText,
            const Wt::WString& aCreatedAt)
            : staffEmail(aStaffEmail),
            commentText(aCommentText),
            createdAt(aCreatedAt){}
    };
}

TechBackupListWidget::TechBackupListWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The technician: " << username << "is looking at the backup list.)";

    dbsession.mapClass<Backup>("backup");
    dbsession.mapClass<CompositeEntity>("composite");
    dbsession.mapClass<Comment>("comment");

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

void TechBackupListWidget::backupList()
{

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<Backup> > Backups;
    Backups backups = dbsession.find<Backup>();
    std::cerr << backups.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("First Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Last Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Date of Birth"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Home Phone"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Cell Phone"));
    table->elementAt(0, 6)->addWidget(new Wt::WText("More"));


    int row = 1;
    for (Backups::const_iterator i = backups.begin(); i != backups.end(); ++i, ++row)
    {

        new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->ptFirstName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->ptLastName_, table->elementAt(row, 2)),
            new Wt::WText((*i)->ptDob_, table->elementAt(row, 3)),
            new Wt::WText((*i)->ptHomePhone_, table->elementAt(row, 4)),
            new Wt::WText((*i)->ptCellPhone_, table->elementAt(row, 5)),
            btnViewBackup = new Wt::WPushButton("Details", table->elementAt(row, 6)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnViewBackup->clicked().connect(boost::bind(&TechBackupListWidget::showBackupDialog, this, (*i)->buId_));

    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addWidget(table);
    addWidget(new Wt::WText("<h3>No-Show Backup Patients</h3>"));
    addWidget(result);

}

WString TechBackupListWidget::setCommentId()
{
    return uuid();
}

void TechBackupListWidget::showBackupDialog(std::string& bu)
{
    staticComment_ = new Wt::WText();
    staticComment_->setText(setCommentId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Backup Patient");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    dbo::Transaction trans(dbsession);

    dbo::ptr<Backup> backup = dbsession.find<Backup>().where("bu_id = ?").bind(bu);

    WString pfname = backup->ptFirstName_;
    WString plname = backup->ptLastName_;
    WString pdob = backup->ptDob_;
    WString phome = backup->ptHomePhone_;
    WString pcell = backup->ptCellPhone_;
    WString buid = backup->buId_;

    trans.commit();

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    Wt::WHBoxLayout *hbox2 = new Wt::WHBoxLayout();
    container->setLayout(hbox);
    container->addStyleClass("backuplistcont");

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    Wt::WVBoxLayout *vbox1 = new Wt::WVBoxLayout();


    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);

    Wt::WContainerWidget *contactInfo = new Wt::WContainerWidget();
    contactInfo->addWidget(new Wt::WText("<h4>Patient Contact Info:</h4>"));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("First Name: " + pfname));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Last Name: " + plname));
    contactInfo->addWidget(new Wt::WBreak());
    contactInfo->addWidget(new Wt::WText("Home Phone: " + phome));
    contactInfo->addWidget(new Wt::WText("Cell Phone: " + pcell));

    vbox1->addWidget(contactInfo);

    dbo::Transaction ctrans(dbsession);

    typedef dbo::collection< dbo::ptr<Comment> > Comments;
    Comments comments = dbsession.find<Comment>().where("backup_id = ?").bind(bu);
    std::cerr << comments.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("User"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Comment"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Added"));


    int row = 1;
    for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i, ++row)
    {
            new Wt::WText((*i)->staffEmail_, table->elementAt(row, 0)),
            new Wt::WText((*i)->commentText_, table->elementAt(row, 1)),
            new Wt::WText((*i)->createdAt_.toString(), table->elementAt(row, 2));

    }

    ctrans.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addStyleClass("backup-comments");
    result->addWidget(table);

    vbox2->addWidget(new Wt::WText("<h3>Comments</h3>"));
    vbox2->addWidget(result);

    vbox2->addWidget(new Wt::WText("<h4>Add Comment</h4>"));
    Wt::WTextEdit *edit = new Wt::WTextEdit();
    edit->setHeight(100);
    vbox2->addWidget(edit);

    Wt::WText *editout = new Wt::WText();



    Wt::WPushButton *save = new Wt::WPushButton("Add Comment");
    vbox2->addWidget(save);
    save->clicked().connect(std::bind([=] () {

        editout->setText("<pre>" + Wt::Utils::htmlEncode(edit->text()) + "</pre>");
        Wt::WString commentTextOut_ = editout->text().toUTF8();

        Wt::WString staffEmail = session_->strUserEmail();
        Wt::WString backupId = bu;
        Wt::Dbo::Transaction htrans(dbsession);

        Wt::Dbo::ptr<Comment> c = dbsession.add(new Comment());

        c.modify()->staffEmail_ = staffEmail.toUTF8();
        c.modify()->commentText_ = commentTextOut_;
        c.modify()->buId_ = backupId.toUTF8();

        //h.modify()->studentFirstName_ = studentFirst.toUTF8();
        //h.modify()->studentLastName_ = studentLast.toUTF8();
        //h.modify()->studentId_ = studentId.toUTF8();

        c.modify()->commentId_ = staticComment_->text().toUTF8();

        htrans.commit();

        dialog->accept();

    }));


    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
