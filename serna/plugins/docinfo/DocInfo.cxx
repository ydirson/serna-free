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
#include "common/Url.h"
#include "common/StringTokenizer.h"
#include "common/XTreeIterator.h"

#include "grove/Nodes.h"
#include "grove/EntityDecl.h"
#include "grove/EntityDeclSet.h"

#include "ui/UiItemSearch.h"
#include "docview/PluginLoader.h"
#include "editableview/EditableView.h"

#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/SernaUiItems.h"
#include "utils/tr.h"

#include "structeditor/StructEditor.h"
#include "catmgr/CatalogManager.h"

#include "DocInfo.h"

#include "editableview/EditableView.h"
#include "structeditor/ElementList.h"
#include "utils/SernaCatMgr.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "catmgr/CatalogManager.h"
#include "xslt/ResultOrigin.h"
#include "xslt/Engine.h"
#include "xslt/ExternalFunction.h"
#include "formatter/AreaPos.h"

#include <QFileInfo>
#include <QDateTime>
#include <map>

using namespace Common;
using namespace GroveLib;
using namespace DocSrcInfo;

//////////////////////////////////////////////////////////////////////////

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(ShowDocInfo, DocInfoPlugin)

DocInfoPlugin::DocInfoPlugin(SernaApiBase* doc, SernaApiBase* properties,
                             char**)
    : DocumentPlugin(doc, properties),
      se_(0)
{
    REGISTER_UI_EXECUTOR(ShowDocInfo);
    buildPluginExecutors();
}

void DocInfoPlugin::postInit()
{
    se_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
}

DEFINE_PLUGIN_CLASS(DocInfoPlugin)

/////////////////////////////////////////////////////////////////////

class ShowDocInfoDialog;

static const String& dsi_prop(const PropertyNode* dsi, const String& name)
{
    return dsi->getSafeProperty(name)->getString();
}

static const String& safe_prop(const PropertyNode* parent, const String& name)
{
    return parent->getSafeProperty(name)->getString();
}

typedef std::map<String, PropertyNode*> PropertyMap;

