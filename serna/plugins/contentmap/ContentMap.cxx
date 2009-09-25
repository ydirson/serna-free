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
#include "csl/Engine.h"
#include "csl/Exception.h"

#include "ui/UiAction.h"
#include "ui/ActionSet.h"
#include "ui/UiItemSearch.h"

#include "formatter/Area.h"

#include "docview/dv_utils.h"
#include "docview/MessageTree.h"

#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "utils/SernaCatMgr.h"
#include "utils/SernaMessages.h"
#include "utils/GrovePosEventData.h"

#include "structeditor/StructDocument.h"
#include "structeditor/StructEditor.h"
#include "structeditor/SE_Pos.h"
#include "structeditor/SetCursorEventData.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "grove/Nodes.h"
#include "grove/Grove.h"

#include "xslt/Exception.h"
#include "xslt/ResultOrigin.h"
#include "xslt/Engine.h"

#include "editableview/EditableView.h"

#include "common/Url.h"

#include "contentmap/ContentMap.h"
#include "contentmap/qt/QtContentMap.h"

#include <iostream>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

// START_IGNORE_LITERALS
const char* CONTENT_MAP             = "ContentMap";
const char* CSL_STYLESHEET          = "csl-stylesheet";
const char* RESOLVED_CSL_STYLESHEET = "resolved-csl-stylesheet";
const char* FOCUS_POLICY_ACTION     = "ContentMap:contentMapFocusPolicy";
// STOP_IGNORE_LITERALS

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(UpdateContentMap, ContentMap)

bool UpdateContentMap::doExecute(ContentMap* contentMap, EventData*)
{
    contentMap->update();
    return true;
}

GROVEPOS_EVENT_IMPL(UpdateContextPos, ContentMap)

bool UpdateContextPos::doExecute(ContentMap* contentMap, EventData*)
{
    contentMap->setPendingPos(pos_);
    return true;
}

SELECTION_EVENT_IMPL(UpdateSelection, ContentMap)

bool UpdateSelection::doExecute(ContentMap* contentMap, EventData*)
{
    contentMap->updateSelection();
    return true;
}

/////////////////////////////////////////////////////////////////////////

using namespace Formatter;

static GrovePos find_nearest_result(const GrovePos& srcPos, 
                                    const XTreeIterator<Node>& foHint)
{
    GrovePos nearest_src = srcPos;
    while (!nearest_src.isNull()) {
        GrovePos result_pos = to_result_pos(
            nearest_src, XTreeIterator<Node>(foHint), false);
        if (!result_pos.isNull()) 
            return result_pos;
        if (GrovePos::ELEMENT_POS == nearest_src.type() && 
            nearest_src.before() && nearest_src.before()->prevSibling())
            nearest_src = GrovePos(nearest_src.node(), 
                                   nearest_src.before()->prevSibling());
        else
            nearest_src = GrovePos(nearest_src.node()->parent(), 
                                   nearest_src.node());
    }
    return GrovePos();
}

static void set_src_pos(StructEditor* se, GrovePos srcPos)
{
    if (srcPos.isNull() || !srcPos.node()->parent() || !srcPos.node()->grove())
        return;
    srcPos = adjust_to_text_pos(srcPos);
    
    AreaPos old_pos = se->editableView().context().areaPos();
    GrovePos fo_hint = se->editViewFoPos();
    Node* root = se->fot()->document()->documentElement();
    XTreeIterator<Node> root_fo_iter(
        xtree_iterator(GrovePos(root, root->firstChild())));

    GrovePos result_pos = find_nearest_result(
        srcPos, (fo_hint.isNull() ? root_fo_iter : ++xtree_iterator(fo_hint)));
    if (result_pos.isNull() && !fo_hint.isNull())
        result_pos = find_nearest_result(srcPos, root_fo_iter);

    AreaPos new_pos = se->toAreaPos(result_pos);
    if (srcPos != se->editViewSrcPos() || new_pos != old_pos) {
        se->removeSelection();
        se->setCursor(srcPos, new_pos, true);
    }
}

////////////////////////////////////////////////////////////////////////////

namespace Sui {
  COMMANDLESS_ITEM_MAKER(ContentMap, ContentMap)
}

ContentMap::ContentMap(PropertyNode* props)
    : LiquidItem(props),
      structEditor_(0)
{
}

