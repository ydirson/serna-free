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
#include "structeditor/se_defs.h"
#include "structeditor/StructEditor.h"
#include "structeditor/SE_Pos.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/impl/PositionWatcher.h"
#include "structeditor/impl/EditPolicyImpl.h"
#include "structeditor/SetCursorEventData.h"

#include "docview/SernaDoc.h"
#include "docview/GoToOriginEventData.h"

#include "common/PropertyTree.h"
#include "common/XTreeIterator.h"
#include "common/DiffuseSearch.h"
#include "common/Message.h"
#include "common/timing_profiler.h"

#include "formatter/AreaPos.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"
#include "xslt/ResultOrigin.h"

#include "grove/Grove.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/NodeExt.h"
#include "grove/SectionSyncher.h"
#include "grove/udata.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "editableview/EditableView.h"

#include "utils/SernaMessages.h"
#include "utils/GrovePosEventData.h"
#include "utils/DocSrcInfo.h"

#include "genui/StructDocumentActions.hpp"


using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace Formatter;

/////////////////////////////////////////////////////////////////////////

class XslOriginCheck {
public:
    XslOriginCheck(const GrovePos& src)
        : node_(src.node()), 
          before_(0),
          after_(0)
        {
            if (!src.isNull()) {
                if (GrovePos::ELEMENT_POS == src.type())
                    before_ = src.before();
                after_ = (before_) 
                    ? before_->prevSibling() : node_->lastChild();
            }
#if 0
            std::cerr << NOTR("node: ") << node_ << std::endl;
            if (node_)
                node_->dump();
            std::cerr << NOTR("before: ") << before_ << std::endl;
            if (before_)
                before_->dump();
            std::cerr << NOTR("after: ") << after_ << std::endl;
            if (after_)
                after_->dump();
#endif
        };
    bool        operator()(const XTreeIterator<Node>& foIter) const;
    
private:
    const Node* node_;
    const Node* before_;
    const Node* after_;
};

bool XslOriginCheck::operator()(const XTreeIterator<Node>& foIter) const
{
    if (!node_)
        return false;

    const Node* i_after = (foIter.before()) 
        ? foIter.before()->prevSibling() : foIter.node()->lastChild();
    
#if 0
    std::cerr << NOTR("i_node:   ") << foIter.node() << NOTR(" (")
              << ((foIter.node()) ? Xslt::resultOrigin(foIter.node()) : 0)
              << NOTR(") i_before: ") << foIter.before() << " ("
              << ((foIter.before()) ? Xslt::resultOrigin(foIter.before()) : 0)
              << NOTR(") i_after: ") <<  i_after << " (" 
              << Xslt::resultOrigin(i_after) << ")" << std::endl;
#endif

    const Node* node_origin = Xslt::resultOrigin(foIter.node());
    if (node_origin != node_)
        return false;
    const Node* before_origin = Xslt::resultOrigin(foIter.before());
    const Node* after_origin = Xslt::resultOrigin(i_after);
    
    if (i_after && foIter.before() && before_origin == after_origin)
        return false;
    
    if (before_ && before_origin == before_)
        return true;
    if (after_ && after_origin == after_) 
        return true;
    
    if (!before_ && !after_ && !foIter.before() && node_origin == node_)
        return true;

    return false;
}

XTreeIterator<Node> xtree_iterator(const GrovePos& pos) 
{
    return XTreeIterator<Node>(
        pos.node(), (GrovePos::ELEMENT_POS == pos.type()) ? pos.before() : 0);
}

GrovePos to_result_pos(const GrovePos& srcPos, const GrovePos& foHint,
                       bool diffuse, bool isForward)
{
    return to_result_pos(srcPos, xtree_iterator(foHint), diffuse, isForward);
}

GrovePos to_result_pos(const GrovePos& srcPos, 
                       const XTreeIterator<Node>& start,
                       bool diffuse, bool isForward)
{
    XTreeIterator<Node> fo_iter = (diffuse)
        ? find_diffuse<Node, XslOriginCheck>(start, XslOriginCheck(srcPos))
        : find_forward<Node, XslOriginCheck>(start, XslOriginCheck(srcPos), 
                                             isForward);
    if (GrovePos::TEXT_POS == srcPos.type() && fo_iter.node() 
        && Node::TEXT_NODE == fo_iter.node()->nodeType())
        return GrovePos(fo_iter.node(), srcPos.idx());
    return GrovePos(fo_iter.node(), fo_iter.before());
}

