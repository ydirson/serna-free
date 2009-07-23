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

#include "formatter/impl/debug.h"
#include "formatter/Area.h"
#include "formatter/impl/Areas.h"
#include "formatter/AreaPos.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/Fo.h"

#include "common/asserts.h"
#include "common/String.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Formatter {

enum CursorLocation { IS_BEFORE, IS_AFTER, IS_ON };

static bool is_generated(const Area* area)
{
    const GroveLib::Node* origin =
        XslFoExt::origin(area->chain()->headFoNode());
    return (!origin || !origin->grove()->registerVisitors());    
}
    
static bool accepts_cursor(const Area* area, CursorLocation location, 
                           ulong idx = 0)
{
    switch (area->type()) {
        case ROOT_AREA :
        case PAGE_AREA :
        case REGION_AREA :
        case SECTION_CORNER_AREA :
            return (IS_ON != location);
            break;

        case BLOCK_AREA :
        case INLINE_AREA : {
            if (IS_ON == location)
                return true;
            else    
                return !area->chain()->hasParentOrigin();
            break;
        }
        case TEXT_AREA: {
            if (is_generated(area))
                return (IS_AFTER == location);
            if (IS_ON == location) {
                const TextArea* text_area =
                    static_cast<const TextArea*>(area);
                return (0 == idx || '\n' != text_area->text()[idx - 1]);
            }
            else
                return false;
            break;
        }
        case LINE_AREA :
        case COMBO_BOX_AREA :
        case LINE_EDIT_AREA :
            return (IS_ON == location);
            break;

        case GRAPHIC_AREA :
        case UNKNOWN_AREA : {
            if (is_generated(area))
                return (IS_ON != location);
            else {
                if (IS_ON != location) 
                    return !area->chain()->hasParentOrigin();
                return true;
            }
            break;
        }
        default:
            return true;
    }
}

/*
 */
AreaPos::AreaPos()
    : area_(0),
      pos_(0)
{
}

AreaPos::AreaPos(const Area* area, ulong pos)
    : area_(area),
      pos_(pos)
{
    if (area_ && pos_ >= area_->chunkPosCount())
        pos_ = area_->chunkPosCount() - 1;
}

AreaPos::AreaPos(const AreaPos& areaPos)
    : area_(areaPos.area()),
      pos_(areaPos.pos())
{
    if (area_ && pos_ >= area_->chunkPosCount())
        pos_ = area_->chunkPosCount() - 1;
}

bool AreaPos::isNull() const
{
    return (0 == area_ || (pos_ >= area_->chunkPosCount()));
}

static Area* before_prev_sibling(const AreaPos* pos)
{
    const Area* chunk = pos->area();
    const Chain* chain_before = chunk->chainBefore(pos->pos());
    Area* prev_child = (chain_before) ? chain_before->lastChunk() : 0;
    while (0 == prev_child && chunk->prevChunk()) {
        chunk = chunk->prevChunk();
        prev_child = chunk->lastChild();
    }
    return prev_child;
}

static Area* before_sibling(const AreaPos* pos)
{
    const Area* chunk = pos->area();
    const Chain* chain_at = chunk->chainAt(pos->pos());
    Area* next_child = (chain_at) ? chain_at->firstChunk() : 0;
    while (0 == next_child && chunk->nextChunk()) {
        chunk = chunk->nextChunk();
        next_child = chunk->firstChild();
    }
    return next_child;
}

