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

#include "common/RefCntPtr.h"
#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"

#include "formatter/impl/debug.h"
#include "formatter/Exception.h"
#include "formatter/impl/Fo.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/impl/Properties.h"
#include "formatter/impl/BorderProperties.h"
#include "formatter/impl/PageMasters.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Formatter
{

/*
 */
PageMasterMap::PageMasterMap(PropertySet* parentSet,
                             const GroveLib::Node* foNode,
                             const MediaInfo& mediaInfo,
                             MessageStream& mstream)
{
    Node* child = foNode->firstChild();
    while (child) {
        switch (Fo::foName(child)) {
            case SIMPLE_PAGE_MASTER :
                try {
                    RefCntPtr<SimpleMaster> master =
                        new SimpleMaster(parentSet, child, mediaInfo, mstream);
                    if (masters_.end() == masters_.find(master->name()))
                        masters_[master->name()] = master;
                    else
                        mstream << XslMessages::pageMasterDuplicated
                                << Message::L_INFO
                                << child->nodeName() << master->name();
                }
                catch (XslException& e) {
                    mstream << XslMessages::creationError << Message::L_ERROR
                            << child->nodeName() << e.what();
                    DDBG << e.what() << std::endl;
                }
                break;
            case PAGE_SEQUENCE_MASTER :
                try {
                    RefCntPtr<SequenceMaster> master =
                        new SequenceMaster(child, *this, mstream);
                    if (masters_.end() == masters_.find(master->name()))
                        masters_[master->name()] = master;
                    else
                        mstream << XslMessages::pageMasterDuplicated
                                << Message::L_INFO
                                << child->nodeName() << master->name();
                }
                catch (XslException& e) {
                    mstream << XslMessages::creationError << Message::L_ERROR
                            << child->nodeName() << e.what();
                }
                break;
            default:
                mstream << XslMessages::foNotSupported
                        << Message::L_WARNING << child->nodeName();
                break;
        }
        child = child->nextSibling();
    }
    if (masters_.empty())
        throw Formatter::Exception(XslMessages::noPageMasters);
}

PageMasterPtr PageMasterMap::getPageMaster(const String& name) const
{
    MasterMap::const_iterator i = masters_.find(name);
    if (masters_.end() != i)
        return (*i).second;
    return 0;
}

/*
 */
SimpleMaster::SimpleMaster(PropertySet* parentSet,
                           const GroveLib::Node* foNode,
                           const MediaInfo& mediaInfo,
                           MessageStream& mstream)
    : PageSpecs(parentSet),
      node_(foNode),
      dpi_(mediaInfo.dpi_),
      name_(get_attr_value(foNode, "master-name"))
{
    if (name_.isNull())
        throw Formatter::Exception(XslMessages::noMasterName);

    Allocation alloc;
    PageWidth& page_width   = getProperty<PageWidth>(alloc);
    PageHeight& page_height = getProperty<PageHeight>(alloc);

    pageSize_ = CRange(page_width.value(), page_height.value());
    alloc.space_ = CRect(CPoint(0, 0), pageSize_);
    margin_ = getMargin(*this, alloc);
    contRect_ = CRect(CPoint(margin_.left_, margin_.top_),
                      CRange(pageSize_.w_ - margin_.left_ - margin_.right_,
                             pageSize_.h_ - margin_.top_ - margin_.bottom_));

    // Find and initialize the regions
    Node* child = foNode->firstChild();
    while (child){
        switch (Fo::foName(child)) {
            case REGION_BODY :
                {
                    try {
                        RegionSpecsPtr region = new RegionSpecs(
                            this, child, mediaInfo, contRect_.extent_);
                        addRegionSpecs(region);
                    }
                    catch (XslException& e) {
                        mstream << XslMessages::creationError
                                << Message::L_ERROR
                                << child->nodeName() << e.what();
                    }
                }
                break;
            default:
                mstream << XslMessages::foNotSupported
                        << Message::L_WARNING << child->nodeName();
                break;
        }
        child = child->nextSibling();
    }
}

/*
 */
MasterReference::MasterReference(const GroveLib::Node* foNode,
                                 const PageMasterMap& pageMasterMap)
    : maxRepeats_(-1)
{
    String master_ref = get_attr_value(foNode, "master-reference");
    if (master_ref.isNull())
        throw Formatter::Exception(XslMessages::noMasterReferenceSpec);
    pageMaster_ = pageMasterMap.getPageMaster(master_ref);
    if (pageMaster_.isNull()) {
        throw Formatter::Exception(XslMessages::noPageMasterDeclared,
                                                                   master_ref);
    }
    if (REPEATABLE_PAGE_MASTER_REFERENCE == Fo::foName(foNode)) {
        String max_repeats = get_attr_value(foNode, "maximum-repeats");
        if (!max_repeats.isEmpty()) {
            bool ok = false;
            maxRepeats_ = max_repeats.toInt(&ok);
            if (!ok)
                maxRepeats_ = -1;
        }
    }
}

PageSpecs& MasterReference::getPageSpecs()
{
    return pageMaster_->getPageSpecs(0);
}

bool MasterReference::isUnbounded() const
{
    return (0 > maxRepeats_);
}

uint MasterReference::maxRepeats() const
{
    if (0 > maxRepeats_)
        return 0;
    return maxRepeats_;
}

/*
 */
SequenceMaster::SequenceMaster(const GroveLib::Node* foNode,
                               const PageMasterMap& pageMasterMap,
                               MessageStream& mstream)
    : name_(get_attr_value(foNode, "master-name"))
{
    if (name_.isNull())
        throw Formatter::Exception(XslMessages::noMasterNameSeq);

    Node* child = foNode->firstChild();
    while (child){
        switch (Fo::foName(child)) {
            case SINGLE_PAGE_MASTER_REFERENCE :
            case REPEATABLE_PAGE_MASTER_REFERENCE :
                {
                    try {
                        push_back(new MasterReference(child, pageMasterMap));
                    }
                    catch (XslException& e) {
                       mstream << XslMessages::creationError
                               << Message::L_ERROR
                               << child->nodeName() << e.what();
                    }
                }
                break;
            default:
                mstream << XslMessages::foNotSupported
                        << Message::L_WARNING << child->nodeName();
                break;
        }
        child = child->nextSibling();
    }
    if (isEmpty())
        throw Formatter::Exception(XslMessages::noMasterRefSeq);
}

PageSpecs& SequenceMaster::getPageSpecs(uint pageNum)
{
    for (iterator i = begin(); i != end(); ++i) {
        if (i->isUnbounded() || pageNum <= i->maxRepeats())
            return i->getPageSpecs();
        pageNum -= i->maxRepeats();
    }
    return last()->getPageSpecs();
}

}
