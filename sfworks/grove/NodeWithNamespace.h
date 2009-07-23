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
/*! \file
 */

#ifndef NODE_WITH_NS_H_
#define NODE_WITH_NS_H_

#include "grove/grove_defs.h"
#include "grove/XmlName.h"
#include "grove/XmlNs.h"
#include "grove/Node.h"
#include "common/String.h"

namespace Common {
    class PropertyNode;
}

namespace GroveLib {

class GROVE_EXPIMP XmlNsMapItem : public Common::XListItem<XmlNsMapItem> {
public:
    XmlNsMapItem(const Common::String& prefix,
                 const Common::String& uri,
                 bool defaulted = false)
        : prefix_(prefix), 
          uri_(uri), 
          defaulted_(defaulted) {}
        
    /*! Returns XML Namespace prefix */
    const Common::String&   prefix() const { return prefix_;  }
    
    /*! Returns XML Namespace URI */
    const Common::String&   uri() const { return uri_; }
    void            setUri(const Common::String& uri) 
    {
        uri_ = uri; 
        defaulted_ = false;
    }
    bool            isDefaulted() const { return defaulted_; }
    void            setDefaulted(bool v) { defaulted_ = v; }
    Common::String  asDeclString() const;

    GROVE_OALLOC(XmlNsMapItem);

private:
    Common::String  prefix_;
    Common::String  uri_;
    bool            defaulted_;
};

class GROVE_EXPIMP XmlNsMapList : public Common::XList<XmlNsMapItem> {
public:
    XmlNsMapItem*   findPrefix(const Common::String& prefix) const
    {
        XmlNsMapItem* item = firstChild();
        for (; item; item = item->nextSibling()) 
            if (item->prefix() == prefix) 
                return item;
        return 0;
    }
    void clear()
    {
        while (firstChild())
            delete firstChild();
    }
    int     mem_size() const;
    ~XmlNsMapList() { clear(); }
};

/*! This interface is inherited by Element and Attr nodes,
    and defines generic interface to namespace information.
 */
class GROVE_EXPIMP NodeWithNamespace : public Node {
public:
    virtual ~NodeWithNamespace();

    const Common::String& name() const { return name_; }
    void                  setName(const Common::String& name) { name_ = name; }

    virtual const String& nodeName() const { return name(); }

    /*! Returns qualified name of the node.*/
    const QualifiedName     qualifiedName() const
    {
        return QualifiedName(localName(), xmlNsPrefix());
    }
    /*! Returns expanded name of the node.*/
    ExpandedName            expandedName() const;

    /*! Returns local name (without namespace) of an element or attribute.*/
    const String    localName() const
    {
        int idx = name().find(':');
        return (idx < 0) ? name() : name().right(name().length() - idx - 1);
    }
    /*! Returns namespace prefix for current element or attribute. Returns
     *  empty string if no namespace prefix defined for this node.
     */
    const String    xmlNsPrefix() const
    {
        int idx = name().find(':');
        return (idx < 0) ? XmlNs::defaultNs() : name().left(idx);
    }
    /*! Returns string corresponding to namespace URI*/
    String          xmlNsUri() const;

    const XmlNsMapList&     nsMapList() const { return nsMapList_; }
    XmlNsMapList&           nsMapList() { return nsMapList_; }

    /*! Attributes only: returns URI of XML namespace of a parent element.*/
    String          xmlElNsUri() const;

    /*! Namespace comparsion functions.*/
    bool                    isSameNs(const String& uri) const 
    {
        return (xmlNsUri() == uri);
    }
    bool                    isSameNs(const NodeWithNamespace* otherNode) const
    {
        return (xmlNsUri() == otherNode->xmlNsUri());
    }
    /*! Returns URI for a given namespace prefix in the current context;
        null string if none.
    */
    String          getXmlNsByPrefix(const String&) const;

    /*! Get the nearest defined prefix which defines \a ns. If not
      found, returns null string.
    */
    const String&   getPrefixByXmlNs(const String& uri) const;

    /*! Add prefix/URI pair mapping to the current node.*/
    XmlNsMapItem*   addToPrefixMap(const String& prefix,
                                   const String& nsUri);
    
    /*! Lookup URI by prefix from the local prefix map only. For
      general prefix lookups, use getXmlNsByPrefix().
    */
    const String&   lookupPrefixMap(const String& pref) const
    {
        const XmlNsMapItem* item = nsMapList_.findPrefix(pref);
        return item ? item->uri() : String::null();
    }

    /*! Remove prefix from prefix map of a current node.*/
    void            eraseFromPrefixMap(const String&);

    /*! Make an attribute string from current XML namespace mappings.*/
    String          dumpXmlNsMap(bool dumpDefaultedAttrs = false) const;
    
    PRTTI_DECL(NodeWithNamespace);

    void                dumpNs() const;
    void                notifyNsMappingChanged(const String&) const;

    // Copy namespace mappings to the given element
    void                copyNsMappings(NodeWithNamespace* to) const;
    void                condCopyNsMappings(NodeWithNamespace* to, 
                                           bool override = false);

    // copy namespace mappings to/from property node
    void                copyNsToProps(Common::PropertyNode*) const;
    void                copyNsFromProps(const Common::PropertyNode*);

    NodeWithNamespace(NodeType t)
        : Node(t) {}

    GROVE_OALLOC(NodeWithNs);

protected:
    int                     node_size() const;

    Common::String          name_;
    XmlNsMapList            nsMapList_; 
};

/////////////////////////////////////////////////////////////////////

} // namespace GroveLib

#endif // NODE_WITH_NS_H_
