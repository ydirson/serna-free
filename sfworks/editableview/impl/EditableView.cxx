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
#include "editableview/impl/debug.h"
#include "editableview/EditableView.h"
#include "editableview/TooltipView.h"

#include "formatter/XslFoExt.h"
#include "formatter/Exception.h"
#include "formatter/area_pos_utils.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"

#include "common/DiffuseSearch.h"
#include "common/safecast.h"
#include "common/MessageUtils.h"

#include "grove/Nodes.h"
#include "groveeditor/GrovePos.h"

#include <QRect>

USING_COMMON_NS;
USING_GROVE_NAMESPACE;
using namespace GroveEditor;

//NOTE: clash between Formatter namespace  and Formatter class
//      makes us to move following functions up before using ns declaration
//TODO: change name of namespace or class
void EditableView::format(const ViewParam& viewParam)
{
    //TimeGuard time_guard;
    //(void)time_guard;

    Formatter::MediaInfo media_info(96, // actually set in canvasview
                         viewParam.showTags_,
                         viewParam.showPaginated_,
                         viewParam.indent_,
                         viewParam.colorScheme_,
                         imageProvider(),
                         inlineObjFactory(),
                         viewParam.showNbsp_);
    //! Preparing formatter
    try {
        context_.set(Formatter::AreaPos(), GrovePos());
        formatter_ = 0;
        formatter_ = makeFormatter(&*messenger_, media_info, this, 
            fontManager(), fot_, areaViewFactory(), tagMetrixTable());
        formatter_->format(false);
        rootArea_ = formatter_->rootArea();
        selection_.clear();
        balancedSelection_.clear();
    }
    catch (Formatter::Exception& e) {
        formatter_.clear();
        throw EditableViewException(EditableViewMessages::editableView,
                                    e.what());
    }
}

bool EditableView::update()
{
    context_.set(Formatter::AreaPos(), context_.srcPos());
    try {
        formatter_->format(true);
    }
    catch (Formatter::Exception& e) {
        formatter_->detachAreaTree();
        formatter_.clear();
        throw EditableViewException(EditableViewMessages::editableView,
                                    e.what());
    }
    return true;
}

USING_FORMATTER_NAMESPACE;

namespace {
    class MarkArea : public EditableView::SelectionOp {
    public:
        virtual void operator()(const Area* area, long from, long to)
        {
            area->getView()->setSelection(from, to);
        }
    };
    static MarkArea mark_area;

    class UnmarkArea : public EditableView::SelectionOp {
    public:
        virtual void operator()(const Area* area, long, long)
        {
            area->getView()->removeSelection();
        }
    };
    static UnmarkArea unmark_area;

    class RepaintArea : public EditableView::SelectionOp {
    public:
        virtual void operator()(const Area* area, long, long)
        {
            area->getView()->repaintSelection();
        }
    };
    static RepaintArea repaint_area;

    static inline const Area* page_of(const Area* area)
    {
        while (area && PAGE_AREA != area->type())
            area = area->parent();
        return area;
    }

    //! used only in get_page_pos
    class PageTest {
    public:
        PageTest(const Area* page)
            : page_(page) {}

