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

#include "common/String.h"
#include "formatter/impl/debug.h"
#include "formatter/impl/Fo.h"
#include "formatter/Area.h"

#include <iomanip>

USING_COMMON_NS;
using namespace std;
namespace Formatter
{

Area::Area(const Chain* chain)
    : chain_(chain),
      base_(0),
      baseShift_(0),
      decor_(NO_DECOR)
{}

bool Area::hasDecoration(Decoration type) const
{
    return (decor_ & type);
}

const Chain* Area::chain() const
{
    return chain_;
}

Area* Area::nextChunk() const
{
    RT_MSG_ASSERT(Chunk::parent(), "Error: Traverse in standalone Area");
    return static_cast<Area*>(Chunk::nextSibling());
}

Area* Area::prevChunk() const
{
    RT_MSG_ASSERT(Chunk::parent(), "Error: Traverse in standalone Area");
    return static_cast<Area*>(Chunk::prevSibling());
}

void Area::setFirstChunk(Chain* chain)
{
    RT_MSG_ASSERT((chain == chain_), "Set chain different to initial");
    RT_MSG_ASSERT(!Chunk::parent(), "Area is already in chain");
    chain->setFirstChunk(this);
}

void Area::insertChunkAfter(Area* area)
{
    Chunk::insertAfter(area);
}

void Area::replace(const AreaPtr& area)
{
    RT_MSG_ASSERT(!Chunk::parent(), "New area is not standalone");
    RT_MSG_ASSERT(area->Chunk::parent(), "Old area is standalone");
    RT_MSG_ASSERT((area->chain() == chain_),
                  "Cannot replace areas from different chains");
    area->replaceWith(this);
}

void Area::replaceWith(Area* chunk)
{
    Chunk::insertBefore(chunk);
    Chunk::remove();
    ATN::remove();
}

void Area::dispose()
{
    Chunk::remove();
    ATN::remove();
}

void Area::removeFromChain(bool recursively)
{
    Chunk::remove();
    if (recursively)
        for (Area* child = firstChild(); child; child = child->nextSibling())
            child->removeFromChain(true);
}

ulong Area::chunkPosCount() const
{
    if (empty())
        return 1;
    //!
    if (isProgression(PARALLEL)) {
        ulong count = 0;
        for (const Area* child = firstChild(); child;
             child = child->nextSibling()) {
            //! TODO: find the right way to operate with not formatted chains
            if (0 == child->chain()->firstChunk()) {
                DBG(XSL.TEST)
                    << "area::chunkPosCount: chain not yet formatted\n";
                break;
            }
            if (this == child->chain()->firstChunk()->parent())
                ++count;
        }
        if (this == lastChild()->chain()->lastChunk()->parent())
            count++;
        return count;
    }

    ulong count = 0;
    const Chain* chain = firstChild()->chain();

    if (this == chain->firstChunk()->parent())
        count++;
    while (chain && this == chain->lastChunk()->parent()) {
        count++;
        chain = chain->nextChain();
        //! TODO: find the right way to operate with not formatted chains
        if (chain && 0 == chain->firstChunk()) {
            DBG(XSL.TEST) << "area::chunkPosCount: chain not yet formatted\n";
            break;
        }
    }
    return count;
}

AreaPos Area::areaPos(const Chain* chain) const
{
    if (empty())
        return AreaPos(0, 0);
    ulong pos = 0;
    chain = chain->prevChain();
    if (chain && !chain->firstChunk())
        return AreaPos(0, 0);
    while (chain && this == chain->firstChunk()->parent()) {
        pos++;
        chain = chain->prevChain();
        //! TODO: find the right way to operate with not formatted chains
        if (chain && 0 == chain->firstChunk()) {
            DBG(XSL.TEST) << "area::areaPos: chain not yet formatted\n";
            break;
        }
    }
    return AreaPos(this, pos);
}

const Chain* Area::chainAt(ulong chunkPos) const
{
    if (empty())
        return 0;
    //!
    if (isProgression(PARALLEL)) {
        ulong count = chunkPos;
        for (const Area* child = firstChild(); child;
             child = child->nextSibling()) {
            //! TODO: find the right way to operate with not formatted chains
            if (0 == child->chain()->firstChunk()) {
                DBG(XSL.TEST)
                    << "area::chunkPosCount: chain not yet formatted\n";
                break;
            }
            if (this == child->chain()->firstChunk()->parent())
                if (count)
                    --count;
                else
                    return child->chain();
        }
        return 0;
    }

    ulong count = chunkPos;
    const Chain* chain = firstChild()->chain();
    while (chain) {
        if (this == chain->firstChunk()->parent())
            if (count)
                count--;
            else
                return chain;
        chain = chain->nextChain();
        //! TODO: find the right way to operate with not formatted chains
        if (chain && 0 == chain->firstChunk()) {
            DBG(XSL.TEST) << "area::chainAt: chain not yet formatted\n";
            break;
        }
    }
    return 0;
}

const Chain* Area::chainBefore(ulong chunkPos) const
{
    if (empty())
        return 0;
    //!
    if (isProgression(PARALLEL)) {
        if (0 == chunkPos) {
            if (this != lastChild()->chain()->firstChunk()->parent())
                return lastChild()->chain();
            else
                return 0;
        }
        return chainAt(chunkPos - 1);
    }

    if (0 == chunkPos) {
        const Chain* chain = firstChild()->chain();
        if (this != chain->firstChunk()->parent())
            return chain;
        else
            return 0;
    }
    return chainAt(chunkPos - 1);
}

bool Area::isProgression(PDirection direction) const
{
    return (direction == chain_->progression());
}

AreaPos Area::mapToAreaPos(CType localX, bool up) const
{
    DBG(XSL.CURSOR)
        << "mapping to area_pos" << " x:" << fix_fmt(localX) << std::endl;
    DBG_IF(XSL.CURSOR) dump(0, false);

    if (isProgression(VERTICAL)) {
        const Area* corner_child = (up) ? lastChild() : firstChild();
        if (corner_child) {
            CType child_x = localX - corner_child->allcPoint().x_ -
                corner_child->contPoint().x_;
            return corner_child->mapToAreaPos(child_x, up);
        }       
        return AreaPos(this, 0);
    }
    for (Area* child = firstChild(); child; child = child->nextSibling()) {
        CType child_x = localX - child->allcPoint().x_ - child->contPoint().x_;

        if (child_x < -child->padd().left_) {
            if (child->prevChunk())
                return child->mapToAreaPos(child_x, up);
            else {
                return AreaPos::getPosBeforeArea(child);
            }
        }
        else
            if (child_x <= child->contRange().w_ + child->padd().left_)
                return child->mapToAreaPos(child_x, up);
            else
                if (!child->nextSibling()) {
                    if (child->nextChunk())
                        return child->mapToAreaPos(child_x, up);
                    else {
                        return AreaPos::getPosAfterArea(child);
                    }
                }
    }
    return AreaPos(this, 0);
}

static void find_top_area(Area* area, const CPoint& local, Area*& topArea, 
                          CPoint& topLocal, int& topLevel, int level)
{
    for (Area* child = area->firstChild(); child;   
         child = child->nextSibling()) {
        CPoint child_local(local - child->allcPoint() - child->contPoint());
        bool y_inside = ((child_local.y_ >= 0) &&
                         (child_local.y_ <= child->contRange().h_));
        bool x_inside = ((child_local.x_ >= 0) &&
                         (child_local.x_ <= child->contRange().w_));
        if (y_inside && x_inside) {
            if (level > topLevel || !topArea) {
                topArea = child;
                topLocal = child_local;
                topLevel = level;
            }
        }
        find_top_area(child, child_local, topArea, topLocal, 
                      topLevel, level + 1);
    }
}

AreaPos Area::mapToCursorPos(const CPoint& local) const
{
    DBG(XSL.CURSOR) << chain()->name() << " mapping local pos" << " x:" << fix_fmt(local.x_)
                    << " y:" << fix_fmt(local.y_) << std::endl;
    DBG_IF(XSL.CURSOR) dump(0, false);

    const bool is_vertical =
        !(isProgression(HORIZONTAL) || isProgression(PARALLEL));

    for (Area* child = firstChild(); child; child = child->nextSibling()) {

        CPoint child_local(local - child->allcPoint() - child->contPoint());

        DBG(XSL.CURSOR) << "  height: " << child->contRange().h_ << std::endl;
        
        bool y_inside = ((child_local.y_ >= 0) &&
                         (child_local.y_ <= child->contRange().h_));
        bool x_inside = ((child_local.x_ >= 0) &&
                         (child_local.x_ <= child->contRange().w_));

        DBG(XSL.CURSOR) << "  child local: " << child_local.x_ << ":"
                        << child_local.y_ << " x_inside: " << x_inside
                        << " y_inside: " << y_inside << std::endl;
        DBG_IF(XSL.CURSOR) child->dump(0, false);

        if (LIST_BLOCK_FO == child->chain()->type() && y_inside && x_inside) {
            Area* top_area = 0;
            CPoint top_local;
            int top_level = 0;
            find_top_area(child, child_local, top_area,
                          top_local, top_level, 0);
            if (top_area) 
                return top_area->mapToCursorPos(top_local);
        }

        switch (chain_->progression()) {
            case VERTICAL :
            case AMORPHOUS :
                if (y_inside)
                    return child->mapToCursorPos(child_local);
                break;
            case PARALLEL : 
            case HORIZONTAL :
                if (x_inside)
                    return child->mapToCursorPos(child_local);
                break;
            default:
                if (y_inside && x_inside)
                    return child->mapToCursorPos(child_local);
                break;
        }

        CRect c_rect(child->allcPoint() + child->contPoint(),
                     child->contRange());
        CPoint bott_right = c_rect.bottomRight();
        CType min = (is_vertical) ? c_rect.origin_.y_: c_rect.origin_.x_;
        CType max = (is_vertical) ? bott_right.y_    : bott_right.x_;
        CType cur = (is_vertical) ? local.y_         : local.x_;

        if (min >= cur)
            if (!child->prevChunk()) {
                AreaPos area_pos = areaPos(child->chain());
                area_pos.findAllowed(true, true);
                return area_pos;
            }
            else
                return child->mapToCursorPos(child_local);

        if (max <= cur && !child->nextSibling())
            if (!child->nextChunk()) {
                const Chain* child_chain = child->chain();
                ulong pos = 0;
                if (child_chain->firstChunk()->parent() == this)
                    pos = areaPos(child_chain).pos() + 1;
                AreaPos area_pos(this, pos);
                area_pos.findAllowed(false, true);
                return area_pos;
            }
            else
                return child->mapToCursorPos(child_local);
    }
    if (AMORPHOUS == chain_->progression())
        return AreaPos();

    const Area* area = this;
    while (!area->empty())
        area = area->lastChild();
    AreaPos area_pos(area, QMAX(int(0), int(area->chunkPosCount() - 1)));
    if (SECTION_CORNER_AREA == area->type()) {
        if (local.x_ < contRange().w_/2.0) {
            area_pos = AreaPos::getPosBeforeArea(this);
            area_pos.findAllowed(false, true);
        }
        else
            area_pos = AreaPos::getPosAfterArea(this);
    }
    area_pos.findAllowed(true, true);
    return area_pos;
}

CRect Area::bordRect() const
{
    CType top(bord_.top_ + padd_.top_);
    CType left(bord_.left_ + padd_.left_);
    CType right(bord_.right_ + padd_.right_);
    CType bottom(bord_.bottom_ + padd_.bottom_);

    return CRect(CPoint(-left, -top),
                 CRange(contRange_.w_ + left + right,
                        contRange_.h_ + top + bottom));
}

static long chunk_pos(const Area* area)
{
    long count = -1;
    const Chain* chain = area->chain();
    while (chain) {
        if (chain->firstChunk()->parent() == area->parent())
            count++;
        else
            break;
        chain = chain->prevChain();
    }
    return count;
}

CRect Area::absCursorRect(ulong chunkPos) const
{
    DBG(XSL.CURSOR) << "Cursor pos:" << chunkPos 
                    << " in:" << name() << std::endl;

    bool prev = false;

    const Chain* chain = chainAt(chunkPos);

    if (chain_->hasParentOrigin()) {
        if (0 == chunkPos && 0 == chain && 0 == prevChunk())
            return parent()->absCursorRect(chunk_pos(this));
        if (0 == chain && 0 == nextChunk())
            return parent()->absCursorRect(chunk_pos(this) + 1);
    }

    if (!chain && !empty()) {
        prev = true;
        if (chunkPos)
            chain = chainAt(chunkPos - 1);
        else
            chain = chainBefore(chunkPos);
    }
    if (chain) {
        Area* area = (prev) ? chain->lastChunk() : chain->firstChunk();

        CRect area_rect(area->bordRect());
        if (area_rect.extent_.h_ == CTYPE_MAX)
            area_rect.extent_.h_ = 2;//area_rect.extent_.w_;
        DBG(XSL.CURSOR) << "Cursor geometry h:" << area_rect.extent_.h_ 
                        << " w:" << area_rect.extent_.h_ << std::endl;
        
        area_rect.origin_ += area->allcPoint() + area->contPoint();
        CPoint top_left = area_rect.origin_;
        CPoint bott_right = area_rect.bottomRight();

        CPoint abs_cont = absAllcPoint_ + contPoint_;
        if (isProgression(VERTICAL))
            if (prev)
                return CRect(abs_cont + CPoint(bott_right.x_,
                                               bott_right.y_ -cursorHeight()),
                             CRange(0, cursorHeight()));
            else
                return CRect(abs_cont + top_left, CRange(0, cursorHeight()));
        else
            if (prev)
                return CRect(abs_cont + CPoint(bott_right.x_, top_left.y_),
                             CRange(0, area_rect.extent_.h_));
            else
                return CRect(abs_cont + top_left,
                             CRange(0, area_rect.extent_.h_));
    }
    return CRect(absAllcPoint_ + contPoint_, CRange(0, cursorHeight()));
}

CPoint Area::mapTo(const Area* ancestor, const CPoint& content) const
{
    CPoint point(content);
    for (const Area* area = this; area; area = area->parent()) {
        if (ancestor == area)
            return point;
        point += area->allcPoint() + area->contPoint();
    }
    return point;
}

CPoint Area::mapFrom(const Area* ancestor, const CPoint& content) const
{
    CPoint point(content);
    for (const Area* area = this; area; area = area->parent()) {
        if (ancestor == area)
            return point;
        point -= area->allcPoint() + area->contPoint();
    }
    return point;
}

void Area::makeView(const AreaViewFactory* factory)
{
    view_ = factory->makeAreaView(this);
}

bool Area::operator==(const Area& area) const
{
    return (area.absAllcPoint_ == absAllcPoint_ &&
            area.allcPoint_ == allcPoint_ &&
            area.contPoint_ == contPoint_ &&
            area.contRange_ == contRange_ &&
            area.bord_ == bord_ &&
            area.padd_ == padd_);
}

bool Area::operator!=(const Area& area) const
{
    return !(operator==(area));
}

void Area::dump(int indent, bool recursively) const
{
    DINDENT(indent);
    String ind;
    ind.fill(QChar(' '), name().length());

    DBG(XSL.AREA) 
        << name() << ":" << pntr(this) << " parent:" << pntr(parent())
        << " chain:" << pntr(chain_) << " d:" << decor_ << std::endl;
    DBG(XSL.AREA) << ind << " children#:" << countChildren();
    if (Chunk::parent()) {
        DBG(XSL.AREA) << " prev-chunk:" << prevChunk() 
                      << " next-chunk:" << nextChunk() << std::endl;
    }
    else {
        DBG(XSL.AREA) << ind << " NOT inserted into area tree !\n";
    }

    DBG(XSL.AREA)
        << ind
        << " x:" << fix_fmt(allcPoint_.x_) << " y:" << fix_fmt(allcPoint_.y_)
        << " ax:" << fix_fmt(absAllcPoint_.x_)
        << " ay:" << fix_fmt(absAllcPoint_.y_)
        << " bl:" << fix_fmt(base_) << "/" << fix_fmt(baseShift_)<< std::endl;
    DBG(XSL.AREA) 
        << ind
        << " cx:" << fix_fmt(contPoint_.x_) << " cy:" << fix_fmt(contPoint_.y_)
        << " cw:" << fix_fmt(contRange_.w_);

    if (CTYPE_MAX == contRange_.h_) {
        DBG(XSL.AREA) << " ch:CTYPE_MAX" << std::endl;
    }
    else {
        DBG(XSL.AREA) << " ch:" << fix_fmt(contRange_.h_) << std::endl;
    }

    DBG(XSL.AREA) << ind
        << " bord: t:" << fix_fmt(bord_.top_) << " b:"
        << fix_fmt(bord_.bottom_) << " l:" << fix_fmt(bord_.left_) << " r:"
        << fix_fmt(bord_.right_) << std::endl;
    DBG(XSL.AREA) << ind
        << " padd: t:" << fix_fmt(padd_.top_) << " b:"
        << fix_fmt(padd_.bottom_) << " l:" << fix_fmt(padd_.left_)
        << " r:" << fix_fmt(padd_.right_) << std::endl;
    if (recursively)
        for (Area* child = firstChild(); child; child = child->nextSibling())
            DBG_IF(XSL.AREA) child->dump(4);
}

/*
*/

bool Chain::isGeometryModified() const
{
    return isGeometryModified_;
}

void Chain::setGeometryModified()
{
    isGeometryModified_ = true;
}

void Chain::setFirstChunk(Area* chunk)
{
    RT_MSG_ASSERT(empty(), "Chain is not empty");
    appendChild(chunk);
}

void Chain::removeAllChunks(bool isToDelete)
{
    if (isToDelete)
        for (Area* area = firstChunk(); area; area = firstChunk())
            area->dispose();
    removeAllChildren();
}

void Chain::dump(int indent) const
{
    DINDENT(indent);
    uint num = 0;
    for (Area* c = firstChild(); c; c = c->nextChunk(), num++) {
        DBG(XSL.AREA) << " chunk# " << num << " :" << c << std::endl;
        DBG_IF(XSL.CHAIN) c->dump(false);
    }
    DBG(XSL.CHAIN) << " chunks:" << num << std::endl;
}

}

