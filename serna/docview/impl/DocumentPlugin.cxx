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
#include "docview/dv_defs.h"
#include "docview/dv_utils.h"
#include "docview/impl/debug_dv.h"
#include "docview/DocumentPlugin.h"
#include "docview/DocumentStateEventData.h"
#include "docview/PluginLoader.h"
#include "docview/MessageView.h"

#include "common/StringTokenizer.h"

#include "ui/UiItem.h"
#include "ui/UiItemSearch.h"
#include "ui/UiAction.h"
#include "ui/ActionSet.h"
#include "ui/IconProvider.h"

#include "common/PropertyTreeEventData.h"
#include "utils/SernaMessages.h"
#include "utils/SernaUiItems.h"

#include "grove/Node.h"
#include "grove/Origin.h"

#include <set>
#include <map>

using namespace Common;

class DocumentPlugin::ExecutorMakerMap
    : public std::map<String, PluginUiExecutorMaker> {};
class DocumentPlugin::ExecutorMap
    : public std::map<String, ActionExecutor*> {};

static void add_plugin_origin(PropertyNode* pn, const String& name)
{
    pn->makeDescendant(Sui::PLUGIN_ORIGIN)->setString(name);
}

static void set_plugin_origin_recursive(PropertyNode* pn, const String& name)
{
    if (pn->name() == Sui::ITEM_PROPS)
        return add_plugin_origin(pn, name);
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling())
        set_plugin_origin_recursive(pn, name);
}

////////////////////////////////////////////////////////////////////////////

PluginUiExecutorBase::~PluginUiExecutorBase()
{
    if (uiAction())
        docPlugin_->sernaDoc()->actionSet()->removeAction(uiAction());
}

////////////////////////////////////////////////////////////////////////////

class PluginDocStateChangeEvent : public CommandEvent<DocumentPlugin> {
public:
    PluginDocStateChangeEvent(const EventData* ed)
        : ed_(ed) {}

    virtual bool doExecute(DocumentPlugin* plugin, EventData*)
    {
        const DocumentStateEventData* event_data =
            dynamic_cast<const DocumentStateEventData*>(ed_);
        if (0 == event_data)
            return false;
        switch (event_data->state_) {
            case DocumentStateEventData::NEW_DOCUMENT_GROVE: {
                DBG(DV.PLUGIN) << "DocumentPlugin: newdocgrove received\n";
                plugin->newDocumentGrove();
                return true;
            }
            case DocumentStateEventData::BEFORE_TRANSFORM: {
                DBG(DV.PLUGIN) << "DocumentPlugin: before-transform received\n";
                plugin->beforeTransform();
                return true;
            }
            case DocumentStateEventData::POST_INIT: {
                DBG(DV.PLUGIN) << "DocumentPlugin: postinit received\n";
                plugin->postInit();
                return true;
            }
            case DocumentStateEventData::BUILD_INTERFACE: {
                DBG(DV.PLUGIN) << "DocumentPlugin: build interface\n";
                plugin->buildPluginInterface();
                return true;
            }
            case DocumentStateEventData::SAVE_PROPERTIES: {
                DBG(DV.PLUGIN) << "DocumentPlugin: save properties\n";
                const PropertyNodePtr& path = event_data->prop_;
                if (!path.isNull() && !path->getString().isEmpty())
                    plugin->saveProperties(path->getString());
                return true;
            }
            case DocumentStateEventData::RESTORE_PROPERTIES: {
                DBG(DV.PLUGIN) << "DocumentPlugin: restore properties\n";
                if (event_data->prop_.isNull())
                    return false;
                plugin->restoreProperties(event_data->prop_.pointer());
                return true;
            }
            case DocumentStateEventData::GET_PROP_TREE_SUFFIXES: {
                if (event_data->prop_.isNull())
                    return false;
                plugin->getPropTreeSuffixes(event_data->prop_.pointer());
                return true;
            }
            case DocumentStateEventData::ABOUT_TO_SAVE: {
                DBG(DV.PLUGIN) << "DocumentPlugin: about-to-save\n";
                plugin->aboutToSave();
                return true;
            }
            case DocumentStateEventData::PRE_CLOSE: {
                DBG(DV.PLUGIN) << "DocumentPlugin: pre-close received\n";
                return plugin->preClose();
            }
            default:
                return false;
        }
    }
private:
    const EventData* ed_;
};

namespace Common {

    template <> CommandEventPtr
    makeCommand<PluginDocStateChangeEvent>(const EventData* ed)
    {
        return new PluginDocStateChangeEvent(ed);
    }

} // namespace Common