        bool    operator()(const AreaPos* pos) const
            {
                return (page_of(pos->area()) == page_);
            }
    private:
        const Area* page_;
    };
    //!
    static inline AreaPos get_page_pos(const Area* page, bool up)
    {
        if (0 == page)
            return AreaPos();
        const Area* area = page;
        if (up)
            while (area->prevChunk() && !area->empty())
                area = area->firstChild();
        else
            while (area->nextChunk() && !area->empty())
                area = area->lastChild();

        AreaPos pos(area, (up) ? 0 : area->chunkPosCount() - 1);
        if (pos.validate<PageTest>(up, PageTest(page)))
            return pos;

        return AreaPos();
    }
    static inline GrovePos get_attr_pos(const Node* node)
    {
        if (0 == node)
            return false;
        Node* elem = 0;
        if (node->nodeType() == Node::ATTRIBUTE_NODE)
            elem = static_cast<const Attr*>(node)->element();
        else
            if (node->parent() &&
                node->parent()->nodeType() == Node::ATTRIBUTE_NODE)
                elem = static_cast<const Attr*>(node->parent())->element();
        if (elem)
            return GrovePos(elem, elem->firstChild());
        return GrovePos();
    }
    //!
    class OriginTest {
    public:
        bool    operator()(const Fo* fo) const
            {
                if (LINE_FO == fo->type())
                    return false;
                const Node* origin = XslFoExt::origin(fo->headFoNode());
                if (origin && origin->grove()->registerVisitors()) {
                    if (0 == fo->headFoNode()->parent())
                        return true;
                    const Node* parent_origin =
                        XslFoExt::origin(fo->headFoNode()->parent());
                    if (origin == parent_origin)
                        return false;
                    if (origin->parent() != parent_origin)
                        return false;
                    return true;
                }
                return false;
            }
    };
    //!
    class GrovePosMaker {
    public:
        GrovePos make(const Fo* fo, int i) const
            {
                const Node* node = XslFoExt::origin(fo->headFoNode());
                if (!node || !node->grove()->registerVisitors())
                    return GrovePos();
                GrovePos attr_pos = get_attr_pos(node);
                if (!attr_pos.isNull())
                    return attr_pos;
                switch (i) {
                    case 1 :
                            return GrovePos(node->parent(), node);
                    case -1 :
                            return GrovePos(node->parent(),
                                            node->nextSibling());
                    default:
                        return GrovePos(node);
                }
                return GrovePos();
            }
    };
    //!
    GrovePos get_origin_pos(const Chain* chain, ulong pos, bool preferPrevNode)
    {
        if (!chain)
            return GrovePos();
        if (chain->hasChildChains()) {
            const Chain* before = chain->chainAt(pos);
            return find_diffuse<Fo, OriginTest, GrovePos, GrovePosMaker>(
                static_cast<const Fo*>(chain),
                static_cast<const Fo*>(before),
                OriginTest(), GrovePosMaker(), preferPrevNode);
        }
        else {
            ulong count = chain->chainPosCount();
            if (pos > count)
                pos = count;
            Node* origin =
                const_cast<Node*>(XslFoExt::origin(chain->headFoNode()));
            if (!origin || !origin->grove()->registerVisitors())
                return GrovePos();
            GrovePos attr_pos = get_attr_pos(origin);
            if (!attr_pos.isNull())
                return attr_pos;
            return GrovePos(origin, pos);
        }
        return GrovePos();
    }
}

/*
 */
void EditContext::set(const AreaPos& areaPos, const GrovePos& srcPos)
{
    areaPos_ = areaPos;
    if (!persistentPosLocked_ || areaPos_.isNull())
        persistentAreaPos_ = areaPos_;
    treePos_ = areaPos_;
    foPos_  = getFoPos(areaPos_);
    srcPos_ = srcPos;
}

GrovePos EditContext::getFoPos(const AreaPos& areaPos)
{
    if (areaPos.isNull())
        return GrovePos();
    ChainPos chain_pos(areaPos);
    if (chain_pos.isNull())
        return GrovePos();
    const Chain* chain = chain_pos.chain();
    ulong pos = chain_pos.pos();
    if (chain->hasChildChains()) {
        const Chain* next = chain->chainAt(pos);
        if (next) {
            const Node* next_node = next->headFoNode();
            return GrovePos(next_node->parent(), next_node);
        }
        const Chain* prev = (pos) ? chain->chainAt(pos - 1) : 0;
        if (prev) {
            const Node* prev_node = prev->tailFoNode();
            return GrovePos(prev_node->parent(), prev_node->nextSibling());
        }
    }
    ulong count = chain->chainPosCount();
    if (pos > count)
        pos = count;
    return GrovePos(chain->headFoNode(), pos);
}

