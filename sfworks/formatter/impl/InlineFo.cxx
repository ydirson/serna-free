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

#include "grove/Nodes.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/BorderProperties.h"

using namespace GroveLib;
using namespace Common;

namespace Formatter {

InlineFo::InlineFo(const FoInit& init)
    : ReferencedFo(init)
{
    tagMetrix_ = data_.tagMetrixTable_.getMetrix(
        node_name(foOrigin_), data_.fontMgr_->getTagFont());
}

void InlineFo::checkProperties(const Allocation& alloc)
{
    ReferencedFo::checkProperties(alloc);
    
    CType min_height = 0;
    if (data_.mediaInfo_.isShowTags_ || empty())
        min_height = tagMetrix_->size().h_;

    accender_ = font_->accender();
    descender_ = font_->descender();
    
    if (min_height > accender_ + descender_) {
        CType delta = (min_height - accender_ - descender_)/2;
        accender_ += delta;
        descender_ += delta;
    }

    lineHeight_ = accender_ + descender_;
}

void InlineFo::calcProperties(const Allocation& alloc)
{
    DBG(XSL.PROPDYN) << "Calc properties:" << this << std::endl;
    if (data_.mediaInfo_.isShowTags_) {
        bord_ = ORect();
        padd_.left_ = tagMetrix_->size(TagMetrix::START).w_;
        padd_.right_ = tagMetrix_->size(TagMetrix::END).w_;
        contColor_ = Rgb(0, 0, 0, true);
    }
    else {
        bord_ = getBorder(*this, alloc);
        padd_ = getPadding(*this, alloc);

        contColor_ = getProperty<BackgroundColor>(alloc).value();
        bordColor_ = getBorderColor(*this, alloc);
    }

    int decoration = getProperty<TextDecoration>(alloc).value();
    font_ = data_.fontMgr_->getFont(
        getProperty<FontFamily>(alloc).value(),
        getProperty<FontStyle>(alloc).value(),
        getProperty<FontWeight>(alloc).value(),
        Font::Decoration(decoration),
        getProperty<FontSize>(alloc, parent()->lineHeight()).pointSize());

    baselineShift_ =
        getProperty<BaselineShift>(alloc, parent()->lineHeight()).value();
}

CType InlineFo::getDecoration(Decoration type, CType& bord, CType& padd) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in BlockFo  */
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
                    padd = tagMetrix_->size(TagMetrix::START).w_;
            }
            else {
                bord = bord_.left_;
                padd = padd_.left_;
            }
            return (bord + padd);
            break;
        case END_DECOR :
            if (tagmode) {
                if (!suppress_tags )
                    padd = tagMetrix_->size(TagMetrix::END).w_;
            }
            else {
                bord = bord_.right_;
                padd = padd_.right_;
            }
            return (bord + padd);
            break;
        case EMPTY_DECOR :
            if (is_empty)
                return tagMetrix_->size(TagMetrix::EMPTY).w_;
            break;
        default:
            return 0;
    }
    return 0;
}

bool InlineFo::needsDecoration(Decoration type) const
{
    CType bord, padd;
    return (0 != getDecoration(type, bord, padd));
}

bool InlineFo::isEnoughSpace(const Allocation& alloc, const Area* after) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in BlockFo  */
    CType w = 0;
    CType bord = 0;
    CType padd = 0;
    if (0 == after)
        w = getDecoration(START_DECOR, bord, padd);
    if (0 == w && empty())
        w = getDecoration(EMPTY_DECOR, bord, padd);
    alloc.enoughW_ = alloc.maxw_ || alloc.space_.extent_.w_ > w;
    alloc.enoughH_ = alloc.maxh_ || 
        alloc.space_.extent_.h_ > (accender_ + descender_);
    return (alloc.enoughW_ && alloc.enoughH_);
}

CRect InlineFo::getSpaceAfter(const Area*, bool isFirst,
                              const Area* child, const CRect& space) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in BlockFo  */
    CType bord = 0;
    CType padd = 0;
    CType w = (isFirst) ? getDecoration(START_DECOR, bord, padd) : 0;
    CType right_x = (child) ? child->allcPoint().x_ + child->allcW() : 0;
    return CRect(CPoint(right_x, 0),
                 CRange(space.extent_.w_ - w - right_x, space.extent_.h_));
}

CPoint InlineFo::getContPoint(const Area* area, bool isFirst) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in BlockFo  */
    CType bord = 0;
    CType padd = 0;
    if (isFirst)
        getDecoration(START_DECOR, bord, padd);
    const InlineArea* i_area = SAFE_CAST(const InlineArea*, area);
    return CPoint(bord + padd, i_area->contPoint_.y_);
}

