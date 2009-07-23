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
#include "formatter/Exception.h"
#include "formatter/impl/FoController.h"

USING_COMMON_NS;

namespace Formatter {

bool is_being_processed(const FoControllerMap* map, const Fo* fo)  
{
    std::pair<FocMapIterator, FocMapIterator> p = map->equal_range(fo);
    for (FocMapIterator i = p.first; i != p.second; ++i) {
        if (fo == (*i).second->fo())
            return true;
    }
    return false;
}

static Fo* next_child_fo(const FoController* foc)
{
    const FoController* child = foc->lastChild();
    Fo* last_fo = (child) ? child->fo().pointer() : 0;
    Fo* next_fo = (last_fo) ? last_fo->nextSibling() : foc->fo()->firstChild();
    return next_fo;
}

/*! Returns true when there is no more  children to be processed, or
    there is no child Fo at all.
 */
static bool is_children_exhausted(const FoController* foc)
{
    for (const FoController* c = foc->firstChild(); c; c = c->nextSibling())
        if (FoController::FINAL != c->state())
            return false;
    return !next_child_fo(foc);
}

/*! Returns true when all children processed, or (for terminal fo)
    all content is processed, and the last processed area contain
    all necessary decorations.
 */
static bool is_chain_finished(const FoController* foc) 
{
    const AreaPtr& last = foc->lastArea();
    if (last.isNull() || last->chain() != foc->fo())
        return false;
    return ((foc->isFinalizing() || is_children_exhausted(foc)) && 
            foc->fo()->isChainFinishedAt(last));
}

//!
FoController::FoController(const AreaViewFactory* viewFactory,
                           const FoPtr& fo, const AreaPtr& lastArea,
                           uint& splitCount, FoControllerMap* foMap)
    : fo_(fo),
      viewFactory_(viewFactory),
      state_(CHAIN_CHECK),
      lastAreaMade_(0),
      splitCount_(splitCount),
      oldArea_(0),
      newArea_(0),
      lastArea_(lastArea),
      failCount_(0),
      isFinalizing_(false),
      foMap_(foMap)
{
    if (lastArea_) {
        RT_MSG_ASSERT((CTYPE_MAX != lastArea_->contRange().h_), "bad ch:");
        RT_MSG_ASSERT((0 != lastArea_->parent()), 
                      "! lastArea_ must have parent");
    }
    foMap_->insert(std::pair<const Fo*, const FoController*>(
                       fo_.pointer(), this));

    if (fo_->isParallel())
        ++splitCount_;
    //! If Fo was modified and registered in ModificationRegistry
    //Common::TreelocRep loc;
    //fo_->getTreeloc(loc);
    //std::cerr << "dereg:  " << loc.toString() << std::endl;

    fo_->deregisterModification();
    fo_->startFormatting();
    //! For newly born Fo`s: lazy construction of Fo subtree
    fo_->buildChildren();
}

FoController::~FoController()
{
    fo_->stopFormatting();
    if (fo_->isParallel())
        --splitCount_;

    FoControllerMap::iterator i = foMap_->find(fo_.pointer());
    RT_MSG_ASSERT(!(foMap_->end() == i), "Fo is already processing");
    foMap_->erase(i);
}

const char* STATE_NAMES[] = { "CHAIN_CHECK", "AREA_PREP", "CHILD_MAKE",
        "CHILD_BUILD", "AREA_CLOSE", "AREA_FAIL", "FINAL" 
        };
    
void FoController::dumpState(bool stateChanged) const
{
    RT_MSG_ASSERT((!fo_->firstChunk() || 
                  fo_->firstChunk()->Area::Chunk::parent()), 
                  "fo != fo_->firstChunk()->parent()");
    
    DBG_IF(XSL.FSM) {
        if (stateChanged)
            DBG(XSL.FSM) << "...state changed ";
        DBG(XSL.FSM) 
            << fo_->name() << ":" << fo_.pointer() << " state:"
            << STATE_NAMES[state_] << " lastAreaMade:" << lastAreaMade_ 
            << "\n     lastArea:" << lastArea_.pointer()
            << " oldArea:" << oldArea_.pointer()
            << " newArea:" << newArea_.pointer()
            << " splits:" << splitCount_ << std::endl;
    }
}

FoController* FoController::process()
{
    dumpState();
    switch (state_) {
        case FINAL :
            {
                RT_MSG_ASSERT(false, "Fo is already processed");
                break;
            }
        case AREA_FAIL :
            {
                RT_MSG_ASSERT(false, "Fo is failed");
                break;
            }
        case CHAIN_CHECK :
            {
                RT_MSG_ASSERT(newArea_.isNull(), "Area should be closed");
                if (is_chain_finished(this)) {
                    state_ = FINAL;
                    dumpState(true);
                    return parent();
                }
                else {
                    state_ = AREA_PREP;
                    dumpState(true);
                    return this;
                }
                break;
            }
        case AREA_PREP :
            {
                RT_MSG_ASSERT(newArea_.isNull(), "Area is already prepared");
                if (prepareArea()) {
                    state_ = CHILD_MAKE;
                    if (isFinalizing_)
                        state_ = AREA_CLOSE;
                    dumpState(true);
                    return this;
                }
                else {
                    state_ = AREA_FAIL;
                    dumpState(true);
                    return parent();
                }
                break;
            }
        case CHILD_MAKE :
            {
                if (is_children_exhausted(this)) {
                    state_ = AREA_CLOSE;
                    dumpState(true);
                    return this;
                }
                if (fo_->isParallel())  
                    makeParallelChild();
                else
                    makeChild();
                if (FINAL == state_)    
                    return parent();
                return this;
                break;
            }
        case CHILD_BUILD :
            {
                RT_MSG_ASSERT(firstChild(), "No child controller");

                if (fo_->isParallel())
                    DBG(XSL.FSM) << "ListItemFo choosing to build: ";
                bool failed_children = false;
                for (FoController* c = firstChild(); c; c = c->nextSibling()) {
                    if (fo_->isParallel())
                        DBG(XSL.FSM) << " " << c;
                    if (AREA_FAIL == c->state())
                        failed_children = true;
                    else
                        if (FINAL != c->state()) {
                            if (fo_->isParallel())
                                DBG(XSL.FSM) << " ok" << std::endl;
                            return c;
                        }
                }
                if (failed_children) {
                    if (fo_->isParallel())
                        DBG(XSL.FSM) << " some children failed" << std::endl;
                    if (!parent())
                        throw Formatter::Exception(
                            XslMessages::foControllerGen);
                    for (FoController* c = firstChild(); c;
                         c = c->nextSibling())
                        if (AREA_FAIL == c->state())
                            c->resetFailedState();
                    state_ = AREA_CLOSE;
                    dumpState(true);
                }
                else {
                    if (fo_->isParallel())
                        DBG(XSL.FSM) << " all children formatted" << std::endl;
                    state_ = CHILD_MAKE;
                    dumpState(true);
                }
                return this;
                break;
            }
        case AREA_CLOSE :
            {
                RT_MSG_ASSERT(!newArea_.isNull(), "No Area to close");

                //! NOTE: is content exhausted at newArea_
                bool is_content_exhausted = false;
                if (isFinalizing_)
                    is_content_exhausted = true;
                else
                    is_content_exhausted = (fo_->isTerminal()) 
                        ? fo_->isChainFinishedAt(newArea_) 
                        : is_children_exhausted(this);

                if (closeArea(is_content_exhausted)) {
                    lastAreaMade_ = lastChunk();
                    state_ = CHAIN_CHECK;
                    dumpState(true);
                    return this;
                }
                else {
                    state_ = AREA_FAIL;
                    dumpState(true);
                    return parent();
                }
                break;
            }
        default :
            RT_MSG_ASSERT(false, "Unknown state");
            break;
    }
    return 0;
}

void FoController::finalize()
{
    RT_MSG_ASSERT(!newArea_.isNull(), "No Area to finalize");
    isFinalizing_ = true;

    bool is_content_exhausted = true;    
    const Area* next_chunk = newArea_->nextChunk();
    while (next_chunk) {
        if (next_chunk->firstChild()) {
            is_content_exhausted = false;
            break;
        }
        next_chunk = next_chunk->nextChunk();
    }
    DBG(XSL.FSM) << "finalizing area, is_content_exhausted:" 
                 << is_content_exhausted << std::endl;
    dumpState();
    if (0 == newArea_->XListItem<Area>::parent()) {
        //! TODO: dump area tree
        throw Formatter::Exception(XslMessages::foControllerFin);
    }
    bool is_finalized = closeArea(is_content_exhausted);
    RT_ASSERT(is_finalized);
    if (!fo_->isChainFinishedAt(lastArea_))
        //! In this case
        state_ = CHAIN_CHECK;
    else
        state_ = FINAL;

    lastAreaMade_ = lastChunk();
    DBG(XSL.FSM) << "Finalized:\n";
    dumpState(true);
}

static bool is_subtree_modified(const Fo* fo)
{
    if (fo->isModified())
        return true;
    for (const Fo* c = fo->firstChild(); c; c = c->nextSibling()) 
        if (is_subtree_modified(c))
            return true;
    return false;
}

void FoController::makeChild()
{
    AreaPtr last_area;
    FoController* child = lastChild();
    Fo* next_fo = (child) ? child->fo()->nextSibling() : fo_->firstChild();

    DBG(XSL.FSM) << "Making child in " << fo_->name() << ": " 
                 << fo_.pointer() << " after: " 
                 << ((child) ? child->fo().pointer() :(Fo*)(0)) << std::endl;

    if (child) {
        if (FINAL != child->state()) { 
            //! Continue processing unfinished child
            state_ = CHILD_BUILD;
            dumpState(true);
            return;
        }
        //! Remove finished child
        uint splits_before = splitCount_;
        last_area = child->lastArea();
        bool is_child_parallel = child->fo()->isParallel();
        child->remove();
        if (is_child_parallel)
            DBG(XSL.FSM) << "Parallel child removed; splits before:"
                         << splits_before << " splits now:"
                         << splitCount_ << std::endl;
    }
    bool is_immediate_sibling = true;
    //! Immediate sibling(s) may be formatted (or successfully moved), so
    //  all following siblings must be checked for modifications
    while (next_fo) {
        if (0 != splitCount_)
            break;

        DBG(XSL.FSM) << "    ... checking child: " << next_fo << std::endl;

        bool is_being_formatted = is_being_processed(foMap_, next_fo);        
        bool is_formatted = (!is_being_formatted && next_fo->firstChunk() &&
                             newArea_ == next_fo->firstChunk()->parent() &&
                             newArea_ == next_fo->lastChunk()->parent() &&
                             !is_subtree_modified(next_fo) && 
                             !next_fo->isGeometryModified());
        if (is_formatted) {
            //! Check if all ancestors are formatted
            FoController* p = this;
            while (p) {
                if (0 == p->newArea()->XListItem<Area>::parent()) {
                    is_formatted = false;
                    break;
                }
                p = p->parent();
            }        
        }
        if (!is_formatted)
            break;
        DBG(XSL.FSM) << "        " << next_fo << " is formatted\n";
        last_area = next_fo->lastChunk();
        next_fo = next_fo->nextSibling();
        is_immediate_sibling = false;
    }
    //! If there are no unfinished children - stop processing
    if (!next_fo) {
        DBG(XSL.FSM) << "    ... No more modified children: finalizing\n";
        finalize();
        return;
    }

    DBG(XSL.FSM) << "Making FoController for child: " << next_fo << std::endl;

    //! If next_fo is not immediate sibling child, adjust Branch 
    if (!is_immediate_sibling && last_area->parent() != newArea_) {
        BranchDataList data_list;
        calc_branch_data(data_list, this, last_area->parent());
        for (BranchData* c = data_list.lastChild(); c; c = c->prevSibling()) 
            c->foc_->prepareArea(c->area_, c->newAreaSpace_, c->newAllcPoint_);
    }

    appendChild(new FoController(viewFactory_, next_fo, 
                                 last_area, splitCount_, foMap_));
    state_ = CHILD_BUILD;
    dumpState(true);
}

void FoController::makeParallelChild()
{
    FoController* child = lastChild();
    Fo* next_fo = (child) ? child->fo()->nextSibling() : fo_->firstChild();

    DBG(XSL.FSM) << "Parallel fo making children: ";
    while (next_fo) {
        DBG(XSL.FSM) << " " << next_fo;
        appendChild(new FoController(
                        viewFactory_, next_fo, 0, splitCount_, foMap_));
        next_fo = next_fo->nextSibling();
    }
    Fo* prev_fo = firstChild()->fo()->prevSibling();
    while (prev_fo) {
        DBG(XSL.FSM) << " " << prev_fo;
        firstChild()->insertBefore(
            new FoController(viewFactory_, prev_fo, 0, splitCount_, foMap_));
        prev_fo =  prev_fo->prevSibling();
    }
    DBG(XSL.FSM) << std::endl;
    state_ = CHILD_BUILD;
    dumpState(true);
}

}
