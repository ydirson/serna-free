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
#include "docview/dv_utils.h"

#include "structeditor/InsertElementItem.h"
#include "common/PropertyTreeEventData.h"
#include "utils/GrovePosEventData.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "utils/Properties.h"

#include "ui/UiItemSearch.h"

#include "grove/ElementMatcher.h"
#include "grove/Nodes.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "structeditor/StructEditor.h"
#include "structeditor/StructDocument.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/SetCursorEventData.h"
#include "structeditor/GroveCommandEventData.h"
#include "structeditor/ElementList.h"
#include "editableview/EditableView.h"

#include <set>
#include <map>
#include <iostream>

using namespace Common;
using namespace AttributesSpace;

namespace InsertElementProps {
// START_IGNORE_LITERALS
    const char* const ELEMENT_LIST      = "element_list";
    const char* const OTHER_ELEMENTS    = "other_elements";
    const char* const RECENT_ELEMENTS   = "recent_elements";
    const char* const CAPTION           = "caption";
// STOP_IGNORE_LITERALS
}

using namespace InsertElementProps;

class RecentElementsCache : public std::map<String, int> {
public:
    RecentElementsCache() 
        : seq_(-1) {}

    void update(const String& name)
    {
        (*this)[name] = seq_--;
    }
    uint find_recent(const String& name)
    {
        const_iterator it = find(name);
        if (it == end())
            return 0;
        return it->second;
    }

private:
    int         seq_;
};

RecentElementsCache& StructEditor::recentElementsCache() 
{
    if (recentElementsCache_.isNull())
        recentElementsCache_ = new RecentElementsCache;
    return *recentElementsCache_;
}

GroveLib::ElementMatcher& StructEditor::elementMatcher() 
{
    if (elementMatcher_.isNull())
        elementMatcher_ = new GroveLib::ElementMatcher(
            getDsi()->getSafeProperty(DocSrcInfo::HIDE_ELEMENTS)->getString());
    return *elementMatcher_;
}

//////////////////////////////////////////////////////////////////////////

GROVEPOS_EVENT_IMPL(InsertElementContextChange, InsertElementItem);

bool InsertElementContextChange::doExecute(InsertElementItem* item,
                                           EventData*)
{
    item->notifyPositionChange(pos_);
    return true;
}

SELECTION_EVENT_IMPL(InsertElementSelectionChange, InsertElementItem)

bool InsertElementSelectionChange::doExecute(InsertElementItem* item,
                                             EventData*)
{
    item->setSelectionChanged(!groveSelection_.isEmpty());
    return true;
}

///////////////////////////////////////////////////////////////////////

InsertElementUtils::InsertElementUtils(StructEditor* se, bool rename)
    : structEditor_(se),
      rename_(rename)
{
}

class ExecuteAndUpdate;