///////////////////////////////////////////////////////////////////////////

const GrovePos NULL_GROVE_POS = GrovePos();

const GrovePos& StructEditor::editViewSrcPos() const
{
    if (editableView_)
        return editableView().context().srcPos();
    else
        return NULL_GROVE_POS;
}

const GrovePos& StructEditor::editViewFoPos() const
{
    if (editableView_)
        return editableView().context().foPos();
    else
        return NULL_GROVE_POS;
}

const Node* find_result(const Node* src, const Node* result, bool diffuse)
{
    using namespace Formatter;

    const Node* fo_node = (diffuse)
        ? find_diffuse<Node, XslOriginTest>(result, XslOriginTest(src))
        : find_forward<Node, XslOriginTest>(result, XslOriginTest(src));

    while (fo_node && fo_node->parent() &&
           XslFoExt::areaChain(fo_node) &&
           XslFoExt::areaChain(fo_node)->hasParentOrigin())
        fo_node = fo_node->parent();
    return fo_node;
}

GrovePos StructEditor::toResultPos(const GrovePos& srcPos, const Node* foHint,
                                   bool diffuse) const
{
    DBG(SE.TEST) << "ToResultPos:\n";
    if (srcPos.isNull())
        return 0;
    RT_MSG_ASSERT((srcPos.node()->root() !=
                   fot_->document()->documentElement()),
                  NOTR("Attempt to convert result pos into result pos"));

    if (0 == foHint)
        foHint = fot_->document()->documentElement();

    if (GrovePos::TEXT_POS == srcPos.type()) {
        const Node* fo_node = find_result(srcPos.node(), foHint, diffuse);
        if (fo_node)
            return GrovePos(fo_node, srcPos.idx());
    }
    else
        if (GrovePos::ELEMENT_POS == srcPos.type()) {
            const Node* before = srcPos.before();
            if (before) {
                const Node* fo_node = find_result(before, foHint, diffuse);
                if (fo_node)
                    return GrovePos(fo_node->parent(), fo_node);
                if (before->prevSibling()) {
                    fo_node =
                        find_result(before->prevSibling(), foHint, diffuse);
                    if (fo_node)
                        return GrovePos(fo_node->parent(),
                                        fo_node->nextSibling());
                }
                fo_node = find_result(srcPos.node(), foHint, diffuse);
                if (fo_node)
                    return GrovePos(fo_node);
            }
            if (srcPos.node()->lastChild()) {
                const Node* fo_node =
                    find_result(srcPos.node()->lastChild(), foHint, diffuse);
                if (fo_node)
                    return GrovePos(fo_node->parent(),
                                    fo_node->nextSibling());
            }
            const Node* fo_node = find_result(srcPos.node(), foHint, diffuse);
            if (fo_node)
                return GrovePos(fo_node);
        }
    return GrovePos();
}

static const AreaPos get_valid(const AreaPos& areaPos,
                               StructEditor::CursorAdjustment adj)
{
    if (StructEditor::DO_NOT_ADJUST == adj)
        return areaPos;
    AreaPos result(areaPos);
    if (StructEditor::ADJUST_FORWARD == adj)
        result.findAllowed(true, true);
    else
        if (StructEditor::ADJUST_BACKWARD == adj)
            result.findAllowed(false, true);
        else
            result.validate();
    return result;
}

Chain* get_formatted_chain(const Node* foNode, EditableView& view)
{
    if (!foNode)
        return 0;
    view.ensureFormatted(foNode);
    Chain* chain = XslFoExt::areaChain(foNode);
    if (chain && chain->firstChunk()) 
        return chain;
    DBG_IF(SE.TEST) {
        if (!chain) 
            Node::dumpSubtree(foNode);
    }
    return 0;
}

