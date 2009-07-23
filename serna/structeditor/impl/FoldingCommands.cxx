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
#include "editableview/EditableView.h"
#include "structeditor/StructEditor.h"
#include "structeditor/SE_Pos.h"
#include "structeditor/LiveNodeLocator.h"
#include "structeditor/impl/debug_se.h"

#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/GrovePosEventData.h"

#include "ui/UiItemSearch.h"

#include "docview/SernaDoc.h"
#include "common/DiffuseSearch.h"
#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"

#include "xslt/Result.h"
#include "xslt/Engine.h"
#include "formatter/Area.h"
#include "formatter/XslFoExt.h"

#include "docview/EventTranslator.h"

#include "grove/udata.h"
#include "grove/ElementMatcher.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommand.h"

#include "formatter/AreaPos.h"
#include "utils/SernaUiItems.h"
#include "genui/StructDocumentActions.hpp"

using namespace Common;
using namespace Formatter;
using namespace GroveLib;
using namespace GroveEditor;
using namespace DocSrcInfo;

//////////////////////////////////////////////////////////////////////////

class FoldableIterator {
public:
    FoldableIterator(StructEditor* se);

    bool                    advance();
    const GroveLib::Node*   origin() const;
    const Xslt::XsltResult* result() const;

private:
    GroveLib::Node* foNode_;
    GroveLib::Node* nextFoNode_;
    const GroveLib::ElementMatcher& matcher_;
};

class FoldCursorGuard {
public:
    FoldCursorGuard(StructEditor* se, const GrovePos& srcPos,
                    const GrovePos& fallbackPos = GrovePos())
        : se_(se),
          srcPos_(srcPos),
          fallbackPos_(fallbackPos),
          foHint_(se_->editViewFoPos())
    {
    }

    ~FoldCursorGuard()
    {
        GrovePos result_pos = se_->toResultPos(srcPos_, foHint_.node());
        if (!result_pos.isNull()) {
            AreaPos area_pos = 
                se_->toAreaPos(result_pos, StructEditor::ADJUST_FORWARD);
            if (se_->setCursor(srcPos_, area_pos, true))
                return;
        }
        if (fallbackPos_.isNull())
            return;
        result_pos = se_->toResultPos(fallbackPos_, foHint_.node());
        if (!result_pos.isNull())
            se_->setCursor(fallbackPos_, se_->toAreaPos(result_pos), true);
    }
private:
    StructEditor*   se_;
    GrovePos        srcPos_;
    GrovePos        fallbackPos_;
    LiveNodeLocator foHint_;
};

static bool is_fo_node_folded(const GroveLib::Node* n)
{
    if (n->nodeType() != Node::ELEMENT_NODE)
        return false;
    return !!static_cast<const Element*>(n)->attrs().
        getAttribute(NOTR("se:fold"));
}

//////////////////////////////////////////////////////////////////////////

GroveLib::ElementMatcher& StructEditor::foldListMatcher()
{
    if (foldListMatcher_.isNull()) {
        const PropertyNode* fold_rules =
            getDsi()->getProperty(DocSrcInfo::FOLDING_RULES);
        String pattern;
        if (fold_rules) {
            pattern = fold_rules->getSafeProperty(FOLDING_LIST)->getString();
            if (pattern.isNull())
                pattern = fold_rules->getSafeProperty(FOLD_IF_LARGE)->
                    getString();
        }
        if (pattern.isNull())
            pattern = NOTR("*");
        foldListMatcher_ = new GroveLib::ElementMatcher(NOTR("*"));
    }
    return *foldListMatcher_;
}

////////////////////////////////////////////////////////////////////////////

