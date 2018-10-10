/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STUDY_ATTACHMENT_H_
#define STUDY_ATTACHMENT_H_

#include <string>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class StudyAttachment
{
public:
        /*! \brief The file name.
        */
        std::wstring fileName;

        /*! \brief The content description.
        */
        std::wstring contentDescription;

        /*! \brief the spooled file name.
        */
        std::string spoolFileName;

        /*! \brief Create an attachment.
        */
        StudyAttachment(const std::wstring aFileName,
                const std::wstring aContentDescription,
                const std::string aSpoolFileName)
                : fileName(aFileName),
                contentDescription(aContentDescription),
                spoolFileName(aSpoolFileName)
        { }

};

class StudyAttachmentDb : public CompositeEntity
{
public:
        Wt::WString attFileName_;
        Wt::WString attId_;
        Wt::WString tempRefId_;
        Wt::WString stdId_;
        Wt::WString attContentDescription_;
        //dbo template for attachment
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);

                dbo::field(a, attFileName_, "att_filename");
                dbo::field(a, tempRefId_, "temp_ref");
                dbo::field(a, stdId_, "study_id");
                dbo::field(a, attId_, "att_id");
                dbo::field(a, attContentDescription_, "att_mime");

        }
};

/*@}*/

#endif // STUDY_ATTACHMENT_H_
