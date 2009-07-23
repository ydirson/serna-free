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
#include "common/Factory.h"
#include "grove/Nodes.h"

#include "formatter/impl/FoFactory.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"

using namespace GroveLib;
using namespace Common;

namespace Formatter {

LineFo::LineFo(PropertySet* parentSet, const Node* foNode, FoData& data,
               uint physicalLevel, uint logicalLevel)
    : FoImpl(parentSet, false, data),
      physicalLevel_(physicalLevel),
      logicalLevel_(logicalLevel)
{
    contColor_ = Rgb(0, 0, 0, true);
    bordColor_ = OValue<Rgb>(contColor_, contColor_, contColor_, contColor_);

    while (foNode && INLINE_CONTENT == contentType(foNode)) {
        Fo* child_fo = Factory<FoFactory>::instance()->makeFo(
            parentSet_, foNode, data_, physicalLevel_ + 1, logicalLevel_ + 1);
        if (child_fo)
            appendChild(child_fo);
        foNode = foNode->nextSibling();
    }
}

void LineFo::calcProperties(const Allocation& alloc)
{
    if (data_.mediaInfo_.isShowTags_) {
        startIndent_ = logicalLevel_ * data_.mediaInfo_.indent_;
        endIndent_ = 0;
    }
    else {
        startIndent_ = getProperty<StartIndent>(alloc).value();
        endIndent_ = getProperty<EndIndent>(alloc).value();
    }
    textAlign_ = getProperty<TextAlign>(alloc).value();
    font_ = data_.fontMgr_->getFont(
        getProperty<FontFamily>(alloc).value(),
        getProperty<FontStyle>(alloc).value(),
        getProperty<FontWeight>(alloc).value(),
        getProperty<TextDecoration>(alloc).value(),
        getProperty<FontSize>(alloc, parent()->lineHeight()).pointSize());
    accender_ = font_->accender();
    descender_ = font_->descender();
    lineHeight_ = accender_ + descender_;
}

bool LineFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ || alloc.space_.extent_.h_ > 0) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ > (startIndent_ +
                                                       endIndent_)));
}

Area* LineFo::makeArea(const Allocation& alloc,
                       const Area*, bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
            return 0;

    LineArea* line = new LineArea(this);

    line->allcPoint_ = alloc.space_.origin_;
    line->contPoint_ = CPoint(startIndent_, 0);
    //!TODO: prevent contRange_ from being nonpositive
    line->contRange_ =
        CRange(alloc.space_.extent_.w_ - startIndent_ - endIndent_,
               alloc.space_.extent_.h_);
    return line;
}

CPoint LineFo::getAllcPoint(const CPoint& spaceAllc) const
{
    return CPoint(spaceAllc.x_, spaceAllc.y_);
}

CRect LineFo::getSpaceAfter(const Area*, bool, const Area* child,
                            const CRect& space) const
{
    CRange line_extent(space.extent_.w_ - startIndent_ - endIndent_,
                       space.extent_.h_);
    if (child) {
        CType right_x = child->allcPoint().x_ + child->allcW();
        return CRect(CPoint(right_x, 0),
                     CRange(line_extent.w_ - right_x, line_extent.h_));
    }
    return CRect(CPoint(0, 0), line_extent);
}

void LineFo::updateGeometry(const AreaPtr& area, const Allocation& alloc,
                            const Area*, bool) const
{
    LineArea* line = SAFE_CAST(LineArea*, area.pointer());
    //! Update allocation point
    line->allcPoint_ = alloc.space_.origin_;

    layoutChildrenOnBaseline(line, accender_, descender_);

    const Area* last_child = line->lastChild();
    area->contRange_.w_ = (last_child)
            ? last_child->allcPoint().x_ + last_child->allcW() : 0;
    CType w = alloc.space_.extent_.w_ - startIndent_ - endIndent_;
    if (ALIGN_LEFT == textAlign_)
        line->contPoint_.x_ = startIndent_;
    else
        if (ALIGN_RIGHT == textAlign_)
            line->contPoint_.x_ = startIndent_ + w - area->contRange_.w_;
        else
            line->contPoint_.x_ = startIndent_ + (w - area->contRange_.w_)/2;

    updateBaseline(line);
}

void LineFo::updateBaseline(const Area* area) const
{
    const CType base = area->absAllcPoint_.y_ + area->base();
    const CType x = area->absAllcPoint_.x_ + area->contPoint_.x_;
    for (Area* a = area->firstChild(); a; a = a->nextSibling()) {
        const CPoint old = a->absAllcPoint_;
        a->absAllcPoint_ = CPoint(x + a->allcPoint().x_,
                                  base - a->base() - a->baselineShift());
        if ((old != a->absAllcPoint_) && a->getView())
            a->getView()->updateGeometry();
        updateBaseline(a);
    }
}

const Node* LineFo::headFoNode() const
{
    return firstChild()->headFoNode();
}

const Node* LineFo::tailFoNode() const
{
    return lastChild()->tailFoNode();
}

void LineFo::dump(int indent) const
{
    DINDENT(indent);
    DBG(XSL.CHAIN) << "LineFo:" << this << std::endl;
    Chain::dump(indent);
    PropertySet::dump(4);

    for (Fo* child = firstChild(); child; child = child->nextSibling())
        DBG_IF(XSL.CHAIN) child->dump(indent);
}

String LineFo::name() const
{
    return "LineFo";
}

} // namespace Formatter