static AreaPos to_area_pos(const GrovePos& resultPos,
                           StructEditor::CursorAdjustment adj,
                           EditableView& view)
{
    DBG(SE.TEST) << "ToAreaPos:\n";
    if (resultPos.isNull())
        return AreaPos();

    if (GrovePos::TEXT_POS == resultPos.type()) {
        TextFo* chain = SAFE_CAST(
            TextFo*, get_formatted_chain(resultPos.node(), view));
        if (!chain)
            return AreaPos();
        ulong pos_count = chain->chainPosCount();
        ulong idx = chain->convertPos(resultPos.idx(), true);
        ChainPos chain_pos(chain, (pos_count - 1 < idx) ? pos_count - 1 : idx);
        return get_valid(chain_pos.toAreaPos(), adj);
    }
    else
        if (GrovePos::ELEMENT_POS == resultPos.type()) {
            const Node* before = resultPos.before();
            const Node* after = (before) ?
                before->prevSibling() : resultPos.node()->lastChild();

            while (before) {
                Chain* child_chain = get_formatted_chain(before, view);
                if (child_chain) {
                    const Area* child = child_chain->firstChunk();
                    return get_valid(child->parent()->areaPos(child_chain),
                                     adj);
                }
                before = before->nextSibling();
            }
            while (after) {
                Chain* child_chain = get_formatted_chain(after, view);
                if (child_chain) {
                    const Area* child = child_chain->lastChunk();
                    const Area* area = child->parent();
                    if (child_chain->firstChunk()->parent() == area)
                        return get_valid(
                            AreaPos(area, area->areaPos(
                                        child_chain).pos() + 1), adj);
                    else
                        return get_valid(AreaPos(area, 0), adj);
                }
                after = after->prevSibling();
            }
            Chain* chain = get_formatted_chain(resultPos.node(), view);
            if (chain) {
                return get_valid(AreaPos(chain->firstChunk(), 0), adj);
            }
        }
    return AreaPos();
}

AreaPos StructEditor::toAreaPos(const GrovePos& resultPos,
                                CursorAdjustment adjustment) const
{
    return to_area_pos(resultPos, adjustment, editableView());
}

//////////////////////////////////////////////////////////////////////

bool StructEditor::setCursor(const GrovePos& srcPos, 
                             const AreaPos& areaPos, bool isTop)
{
    //editPolicy_->finishIM();
    GrovePos old_pos = editViewSrcPos();
    
    editPolicy_->resetEnterPressCount();
    TPROF_CALL(Sc, editableView().setCursor(areaPos, srcPos, isTop));
    showContextInfo();

    if (srcPos.isNull() && old_pos.isNull())
        return true;
        
    if (positionWatcher_->isModified() || old_pos.isNull() || 
        srcPos.isNull() || old_pos.contextNode() != srcPos.contextNode() ||
        old_pos.node()->parent() != srcPos.node()->parent()) {

        //! Notify Position Change
        positionWatcher_->updatePos(srcPos);
        enableActions(srcPos);
        GrovePosEventData ed(srcPos);
        elementContextChange().dispatchEvent(&ed);
    }
    return true;
}

bool StructEditor::setCursor(const AreaPos& areaPos, bool isTop)
{
    return setCursor(EditContext::getSrcPos(areaPos), areaPos, isTop);
}

void StructEditor::setCursorFromTreeloc()
{
    String sv = getDsi()->getSafeProperty
        (DocSrcInfo::CURSOR_TREELOC)->getString();
    if (sv.length() < 2)
        return;
    GroveEditor::GrovePos src_pos(sv, grove()->document(), false);
    if (!src_pos.node())
        return;
    if (src_pos.type() == GroveEditor::GrovePos::ELEMENT_POS &&
        src_pos.before() && !src_pos.before()->firstChild())
            src_pos = GrovePos(src_pos.before());

    GrovePos result_pos = toResultPos(src_pos, 0);
    if (!result_pos.isNull())
        setCursor(src_pos, toAreaPos(result_pos), true);
}