static bool do_fold(const Xslt::XsltResult* result,
                    StructEditor* se, bool doFold)
{
    if (0 == result)
        return false;
    GroveLib::Node* origin =
        const_cast<GroveLib::Node*>(Xslt::resultOrigin
            (const_cast<Xslt::XsltResult*>(result)->node()));
    if (0 == origin)
        return false;
    DDBG << "DoFold: origin = " << origin->nodeName()
        << " (" << origin << ") doFold=" << doFold << std::endl;
    DDBG << "DoFold: close_origin = " << result->origin()->nodeName()
        << " (" << result->origin() << ")\n";
    //origin->dump();
    //DDBG << "------------\n";
    //result->origin()->dump();
    GrovePos cpos(origin, origin->firstChild());
    FoldCursorGuard cursor_guard(se, cpos, se->editViewSrcPos());
    set_node_fold_state(origin, doFold);
    result->notifyTemplateStateChange();
    se->update();
    return true;
}

static GroveLib::Node* find_folded_aos(const GroveLib::Node* foNode)
{
    GroveLib::Node* foundNode = 0;
    for (const Node* node = foNode; node; node = node->parent()) {
        if (Xslt::resultOrigin(node) && is_fo_node_folded(node))
            foundNode = const_cast<Node*>(node);
    }
    return foundNode;
}

////////////////////////////////////////////////////////////////////////////

FoldableIterator::FoldableIterator(StructEditor* se)
    : foNode_(0),
      matcher_(se->foldListMatcher())
{
    nextFoNode_ = se->editViewFoPos().node();
    GroveLib::Node* foundNode = find_folded_aos(foNode_);
    if (foundNode)
        nextFoNode_ = foundNode;
}

bool FoldableIterator::advance()
{
    foNode_ = nextFoNode_;
    if (0 == foNode_)
        return false;
    for (; foNode_ && origin(); foNode_ = parentNode(foNode_)) {
        if (origin()->nodeType() != GroveLib::Node::ELEMENT_NODE)
            continue;
        if (!matcher_.matchElement(static_cast<const Element*>(origin())))
            continue;
        nextFoNode_ = foNode_;
        const Node* prevOrigin = origin();
        for (; nextFoNode_; nextFoNode_ = parentNode(nextFoNode_)) {
            const GroveLib::Node* resOrigin = Xslt::resultOrigin(nextFoNode_);
            if (!resOrigin || resOrigin != prevOrigin)
                break;
        }
        const GroveLib::Node* resOrigin = origin();
        if (resOrigin->nodeType() == GroveLib::Node::TEXT_NODE)
            resOrigin = parentNode(resOrigin);
        return resOrigin->parent() && resOrigin->parent()->parent();
    }
    return false;
}

const GroveLib::Node* FoldableIterator::origin() const
{
    return Xslt::resultOrigin(foNode_);
}

const Xslt::XsltResult* FoldableIterator::result() const
{
    return foNode_->asConstXsltResult();
}

////////////////////////////////////////////////////////////////////////

typedef std::list<const Xslt::XsltResult*>  ResultList;
typedef ResultList::const_iterator          ResultIterator;

static void find_folded_results(Node* foNode, bool subtree,
                                ResultList& results)
{
    const Xslt::XsltResult* result = foNode->asConstXsltResult();
    if (result) {
        Node* origin = const_cast<Node*>(Xslt::resultOrigin(foNode));
        if (origin && is_fo_node_folded(foNode)) {
            results.push_back(result);
            if (!subtree)
                return;
        }
    }
    for (Node* c = foNode->firstChild(); c; c = c->nextSibling())
        find_folded_results(c, subtree, results);
}

static void reset_fold_state(Node* n)
{
    set_node_fold_state(n, false);
    for (n = n->firstChild(); n; n = n->nextSibling())
       reset_fold_state(n);
}