ContentMapWidget* ContentMap::contentMapWidget() const 
{ 
    return contentMapWidget_; 
}

Csl::Engine* ContentMap::engine() const 
{
    return engine_.pointer();
}

void ContentMap::sendSrcPos(const GrovePos& pos)
{
    set_src_pos(structEditor_, pos);
    if (structEditor_ && focusPolicyAction()->getBool(Sui::IS_TOGGLED))
        structEditor_->grabFocus();
}

static ChainPos chain_pos(const GrovePos& pos, StructEditor* se)
{
    if (pos.before()) {
        Chain* chain = get_formatted_chain(
            find_result(pos.before(), fo_hint(se->editViewFoPos()), true),
            se->editableView());
        if (chain)
            return  ChainPos(chain->parentChain(), chain->chainPos());
    }
    if (pos.node()->lastChild()) {
        Chain* chain = get_formatted_chain(
            find_result(pos.node()->lastChild(), 
                        fo_hint(se->editViewFoPos()), true),
            se->editableView());
        if (chain)
            return  ChainPos(chain->parentChain(), chain->chainPos() + 1);
    }
    Chain* chain = get_formatted_chain(
        find_result(pos.node(), fo_hint(se->editViewFoPos()), true),
        se->editableView());
    if (chain)
        return  ChainPos(chain, chain->chainPosCount());

    return ChainPos();
}

void ContentMap::removeSelection()
{
    structEditor_->removeSelection();
}

void ContentMap::getSelection(GrovePos& from, GrovePos& to) const
{
    structEditor_->getSelection(from, to, StructEditor::SILENT_OP);
}

void ContentMap::extendSelection(const GrovePos& to)
{
    if (to.isNull())
        return;
    
    Selection selection = structEditor_->editableView().getSelection(true);
    ChainPos start(selection.tree_.start().toAreaPos(
                       structEditor_->editableView().rootArea()));
    ChainPos end(chain_pos(to, structEditor_));

    selection.src_.extendTo(to);
    structEditor_->setSelection(ChainSelection(start, end), selection.src_);
    
    if (focusPolicyAction()->getBool(Sui::IS_TOGGLED))
        structEditor_->grabFocus();

    IdleHandler::deregisterHandler(this);
    pendingPos_ = GrovePos();
}

void ContentMap::setSelection(const GrovePos& from, const GrovePos& to)
{
    if (from.isNull() || to.isNull())
        return;
    ChainPos start(chain_pos(from, structEditor_));
    ChainPos end(chain_pos(to, structEditor_));
    structEditor_->setSelection(ChainSelection(start, end),
                                GroveSelection(from, to));
    if (focusPolicyAction()->getBool(Sui::IS_TOGGLED))
        structEditor_->grabFocus();
}

void ContentMap::cutSelection(const GrovePos& from, const GrovePos& to)
{
    if (from.isNull() || to.isNull())
        return;
    if (structEditor_->isEditableEntity(from, 
        StructEditor::ANY_OP|StructEditor::NOROOT_OP) != StructEditor::POS_OK)
            return;
    structEditor_->executeAndUpdate(
        structEditor_->groveEditor()->cut(from, to));
}

void ContentMap::update()
{
    if (!engine_.isNull())
        engine_->update();
}

void ContentMap::setPendingPos(const GrovePos& src)
{
    pendingPos_ = src;
    IdleHandler::registerHandler(this);
}

void ContentMap::updateSelection()
{
    GrovePos from, to;
    structEditor_->getSelection(from, to, StructEditor::SILENT_OP);
    contentMapWidget_->showSelection(from, to);
}

bool ContentMap::processQuanta()
{
    IdleHandler::deregisterHandler(this);
    contentMapWidget_->setCurrentItem(pendingPos_);
    pendingPos_ = GrovePos();
    return false;
}

void ContentMap::grabFocus() const
{
    if (!contentMapWidget_->hasFocus())
        contentMapWidget_->setFocus();
    //contentMapWidget_->grabFocus();
}

class SetContentPos;

