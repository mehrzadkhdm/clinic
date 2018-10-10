// This may look like C code, but it's really -*- C++ -*-
/*
* Copyright (C) 2015 Cody Scherer, PES7 Inc. <cody@pes7.com>
*
* See the LICENSE file for terms of use.
*/
#ifndef STUDY_ATTACHMENT_EDIT_H_
#define STUDY_ATTACHMENT_EDIT_H_

#include <Wt/WContainerWidget>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

namespace Wt {
    class WFileUpload;
    class WText;
    class WCheckBox;
}

class StudyAttachment;
class StudyComposer;
class Option;

using namespace Wt;
using namespace Wt::Dbo;

/**
* @addtogroup composerexample
*/
/*@{*/

/*! \brief An edit field for an email StudyAttachment.
*
* This widget managements one StudyAttachment edit: it shows a file upload
* control, handles the upload, and gives feed-back on the file
* uploaded.
*
* This widget is part of the %Wt composer example.
*/
class StudyAttachmentEdit : public WContainerWidget
{
public:
    /*! \brief Creates an StudyAttachment edit field.
    */
    StudyAttachmentEdit(const char *conninfo, StudyComposer *composer, WString ref, WContainerWidget *parent = 0);

    /*! \brief Updates the file now.
    *
    * Returns whether a new file will be uploaded. If so, the uploadDone
    * signal will be signalled when the file is uploaded (or failed to
    * upload).
    */
    bool uploadNow();

    /*! \brief Returns whether the upload failed.
    */
    bool uploadFailed() const { return uploadFailed_; }

    /*! \brief Returns the StudyAttachment.
    */
    std::vector<StudyAttachment> attachments();

    /*! \brief Signal emitted when new StudyAttachment(s) have been uploaded (or failed
    *         to upload.
    */
    Signal<void>& uploadDone() { return uploadDone_; }

private:
    StudyComposer    *composer_;

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

    Signal<void> uploadDone_;

    //! The WFileUpload control.
    WFileUpload *upload_;

    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }


    class UploadInfo : public WContainerWidget
    {
    public:
        UploadInfo(const Http::UploadedFile& f, WContainerWidget *parent = 0);

        Http::UploadedFile info_;

        //! Anchor referencing the file.
        WAnchor   *downloadLink_;

        //! The check box to keep or discard the uploaded file.
        WCheckBox *keep_;
    };

    std::vector<UploadInfo *> uploadInfo_;

    //! The text box to display an error (empty or too big file)
    WText *error_;

    //for the UUID
    Wt::WString setRefId();
    Wt::WString strRefId();
    Wt::WText *reviewRefId_;

    Wt::WString strReferralId();
    Wt::WText *referralId_;

    Wt::WText *staticRef_;
    Wt::WText *outRefId_;

    //! The option to cancel the file upload
    Option *remove_;

    //! The state of the last upload process.
    bool uploadFailed_;

    //! Slot triggered when the WFileUpload completed an upload.
    void uploaded();

    //! Slot triggered when the WFileUpload received an oversized file.
    void fileTooLarge(::int64_t size);

    //! Slot triggered when the users wishes to remove this StudyAttachment edit.
    void remove();
};

/*@}*/

#endif // STUDY_ATTACHMENT_EDIT_H_
