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
#include "docview/SernaDoc.h"
#include "docview/impl/debug_dv.h"
#include "docview/DocumentStateEventData.h"
#include "docview/DocumentStateFactory.h"

#include "ui/UiProps.h"
#include "ui/UiItemSearch.h"
#include "ui/ActionSet.h"

#include "docutils/doctags.h"

#include "common/Url.h"
#include "common/PathName.h"

#include "proputils/PropertyTreeSaver.h"

#include "utils/Config.h"
#include "utils/HelpAssistant.h"
#include "common/PropertyTreeEventData.h"
#include "utils/SernaUiItems.h"
#include "utils/DocSrcInfo.h"
#include "utils/tr.h"

#include <QApplication>
#include <QFile>
#include <set>


namespace {
    typedef std::set<Common::String> StringSet;
}

using namespace Common;

const char * const UI_ACTION        = NOTR("uiAction");
const char * const UI_ACTION_LIST   = NOTR("uiActions");

//////////////////////////////////////////////////////////////////////////

String removed_item_path(Sui::Item* item)
{
    return (String(Sui::REMOVED_PLUGIN_ITEMS) + "/" +
            item->get(Sui::PLUGIN_ORIGIN) + "/" +
            item->itemClass() + ":" + item->get(Sui::NAME));
}

DOCVIEW_EXPIMP bool is_original_level(const SernaDoc::Level l)
{
    switch (l) {
        case SernaDoc::ORIGINAL_TEMPLATE_LEVEL:
        case SernaDoc::BUILTIN_LEVEL:
            return true;
        default:
            return false;
    }
}

static String removed_item_path(const PropertyNode* props)
{
    return (String(Sui::REMOVED_PLUGIN_ITEMS) + "/" +
            props->getSafeProperty(Sui::PLUGIN_ORIGIN)->getString() + "/" +
            props->parent()->name() + ":" + 
            props->getSafeProperty(Sui::NAME)->getString());
}

void save_ui_item(Sui::Item* item, PropertyNode* node, bool isTopLevel)
{
    //! Make property node for the Sui::Item
    PropertyNode* item_node = 0;
    if (isTopLevel) {
        item_node = node->makeDescendant(item->itemClass());
        item_node->removeAllChildren();
    }
    else {
        item_node = new PropertyNode(item->itemClass());
        node->appendChild(item_node);
    }
    //! Save Item`s properties
    PropertyNode* properties = item_node->makeDescendant("properties");
    properties->merge(item->itemProps(true), true);
    Sui::Document* doc = (isTopLevel) ? dynamic_cast<Sui::Document*>(item) : 0;
    if (doc) {
        PropertyNode* actions = properties->makeDescendant(UI_ACTION_LIST);
        actions->removeAllChildren();
        const Sui::ActionSet::ActionPtrMap& action_map =
            doc->actionSet()->actionMap();
        Sui::ActionSet::ActionPtrMap::const_iterator i = action_map.begin();
        for (; i != action_map.end(); i++) {
            if (NOTR("true") == i->second->get(Sui::IS_GENERATED))
                continue;
            PropertyNode* action_name = new PropertyNode(UI_ACTION);
            //new PropertyNode(i->second->get(Sui::Action::NAME));
            action_name->merge(i->second->properties(), true);
            actions->appendChild(action_name);
        }
    }

    //! Save Item`s children
    PropertyNode* generates_children =
        properties->getProperty(Sui::GENERATES_CHILDREN);
    if (0 == generates_children || !generates_children->getBool())
        for (Sui::Item* i = item->firstChild(); i; i = i->nextSibling())
            save_ui_item(i, item_node, false);
}

static Sui::Item* restore_ui_item(Sui::Document* doc, const PropertyNode* node,
                                 Sui::Item* nodeItem, Sui::Item* parent)
{
    Sui::Item* item = 0;
    PropertyNode* props = node->getProperty(Sui::ITEM_PROPS);
    if (props && props->getProperty(Sui::PLUGIN_ORIGIN)) {
        String prop_path = removed_item_path(props);
        PropertyNode* removed_item = doc->property(prop_path);
        if (removed_item)
            return 0;
    }
    if (nodeItem) {
        item = nodeItem;
        if (props) {
            doc->itemProps()->merge(props, true);
            PropertyNode* actions = doc->property(UI_ACTION_LIST);
            if (actions) {
                for (PropertyNode* i = actions->firstChild(); i;
                     i = i->nextSibling()) {
                    if (i->name() != UI_ACTION)
                        continue;
                    PropertyNode* name = i->getProperty(Sui::NAME);
                    if (!name || name->getString().isEmpty())
                        continue;
                    Sui::Action* action =
                        doc->actionSet()->findAction(name->getString());
                    if (action)
                        action->properties()->merge(i, true);
                }
                actions->remove();
            }
        }
    }
    else {
        if (!doc->isToMakeItem(props))
            return 0;
        item = doc->makeItem(node->name(), props);
        if (0 == item)
            return 0;
        if (parent)
            parent->appendChild(item);
    }

    for (const PropertyNode* child_node = node->firstChild();
         child_node; child_node = child_node->nextSibling()) {
        if (Sui::ITEM_PROPS == child_node->name())
            continue;
        restore_ui_item(doc, child_node, 0, item);
    }
    return item;
}