void ShowDocInfo::execute()
{
    StructEditor* se = plugin()->structEditor();
    Grove* grove = se->grove();
    PropertyTreeEventData data;
    const PropertyNode* dsi = se->getDsi();
    const String filename = DocSrcInfo::docPathFilename(dsi);
    data.root()->setName(filename);

    ///////
    PropertyNode* prop = data.root()->makeDescendant(tr("General"));
    prop->appendChild(new PropertyNode(tr("Filename"), filename));
    const String& doc_path = dsi_prop(dsi, DOC_PATH);
    prop->appendChild(new PropertyNode(
                          tr("Location"), Url(doc_path)[Url::COLLPATH]));
    QFileInfo file_info(doc_path);
    prop->appendChild(new PropertyNode( 
                          tr("Size"), String::number(ulong(file_info.size()))));
    const String format = NOTR("dddd dd MMMM yyyy hh:mm:ss");
    prop->appendChild(new PropertyNode(
                          tr("Creation Time"), 
                          String(file_info.created().toString(format))));
    prop->appendChild(new PropertyNode(
                          tr("Modification Time"), 
                          String(file_info.lastModified().toString(format))));
    prop->appendChild(new PropertyNode(tr("Root Entity Encoding"), 
                                       Encodings::encodingName(
                                           grove->topDecl()->encoding())));
    PropertyNode* templ_prop = new PropertyNode(tr("Template"));
    templ_prop->appendChild(new PropertyNode(
                                tr("Category"), 
                                dsi_prop(se->getDsi(), TEMPLATE_CATEGORY)));
    templ_prop->appendChild(new PropertyNode(
                                tr("Name"), 
                                dsi_prop(se->getDsi(), TEMPLATE_NAME)));
    templ_prop->appendChild(new PropertyNode(
                                tr("Filename"),
                                String(Url(dsi_prop(se->getDsi(),
                                                    TEMPLATE_PATH)))));
    prop->appendChild(templ_prop);
    PropertyNode* specific_path =
        se->sernaDoc()->property(NOTR("#specific-file-path"));
    if (specific_path) {
        specific_path = specific_path->copy(true);
        specific_path->setName(tr("Specific Files"));
        prop->appendChild(specific_path);
    }

    //////
    prop = data.root()->makeDescendant("XML");
    prop->appendChild(new PropertyNode(NOTR("DTD PUBID"), 
                                       dsi_prop(dsi, DOCINFO_PUBID)));
    prop->appendChild(new PropertyNode(NOTR("DTD SYSID"), 
                                       dsi_prop(dsi, DOCINFO_SYSID)));
    const Element* root = grove->document()->documentElement();
    prop->appendChild(new PropertyNode(tr("Document Element"),
                                       root->nodeName()));
    
    prop->appendChild(new PropertyNode(
                          tr("Schema"), 
                          String(Url(dsi_prop(dsi, RESOLVED_SCHEMA_PATH)))));
    prop->appendChild(new PropertyNode(
                          tr("Stylesheet"),
                          String(Url(dsi_prop(dsi, RESOLVED_STYLE_PATH)))));
    prop->appendChild(new PropertyNode(
                          tr("CSL Stylesheet"),
                          String(Url(dsi_prop(dsi, "resolved-csl-stylesheet")))));

    const PropertyNode* elem_help_prop = 
        dsi->getProperty(RESOLVED_ELEM_HELP_FILE);
    if (elem_help_prop)
        prop->appendChild(new PropertyNode(tr("Element Help File"), 
            elem_help_prop->getString()));

    
    //Vector<String> catalogs = se->uriMapper()->getCatalogsList();
    prop = prop->makeDescendant("XML Catalogs");
    se->uriMapper()->catalogManager()->getCatalogTree(prop);
    

    //for (int i = 0; i < catalogs.size(); i++) {
    //    String num;
    //    num.setNum(i + 1);
    //    prop->appendChild(new PropertyNode(
    //                          NOTR("Catalog #") + num, catalogs[i]));
    //}
    

    //////
    prop = data.root()->makeDescendant(tr("External Entities"));
    const EntityDeclSet* decl_set = grove->entityDecls();
    for (EntityDeclSet::iterator i = decl_set->begin(); 
         i != decl_set->end(); ++i) {
        if ((*i)->declOrigin() != EntityDecl::prolog)
            continue;
        if ((*i)->declType() == EntityDecl::externalGeneralEntity) {
            ExternalEntityDecl* decl = (*i)->asExternalEntityDecl();
            PropertyNode* entity_prop = new PropertyNode(decl->name());
            entity_prop->appendChild(
                new PropertyNode(tr("Filename"), decl->entityPath(grove)));
            entity_prop->appendChild(
                new PropertyNode(tr("Encoding"), 
                                 Encodings::encodingName(decl->encoding())));
            prop->appendChild(entity_prop);
        }
    }

    //////
    prop = data.root()->makeDescendant(tr("Statistics"));
    long elems = 0;
    long words = 0;
    long symbols = 0;
    long symbols_whitespace = 0;
    Node* node = grove->document()->documentElement();
    while (node) {
        switch (node->nodeType()) {
            case Node::ELEMENT_NODE :
                elems++;
                break;
            case Node::TEXT_NODE : {
                const String& text = static_cast<Text*>(node)->data();
                symbols_whitespace += text.length();
                for (StringTokenizer t(text); t; ) {
                    String token = t.next();
                    words++;
                    symbols += token.length();
                }
                break;
            }
            default:
                break;
        }
        if (node->firstChild())
            node = node->firstChild();
        else {
            while (node) {
                if (node->nextSibling()) {
                    node = node->nextSibling();
                    break;
                }
                node = node->parent();
            }
        }
    }
    prop->appendChild(new PropertyNode(tr("Elements"),
        String::number(elems)));
    prop->appendChild(new PropertyNode(tr("Words"),
        String::number(words)));
    prop->appendChild(new PropertyNode(tr("Symbols"),
        String::number(symbols)));
    prop->appendChild(new PropertyNode(tr("Symbols & Whitespaces"), 
                                       String::number(symbols_whitespace)));
    const uint page_num = se->editableView().rootArea()->countChildren();
    prop->appendChild(new PropertyNode(tr("Rendered Pages"), 
                                       String::number(page_num)));
    prop->appendChild(new PropertyNode(tr("Elements Per Page"), 
                                       String::number(elems / page_num)));
    PropertyNode* limit = se->getDsi()->getProperty(
        FOLDING_RULES + String("/") + LARGE_DOC_LIMIT);
    PropertyNode* elems_per_page = se->getDsi()->getProperty(
        FOLDING_RULES + String("/") + ELEMS_PER_PAGE);
    const bool is_large = (limit && elems_per_page)
        ? elems >= (limit->getInt() * elems_per_page->getInt())
        : false;
    prop->appendChild(new PropertyNode(tr("Considered Large")));
    prop->lastChild()->setString(is_large ? tr("Yes") : tr("No"));

    //////
    prop = data.root()->makeDescendant(tr("Loaded Plugins"));
    PluginLoader& plugin_loader = pluginLoader();
    PropertyMap plugin_map;
    for (PropertyNode* plugin = plugin_loader.getPluginProps()->firstChild();
         plugin; plugin = plugin->nextSibling()) {
        const String& name = safe_prop(plugin, PluginLoader::PLUGIN_NAME);
        if (plugin_loader.isLoaded(plugin, se->sernaDoc())) 
            plugin_map[name] = plugin;
    }
    for (PropertyMap::const_iterator i = plugin_map.begin();
         i != plugin_map.end(); i++) {
        PropertyNode* plugin_prop = new PropertyNode((*i).first);

        plugin_prop->appendChild(
            new PropertyNode(tr("Description"), 
                             safe_prop((*i).second,
                                       PluginLoader::PLUGIN_DESC)));
        plugin_prop->appendChild(
            new PropertyNode(tr("SPD Filename"), 
                             safe_prop((*i).second, 
                                       PluginLoader::SPD_FILE_NAME)));
        plugin_prop->appendChild(
            new PropertyNode(tr("SPD Path"), 
                             safe_prop((*i).second, 
                                       PluginLoader::RESOLVED_PATH)));
        plugin_prop->appendChild(
            new PropertyNode(tr("DLL Path"), 
                             safe_prop((*i).second,
                                       PluginLoader::PLUGIN_DLL)));
        plugin_prop->appendChild(
            new PropertyNode(tr("DLL Resolved Path"), 
                             safe_prop((*i).second,
                                       PluginLoader::RESOLVED_DLL)));
        PropertyNode* preload = 
            (*i).second->getProperty(PluginLoader::PRELOAD_DLL);
        if (preload)
            plugin_prop->appendChild(preload->copy());
        
        plugin_prop->appendChild(new PropertyNode(tr("Load For"), 
            safe_prop((*i).second, PluginLoader::LOAD_FOR)));
        prop->appendChild(plugin_prop);
    }
    
    //////
    prop = data.root()->makeDescendant(tr("Other"));
    PropertyNode* folding = se->getDsi()->getProperty(FOLDING_RULES);
    if (folding)
        prop->appendChild(folding->copy(true));

    makeCommand<ShowDocInfoDialog>(&data)->execute(se);
}

