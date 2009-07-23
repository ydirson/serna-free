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

#include "formatter/impl/formatter_utils.h"
#include "formatter/Exception.h"
#include "formatter/impl/Fo.h"
#include "formatter/impl/PageMaster.h"
#include "formatter/impl/Properties.h"
#include "formatter/impl/BorderProperties.h"

USING_GROVE_NAMESPACE;
USING_COMMON_NS;

namespace Formatter
{

RegionSpecs::RegionSpecs(PropertySet* parentSet, const Node* foNode,
                         const MediaInfo& mediaInfo, const CRange& parentSize)
    : PropertySet(parentSet),
      node_(foNode),
      dpi_(mediaInfo.dpi_),
      name_(get_attr_value(foNode, "region-name"))
{
    if (name_.isEmpty())
        name_ = "xsl-region-body";

    Allocation alloc;
    alloc.space_ = CRect(CPoint(0, 0), parentSize);
    alloc.maxw_ = alloc.maxh_ = true;
    margin_ = getMargin(*this, alloc);
    regionRect_ = CRect(CPoint(margin_.left_, margin_.top_),
                        CRange(parentSize.w_ - margin_.left_ - margin_.right_,
                               parentSize.h_ - margin_.top_ -margin_.bottom_));
}

/*
 */
RegionSpecsPtr PageSpecs::getRegionSpecs(const String& name) const
{
    RegionSpecsMap::const_iterator i = regions_.find(name);
    if (regions_.end() != i)
        return (*i).second;
    return 0;
}

void PageSpecs::addRegionSpecs(const RegionSpecsPtr& region)
{
    RegionSpecsMap::const_iterator i = regions_.find(region->name());
    if (regions_.end() != i) {
        throw Formatter::Exception(XslMessages::foRegionDup, region->name());
    }
    regions_[region->name()] = region;
}

}
