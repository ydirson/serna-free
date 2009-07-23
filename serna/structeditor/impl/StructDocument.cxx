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
#include "structeditor/impl/debug_se.h"
#include "structeditor/StructDocument.h"
#include "structeditor/StructEditor.h"
#include "structeditor/impl/EditPolicyImpl.h"
#include "structeditor/SernaDragData.h"

#include "docview/MessageView.h"
#include "docview/MessageTree.h"
#include "docview/EventTranslator.h"
#include "docview/DocumentStateEventData.h"
#include "docview/PluginLoader.h"

#include "ui/UiStackItem.h"
#include "ui/ActionSet.h"
#include "ui/MimeHandler.h"

#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/GrovePosEventData.h"
#include "utils/Config.h"

#include "common/PathName.h"
#include "common/PropertyTree.h"
#include "common/XTreeIterator.h"
#include "common/CommandEvent.h"
#include "common/StringTokenizer.h"
#include "common/ScopeGuard.h"

#include "genui/StructDocumentActions.hpp"

USING_COMMON_NS;

class SetSrcPos;
class StructFinder;
class InsertElementItem;
class AttributeItem;
class StructGoToDocOrigin;

StructDocument::StructDocument(const CommandEventPtr& event,
                               const PropertyNode* dsi, Sui::Item* prevDoc,
                               const DocBuilder* builder)
    : SernaDoc(builder, prevDoc),
      MessageViewHolder(this)
{
    setDsi(const_cast<PropertyNode*>(dsi));

    PropertyNodePtr maker_props;

    /////////
    maker_props = addItemMaker(Sui::STRUCT_EDITOR, 0, Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);

    /////////
    maker_props = addItemMaker(Sui::MESSAGE_VIEW,
         new Sui::CustomItemMaker(Sui::makeUiItem<MessageView>),
         Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);

    /////////
    maker_props = addItemMaker(
        Sui::FINDER, new Sui::CustomItemMaker(Sui::makeUiItem<StructFinder>),
        Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);

    /////////
    maker_props = addItemMaker(
        Sui::INSERT_ELEMENT,
        new Sui::CustomItemMaker(Sui::makeUiItem<InsertElementItem>),
        Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);

    /////////
    maker_props = addItemMaker(
        Sui::CHANGE_ATTRIBUTES,
        new Sui::CustomItemMaker(Sui::makeUiItem<AttributeItem>),
        Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);
    maker_props->makeDescendant(Sui::IS_SINGLETON)->setBool(true);

    builder->buildActions(dispatcher(), actionSet());

    structEditor_ = new StructEditor(
        actionSet()->makeAction(Sui::STRUCT_EDITOR), this);
    setMessageTreeHolder(&*structEditor_);

    //! Loading available plugins
    pluginLoader().loadFor(NOTR("wysiwyg-mode"), this);
    String plugins = dsi->getSafeProperty("load-plugins")->getString();

    for (StringTokenizer st(plugins); st; )
        pluginLoader().load(st.next(), this);

    set_trace_tags();

    if (!event->execute(structEditor_.pointer()))
        throw int(0);

    if (getDsi()->getProperty("#has-xml-errors"))
        return;

    setNextContext(structEditor_.pointer());

    buildInterface();

    itemProps()->makeDescendant(Sui::ICON)->setString(NOTR("document"));

    PropertyNode* prop = dsi->getProperty(Sui::INSCRIPTION);
    if (prop)
        itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
            prop->getString());
    else
        itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
            DocSrcInfo::docPathFilename(dsi));

    itemProps()->makeDescendant(Sui::TOOLTIP)->setString(
        DocSrcInfo::makeDsiTooltip(dsi));

    DocumentStateEventData newDocEd(DocumentStateEventData::BUILD_INTERFACE);
    stateChangeFactory().dispatchEvent(&newDocEd);

    if (messageView()) {
        messageView()->subscribeToUpdates(structEditor_->viewUpdate());
        structEditor_->setNextContext(messageView());
        messageView()->gotoMessage().subscribe(
            makeCommand<StructGoToDocOrigin>, &*structEditor_);
    }
    structEditor_->postInit();
    register_struct_dnd_callbacks(this);
    showStageInfo();
}

StructDocument::~StructDocument()
{
    if (structEditor_->parent()) {
        structEditor_->remove();
        structEditor_ = 0;
    }
}

void StructDocument::grabFocus() const
{
    if (structEditor_->editPolicy())
        structEditor_->editPolicy()->focusInEvent(true);
    structEditor_->grabFocus();
}

bool StructDocument::restoreView(const String& restoreFrom)
{
    if (SernaDoc::restoreView(restoreFrom)) {
        // we cannot use ReloadStructDocument command event here because it
        // introduces dependency of structeditor on core
        static_cast<StructDocumentActions&>(uiActions()).
            reloadDocument()->dispatch();
        return true;
    }
    return false;
}