bool AreaPos::isValid() const
{
    if (isNull())
        return false;
        
    const Area* chunk = area_;
    Area* prev_chain_area = (area_->chainBefore(pos_))
        ? area_->chainBefore(pos_)->lastChunk() : 0;
    while (0 == prev_chain_area && chunk->prevChunk()) {
        chunk = chunk->prevChunk();
        prev_chain_area = chunk->lastChild();
    }
    chunk = area_;
    Area* next_chain_area = (area_->chainAt(pos_))
        ? area_->chainAt(pos_)->firstChunk() : 0;
    while (0 == next_chain_area && chunk->nextChunk()) {
        chunk = chunk->nextChunk();
        next_chain_area = chunk->firstChild();
    }

    if (!accepts_cursor(area_, IS_ON, pos_))
        return false;

    if (prev_chain_area && !accepts_cursor(prev_chain_area, IS_AFTER))
        return false;

    if (next_chain_area && !accepts_cursor(next_chain_area, IS_BEFORE))
        return false;

    return true;    
}

bool AreaPos::validate()
{
    if (isNull())
        return false;
    const Area* prev = before_prev_sibling(this);
    const Area* next = before_sibling(this);
    bool find_back = (isBackCorner() ||
                      (prev && next && prev->type() == LINE_AREA &&
                       next->type() != LINE_AREA));
    findAllowed(!find_back, true);
    return (!isNull() && isValid());
}

void AreaPos::findAllowed(bool fwd, bool lookOpposite)
{
    RT_ASSERT(!isNull());
    // Search in given direction
    AreaPos old(*this);
    while (!isNull() && !isValid()) {
        if (fwd)
            (*this)++;
        else
            (*this)--;
    }
    if ((isNull() || !isValid()) && lookOpposite) {
        // Search in opposite direction
        *this = old;
        while (!isNull() && !isValid())
            if (fwd)
                (*this)--;
            else
                (*this)++;
    }
}

bool AreaPos::isFrontCorner() const
{
    RT_ASSERT(!isNull());
    return (0 == pos_ &&
            0 == area_->prevChunk() &&
            0 == area_->chainBefore(pos_));
}

bool AreaPos::isBackCorner() const
{
    RT_ASSERT(!isNull());
    return ((area_->chunkPosCount() == pos_ + 1) &&
            0 == area_->nextChunk() &&
            0 == area_->chainAt(pos_));
}

void AreaPos::getPagePos(uint& page, uint& max_page) const
{
    RT_ASSERT(!isNull());
    const Area* area = area_;
    while (area && PAGE_AREA != area->type())
        area = area->parent();
    if (area) {
        page = static_cast<const PageArea*>(area)->pageNum();
        max_page = area->parent()->countChildren();
    }
}

AreaPos AreaPos::getPosBeforeArea(const Area* area)
{
    const Chain* chain = area->chain();
    area = area->parent();
    if (0 == area)
        return AreaPos();
    return area->areaPos(chain);
}

AreaPos AreaPos::getPosBeforeArea() const
{
    return getPosBeforeArea(area_);
}

AreaPos AreaPos::getPosAfterArea(const Area* area)
{
    const Chain* chain = area->chain();
    area = area->parent();
    ulong pos = 0;
    if (0 == area)
        return AreaPos();
    //! TODO: find the right way to operate with not formatted chains
    if (chain && 0 == chain->firstChunk()) {
        DBG(XSL.TEST) << "AreaPos++: chain not yet formatted\n";
        return AreaPos();
    }
    //!
    if (area->isProgression(PARALLEL)) {
        const Chain* next_chain = chain->nextChain();
        if (next_chain) {
            if (0 == next_chain->firstChunk()) {
                DBG(XSL.TEST) << "AreaPos++: chain not yet formatted\n";
                return AreaPos();
            }
            area = next_chain->firstChunk()->parent();
            pos = area->areaPos(next_chain).pos();
            return AreaPos(area, pos);
        }
    }
    if (chain->firstChunk()->parent() == area)
        pos = area->areaPos(chain).pos() + 1;
    return AreaPos(area, pos);
}

AreaPos AreaPos::getPosAfterArea() const
 {
    return getPosAfterArea(area_);
}