void StructEditor::showPageInfo()
{
    const AreaPos& area_pos(editableView().context().areaPos());
    Sui::Action* pageInfo = uiActions().pageInfo();
    if (area_pos.area()) {
        uint page = 0;
        uint max_page = 0;
        area_pos.getPagePos(page, max_page);
        String page_s;
        page_s.setNum(page);
        String max_page_s;
        max_page_s.setNum(max_page);
        pageInfo->property(Sui::INSCRIPTION)->setString(
            tr("Page %1 of %2").arg(page_s).arg(max_page_s));
    }
    else
        pageInfo->property(Sui::INSCRIPTION)->setString("");
}

void StructEditor::showCursorInfo()
{
    String info;
    info.reserve(128);
    const GrovePos& src_pos = editViewSrcPos();
    if (!src_pos.isNull() && src_pos.node()) {
        const Node* node = src_pos.node();
        int line = -1;
        int column = src_pos.idx() + 1;
 
        const AreaPos& area_pos(editableView().context().areaPos());
        if (!area_pos.isNull() && 
            area_pos.area()->type() == TEXT_AREA) {
            const TextFo* text_fo = static_cast<const TextFo*>(
                area_pos.area()->chain());
            if (text_fo->isPreserveLinefeed()) {
                line = area_pos.area()->XListItem<Area>::siblingIndex() + 1;
                column = area_pos.pos() + 1;
            }
        }
        if (line < 0)
            info += "[" + String::number(column) + "]";
        else
            info += tr(" (Line: %1 Col %2)").arg(String::number(line)).
                    arg(String::number(column));
        while (node && node->parent()) {
            info.insert(0, node->nodeName());
            info.insert(0, "/");
            node = node->parent();
        }
        String ename;
        const EntityReferenceStart* ers = src_pos.getErs();
        while (ers && ers->getSectParent()) {
            const EntityDecl* ed = ers->entityDecl();
            ers = static_cast<const EntityReferenceStart*>
                (ers->getSectParent());
            ename.insert(0, ed->asAnnotationString());
            if (ers->getSectParent())
                ename.insert(0, "/");
        }
        if (!ename.isEmpty())
            info.insert(0, String(tr("(%1) ").arg(ename)));
    } 
    else
        info = "";
    //if (editPolicy_->isComposing())
    //    info.insert(0, NOTR("** COMPOSING ** "));
    doc_->showContextInfo(info);
}

void StructEditor::showContextInfo()
{
    showPageInfo();
    showCursorInfo();
}

bool StructEditor::getCheckedPos(GrovePos& srcPos, int opclass) const
{
    srcPos = editViewSrcPos();
    if (srcPos.isNull())
        return false;
    return isEditableEntity(srcPos, opclass) == POS_OK;
}

//////////////////////////////////////////////////////////////////////

StructEditor::PositionStatus
StructEditor::getSelection(GrovePos& from, GrovePos& to, int opClass)
{
    const GroveSelection& selection = editableView().getSelection().src_;
    GroveSelection::Status status = selection.status();
    if (GroveSelection::IS_VALID_SELECTION != status) {
        if (selection.isEmpty())
            return POS_FAIL;
        if (SILENT_OP & opClass)
            return POS_FAIL;
        String err;
        switch (status) {
            case GroveSelection::IS_CROSS_SECTION :
                err = tr("Selection crosses entity or redlining "
                         "section corners");
                break;
            case GroveSelection::IS_EMPTY_SELECTION:
            case GroveSelection::IS_NULL_SELECTION:
                err = tr("Selection is empty, or one end of selection "
                         "points to generated text");
                break; 
            case GroveSelection::IS_ATTRIBUTE_SELECTION:
            default:
                err = tr("Selection points to attribute");
                break;
        };
        doc_->showMessageBox(
            SernaDoc::MB_WARNING, tr("Invalid Selection"), err, tr("Close"));
        return POS_FAIL;
    }

    from = selection.minPos();
    to = selection.maxPos();
    const PositionStatus pstat_from = isEditableEntity(from, opClass);
    if (POS_FAIL == pstat_from)
        return POS_FAIL;
    const PositionStatus pstat_to = isEditableEntity(to, opClass);
    if (POS_FAIL == pstat_to)
        return POS_FAIL;
    const bool is_readonly = 
        (POS_RDONLY == pstat_from || POS_RDONLY == pstat_to);

    DBG(SE.TEST) << "Selection OP: min/max = ";
    DBG_EXEC(SE.TEST, from.dump());
    DBG_EXEC(SE.TEST, to.dump());
    return is_readonly ? POS_RDONLY : POS_OK;
}