Sui::Item* StructDocument::findItem(const Sui::ItemPred& pred) const
{

    if (structEditor_) {
        if (pred(structEditor_.pointer()))
            return structEditor_.pointer();
        MessageView* mv = messageView(false);
        if (mv && pred(mv))
            return mv;
    }
    return SernaDoc::findItem(pred);
}

Sui::Item* StructDocument::makeItem(const String& itemClass,
                                   PropertyNode* properties) const
{
    if (Sui::STRUCT_EDITOR == itemClass)
        return structEditor_->parent() ? 0 : structEditor_.pointer();
    return SernaDoc::makeItem(itemClass, properties);
}

void StructDocument::updateTooltip()
{
    property(Sui::INSCRIPTION)->setString(
        DocSrcInfo::docPathFilename(getDsi()));
    property(Sui::TOOLTIP)->setString(
        DocSrcInfo::makeDsiTooltip(getDsi()));
}

bool StructDocument::canCloseDocument() const
{
    structEditor_->saveContextToDsi();
    return structEditor_->isModified();
}

class DoSaveStructDocWithDsi;

Common::CommandEventPtr StructDocument::makeSaveEvent(PropertyNode* dsi) const
{
    PropertyTreeEventData ed(dsi);
    return makeCommand<DoSaveStructDocWithDsi>(&ed);
}

static void template_desc(const PropertyNode* prop,
                          const String& path,
                          String* comment)
{
    using namespace DocSrcInfo;
    if (!comment)
        return;
    *comment = prop->getSafeProperty(TEMPLATE_CATEGORY)->getString() + "/" +
        prop->getSafeProperty(TEMPLATE_NAME)->getString();
    *comment += " (" + path + ")";
}

String StructDocument::getLevelFile(Level level, String* comment) const
{
    using namespace DocSrcInfo;

    switch (level) {
        case BUILTIN_LEVEL: {
            PathName path(config().getDataDir());
            path.append(NOTR("ui")).append(NOTR("StructDocument"));
            if (comment)
                *comment = tr("Builtin View", "SaveView for:");
            return path.name();
        }
        case ORIGINAL_TEMPLATE_LEVEL: {
            String template_path = getDsi()->getSafeProperty(
                TEMPLATE_PATH)->getString();
            if (!template_path.isEmpty()) {
                PathName path(template_path);
                String fn = path.dirname().name();
                fn += PathName::DIR_SEP;
                fn += path.basename();
                template_desc(getDsi(), path.filename(), comment);
                return fn;
            } else {
                PathName path(config().getDataDir());
                path.append(NOTR("ui")).append(NOTR("no_template_view"));
                if (comment)
                    *comment = tr("Document without Template", "SaveView for:");
                return path.name();
            }
            break;
        }
        case RECENT_DOCUMENT_LEVEL: {
            if (getDsi()->getProperty(IS_NEW_DOCUMENT))
                return String();
            PropertyNode* dsi_id =
                getDsi()->getProperty(DSI_ID);
            if (dsi_id && !dsi_id->getString().isEmpty()) {
                String recent_name = NOTR("recent_") + dsi_id->getString();
                PathName path(config().getConfigDir());
                path.append(NOTR("tmp")).append(recent_name);
                if (comment)
                    *comment = recent_name;
                return path.name();
            }
            break;
        }
        case TEMPLATE_LEVEL: {
            PropertyNode* template_path =
                getDsi()->getProperty(TEMPLATE_PATH);
            if (template_path && !template_path->getString().isEmpty()) {
                PathName path(template_path->getString());
                String basename = config().getConfigDir();
                basename += PathName::DIR_SEP;
                basename += path.basename();
                template_desc(getDsi(), path.filename(), comment);
                return basename;
            } else {
                PathName path(config().getConfigDir());
                path.append(NOTR("no_template_view"));
                if (comment)
                    *comment = tr("Document without Template", "SaveView for:");
                return path.name();
            }
        }
        default:
            break;
    }
    if (comment)
        *comment = String();
    return String();
}

class ExecuteStructUiCmd : public CommandEvent<StructEditor> {
public:
    ExecuteStructUiCmd(ActionExecutor* ue)
        : ue_(ue) {}
    virtual bool doExecute(StructEditor*, EventData*)
    {
        ue_->execute();
        return true;
    }
private:
    ActionExecutor* ue_;
};

class ExecuteSernaDocUiCmd : public CommandEvent<SernaDoc> {
public:
    ExecuteSernaDocUiCmd(ActionExecutor* ue)
        : ue_(ue) {}
    virtual bool doExecute(SernaDoc*, EventData*)
    {
        ue_->execute();
        return true;
    }
private:
    ActionExecutor* ue_;
};

CommandEventPtr StructDocument::makeUiEventExecutor(ActionExecutor* ue)
{
    if (structEditor_ && structEditor_->parent())
        return new ExecuteStructUiCmd(ue);
    return new ExecuteSernaDocUiCmd(ue);
}
