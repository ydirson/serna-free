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
#include "utils/DocTemplate.h"
#include "utils/utils_debug.h"
#include "utils/MsgBoxStream.h"
#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "common/Singleton.h"
#include "common/Url.h"
#include "common/PathName.h"
#include "common/StringTokenizer.h"
#include "grove/Nodes.h"
#include "grove/EntityDeclSet.h"
#include "spgrovebuilder/SpGroveBuilder.h"

#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <map>
#include <set>

using namespace Common;
using namespace GroveLib;
using namespace DocSrcInfo;

namespace DocTemplate {

static const char* const SERNA_TEMPLATE_URI1 =
    NOTR("http://www.syntext.com/Extensions/DocTemplate-1.0");
static const char* const SERNA_TEMPLATE_URI2 =
    NOTR("http://www.syntext.com/Extensions/DocTemplate-2.0");

static const char* const DEFAULT_TEMPLATE_FN = NOTR("default-template.xml");

typedef SpGroveBuilder TemplateGroveBuilder;

static bool is_dt_uri(const String& uri)
{
    return uri == SERNA_TEMPLATE_URI1 || uri == SERNA_TEMPLATE_URI2;
}

static String text_value(const Element* e)
{
    String rs("");
    for (const Node* n = e->firstChild(); n; n = n->nextSibling())
        if (n->nodeType() == Node::TEXT_NODE)
            rs += static_cast<const Text*>(n)->data();
    return rs;
}

static void add_property(PropertyNode* parent, const Element* elem)
{
    PropertyNode* property = new PropertyNode(elem->localName());
    parent->appendChild(property);
    for (const Node* c = elem->firstChild(); c; c = c->nextSibling())
        if (c->nodeType() == Node::ELEMENT_NODE)
            add_property(property, static_cast<const Element*>(c));
    if (!property->firstChild())
        property->setString(text_value(elem));
}

static void add_composite_property(PropertyNode* tn, const Element* elem)
{
    if (NOTR("publish") == elem->localName()) {
        PropertyNode* publish_info = tn->makeDescendant("publish-info");
        PropertyNode* method = new PropertyNode(NOTR("method"));
        publish_info->appendChild(method);
        for (const Node* c = elem->firstChild(); c; c = c->nextSibling())
            if (c->nodeType() == Node::ELEMENT_NODE)
                add_property(method, static_cast<const Element*>(c));
    }
    else
        add_property(tn, elem);
    return;
}

bool XmlReader::isValid() const
{
    return !!root()->getProperty(TEMPLATE_NAME);
}

static void add_docroot_elem(PropertyNode* root, const GroveLib::Element* elem)
{
    const Node* n = elem->firstChild();
    PropertyNodePtr result = new PropertyNode(DOCUMENT_SKELETON);
    const Element* docrootElem = 0;
    for (; n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE)
            continue;
        const Element* e = static_cast<const Element*>(n);
        if (!is_dt_uri(e->xmlNsUri()))
            continue;
        if (e->localName() == NOTR("name"))
            result->makeDescendant(NOTR("name"), text_value(e));
        else if (e->localName() == NOTR("content"))
            docrootElem = e;
    }
    if (!result->getProperty("name"))
        return;
    root->appendChild(result.pointer());
    if (!docrootElem)
        return;
    for (n = docrootElem->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() == Node::ELEMENT_NODE) {
            docrootElem = static_cast<const Element*>(n);
            break;
        }
    }
    if (!docrootElem)
        return;
    GroveLib::GrovePtr docrootGrove = new Grove;
    String sysid = root->getSafeProperty(DOCINFO_SYSID)->getString();
    if (!sysid.isEmpty()) {
        RefCntPtr<ExternalEntityDecl> ed =
            new ExternalEntityDecl;
        ed->setName(docrootElem->nodeName());
        ed->setDeclType(EntityDecl::doctype);
        ed->setDataType(EntityDecl::sgml);
        ed->setDeclOrigin(EntityDecl::prolog);
        ed->setDeclModified();
        ed->setSysid(sysid);
        ed->setPubid(root->getSafeProperty(DOCINFO_PUBID)->getString());
        docrootGrove->setDoctypeName(ed->name());
        docrootGrove->parameterEntityDecls().insertDecl(ed.pointer());
    }
    docrootGrove->document()->appendChild(docrootElem->copyAsFragment());
    Element* e = docrootGrove->document()->documentElement();
    const GroveLib::XmlNsMapItem* xmi = docrootElem->nsMapList().firstChild();
    for (; xmi; xmi = xmi->nextSibling())
        e->addToPrefixMap(xmi->prefix(), xmi->uri());
    result->appendChild(new GroveProperty(docrootGrove.pointer()));
}