static void adjust_pos(ChainPos& pos, bool isStart) 
{
    if (pos.isNull())
        return;
    while (pos.chain()->hasParentOrigin()) {
        if (!isStart && 0 == pos.pos()) {
            pos = ChainPos(pos.chain()->parentChain(), 
                           pos.chain()->chainPos());
            continue;
        }
        if (isStart && pos.chain()->chainPosCount() == pos.pos() + 1) {
            pos = ChainPos(pos.chain()->parentChain(), 
                           pos.chain()->chainPos() + 1);
            continue;
        }
        return;
    }
}

static inline ChainSelection get_balanced(const ChainSelection& sel)
{
    ChainPos start(sel.start());
    ChainPos end(sel.end());
    adjust_pos(start, true);
    adjust_pos(end, false);
    ChainSelection chain_selection(start, end);
    chain_selection.balance();
    return chain_selection;
}

ChainSelection get_chain_selection(
    const GroveSelection& src, const GrovePos& foHint, const StructEditor* se)
{
    if (!src.start().isNull() &&  !src.end().isNull() && 
        src.start().node()->grove() != src.end().node()->grove())
        return ChainSelection();

    GrovePos fo_hint = foHint;
    if (fo_hint.isNull()) {
        Node* root = se->fot()->document()->documentElement();
        fo_hint = GrovePos(root, root->firstChild());
    }
    GrovePos min_result = 
        to_result_pos(src.minPos(), fo_hint, false, foHint.isNull());
    if (min_result.isNull())
        return ChainSelection();
    
    GrovePos max_result = to_result_pos(src.maxPos(), min_result, false, true);
    if (max_result.isNull())
        return ChainSelection();

    AreaPos min_area_pos = to_area_pos(
        min_result, StructEditor::DO_NOT_ADJUST, se->editableView());

    if (EditContext::getSrcPos(min_area_pos) != src.minPos()) {
        std::cerr << NOTR("EditContext::getSrcPos(min_area_pos) != min_result\n");
        src.minPos().dump();
        EditContext::getSrcPos(min_area_pos).dump();
    }
    
    AreaPos max_area_pos = se->toAreaPos(
        max_result, StructEditor::DO_NOT_ADJUST);
    
    if (src.start() > src.end())
        return ChainSelection(max_area_pos, min_area_pos);
    return ChainSelection(min_area_pos, max_area_pos);
}

void StructEditor::setSelection(const ChainSelection& chainRaw, 
                                const GroveSelection& srcRaw, 
                                bool isSetCursor, bool setCursorToStart,
                                bool clearSelectionHistory)
{
    if (!srcRaw.start().isNull() &&  !srcRaw.end().isNull() && 
        srcRaw.start().node()->grove() != srcRaw.end().node()->grove())
        return;
    
    if (srcRaw.isEmpty()) {
        removeSelection();
        if (isSetCursor && !chainRaw.start().isNull()) {
            AreaPos area_pos = chainRaw.start().toAreaPos();
            if (!area_pos.isNull()) {
                if (!area_pos.isValid())
                    area_pos.findAllowed(!setCursorToStart, true);
                if (!area_pos.isNull()) 
                    setCursor(area_pos);
            }   
        }
        return;
    }
    TreeSelection tree_raw(chainRaw.start(), chainRaw.end());
    const Selection selection = editableView().getSelection(true);
    if (selection.src_ == srcRaw && selection.tree_ == tree_raw)
        return;

    if (clearSelectionHistory)
        selectionHistory().clear();

    //! Balance selection
    GroveSelection src_balanced = srcRaw;
    src_balanced.balance();
    
    ChainSelection chain_balanced = chainRaw;
    GrovePos start_result, end_result;
    
    chain_balanced = get_chain_selection(
        src_balanced, 
        (chainRaw.start().isNull()) 
        ? EditContext::getFoPos(editableView().context().areaPos())
        : EditContext::getFoPos(chainRaw.start().toAreaPos()),
         this);
    if (chain_balanced.isEmpty())
        chain_balanced = get_balanced(chainRaw);
    
    editableView().setSelection(
        Selection(tree_raw, srcRaw),
        Selection(TreeSelection(chain_balanced.start(), chain_balanced.end()), 
                  src_balanced));
    
    //! Set cursor to the corner of selection
    if (isSetCursor) {
        TreeSelection tree = editableView().getSelection().tree_;
        AreaPos area_pos;
        if (!tree.isEmpty()) {
            TreePos tree_pos = (setCursorToStart) ? tree.start() : tree.end();
            area_pos = tree_pos.toAreaPos(editableView().rootArea());
        }
        else
            if (!tree.start().isNull())
                area_pos = tree.start().toAreaPos(editableView().rootArea());
        if (!area_pos.isNull()) {
            if (!area_pos.isValid())
                area_pos.findAllowed(!setCursorToStart, true);
            if (!area_pos.isNull()) 
                setCursor(area_pos);
        }
    }

    notifySelectionChange(chain_balanced, src_balanced);
}

