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
#include "XslMessages.h"
#include "common/Factory.h"

#include "formatter/impl/FoFactory.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/BorderProperties.h"

using namespace GroveLib;
using namespace Common;

namespace Formatter {

BlockLevelFo::BlockLevelFo(const FoInit& init)
    : ReferencedFo(init), 
      contentWidth_(0),
      lfTreatment_(PRESERVE_LINEFEED)
{
    tagMetrix_ = data_.tagMetrixTable_.getMetrix(
        node_name(foOrigin_), data_.fontMgr_->getTagFont());
}

bool BlockLevelFo::isPreserveLinefeed() const 
{
    return PRESERVE_LINEFEED == lfTreatment_;
}

bool BlockLevelFo::bodyStart(const FunctionArgList&, ValueTypePair& pair,
                             const PropertyContext*, const Allocation&) const
{
    for (const Fo* fo = parent(); fo; fo = fo->parent()) {
        if (LIST_BLOCK_FO == fo->type()) {
            const ListBlockFo* list_block =
                static_cast<const ListBlockFo*>(fo);
            pair.type_ = Value::V_NUMERIC;
            pair.value_.nval_ =
                list_block->startIndent_ + list_block->startsDist_;
            return true;
        }
    }
    return false;
}

bool BlockLevelFo::labelEnd(const FunctionArgList&, ValueTypePair& pair,
                            const PropertyContext*,
                            const Allocation& alloc) const
{
    for (const Fo* fo = parent(); fo; fo = fo->parent()) {
        if (LIST_BLOCK_FO == fo->type()) {
            const ListBlockFo* list_block =
                static_cast<const ListBlockFo*>(fo);
            pair.type_ = Value::V_NUMERIC;
            pair.value_.nval_ =
                alloc.space_.extent_.w_ - list_block->startsDist_ -
                list_block->startIndent_ + list_block->separation_;
            return true;
        }
    }
    return false;
}

void BlockLevelFo::calcProperties(const Allocation& alloc)
{
    if (data_.mediaInfo_.isShowTags_) {
        bord_ = ORect();

        padd_.top_ = padd_.bottom_ = tagMetrix_->size().h_;
        padd_.left_ = padd_.right_ = 0;

        contColor_ = getSchemeColor();

        startIndent_ = logicalLevel_ * data_.mediaInfo_.indent_;
        endIndent_ = 0;
    }
    else {
        bord_ = getBorder(*this, alloc);
        padd_ = getPadding(*this, alloc);

        contColor_ = getProperty<BackgroundColor>(alloc).value();
        bordColor_ = getBorderColor(*this, alloc);

        startIndent_ = getProperty<StartIndent>(alloc).value();
        endIndent_ = getProperty<EndIndent>(alloc).value();
    }
    font_ = data_.fontMgr_->getFont(
        getProperty<FontFamily>(alloc).value(),
        getProperty<FontStyle>(alloc).value(),
        getProperty<FontWeight>(alloc).value(),
        getProperty<TextDecoration>(alloc).value(),
        getProperty<FontSize>(alloc, parent()->lineHeight()).pointSize());
    CType accender = font_->accender();
    CType descender = font_->descender();
    lineHeight_ = accender + descender;
    lfTreatment_ = getProperty<LinefeedTreatment>(alloc).value();

    DBG(XSL.TYPES) << "BlockColor:" << std::endl;
    DBG_IF(XSL.TYPES) contColor_.dump();
    DBG(XSL.TYPES) << "BlockBorders:" << std::endl;
    DBG_IF(XSL.TYPES) bord_.dump();
    DBG(XSL.TYPES) << "BlockBorderColors:" << std::endl;
    DBG_IF(XSL.TYPES) dump_oval(bordColor_);
}

CType BlockLevelFo::getDecoration(Decoration type, CType& bord,
                                  CType& padd) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */

    const bool is_empty = empty();
    bool suppress_tags = hasParentOrigin_ || is_empty;
    const bool tagmode = data_.mediaInfo_.isShowTags_;

    if (tagmode && firstChild()) {
        Fo* child = firstChild();
        if (!child->nextSibling() &&
            child->hasParentOrigin() && child->empty())
            suppress_tags = true;
    }

    bord = padd = 0;