XmlReader::XmlReader(const String& filename, bool dt)
{
    String fn;
    if (dt)
        fn = filename + '/' + DEFAULT_TEMPLATE_FN;
    else
        fn = filename;
    TemplateGroveBuilder* gb = new TemplateGroveBuilder;
    gb->setMessenger(msgbox_stream().getMessenger());
    GrovePtr grove(gb->buildGroveFromFile(fn));
    if (grove.isNull()) {
        DBG(UTILS.DT) << "Cannot build grove from template " << filename
            << std::endl;
        return;
    }
    const Element* root_elem = grove->document()->documentElement();
    if (0 == root_elem) {
        DBG(UTILS.DT) << "No root element in document template " << filename
            << std::endl;
        return;
    }
    if (!is_dt_uri(root_elem->xmlNsUri())) {
        DBG(UTILS.DT) << "Root element in document template must have "
            << SERNA_TEMPLATE_URI2 << " namespace" << std::endl;
        return;
    }
    root_elem->copyNsToProps(root()->makeDescendant(NS_MAP));
    for (const Node* n = root_elem->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE)
            continue;
        const Element* e = static_cast<const Element*>(n);
        if (!is_dt_uri(e->xmlNsUri()))
            continue;
        if (e->localName() == DOCUMENT_SKELETON)
            add_docroot_elem(root(), e);
        else {
            bool hasChildren = false;
            const Node* nn = e->firstChild();
            for (; nn; nn = nn->nextSibling())
                if (nn->nodeType() == GroveLib::Node::ELEMENT_NODE)
                    hasChildren = true;
            if (hasChildren)
                add_composite_property(root(), e);
            else
                root()->makeDescendant(e->localName(), text_value(e));
        }
    }
    QFileInfo fi(filename);
    root()->makeDescendant(TEMPLATE_PATH, fi.absFilePath());
}

XmlReader::~XmlReader()
{
}

///////////////////////////////////////////////////////////////////////

DocTemplateHolder::DocTemplateHolder()
{
    updateTemplates();
}

DocTemplateHolder& DocTemplateHolder::instance()
{
    return SingletonHolder<DocTemplateHolder>::instance();
}

static void sort_templates(PropertyNode* category)
{
    typedef std::multimap<int, PropertyNodePtr> CatMap;
    CatMap cat_map;
    PropertyNode* pn = category->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        const PropertyNode* prio = pn->getProperty(TEMPLATE_SHOWUP_PRIORITY);
        cat_map.insert(CatMap::value_type(prio ? -prio->getInt() : 0, pn));
    }
    category->removeAllChildren();
    CatMap::const_iterator it = cat_map.begin();
    for (; it != cat_map.end(); ++it)
        category->appendChild(it->second.pointer());
}

static void category_sort(PropertyNode* root)
{
    typedef std::map<String, PropertyNodePtr> Pmap;
    Pmap cset;
    PropertyNode* pn = root->firstChild();
    for (; pn; pn = pn->nextSibling())
        cset[pn->name()] = pn;
    root->removeAllChildren();
    for (Pmap::iterator it = cset.begin(); it != cset.end(); ++it)
        root->appendChild(it->second.pointer());
}

static void process_template(const String& dir, const String& path,
                             PropertyNode* root)
{
    XmlReader xreader(path);
    if (!xreader.isValid())
        return;
    PropertyNode* tpl_root = xreader.root();
    String cat_name = tpl_root->getSafeProperty(TEMPLATE_CATEGORY)->getString();
    String tmpl_name = tpl_root->getSafeProperty(TEMPLATE_NAME)->getString();
    if (cat_name.isEmpty()) {
        cat_name = tmpl_name;
        tpl_root->makeDescendant(TEMPLATE_CATEGORY, tmpl_name, true);
    }
    PropertyNode* category = root->makeDescendant(cat_name);
    if (!category->getProperty(tmpl_name)) {
        tpl_root->setName(tmpl_name);
        tpl_root->makeDescendant(TEMPLATE_DIR, dir, true);
        category->appendChild(tpl_root);
    }
}

