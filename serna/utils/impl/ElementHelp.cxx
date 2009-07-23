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
// Copyright (c) 2006 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "utils/ElementHelp.h"
#include "utils/utils_debug.h"
#include "common/PropertyTree.h"
#include "common/Singleton.h"
#include "common/Url.h"
#include "spgrovebuilder/SpGroveBuilder.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include <map>

using namespace Common;

// START_IGNORE_LITERALS
const char HelpHandle::ELEMENT_HELP[] = "element-help";
const char HelpHandle::ATTR_HELP[]    = "attr-help";
const char HelpHandle::ATTR_NAME[]    = "name";
const char HelpHandle::ATTR_GROUP[]   = "attr-group";
const char HelpHandle::ATTR_GROUP_REF[] = "attr-group-ref";
const char HelpHandle::MATCH[]        = "match";
const char HelpHandle::NSURI[]        = "nsuri";
const char HelpHandle::SHORT_HELP[]   = "short-help";
const char HelpHandle::QTA_HELP[]     = "qta-help";
const char HelpHandle::QTA_ADP_FILE[] = "adp-file";
const char HelpHandle::QTA_HREF[]     = "href";
// STOP_IGNORE_LITERALS


static const char SERNA_EHELP_NS[] = 
    NOTR("http://www.syntext.com/Extensions/ElementHelp-1.0");

namespace {
    
class HelpFilesMap : public std::map<String, HelpHandle*> {};

static HelpFilesMap& help_files()
{
    return SingletonHolder<HelpFilesMap>::instance();
}

class HelpHandleImpl : public HelpHandle {
public:
    HelpHandleImpl(const String& filename);
    
    virtual PropertyNodePtr     elemHelp(const String& elem,
                                         const GroveLib::Node*) const;
    virtual PropertyNodePtr     attrHelp(const String& elem,
                                         const String& attrName,
                                         const GroveLib::Node*) const;
    ~HelpHandleImpl()
    {
        HelpFilesMap::iterator it = help_files().find(filename_);
        if (help_files().end() != it)
            help_files().erase(it);
    }
private:
    virtual const PropertyNode*    findElemHelp(const String& elem,
                                                const GroveLib::Node*) const;
    void          adjustAdp(PropertyNode* prop) const;
    
    String               filename_;
    PropertyTree         helpTree_;
    PropertyTree         attrGroups_;
    String               defaultAdp_;
};

} // namespace

UTILS_EXPIMP HelpHandlePtr get_help_handle(const String& filename)
{
    HelpFilesMap::iterator it = help_files().find(filename);
    if (it != help_files().end())
        return it->second;
    HelpHandleImpl* handle = new HelpHandleImpl(filename);
    help_files()[filename] = handle;
    return handle;
}

static String get_ns(const String& qname, 
                     const GroveLib::Node* nsRes,
                     bool  isAttr)
{
    while (nsRes && nsRes->nodeType() != GroveLib::Node::ELEMENT_NODE)
        nsRes = parentNode(nsRes);
    if (0 == nsRes)
        return GroveLib::XmlNs::defaultNs();
    int idx = qname.find(':');
    const GroveLib::Element* const e = CONST_ELEMENT_CAST(nsRes);
    if (isAttr)
        return (idx <= 0) ? GroveLib::XmlNs::defaultNs() 
                          : e->getXmlNsByPrefix(qname.left(idx));
    else
        return (idx <= 0) ? e->getXmlNsByPrefix(GroveLib::XmlNs::defaultNs())
                          : e->getXmlNsByPrefix(qname.left(idx));
}

static String get_localname(const String& qname)
{
    int idx = qname.find(':');
    return (idx <= 0) ? qname : qname.mid(idx + 1);
}

static PropertyNode* make_nameprop(const String& qname, 
                                   const GroveLib::Element* elem,
                                   bool isAttr = false)
{
    String lname = get_localname(qname);
    return new PropertyNode(isAttr ? '@' + lname : lname, 
        get_ns(qname, elem, isAttr));
}

static void process_help_props(GroveLib::Element* elem,
                               PropertyNode* help_prop)
{
    GroveLib::Node* n = elem->firstChild();
    const GroveLib::Attr* a;
    GroveLib::Element* e = 0;
    for (; n; n = n->nextSibling()) {
        if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
            continue;
        e = ELEMENT_CAST(n);
        String local_name = e->localName();
        if (local_name == HelpHandle::SHORT_HELP) {
            if (!e->firstChild())
                continue;
            GroveLib::DocumentFragmentPtr dfp = 
                e->firstChild()->takeAsFragment(e->lastChild());
            String data;
            dfp->saveAsXmlString(data);
            help_prop->appendChild(
                new PropertyNode(HelpHandle::SHORT_HELP, data));
        } else if (local_name == HelpHandle::QTA_HELP) {
            a = e->attrs().getAttribute(HelpHandle::QTA_HREF);
            if (0 == a)
                continue;
            PropertyNode* qta_prop = new PropertyNode(HelpHandle::QTA_HELP);
            help_prop->appendChild(qta_prop);
            qta_prop->appendChild(new PropertyNode
                (HelpHandle::QTA_HREF, a->value()));
            a = e->attrs().getAttribute(HelpHandle::QTA_ADP_FILE);
            if (a)
                qta_prop->appendChild(new PropertyNode(
                    HelpHandle::QTA_ADP_FILE, a->value()));
        } else if (local_name == HelpHandle::ATTR_HELP) {
            a = e->attrs().getAttribute(HelpHandle::ATTR_NAME);
            if (0 == a || a->value().isEmpty())
                continue;
            PropertyNode* attr_prop = make_nameprop(a->value(), e, true);
            help_prop->appendChild(attr_prop);
            process_help_props(e, attr_prop);
        } else if (local_name == HelpHandle::ATTR_GROUP_REF) {
            a = e->attrs().getAttribute(HelpHandle::ATTR_NAME);
            if (0 == a || a->value().isEmpty())
                continue;
            help_prop->appendChild(new PropertyNode(
                HelpHandle::ATTR_GROUP_REF, a->value()));
        }
    }
}