AreaPos& AreaPos::operator+=(ulong shift)
{
    RT_ASSERT(!isNull());

    if (!area_->empty()) {
        if (isBackCorner())
            *this = getPosAfterArea();
        else {
            const Chain* chain_after = area_->chainAt(pos_);
            if (chain_after)
                area_ = chain_after->firstChunk();
            else
                area_ = area_->nextChunk();
            pos_ = 0;
        }
    }
    else {
        pos_ += shift;
        if (pos_ >= area_->chunkPosCount()) {
            if (area_->nextChunk()) {
                area_ = area_->nextChunk();
                pos_ = 0;
            }
            else
                *this = getPosAfterArea();
        }
    }
    return *this;
}

AreaPos& AreaPos::operator-=(ulong shift)
{
    RT_ASSERT(!isNull());

    if (!area_->empty()) {
        if (isFrontCorner())
            *this = getPosBeforeArea();
        else {
            const Chain* chain_before = area_->chainBefore(pos_);
            pos_ = 0;
            //! TODO: find the right way to operate with not formatted chains
            if (chain_before && 0 == chain_before->firstChunk()) {
                DBG(XSL.TEST) << "AreaPos--: chain not yet formatted\n";
                area_ = 0;
                return *this;
            }
            if (chain_before)
                area_ = chain_before->lastChunk();
            else
                area_ = area_->prevChunk();
            if (area_)
                pos_ = area_->chunkPosCount() - 1;
        }
    }
    else {
        if (pos_ < shift) {
            if (area_->prevChunk()) {
                area_ = area_->prevChunk();
                pos_ = area_->chunkPosCount() - 1;
            }
            else
                *this = getPosBeforeArea();
        }
        else
            pos_ -= shift;
    }
    return *this;
}

bool AreaPos::operator<(const AreaPos& p) const
{
    RT_ASSERT(!isNull());
    XTreeNodeCmpResult cmp = area_->comparePos(p.area());
    if (EQUAL == cmp)
        return (pos_ < p.pos());
    return (LESS == cmp);
}

bool AreaPos::operator==(const AreaPos& p) const
{
    return (pos_ == p.pos() && area_ == p.area());
}

bool AreaPos::operator!=(const AreaPos& p) const
{
    return !operator==(p);
}

void AreaPos::dump() const
{
    DBG(XSL.TYPES) << "AreaPos: area:" << area_ << "(" << area_->name() 
                   << ") " << " pos " << pos_ << std::endl;
}

/*
 */
ChainPos::ChainPos()
    : chain_(0), pos_(0)
{
}

ChainPos::ChainPos(const AreaPos& areaPos)
    : chain_(0),
      pos_(0)
{
    if (areaPos.isNull())
        return;

    const Area* chunk = areaPos.area();
    chain_ = areaPos.area()->chain();
    pos_ = areaPos.pos();
    // If this is top-level area ...
    if (!chain_->hasChildChains())
        for (chunk = chunk->prevChunk(); chunk; chunk = chunk->prevChunk())
            pos_ += chunk->chunkPosCount() - 1;
    else {
        // If the chain contains subchain(s)
        const Chain* sub_chain = chunk->chainAt(pos_);
        if (sub_chain)
            pos_ = sub_chain->chainPos();
        else
            if (0 < pos_) {
                sub_chain = chunk->chainAt(pos_ - 1);
                pos_ = sub_chain->chainPos() + 1;
            }
            else {
                for (chunk = chunk->nextChunk(); chunk;
                     chunk = chunk->nextChunk()) {
                    sub_chain = chunk->chainAt(0);
                    if (sub_chain) {
                        pos_ = sub_chain->chainPos();
                        break;
                    }
                }
                if (0 == sub_chain)
                    pos_ = chain_->chainPosCount() - 1;
            }
    }
}

ChainPos::ChainPos(const Chain* chain, ulong pos)
    : chain_(chain),
      pos_(pos)
{
    if (chain_ && pos_ >=  chain_->chainPosCount())
        pos_ = chain_->chainPosCount() - 1;
}

