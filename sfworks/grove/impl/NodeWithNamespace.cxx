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
#include "grove/grove_trace.h"
#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/NodeWithNamespace.h"
#include "common/PropertyTree.h"

using namespace Common;

GROVE_NAMESPACE_BEGIN

void NodeWithNamespace::eraseFromPrefixMap(const String& prefix)
{
    XmlNsMapItem* item = nsMapList_.findPrefix(prefix);
    if (!item)
        return;
    delete item;
    notifyNsMappingChanged(prefix);
}

XmlNsMapItem* NodeWithNamespace::addToPrefixMap(const String& prefix,
                                       const String& nsuri)
{
    XmlNsMapItem* item = nsMapList_.findPrefix(prefix);
    if (item) 
        item->setUri(nsuri);
    else 
        nsMapList_.appendChild((item = new XmlNsMapItem(prefix, nsuri)));
    notifyNsMappingChanged(prefix);
    return item;
}

ExpandedName NodeWithNamespace::expandedName() const
{
    return ExpandedName(localName(), xmlNsUri());
}

void NodeWithNamespace::copyNsMappings(NodeWithNamespace* to) const
{
    const XmlNsMapItem* item = nsMapList_.firstChild();    
    for (; item; item = item->nextSibling())
        to->nsMapList_.appendChild(new XmlNsMapItem(item->prefix(),
            item->uri(), item->isDefaulted()));
}
    
void NodeWithNamespace::condCopyNsMappings(NodeWithNamespace* to, bool ov)
{
    XmlNsMapItem* item = nsMapList_.firstChild();    
    for (; item; item = item->nextSibling()) {
        XmlNsMapItem* ti = to->nsMapList_.findPrefix(item->prefix());
        if (0 == ti) {
            to->nsMapList_.appendChild(new XmlNsMapItem(item->prefix(),
                item->uri(), item->isDefaulted()));
            continue;
        }
        if (ov) 
            ti->setUri(item->uri());
    }
}

String NodeWithNamespace::getXmlNsByPrefix(const String& prefix) const
{
    const Node* n = this;
    while (n) {
        const NodeWithNamespace* nn = n->asConstNodeWithNamespace();
        if (0 == nn) {
            n = parentNode(n);
            continue;
        }
        const String& uri = nn->lookupPrefixMap(prefix);
        if (!uri.isNull())
            return uri;
        n = parentNode(n);
    }
    return String::null();
}

const String& NodeWithNamespace::getPrefixByXmlNs(const String& uri) const
{
    const Node* n = this;
    while (n) {
        const NodeWithNamespace* nn = n->asConstNodeWithNamespace();
        if (nn == 0)
            return String::null();
        const XmlNsMapItem* item = nn->nsMapList().firstChild();
        for (; item; item = item->nextSibling()) {
            if (item->uri() == uri)
                return item->prefix();
        }
        n = nn->parent();
    }
    return String::null();
}

String NodeWithNamespace::xmlNsUri() const
{
    String prefix = xmlNsPrefix();
    if (nodeType() == ATTRIBUTE_NODE && prefix.isEmpty())
        return XmlNs::defaultNs();
    return getXmlNsByPrefix(prefix);
}

String NodeWithNamespace::xmlElNsUri() const
{
    if (nodeType() != ATTRIBUTE_NODE || 0 == parent())
        return String::null();
    return static_cast<const Attr*>(this)->element()->xmlNsUri();
}

int XmlNsMapList::mem_size() const
{
    int s = 0;
    const XmlNsMapItem* map_item = firstChild();
    for (; map_item; map_item = map_item->nextSibling())
        s += sizeof(*map_item) + str_mem(map_item->prefix()) +
             str_mem(map_item->uri());
    return s;
}

String XmlNsMapItem::asDeclString() const
{
    String os;
    os.reserve(256);
    if (prefix().isEmpty()) {
        os.append(" xmlns=\"");
        os.append(uri());
        os.append('"');
    } else {
        os.append(" xmlns:");
        os.append(prefix());
        os.append("=\"");
        os.append(uri());
        os.append('"');
    }
    return os;
}

Common::String NodeWithNamespace::dumpXmlNsMap(bool dumpDefaultedAttrs) const
{
    String os;
    os.reserve(256);
    bool ignoreP = nodeType() != ATTRIBUTE_NODE &&
        parent() && parent()->nodeType() == DOCUMENT_NODE;
    if (prevSibling() && 
        prevSibling()->nodeType() == Node::ENTITY_REF_START_NODE) {
            const EntityReferenceStart* ers = CONST_ERS_CAST(prevSibling());
            if (ers->entityDecl() && 
                ers->entityDecl()->declType() == EntityDecl::xinclude) 
                    ignoreP = true;
    }
    const XmlNsMapItem* map_item = nsMapList_.firstChild();
    for (; map_item; map_item = map_item->nextSibling()) {
        const String& prefix = map_item->prefix();
        const String& uri = map_item->uri();
        if ((map_item->isDefaulted() && !dumpDefaultedAttrs) ||
            (map_item->isDefaulted() && prefix.isEmpty() && uri.isEmpty()) ||
            prefix == XmlNs::xmlpref() ||
            (ignoreP && prefix.isEmpty() && uri.isEmpty()))
                continue;
        os.append(map_item->asDeclString());
    }
    return os;
}

int NodeWithNamespace::node_size() const
{
    return Node::node_size() + sizeof(*this) + nsMapList_.mem_size();
}

void NodeWithNamespace::copyNsToProps(Common::PropertyNode* props) const
{
    const XmlNsMapItem* ni = nsMapList().firstChild();
    for (; ni; ni = ni->nextSibling()) {
        PropertyNode* nsitem = new PropertyNode("nsmapitem");
        nsitem->makeDescendant("prefix", ni->prefix());
        nsitem->makeDescendant("uri"), ni->uri();
        props->appendChild(nsitem);
    }  
}

void NodeWithNamespace::copyNsFromProps(const Common::PropertyNode* props)
{
    if (!props)
        return;
    const PropertyNode* pn = props->firstChild();
    for (; pn; pn = pn->nextSibling()) 
        if (pn->name() == NOTR("nsmapitem"))
            addToPrefixMap(pn->getString("prefix"), pn->getString("uri"));
}

void NodeWithNamespace::dumpNs() const
{
#ifdef GROVE_DEBUG
    DDBG << "XMLNS: local:" << localName();
    if (!xmlNsPrefix().isEmpty())
        DDBG << " prefix:" << abr(xmlNsPrefix());
    String nsuri   = xmlNsUri();
    String elnsuri = xmlElNsUri();
    if (!nsuri.isEmpty())
        DDBG << " NSURI:" << abr(nsuri);
    if (!elnsuri.isEmpty())
        DDBG << " ELNS_URI:" << abr(elnsuri);
    const XmlNsMapItem* item = nsMapList_.firstChild();
    if (item) {
        DDBG << " pmap:" << std::endl;
        for (; item; item = item->nextSibling())
            DDBG << abr(item->prefix()) << '-'
                << abr(item->uri()) << std::endl;
    }
    else
        DDBG << std::endl;
#endif // GROVE_DEBUG
}

NodeWithNamespace::~NodeWithNamespace()
{
}

PRTTI_IMPL(NodeWithNamespace)

GROVE_NAMESPACE_END
