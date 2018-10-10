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
#include "TechShiftNoteWidget.h"
#include "ShiftNote.h"

#include "UserSession.h"

using namespace Wt;
namespace dbo = Wt::Dbo;

TechShiftNoteWidget::TechShiftNoteWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
    WContainerWidget(parent),
    conninfo_(conninfo),
    pg_(conninfo),
    session_(session)
{
    dbsession.setConnection(pg_);

    pg_.setProperty("show-queries", "true");

    Wt::WString username = session_->strUserEmail();
    Wt::log("notice") << "(The staff member: " << username << "is looking at the backup list.)";

    dbsession.mapClass<ShiftNote>("shiftnote");
    dbsession.mapClass<CompositeEntity>("composite");

}

void TechShiftNoteWidget::noteList()
{
    Wt::WContainerWidget *container = new Wt::WContainerWidget();

    Wt::WPushButton *newShiftNote = new Wt::WPushButton("New End of Shift Note");
    newShiftNote->setDefault(true);

    container->addWidget(newShiftNote);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());

    newShiftNote->clicked().connect(std::bind([=] () {
        TechShiftNoteWidget::newShiftNoteDialog();
    }));

    addWidget(container);

}

WString TechShiftNoteWidget::setShiftNoteId()
{
    return uuid();
}

void TechShiftNoteWidget::newShiftNoteDialog()
{
    staticShiftNote_ = new Wt::WText();
    staticShiftNote_->setText(setShiftNoteId());

    //write dialog for Tech to do something with referral. View details and Download PDF will be a couple of functions.
    Wt::WDialog *dialog = new Wt::WDialog("End of Shift Note");
    dialog->setMinimumSize(1000, 700);
    dialog->setMaximumSize(1000, 700);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(dialog->contents());

    Wt::WString username = session_->strUserEmail();
    
    container->addWidget(new Wt::WText("<h4>Employee Email: "+username+"</h4>"));

    Wt::WComboBox *cb = new Wt::WComboBox();
    cb->setStyleClass("cal-month-selection");
    cb->addItem("Select Location");
    cb->addItem("Downey");
    cb->addItem("Long Beach");

    cb->setCurrentIndex(0); // Show 'Today's Month Number index initially.
    // cb->setMargin(10, Wt::Right);
    container->addWidget(cb);

    Wt::WLabel *noteBodyLbl = new Wt::WLabel("End of Shift Notes:");
    Wt::WTextArea *noteBody = new Wt::WTextArea();

    container->addWidget(noteBodyLbl);
    container->addWidget(new Wt::WBreak());
    container->addWidget(noteBody);



    Wt::WPushButton *save = new Wt::WPushButton("Save Shift Note", dialog->footer());
    save->setDefault(true);

    save->clicked().connect(std::bind([=] () {

        Wt::WString bodyOut = noteBody->text();

        Wt::Dbo::Transaction htrans(dbsession);
        Wt::Dbo::ptr<ShiftNote> sn = dbsession.add(new ShiftNote());

        sn.modify()->employeeEmail_ = username.toUTF8();
        sn.modify()->shiftLocation_ = cb->currentText().toUTF8();
        sn.modify()->noteSent_ = Wt::WDateTime::currentDateTime();
        sn.modify()->noteBody_ = bodyOut.toUTF8();
        sn.modify()->noteId_ = staticShiftNote_->text().toUTF8();

        htrans.commit();

        Mail::Message message;
        message.setFrom(Mail::Mailbox("clinicore@statsleep.com", "Clinicore"));
        message.addRecipient(Mail::To, Mail::Mailbox("shiftnotes@statsleep.com", "USC Shift Notes"));
        message.setSubject("Technician End of Shift Notes");
        message.setBody("Employee Name: "+username.toUTF8() + " Shift Location: "+cb->currentText().toUTF8()+" Shift Notes: "+bodyOut.toUTF8());
        message.addHtmlBody ("<p>Employee Name: "+username.toUTF8()+"</p><br><p>Shift Location: "+cb->currentText().toUTF8()+"</p><br><p>Shift Notes: "+bodyOut.toUTF8()+"</p><br><br>Please <a href=\"https://clinicore.statsleep.com/admin/shift-notes\">Log In to the Admin Area</a> to check details.</p><br><p>Thanks,</p><p>Clinicore</p>");
        Mail::Client client;
        client.connect("localhost");
        client.send(message);

        dialog->accept();

    }));


    container->show();

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    dialog->show();

}
