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
#include "common/common_defs.h"
#include "common/PropertyTree.h"
#include "common/PathName.h"
#include "common/OsEnv.h"
#include "grove/Nodes.h"
#include "grove/StripInfo.h"
#include "catmgr/CatalogManager.h"
#include "spgrovebuilder/SpGroveBuilder.h"
#include "PropertyTreeSaver.h"

#include <stdlib.h>
#include <iostream>
#include <memory>

USING_COMMON_NS
using namespace GroveLib;

namespace PropUtils {

#if defined(_WIN32)
static const char TARGET_PLATFORM[]  = "win";
#elif defined(__APPLE__)
static const char TARGET_PLATFORM[]  = "mac";
#elif defined(__linux__)
static const char TARGET_PLATFORM[]  = "linux";
#elif defined(__FreeBSD__)
static const char TARGET_PLATFORM[]  = "freebsd";
#elif defined(__sun__)
static const char TARGET_PLATFORM[]  = "sunos";
#else
# error UNKNOWN PLATFORM!
#endif

class PropertyStripInfo : public GroveLib::StripInfo {
public:
    /// Returns TRUE if contents of \a elem can be safely stripped,
    /// or FALSE when linefeeds and whitespaces needs to be preserved.
    virtual bool    checkStrip(const Element* elem) const
    {
        for (Node* n = elem->firstChild(); n; n = n->nextSibling())
            if (Node::ELEMENT_NODE == n->nodeType())
                return true;
        return false;
    }
    virtual void    strip(Node*) const {}

    virtual ~PropertyStripInfo() {}
};

typedef RefCntPtr<Element> ElemPtr;

PropertyTreeSaver::PropertyTreeSaver(PropertyNode* root, const String& rtag)
    : rootTag_(rtag), root_(root)
{
}

static bool check_platform(const Node* n)
{
    if (n->nodeType() != Node::ELEMENT_NODE)
        return true;
    const Attr* platform_att =
        static_cast<const Element*>(n)->attrs().getAttribute(NOTR("platform"));
    if (platform_att && platform_att->value().find(TARGET_PLATFORM) < 0)
        return false;
    const Attr* options =
        static_cast<const Element*>(n)->attrs().getAttribute(NOTR("options"));
    if (options)
#ifdef NDEBUG
        return options->value().find(NOTR("release")) >= 0;
#else // NDEBUG
        return options->value().find(NOTR("debug")) >= 0;
#endif // NDEBUG
    return true;
}

static bool has_element_content(const Node* n)
{
    for (const Node* sn = n->firstChild(); sn; sn = sn->nextSibling())
        if (sn->nodeType() == Node::ELEMENT_NODE)
            return true;
    return false;
}

static String collect_text(const Node* n)
{
    String text;
    for (const Node* sn = n->firstChild(); sn; sn = sn->nextSibling())
        if (sn->nodeType() == Node::TEXT_NODE)
            text += CONST_TEXT_CAST(sn)->data();
    return text;
}

static void build_tree(const Node* pn, PropertyNode* ptn)
{
    for (const Node* n = pn->firstChild(); n; n = n->nextSibling()) {
        if (!check_platform(n))
            continue;
        if (has_element_content(n)) {
            PropertyNode* npn = new PropertyNode(n->nodeName());
            ptn->appendChild(npn);
            build_tree(n, npn);
        } else {
            if (n->nodeType() != Node::ELEMENT_NODE)
                continue;
            ptn->appendChild(new PropertyNode(n->nodeName(), collect_text(n)));
        }
    }
}

static PropertyNode* add_property(PropertyNode* parent,
                                  PropertyNode* before,
                                  const Node* n)
{
    PropertyNode* npn = new PropertyNode(n->nodeName());
    if (before)
        before->insertBefore(npn);
    else
        parent->appendChild(npn);
    return npn;
}

static String get_mode(const Node* n)
{
    const Attr* mode_attr = 0;
    if (n->nodeType() == Node::ELEMENT_NODE)
        mode_attr = static_cast<const Element*>(n)->
            attrs().getAttribute(NOTR("merge"));
    if (0 == mode_attr)
        return String();
    return mode_attr->value().lower();
}

static uint get_count(const Node* n)
{
    const Attr* count_attr = 0;
    if (n->nodeType() == Node::ELEMENT_NODE)
        count_attr = static_cast<const Element*>(n)->
            attrs().getAttribute(NOTR("count"));
    if (0 == count_attr)
        return 1;
    return count_attr->value().toUInt();
}

static PropertyNode* get_prop(PropertyNode* parent,
                              const String& name,
                              int count)
{
    PropertyNode* pn = parent->firstChild();
    for (; pn; pn = pn->nextSibling())
        if (pn->name() == name && --count == 0)
            return pn;
    return 0;
}

static void merge_tree(const Node* pn, PropertyNode* ptn)
{
    if (!has_element_content(pn)) {
        String mode = get_mode(pn);
        String text = collect_text(pn);
        if (mode == NOTR("append-string"))
            ptn->setString(ptn->getString() + text);
        else if (mode == NOTR("prepend-string"))
            ptn->setString(text + ptn->getString());
        else
            ptn->setString(text);
        return;
    }
    for (const Node* n = pn->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE)
            continue;
        if (!check_platform(n))
            continue;
        String mode = get_mode(n);
        PropertyNode* existing_prop =
            get_prop(ptn, n->nodeName(), get_count(n));
        if (0 == existing_prop) { // add
            if (mode == NOTR("remove"))
                continue;
            if (mode == NOTR("prepend"))
                merge_tree(n, add_property(ptn, ptn->firstChild(), n));
            else
                merge_tree(n, add_property(ptn, 0, n));
            continue;
        }
        if (mode == NOTR("remove")) {
            existing_prop->remove();
            continue;
        }
        if (mode == NOTR("append")) {
            merge_tree(n, add_property(ptn, 0, n));
            continue;
        }
        if (mode == NOTR("prepend")) {
            merge_tree(n, add_property(ptn, existing_prop, n));
            continue;
        }
        if (mode == NOTR("replace")) {
            merge_tree(n, add_property(ptn, existing_prop, n));
            existing_prop->remove();
            continue;
        }
        merge_tree(n, existing_prop);
    }
}

