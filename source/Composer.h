// This may look like C code, but it's really -*- C++ -*-
/*
* Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
*
* See the LICENSE file for terms of use.
*/
#ifndef COMPOSER_H_
#define COMPOSER_H_

#include <Wt/WCompositeWidget>

#include "Attachment.h"

namespace Wt {
    class WLineEdit;
    class WPushButton;
    class WTable;
    class WText;
    class WTextArea;
}

class AddresseeEdit;
class AttachmentEdit;
class ContactSuggestions;
class OptionList;
class Option;

using namespace Wt;

/**
* @addtogroup composerexample
*/
/*@{*/

/*! \brief An E-mail composer widget.
*
* This widget is part of the %Wt composer example.
*/
class Composer : public WCompositeWidget
{
public:
    /*! \brief Construct a new Composer
    */
    Composer(const char *conninfo, WString ref, WContainerWidget *parent = 0);


    /*! \brief Get the list of attachments.
    *
    * The ownership of the attachment spool files is transferred
    * to the caller as well, be sure to delete them !
    */
    std::vector<Attachment> attachments() const;

    /*! \brief Get the message.
    */
    const WString& message() const;

    WString refId;

public:
    /*! \brief The message is ready to be sent...
    */
    Wt::Signal<void> send;

    /*! \brief The message must be discarded.
    */
    Wt::Signal<void> discard;

private:
    WContainerWidget *layout_;
    WPushButton      *topSendButton_, *topSaveNowButton_, *topDiscardButton_;
    WPushButton      *botSendButton_, *botSaveNowButton_, *botDiscardButton_;
    WText            *statusMsg_;

    WTable           *edits_;
    //! Option for attaching a file.
    Option           *attachFile_;
    //! Option for attaching another file.
    Option           *attachOtherFile_;

    //! Array which holds all the attachments, including one extra invisible one.
    std::vector<AttachmentEdit *> attachments_;

    //! state when waiting asyncrhonously for attachments to be uploaded
    bool saving_, sending_;

    //! number of attachments waiting to be uploaded during saving
    int attachmentsPending_;

    /*!\brief Add an attachment edit.
    */
    void attachMore(const char *conninfo, WString ref);

    /*!\brief Remove the given attachment edit.
    */
    void removeAttachment(AttachmentEdit *attachment);

    /*! \brief Slot attached to the Send button.
    *
    * Tries to save the mail message, and if succesfull, sends it.
    */
    void sendIt();

    /*! \brief Slot attached to the Save now button.
    *
    * Tries to save the mail message, and gives feedback on failure
    * and on success.
    */
    void saveNow();

    /*! \brief Slot attached to the Discard button.
    *
    * Discards the current message: emits the discard event.
    */
    void discardIt();

    /*! \brief Slotcalled when an attachment has been uploaded.
    *
    * This used during while saving the email and waiting
    * for remaining attachments to be uploaded. It is connected
    * to the AttachmentEdit control signals that are emitted when
    * an attachment has been processed.
    */
    void attachmentDone();

private:
    // create the user-interface
    void createUi(const char *conninfo, WString ref);

    /*! \brief All attachments have been processed, determine the result
    *         of saving the message.
    */
    void saved();

    /*! \brief Set the status, and apply the given style.
    */
    void setStatus(const WString& text, const WString& style);

    friend class AttachmentEdit;
};

/*@}*/

#endif // COMPOSER_H_
