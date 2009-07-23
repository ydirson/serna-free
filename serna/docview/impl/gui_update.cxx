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
#include "common/PropertyTree.h"
#include "common/String.h"
#include "ui/UiProps.h"
#include "utils/SernaUiItems.h"

#include <map>
#include <list>
#include <iostream>

using namespace Common;

// START_IGNORE_LITERALS
const char * const UI_ACTION        = "uiAction";
const char * const UI_ACTION_LIST   = "uiActions";
// STOP_IGNORE_LITERALS

typedef std::map<String, PropertyNodePtr> PropertyMap;

static void exclude_actions(PropertyMap& actionMap, 
                            PropertyNode* actionList, bool add)
{
    if (!actionList) 
        return;
    for (PropertyNode* c = actionList->firstChild(); c; c= c->nextSibling()) {
        if (c->name() != UI_ACTION || c->getProperty(Sui::PLUGIN_ORIGIN))
            continue;
        PropertyNode* name = c->getProperty(Sui::NAME);
        if (!name || name->getString().isEmpty())
            continue;
        if (add) 
            actionMap[name->getString()] = c;
        else {
            PropertyMap::iterator i = actionMap.find(name->getString());
            if (i != actionMap.end())
                actionMap.erase(i);
        }
    }
}

typedef std::list<PropertyNode*> PropertyList;

static void collect_action_items(PropertyNode* node, 
                                 const PropertyMap& actionMap,
                                 PropertyList& itemList, bool add)
{
    PropertyNode* props = node->getProperty(Sui::ITEM_PROPS);
    if (props) {
        const String& action = props->getSafeProperty(Sui::ACTION)->getString();
        if (actionMap.find(action) != actionMap.end()) {
            if (add)
                itemList.push_back(node);
            else {
                for (PropertyList::iterator i = itemList.begin();
                     i != itemList.end(); i++)
                    if ((*i) == node) {
                        itemList.remove(node);
                        break;
                    }
            }
        }
    }
    for (PropertyNode* child_node = node->firstChild();
         child_node; child_node = child_node->nextSibling()) {
        if (Sui::ITEM_PROPS == child_node->name())
            continue;
        collect_action_items(child_node, actionMap, itemList, add);
    }
}

String item_name(const PropertyNode* node)
{
    const PropertyNode* props = node->getProperty(Sui::ITEM_PROPS);
    if (props) 
        return props->getSafeProperty(Sui::NAME)->getString();
    return String();
}

static PropertyNode* find_similar_child(PropertyNode* parent, 
                                        PropertyNode* sample)
{
    for (PropertyNode* child = parent->firstChild(); child; 
         child = child->nextSibling()) {
        if (child->name() != sample->name())
            continue;
        if (item_name(child) == item_name(sample))
            return child;
        //if (is_singleton(child->name())) 
        //    return child;
    }
    return 0;
}

static void insert_branch(PropertyList& missedBranch,
                          PropertyNode* ifaceParent)
{
    //std::cerr << "building branch\n";
    PropertyNode* iface_before = 0;
    for (PropertyNode* before = missedBranch.front()->nextSibling(); 
         before; before = before->nextSibling()) {
        iface_before = find_similar_child(ifaceParent, before);
        if (iface_before)
            break;
    }
    PropertyNodePtr branch = new PropertyNode();
    PropertyNode* leaf_node = branch.pointer();
    while (!missedBranch.empty()) {
        PropertyNode* node = missedBranch.front();
        PropertyNodePtr insertion_node = node->copy(false);
        PropertyNode* props = node->getProperty(Sui::ITEM_PROPS);
        if (props) 
            insertion_node->appendChild(props->copy(true));
        leaf_node->appendChild(insertion_node.pointer());
        leaf_node = insertion_node.pointer();
        missedBranch.pop_front();
    }
    PropertyNodePtr to_insert = branch->firstChild();
    to_insert->remove();
    //std::cerr << "Insertion tree\n";
    //to_insert->dump();
    if (iface_before)
        iface_before->insertBefore(to_insert.pointer());
    else    
        ifaceParent->appendChild(to_insert.pointer());
}