static void unfold_subtree(StructEditor* se, Node* foNode, bool recursively)
{
    if (foNode) {
        FoldCursorGuard cursor_guard(se, se->editViewSrcPos());
        ResultList res_list;
        find_folded_results(foNode, recursively, res_list);
        for (ResultIterator i = res_list.begin(); i != res_list.end(); i++) {
            if (recursively)
                reset_fold_state(const_cast<Node*>((*i)->origin()));
            else
                set_node_fold_state(const_cast<Node*>((*i)->origin()), false);
            (*i)->notifyTemplateStateChange();
        }
        se->update();
    }
}

////////////////////////////////////////////////////////////////////////

#if 0

static void dump_pos(const char* str, const GrovePos& pos)
{
    pos.dump();
    std::cerr << NOTR("pos ") << str << NOTR(": ");
    const XsltResult* res = dynamic_cast<const XsltResult*>(pos.node());
    std::cerr << res->origin()->nodeName() << " ";
    if (pos.before()) {
        res = dynamic_cast<const XsltResult*>(pos.before());
        std::cerr << NOTR(" before: ") << res->origin()->nodeName();
    }
    std::cerr << std::endl;
}

#endif

SIMPLE_COMMAND_EVENT_IMPL(FoldElement, StructEditor)

bool FoldElement::doExecute(StructEditor* se, EventData*)
{
    const TreeSelection& tsel = se->editableView().getSelection().tree_;
    if (tsel.isEmpty()) {
        FoldableIterator fold_iter(se);
        return fold_iter.advance() && do_fold(fold_iter.result(), se, true);
    }
    GroveLib::Node *n1 = 0, *n2 = 0, *temp;
    GroveEditor::GrovePos from_sel = EditContext::getFoPos(
        tsel.minLoc().toAreaPos(se->editableView().rootArea()));
    GroveEditor::GrovePos to_sel = EditContext::getFoPos(
        tsel.maxLoc().toAreaPos(se->editableView().rootArea()));
    if (from_sel.isNull() || to_sel.isNull())
        return false;
    if (from_sel.type() == GrovePos::TEXT_POS)
        from_sel = GrovePos(from_sel.node()->parent(), from_sel.node());
    if (to_sel.type() == GrovePos::TEXT_POS)
        to_sel = GrovePos(to_sel.node()->parent(), to_sel.node());
    n1 = from_sel.adjustBoundaryPos().contextNode();
    to_sel = to_sel.adjustBoundaryPos();
    //dump_pos("from: ", from_sel);

    if (!to_sel.node()->firstChild())
        to_sel = GrovePos(to_sel.node()->parent(), to_sel.node());
    if (to_sel.before())
        to_sel = GrovePos(to_sel.node(), to_sel.before()->prevSibling());
    if (!to_sel.before())
        to_sel = GrovePos(to_sel.node(), to_sel.node()->lastChild());
    n2 = to_sel.contextNode();
    //dump_pos("to: ", to_sel);
    if (n1->nodeType() == GroveLib::Node::TEXT_NODE)
        n1 = n1->parent();
    if (n2->nodeType() == GroveLib::Node::TEXT_NODE)
        n2 = n2->parent();

    switch (n1->commonChildren(n2, n1, n2)) {
        case NONCOMP:
            return false;
        case GREATER:   // paranoia - shouldn't be with minLoc/maxLoc
            temp = n1; n1 = n2; n2 = temp;
            break;
        default:
            break;
    }
    RT_ASSERT(n1->parent() == n2->parent());
    RT_ASSERT(n1->siblingIndex() <= n2->siblingIndex());
    GrovePos cursor_pos;
    const ElementMatcher& matcher = se->foldListMatcher();
    for (;;) {
        const Xslt::XsltResult* result = n1->asConstXsltResult();
        if (0 == result)
            return false;
        GroveLib::Node* origin =
            const_cast<GroveLib::Node*>(Xslt::resultOrigin(n1));
        if (origin && origin->nodeType() == GroveLib::Node::ELEMENT_NODE
            && matcher.matchElement(static_cast<const Element*>(origin))) {
                set_node_fold_state(origin, true);
                //std::cerr << "SET FOLD: " << origin->nodeName() << std::endl;
                result->notifyTemplateStateChange();
                if (cursor_pos.isNull())
                    cursor_pos = GrovePos(origin->parent(), origin);
        }
        if (n1 == n2)
            break;
        n1 = n1->nextSibling();
    }
    FoldCursorGuard cursor_guard(se, se->editViewSrcPos(), cursor_pos);
    se->removeSelection();
    se->update();
    return true;
}