typedef std::set<String> SSet;

static void process_templates_subdir(PropertyNode* root,
                                     const String& dirpath,
                                     SSet& processed)
{
    if (processed.find(dirpath) != processed.end())
        return;
    processed.insert(dirpath);
    QDir pdir(dirpath);
    QString filter(NOTR("*.sdt"));
    QStringList qsl = pdir.entryList(filter,
        QDir::Files | QDir::Readable, QDir::Name);
    for (QStringList::iterator qit = qsl.begin(); qit != qsl.end(); ++qit)
        process_template(dirpath, pdir.absFilePath(*qit), root);
}

static void process_templates_dir(PropertyNode* root,
                                  const String& dirpath,
                                  SSet& processed)
{
    if (dirpath.isEmpty())
        return;
    QDir templates_dir(dirpath);
    QFileInfoList dirs(templates_dir.entryInfoList(QDir::Dirs, QDir::Name));
    if (!dirs.empty()) {
        QFileInfoList::const_iterator dit = dirs.begin();
        for (; dit != dirs.end(); ++dit) {
            if (dit->fileName() == "." || dit->fileName() == "..")
                continue;
            process_templates_subdir(root, dit->absFilePath(), processed);
        }
    }
}


void DocTemplateHolder::updateTemplates()
{
    static const char path_sep[] = { PathName::PATH_SEP, 0 };
    SSet processed;
    String templates_path = config().root()->
        getSafeProperty("vars/templates")->getString();
    String plugins_path = config().getDataDir() + NOTR("/plugins");
    String addtl_plugins_path = config().root()->
        getSafeProperty("vars/ext_plugins")->getString();

    process_templates_dir(root(), templates_path, processed); 
    process_templates_dir(root(), plugins_path,   processed);
    for (StringTokenizer st(addtl_plugins_path, path_sep); st; ) {
        String add_dir(st.next());
        if (add_dir.isEmpty())
            continue;
        process_templates_dir(root(), add_dir, processed);
    }
    process_templates_subdir(root(), templates_path, processed);

    category_sort(root());

    // put default template to be the first
    PropertyNodePtr default_cat = root()->getProperty("Default");
    if (default_cat) {
        default_cat->remove();
        if (root()->firstChild())
            root()->firstChild()->insertBefore(default_cat.pointer());
        else
            root()->appendChild(default_cat.pointer());
    }
    PropertyNode* category = root()->firstChild();
    for (; category; category = category->nextSibling())
        sort_templates(category);
}

