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

#include <typeinfo>

#include "common/Factory.h"
#include "grove/Nodes.h"

#include "formatter/Exception.h"
#include "formatter/impl/debug.h"

#include "formatter/impl/FoFactory.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/PageMaster.h"
#include "formatter/impl/Properties.h"
#include "formatter/impl/BorderProperties.h"

using namespace GroveLib;
using namespace Common;

namespace Formatter {

Fo* find_sibling_fo(const GroveLib::Node* fo_node, bool next)
{
    while (fo_node) {
        Fo* fo = getFoByNode(fo_node);
        if (fo)
            return fo;
        fo_node = (next) ? fo_node->nextSibling() : fo_node->prevSibling();
    }
    return 0;
}

String node_name(const Node* origin)
{
    if (0 == origin)
        return "generated";
    String name = (Node::ATTRIBUTE_NODE == origin->nodeType())
        ? ("@" + origin->nodeName())
        : (origin->nodeName());
    return name;
}

/*
 */
ReferencedFo::ReferencedFo(const FoInit& init)
    : FoImpl(init.parentset_, init.hasParentOrigin_, init.data_),
      foNode_(init.foNode_),
      foOrigin_(XslFoExt::origin(init.foNode_)),
      physicalLevel_(init.physicalLevel_),
      logicalLevel_(init.logicalLevel_),
      trackChildren_(false)
{
    Chain* foChain = XslFoExt::areaChain(foNode_);
    // assign new chain to xsl_fo only if it does not reference to it
    if (this != foChain) {
        RT_MSG_ASSERT(!foChain, "old chain is not removed");
        XslFoExt::setAreaChain(foNode_, this);
    }
}

ReferencedFo::~ReferencedFo()
{
    //DDBG << "RefFo destroyed " << foNode->nodeName() << std::endl;
    Chain* foChain = XslFoExt::areaChain(foNode_);
    //! Let fo node know that chain is removed
    if (this == foChain)
        XslFoExt::setAreaChain(foNode_, 0);
}

CPoint ReferencedFo::getAllcPoint(const CPoint& spaceAllc) const
{
    return spaceAllc;
}

bool ReferencedFo::isEnoughSpace(const Allocation&, const Area*) const
{
    return true;
}

void ReferencedFo::buildChildren()
{
    //DBG(XSL.FODYN) << "buildChildren:" << name() << this
    //               << " parent:" << parent() << std::endl;
    if (empty()) {
        Node* child = foNode_->firstChild();
        while (child) {
            Fo* child_fo = makeChildFo(child);
            if (child_fo) {
                appendChild(child_fo);
                child_fo->buildChildren();
                child = child_fo->tailFoNode()->nextSibling();
            }
            else
                child = child->nextSibling();
        }
    }
    trackChildren_ = true;
}

Fo* ReferencedFo::makeChildFo(const Node* childFoNode)
{
    uint child_level = logicalLevel_ + 1;
    const Node* origin = XslFoExt::origin(foNode_);
    if (origin) {
        const Node* child_origin = XslFoExt::origin(childFoNode);
        if (origin == child_origin)
            child_level = logicalLevel_;
    }
    Fo* fo = Factory<FoFactory>::instance()->makeFo(
        this, childFoNode, data_, physicalLevel_ + 1, child_level);
    return fo;
}

void ReferencedFo::childInserted(const Node* child)
{
    DBG(XSL.FODYN) << "Child fo_node inserted:" << std::endl;
    Fo* child_fo = makeChildFo(child);
    if (child_fo) {
        DBG_IF(XSL.FODYN) child_fo->dump(4);
        insertChildFo(child_fo, child);
    }
}

void ReferencedFo::childRemoved(const Node*, const Node* child)
{
    DBG(XSL.FODYN) << "Child fo_node removed:" << std::endl;
    Fo* removed_fo = getFoByNode(child);
    if (removed_fo) {
        DBG_IF(XSL.FODYN) removed_fo->dump(4);
        removeChildFo(removed_fo);
    }
}

void ReferencedFo::attributeChanged(const GroveLib::Attr* attr)
{
    if (trackChildren_) {
        DBG(XSL.PROPDYN) << "Attribute changed: " << attr->name()
                         << "=" << attr->value() << std::endl;
        changeProperty(attr->localName());
    }
    else
        DBG(XSL.PROPDYN)
            << "Attribute changed but not tracked: " << attr->name()
            << "=" << attr->value()
            << "\n       in " << name() << ' ' << this << std::endl;
}

void ReferencedFo::attributeRemoved(const GroveLib::Element*,
                                    const GroveLib::Attr* attr)
{
    if (trackChildren_) {
        DBG(XSL.PROPDYN) << "Attribute removed: " << attr->name() << std::endl;
        removeProperty(attr->localName());
    }
    else
        DBG(XSL.PROPDYN)
            << "Attribute removed but not tracked: " << attr->name()
            << "\n       in " << name() << ' ' << this << std::endl;
}

void ReferencedFo::attributeAdded(const GroveLib::Attr* attr)
{
    if (trackChildren_) {
        DBG(XSL.PROPDYN) << "Attribute added: " << attr->name()
                         << "=" << attr->value() << std::endl;
        addProperty(attr->localName());
    }
    else
        DBG(XSL.PROPDYN)
            << "Attribute added but not tracked: " << attr->name()
            << "=" << attr->value()
            << "\n       in " << name() << ' ' << this << std::endl;
}

Rgb ReferencedFo::getSchemeColor() const
{
    return (logicalLevel_ % 2)
        ? data_.mediaInfo_.colorScheme_->evenContColor()
        : data_.mediaInfo_.colorScheme_->oddContColor();
}

/*
 */
RootFo::RootFo(const Node* foRoot, FoData& data)
    : ReferencedFo(FoInit(0, foRoot, data, 0, 0, false))
{
    contColor_ = Rgb(0, 0, 0, true);

    if (ROOT != foName(foRoot))
        throw Formatter::Exception(XslMessages::rootNotFound);

    Node* child = foRoot->firstChild();
    while (child) {
        switch (foName(child)) {
            case LAYOUT_MASTER_SET :
                {
                    pageMasterMap_ =
                        new PageMasterMap(this, child, data_.mediaInfo_,
                                          data_.mstream_);
                }
                break;
            case PAGE_SEQUENCE :
                {
                    if (pageMasterMap_.isNull())
                       throw Formatter::Exception(XslMessages::foOrder);
                    appendChild(
                        new PageFo(child, data_, *pageMasterMap_.pointer()));
                }
                break;
            case COMMENT :
            case PI :
                break;
            default:
                data_.mstream_
                    << XslMessages::foNotAllowed << Message::L_WARNING
                    << child->nodeName() << node()->nodeName();
                break;
        }
        child = child->nextSibling();
    }
    if (pageMasterMap_.isNull())
        throw Formatter::Exception(XslMessages::noLayoutMasterSet);
    if (empty())
        throw Formatter::Exception(XslMessages::noPageSequence);
}

Area* RootFo::makeArea(const Allocation&, const Area* after, bool) const
{
    RT_MSG_ASSERT(!after, "Only one root area may exist at a time");
    rootArea_ = new RootArea(this);
    return rootArea_.pointer();
}
 
bool RootFo::isReference(CRange& contRange) const
{
    contRange.w_ = contRange.h_ = CTYPE_MAX;
    return true;
}

CRect RootFo::getSpaceAfter(const Area*, bool, const Area* child,
                            const CRect&) const
{
    if (child)
        return CRect(CPoint(0, child->allcPoint().y_ + child->allcH() +
                            data_.mediaInfo_.indent_),
                     CRange(-1, -1));
    return CRect(CPoint(0, data_.mediaInfo_.indent_), CRange(-1, -1));
}

void RootFo::updateGeometry(const AreaPtr& area, const Allocation&,
                            const Area*, bool) const
{
    const Area* last_child = area->lastChild();
    RootArea* root = SAFE_CAST(RootArea*, area.pointer());
    if (last_child) {
        root->contRange_.h_ = last_child->allcPoint().y_ +
            last_child->allcH() + data_.mediaInfo_.indent_;
        root->contRange_.w_ = last_child->allcW();
    }
    else
        root->contRange_ = CRange();
}

String RootFo::name() const
{
    return "RootFo";
}

/*
 */
PageFo::PageFo(const Node* foPage, FoData& data,
               const PageMasterMap& pageMasterMap)
    : ReferencedFo(FoInit(0, foPage, data, 0, 0, false))

{
    String master_ref = get_attr_value(foPage, "master-reference");
    if (master_ref.isNull())
        throw Formatter::Exception(XslMessages::noMasterReference);
    pageMaster_ = pageMasterMap.getPageMaster(master_ref);
    if (pageMaster_.isNull())
        throw Formatter::Exception(XslMessages::noPageMaster);
    pageSpecs_ = &pageMaster_->getPageSpecs(0);

    setParentSet(&pageMaster_->getPageSpecs(0));

    Node* child = foPage->firstChild();
    while (child) {
        switch (foName(child)) {
            case FLOW :
                {
                    appendChild(new FlowFo(child, data_,
                                           pageMaster_->getPageSpecs(0)));
                }
                break;
            default:
                data_.mstream_ << XslMessages::foNotSupported
                               << Message::L_WARNING << child->nodeName();
                break;
        }
        child = child->nextSibling();
    }
    contColor_ = getSchemeColor();
}

Area* PageFo::makeArea(const Allocation& alloc, const Area* after, bool) const
{
    int page_num = (after)
        ? static_cast<const PageArea*>(after)->pageNum()
        : 0;
    const PageSpecs& specs = pageMaster_->getPageSpecs(0);

    PageArea* page = new PageArea(this, specs.pageSize(), page_num + 1);
    page->allcPoint_ = alloc.space_.origin_;
    page->contPoint_ = CPoint(specs.margin().left_, specs.margin().top_);
    page->contRange_ = CRange(specs.pageSize().w_ - specs.margin().left_ -
                              specs.margin().right_,
                              specs.pageSize().h_ - specs.margin().top_ -
                              specs.margin().bottom_);
    return page;
}

bool PageFo::isReference(CRange& contRange) const
{
    const PageSpecs& specs = pageMaster_->getPageSpecs(0);
    contRange.w_ = specs.pageSize().w_ - specs.margin().left_ -
        specs.margin().right_;
    contRange.h_ = specs.pageSize().h_ - specs.margin().top_ -
        specs.margin().bottom_;
    return true;
}

CRect PageFo::getSpaceAfter(const Area*, bool, const Area*, const CRect&) const
{
    //! FlowFo itself knows the size of it`s region
    return CRect(CPoint(0, 0), CRange(-1, -1));
}

void PageFo::updateGeometry(const AreaPtr& area, const Allocation&,
                            const Area*, bool) const
{
    if (data_.mediaInfo_.isPaginated_)
        return;
    
    const Area* last_child = area->lastChild();
    PageArea* page = SAFE_CAST(PageArea*, area.pointer());
    if (last_child) {
        page->contRange_.h_ = last_child->allcH() + data_.mediaInfo_.indent_;
        page->contRange_.w_ = last_child->allcW();
    }
    else
        page->contRange_ = CRange();
}

void PageFo::expandGeometry(const AreaPtr& area, const CRect&) const
{
    if (data_.mediaInfo_.isPaginated_)
        return;    
    PageArea* page = SAFE_CAST(PageArea*, area.pointer());
    page->contRange_.h_ = CTYPE_MAX; //space.extent_.h_;
}

String PageFo::name() const
{
    return "PageFo";
}

/*
 */
FlowFo::FlowFo(const Node* foFlow, FoData& data,
               const PageSpecs& pageSpecs)
    : ReferencedFo(FoInit(0, foFlow, data, 0, 0, false))
{
    String flow_name = get_attr_value(foFlow, "flow-name");
    if (flow_name.isNull())
        throw Formatter::Exception(XslMessages::noFlowName);
    regionSpecs_ = pageSpecs.getRegionSpecs(flow_name);
    if (regionSpecs_.isNull())
        throw Formatter::Exception(XslMessages::noRegionSpecs);
    setParentSet(regionSpecs_.pointer());
}

const RegionSpecs* FlowFo::regionSpecs() const
{
    return regionSpecs_.pointer();
}

void FlowFo::calcProperties(const Allocation& alloc)
{
    if (data_.mediaInfo_.isShowTags_)
        contColor_ = getSchemeColor();
    else
        contColor_ = getProperty<BackgroundColor>(alloc).value();
}

bool FlowFo::isReference(CRange& contRange) const
{
    contRange = regionSpecs_->viewport().extent_;
    return true;
}

Area* FlowFo::makeArea(const Allocation&, const Area*, bool) const
{
    RegionArea* region = new RegionArea(this);
    region->allcPoint_ = regionSpecs_->viewport().origin_;
    region->contRange_ = regionSpecs_->viewport().extent_;

    return region;
}

CPoint FlowFo::getAllcPoint(const CPoint&) const
{
    return regionSpecs_->viewport().origin_;
}

CRect FlowFo::getSpaceAfter(const Area*, bool, const Area* child,
                            const CRect&) const
{
    if (child) {
        CType bottom_y = child->allcPoint().y_ + child->allcH();
        if (!data_.mediaInfo_.isPaginated_)
            return CRect(CPoint(0, bottom_y),
                         CRange(regionSpecs_->viewport().extent_.w_, 
                                CTYPE_MAX));
        return CRect(CPoint(0, bottom_y),
                     CRange(regionSpecs_->viewport().extent_.w_,
                            regionSpecs_->viewport().extent_.h_ - bottom_y));
    }
    if (!data_.mediaInfo_.isPaginated_)    
        return CRect(CPoint(0, 0), 
                     CRange(regionSpecs_->viewport().extent_.w_, CTYPE_MAX));
    return CRect(CPoint(0, 0), regionSpecs_->viewport().extent_);
}

void FlowFo::updateGeometry(const AreaPtr& area, const Allocation&,
                            const Area*, bool) const
{
    if (data_.mediaInfo_.isPaginated_)
        return;

    const Area* last_child = area->lastChild();
    RegionArea* page = SAFE_CAST(RegionArea*, area.pointer());
    if (last_child) {
        page->contRange_.h_ = last_child->allcPoint().y_ +
            last_child->allcH() + data_.mediaInfo_.indent_;
        page->contRange_.w_ = last_child->allcW();
    }
    else
        page->contRange_ = CRange();
}

void FlowFo::expandGeometry(const AreaPtr& area, const CRect&) const
{
    if (data_.mediaInfo_.isPaginated_)
        return;    
    RegionArea* region = SAFE_CAST(RegionArea*, area.pointer());
    region->contRange_.h_ = CTYPE_MAX; //space.extent_.h_;
}

Fo* FlowFo::makeChildFo(const Node* foNode)
{
    if (BLOCK_CONTENT == contentType(foNode)) {
        return ReferencedFo::makeChildFo(foNode);
    }
    if (foName(foNode) != COMMENT && foName(foNode) != PI)
        data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                       << foNode->nodeName() << node()->nodeName();
    return 0;
}

String FlowFo::name() const
{
    return "FlowFo";
}

} // namespace Formatter