GrovePos EditContext::getSrcPos(const AreaPos& areaPos, bool preferPrevNode)
{
    if (areaPos.isNull())
        return GrovePos();
    ChainPos chain_pos(areaPos);
    GrovePos orig = get_origin_pos(
        chain_pos.chain(), chain_pos.pos(), preferPrevNode);
    if (!orig.isNull()) {
        GrovePos fo_pos = getFoPos(areaPos);
        Chain* chain = XslFoExt::areaChain(fo_pos.node());
        if (COMBO_BOX_FO == chain->type() || LINE_EDIT_FO == chain->type()) {
            if (GrovePos::TEXT_POS == orig.type())
                return GrovePos(orig.text(), 0);
            Node* node = orig.node()->firstChild();
            if (node && Node::TEXT_NODE == node->nodeType())
                return GrovePos(TEXT_CAST(node), 0);
        }
        if (GrovePos::TEXT_POS == orig.type()) {
            if (TEXT_FO == chain->type()) {
                TextFo* text_fo = SAFE_CAST(TextFo*, chain);
                return GrovePos(orig.text(),
                                text_fo->convertPos(orig.idx(), false));
            }
        }
        return orig;
    }
    return GrovePos();
}

AreaPos EditContext::getWordCursor(bool left) const
{
    if (areaPos_.isNull())
        return areaPos_;
    ChainPos chain_pos(areaPos_);
    if (TEXT_AREA == areaPos_.area()->type()) {
        const TextFo* chain = SAFE_CAST(const TextFo*, chain_pos.chain());
        String text = chain->strippedText();
        unsigned pos = chain_pos.pos();
        unsigned next = pos;
        for (;;) {
            bool next_ok = ((text.length() == next) || (' ' != text[next]));
            bool prev_ok = ((0 == next) || (' ' == text[next - 1]));
            if (pos != next && next_ok && prev_ok)
                break;
            if (left)
                if (0 < next)
                    --next;
                else
                    break;
            else
                if (text.length() > next)
                    ++next;
                else
                    break;
        }
        ChainPos word_pos(chain, next);
        AreaPos area_pos(word_pos.toAreaPos());
        if (pos == next) {
            if (left)
                area_pos--;
            else
                area_pos++;
            if (!area_pos.isNull())
                area_pos.findAllowed(!left, false);
            else
                return AreaPos();
        }
        return area_pos;
    }
    return getShiftedCursor((left) ? -1 : 1);
}

AreaPos EditContext::getLineCursor(bool start) const
{
    if (areaPos_.isNull())
        return areaPos_;
    const Area* area = areaPos_.area();
    while (area && BLOCK_AREA != area->type()) {
        if (LINE_AREA == area->type()) {
            if (start)
                while (area->firstChild() && area->firstChild()->prevChunk())
                    area = area->firstChild();
            else
                while (area->lastChild() && area->lastChild()->nextChunk())
                    area = area->lastChild();
            AreaPos pos(area, (start) ? 0 : area->chunkPosCount());
            pos.findAllowed(start, false);
            if (!pos.isNull() && pos.isValid())
                return pos;
            else
                return AreaPos();
        }
        area = area->parent();
    }
    return AreaPos();
}

AreaPos EditContext::getShiftedCursor(int shift) const
{
    if (areaPos_.isNull())
        return areaPos_;
    //! Increment position
    AreaPos pos = areaPos_;
    if (shift > 0)
        pos += abs(shift);
    else
        pos -= abs(shift);

    if (pos.isNull())
        return pos;

    //! Normalize incremented position
    pos.findAllowed(shift > 0, false);

    if (!pos.isNull() && pos.isValid())
        return pos;
    return AreaPos();
}

/*
 */
EditableView::EditableView(Messenger* messenger,
                           const GrovePtr& fot, EditPolicy* editPolicy)
    : messenger_(messenger),
      fot_(fot),
      editPolicy_(editPolicy),
      rootArea_(0),
      isSelectionBalancing_(false),
      formattingMode_(VISIBLE)
{
    DBG(EV.TEST) << "EditableView created\n";
    context_.lockPersistentPos(false);
}

EditableView::~EditableView()
{
    DBG(EV.TEST) << "EditableView destroyed\n";
}