    switch (type) {
        case START_DECOR :
            if (tagmode) {
                if (!suppress_tags )
                    padd = tagMetrix_->size(TagMetrix::START).h_;
            }
            else {
                bord = bord_.top_;
                padd = padd_.top_;
            }
            return (bord + padd);
            break;
        case END_DECOR :
            if (tagmode) {
                if (!suppress_tags )
                    padd = tagMetrix_->size(TagMetrix::END).h_;
            }
            else {
                bord = bord_.bottom_;
                padd = padd_.bottom_;
            }
            return (bord + padd);
            break;
        case EMPTY_DECOR :
            if (is_empty)
                return tagMetrix_->size(TagMetrix::EMPTY).h_;
            break;
        default:
            return 0;
    }
    return 0;
}

bool BlockLevelFo::needsDecoration(Decoration type) const
{
    CType bord, padd;
    return (0 != getDecoration(type, bord, padd));
}

bool BlockLevelFo::isEnoughSpace(const Allocation& alloc,
                                 const Area* after) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */
    CType h = 0;
    CType bord = 0;
    CType padd = 0;
    if (0 == after)
        h = getDecoration(START_DECOR, bord, padd);
    if (0 == h && empty())
        h = getDecoration(EMPTY_DECOR, bord, padd);
    CType width = (0 != contentWidth_)
        ? (startIndent_ + contentWidth_) : (startIndent_ + endIndent_);
    return ((alloc.maxh_ || alloc.space_.extent_.h_ > h) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ > width));
}

CRect BlockLevelFo::getSpaceAfter(const Area*, bool isFirst,
                                  const Area* child, const CRect& space) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */
    CType bord = 0;
    CType padd = 0;
    CType h = (isFirst) ? getDecoration(START_DECOR, bord, padd) : 0;
    CType bottom_y = (child) ? child->allcPoint().y_ + child->allcH() : 0;
    CType child_allc_x = -startIndent_;
    CType child_extent_w = space.extent_.w_;
    CRange range;
    if (isReference(range)) {
        child_allc_x = 0;
        if (0 != contentWidth_)
            child_extent_w = contentWidth_;
        else
            child_extent_w -= (startIndent_ + endIndent_);
    }
    return CRect(CPoint(child_allc_x, bottom_y),
                 CRange(child_extent_w, space.extent_.h_ - h - bottom_y));
}

CPoint BlockLevelFo::getContPoint(const Area*, bool isFirst) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */
    CType bord = 0;
    CType padd = 0;
    if (isFirst)
        getDecoration(START_DECOR, bord, padd);
    return CPoint(startIndent_, bord + padd);
}

CType BlockLevelFo::contWidth(const CType& allocW) const
{
    if (contentWidth_)
        return contentWidth_;
    return (allocW - startIndent_ - endIndent_);
}

Area* BlockLevelFo::makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, after))
            return 0;

    BlockArea* block = new BlockArea(this);

    CType bord_top = 0;
    CType padd_top = 0;
    if (0 == after) {
        CType top = getDecoration(START_DECOR, bord_top, padd_top);
        if (0 != top) // if needsDecoration(START_DECOR)
            block->decor_ |= START_DECOR;
    }
    block->allcPoint_ = alloc.space_.origin_;
    block->contPoint_ = CPoint(startIndent_, bord_top + padd_top);
    //!TODO: prevent contRange_ from being nonpositive
    block->contRange_ = CRange(contWidth(alloc.space_.extent_.w_),
                               alloc.space_.extent_.h_ - bord_top - padd_top);
    block->bord_ = bord_;
    block->bord_.top_ = bord_top;
    block->bord_.bottom_ = 0;

    block->padd_ = padd_;
    block->padd_.top_ = padd_top;
    block->padd_.bottom_ = 0;

    return block;
}

CType BlockLevelFo::childrenHeight(const Area* area) const
{
    const Area* last_child = area->lastChild();
    if (last_child)
        return (last_child->allcPoint().y_ + last_child->allcH());
    return 0;
}

void BlockLevelFo::settleEmptyBlock(BlockArea* block, const Area* prevChunk,
                                    bool& force_space, CType& space_left) const
{
    CType bord = 0;
    CType padd = 0;
    const CType h = getDecoration(EMPTY_DECOR, bord, padd);
    const bool needs_empty_decor =
        (0 == prevChunk || !prevChunk->hasDecoration(EMPTY_DECOR));

    if (needs_empty_decor && (force_space || space_left >= h)) {
        block->contRange_.h_ = h;
        block->decor_ |= EMPTY_DECOR;
        force_space = false;
    }
    else {
        block->contRange_.h_ = 0;
        block->decor_ &= ~EMPTY_DECOR;
        if (needs_empty_decor)
            space_left = 0;
    }
}