void ContentMap::init()
{
    //! Find StructEditor
    if (!engine_.isNull())
        return;
    Sui::Document* doc = dynamic_cast<Sui::Document*>(documentItem());
    structEditor_ = (doc) 
        ? dynamic_cast<StructEditor*>(
            doc->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR))) 
        : 0;
    if (!structEditor_)
        return;
    focusPolicyAction_ =
        doc->actionSet()->findAction(FOCUS_POLICY_ACTION);

    //! Resolve vars
    PropertyNode* dsi = structEditor_->getDsi();
    PropertyNode* csl_stylesheet = dsi->getProperty(CSL_STYLESHEET);
    PropertyNode* resolved_stylesheet =
        dsi->getProperty(RESOLVED_CSL_STYLESHEET);

    if (!resolved_stylesheet && csl_stylesheet) {
        String docpath = 
            dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
        String tmplpath =
            dsi->getSafeProperty(DocSrcInfo::TEMPLATE_PATH)->getString();
        PropertyTree localVars;
        if (!tmplpath.isEmpty())
            localVars.root()->makeDescendant(
                DocSrcInfo::TEMPLATE_DIR, Url(tmplpath)[Url::DIRPATH], true);
        
        resolved_stylesheet = dsi->makeDescendant(
            RESOLVED_CSL_STYLESHEET, config().resolveResource(
                CSL_STYLESHEET, csl_stylesheet->getString(),
                docpath, localVars.root()));
    }

    //! Initialize engine
    engine_ = Csl::Engine::make(structEditor_->messageTree(), 
                                structEditor_->uriMapper()->catalogManager());
    engine_->setNumberCache(structEditor_->xsltEngine()->numberCache());
    String stylesheet[] = {
        ((resolved_stylesheet) ? resolved_stylesheet->getString() : String()),
        config().root()->getString("vars/default_csl"),
        config().getDataDir() + 
            NOTR("/plugins/syntext/default/default-content-map.csl")
    };
    bool is_style_ok = false;
    for (uint i = 0; i < 3; i++) {
        if (stylesheet[i].isEmpty())
            continue;
        try {
            engine_->setStyle(stylesheet[i]);
            engine_->setInstanceMaker(make_qt_csl_instance);
            //engine_->transform(structEditor_->grove()->document());
            is_style_ok = true;
            break;
        }
        catch (CslException& e) {
            structEditor_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("ContentMap: Stylesheet Error"), 
                e.what(), tr("&Close"));
        }
    }
    if (!is_style_ok)
        return;

    buildChildren(&restore_ui_item);

    //! Subscribe to events
    structEditor_->viewUpdate().subscribe(
        makeCommand<UpdateContentMap>, this);
    structEditor_->elementContextChange().subscribe(
        makeCommand<UpdateContextPos>, this);
    structEditor_->selectionChange().subscribe(
        makeCommand<UpdateSelection>, this);
    return;
}

void ContentMap::showContextMenu(const QPoint& point)
{
    String menu_name = contentMapWidget_->contextMenuAt(point);
    if (menu_name.isEmpty()) {
        LiquidItem::showContextMenu(point);
    }
    else {
        Sui::Item* menu = findItem(Sui::ItemName(menu_name));
        if (menu) 
            menu->showContextMenu(point);
    }
}

////////////////////////////////////////////////////////////////////////////

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SetFocusToContentMap, ContentMapPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SelectContentMapItem, ContentMapPlugin)

ContentMapPlugin::ContentMapPlugin(SernaApiBase* doc,
                                   SernaApiBase* properties, char**)
    : DocumentPlugin(doc, properties)
{
    REGISTER_UI_EXECUTOR(SetFocusToContentMap);
    REGISTER_UI_EXECUTOR(SelectContentMapItem);
    buildPluginExecutors();

    PropertyNodePtr maker_props = sernaDoc()->addItemMaker(
        CONTENT_MAP, new Sui::CustomItemMaker(Sui::makeUiItem<ContentMap>), 
        Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);
}

void SetFocusToContentMap::execute()
{
    Sui::Item* content_map = 
        plugin()->sernaDoc()->findItem(Sui::ItemClass(CONTENT_MAP));
    if (content_map)
        content_map->grabFocus();   
}

void SelectContentMapItem::execute()
{
    Sui::Item* content_map = 
        plugin()->sernaDoc()->findItem(Sui::ItemClass(CONTENT_MAP));
    ContentMapWidget* widget = (content_map) 
        ? dynamic_cast<ContentMap*>(content_map)->contentMapWidget() : 0;
    if (widget) {
        Q3ListViewItem* item = widget->currentItem();
        if (item)
            widget->selectItem(item);
    }
}

DEFINE_PLUGIN_CLASS(ContentMapPlugin)