bool EditableView::ensureFormatted(const Node* foNode)
{
    while (!formatter_->isFinished()) {
        if (formatter_->isFormatted(foNode))
            return true;
        update();
    }
    return (0 != XslFoExt::areaChain(foNode));
}

void EditableView::detachAreaTree()
{
    if (formatter_) {
        formatter_->detachAreaTree();
        fontManager()->clear();
    }
}

void EditableView::continuePostponedFormatting()
{
    if (!formatter_->isFinished()) {
        hideCursor();
        //! Save cursor position
        TreePos cursor_pos(context_.areaPos());
        TreePos persistent_pos(context_.persistent());

        formatter_->format(true);
        //! Change formatting mode when postponed
        switch (formattingMode_) {
            case VISIBLE :
                break;
            case IDLE :
            case PAGE :
            case WHOLE :
                formattingMode_ = VISIBLE;
                break;
            default:
                break;
        }
        //! Restore cursor position
        context_.set(persistent_pos.toAreaPos(rootArea_), GrovePos());
        context_.lockPersistentPos(true);
        AreaPos area_pos = cursor_pos.toAreaPos(rootArea_);
        context_.set(area_pos, EditContext::getSrcPos(area_pos));
        context_.lockPersistentPos(false);

        *pstream() << "";
        showCursor();
    }
}

bool EditableView::isToPostpone(const Area* area)
{
    switch (formattingMode_) {
        case VISIBLE :
            if (VERTICAL == area->chain()->progression() ||
                PARALLEL == area->chain()->progression())
                return isBelowVisibleRange(area);
            break;
        case IDLE :
            // if (someEventComes) return true;
            return false;
            break;
        case PAGE :
            return (PAGE_AREA == area->type());
            break;
        case WHOLE :
            return false;
            break;
        default:
            break;
    }
    return false;
}

void EditableView::setCursor(const AreaPos& areaPos, const GrovePos& srcPos, 
                             bool top)
{
    context_.set(areaPos, srcPos);
    adjustView(areaPos, top);
}

AreaPos EditableView::getInitialCursor() const
{
    if (rootArea_) {
        const Area* first_page = rootArea_->firstChild();
        if (first_page)
            return get_page_pos(first_page, true);
    }
    return AreaPos();
}

static AreaPos get_pos_beside_chain(const Area* area, bool before)
{
    const Area* corner_area = (before)
        ? area->chain()->firstChunk() : area->chain()->lastChunk();
    return (before)
        ? AreaPos::getPosBeforeArea(corner_area)
        : AreaPos::getPosAfterArea(corner_area);
}

static AreaPos move_cursor_up(const AreaPos& areaPos, const Area* rootArea,
                              const CPoint persistent, bool up)
{
    AreaPos area_pos = areaPos;
    const Area* area = area_pos.area();

    if (area->isProgression(HORIZONTAL)) {
        while (area->parent()->isProgression(HORIZONTAL))
            area = area->parent();
        const bool is_corner_chunk = (up)
            ? !area->prevChunk() : !area->nextChunk();
        if (!is_corner_chunk) {
            Area* sibling_chunk = (up) ? area->prevChunk() : area->nextChunk();
            AreaPos area_pos = sibling_chunk->mapToAreaPos(
                sibling_chunk->mapFrom(rootArea, persistent).x_, up);
            area_pos = find_valid_pos(area_pos, !up, false);
            return area_pos;
        }
        area_pos = (up)
            ? AreaPos::getPosBeforeArea(area)
            : AreaPos::getPosAfterArea(area);
    }
    else
        if (area->isProgression(PARALLEL)) {
            area_pos = get_pos_beside_chain(area, up);
        }
        else
            if (area->isProgression(VERTICAL)) {
                if (up)
                    area_pos--;
                else
                    area_pos++;
            }

    while (!area_pos.isNull()) {
        if (area_pos.area()->isProgression(HORIZONTAL) ||
            area_pos.area()->isProgression(PARALLEL)) {
            bool from_descendant = false;
            for (const Area* ancestor = area; ancestor;
                 ancestor = ancestor->parent())
                if (ancestor->chain() == area_pos.area()->chain()) {
                    from_descendant = true;
                    break;
                }
            if (!from_descendant) {
                area_pos = area_pos.area()->mapToAreaPos(
                    area_pos.area()->mapFrom(rootArea, persistent).x_, up);
                area_pos = find_valid_pos(area_pos, !up, false);
            }
            else
                if (area_pos.area()->isProgression(PARALLEL))
                    area_pos = get_pos_beside_chain(area_pos.area(), up);
            if (area_pos.isNull() || area_pos.isValid())
                return area_pos;
        }
        else
            if (area_pos.area()->isProgression(VERTICAL) &&
                area_pos.isValid())
                return area_pos;

        area = area_pos.area();
        if (up)
            area_pos--;
        else
            area_pos++;
    }
    return AreaPos();
}

