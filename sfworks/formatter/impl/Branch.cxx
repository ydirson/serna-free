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

#include <list>
#include "common/XTreeNode.h"
#include "formatter/impl/debug.h"
#include "formatter/impl/Fo.h"
#include "formatter/impl/Branch.h"
#include "formatter/impl/formatter_utils.h"

namespace Formatter {

USING_COMMON_NS;

struct BranchItemInit {
    BranchItemInit(Fo* fo, Area* area) : fo_(fo), area_(area) {};
    Fo*     fo_;
    Area*   area_;
};

Branch::Branch(const FoPtr& branchFo, FormattedView* view,
               const AreaViewFactory* viewFactory, FoControllerMap* foMap,  
               bool isPaginated)
    : view_(view),
      current_(0),
      lastAreaMade_(0),
      splitCount_(0),
      isPaginated_(isPaginated)
{
    //! Trace areas & fo`s to the root to initialize FoControllers
    std::list<BranchItemInit> init_list;
    //! BranchFo (the terminal one) will be reformatted from begining

    DBG(XSL.BRANCH) 
        << "Creating Branch for Fo:" << branchFo.pointer() << std::endl;
    Area* area = branchFo->firstChunk();

    for (Fo* fo = branchFo.pointer(); fo; fo = fo->parent()) {
        //! Reformat the whole parallel fo
        if (fo->isParallel()) {
            init_list.clear();
            area = fo->firstChunk();
        }
        RT_MSG_ASSERT((!area || area->Area::Chunk::parent()), 
                      "Branch Error: Attempt to format atandalone area!");
        
        Area* last_area = area ? area->prevChunk() : 0; 
        DBG(XSL.BRANCH)
            << "Fo<" << fo->name() << ">:" << fo
            << " area:" << area << " last_area:" << last_area << std::endl;
        if (0 == last_area) {
            DBG(XSL.BRANCH) << "  0 == last_area" << std::endl;
            Fo* prev = fo->prevSibling();
            if (prev) {
                RT_MSG_ASSERT(fo->prevSibling()->lastChunk(),
                              "Branch creation error: previous"
                              "sibling Fo is not yet formatted");
                last_area = prev->lastChunk();
                DBG(XSL.BRANCH) << "    prev != 0" << " last_area:"
                                << last_area<< std::endl;
            }
            if (0 == area || (fo == branchFo &&
                              PREV_FO_MODIFIED == fo->modificationType())) {
                area = last_area;
                DBG(XSL.BRANCH)
                    << "    PREV_FO_MODIFIED" << " area:" << area<< std::endl;
            }
        }
        init_list.push_front(BranchItemInit(fo, last_area));
        if (area)
            area = area->parent();
        else
            if (fo->parent())
                area = fo->parent()->firstChunk();
    }
    //! Construct branch items
    for (std::list<BranchItemInit>::const_iterator i = init_list.begin();
         i != init_list.end(); i++) {
        Area* last_area = (*i).area_;
        DBG(XSL.BRANCH)
            << "Branch item fo:" << (*i).fo_ << "<" << (*i).fo_->name()
            << "> last_area:" << last_area << " area:"
            << (*i).area_ << std::endl;
        FoController* fo_controller = new FoController(
            viewFactory, (*i).fo_, last_area, splitCount_, foMap);
        if (0 == current_)
            root_ = fo_controller;
        else
            current_->appendChild(fo_controller);
        current_ = fo_controller;
    }
    current_ = root_.pointer();
}

BranchState Branch::process(bool isToPostpone)
{
    FoController::State last_state = current_->state();
    while (current_) {
        DBG(XSL.BRANCH) << "Branch: processing fo:" 
                        << current_->fo().pointer() << std::endl;
        FoController* nxt = current_->process();
        {
            Area* area = current_->fo()->firstChunk();
            RT_MSG_ASSERT((!area || area->Area::Chunk::parent()), 
                          "Error: branch Area has no paret area");
        }

        //! Delete next branch in case of equality
        if (0 == splitCount_ && current_ != nxt) {
            while (nextSibling() && *this == *nextSibling()) {
                DBG(XSL.FORMATTER) << " ... next branch removed" << std::endl;
                nextSibling()->remove();
            }
        }

        //! Stop if branch formatting is finished
        if (0 == nxt) {
            RT_MSG_ASSERT((FoController::FINAL == current_->state()), 
                          "Area tree formatting failed");
            DBG(XSL.BRANCH) << "Branch: " << this << " formatting finished\n";
            root_ = 0;
            current_ = 0;
            return FINISHED;
        }
        current_ = nxt;

#if 1 //! Turn this condition to '0' to suppress multi-branch optimization
        if (FoController::CHAIN_CHECK == current_->state() &&
            FoController::AREA_CLOSE == last_state && 0 == splitCount_ &&
            current_->lastAreaMade()) {
            
            lastAreaMade_ = current_->lastAreaMade();
            const bool is_to_check = (!isPaginated_) 
                ? (BLOCK_AREA == lastAreaMade_->type() || 
                   PAGE_AREA == lastAreaMade_->type())
                : PAGE_AREA == lastAreaMade_->type();
            bool area_is_inserted = true;
            for (const Area* a = lastAreaMade_; a; a = a->parent()) {
                if (a->chain()->parentChain() && !a->parent()) {
                    area_is_inserted = false; 
                    break;
                }
            }
            if (isToPostpone && is_to_check && area_is_inserted &&
                view_->isToPostpone(lastAreaMade_)) {
                CType bottom =  
                    lastAreaMade_->absAllcPoint().y_ + lastAreaMade_->allcH();
                DBG(XSL.BRANCH) << "Branch: " << this << " postponed bottom:" 
                                << bottom << " page:" 
                                << lastAreaMade_->siblingIndex() << std::endl;
                if (!isPaginated_)
                    view_->updateSize(lastAreaMade_);
                return POSTPONED;
            }
        }
#endif
        last_state = current_->state();
    }
    return FINISHED;
}

FoController* Branch::lastChild() const
{
    FoController* last = root_.pointer();
    if (0 == last)
        return 0;
    for (;;) {
        if (last->firstChild()) {
            RT_MSG_ASSERT((last->firstChild() == last->lastChild()),
                          "Attempt to find last FoController "
                          "in splitted branch");
            last = last->firstChild();
        }
        else
            return last;
    }
    return 0;
}

bool Branch::willProcess(const FoPtr& fo) const
{
    return (LESS == lastChild()->fo()->comparePos(fo.pointer()));
}

bool Branch::willProcess(const GroveLib::Node* foNode) const
{
    return is_fo_node_less(lastChild()->fo()->headFoNode(), foNode);
    //return (LESS == lastChild()->fo()->headFoNode()->comparePos(foNode));
}

bool Branch::isProcesses(const GroveLib::Node* foNode) const
{
    const GroveLib::Node* node = lastChild()->fo()->headFoNode();
    XTreeNodeCmpResult cmp = node->comparePos(foNode);
    if (LESS == cmp)
        return true;
    while (node) {
        if (foNode == node)
            return true;
        node = node->parent();
    }
    return false;
    //return is_fo_node_less(lastChild()->fo()->headFoNode(), foNode);
    //return (LESS == lastChild()->fo()->headFoNode()->comparePos(foNode));
}

static bool proc(const FoController* fc, const Fo* fo, const FoController* l)
{
    if (fc->fo() == fo && fc != l)
        return true;
    for (fc = fc->firstChild(); fc; fc = fc->nextSibling())
        if (proc(fc, fo, l))
            return true;
    return false;
}

bool Branch::isProcesses(Fo* fo, const FoController* preserveBranch) const
{
    return proc(root_.pointer(), fo, preserveBranch);
}

bool Branch::operator==(const Branch& branch) const
{
    const FoController* this_item = lastChild();
    const FoController* other_item = branch.lastChild();
    if (this_item->fo().pointer() != other_item->fo().pointer())
        return false;
    return true;

    while (this_item && other_item) {
        if (this_item->state() != other_item->state())
            return false;
        this_item = this_item->parent();
        other_item = other_item->parent();
    }
    return true;
}

} // namespace Formatter
