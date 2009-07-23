// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
/*! \file
 */

#ifndef FORMATTER_PAGE_MASTERS_H
#define FORMATTER_PAGE_MASTERS_H

#include "common/CDList.h"
#include "grove/Node.h"
#include "formatter/MediaInfo.h"
#include "formatter/impl/PageMaster.h"

namespace Formatter
{

/*!
 */
class SimpleMaster : public PageMaster,
                     private PageSpecs {
public:
    FORMATTER_OALLOC(SimpleMaster);

    SimpleMaster(PropertySet* parentSet, const GroveLib::Node* foNode,
                 const MediaInfo& mediaInfo,
                 COMMON_NS::MessageStream& mstream);
    //!
    const COMMON_NS::String&    name() const { return name_; }
    //!
    PageSpecs&              getPageSpecs(uint) { return *this; }
private:
    //!
    const GroveLib::Node*   node() const { return node_.pointer(); }
    //!
    double                  dpi() const { return dpi_; }
private:
    ConstNodePtr            node_;
    double                  dpi_;
    COMMON_NS::String       name_;
    CRect                   contRect_;
};

/*!
 */
class MasterReference : public COMMON_NS::CDListItem<MasterReference> {
public:
    MasterReference(const GroveLib::Node* foNode,
                    const PageMasterMap& pageMasterMap);
    //!
    PageSpecs&      getPageSpecs();
    //!
    bool            isUnbounded() const;
    //
    uint            maxRepeats() const;
private:
    PageMasterPtr   pageMaster_;
    long            maxRepeats_;
};

/*! brief
 */
class SequenceMaster : public PageMaster,
                       private COMMON_NS::CDList<MasterReference> {
public:
    FORMATTER_OALLOC(SequenceMaster);

    SequenceMaster(const GroveLib::Node* foNode,
                   const PageMasterMap& pageMasterMap,
                   COMMON_NS::MessageStream& mstream);
    //!
    const COMMON_NS::String&    name() const { return name_; }
    //!
    PageSpecs&          getPageSpecs(uint pageNum);
private:
    COMMON_NS::String   name_;
};

}

#endif  // FORMATTER_PAGE_MASTERS_H
