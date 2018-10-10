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
#include "AdminShiftNoteWidget.h"
#include "Order.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "ShiftNote.h"
#include "Comment.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "UserSession.h"

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

AdminShiftNoteWidget::AdminShiftNoteWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at the backup list.)";


    dbsession.mapClass<ShiftNote>("shiftnote");
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

void AdminShiftNoteWidget::showNotes()
{

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<ShiftNote> > ShiftNotes;
    ShiftNotes shiftnotes = dbsession.find<ShiftNote>().orderBy("note_sent desc");
    std::cerr << shiftnotes.size() << std::endl;

    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Technician"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Shift Location"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Date Sent"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Note"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("View"));


    int row = 1;
    for (ShiftNotes::const_iterator i = shiftnotes.begin(); i != shiftnotes.end(); ++i, ++row)
    {

        new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->employeeEmail_, table->elementAt(row, 1)),
            new Wt::WText((*i)->shiftLocation_, table->elementAt(row, 2)),
            new Wt::WText((*i)->noteSent_.toString("MM/dd/yyyy h:mm a"), table->elementAt(row, 3)),
            new Wt::WText((*i)->noteBody_, table->elementAt(row, 4)),
            btnViewNote = new Wt::WPushButton("Details", table->elementAt(row, 5)),
            //click 'PDF' button on any table row, connect downloadReferral with that referral uuid
            btnViewNote->clicked().connect(boost::bind(&AdminShiftNoteWidget::showShiftNote, this, (*i)->noteId_));

    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addWidget(table);
    addWidget(new Wt::WText("<h3>Night Technician End of Shift Notes</h3>"));
    addWidget(result);

}

void AdminShiftNoteWidget::showShiftNote(Wt::WString sn)
{
    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("Shift Note");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    dbo::Transaction trans(dbsession);

    dbo::ptr<ShiftNote> shiftnote = dbsession.find<ShiftNote>().where("note_id = ?").bind(sn);

    WString technician = shiftnote->employeeEmail_;
    WString shiftloc = shiftnote->shiftLocation_;
    WString sent = shiftnote->noteSent_.toString("MM/dd/yyyy h:mm a");
    WString body = shiftnote->noteBody_;

    trans.commit();

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    container->addWidget(new Wt::WText("<h4>Technician: " + technician + "</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("<h4>Shift Location: " + shiftloc + "</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("<h4>Note Sent: " + sent + "</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText("<h4>Shift Note:</h4>"));
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WText(body));
    
    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