////////////////////////////////////////////////////////////////////////////

typedef std::set<String> RemapSet;

static void adjust_action_name_recursive(const RemapSet& rs,
                                         PropertyNode* pn,
                                         const String& pluginName)
{
    if (pn->name() == Sui::ITEM_PROPS) {
        PropertyNode* icon = pn->getProperty(Sui::ICON);
        if (icon && icon->getString().left(2) == "%:")
            icon->setString(pluginName + icon->getString().mid(1));
        PropertyNode* action = pn->getProperty(Sui::ACTION);
        if (0 == action || rs.find(action->getString()) == rs.end())
            return;
        action->setString(pluginName + ':' + action->getString());
    }
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling())
        adjust_action_name_recursive(rs, pn, pluginName);
}

////////////////////////////////////////////////////////////////////////////

class DocumentPlugin::PropTreeMap : public std::map<String, PropertyNodePtr>{};

DocumentPlugin::DocumentPlugin(SernaApiBase* sernaDoc,
                               SernaApiBase* properties)
    : sernaDoc_(static_cast<SernaDoc*>(sernaDoc)),
      pluginProperties_(static_cast<PropertyNode*>(properties)),
      executorMakerMap_(new ExecutorMakerMap),
      executorMap_(new ExecutorMap),
      messageTree_(0),
      suffixMap_(new PropTreeMap())
{
    pluginName_ = pluginProperties()->
        getSafeProperty(PluginLoader::PLUGIN_NAME)->getString();
    String respath = pluginProperties()->
        getSafeProperty(PluginLoader::RESOLVED_PATH)->getString();

    PropertyNode* ui = pluginProperties()->getProperty("ui");
    if (ui) {
        // 1. set plugin origin on UI items and actions
        set_plugin_origin_recursive(ui, pluginName());    
        // 2. adjust action names
        String prefix = pluginName() + ':';
        RemapSet remap_set;
        PropertyNode* action = ui->getSafeProperty("uiActions")->firstChild();
        for (; action; action = action->nextSibling()) {
            PropertyNode* name = action->getProperty(Sui::NAME);
            if (0 == name)
                continue;
            PropertyNode* icon = action->getProperty(Sui::ICON);
            if (icon && icon->getString().left(2) == "%:")
                icon->setString(prefix + icon->getString().mid(2));
            if (name->getString().find(prefix) == 0)
                continue;
            remap_set.insert(name->getString());
            name->setString(prefix + name->getString());
        }
        // 3. adjust action names in ui items
        for (PropertyNode* i = ui->firstChild(); i; i = i->nextSibling()) {
            if (NOTR("uiActions") == i->name())
                continue;
            adjust_action_name_recursive(remap_set, i, pluginName());
        }
    }
    Sui::icon_provider().registerIconsFromDir(respath + NOTR("/icons"),
                                              pluginName_);
    sernaDoc_->stateChangeFactory().subscribe(
        makeCommand<PluginDocStateChangeEvent>, this);
}

DocumentPlugin::~DocumentPlugin()
{
    ExecutorMap::iterator it = executorMap_->begin();
    for (; it != executorMap_->end(); ++it)
        delete it->second;
    for (uint i = 0; i < emptyActions_.size(); ++i)
        sernaDoc()->actionSet()->removeAction(emptyActions_[i].pointer());
}

////////////////////////////////////////////////////////////////////////////

void DocumentPlugin::postInit()
{
    buildPluginInterface();
}

void DocumentPlugin::buildPluginInterface()
{
    PropertyNode* ui_items = pluginProperties()->getProperty("ui/uiItems");
    if (ui_items)
        install_ui_items(sernaDoc(), ui_items);
}

Sui::ItemPtr DocumentPlugin::makeUiItem(const String& name,
                                       PropertyNode* properties)
{
    Sui::ItemPtr item = sernaDoc()->makeItem(name, properties);
    if (!item.isNull())
        add_plugin_origin(item->itemProps(), pluginName());
    return item;
}

Sui::Action* DocumentPlugin::findPluginUiAction(const String& name) const
{
    return sernaDoc()->actionSet()->findAction(pluginName_ + ':' + name);
}

////////////////////////////////////////////////////////////////////////////