bool ChainPos::isNull() const
{
    return (0 == chain_ || (pos_ >= chain_->chainPosCount()));
}

bool ChainPos::isFrontCorner() const
{
    RT_ASSERT(!isNull());
    return (0 == pos_);
}

bool ChainPos::isBackCorner() const
{
    RT_ASSERT(!isNull());
    return (chain_->chainPosCount() == (pos_ + 1));
}

AreaPos ChainPos::toAreaPos() const
{
    if (isNull())
        return AreaPos();

    if (!chain_->hasChildChains()) {
        ulong pos = pos_;
        for (Area* chunk = chain_->firstChunk(); chunk;
             chunk = chunk->nextChunk()) {
            ulong pos_count = chunk->chunkPosCount() - 1;
            if (pos_count < pos)
                pos -= pos_count;
            else
                return AreaPos(chunk, pos);
        }
    }
    else {
        Area* next_head = (!isBackCorner())
            ? chain_->chainAt(pos_)->firstChunk() : 0;
        Area* prev_tail = (pos_)
            ? chain_->chainAt(pos_ - 1)->lastChunk() : 0;
        bool use_next =
            (next_head && ((0 == prev_tail) ||
                           (next_head->parent() == prev_tail->parent())));
        bool use_prev = (prev_tail && ((0 == next_head) || !use_next));
        if (use_next)
            return next_head->parent()->areaPos(next_head->chain());
        if (use_prev)
            return AreaPos(prev_tail->parent(),
                           prev_tail->parent()->chunkPosCount() - 1);
    }
    return AreaPos(0, 0);
}

bool ChainPos::operator<(const ChainPos& p) const
{
    if (p.chain() == chain_)
        return (pos_ < p.pos());
    Chain* common = p.chain()->commonAos(chain_);
    if (common == chain_) {
        const Chain* p_chain = p.chain();
        while (p_chain && p_chain->parentChain() != common)
            p_chain = p_chain->parentChain();
        if (p_chain)
            return pos_ <= p_chain->chainPos();
    }
    if (common == p.chain()) {
        const Chain* p_chain = chain_;
        while (p_chain && p_chain->parentChain() != common)
            p_chain = p_chain->parentChain();
        if (p_chain)
            return p.pos() > p_chain->chainPos();
    }
    return (LESS == static_cast<const Fo*>(chain_)->comparePos(
        static_cast<const Fo*>(p.chain())));
}

bool ChainPos::operator>(const ChainPos& p) const
{
    return (!operator==(p) && !operator<(p));
}

bool ChainPos::operator==(const ChainPos& p) const
{
    return (p.chain() == chain_) && (p.pos() == pos_);
}

bool ChainPos::operator!=(const ChainPos& p) const
{
    return !operator==(p);
}

void ChainPos::dump() const
{
    DBG(XSL.TYPES)
        << "ChainPos: chain:" << chain_ << " pos" << pos_ << std::endl;
}

/*!
 */
ChainSelection::ChainSelection()
    : start_(),
      end_()
{}

ChainSelection::ChainSelection(const ChainPos& pos)
    : start_(pos),
      end_(pos)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = ChainPos();
}

ChainSelection::ChainSelection(const ChainPos& start, const ChainPos& end)
    : start_(start),
      end_(end)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = ChainPos();
}

ChainSelection::ChainSelection(const AreaPos& start, const AreaPos& end)
    : start_(start),
      end_(end)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = ChainPos();
}

ChainSelection::ChainSelection(const ChainSelection& sel)
    : start_(sel.start()),
      end_(sel.end())
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = ChainPos();
}

bool ChainSelection::operator==(const ChainSelection& s) const
{
    return (start_ == s.start() && end_ == s.end());
}

bool ChainSelection::operator!=(const ChainSelection& s) const
{
    return !operator==(s);
}

