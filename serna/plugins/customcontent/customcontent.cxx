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
#include "ui/UiItem.h"
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"
#include "ui/IconProvider.h"
#include "ui/ActionSet.h"

#include "proputils/PropertyTreeSaver.h"
#include "utils/GrovePosEventData.h"
#include "utils/tr.h"

#include "docview/SernaDoc.h"
#include "docview/Clipboard.h"

#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "editableview/EditableView.h"

#include "debug_cc.h"
#include "customcontent.h"
#include "CustomContentDialog.h"
#include "CustomContentListDialog.h"
#include <qapplication.h>

namespace ContentProps
{
// START_IGNORE_LITERALS
    const char* const SCC_ROOT      = "serna-custom-content";
    const char* const CONTENT_ITEM  = "custom-content";
    const char* const NAME          = Sui::NAME;
    const char* const INSCRIPTION   = Sui::INSCRIPTION;
    const char* const ICON          = Sui::ICON;
    const char* const HOT_KEY       = Sui::ACCEL;

    const char* const CONTEXT_SPECS = "context-specs";
    const char* const CONTENT       = "content";
    const char* const PATTERN_STR   = "pattern";
    const char* const XPATH_LOCATION= "xpath-location";
    const char* const REL_LOCATION  = "relative-location";
    const char* const TEXT          = "text";
    const char* const MAKE_TOOL_BUTTON  = "make-tool-button";

    const char* const INSERT_BEFORE = "Insert Before:";
    const char* const INSERT_AFTER  = "Insert After:";
    const char* const APPEND_TO     = "Append To:";

    const char* const SUFFIX        = ".scc";
}

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace ContentProps;

RelativeLocation location(const String& str)
{
    if ("after" == str)
        return INSERT_AFTER_;
    if ("before" == str)
        return INSERT_BEFORE_;
    if ("append" == str)
        return APPEND_TO_;
    return CURRENT_POS_;
}

const char* location_string(RelativeLocation l)
{
    switch (l) {
        case INSERT_AFTER_ :
            return "after";
        case INSERT_BEFORE_ :
            return "before";
        case APPEND_TO_ :
            return "append";
        default:
            return "";
    }
    return "";
}

static void adjust_action_props(PropertyNode* pn)
{
    String tooltip = pn->getString(Sui::TOOLTIP);
    if (tooltip.isEmpty()) {
        tooltip = pn->getString(INSCRIPTION);
        tooltip.replace("&", "");
        pn->makeDescendant(Sui::TOOLTIP, tooltip, true);
    }
    pn->makeDescendant("commandEvent", NOTR("InsertCustomContent"), true);
    pn->makeDescendant(Sui::TRANSLATION_CONTEXT, NOTR("scc"), false);
}

// STOP_IGNORE_LITERALS

const String location_inscription(RelativeLocation l)
{
    switch (l) {
        case INSERT_AFTER_ :
            return qApp->translate("CustomContent", "After");
        case INSERT_BEFORE_ :
            return qApp->translate("CustomContent", "Before");
        case APPEND_TO_ :
            return qApp->translate("CustomContent", "Append");
        default:
            return qApp->translate("CustomContent", "Current");
    }
    return "";
}

class CustomItem : public Sui::ItemPred {
public:
    CustomItem(const Sui::Action* action, const String& itemClass)
        : action_(action),
          itemClass_(itemClass) {}

    virtual bool        operator()(const Sui::Item* item) const
    {
        return (item->action() == action_ && item->itemClass() == itemClass_);
    }
private:
    const Sui::Action*   action_;
    String              itemClass_;
};

// START_IGNORE_LITERALS
const char* const SCC_TOOLBAR       = "customContentToolbar";
const char* const SCC_MENU          = "customContentSubmenu";
const char* const CONTENT_SEPARATOR = "customContentSeparator";
// STOP_IGNORE_LITERALS

//////////////////////////////////////////////////////////////////////////

PropertyNode* CustomContentPropMaker::makeProperty()
{
    PropertyNode* content_list = new PropertyNode(SCC_ROOT);
    for (ActionIterator i = actionList_.begin(); i != actionList_.end(); i++) {
        PropertyNode* action_props = (*i)->properties();
        PropertyNode* content = new PropertyNode(CONTENT_ITEM);
        content->appendChild(action_props->getSafeProperty(NAME)->copy());
        content->appendChild(action_props->getSafeProperty(
                                 INSCRIPTION)->copy());
        content->appendChild(action_props->getSafeProperty(ICON)->copy());
        content->appendChild(action_props->getSafeProperty(HOT_KEY)->copy());
        content->appendChild(
            action_props->getSafeProperty(CONTEXT_SPECS)->copy(true));
        content_list->appendChild(content);
        content->appendChild(
            action_props->getSafeProperty(MAKE_TOOL_BUTTON)->copy());
    }
    return content_list;
}