AreaPos EditableView::getUpCursor(bool up) const
{
    DBG(EV.TEST) << "getUpCursor\n";
    DBG_IF(EV.TEST) context_.areaPos().dump();

    if (context_.areaPos().isNull())
        return AreaPos();

    AreaPos area_pos = context_.areaPos();
    AreaPos pers_pos = (context_.persistent().isNull())
        ? area_pos : context_.persistent();
    CPoint persistent(pers_pos.area()->absCursorRect(pers_pos.pos()).origin_);

    return move_cursor_up(area_pos, rootArea_, persistent, up);
}


AreaPos EditableView::getPageCursor(bool up, bool toCorner)
{
    if (context_.areaPos().isNull())
        return AreaPos();

    AreaPos pos = context_.areaPos();
    const Area* page = pos.area();
    while (PAGE_AREA != page->type())
        page = page->parent();

    if (toCorner) {
        if (!up && !formatter_->isFinished()) {
            formattingMode_ = WHOLE;
            continuePostponedFormatting();
        }
        page = (up) ? page->chain()->firstChunk() : page->chain()->lastChunk();
        pos = get_page_pos(page, up);
    }
    else {
        if (!up && !formatter_->isFinished()) {
            formattingMode_ = PAGE;
            continuePostponedFormatting();
        }
        pos = get_page_pos(page, up);

        if (!pos.isNull() && context_.areaPos() == pos) {
            if (!up && !formatter_->isFinished()) {
                formattingMode_ = PAGE;
                continuePostponedFormatting();
            }
            Area* sibling_page = (up)
                ? page->prevSibling()
                : page->nextSibling();
            if (sibling_page)
                pos = get_page_pos(sibling_page, !up);
            else
                return AreaPos();
        }
    }
    return (context_.areaPos() == pos) ? AreaPos() : pos;
}

void EditableView::setSelection(const Selection& raw,   
                                const Selection& balanced, bool force)
{
    if (!force && raw.tree_ == selection_.tree_ && raw.src_ == selection_.src_)
        return;
    //! Repaint balanced selection
    if (isSelectionBalancing_) {
        if (balanced.tree_ != balancedSelection_.tree_) {
            inSelectionDo(balancedSelection_.tree_, unmark_area);
            inSelectionDo(balanced.tree_, mark_area);
            repaintSelection(balancedSelection_.tree_, balanced.tree_);
        }
    }
    //! Repaint unbalanced selection
    else {
        inSelectionDo(selection_.tree_, unmark_area);
        inSelectionDo(raw.tree_, mark_area);
        repaintSelection(selection_.tree_, raw.tree_);
    }
    selection_ = raw;
    balancedSelection_ = balanced;
    
    repaintView();
}

void EditableView::setSelectionBalancing(bool isOn)
{
    isSelectionBalancing_ = isOn;
    if (selection_.tree_ == balancedSelection_.tree_)
        return;
    if (isSelectionBalancing_) {
        inSelectionDo(selection_.tree_, unmark_area);
        inSelectionDo(balancedSelection_.tree_, mark_area);
        repaintSelection(selection_.tree_, balancedSelection_.tree_);
    }
    else {
        inSelectionDo(balancedSelection_.tree_, unmark_area);
        inSelectionDo(selection_.tree_, mark_area);
        repaintSelection(balancedSelection_.tree_, selection_.tree_);
    }
}