ChainSelection ChainSelection::intersection(const ChainSelection& sel) const
{
    if ((isEmpty() && sel.isEmpty()) ||
        (maxPos() < sel.minPos()) || (minPos() > sel.maxPos()))
        return ChainSelection();

    ChainPos start((minPos() < sel.minPos()) ? sel.minPos() : minPos());
    ChainPos end((maxPos() > sel.maxPos()) ? sel.maxPos() : maxPos());
    if (start < end)
        return ChainSelection(start, end);
    return ChainSelection(start);
}

static inline const Chain* descendant_or_self(const Chain* chain,
                                              const Chain* ancestor)
{
    while (chain && chain != ancestor &&
           chain->parentChain() && chain->parentChain() != ancestor)
        chain = chain->parentChain();
    return chain;
}

static inline ChainPos adjust_to_aos(const Chain* chain, ulong pos,
                                     const Chain* aos, ulong shift = 0)
{
    if (chain == aos)
        return ChainPos(chain, pos);
    if (TEXT_FO == static_cast<const Fo*>(chain)->type() &&
        chain->parentChain() == aos) {
        if (0 == pos)
            return ChainPos(aos, chain->chainPos());
        if (chain->chainPosCount() == pos + 1)
            return ChainPos(aos, chain->chainPos() + 1);
        return ChainPos(chain, pos);
    }
    return ChainPos(aos, chain->chainPos() + shift);
}

const Chain* ChainSelection::commonChain() const
{
    return start_.chain()->commonAos(end_.chain());
}

void ChainSelection::balance()
{
    if (isEmpty())
        return;
    const Chain* common_chain = commonChain();
    const Chain* s_chain = descendant_or_self(start_.chain(), common_chain);
    const Chain* e_chain = descendant_or_self(end_.chain(), common_chain);
    if (0 == s_chain || 0 == e_chain)
        return;
    const bool reverse = start_ > end_;
    if (reverse) {
        start_ = adjust_to_aos(s_chain, start_.pos(), common_chain, 1);
        end_ = adjust_to_aos(e_chain, end_.pos(), common_chain);
    }
    else {
        start_ = adjust_to_aos(s_chain, start_.pos(), common_chain);
        end_ = adjust_to_aos(e_chain, end_.pos(), common_chain, 1);
    }
}

const ChainPos& ChainSelection::minPos() const
{
    if (start_ < end_)
        return start_;
    else
        return end_;
}

const ChainPos& ChainSelection::maxPos() const
{
    if (end_ > start_)
        return end_;
    else
        return start_;
}

void ChainSelection::dump() const
{
    DBG(XSL.TYPES) << "ChainSelection:" << std::endl;
    DBG(XSL.TYPES) << "From:" << std::endl;
    DBG_IF(XSL.TYPES) minPos().dump();
    DBG(XSL.TYPES) << "To:" << std::endl;
    DBG_IF(XSL.TYPES) maxPos().dump();
}

/*
 */
TreePos::TreePos(const AreaPos& areaPos)
{
    if (!areaPos.isNull()) {
        ChainPos chain_pos(areaPos);
        chain_pos.chain()->treeloc(treeloc_);
        treeloc_.append(chain_pos.pos());
    }
}

TreePos::TreePos(const ChainPos& chain_pos)
{
    if (!chain_pos.isNull()) {
        chain_pos.chain()->treeloc(treeloc_);
        treeloc_.append(chain_pos.pos());
    }
}

TreePos::TreePos(const TreelocRep& treeloc)
    : treeloc_(treeloc)
{
}

bool TreePos::isNull() const
{
    return (0 == treeloc_.length());
}

AreaPos TreePos::toAreaPos(const Area* root) const
{
    return toChainPos(root).toAreaPos();
}

