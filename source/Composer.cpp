/*
* Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
*
* See the LICENSE file for terms of use.
*/
#include <iostream>
#include <functional>
#include "AttachmentEdit.h"
#include "Composer.h"
#include "Option.h"
#include "OptionList.h"

#include <Wt/WContainerWidget>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WStringUtil>
using namespace std;
Composer::Composer(const char *conninfo, WString ref, WContainerWidget *parent)
    : WCompositeWidget(parent),
    saving_(false),
    sending_(false)
{
    setImplementation(layout_ = new WContainerWidget());

    createUi(conninfo, ref);
}


std::vector<Attachment> Composer::attachments() const
{
    std::vector<Attachment> attachments;

    for (unsigned i = 0; i < attachments_.size() - 1; ++i) {
        std::vector<Attachment> toadd = attachments_[i]->attachments();

        attachments.insert(attachments.end(), toadd.begin(), toadd.end());
    }

    return attachments;
}



void Composer::createUi(const char *conninfo, WString ref)
{
    // horizontal layout container, used for top and bottom buttons.
    WContainerWidget *horiz;

    /*
    * Top buttons
    */
    horiz = new WContainerWidget(layout_);
    horiz->setPadding(5);

    /*
    * Attachments
    */
    edits_ = new WTable(layout_);
    edits_->resize(WLength(100, WLength::Percentage), WLength::Auto);
    edits_->elementAt(0, 0)->resize(WLength(1, WLength::Percentage),
        WLength::Auto);

    new WImage("icons/paperclip.png", edits_->elementAt(5, 0));
    edits_->elementAt(5, 0)->setContentAlignment(AlignRight | AlignTop);
    edits_->elementAt(5, 0)->setPadding(3);

    // Attachment edits: we always have the next attachmentedit ready
    // but hidden. This improves the response time, since the show()
    // and hide() slots are stateless.
    attachments_.push_back(new AttachmentEdit(conninfo, this, ref, edits_->elementAt(5, 1)));
    attachments_.back()->hide();

    /*
    * Two options for attaching files. The first does not say 'another'.
    */
    attachFile_ = new Option(tr("msg.attachfile"),
        edits_->elementAt(5, 1));
    attachOtherFile_ = new Option(tr("msg.attachanother"),
        edits_->elementAt(5, 1));
    attachOtherFile_->hide();


    /*
    * Option event to attach the first attachment.
    *
    * We show the first attachment, and call attachMore() to prepare the
    * next attachment edit that will be hidden.
    *
    * In addition, we need to show the 'attach More' option, and hide the
    * 'attach' option.
    */
    attachFile_->item()->clicked().connect(attachments_.back(), &WWidget::show);
    attachFile_->item()->clicked().connect(attachOtherFile_, &WWidget::show);
    attachFile_->item()->clicked().connect(attachFile_, &WWidget::hide);

    attachFile_->item()->clicked().connect(std::bind([=]() {
        Composer::attachMore(conninfo, ref);
    }));

    attachOtherFile_->item()->clicked().connect(std::bind([=]() {
        Composer::attachMore(conninfo, ref);
    }));
}

void Composer::attachMore(const char *conninfo, WString ref)
{
    /*
    * Create and append the next AttachmentEdit, that will be hidden.
    */
    AttachmentEdit *edit = new AttachmentEdit(conninfo, this, ref);
    edits_->elementAt(5, 1)->insertBefore(edit, attachOtherFile_);
    attachments_.push_back(edit);
    attachments_.back()->hide();

    // Connect the attachOtherFile_ option to show this attachment.
    attachOtherFile_->item()->clicked()
        .connect(attachments_.back(), &WWidget::show);
}

void Composer::removeAttachment(AttachmentEdit *attachment)
{
    /*
    * Remove the given attachment from the attachments list.
    */
    std::vector<AttachmentEdit *>::iterator i
        = std::find(attachments_.begin(), attachments_.end(), attachment);

    if (i != attachments_.end()) {
        attachments_.erase(i);
        delete attachment;

        if (attachments_.size() == 1) {
            /*
            * This was the last visible attachment, thus, we should switch
            * the option control again.
            */
            attachOtherFile_->hide();
            attachFile_->show();
            attachFile_->item()->clicked()
                .connect(attachments_.back(), &WWidget::show);
        }
    }
}

void Composer::sendIt()
{
    if (!sending_) {
        sending_ = true;

        /*
        * First save -- this will check for the sending_ state
        * signal if successfull.
        */
        saveNow();
    }
}

void Composer::saveNow()
{
    if (!saving_) {
        saving_ = true;

        /*
        * Check if any attachments still need to be uploaded.
        * This may be the case when fileupload change events could not
        * be caught (for example in Konqueror).
        */
        attachmentsPending_ = 0;

        for (unsigned i = 0; i < attachments_.size() - 1; ++i) {
            if (attachments_[i]->uploadNow()) {
                ++attachmentsPending_;

                // this will trigger attachmentDone() when done, see
                // the AttachmentEdit constructor.
            }
        }

        std::cerr << "Attachments pending: " << attachmentsPending_ << std::endl;
        if (attachmentsPending_)
            setStatus(tr("msg.uploading"), "status");
        else
            saved();
    }
}

void Composer::attachmentDone()
{
    if (saving_) {
        --attachmentsPending_;
        std::cerr << "Attachments still: " << attachmentsPending_ << std::endl;

        if (attachmentsPending_ == 0)
            saved();
    }
}

void Composer::setStatus(const WString& text, const WString& style)
{
    statusMsg_->setText(text);
    statusMsg_->setStyleClass(style);
}

void Composer::saved()
{
    /*
    * All attachments have been processed.
    */

    bool attachmentsFailed = false;
    for (unsigned i = 0; i < attachments_.size() - 1; ++i)
        if (attachments_[i]->uploadFailed()) {
            attachmentsFailed = true;
            break;
        }

    if (attachmentsFailed) {
        setStatus(tr("msg.attachment.failed"), "error");
    }
    else {
#ifndef WIN32
        time_t t = time(0);
        struct tm td;
        gmtime_r(&t, &td);
        char buffer[100];
        strftime(buffer, 100, "%H:%M", &td);
#else
        char buffer[] = "server"; // Should fix this; for now just make sense
#endif
        setStatus(tr("msg.ok"), "status");
        statusMsg_->setText(std::string("Draft saved at ") + buffer);

        if (sending_) {
            send.emit();
            return;
        }
    }

    saving_ = false;
    sending_ = false;
}

void Composer::discardIt()
{
    discard.emit();
}
