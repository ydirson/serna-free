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
#include "formatter/impl/FoController.h"
#include <iomanip>
#include "formatter/impl/Areas.h"

const uint MAX_FAIL_COUNT = 5;

USING_COMMON_NS;

namespace Formatter {

Area* FoController::lastChunk() const
{
    if (!lastArea_.isNull() && lastArea_->chain() == fo_)
        return lastArea_.pointer();
    return 0;
}

CPoint FoController::absContPoint() const
{
    return newAllcPoint_ +
        fo_->getContPoint(newArea_.pointer(), 0 == lastChunk());
}

inline void FoController::setNewAreaSpace(const Area* insertAfter)
{
    if (parent()) {
        newAreaSpace_ = parent()->getSpaceAfter(insertAfter);
        newAllcPoint_ = parent()->absContPoint() +
            fo_->getAllcPoint(newAreaSpace_.space_.origin_);
    RT_MSG_ASSERT((CTYPE_MAX != newAreaSpace_.space_.origin_.y_), "bad y_");
    RT_MSG_ASSERT((0 != newAreaSpace_.space_.extent_.h_), "bad dy");
    }
    else {
        newAreaSpace_.area_ = 0;
        newAreaSpace_.space_ = CRect();
        newAreaSpace_.maxh_ = newAreaSpace_.maxw_ = true;
        newAllcPoint_ = CPoint();
    }
}

/*! NOTE: Do not alter area tree or area chain while preparing new area.
          Newly created area may be thrown away later (when closed) so
          don`t affect others
 */
bool FoController::prepareArea()
{
    RT_MSG_ASSERT(newArea_.isNull(), "Area is already prepared");

    const Area* last_chunk = lastChunk();
    const Area* insert_after = lastArea_.pointer();
    if (parent() && insert_after &&
        insert_after->parent() != parent()->newArea())
        insert_after = 0;

    DBG(XSL.LAYOUT) << "Preparing area, last_chunk: " << last_chunk 
                    << " insert_after: " << insert_after 
                    << "\n               parent area: " 
                    << ((parent()) 
                        ? parent()->newArea().pointer() : (Area*)(0))
                    << std::endl;

    /* If this FoController and its parent lays children out in the same
       direction then new area should not be created in
       lastArea_->parent area. So return false now to proceed later. */
    if (parent() && last_chunk &&
        last_chunk->parent() == parent()->newArea().pointer() &&
        (direction() == parent()->direction() ||
         //TODO: ckeck parent()->fo_->isParallel() below
         fo_->parent()->isParallel() || fo_->isParallel())) {
        DBG(XSL.LAYOUT) << "   !!! Stop at this parent !!!" << std::endl;
        discardChildrenAfter(insert_after);
        failCount_++;
        return false;
    }
    setNewAreaSpace(insert_after);
    fo_->checkProperties(newAreaSpace_);

    if (!fo_->isTerminal()) {
        oldArea_ = (last_chunk) ? last_chunk->nextChunk() : fo_->firstChunk();
        if (!oldArea_.isNull() && fo_->isEnoughSpace(newAreaSpace_,
                                                     last_chunk)) {
            DBG(XSL.LAYOUT) << "  Using old area:" << std::endl;
            DBG_EXEC(XSL.LAYOUT, oldArea_->dump(false));

            newArea_ = oldArea_;
            fo_->expandGeometry(oldArea_, newAreaSpace_.space_);

            DBG_EXEC(XSL.LAYOUT, newAreaSpace_.dump());
            handleAffectedChildren();
            return true;
        }
    }
    DBG(XSL.LAYOUT) << "   Making new area ... " << std::endl;
    bool force_to_make = (failCount_ > MAX_FAIL_COUNT);

    newAreaSpace_.enoughW_ = newAreaSpace_.enoughH_ = true;
    DBG_EXEC(XSL.LAYOUT, newAreaSpace_.dump());

    newArea_ = fo_->makeArea(newAreaSpace_, last_chunk, force_to_make);
    if (newArea_.isNull()) {
        DBG(XSL.LAYOUT) << "   !!! area does not fit:" << std::endl;
        DBG_EXEC(XSL.LAYOUT, newAreaSpace_.dump());
        DBG_EXEC(XSL.LAYOUT, fo_->dump());
        failCount_++;
        discardChildrenAfter(insert_after);
        return false;
    }
    DBG(XSL.LAYOUT) << "   newAllcPoint x:" << fix_fmt(newAllcPoint_.x_)
                    << " y:" << fix_fmt(newAllcPoint_.y_) << std::endl;

    DBG(XSL.LAYOUT) << "   new area:" << std::endl;
    DBG_EXEC(XSL.LAYOUT, newArea_->dump(false));
    if (last_chunk)
        oldArea_ = last_chunk->nextChunk();
    else
        oldArea_ = fo_->firstChunk();
    if (!oldArea_.isNull()) {
        DBG(XSL.LAYOUT) << "   old area:" << std::endl;
        DBG_EXEC(XSL.LAYOUT, oldArea_->dump(false));
    }
    failCount_ = 0;

    return true;
}

static bool is_empty_area(const Area* area)
{
    if (!area)
        return true;
    if (area->contRange().h_ && !area->firstChild())
        return false;
    if (!is_empty_area(area->firstChild()))
        return false;
    return true;
}

bool FoController::closeArea(bool isContentExhausted)
{
    RT_MSG_ASSERT(!newArea_.isNull(), "No area to close");

    if (newArea_->lastChild() && newArea_->lastChild()->nextChunk())
        isContentExhausted = false;
    
    //! Let area occupy space necessary to carry children
    DBG(XSL.LAYOUT) << " closing updated area: " << newArea_.pointer() 
                    << " isContentExhausted:" << isContentExhausted 
                    << std::endl;
    DBG_EXEC(XSL.LAYOUT, newAreaSpace_.dump());

    newArea_->absAllcPoint_ = newAllcPoint_;
    fo_->updateGeometry(newArea_, newAreaSpace_,
                        lastChunk(), isContentExhausted);

    //! Throw area away if it is absolutely empty
    if (!fo_->hasContent(newArea_) && !newArea_->hasDecoration(ANY_DECOR)) {
        discardChildrenAfter(lastArea_.pointer());
        DBG(XSL.LAYOUT) << "   empty area removed:" << std::endl;
        oldArea_ = newArea_ = 0;
        return false;
    }   

#if 1
    if (removeLineArea())
        return false;
    if (LIST_ITEM_FO == fo_->type()) {
        DBG(XSL.LAYOUT) << "   listItem children check\n";
        for (FoController* c = firstChild(); c; c = c->nextSibling()) {
            DBG_IF(XSL.LAYOUT) c->dump();
            if (!c->lastArea().isNull() && c->lastArea()->prevChunk())
                continue;
            if (is_empty_area(c->lastArea().pointer())) {
                discardChildrenAfter(lastArea_.pointer());
                DBG(XSL.LAYOUT) << "   !!! Empty LIST_ITEM area removed:\n";
                oldArea_ = newArea_ = 0;

                removeAllChildren();
                for (Fo* fo = fo_->firstChild(); fo; fo = fo->nextSibling()) 
                    appendChild(new FoController(viewFactory_, fo,
                                                 0, splitCount_, foMap_));
                return false;
            }
        }
    }
#endif

    if (newArea_ == oldArea_) {
        DBG(XSL.LAYOUT) << "   closing newArea_ == oldArea_" << std::endl;
        newArea_->ATN::remove();
        placeAreaNode();
    }
    else {
        if (oldArea_.isNull())
            placeArea();
        else
            if (*oldArea_.pointer() != *newArea_.pointer())
                replaceArea();
            else {
                DBG(XSL.LAYOUT) 
                    << "   old area used - no replacement" << std::endl;
                newArea_ = oldArea_;
                newArea_->absAllcPoint_ = newAllcPoint_;
                newArea_->remove();
                placeAreaNode();
            }
    }

    if (viewFactory_)
        if (newArea_->getView()) {
            newArea_->getView()->updateGeometry();
            DBG(XSL.LAYOUT) << "   abs_alloc x:" << newAllcPoint_.x_
                            << " y:" << newAllcPoint_.y_ << std::endl;
        }
        else
            newArea_->makeView(viewFactory_);

    //! If chain is finished - remove remaining (old) chunks
    if (isContentExhausted && fo_->isChainFinishedAt(newArea_)) {
        discardChainAfter(newArea_);
        if (parent())
            parent()->handleAffectedSiblings(newArea_.pointer());
    }

    lastArea_ = newArea_;
    oldArea_ = newArea_ = 0;

    DBG(XSL.LAYOUT) << "   area closed:" << std::endl;
    DBG_IF(XSL.LAYOUT) lastArea_->dump();

    //! Check if closed area fits to previous page
    checkSpaceAbove(); 
    RT_MSG_ASSERT((CTYPE_MAX != lastArea_->allcH()), 
                  "Invalid area height after close");

    return true;
}

static const TextArea* text_area(const Area* area)
{   
    if (TEXT_AREA == area->type())
        return static_cast<const TextArea*>(area);
    for (const Area* a = area->firstChild(); a; a = a->nextSibling()) {
        const TextArea* ta = text_area(a);
        if (ta)
            return ta;
    }
    return 0;
}

void FoController::moveArea(Area* area, const CPoint& shift) const
{
    area->absAllcPoint_ += shift;
    if (area->getView())
        area->getView()->updateGeometry();
    for (Area* a = area->firstChild(); a; a = a->nextSibling()) 
        moveArea(a, shift);
}

void FoController::prepareArea(Area* area, const Allocation& newAreaSpace,
                               const CPoint& newAllcPoint)
{
    DBG(XSL.LAYOUT) << "Preparing area:" << std::endl;
    dumpState();

    lastArea_ = area->ATN::prevSibling();
    RT_MSG_ASSERT((lastArea_.isNull() || 0 != lastArea_->parent()), 
                  "! lastArea_ must have parent");
    failCount_ = 0;
    newAreaSpace_ = newAreaSpace;
    newAllcPoint_ = newAllcPoint;
    
    newAreaSpace_.enoughW_ = newAreaSpace_.enoughH_ = true;
    fo_->checkProperties(newAreaSpace_);

    oldArea_ = newArea_ = area;
    DBG(XSL.LAYOUT) << "==== prepared:" << std::endl;
    dumpState();
}

Area* prevParallelArea(const Fo* fo, const Area* parent)
{
    for (fo = fo->prevSibling(); fo; fo = fo->prevSibling())
        for (Area* area = fo->firstChunk(); area; area = area->nextChunk())
            if (area->parent() == parent)
                return area;
    return 0;
}

inline void FoController::placeAreaNode()
{
    if (parent()) {
        Area* parent_area = parent()->newArea().pointer();
        Area* after = (fo_->parent() && fo_->parent()->isParallel())
            ? prevParallelArea(fo_.pointer(), parent_area) 
            : lastArea_.pointer();
        if (after && after->parent() == parent_area)
            after->insertAfter(newArea_.pointer());
        else {
            Area* before = parent_area->firstChild();
            if (before)
                before->insertBefore(newArea_.pointer());
            else
                parent_area->appendChild(newArea_.pointer());
        }
    }
}

inline void FoController::placeArea()
{
    RT_MSG_ASSERT(oldArea_.isNull(), "New area should replace the old one");
    //! Place area to area tree
    placeAreaNode();
    //! Place area to chain
    Area* last_chunk = lastChunk();
    if (last_chunk)
        last_chunk->insertChunkAfter(newArea_.pointer());
    else
        newArea_->setFirstChunk(fo_.pointer());
}

inline void FoController::replaceArea()
{
    RT_MSG_ASSERT(!oldArea_.isNull(), "No area to be replaced");
    DBG(XSL.LAYOUT) << "replacing old area ======" << std::endl;
    DBG_IF(XSL.LAYOUT) oldArea_->dump(false);
    DBG(XSL.LAYOUT) << "with new area:" << std::endl;
    DBG_IF(XSL.LAYOUT) newArea_->dump(false);
    //! Place area to area tree
    placeAreaNode();
    //! Replace old area in chain and remove it from tree
    newArea_->replace(oldArea_);

    DBG_IF(XSL.LAYOUT) fo_->dump();
    DBG(XSL.LAYOUT) << "end of replacement ======" << std::endl;
}

inline void FoController::discardChainAfter(const AreaPtr& area)
{
    while (area->nextChunk()) {
        Area* child = area->nextChunk();
        DBG(XSL.LAYOUT) << "Discarding area:" << child << std::endl;
        DBG_IF(XSL.LAYOUT) child->dump();
        RT_MSG_ASSERT(child->parent(), "Cannot discard parentless area");
        child->dispose();
    }
}

void FoController::discardChildrenAfter(const Area* area) const
{
    if (fo_->parent()->isParallel())
        return;

    DBG(XSL.LAYOUT) << "Discarding children after area:" << area 
                    << " in chain:" << fo_.pointer() << std::endl;

    Area* chunk = (area)
        ? area->nextSibling()
        : parent()->newArea()->firstChild();
    if (!chunk) {
        DBG(XSL.LAYOUT) 
            << "  no children after area to discard:" << area << std::endl;
        return;
    }

    while (chunk) {
        DBG(XSL.LAYOUT) << "Discarding chunk:" << chunk << std::endl;
        
        Chain* chain = const_cast<Chain*>(chunk->chain());
        bool remove_chunks = (area)
            ? (chain != area->chain()) : (!chunk->prevChunk());
        if (remove_chunks) {
            DBG(XSL.LAYOUT) << "   chain discarded:" << chain << std::endl;
            Fo* chain_fo = static_cast<Fo*>(chain);
            chain_fo->detachFromFormatter(this);
            chain->removeAllChunks(true);
        }
        else {
            DBG(XSL.LAYOUT) << "   chunk disposed:" << chunk << std::endl;
            chunk->dispose();
        }
        chunk = (area)
            ? area->nextSibling()
            : parent()->newArea()->firstChild();
    }
}

void FoController::handleAffectedChildren() const
{
    //! TODO: check all parallel children
    DBG(XSL.LAYOUT) << "Finding affected children...\n";
    handleAffectedSiblings(0);
}

void FoController::handleAffectedSiblings(const Area* child) const
{
    //RT_MSG_ASSERT(!child->nextChunk(), "HandleArea: Chain is not finished!");

    if (fo_->isParallel())
        return;
    DBG(XSL.SHIFT) << "Finding affected siblings after area: " 
                   << child << std::endl;
    
    Chain* next_chain = (child) 
        ? child->chain()->nextChain() : fo_->firstChild();
    const Area* after_area = child;

    while (next_chain) {
        //! Checking if following siblings must be formatted unconditionally
        if (next_chain->isModified()) { 
            DBG(XSL.SHIFT) << "!!! sibling is modified: " 
                           << next_chain << std::endl;
            return; 
        }        
        if (next_chain->isGeometryModified()) { 
            DBG(XSL.SHIFT) << "!!! sibling geometry is already modified: " 
                           << next_chain << std::endl;
            return;
        }
        Area* next_area = next_chain->firstChunk();
        if (!next_area) {
            DBG(XSL.SHIFT) << "no area in sibling:" << next_chain << std::endl;
            return;
        }
        if (HORIZONTAL == fo_->progression()) {
            DBG(XSL.SHIFT) << "areas must be reformatted in horizontal layout:"
                           << next_chain << " geometry modified\n";
            next_chain->setGeometryModified();
            return;
        }        
        if (next_area->parent() != newArea_) {
            DBG(XSL.SHIFT) << "wrong parent area in sibling: " 
                           << next_chain << " geometry modified\n";
            next_chain->setGeometryModified();
            return;
        }
        //! Checking if sibling area geometry is affected
        CRect  space = getSpaceAfter(after_area).space_;
        CPoint area_abs_allc = next_area->absAllcPoint();
        CPoint new_allc = static_cast<Fo*>(next_chain)->getAllcPoint(
            space.origin_);
        CPoint new_abs_allc = newAllcPoint_ + newArea_->contPoint() + new_allc;

        //! Stop handling if area is not affected
        if (area_abs_allc == new_abs_allc) 
            return; 

        //! Checking the possibility to move following sibling area
        DBG(XSL.SHIFT) << "Area has to be moved:" << next_area << std::endl;
        if (is_being_processed(foMap_, static_cast<const Fo*>(next_chain))) {
            DBG(XSL.SHIFT) 
                << "!!! sibling is being formatted by another branch"
                << next_chain << std::endl;
            next_chain->setGeometryModified();
            return;
        }
        if (next_area != next_chain->lastChunk()) {
            DBG(XSL.SHIFT) << "Cannot move fragmented area in sibling: " 
                           << next_chain << " geometry modified\n";
            next_chain->setGeometryModified();
            return; 
        }

        //! Checking that there is enough space to move affected area to 
        DBG(XSL.SHIFT) << "space x:" << space.origin_.x_ 
                       << " y:" << space.origin_.y_ 
                       << " w:" << space.extent_.w_
                       << " h:" << space.extent_.h_ << std::endl;
        if (space.extent_.w_ < next_area->allcW() || 
            space.extent_.h_ < next_area->allcH()) {
            next_chain->setGeometryModified();
            DBG(XSL.SHIFT) << "Cannot move area:\n";
            DBG_IF(XSL.SHIFT) next_area->dump();
            return;
        }

        DBG(XSL.SHIFT) << "parent area:\n";
        DBG_IF(XSL.SHIFT) newArea_->dump();
        DBG(XSL.SHIFT) << "affected area:\n";
        DBG_IF(XSL.SHIFT) next_area->dump();

        CPoint shift = new_abs_allc - area_abs_allc;
        DBG(XSL.SHIFT) << "  sibling area: " << next_area
                       << " ax:" << area_abs_allc.x_ << "/" << new_abs_allc.x_
                       << " ay:" << area_abs_allc.y_ << "/" << new_abs_allc.y_
                       << std::endl;
        
        DBG_IF(XSL.SAFE) {
            next_chain->setGeometryModified();
            return;
        }

        DBG(XSL.SHIFT) << "Moving affected area by x:" 
                       << shift.x_ << " y:" << shift.y_ << std::endl;
        next_area->allcPoint_ = new_allc;
        
        DBG_IF(XSL.SHIFT) {
            const TextArea* text = text_area(next_area);
            if (text) {
                DBG(XSL.SHIFT) << " moving: " << text->text().toString()
                               << " by:" << shift.y_ << std::endl;
            }
        }
        
        moveArea(next_area, shift);
        DBG_IF(XSL.SHIFT) next_area->dump();

        next_chain = next_chain->nextChain();
        after_area = next_area;
    }
}

Allocation FoController::getSpaceAfter(const Area* childArea) const
{
    Allocation alloc;

    alloc.area_ = newArea_.pointer();

    if (AMORPHOUS == fo_->progression())
        alloc.maxw_ = alloc.maxh_ = true;
    else {
        bool vert = (VERTICAL == fo_->progression() ||
                     PARALLEL == fo_->progression());

        alloc.maxw_ = (newAreaSpace_.maxw_ &&
                       (vert || (!vert && 0 == childArea)));
        alloc.maxh_ = (newAreaSpace_.maxh_ &&
                       (!vert || (vert && 0 == childArea)));
    }
    alloc.space_ = fo_->getSpaceAfter(newArea_.pointer(), 0 == lastChunk(),
                                      childArea, newAreaSpace_.space_);
    return alloc;
}

PDirection FoController::direction() const
{
    return fo_->progression();
}

/////////////////////////////////////////////////////////////////////////////

//! Removes the created line if one of its children does not fit vertical limit

bool FoController::removeLineArea() 
{
    if (LINE_FO != fo_->type()) 
        return false;
    
    for (FoController* c = firstChild(); c; c = c->firstChild()) {
        if (!c->newAreaSpace_.enoughH_) {

            DBG(XSL.LAYOUT) << "!!! One of LineArea children does not fit:\n";
            c->newAreaSpace_.dump();
            DBG_IF(XSL.LAYOUT) newArea_->dump();

            //! Discard partially formatted line
            DBG(XSL.LAYOUT) << "   discarding areas after:" 
                            << lastArea_.pointer() << std::endl;
            discardChildrenAfter(lastArea_.pointer());
            if (newArea_->parent())
                newArea_->dispose();            
            oldArea_ = newArea_ = 0;
            DBG(XSL.LAYOUT) << "   LineArea removed:"   
                            << newArea_.pointer() << std::endl;

            DBG(XSL.LAYOUT) << "   removing child controller:" 
                            << firstChild()->fo() << std::endl;
            removeAllChildren();

            //! Recreate child controllers to begin formatting just after
            //  previous line area
            DBG(XSL.LAYOUT) << "   recreating child controllers\n";
            FoController* foc = this;
            if (lastAreaMade_) {
                for (Area* area = lastAreaMade_->lastChild(); 
                     area; area = area->lastChild()) {
                    const Fo* child_fo = 
                        dynamic_cast<const Fo*>(area->chain());
                    DBG(XSL.LAYOUT) << "   attaching new child controller fo:" 
                                    << child_fo << " to area:" << area
                                    << std::endl;
                    foc->appendChild(
                        new FoController(viewFactory_,
                                         const_cast<Fo*>(child_fo),
                                         area, splitCount_, foMap_));
                    foc = foc->firstChild();
                    foc->dumpState();
                }
            }
            return true;
        }
    }
    return false;
}

void FoController::dump() const 
{
    DBG(XSL.LAYOUT) << "FoController: " << this << std::endl;

    DBG(XSL.LAYOUT) << "oldArea: " << oldArea_.pointer() << std::endl;
    if (!oldArea_.isNull())
        oldArea_->dump(1);
    DBG(XSL.LAYOUT) << "newArea: " << newArea_.pointer() << std::endl;
    if (!newArea_.isNull())
        newArea_->dump(1);
    DBG(XSL.LAYOUT) << "lastArea: " << lastArea_.pointer() << std::endl;
    if (!lastArea_.isNull())
        lastArea_->dump(1);

    DBG(XSL.LAYOUT) << "newAreaSpace: " << std::endl;
    newAreaSpace_.dump();
}

/////////////////////////////////////////////////////////////////////////////

//! If created area can be placed above (in parent chain) move it and
//  reposition FoController branch

//! NOTE: 1. currently only LINE_AREAs can be placed above.
//        2. to move block areas, it is sometimes necessary to cut off
//           decorations of nested child areas and retarget the branch.

void FoController::checkSpaceAbove()
{
    if (splitCount_ > 0 || !lastArea_->parent() || lastArea_->prevSibling() ||
        lastArea_->prevChunk() || !lastArea_->parent()->parent())
        return;
    Area* parent_area = lastArea_->parent()->prevChunk();
    if (!parent_area) 
        return;
    
    DBG(XSL.LAYOUT) << "checkSpaceAbove: " << this << std::endl;
    //! Trace area tree up to area, which does not differ with one
    //  kept in FoController
    BranchDataList data_list;
    FoController* foc = parent();

    data_list.appendChild(new BranchData(this, 0, parent_area->lastChild()));
    calc_branch_data(data_list, foc, parent_area);
    
    //! Check if there enough space in parent area previous chunk
    const CRect& space = data_list.firstChild()->newAreaSpace_.space_;
    if (space.extent_.w_ < lastArea_->allcW() || 
        space.extent_.h_ < lastArea_->allcH()) 
        return;

    DBG(XSL.LAYOUT) << "   space: ";
    DBG_IF(XSL.LAYOUT) { 
        space.extent_.dump();
        lastArea_->contRange().dump();
    }
    
    //! Move area to tne new parent
    BranchData* data = data_list.firstChild();
    lastArea_->ATN::remove();
    if (data->after_)
        data->after_->ATN::insertAfter(lastArea_.pointer());
    else 
        parent_area->ATN::appendChild(lastArea_.pointer());
    lastArea_->allcPoint_ = fo_->getAllcPoint(space.origin_);
    const CPoint shift = data->newAllcPoint_ - lastArea_->absAllcPoint();
    moveArea(lastArea_.pointer(), shift);
    
    //! Reposition branch to look at new areas
    data_list.firstChild()->remove();
    for (BranchData* c = data_list.lastChild(); c; c = c->prevSibling()) 
        c->foc_->prepareArea(c->area_, c->newAreaSpace_, c->newAllcPoint_);
}

void calc_branch_data(BranchDataList& dataList, FoController* foc,
                      Area* newArea)
{
    //! Trace area tree up to (parent) area, which does not differ with one
    //  kept in FoController
    while (newArea && foc) { 
        if (foc->newArea() == newArea)
            break;        
        dataList.appendChild(
            new BranchData(foc, newArea, newArea->Area::ATN::prevSibling()));
        foc = foc->parent();
        newArea = newArea->parent();
    }

    //! Calculate spaces for all FoController, as if they processes other areas
    for (BranchData* i = dataList.lastChild(); i; i = i->prevSibling()) {
        BranchData* next = i->nextSibling();
        if (0 == next) {
            i->newAreaSpace_ = i->foc_->parent()->getSpaceAfter(i->after_);
            i->newAllcPoint_ = 
                i->foc_->parent()->absContPoint() +
                i->foc_->fo()->getAllcPoint(i->newAreaSpace_.space_.origin_);
            continue;
        }
        else {
            i->newAreaSpace_.space_ = next->foc_->getSpaceAfter(
                next->area_, i->after_, next->newAreaSpace_.space_);
            i->newAllcPoint_ = 
                next->area_->absAllcPoint() + next->area_->contPoint() +
                i->foc_->fo()->getAllcPoint(i->newAreaSpace_.space_.origin_);
        }
    }
}

inline CRect FoController::getSpaceAfter(const Area* area,
                                         const Area* childArea, 
                                         const CRect& newAreaSpace) const
{
    return fo_->getSpaceAfter(
        area, 0 == area->prevChunk(), childArea, newAreaSpace);
}

}