bool InsertElementUtils::doInsert(const Common::String& name)
{
    GroveEditor::GrovePos pos, to;
    bool has_selection =
        !structEditor_->editableView().getSelection().src_.isEmpty();
    if (has_selection) {
        if (StructEditor::POS_OK != structEditor_->getSelection(pos, to, 
                                                   StructEditor::STRUCT_OP))
            return false;
    } 
    else {
        if (!structEditor_->getCheckedPos(pos, StructEditor::STRUCT_OP))
            return false;
    }
    if (pos.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
        pos = GroveEditor::GrovePos(pos.node()->parent(), pos.node());
    structEditor_->recentElementsCache().update(name);
    GroveCommandEventData gcmd(
        (has_selection)
        ? structEditor_->groveEditor()->tagRegion(
            pos, to, name, 0/*existing_attrs*/)
        : structEditor_->groveEditor()->insertElement(
            pos, name, 0 /*existing_attrs*/));
    return makeCommand<ExecuteAndUpdate>(&gcmd)->execute(structEditor_);
}

void InsertElementUtils::loadElementList(PropertyNode* root,
                                         const GroveEditor::GrovePos& pos)
{
    typedef ElementList::PtnSet PtnSet;
    root->removeAllChildren();
    if (structEditor_->isEditableEntity(pos, StructEditor::SILENT_OP|
        StructEditor::STRUCT_OP) != StructEditor::POS_OK)
            return;
    PropertyNode* elements = root->makeDescendant(OTHER_ELEMENTS);
    PropertyNode* recent   = root->makeDescendant(RECENT_ELEMENTS);
    const ElementList::PtnSet* element_list = 0;
    ElementList::PtnSet renameSet;
    GroveLib::Element* elem = traverse_to_element(pos);
    RecentElementsCache& rcache = structEditor_->recentElementsCache();
    GroveLib::ElementMatcher& elemMatcher = structEditor_->elementMatcher();

    if (structEditor_->schema()) {
        ElementList* eli = &structEditor_->elementList();
        eli->setModified();
        eli->reload();
        if (rename_) {
            eli->getRenameSet(renameSet);
            element_list = &renameSet;
        } else
            element_list = &structEditor_->elementList();
    } else {
        typedef std::map<int, String> RecentMap;
        RecentMap rmap;
        RecentElementsCache::const_iterator rci = rcache.begin();
        for (; rci != rcache.end(); ++rci)
            rmap[rci->second] = rci->first;
        RecentMap::iterator ri = rmap.begin();
        for (uint i = 0; ri != rmap.end(); ++ri, ++i) {
            if (i < 5)
                recent->appendChild(new PropertyNode(ri->second));
            else
                elements->appendChild(new PropertyNode(ri->second));
        }
        return;
    }
    typedef std::map<int, PropertyNodePtr> RecentMap;
    RecentMap rmap;
    PtnSet::const_iterator ni = element_list->begin();
    for (; ni != element_list->end(); ++ni) {
        uint seq = rcache.find_recent((*ni)->name());
        if (seq)
            rmap[seq] = *ni;
    }
    typedef std::set<const PropertyNode*> OmitSet;
    OmitSet oset;
    RecentMap::iterator ri = rmap.begin();
    for (uint i = 0; ri != rmap.end() && i < 5; ++ri, ++i) {
        ri->second->remove();
        recent->appendChild(ri->second.pointer());
        oset.insert(ri->second.pointer());
    }
    for (ni = element_list->begin(); ni != element_list->end(); ++ni) {
        if (elemMatcher.matchElement(elem, (*ni)->name()))
            continue;
        if (oset.find(ni->pointer()) == oset.end()) {
            (*ni)->remove();
            elements->appendChild(ni->pointer());
        }
    }
}

InsertElementUtils::~InsertElementUtils()
{
}

//////////////////////////////////////////////////////////////////////////

InsertElementItem::InsertElementItem(Sui::Action*, PropertyNode* props)
    : LiquidItem(props), 
      myWidget_(0),
      selectionState_(-1), 
      hasCdata_(false)
{
    itemProps()->makeDescendant("caption")->setString(tr("Insert Element"));
}

StructEditor* InsertElementItem::structEditor() const
{
    if (!utils_.isNull())
        return utils_->structEditor();
    return 0;
}

void InsertElementItem::doInsert(const Common::String& elemName)
{
    if (!utils_.isNull())
        utils_->doInsert(elemName);
}

void InsertElementItem::notifyPositionChange(const GroveEditor::GrovePos& pos)
{
    StructEditor* se = structEditor();
    if (0 == se || !se->editableView().getSelection().src_.isEmpty())
        return;
    pendingPos_ = pos;
    IdleHandler::registerHandler(this);
}

void InsertElementItem::setSelectionChanged(bool hasSelection)
{
    bool state_changed = (char)hasSelection != selectionState_;
    selectionState_ = hasSelection;
    bool hasCdata = false;
    StructEditor* se = structEditor();
    if (0 == se)
        return;
    if (hasSelection) {
        GroveEditor::GrovePos from, to;
        if (StructEditor::POS_OK == se->getSelection(
                from, to, (StructEditor::SILENT_OP|StructEditor::STRUCT_OP))) {
            if (from.type() == GroveEditor::GrovePos::TEXT_POS &&
                from.idx() < int(from.text()->data().length()))
                    hasCdata = true;
            if (to.type() == GroveEditor::GrovePos::TEXT_POS && to.idx())
                hasCdata = true;
            if (!from.compareElemPos(pendingPos_)) {
                pendingPos_ = from;
                state_changed = true;
                IdleHandler::registerHandler(this);
            }
        }
    }
    else {
        GroveEditor::GrovePos pos;
        utils_->structEditor()->getCheckedPos(pos, 
            StructEditor::SILENT_OP|StructEditor::STRUCT_OP);
        if (!pendingPos_.compareElemPos(pos)) {
            pendingPos_ = pos;
            IdleHandler::registerHandler(this);
            state_changed = true;
        }
        hasCdata_ = false;
    }
    if (hasCdata_ != hasCdata) {
        state_changed = true;
        hasCdata_ = hasCdata;
    }
    if (state_changed && myWidget_)
        myWidget_->updateSelectionState(hasSelection, hasCdata_);
}

class InsertElementSelectionChange;
class InsertElementContextChange;

void InsertElementItem::inserted()
{
    Sui::Item* doc = documentItem();
    if (0 == doc)
        return;
    StructEditor* se = dynamic_cast<StructEditor*>(
        doc->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    if (0 == se)
        return;
    utils_ = new InsertElementUtils(se, false);
    se->selectionChange().subscribe(
        makeCommand<InsertElementSelectionChange>, this);
    se->elementContextChange().subscribe(
        makeCommand<InsertElementContextChange>, this);
}

void InsertElementItem::init(InsertElementNotifications* myWidget)
{
    myWidget_ = myWidget;
    if (0 == utils_)
        return;
    setSelectionChanged(
        !utils_->structEditor()->editableView().getSelection().src_.isEmpty());
}

void InsertElementItem::removed()
{
    unsubscribeFromAll();
    utils_ = 0;
}

bool InsertElementItem::processQuanta()
{
    IdleHandler::deregisterHandler(this);
    if (widget(0)) 
        updateElementList();
    return false;
}

void InsertElementItem::updateElementList()
{
    if (utils_.isNull())
        return;
    PropertyTree elemList;
    utils_->loadElementList(elemList.root(), pendingPos_);
    if (myWidget_) {
        myWidget_->setElementList(elemList.root());
        myWidget_->updateElementList();
    }
    pendingPos_ = GroveEditor::GrovePos();
}

InsertElementItem::~InsertElementItem()
{
}

InsertElementNotifications::~InsertElementNotifications()
{
    if (item_)
        item_->myWidget_ = 0;
}

//////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(InsertElement, StructEditor)

bool InsertElement::doExecute(StructEditor* se, EventData*)
{
    InsertElementItem* item = dynamic_cast<InsertElementItem*>
        (se->sernaDoc()->findItem(Sui::ItemClass(Sui::INSERT_ELEMENT)));
    if (item) {
        item->grabFocus();
        return true;
    }
    PropertyNode* item_props = new PropertyNode(Sui::ITEM_PROPS);
    item_props->makeDescendant(Sui::NAME, Sui::INSERT_ELEMENT);
    item = new InsertElementItem(0, item_props);
    se->sernaDoc()->appendChild(item);
    item->buildChildren(&restore_ui_item);
    item->attach();
    item->grabFocus();
    return true;
}

//////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(RenameElement, StructEditor)

class RenameElementWidget;

bool RenameElement::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos pos;
    if (!se->getCheckedPos(pos,
        StructEditor::STRUCT_OP|StructEditor::PARENT_OP))
            return false;
    GroveLib::Element* elem = traverse_to_element(pos);
    if (!elem)
        return false;
    if (elem->parent()->nodeType() != GroveLib::Node::ELEMENT_NODE) {
        se->sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
            tr("Cannot rename element"),
            tr("Cannot rename root element"), tr("&Ok")); 
        return false;
    }
    InsertElementUtils ibase(se, true);
    PropertyTreeEventData ed;
    ibase.loadElementList(ed.root(), pos);
    ed.root()->makeDescendant(CAPTION,
        tr("Rename Element <%1>").arg(elem->nodeName()));
    if (!makeCommand<RenameElementWidget>(&ed)->execute(se, &ed))
        return false;
    String name = ed.root()->getSafeProperty(
        ElementSpace::ELEMENT_NAME)->getString();
    if (name.isEmpty())
        return false;
    return se->executeAndUpdate(se->groveEditor()->renameElement(pos, name));
}

namespace Sui {
    CUSTOM_ITEM_MAKER(InsertElementItem, InsertElementItem)
}

/////////////////////////////////////////////////////////////////////////