/////////////

class UnfoldSelectionOp : public EditableView::SelectionOp {
public:
    UnfoldSelectionOp(StructEditor* se)
        : se_(se) {}
    virtual void operator()(const Area* area, long, long)
    {
        if (0 == area->chain())
            return;
        GroveLib::Node* foNode =
            const_cast<GroveLib::Node*>(area->chain()->headFoNode());
        if (!is_fo_node_folded(foNode))
            return;
        const GroveLib::Node* resOrigin = Xslt::resultOrigin(foNode);
        const Xslt::XsltResult* res = foNode->asConstXsltResult();
        if (0 == resOrigin)
            return;
        set_node_fold_state(const_cast<Node*>(resOrigin), false);
        res->notifyTemplateStateChange();
    }
private:
    StructEditor* se_;
};

static bool is_descendant(const GroveLib::Node* refNode,
                          const GroveLib::Node* n)
{
    if (0 == n)
        return false;
    for (n = parentNode(n); n; n = parentNode(n))
        if (n == refNode)
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////

class FoldedFoCheck {
public:
    FoldedFoCheck(const Node* srcNode)
        : srcNode_(srcNode) {}
    bool        operator()(const XTreeIterator<const Node>& foIter) const {
        return (srcNode_ == Xslt::resultOrigin(foIter.node()) && 
                is_fo_node_folded(foIter.node()));
    }
private:
    const Node* srcNode_;
};

class OriginCheck {
public:
    OriginCheck(const Node* srcNode)
        : srcNode_(srcNode) {}
    bool        operator()(const XTreeIterator<const Node>& foIter) const {
        return srcNode_ == Xslt::resultOrigin(foIter.node());
    }
private:
    const Node* srcNode_;
};

static const Node* find_result_node(const Node* node, const Node* before, 
                                    const Node* srcNode)
{
    XTreeIterator<const Node> iter(node, before);
    XTreeIterator<const Node> result_iter =
        find_forward<const Node, OriginCheck>(iter, OriginCheck(srcNode));
    return result_iter.node();
}

static const Node* find_unfolded_result(const Node* srcNode, StructEditor* se)
{
    if (!srcNode)
        return 0;
    const Node* root = se->fot()->document()->documentElement();
    XTreeIterator<const Node> iter(root, root->firstChild());
    XTreeIterator<const Node> result_iter =
        find_forward<const Node, FoldedFoCheck>(iter, FoldedFoCheck(srcNode));
    const Node* result = result_iter.node();
    
    if (!result) {
        const Node* parent = 
            find_unfolded_result(srcNode->parent(), se);
        if (!parent || srcNode->parent() != Xslt::resultOrigin(parent))
            //! Return nearest unfolded ancestor
            return parent;        
        result = find_result_node(parent, parent->firstChild(), srcNode);
        if (!result)
            return parent;
    }
    if (is_fo_node_folded(result)) {
        const Node* parent = result->parent();
        const Node* before = result->prevSibling();
        do_fold(result->asConstXsltResult(), se, false);
        if (before)
            before = before->nextSibling();
        result = find_result_node(parent, before, srcNode);
    }
    return result;
}

class SetSrcPos;

SIMPLE_COMMAND_EVENT_IMPL(UnfoldElement, StructEditor)

bool UnfoldElement::doExecute(StructEditor* se, EventData*)
{
    if (se->editableView().getSelection().src_.isEmpty()) {
        GrovePos src_pos = se->editableView().context().srcPos();
        const Node* result = find_unfolded_result(src_pos.node(), se);
        if (result) {
            GrovePos result_pos = to_result_pos(
                src_pos, GrovePos(result, result->firstChild()), false);
            if (result_pos.isNull())
                result_pos = GrovePos(result, result->firstChild());
            se->setCursor(src_pos, se->toAreaPos(result_pos), true);
            return true;
        }
        return false;
    }
    UnfoldSelectionOp unfold_selection(se);
    FoldCursorGuard cursor_guard(se, se->editViewSrcPos());
    se->editableView().inSelectionDo(se->editableView().getSelection().tree_,
                                     unfold_selection);
    se->removeSelection();
    se->update();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(UnfoldDescendants, StructEditor)

bool UnfoldDescendants::doExecute(StructEditor* se, EventData*)
{
    GroveLib::Node* folded = find_folded_aos(se->editViewFoPos().node());
    if (0 == folded) {
        const ElementMatcher& em = se->foldListMatcher();
        const GroveLib::Node* n = se->editViewSrcPos().node();
        for (; n; n = parentNode(n)) {
            if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
                continue;
            if (em.matchElement(static_cast<const GroveLib::Element*>(n)))
                break;
        }
        if (0 == n)
            return false;
        GrovePos fo_pos =
            se->toResultPos(GrovePos(n), se->editViewFoPos().node());
        if (fo_pos.isNull())
            return false;
        unfold_subtree(se, fo_pos.node(), true);
        return true;
    }
    const GroveLib::Node* resOrigin = Xslt::resultOrigin(folded);
    GroveLib::Node* n = se->lastSetPosNode();
    if (!is_descendant(resOrigin, n)) {
        unfold_subtree(se, folded, true);
        return true;
    }
    for (; n != resOrigin; n = parentNode(n))
        set_node_fold_state(n, false);
    reset_fold_state(se->lastSetPosNode());
    do_fold(folded->asConstXsltResult(), se, false);
    GrovePosEventData ed(se->lastSetPosNode());
    return makeCommand<SetSrcPos>(&ed)->execute(se);
}

SIMPLE_COMMAND_EVENT_IMPL(UnfoldAll, StructEditor)

bool UnfoldAll::doExecute(StructEditor* se, EventData*)
{
    unfold_subtree(se, se->editViewFoPos().node()->root(), true);
    return true;
}

///////////////////////////////////////////////////////////////////////////

UICMD_EVENT_IMPL(FoldAncestor, StructEditor)

bool FoldAncestor::doExecute(StructEditor* se, EventData*)
{
    if (!activeSubAction())
        return false;
    int depth = activeSubAction()->getInt(Sui::NAME);
    FoldableIterator fold_iter(se);
    while (fold_iter.advance() && depth-- > 0);
    if (0 >= depth && fold_iter.result())
        return do_fold(fold_iter.result(), se, true);
    return false;
}

SIMPLE_COMMAND_EVENT_IMPL(UpdateFoldMenu, StructEditor)

bool UpdateFoldMenu::doExecute(StructEditor* se, EventData*)
{
    Sui::Action* menu_action = se->uiActions().collapseAncestor();
    menu_action->removeAllChildren();
    FoldableIterator fold_iter(se);
    for (int depth = 0; fold_iter.advance(); ++depth) {
        PropertyNodePtr prop(new PropertyNode(Sui::ACTION));
        prop->makeDescendant(Sui::INSCRIPTION, 
            '<' + fold_iter.origin()->nodeName() + '>', true);
        prop->makeDescendant(Sui::NAME)->setInt(depth);
        Sui::Action* sub_action = Sui::Action::make(prop.pointer());
        sub_action->setEnabled(true);
        menu_action->appendChild(sub_action);
    }
    se->uiActions().collapseAncestorMenuCmd()->setEnabled(
        !!menu_action->firstChild());
    return true;
}