HelpHandleImpl::HelpHandleImpl(const String& filename)
    : filename_(filename)
{
    GroveLib::GroveBuilder* gb = new GroveLib::SpGroveBuilder;
    gb->setMessenger(new SilentMessenger);
    GroveLib::GrovePtr grove = gb->buildGroveFromFile(filename);
    if (grove.isNull())
        return;
    GroveLib::Element* e = grove->document()->documentElement();
    if (0 == e)
        return;
    const GroveLib::Attr* a = e->attrs().getAttribute(QTA_ADP_FILE);
    if (a)
        defaultAdp_ = a->value();
    GroveLib::Node* n = e->firstChild();
    for (; n; n = n->nextSibling())
    {
        if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
            continue;
        e = ELEMENT_CAST(n);
        if (e->xmlNsUri() != SERNA_EHELP_NS)
            continue;
        if (e->localName() == ATTR_GROUP) {
            a = e->attrs().getAttribute(ATTR_NAME);
            if (0 == a || a->value().isEmpty())
                continue;
            PropertyNodePtr attr_grp = new PropertyNode(a->value());
            DBG(UTILS.EHELP) << "Processing attr group: " 
                << a->value() << std::endl;
            DBG_IF(UTILS.EHELP) GroveLib::Node::dumpSubtree(e);
            process_help_props(e, attr_grp.pointer());
            attrGroups_.root()->appendChild(attr_grp.pointer());
            continue;
        }
        if (e->localName() != ELEMENT_HELP)
            continue;
        a = e->attrs().getAttribute(MATCH);
        if (0 == a || a->value().isEmpty())
            continue;
        PropertyNodePtr match_node = make_nameprop(a->value(), e);
        process_help_props(e, match_node.pointer());
        helpTree_.root()->appendChild(match_node.pointer());
    }
    DBG(UTILS.EHELP) << "Built EHELP proptree:\n";
    DBG_IF(UTILS.EHELP) helpTree_.root()->dump();
    DBG(UTILS.EHELP) << "----------------------------\n";
    DBG_IF(UTILS.EHELP) attrGroups_.root()->dump();
}

const PropertyNode* 
HelpHandleImpl::findElemHelp(const String& elem, 
                             const GroveLib::Node* nsRes) const
{
    const PropertyNode* pn = helpTree_.root()->firstChild();
    String lname(get_localname(elem)), ns(get_ns(elem, nsRes, false));
    for (; pn; pn = pn->nextSibling()) 
        if (pn->name() == lname &&  pn->getString() == ns)
            return pn;
    return 0;
}

// check ADP-FILE property; adjust relative path to absolute
void HelpHandleImpl::adjustAdp(PropertyNode* prop) const
{
    if (0 == prop)
        return;
    PropertyNode* adp = prop->makeDescendant(QTA_ADP_FILE, defaultAdp_, false);
    if (Url(adp->getString()).isRelative())
        adp->setString(Url(filename_).combinePath2Path(adp->getString()));
}

PropertyNodePtr
HelpHandleImpl::elemHelp(const String& elem, const GroveLib::Node* nsRes) const
{
    const PropertyNode* pn = findElemHelp(elem, nsRes);
    if (0 == pn)
        return 0;
    PropertyNodePtr result = new PropertyNode(pn->name(), pn->getString());
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling()) {
        if (pn->name() == SHORT_HELP)
            result->appendChild(pn->copy(true));
        else if (pn->name() == QTA_HELP) {
            result->appendChild(pn->copy(true));
            adjustAdp(result->lastChild());
        }
    }
    DBG(UTILS.EHELP) << "ElemHelp: "; DBG_IF(UTILS.EHELP) result->dump();
    return result;
}

PropertyNodePtr 
HelpHandleImpl::attrHelp(const String& elem,
                         const String& attrName,
                         const GroveLib::Node* nsContext) const
{
    const PropertyNode* elem_help = findElemHelp(elem, nsContext);
    if (0 == elem_help)
        return 0;
    String attr_lname = '@' + get_localname(attrName);
    String attr_ns    = get_ns(attrName, nsContext, true);
    const PropertyNode* pn = elem_help->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() == attr_lname && pn->getString() == attr_ns) {
            PropertyNodePtr result = pn->copy(true);
            adjustAdp(result->getProperty(QTA_HELP));
            DBG(UTILS.EHELP) << "AttrHelp: "; 
            DBG_IF(UTILS.EHELP) result->dump();
            return result;
        }
    }
    // unable to find in local attributes - try to resolve attribute groups
    for (pn = elem_help->firstChild(); pn; pn = pn->nextSibling()) {
        if (pn->name() != ATTR_GROUP_REF)
            continue;
        const PropertyNode* group = 
            attrGroups_.root()->getProperty(pn->getString());
        if (0 == group)
            continue;
        const PropertyNode* attr_help = group->firstChild();
        for (; attr_help; attr_help = attr_help->nextSibling()) {
            if (attr_help->name() == attr_lname && 
                attr_help->getString() == attr_ns) {
                    PropertyNodePtr result = attr_help->copy(true);
                    adjustAdp(result->getProperty(QTA_HELP));
                    DBG(UTILS.EHELP) << "AttrHelp: "; 
                    DBG_IF(UTILS.EHELP) result->dump();
                    return result;
            }
        }
    }
    return 0;
}