void BlockLevelFo::updateGeometry(const AreaPtr& area, const Allocation& alloc,
                                  const Area* prevChunk, bool isLast) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */
    CType bord = 0;
    CType padd = 0;
    CType space_left = alloc.space_.extent_.h_;
    bool  force_space = alloc.maxh_;

    BlockArea* block = SAFE_CAST(BlockArea*, area.pointer());
    //! Update allocation point
    block->allcPoint_ = alloc.space_.origin_;
    block->contPoint_.x_ = startIndent_;
    const bool start_decor_fits =
        (space_left >= getDecoration(START_DECOR, bord, padd));
    //! Update start decoration
    if ((bord || padd) && !prevChunk && (force_space || start_decor_fits)) {
        block->bord_.top_ = bord;
        block->padd_.top_ = padd;
        force_space = false;
    }
    else
        block->bord_.top_ = block->padd_.top_ = 0;
    block->contPoint_.y_ = block->bord_.top_ + block->padd_.top_;
    if (0 != block->contPoint_.y_)
        block->decor_ |= START_DECOR;
    else
        block->decor_ &= ~START_DECOR;
    space_left -= block->contPoint_.y_;
    //! Update content range
    block->contRange_.w_ = contWidth(alloc.space_.extent_.w_);
    if (empty()) {
        settleEmptyBlock(block, prevChunk, force_space, space_left);
    }
    else {
        block->contRange_.h_ = childrenHeight(area.pointer());
        block->decor_ &= ~EMPTY_DECOR;
    }
    space_left -= block->contRange_.h_;
    //! Update end decoration
    const bool end_decor_fits =
        (space_left >= getDecoration(END_DECOR, bord, padd));
    if (isLast && (force_space || end_decor_fits)) {
        block->bord_.bottom_ = bord;
        block->padd_.bottom_ = padd;
        force_space = false;
    }
    else
        block->bord_.bottom_ = block->padd_.bottom_ = 0;
    if (0 != block->bord_.bottom_ + block->padd_.bottom_)
        block->decor_ |= END_DECOR;
    else
        block->decor_ &= ~END_DECOR;
}


void BlockLevelFo::expandGeometry(const AreaPtr& area,
                                  const CRect& space) const
{
    if (data_.mediaInfo_.isPaginated_)
        return;    
    BlockArea* block = SAFE_CAST(BlockArea*, area.pointer());
    block->contRange_.h_ = space.extent_.h_;
}