ChainPos TreePos::toChainPos(const Area* root) const
{
    if (!root || isNull())
        return ChainPos();
    ulong pos = treeloc_[treeloc_.length() - 1];
    TreelocRep chain_loc(128);
    for (int i = treeloc_.length() - 2; i >= 0; --i)
        chain_loc += treeloc_[i];
    const Chain* chain = root->chain()->byTreeloc(chain_loc);
    if (!chain)
        return ChainPos();
    if (pos >= chain->chainPosCount())
        pos = chain->chainPosCount();
    return ChainPos(chain, pos);
}

bool TreePos::operator<(const TreePos& p) const
{
    return (treeloc_ < p.treeloc());
}

bool TreePos::operator<=(const TreePos& p) const
{
    return ((treeloc_ < p.treeloc()) || (treeloc_ == p.treeloc()));
}

bool TreePos::operator>(const TreePos& p) const
{
    return (p.treeloc() < treeloc_);
}

bool TreePos::operator>=(const TreePos& p) const
{
    return ((p.treeloc() < treeloc_) || (treeloc_ == p.treeloc()));
}

bool TreePos::operator==(const TreePos& p) const
{
    return (treeloc_ == p.treeloc());
}

bool TreePos::operator!=(const TreePos& p) const
{
    return !operator==(p);
}

void TreePos::dump() const
{
    DBG(XSL.TYPES) << "TreePos treeloc:" << treeloc_.toString() << std::endl;
}

/*!
 */
TreeSelection::TreeSelection()
    : start_(),
      end_()
{}

TreeSelection::TreeSelection(const TreePos& t)
    : start_(t),
      end_(t)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = TreePos();
}

TreeSelection::TreeSelection(const TreePos& start, const TreePos& end)
    : start_(start),
      end_(end)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = TreePos();
}

TreeSelection::TreeSelection(const AreaPos& start, const AreaPos& end)
    : start_(start),
      end_(end)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = TreePos();
}

TreeSelection::TreeSelection(const TreeSelection& r)
    : start_(r.start()),
      end_(r.end())
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = TreePos();
}

bool TreeSelection::operator==(const TreeSelection& s) const
{
    return (start_ == s.start() && end_ == s.end());
}

bool TreeSelection::operator!=(const TreeSelection& s) const
{
    return !operator==(s);
}

TreeSelection TreeSelection::intersection(const TreeSelection& r) const
{
    if ((isEmpty() && r.isEmpty()) ||
        (maxLoc() < r.minLoc()) ||
        (minLoc() > r.maxLoc()))
        return TreeSelection();

    TreePos left(minLoc());
    if (left < r.minLoc())
        left = r.minLoc();

    TreePos right(maxLoc());
    if (r.maxLoc() < right)
        right = r.maxLoc();

    if (left < right)
        return TreeSelection(left, right);
    return TreeSelection(left);
}

TreePos TreeSelection::lastCommon() const
{
    if (start_ == end_)
        return start_;
    uint min  = (start_.treeloc().length() < end_.treeloc().length()) ?
        start_.treeloc().length() : end_.treeloc().length();

    TreelocRep common;
    for (uint c = 0; c < min; c++) {
        ulong i1 = minLoc().treeloc()[c];
        ulong i2 = maxLoc().treeloc()[c];
        common.append((i1 < i2) ? i1 : i2);
        if (i1 != i2)
            break;
    }
    return TreePos(common);
}

bool TreeSelection::isBalanced() const
{
    return ((start_.treeloc().length() == end_.treeloc().length()) &&
            (start_.treeloc().length() == lastCommon().treeloc().length()));
}

const TreePos& TreeSelection::minLoc() const
{
    if (start_ < end_)
        return start_;
    else
        return end_;
}

const TreePos& TreeSelection::maxLoc() const
{
    if (end_ > start_)
        return end_;
    else
        return start_;
}

void TreeSelection::dump() const
{
    DBG(XSL.TYPES) << "From:" << std::endl;
    DBG_IF(XSL.TYPES) minLoc().dump();
    DBG(XSL.TYPES) << "To:" << std::endl;
    DBG_IF(XSL.TYPES) maxLoc().dump();
}

}