void StructEditor::removeSelection()
{
    selectionHistory().clear();
    const Selection& selection = editableView().getSelection(true);
    if (!selection.tree_.isEmpty() || !selection.src_.isEmpty()) {
        editableView().removeSelection();
        notifySelectionChange(ChainSelection(), GroveSelection());
    }
}

void StructEditor::extendSelectionTo(const GrovePos& srcPos,
                                     const AreaPos& areaPos)
{
    Selection selection = editableView().getSelection(true);
    if (selection.tree_.isEmpty())
        selection.tree_ = TreeSelection(editableView().context().treePos());

    ChainSelection chain_raw(
        selection.tree_.start().toAreaPos(editableView().rootArea()), areaPos);

    if (selection.src_.isEmpty())
        selection.src_ = GroveSelection(editableView().context().srcPos());
    selection.src_.extendTo(srcPos);

    setSelection(chain_raw, selection.src_, false);
}

////////////////////////////////////////////////////////

void PositionWatcher::updatePos(const GrovePos& pos)
{
    deregisterFromAllNodes();
    modified_ = false;
    if (0 == pos.node())
        return;
    pos.node()->registerNodeVisitor(this, NodeVisitor::NOTIFY_CHILD_INSERTED |
                                    NodeVisitor::NOTIFY_CHILD_REMOVED);
}


////////////////////////////////////////////////////////

static bool is_folded(const Node* n)
{
    return (n->nodeType() == Node::ELEMENT_NODE &&
            static_cast<const Element*>(n)->attrs().getAttribute(NOTR("se:fold")));
}

const Node* fo_hint(const GrovePos& foPos)
{
    const Node* fo_hint = 0;
    if (!foPos.isNull()) {
        if (foPos.type() == GrovePos::ELEMENT_POS && foPos.before())
            fo_hint = foPos.before()->nextSibling();
        if (0 == fo_hint)
            fo_hint = foPos.node();
    }
    return fo_hint;
}

static bool set_src_pos(StructEditor* se, const Node* origin,
                        bool adjust = true)
{
    if (0 == origin || !origin->parent() || !origin->grove())
        return false;
    se->removeSelection();

    const Node* fohint = fo_hint(se->editViewFoPos());

    const Node* adjOrigin = 0;
    if (adjust && Node::ELEMENT_NODE == origin->nodeType() &&
        origin->firstChild()) {
            adjOrigin = origin->firstChild();
            while (adjOrigin && adjOrigin->nodeType() != Node::ELEMENT_NODE &&
                   adjOrigin->nodeType() != Node::TEXT_NODE)
                adjOrigin = adjOrigin->nextSibling();
    }
    GrovePos def_pos(adjOrigin ? adjOrigin : origin);
    GrovePos result_pos;
    AreaPos old_pos = se->editableView().context().areaPos();
    if (0 == fohint)
        result_pos = se->toResultPos(def_pos, 0, false);
    else {
        typedef XTreeWalkIterator<Node> NodeIter;
        NodeIter xit(const_cast<Node*>(fohint), true);
        for (; xit.node(); ++xit) {
            Node* foNode = xit.node();
            const Node* resOrigin = Xslt::resultOrigin(foNode);
            if (0 == resOrigin)
                continue;
            if (resOrigin != adjOrigin && resOrigin != origin)
                continue;
            if (resOrigin == origin && !is_folded(foNode))
                continue;
            AreaPos area_pos = se->toAreaPos(GrovePos(foNode));
            if (!area_pos.isNull() && area_pos != old_pos) {
                se->setCursor(area_pos, true);
                return true;
            }
        }
    }
    if (result_pos.isNull())
        result_pos = se->toResultPos(def_pos, 0, false);
    if (!result_pos.isNull())
        return se->setCursor(se->toAreaPos(result_pos), true);
    return true;
}

