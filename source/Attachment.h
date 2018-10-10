/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef ATTACHMENT_H_
#define ATTACHMENT_H_

#include <string>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include "CompositeEntity.h"

namespace dbo = Wt::Dbo;

class Attachment
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
        Attachment(const std::wstring aFileName,
                const std::wstring aContentDescription,
                const std::string aSpoolFileName)
                : fileName(aFileName),
                contentDescription(aContentDescription),
                spoolFileName(aSpoolFileName)
        { }

};

class AttachmentDb : public CompositeEntity
{
public:
        Wt::WString attFileName_;
        Wt::WString attId_;
        Wt::WString ptId_;
        Wt::WString tempRefId_;
        Wt::WString attContentDescription_;
        //dbo template for attachment
        template<class Action>
        void persist(Action& a)
        {
                CompositeEntity::persist(a);

                dbo::field(a, attFileName_, "att_filename");
                dbo::field(a, tempRefId_, "temp_ref");
                dbo::field(a, attId_, "att_id");
                dbo::field(a, attContentDescription_, "att_mime");
                dbo::field(a, ptId_, "pt_id" );

        }
};

/*@}*/

#endif // ATTACHMENT_H_