bool PropertyTreeSaver::prepare_read(const String& filename, ElemPtr& ep)
{
    if (!set_filename(filename))
        return false;
    if (!PathName(filename_).exists())
        return error(NOTR("File does not exist: ") + filename);
    SpGroveBuilder* spgb = new SpGroveBuilder(GroveBuilder::pureXmlData);
    std::auto_ptr<CatMgr::CatalogManager> cmgr(CatMgr::global_catalog().copy());
    String xmlCatPath(get_env(NOTR("XML_CATALOG_FILES")));
    cmgr->addCatalogList(xmlCatPath);
    spgb->setCatalogManager(cmgr.get());
    GrovePtr g = spgb->buildGroveFromFile(filename_, false);
    spgb->setCatalogManager(0);
    if (g.isNull())
        return error(NOTR("Empty grove built"));
    ep = g->document()->documentElement();
    if (0 == ep)
        return error(NOTR("No document element"));
    if (!rootTag_.isEmpty() && rootTag_ != ep->name())
        return error(NOTR("Root tag name mismatch: ") + ep->name() +
                     NOTR(" expected: ") + rootTag_);
    return true;
}

bool PropertyTreeSaver::readPropertyTree(const String& filename)
{
    ElemPtr ep;
    if (!prepare_read(filename, ep))
        return false;
    if (root_->firstChild())
        root_->firstChild()->removeGroup(root_->lastChild());
    build_tree(ep.pointer(), root_.pointer());
    rootTag_ = ep->nodeName();
    return true;
}

bool PropertyTreeSaver::mergePropertyTree(const String& filename)
{
    ElemPtr ep;
    if (!prepare_read(filename, ep))
        return false;
    merge_tree(ep.pointer(), root_.pointer());
    if (rootTag_.isEmpty())
        rootTag_ = ep->nodeName();
    return true;
}

static void build_grove(Element* pn, const PropertyNode* ptn)
{
    for (const PropertyNode* n = ptn->firstChild(); n; n = n->nextSibling()) {
        if (!n->name().isEmpty() && n->name().at(0) == '#')
            continue;
        Element* ne = new Element(n->name());
        pn->appendChild(ne);
        if (n->firstChild())
            build_grove(ne, n);
        if (!ne->firstChild())
            ne->appendChild(new Text(n->getString()));
    }
}

bool PropertyTreeSaver::savePropertyTree(const String& filename)
{
    if (!set_filename(filename))
        return false;
    GrovePtr g = new Grove;
    Element* rootElem = new Element(rootTag_);
    g->document()->appendChild(rootElem);
    build_grove(rootElem, root_.pointer());
    g->setTopSysid(filename_);
    PropertyStripInfo strip_info;
    return g->saveAsXmlFile(Grove::GS_DEF_FILEFLAGS|Grove::GS_INDENT,
        &strip_info);
}

bool PropertyTreeSaver::set_filename(const String& filename)
{
    if (!filename.isNull())
        filename_ = filename;
    if (filename_.isEmpty())
        return error(NOTR("Invalid (empty) file name"));
    return true;
}

bool PropertyTreeSaver::error(const COMMON_NS::String& s)
{
    errmsg_ = s + ", file: " + filename_;
    return false;
}

PropertyTreeSaver::~PropertyTreeSaver()
{
}

} // namespace PropUtils