///////////////////////////////////////////////////////////////////////////

class STRUCTEDITOR_EXPIMP SetSrcPos : public CommandEvent<StructEditor> {
public:
    SetSrcPos(const GrovePosEventData& grovePos)
        : node_(grovePos.pos_.node()) {}

    virtual bool doExecute(StructEditor* se, EventData*)
    {
        se->setLastPosNode(node_);
        for (Node* n = node_; n; n = n->parent())
            if (is_node_folded(n))
                node_ = n;
        return set_src_pos(se, node_);
    }
private:
    Node* node_;
};

COMMON_NS_BEGIN
template<> STRUCTEDITOR_EXPIMP
CommandEventPtr makeCommand<SetSrcPos>(const EventData* ed)
{
    return new SetSrcPos(static_cast<const GrovePosEventData&>(*ed));
}
COMMON_NS_END

/////////

static bool go_to_line(StructEditor* se, int line)
{
    XTreeDocOrderIterator<Node> it(se->grove()->document());
    Node* lastn = 0;
    for (; it.node(); ++it) {
        const NodeExt* node_ext = it.node()->nodeExt();
        if (0 == node_ext)
            continue;
        const LineLocExt* lext = node_ext->asConstLineLocExt();
        if (0 == lext)
            continue;
        lastn = it.node();
        if (lext->line() >= line)
            break;
    }
    if (lastn)
        return set_src_pos(se, lastn);
    return false;
}

class Attributes;

GOTO_ORIGIN_EVENT_IMPL(StructGoToDocOrigin, StructEditor)

bool StructGoToDocOrigin::doExecute(StructEditor* se, EventData*)
{
    if (!origin_.docOrigin())
        return false;
    const NodeOrigin* no = 
        dynamic_cast<const NodeOrigin*>(origin_.docOrigin());
    if (no) {
        const Node* n = no->node();
        if (n->grove() && n->grove()->parent()) {
            se->sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                tr("Cannot set cursor to message origin"),
                tr("The origin of this message belongs to external file, "
                   "not to the currently edited document."), tr("&Ok"));
            return false;
        }
        if (Node::ATTRIBUTE_NODE == n->nodeType())
            n = parentNode(n);
        if (!set_src_pos(se, n, false))
            return false;
        if (Node::ATTRIBUTE_NODE == no->node()->nodeType() ||
            (origin_.moduleOrigin() && origin_.moduleOrigin()->node() &&
            NOTR("attribute") == static_cast<const NodeWithNamespace*>
            (origin_.moduleOrigin()->node())->localName()))
                return makeCommand<Attributes>()->execute(se);
        se->grabFocus();
        return true;
    }
    const PlainOrigin* po =
        dynamic_cast<const PlainOrigin*>(origin_.docOrigin());
    bool ok = false;
    if (po) {
        ok = go_to_line(se, po->line());
        if (ok)
            se->grabFocus();
    }
    return ok;
}

/////////

void StructEditor::setCursorFromLineInfo()
{
    if (getDsi()->getProperty(DocSrcInfo::CURSOR_TREELOC))
        return;
    PropertyNode* ln_prop = getDsi()->getProperty(DocSrcInfo::LINE_NUMBER);
    if (0 == ln_prop)
        return;
    (void) go_to_line(this, ln_prop->getInt());
}