//////////////////////////////////////////////////////////////////////////

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(NewCustomContent, CustomContentPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(EditCustomContent, CustomContentPlugin)

class InsertCustomContent;

CustomContentPlugin::CustomContentPlugin(SernaApiBase* doc,
                                         SernaApiBase* properties,
                                         char**)
    : DocumentPlugin(doc, properties),
      se_(0)
{
    REGISTER_UI_EXECUTOR(NewCustomContent);
    REGISTER_UI_EXECUTOR(EditCustomContent);
    REGISTER_UI_EXECUTOR(InsertCustomContent);
    buildPluginExecutors();

    sccRoot_ = registerPropTree(SUFFIX, SCC_ROOT);
    for (PropertyNode* p = sccRoot_->firstChild(); p;) {
        if (CONTENT_ITEM == p->name() && p->getProperty(CONTEXT_SPECS)) {
            PropertyNodePtr action_prop = p->copy(true);
            adjust_action_props(&*action_prop);
            ActionExecutor* executor = 
                buildPluginExecutor(action_prop.pointer());
            if (executor) {
                actionList_.push_back(executor->uiAction());
                executor->uiAction()->properties()->makeDescendant(
                    MAKE_TOOL_BUTTON, "true", false);
            }
            p = p->nextSibling();
        } else {
            PropertyNode* next = p->nextSibling();
            p->remove(); // clean up junk
            p = next;
        }
    }
}

void CustomContentPlugin::buildPluginInterface()
{
    //! Make items for custom content
    DocumentPlugin::buildPluginInterface();
    Sui::Item* toolbar = sernaDoc()->findItem(Sui::ItemName(SCC_TOOLBAR));
    if (toolbar)
        toolSeparator_ = toolbar->findItem(Sui::ItemName(CONTENT_SEPARATOR));
    Sui::Item* menu = sernaDoc()->findItem(Sui::ItemName(SCC_MENU));
    if (menu)
        menuSeparator_ = menu->findItem(Sui::ItemName(CONTENT_SEPARATOR));
    for (ActionIterator i = actionList_.begin(); i!= actionList_.end(); i++)
        makeContentItems(toolbar, menu, (*i).pointer());
}

void CustomContentPlugin::postInit()
{
    se_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
}

void CustomContentPlugin::saveProperties(const String& path)
{
    CustomContentPropMaker prop_maker(actionList());
    PropertyNodePtr prop = prop_maker.makeProperty();
    sccRoot_->removeAllChildren();
    while (prop->firstChild()) {
        PropertyNodePtr child = prop->firstChild();
        child->remove();
        sccRoot_->appendChild(child.pointer());
    }
    DocumentPlugin::saveProperties(path);
}

static bool make_item(Sui::Item* parent, const String& itemClass,
                      const Sui::Action* action, Sui::ItemPtr& separator,
                      const String& separatorClass, Sui::Document* doc)
{
    String action_name = action->get(Sui::NAME);
    if (!parent || doc->findItem(CustomItem(action, itemClass)))
        return false;
    String item_name = action_name + itemClass;
    PropertyNodePtr item_props = new PropertyNode(Sui::ITEM_PROPS);
    item_props->makeDescendant(Sui::NAME)->setString(item_name);
    item_props->makeDescendant(Sui::ACTION)->setString(action_name);
    Sui::ItemPtr item = doc->makeItem(itemClass, item_props.pointer());
    if (!item.isNull()) {
        if (separator.isNull()) {
            PropertyNodePtr s_props = new PropertyNode(Sui::ITEM_PROPS);
            s_props->makeDescendant(Sui::NAME)->setString(
                CONTENT_SEPARATOR);
            separator = doc->makeItem(separatorClass, s_props.pointer());
            parent->appendChild(separator.pointer());
        }
        parent->appendChild(item.pointer());
        return true;
    }
    return false;
}


void CustomContentPlugin::makeContentItems(Sui::Item* toolbar, Sui::Item* menu,
                                           const Sui::Action* action)
{
    if (!action)
        return;
    if (action->getBool(MAKE_TOOL_BUTTON))
        make_item(toolbar, Sui::TOOL_BUTTON, action, toolSeparator_, 
                  Sui::TOOL_BAR_SEPARATOR, sernaDoc());
    else {
        Sui::Item* button = 
            toolbar->findItem(CustomItem(action, Sui::TOOL_BUTTON));
        if (button)
            button->removeItem();
    }
    make_item(menu, Sui::MENU_ITEM, action, menuSeparator_, 
              Sui::MENU_SEPARATOR, sernaDoc());
}

void CustomContentPlugin::makeContentItems(const Sui::Action* action)
{
    Sui::Item* toolbar =
        sernaDoc()->findItem(Sui::ItemName(SCC_TOOLBAR));
    Sui::Item* menu = sernaDoc()->findItem(Sui::ItemName(SCC_MENU));
    makeContentItems(toolbar, menu, action);
    if (toolbar)
        toolbar->attach(true);
    if (menu) 
        menu->attach(true);
}

Sui::Action* CustomContentPlugin::addContent(PropertyNode* content)
{
    if (CONTENT_ITEM != content->name())
        return 0;
    adjust_action_props(content);
    ActionExecutor* executor = buildPluginExecutor(content);
    if (!executor)
        return 0;
    Sui::Action* action = executor->uiAction();
    actionList_.push_back(action);
    makeContentItems(action);
    return action;
}

void CustomContentPlugin::deleteContent(Sui::Action* action)
{
    if (action) {
        se_->sernaDoc()->actionSet()->removeAction(action);
        // TODO: action->remove();
        for (ActionList::iterator i = actionList_.begin();
             i != actionList_.end(); i++) {
            if ((*i).pointer() == action) {
                actionList_.remove(*i);
                break;
            }
        }
    }
    if (actionList_.empty()) {
        if (!toolSeparator_.isNull()) {
            toolSeparator_->removeItem();
            toolSeparator_ = 0;
        }
        if (!menuSeparator_.isNull()) {
            menuSeparator_->removeItem();
            menuSeparator_ = 0;
        }
    }
}

bool CustomContentPlugin::processQuanta()
{
    IdleHandler::deregisterHandler(this);
    return false;
}

bool CustomContentPlugin::isValidName(const Common::String& name) const
{
    return (!name.isEmpty() &&
            !se_->sernaDoc()->actionSet()->findAction(name));
}

String CustomContentPlugin::getValidName(const Common::String& name) const
{
    for (int suffix = 1; suffix < 10000; suffix++) {
        String new_name = name + '_' + String::number(suffix);
        if (isValidName(new_name))
            return new_name;
    }
    return String();
}


DEFINE_PLUGIN_CLASS(CustomContentPlugin)

////////////////////////////////////////////////////////////////////////////

class StructCopy;

void NewCustomContent::execute()
{
    StructEditor* se = plugin()->structEditor();
    if (!se)
        return;
    String data;
    String name = plugin()->getValidName(NOTR("customContent"));
    //! Getting selected content
    if (!se->editableView().getSelection().src_.isEmpty()) {
        if (!makeCommand<StructCopy>()->execute(plugin()->sernaDoc()))
            return;
        const DocumentFragment* fragment = 
            serna_clipboard().getStructClipboard(false);
        if (fragment->firstChild()) {
            if (Node::TEXT_NODE != fragment->firstChild()->nodeType())
                name = fragment->firstChild()->nodeName();
            fragment->saveAsXmlString(data, Grove::GS_SAVE_CONTENT|
                (se->stripInfo() ? Grove::GS_INDENT : 0), se->stripInfo());
            data.replace(NOTR("&quot;"),NOTR("\"")).replace(NOTR("&apos;"),NOTR("'")).
                replace(NOTR("&amp;"),NOTR("&")).replace(NOTR("&lt;"),NOTR("<")).replace(NOTR("&gt;"),NOTR(">"));
        }
    }
    //! Making custom content property
    PropertyNodePtr content = new PropertyNode(CONTENT_ITEM);
    String inscription = tr("Insert %0").arg(name);
    content->makeDescendant(NAME)->setString(name);
    content->makeDescendant(INSCRIPTION)->setString(inscription);
    content->makeDescendant(Sui::TOOLTIP)->setString(inscription);
    content->makeDescendant(ICON)->setString(String(name).
        replace(NOTR("customContent"), NOTR("custom_content")));
    content->makeDescendant(CONTEXT_SPECS + String("/") +
                            CONTENT + String("/") + TEXT)->setString(data);
    content->makeDescendant(MAKE_TOOL_BUTTON)->setBool(true);
    //! Making ui controls
    if (QDialog::Accepted == CustomContentDialog(
            plugin(), content.pointer(), se->sernaDoc()->widget(0), se).exec())
        plugin()->addContent(content.pointer());
}

void EditCustomContent::execute()
{
    if (StructEditor* se = plugin()->structEditor())
        CustomContentListDialog(se, plugin()).exec();
}