ActionExecutor* DocumentPlugin::buildPluginExecutor(
    const PropertyNode* actionProp)
{
    DBG(DV.PLUGIN) 
        << "DocumentPlugin::buildPluginExecutor "
        << actionProp->getSafeProperty(Sui::NAME)->getString() << std::endl;
    Sui::Action* action = sernaDoc()->actionSet()->makeAction(actionProp);
    add_plugin_origin(action->properties(), pluginName());
    const PropertyNode* enabled_prop = actionProp->getProperty("is-enabled");
    if (enabled_prop)
        action->setEnabled(enabled_prop->getBool());
    else
        action->setEnabled(true);
    const PropertyNode* ev_node = actionProp->getProperty("commandEvent");
    if (0 == ev_node) {
        DBG(DV.PLUGIN) << "DP: No command event for: "
            << action->get(Sui::NAME) << std::endl;
        emptyActions_.push_back(action);
        return 0;
    }
    ExecutorMakerMap::iterator it =
        executorMakerMap_->find(ev_node->getString());
    if (it == executorMakerMap_->end()) {
        DBG(DV.PLUGIN) << "DP: Cannot find <" << ev_node->getString()
            << "> in executor maker map, for action=" 
            << action->get(Sui::NAME) << std::endl;
        return 0;
    }
    ActionExecutor* executor = (*it->second)(action, this);
    sernaDoc()->registerActionExecutor(executor);
    (*executorMap_)[ev_node->getString()] = executor;
    DBG(DV.PLUGIN) 
        << "DocumentPlugin::buildPluginExecutor: built plugin executor: "
        << actionProp->getSafeProperty(Sui::NAME)->getString() << std::endl;
    return executor;
}

void DocumentPlugin::buildPluginExecutors()
{
    const PropertyNode* action_p =
        pluginProperties()->getProperty("ui/uiActions");
    if (0 == action_p)
        return;
    for (action_p = action_p->firstChild(); action_p; 
         action_p = action_p->nextSibling())
        buildPluginExecutor(action_p);
}

void DocumentPlugin::registerExecutor(const String& name,
                                      PluginUiExecutorMaker maker)
{
    DBG(DV.PLUGIN) << "DocumentPlugin::registerExecutor " << name << std::endl;
    (*executorMakerMap_)[name] = maker;
}

ActionExecutor* DocumentPlugin::findPluginExecutor(const String& name) const
{
    ExecutorMap::const_iterator it = executorMap_->find(name);
    if (it == executorMap_->end())
        return 0;
    return it->second;
}

///////////////////////////////////////////////////////////////////////

typedef Common::CommandEventMakerRegistry MR;

bool DocumentPlugin::executeCommandEvent(const String& name)
{
    return MR::execute(sernaDoc(), name);
}

bool DocumentPlugin::executeCommandEvent(const String& name,
                                         PropertyNode* in,
                                         PropertyNode* out)
{
    return MR::execute(sernaDoc(), name, in, out);
}

///////////////////////////////////////////////////////////////////////

const PropertyNode* DocumentPlugin::getPropTree(const String& suffix) const
{
    PropTreeMap::iterator i = suffixMap_->find(suffix);
    if (i != suffixMap_->end())
        return (*i).second.pointer();
    return 0;
}

PropertyNode* DocumentPlugin::registerPropTree(const String& suffix, 
                                               const String& propName)
{
    PropTreeMap::iterator i = suffixMap_->find(suffix);
    if (i != suffixMap_->end())
        return (*i).second.pointer();
    PropertyNode* prop = new PropertyNode(propName);
    (*suffixMap_.pointer())[suffix] = prop;
    
    PropertyNodePtr specific_path = 
        prop->makeDescendant("#specific-file-path");
    SernaDoc::Level level;
    load_most_specific_file(sernaDoc(), prop, suffix, level);
    if (specific_path->parent()) 
        specific_path->remove();
    else {
        specific_path->setString(sernaDoc()->getLevelFile(
            SernaDoc::BUILTIN_LEVEL) + suffix);
    }
    specific_path->setName(pluginName());
    sernaDoc()->itemProps()->makeDescendant(
        "#specific-file-path")->appendChild(specific_path.pointer());
    return prop;
}

void DocumentPlugin::saveProperties(const String& path)
{
    for (PropTreeMap::iterator i = suffixMap_->begin();
         i != suffixMap_->end(); i++) {
        save_specific_file((*i).second.pointer(), path, (*i).first);
    }
}

void DocumentPlugin::restoreProperties(const PropertyNode* fromList) 
{
    for (PropTreeMap::iterator i = suffixMap_->begin();
         i != suffixMap_->end(); i++) 
        restore_specific_file(fromList, (*i).first);
}

void DocumentPlugin::getPropTreeSuffixes(PropertyNode* suffixList) const
{
    if (!suffixList)
        return;
    for (PropTreeMap::const_iterator i = suffixMap_->begin(); 
         i != suffixMap_->end(); i++)
        suffixList->appendChild(new PropertyNode((*i).first));
}
