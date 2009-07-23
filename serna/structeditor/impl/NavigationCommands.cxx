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
#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/SE_Pos.h"
#include "groveeditor/GrovePos.h"
#include "xslt/ResultOrigin.h"
#include "editableview/EditableView.h"
#include "docview/EventTranslator.h"

#include "formatter/AreaPos.h"

#include "common/CommandEvent.h"
#include "common/XTreeIterator.h"
#include "common/DiffuseSearch.h"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace Formatter;                  
    

///////////////////////////////////////////////////////////

static const Chain* find_original_chain(const Chain* chain)
{
    while (chain && chain->parentChain() &&
           (chain->hasParentOrigin() || LINE_FO == chain->type()))
        chain = chain->parentChain();
    return chain;
}

static bool goBesideElement(StructEditor* se, bool isBefore, bool doSelection)
{
    ChainPos chain_pos = se->editableView().context().areaPos();
    if (chain_pos.isNull())
        return false;

    const Chain* chain = chain_pos.chain();
    if (TEXT_FO == chain->type())
        chain = chain->parentChain();
    chain = find_original_chain(chain);
    //! Find position beside chain
    if (chain_pos.chain() == chain) {
        long pos = chain_pos.pos();
        chain_pos = ChainPos();
        if (isBefore) {
            if (0 < pos)
                chain_pos = ChainPos(chain, pos - 1);
        }
        else {
            if ((int(chain->chainPosCount()) - 1) > pos)
                chain_pos = ChainPos(chain, pos + 1);
        }
    }
    else
        chain_pos = ChainPos();
    if (chain_pos.isNull()) {
        chain = find_original_chain(chain);
        const Chain* parent_chain = chain->parentChain();
        if (0 == chain || 0 == parent_chain)
            return false;
        if (isBefore)
            chain_pos = ChainPos(parent_chain, chain->chainPos());
        else
            chain_pos = ChainPos(parent_chain, chain->chainPos() + 1);
    }
    //! Set position
    AreaPos area_pos(chain_pos.toAreaPos());
    area_pos.findAllowed(!isBefore, true);
    if (area_pos.isNull())
        return false;

    if (doSelection)
        se->extendSelectionTo(EditContext::getSrcPos(area_pos), area_pos);
    se->setCursor(area_pos, true);
    return true;
}

static bool checkSelection(const Common::EventData* ed)
{
    return (ed && !dynamic_cast<const UiEventData*>(ed));
}

COMMAND_EVENT_WITH_DATA_IMPL(GoToPrevElement, StructEditor)
COMMAND_EVENT_WITH_DATA_IMPL(GoToNextElement, StructEditor)

bool GoToPrevElement::doExecute(StructEditor* se, EventData*)
{
    return goBesideElement(se, true, checkSelection(ed_));
}

bool GoToNextElement::doExecute(StructEditor* se, EventData*)
{
    return goBesideElement(se, false, checkSelection(ed_));
}

///////////////////////////////////////////////////////////

static bool goToElementCorner(StructEditor* se,
                              bool isToStart, bool doSelection)
{
    using namespace Formatter;

    ChainPos chain_pos = se->editableView().context().areaPos();
    if (chain_pos.isNull())
        return false;

    const Chain* chain = chain_pos.chain();
    if (TEXT_FO == chain->type())
        chain = chain->parentChain();
    chain = find_original_chain(chain);

    if (isToStart)
        chain_pos = ChainPos(chain, 0);
    else
        chain_pos = ChainPos(chain, chain->chainPosCount() - 1);

    AreaPos area_pos(chain_pos.toAreaPos());
    area_pos.findAllowed(isToStart, true);
    if (area_pos.isNull())
        return false;

    if (doSelection)
        se->extendSelectionTo(EditContext::getSrcPos(area_pos), area_pos);
    se->setCursor(area_pos, true);
    return true;
}

COMMAND_EVENT_WITH_DATA_IMPL(GoToElementStart, StructEditor)
COMMAND_EVENT_WITH_DATA_IMPL(GoToElementEnd, StructEditor)

bool GoToElementStart::doExecute(StructEditor* se, EventData*)
{
    return goToElementCorner(se, true, checkSelection(ed_));
}

bool GoToElementEnd::doExecute(StructEditor* se, EventData*)
{
    return goToElementCorner(se, false, checkSelection(ed_));
}

///////////////////////////////////////////////////////////////////////////

COMMAND_EVENT_WITH_DATA_IMPL(GoToNextChoice, StructEditor)

class ChoiceOriginCheck {
public:
    bool operator()(const XTreeIterator<Node>& foIter) const
    {
        const Node* node_origin = Xslt::resultOrigin(foIter.node());
        return (node_origin && Node::CHOICE_NODE == node_origin->nodeType());
    }
};

class InsertElement;

bool GoToNextChoice::doExecute(StructEditor* se, EventData*)
{
    AreaPos old_pos = se->editableView().context().areaPos();
    GrovePos fo_hint = se->editViewFoPos();
    Node* root = se->fot()->document()->documentElement();
    XTreeIterator<Node> root_fo_iter(
        xtree_iterator(GrovePos(root, root->firstChild())));

    XTreeIterator<Node> fo_iter = find_forward<Node, ChoiceOriginCheck>(
        fo_hint.isNull() ? root_fo_iter : ++xtree_iterator(fo_hint),
        ChoiceOriginCheck(), true);
    if (!fo_iter.node() && !fo_hint.isNull())
        fo_iter = find_forward<Node, ChoiceOriginCheck>(
            root_fo_iter, ChoiceOriginCheck(), true);
    if (!fo_iter.node())
        return false;

    AreaPos new_pos = se->toAreaPos(GrovePos(fo_iter.node(), 
                                             fo_iter.before()));
    if (new_pos != old_pos) {
        se->removeSelection();
        se->setCursor(new_pos, true);
        makeCommand<InsertElement>()->execute(se);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////

#if 0
# include <iostream>
SIMPLE_COMMAND_EVENT_IMPL(TestKey, StructEditor)
bool TestKey::doExecute(StructEditor* se, EventData*)
{
    std::cerr << "HI!!!!\n";
}
#endif 