void BlockFo::childInserted(const Node* child)
{
    DBG(XSL.FODYN)
        << name() << ":" << this << " child fo_node inserted:" << std::endl;
    if (INLINE_CONTENT == contentType(child)) {
        DBG(XSL.FODYN) << "before insertion ========" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);
        Fo* next_fo = find_sibling_fo(child->nextSibling(), true);
        Fo* prev_fo = find_sibling_fo(child->prevSibling(), false);
        if (next_fo) {
            if (INLINE_CONTENT == next_fo->contentType()) {
                Fo* child_fo = Factory<FoFactory>::instance()->makeFo(
                    this, child, data_, physicalLevel_ + 1, logicalLevel_ + 1);
                next_fo->insertBefore(child_fo);
                child_fo->registerModification(THIS_FO_MODIFIED);
                DBG(XSL.FODYN) << "after insertion --------"
                               << std::endl;
                DBG_IF(XSL.FODYN) dump(4);
                DBG(XSL.FODYN) << "end of insertion ======="
                               << std::endl;
                return;
            }
        }
        if (prev_fo) {
            if (INLINE_CONTENT == prev_fo->contentType()) {
                Fo* child_fo = Factory<FoFactory>::instance()->makeFo(
                    this, child, data_, physicalLevel_ + 1, logicalLevel_ + 1);
                prev_fo->insertAfter(child_fo);
                child_fo->registerModification(THIS_FO_MODIFIED);
                DBG(XSL.FODYN) << "after insertion --------"
                               << std::endl;
                DBG_IF(XSL.FODYN) dump(4);
                DBG(XSL.FODYN) << "end of insertion ======="
                               << std::endl;
                return;
            }
        }
        Fo* child_fo = makeChildFo(child);
        if (!child_fo)
            return;
        insertChildFo(child_fo, child);
        DBG(XSL.FODYN) << "after insertion --------" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);
        DBG(XSL.FODYN) << "end of insertion =======" << std::endl;
    }
    else {
        DBG(XSL.FODYN) << "before insertion ========" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);
        Fo* child_fo = makeChildFo(child);
        if (!child_fo)
            return;
        Fo* next_fo = find_sibling_fo(child->nextSibling(), true);
        if (next_fo) {
            if (INLINE_CONTENT == next_fo->contentType()) {
                if (child->prevSibling()) {
                    Fo* prev_fo = getFoByNode(child->prevSibling());
                    if (INLINE_CONTENT == prev_fo->contentType()) {
                        //! Split LineFo
                        LineFo* head = SAFE_CAST(LineFo*, prev_fo->parent());
                        LineFo* tail =
                            new LineFo(this, 0, data_,
                                       physicalLevel_ + 1, logicalLevel_ + 1);
                        head->insertAfter(tail);
                        for (FoPtr fo = next_fo; !fo.isNull();
                             fo = prev_fo->nextSibling()) {
                            fo->removeFo();
                            tail->appendChild(fo.pointer());
                        }
                        head->insertAfter(child_fo);
                        head->lastChild()->
                            registerModification(THIS_FO_MODIFIED);
                    }
                    else
                        prev_fo->insertAfter(child_fo);
                }
                else
                    firstChild()->insertBefore(child_fo);
            }
            else
                next_fo->insertBefore(child_fo);
        }
        else
            appendChild(child_fo);

        child_fo->registerModification(THIS_FO_MODIFIED);
        DBG(XSL.FODYN) << "after insertion --------" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);
        DBG(XSL.FODYN) << "end of insertion =======" << std::endl;
    }
}

void BlockFo::childRemoved(const Node*, const Node* child)
{
    DBG(XSL.FODYN)
        << name() << ":" << this << " child fo-node removed" << std::endl;
    Fo* removed_fo = getFoByNode(child);
    if (!removed_fo)
        return;
    DBG_IF(XSL.FODYN) removed_fo->dump(4);
    if (this != removed_fo->parent()) {
        LineFo* line = SAFE_CAST(LineFo*, removed_fo->parent());
        //! Remove empty LineFo
        if (!removed_fo->nextSibling() && !removed_fo->prevSibling())
            removeChildFo(line);
        else
            line->removeChildFo(removed_fo);
    }
    else {
        Fo* prev = removed_fo->prevSibling();
        Fo* next = removed_fo->nextSibling();
        //! Join surrounding LineFo`s
        if (prev && next && (LINE_FO == next->type() &&
                             LINE_FO == prev->type())) {
            removed_fo->removeFo();
            Fo* tail_first = next->firstChild();
            for (FoPtr fo = next->firstChild(); !fo.isNull();
                 fo = next->firstChild()) {
                fo->removeFo();
                prev->appendChild(fo.pointer());
            }
            next->removeFo();
            tail_first->registerModification(THIS_FO_MODIFIED);
        }
        else
            removeChildFo(removed_fo);
    }
    DBG(XSL.FODYN) << "after removal --------" << std::endl;
    DBG_IF(XSL.FODYN) dump(4);
    DBG(XSL.FODYN) << "end of removal =======" << std::endl;
}

Fo* BlockFo::makeChildFo(const Node* foNode)
{
    switch (contentType(foNode)) {
        case INLINE_CONTENT :
            return new LineFo(this, foNode, data_,
                              physicalLevel_ + 1, logicalLevel_ + 1);
            break;
        case TABLE_CONTENT :
            if (data_.mediaInfo_.isShowTags_)
                return ReferencedFo::makeChildFo(foNode);
            break;
        case BLOCK_CONTENT :
            return ReferencedFo::makeChildFo(foNode);
            break;

        default:
            break;
    }
    if (foName(foNode) != COMMENT && foName(foNode) != PI)
        data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                       << foNode->nodeName() << node()->nodeName();
    return 0;
}

String BlockFo::name() const
{
    return "BlockFo";
}

String BlockFo::areaName() const
{
    return "BlockArea";
}

} // namespace Formatter