bool DocTemplateHolder::findMatched(Common::PropertyNode* putTo,
                                    const Common::PropertyNode* props,
                                    const Common::String& filename) const
{
    if (!putTo || !props)
        return false;
    String rootElement = props->getSafeProperty(DOCINFO_DOCELEM)->getString();
    String doctype = props->getSafeProperty(DOCINFO_DOCTYPE)->getString();
    String nons_schema = props->getSafeProperty(NONS_SCHEMA_PATH)->getString();
    DBG(UTILS.DT) << "findMatched: root=" << rootElement 
        << ", dt=" << doctype << ", nons_schema=" << nons_schema << std::endl;
    typedef std::multimap<int, const PropertyNode*> Tmap;
    Tmap template_map;
    const PropertyNode* category = root()->firstChild();
    DBG_IF(UTILS.DT) {
        DBG(UTILS.DT) << "Categories: ";
        for (; category; category = category->nextSibling()) 
            DBG(UTILS.DT) << category->name() << " ";
        category = root()->firstChild();
        DBG(UTILS.DT) << std::endl;
    }
    for (; category; category = category->nextSibling()) {
        const PropertyNode* tmpl = category->firstChild();
        for (; tmpl; tmpl = tmpl->nextSibling()) {
            String guessString =
                tmpl->getSafeProperty(TEMPLATE_DTD_GUESS)->getString();
            String extGuess =
                tmpl->getSafeProperty(TEMPLATE_FN_GUESS)->getString();
            String rootGuess =
                tmpl->getSafeProperty(TEMPLATE_ROOT_GUESS)->getString();
            String sguess =
                tmpl->getSafeProperty(TEMPLATE_NONS_SCHEMA_GUESS)->getString();
            int score = 0;
            if (!extGuess.isEmpty() && !filename.isEmpty()) {
                QRegExp qreg(extGuess);
                if (qreg.isValid() && qreg.search(filename) >= 0) {
                    ++score;
                    DBG(UTILS.DT) << "DTM: " << category->name()
                        << "/" << tmpl->name() << " matched extGuess<"
                        << extGuess << ">\n";
                }
            }
            if (!guessString.isEmpty() && !doctype.isEmpty()) {
                QRegExp qreg(guessString);
                if (qreg.isValid() && qreg.search(doctype) >= 0) {
                    score += 2;
                    score *= 2;
                    DBG(UTILS.DT) << "DTM: " << category->name()
                        << "/" << tmpl->name() << " matched dtd-guess<"
                        << guessString << ">\n";
                }
            }
            if (!rootGuess.isEmpty() && !rootElement.isEmpty()) {
                QRegExp qreg(rootGuess);
                if (qreg.isValid() && qreg.search(rootElement) >= 0) {
                    DBG(UTILS.DT) << "DTM: " << category->name()
                        << "/" << tmpl->name() << " matched rootelem-guess<"
                        << rootGuess << ">\n";
                    ++score;
                }
            }
            if (!sguess.isEmpty() && !nons_schema.isEmpty()) {
                QRegExp qreg(sguess);
                if (qreg.isValid() && qreg.search(nons_schema) >= 0) {
                    DBG(UTILS.DT) << "DTM: " << category->name()
                        << "/" << tmpl->name() << " matched nons-schema-guess<"
                        << sguess << ">\n";
                    ++score;
                }
            }
            if (score == 0)
                continue;
            score = score * 100;
            if (tmpl->getProperty(TEMPLATE_PRIORITY))
                score += tmpl->getProperty(TEMPLATE_PRIORITY)->getInt();
            DBG(UTILS.DT) << "DTM: template " << category->name() << "/"
                << tmpl->name() << " matched: pscore="
                << score << std::endl;
            template_map.insert(Tmap::value_type(-score, tmpl));
        }
    }
    Tmap::const_iterator it = template_map.begin();
    for (; it != template_map.end(); ++it)
        putTo->makeDescendant(it->second->parent()->name())->
            appendChild(it->second->copy(true));
    return template_map.begin() != template_map.end();
}

bool DocTemplateHolder::getDocProperties(const Common::String& path,
                                         Common::PropertyNode* root)
{
    if (!root)
        return false;
    DBG(UTILS.DT) << "getDocProperties, path=" << path << std::endl;    
    bool matched = false;
    PiReader pi(path);
    PropertyTree matched_templates;
    instance().findMatched(matched_templates.root(), pi.root(), path);
    PropertyNode* best_template = matched_templates.root()->firstChild();
    if (best_template)
        best_template = best_template->firstChild();
    if (best_template) {
        root->merge(best_template, true);
        matched = true;
    }
    root->merge(pi.root(), true);
    DBG(UTILS.DT) << "getDocProperties: matched=" << matched << ", best="
        << (best_template 
            ? best_template->getSafeProperty(TEMPLATE_CATEGORY)->getString()
            : String(NOTR("<no best template>"))) << std::endl;
    DBG_IF(UTILS.DT) root->dump();
    return matched;
}

} // namespace DocTemplate

GroveProperty::GroveProperty(GroveLib::Grove* g)
    : PropertyNode(TEMPLATE_DOCROOTGROVE), grove_(g)
{
}

bool GroveProperty::setValue(const PropertyNode* other)
{
    bool changed = PropertyNode::setValue(other);
    const GroveProperty* gp = dynamic_cast<const GroveProperty*>(other);
    if (gp)
        grove_ = gp->grove_;
    return changed;
}

PropertyNode* GroveProperty::copy(bool) const
{
    return new GroveProperty(grove_.pointer()); // has no children
}

GroveProperty::~GroveProperty()
{
}