void EditableView::removeSelection()
{
    setSelection(Selection(), Selection());
}

void EditableView::repaintSelection(const TreeSelection& old_s,
                                    const TreeSelection& new_s) const
{
    TreeSelection i(old_s.intersection(new_s));
    if (i.isEmpty()) {
        inSelectionDo(old_s, repaint_area);
        inSelectionDo(new_s, repaint_area);
        return;
    }
    if (old_s.minLoc() < i.minLoc())
        inSelectionDo(TreeSelection(old_s.minLoc(), i.minLoc()), repaint_area);
    if (i.maxLoc() < old_s.maxLoc())
        inSelectionDo(TreeSelection(i.maxLoc(), old_s.maxLoc()), repaint_area);

    if (new_s.minLoc() < i.minLoc())
        inSelectionDo(TreeSelection(new_s.minLoc(), i.minLoc()), repaint_area);
    if (i.maxLoc() < new_s.maxLoc())
        inSelectionDo(TreeSelection(i.maxLoc(), new_s.maxLoc()), repaint_area);
}

void EditableView::inSelectionDo(const Chain* chain, ulong level,
                                 bool isStartCorner, bool isEndCorner,
                                 const TreePos& start, const TreePos& end,
                                 SelectionOp& op) const
{
    if (0 == chain)
        return;
    ulong from = 0;
    isStartCorner &= start.treeloc().length() > level;
    if (isStartCorner)
        from  = start.treeloc()[level];

    ulong to = chain->chainPosCount() - 1;
    isEndCorner &= end.treeloc().length() > level;
    if (isEndCorner)
        to = end.treeloc()[level];

    //! Processing of areas
    AreaPos area_from((isStartCorner)
                      ? ChainPos(chain, from).toAreaPos()
                      : AreaPos(chain->firstChunk(), 0));

    AreaPos area_to;
    if (isEndCorner)
        area_to = ChainPos(chain, to).toAreaPos();
    else {
        //! TODO: find the right way to operate with empty chains
        if (0 == chain->lastChunk()) {
            DBG(EV.TEST) << "inSelectionDo: chain not yet formatted\n";
            area_to = AreaPos();
        }
        else
            area_to = AreaPos(chain->lastChunk(),
                              chain->lastChunk()->chunkPosCount() - 1);
    }

    for (const Area* i = area_from.area(); i; i = i->nextChunk()) {
        long i_from = (i == area_from.area() && isStartCorner)
            ? (long)area_from.pos() : -1;
        long i_to = (i == area_to.area() && isEndCorner)
            ? (long)area_to.pos() : i->chunkPosCount();
        op.operator()(i, i_from, i_to);
        if (i == area_to.area())
            break;
    }
    //! Processing of child chains
    if (chain->hasChildChains()) {
        if (end.treeloc().length() - 1 == level && isEndCorner) {
            if (0 < to)
                to--;
            else
                return;
        }
        for (ulong pos = from; pos <= to; pos++) {
            const Chain* child = chain->chainAt(pos);
            if (child)
                inSelectionDo(child, level + 1,
                              (isStartCorner && pos == from),
                              (isEndCorner && pos == to),
                              start, end, op);
        }
    }
}

void EditableView::inSelectionDo(const TreeSelection& r, SelectionOp& op) const
{
    if (r.isEmpty())
        return;

    TreePos start(r.minLoc());
    TreePos end(r.maxLoc());

    ulong from = start.treeloc()[0];
    ulong to = end.treeloc()[0];

    for (ulong pos = from; pos <= to; pos++)
        inSelectionDo(rootArea_->chain()->chainAt(pos),
                      1, pos == from, pos == to, start, end, op);
}

////////////////////////////////////////////////////////////////

QRect TooltipView::tooltipRect() const
{
    return QRect(0, 0, 0, 0);
}