Area* InlineFo::makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    InlineArea* area = new InlineArea(this);

    CType bord_left = 0;
    CType padd_left = 0;
    if (0 == after) {
        CType left = getDecoration(START_DECOR, bord_left, padd_left);
        if (0 != left) // if needsDecoration(START_DECOR)
            area->decor_ |= START_DECOR;
    }
    //! Using small allocation rectangle
    area->allcPoint_ = alloc.space_.origin_;
    area->contPoint_ = CPoint(bord_left + padd_left, 0);
    area->contRange_ = CRange(alloc.space_.extent_.w_ - bord_left - padd_left,
                              alloc.space_.extent_.h_);
    area->bord_ = bord_;
    area->bord_.left_ = bord_left;
    area->bord_.right_ = 0;

    area->padd_ = padd_;
    area->padd_.left_ = padd_left;
    area->padd_.right_ = 0;

    return area;
}

void InlineFo::updateGeometry(const AreaPtr& inlineArea,
                              const Allocation& alloc,
                              const Area* prevChunk, bool isLast) const
{
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo  */
    CType bord = 0;
    CType padd = 0;
    CType space_left = alloc.space_.extent_.w_;
    bool  force_space = alloc.maxw_;

    InlineArea* area = SAFE_CAST(InlineArea*, inlineArea.pointer());
    area->baseShift_ = baselineShift_;

    //! Update allocation point
    area->allcPoint_ = alloc.space_.origin_;
    //! Update start decoration
    bool start_decor_fits =
        space_left >= getDecoration(START_DECOR, bord, padd);
    if ((bord || padd ) && !prevChunk && (force_space || start_decor_fits)) {
        area->bord_.left_ = bord;
        area->padd_.left_ = padd;
        force_space = false;
    }
    else
        area->bord_.left_ = area->padd_.left_ = 0;
    area->contPoint_.x_ = area->bord_.left_ + area->padd_.left_;
    if (0 != area->contPoint_.x_)
        area->decor_ |= START_DECOR;
    else
        area->decor_ &= ~START_DECOR;
    space_left -= area->contPoint_.x_;

    //! Update content rectangle
    if (empty()) {
        area->contRange_.h_ = accender_ + descender_;
        area->base_ = accender_;
        CType w = getDecoration(EMPTY_DECOR, bord, padd);
        const bool needs_empty_decor =
            (0 == prevChunk || !prevChunk->hasDecoration(EMPTY_DECOR));

        if (needs_empty_decor && (force_space || space_left >= w)) {
            area->contRange_.w_ = getDecoration(EMPTY_DECOR, bord, padd);
            area->decor_ |= EMPTY_DECOR;
            force_space = false;
        }
        else {
            area->contRange_.w_ = 0;
            area->decor_ &= ~EMPTY_DECOR;
            if (needs_empty_decor)
                space_left = 0;
        }
    }
    else {
        layoutChildrenOnBaseline(area, accender_, descender_);

        const Area* last_child = area->lastChild();
        area->contRange_.w_ = (last_child)
            ? last_child->allcPoint().x_ + last_child->allcW() : 0;
        area->decor_ &= ~EMPTY_DECOR;
    }
    space_left -= area->contRange_.w_;

    //! Update end decoration
    bool end_decor_fits = space_left >= getDecoration(END_DECOR, bord, padd);
    if (isLast && (force_space || end_decor_fits)) {
        area->bord_.right_ = bord;
        area->padd_.right_ = padd;
        force_space = false;
    }
    else
        area->bord_.right_ = area->padd_.right_ = 0;
    if (0 != area->bord_.right_ + area->padd_.right_)
        area->decor_ |= END_DECOR;
    else
        area->decor_ &= ~END_DECOR;
}

Fo* InlineFo::makeChildFo(const Node* foNode)
{
    if (INLINE_CONTENT != Fo::contentType(foNode)) {
        //!TODO: uncomment
        //data_.mstream_ << XslMessages::foConvertedToInline << Message::L_INFO
        //               << foNode->nodeName() << node()->nodeName();
        uint child_level = logicalLevel_ + 1;
        bool has_parent_origin = false;
        const Node* origin = XslFoExt::origin(node());
        if (origin) {
            const Node* child_origin = XslFoExt::origin(foNode);
            if (origin == child_origin) {
                child_level = logicalLevel_;
                has_parent_origin = true;
            }
        }
        return new InlineFo(FoInit(this, foNode,
                            data_, physicalLevel_ + 1, child_level,
                            has_parent_origin));
    }
    return ReferencedFo::makeChildFo(foNode);
}

String InlineFo::name() const
{
    return "InlineFo";
}

} // namespace Formatter