Sui::Item* restore_ui_item(Sui::Document* doc, const PropertyNode* node,
                          Sui::Item* nodeItem)
{
    return restore_ui_item(doc, node, nodeItem, 0);
}

//////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CannotSaveWarningDialog, SernaDoc)

bool CannotSaveWarningDialog::doExecute(SernaDoc* sd, EventData*)
{
    sd->showMessageBox(SernaDoc::MB_CRITICAL, 
        qApp->translate("CannotSaveWarningDialog", "Cannot Save Document"),
        qApp->translate("CannotSaveWarningDialog", 
            "<qt><nobr>Cannot save the document "
            "(write error or permission denied).<br/>"
            "You can try to save document to other location "
            "(use Document->Save As)</qt>"), tr("&Ok"));
    return true;
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(HelpDialog, SernaDoc)

bool HelpDialog::doExecute(SernaDoc*, EventData*)
{
    helpAssistant().show("");
    return true;
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(SaveView, SernaDoc)

bool SaveView::doExecute(SernaDoc* doc, EventData*)
{
    toggle_save_view(doc, false);
    String save_path, comment;
    save_path = doc->getLevelFile(SernaDoc::TEMPLATE_LEVEL, &comment);
    if (save_path.isEmpty())
        return false;
    if (doc->showMessageBox(SernaDoc::MB_INFO, 
        qApp->translate("SaveView", "Save View"),
        qApp->translate("SaveView", 
          "%0Do you want to save view for <b>%1</b> to").
            arg(NOTR("<nobr>")).arg(comment) + QString(NOTR(
                "<nobr><br/><code>%1</code> ?</nobr>"))
            .arg(save_path), tr("&Save"), tr("&Cancel")))
                return false;
    doc->saveView(save_path);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(RestoreView, SernaDoc)

bool RestoreView::doExecute(SernaDoc* doc, EventData*)
{
    String comment;
    String path(doc->getLevelFile(SernaDoc::ORIGINAL_TEMPLATE_LEVEL, &comment));
    if (!QFile(path + NOTR(".sui")).exists())
        path = doc->getLevelFile(SernaDoc::BUILTIN_LEVEL, &comment);
    if (doc->showMessageBox(SernaDoc::MB_INFO,
        qApp->translate("RestoreView", "Restore View"),
        qApp->translate("RestoreView", 
          "%0Do you want to restore view as <b>%1</b> from").
            arg(NOTR("<nobr>")).arg(comment) + QString(NOTR(
                "<nobr><br/><code>%1</code> ?</nobr>"))
            .arg(path), tr("&Restore"), tr("&Cancel")))
                return false;
    doc->restoreView(path);
    return true;
}

/////////////////////////////////////////////////////////////////

bool save_specific_file(PropertyNode* prop,
                        const String& path, const String& extension)
{
    if (path.isEmpty() || !prop)
        return false;
    String filename = path + extension;
    PropUtils::PropertyTreeSaver saver(prop, prop->name());
    if (saver.savePropertyTree(filename)) {
        DBG(DV.VIEWS) << "save_specific_file: " << filename << std::endl;
        return true;
    }
    DBG(DV.VIEWS) << " ERROR: " << saver.errmsg() << std::endl;
    return false;
}

#ifndef _NDEBUG
static const char* level_names[] = {
    NOTR("Recent"), NOTR("Template"), NOTR("OriginalTemplate"),
    NOTR("Builtin")
};
#endif // _NDEBUG

bool load_most_specific_file(SernaDoc* doc, PropertyNode* prop,
                             const String& extension, SernaDoc::Level& level)
{
    DBG(DV.VIEWS) << "load_most_specific_file:\n";
    PropUtils::PropertyTreeSaver loader(prop, prop->name());
    PropertyNodePtr specific_path = prop->getProperty("#specific-file-path");
    if (specific_path)
        specific_path->remove();
    for (SernaDoc::Level l = SernaDoc::RECENT_DOCUMENT_LEVEL;
         l < SernaDoc::BUILTIN_LEVEL; l = SernaDoc::Level(l + 1)) {
        String base_path = doc->getLevelFile(l);
        if (base_path.isEmpty())
            continue;
        base_path += extension;
        DBG(DV.VIEWS) << "  [" << level_names[l] << "] <"
                     << base_path << ">";
        prop->removeAllChildren();
        if (loader.readPropertyTree(base_path)) {
            DBG(DV.VIEWS) << " Ok!"<< std::endl;
            level = l;
            if (!specific_path.isNull()) {
                specific_path->setString(base_path);
                prop->appendChild(specific_path.pointer());
            }
            return true;
        }
        else {
            DDBG << loader.errmsg() << std::endl;
            PropertyNodePtr null = new PropertyNode(NOTR("NULL"));
            PropUtils::PropertyTreeSaver null_loader(null.pointer(),
                                                     null->name());
            if (null_loader.readPropertyTree(base_path)) {
                DBG(DV.VIEWS) << " NULL property tree read!"<< std::endl;
                level = l;
                if (!specific_path.isNull()) {
                    specific_path->setString(base_path);
                    prop->appendChild(specific_path.pointer());
                }
                return true;
            }
            else
                DDBG << null_loader.errmsg() << std::endl;
        }
        DBG(DV.VIEWS) << std::endl;
    }
    return false;
}

void restore_specific_file(const PropertyNode* fromList,
                           const String& suffix)
{
    DBG(DV.VIEWS) << "restore_specific_file: " << std::endl;
    PropertyNode* level_prop = fromList->firstChild();
    for (; level_prop; level_prop = level_prop->nextSibling()) {
        if (!level_prop->getBool()) 
            continue;
        DBG(DV.VIEWS) << "remove suffix file: " 
            << (level_prop->name() + suffix) << std::endl;
        QFile level_file(level_prop->name() + suffix);
        level_file.remove();
    }
    if (fromList->name().isEmpty())
        return;
    level_prop = fromList->firstChild();
    for (; level_prop; level_prop = level_prop->nextSibling()) {
        if (level_prop->getBool())
            continue;
        String level_filename(level_prop->name() + suffix);
        QFile from_file(level_filename);
        if (!from_file.exists())
            continue;
        PropertyNodePtr prop = new PropertyNode;
        PropUtils::PropertyTreeSaver copier(prop.pointer(), String());
        if (!copier.readPropertyTree(level_filename)) {
            DBG(DV.VIEWS) << "error reading level file: "
                << copier.errmsg() << std::endl;
            continue;
        }
        if (!copier.savePropertyTree(fromList->name() + suffix))
            DBG(DV.VIEWS) << "error saving recent level file: "
                << copier.errmsg() << std::endl;
        DBG(DV.VIEWS) << "Copied " << level_filename << " to " << 
            (fromList->name() + suffix) << std::endl;
        return;
    }
}

//////////////////////////////////////////////////////////////////////////

void install_ui_item(Sui::Document* doc, const Sui::ItemPtr& item)
{
    if (item.isNull() || item->parent())
        return;
    String parent = item->get(NOTR("parent"));
    String before = item->get(NOTR("before"));
    String after  = item->get(NOTR("after"));
    Sui::Item* parent_item = 0;
    Sui::Item* before_item = 0;
    Sui::Item* after_item  = 0;
    if (!parent.isEmpty())
        parent_item = doc->findItem(Sui::ItemName(parent));
    Sui::Item* search_base = parent_item ? parent_item : doc;
    if (!before.isEmpty())
        before_item = search_base->findItem(Sui::ItemName(before));
    if (!after.isEmpty())
        after_item = search_base->findItem(Sui::ItemName(after));
    if (before_item)
        before_item->insertBefore(item.pointer());
    else if (after_item)
        after_item->insertAfter(item.pointer());
    else if (parent_item)
        parent_item->appendChild(item.pointer());
    else
        doc->appendChild(item.pointer());
}

//! Following function defined in gui_update.cxx
String item_name(const PropertyNode* node);

static Sui::Item* find_child_item(Sui::Document* doc, Sui::Item* parent,
                                 const PropertyNode* sample)
{
    PropertyNodePtr maker_props = doc->getItemMakerProps(sample->name());
    if (maker_props && 
        maker_props->getSafeProperty(Sui::IS_SINGLETON)->getBool()) {
        Sui::Item* singleton = doc->findItem(Sui::ItemClass(sample->name()));
        if (singleton)
            return singleton;
    }
    String sample_name = item_name(sample);
    for (Sui::Item* child = parent->firstChild(); child;
         child = child->nextSibling()) {
        if (child->itemClass() != sample->name())
            continue;
        if (sample_name.isEmpty() ||
            child->get(Sui::NAME) == sample_name)
            return child;
    }
    return 0;
}

//! Return anchor item (after or before) to insert Item instantiated
//  from "sample"
static Sui::Item* find_anchor(Sui::Item* parent, const Sui::ItemPtr& branch,
                             const String& propName)
{
    //std::cerr << "find item : " << propName << std::endl;
    PropertyNode* name = branch->property(propName);
    if (!name)
        return 0;
    for (Sui::Item* child = parent->firstChild(); child;
         child = child->nextSibling()) {
        if (child->get(Sui::NAME) == name->getString())
            return child;
    }
    return 0;
}

void install_ui_branch(Sui::Document* doc, Sui::Item* parent,
                       const PropertyNode* branch)
{
    for (const PropertyNode* child = branch->firstChild(); child;
         child = child->nextSibling()) {

        Sui::Item* installed = find_child_item(doc, parent, child);
        if (installed) {
            //std::cerr <<"Item found: "<< installed->itemClass() << std::endl;
            install_ui_branch(doc, installed, child);
            continue;
        }
        //std::cerr << "making item: " << child->name() << std::endl;
        Sui::ItemPtr item = restore_ui_item(doc, child, 0);
        if (!item)
            continue;
        Sui::Item* before = find_anchor(parent, item, NOTR("before"));
        if (!before) {
            Sui::Item* after = find_anchor(parent, item, NOTR("after"));
            if (after)
                before = after->nextSibling();
        }
        if (before)
            before->insertBefore(item.pointer());
        else
            parent->appendChild(item.pointer());
        //std::cerr << "Made item: " << item->itemClass() << std::endl;
    }
}

void install_ui_items(Sui::Document* doc, const PropertyNode* node)
{
    for (PropertyNode* i = node->firstChild(); i; i = i->nextSibling()) {
        if (NOTR("MainWindow") == i->name()) {
            //node->dump();
            //std::cerr << "installing item: " << i->name() << std::endl;
            install_ui_branch(doc, doc, i);
            continue;
        }
        String name = i->getSafeProperty("properties/name")->getString();
        if (name.isEmpty())
            continue;
        Sui::ItemPtr item = doc->findItem(Sui::ItemName(name));
        if (item.isNull()) {
            item = restore_ui_item(doc, i, 0);
            if (!item.isNull())
                install_ui_item(doc, item);
        }
    }
}

////////////////////////////////////////////////////////////////////////

static void append_url(StringSet& urlSet, const String& url,
                       PropertyNode* urlList)
{
    if (urlSet.find(url) != urlSet.end())
        return;
    urlList->appendChild(new PropertyNode(NOTR("url"), url));
    urlSet.insert(url);
}

void collect_urls(StringSet& url_set, PropertyNode* prop, 
                  PropertyNode* urlList, bool removeUrlList)
{
    if (!prop)
        return;
    PropertyNodePtr url_list = prop->getProperty("url-list");
    if (url_list.isNull()) 
        return;
    url_list = url_list->copy(true);
    for (PropertyNode* url = url_list->firstChild();
         url; url = url->nextSibling()) {
        if (NOTR("url") != url->name())
            continue;
        append_url(url_set, url->getString(), urlList);
    }
    if (removeUrlList)
        url_list->remove();
}

void make_browse_url_list(const String& baseUrl, PropertyNode* protocol,
                          PropertyNode* urlListParent,
                          PropertyNode* savedProtocol)
{
    if (!protocol)
        return;
    PropertyNode* url_list = urlListParent->makeDescendant("url-list");
    url_list->removeAllChildren();

    StringSet url_set;
    collect_urls(url_set, protocol, url_list, true);
    String base_url = baseUrl;
    if (!base_url.isEmpty()) {
        base_url = Url(baseUrl)[Url::COLLPATH];
        if (!base_url.isEmpty())
            append_url(url_set, base_url, url_list);
    }
    collect_urls(url_set, savedProtocol, url_list, false);
    //if (savedProtocol) 
    //    protocol->merge(savedProtocol, true);
}

void save_recent_urls(const PropertyNode* prop, const String& protocol)
{
    if (!prop)
        return;
    PropertyNode* recent_url =
        prop->getSafeProperty("recent-url-list")->firstChild();
    int c = 0;
    PropertyNode* url_list = 0;
    for (; recent_url; recent_url = recent_url->nextSibling()) {
        if (!url_list) {
            url_list = config().root()->makeDescendant(
                "dav/protocols/" + protocol + "/url-list");
            url_list->removeAllChildren();
        }
        url_list->appendChild(recent_url->copy(true));
        if (++c >= 20)
            break;
    }
}