static bool update_items(PropertyNode* iface, PropertyNode* builtin)
{
    PropertyMap new_actions;
    exclude_actions(
        new_actions, builtin->getProperty("properties/uiActions"), true);
    exclude_actions(
        new_actions, iface->getProperty("properties/uiActions"), false);

    PropertyList builtin_items;
    collect_action_items(builtin, new_actions, builtin_items, true);
    if (builtin_items.begin() == builtin_items.end())
        return false;

    for (PropertyList::iterator i = builtin_items.begin();
         i != builtin_items.end(); i++) {
        //std::cerr << "missing item\n";
        //(*i)->dump();
        PropertyList missed_branch;
        for (PropertyNode* n = (*i); n && n->parent(); n = n->parent())
            missed_branch.push_front(n);
        PropertyNode* iface_parent = iface;
        while (!missed_branch.empty()) {
            //std::cerr << "searching item <" << missed_branch.front()->name() 
            //        << "> " << item_name(missed_branch.front()) << std::endl;
            PropertyNode* iface_child = 
                find_similar_child(iface_parent, missed_branch.front());
            if (iface_child) {
                iface_parent = iface_child;
                missed_branch.pop_front();
            }
            else {
                insert_branch(missed_branch, iface_parent);
                break;
            }
        }
    }
    return true;
}

static PropertyNode* find_similar_item(PropertyNode* node, 
                                       PropertyNode* sample)
{
    PropertyNode* props = node->getProperty(Sui::ITEM_PROPS);
    if (props && node->name() == sample->name()) {
        const String node_name = item_name(node);
        if (!node_name.isEmpty() && node_name == item_name(sample))
            return node;
    }
    for (PropertyNode* child_node = node->firstChild();
         child_node; child_node = child_node->nextSibling()) {
        if (Sui::ITEM_PROPS == child_node->name())
            continue;
        PropertyNode* similar_item = find_similar_item(child_node, sample);
        if (similar_item)
            return similar_item;
    }
    return 0;
}

static bool clean_interface(PropertyNode* iface, PropertyNode* builtin)
{
    PropertyList obsolete_items;
    PropertyMap obsolete_actions;
    exclude_actions(obsolete_actions, 
                    iface->getProperty("properties/uiActions"), true);
    exclude_actions(obsolete_actions, 
                    builtin->getProperty("properties/uiActions"), false);
    for (PropertyMap::iterator i = obsolete_actions.begin();
         i != obsolete_actions.end(); i++) {
        (*i).second->remove();
    }
    collect_action_items(iface, obsolete_actions, obsolete_items, true);
    if (obsolete_items.begin() == obsolete_items.end()) 
        return false;
    while (!obsolete_items.empty()) {
        PropertyNode* obsolete_item = obsolete_items.front();
        obsolete_items.pop_front();
        PropertyNode* new_item = find_similar_item(builtin, obsolete_item);
        if (new_item) {
            //std::cerr << "  replaced with:\n";
            //new_item->dump();
            obsolete_item->insertBefore(new_item->copy(true));
            collect_action_items(obsolete_item, obsolete_actions, 
                                 obsolete_items, false);
            obsolete_item->remove();
        }   
        else {
        //    std::cerr << "obsolete item not replaced:\n";
        //    obsolete_item->dump();
            obsolete_item->remove();
        }
    }
    return true;
}

static bool update_actions(PropertyNode* iface, PropertyNode* builtin)
{
    PropertyMap old_actions;

    PropertyNode* iface_actions = 
        iface->getProperty("properties/uiActions");
    PropertyNode* builtin_actions = 
        builtin->getProperty("properties/uiActions");
    if (!iface_actions || !builtin_actions)
        return false;

    PropertyNode* action = iface_actions->firstChild();
    for (; action; action = action->nextSibling()) {
        if (action->name() != UI_ACTION || 
            action->getProperty(Sui::PLUGIN_ORIGIN) ||
            !action->getProperty(Sui::NAME))
            continue;
        for (PropertyNode* prop = action->firstChild(); prop; 
             prop = prop->nextSibling()) {
            if (-1 != prop->name().find(NOTR("toggled_o"))) {
                old_actions[action->getProperty(Sui::NAME)->getString()] = 
                    action;
                //std::cerr << "old action:\n";
                //action->dump();
                break;
            }
        }
    }
    for (action = builtin_actions->firstChild(); action; 
         action = action->nextSibling()) {
        if (action->name() != UI_ACTION ||
            action->getProperty(Sui::PLUGIN_ORIGIN) ||
            !action->getProperty(Sui::NAME))
            continue;
        PropertyMap::iterator i = 
            old_actions.find(action->getProperty(Sui::NAME)->getString());
        if (i != old_actions.end()) {
            //std::cerr << "old action replaced with:\n";
            //action->dump();
            (*i).second->insertBefore(action->copy(true));
            (*i).second->remove();
            old_actions.erase(i);
        }
    }
    return true;
}

bool update_interface(PropertyNode* iface, PropertyNode* builtin)
{
    bool is_updated = clean_interface(iface, builtin);
    is_updated |= update_items(iface, builtin);
    is_updated |= update_actions(iface, builtin);
    return is_updated;
}

