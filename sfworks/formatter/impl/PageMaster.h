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

#ifndef FORMATTER_PAGE_MASTER_H
#define FORMATTER_PAGE_MASTER_H

#include <map>
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

#include "grove/Node.h"

#include "formatter/formatter_defs.h"
#include "formatter/types.h"
#include "formatter/MediaInfo.h"
#include "formatter/impl/Property.h"

namespace Formatter
{

class RegionSpecs : public COMMON_NS::RefCounted<>,
                    public PropertySet {
public:
    RegionSpecs(PropertySet* parentSet, const GroveLib::Node* foNode,
                const MediaInfo& mediaInfo, const CRange& parentSize);
    //!
    const COMMON_NS::String&    name() const { return name_; }
    //!
    const CRect&            viewport() const { return regionRect_; }
    //!
    const ORect&            margin() const { return margin_; }

private:
    //!
    const GroveLib::Node*   node() const { return node_.pointer(); }
    //!
    double                  dpi() const { return dpi_; }
private:
    ConstNodePtr            node_;
    double                  dpi_;
    COMMON_NS::String       name_;
    ORect                   margin_;
    CRect                   regionRect_;
};

typedef COMMON_NS::RefCntPtr<RegionSpecs> RegionSpecsPtr;

/*
 */
class PageSpecs : public PropertySet {
public:
    typedef std::map<COMMON_NS::String, RegionSpecsPtr> RegionSpecsMap;

    PageSpecs(PropertySet* parentSet)
        : PropertySet(parentSet) {};

    //!
    const CRange&   pageSize() const { return pageSize_; }
    //!
    const ORect&    margin() const { return margin_; }
    //!
    RegionSpecsPtr  getRegionSpecs(const COMMON_NS::String& name) const;
protected:
    //!
    void            addRegionSpecs(const RegionSpecsPtr& region);
protected:
    ORect           margin_;
    CRange          pageSize_;
private:
    RegionSpecsMap  regions_;
};

/*
 */
class PageMaster : public COMMON_NS::RefCounted<> {
public:
    FORMATTER_OALLOC(PageMaster);

    //! Returns the page specs for the N`th page in page sequence
    virtual PageSpecs&  getPageSpecs(uint pageNum) = 0;
    virtual ~PageMaster() {}
};

typedef COMMON_NS::RefCntPtr<PageMaster> PageMasterPtr;

/*
 */
class PageMasterMap {
public:
    typedef std::map<COMMON_NS::String, PageMasterPtr> MasterMap;

    PageMasterMap(PropertySet* parentSet, const GroveLib::Node* foNode,
                  const MediaInfo& mediaInfo,
                  COMMON_NS::MessageStream& mstream);
    //!
    PageMasterPtr   getPageMaster(const COMMON_NS::String& name) const;
private:
    MasterMap       masters_;
};

}

#endif  // FORMATTER_PAGE_MASTER_H
