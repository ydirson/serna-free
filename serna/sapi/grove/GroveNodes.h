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
#ifndef SAPI_GROVE_NODE_H_
#define SAPI_GROVE_NODE_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/SString.h"
#include "sapi/common/xtn_wrap.h"
#include "sapi/grove/GroveDecls.h"
#include "sapi/grove/GroveEntity.h"
#include "sapi/grove/Grove.h"

namespace SernaApi {

/// Base class for all XML grove nodes.
class SAPI_EXPIMP GroveNode : public RefCountedWrappedObject {
public:
    enum NodeType {
        UNDEFINED_NODE = 0,

        ELEMENT_NODE = 1, 
        ATTRIBUTE_NODE = 2, 
        TEXT_NODE = 3,

        PI_NODE = 7, 
        COMMENT_NODE = 8, 
        DOCUMENT_NODE = 9,
        DOCUMENT_FRAGMENT_NODE = 11, 
        MAX_DOM_NODETYPE = 12,

        MARKED_SECTION_START_NODE = 16, 
        MARKED_SECTION_END_NODE = 24,

        ENTITY_REF_START_NODE = 17, 
        ENTITY_REF_END_NODE = 25,

        REDLINE_START_NODE = 18, 
        REDLINE_END_NODE = 26,

        DOCUMENT_PROLOG_NODE = 32,
        ENTITY_DECL_NODE ,
        ELEMENT_DECL_NODE, 
        ATTR_DECL_NODE,
        SSEP_NODE, 
        CHOICE_NODE,
        MAX_NODETYPE
    };

    GroveNode(SernaApiBase* = 0);

    virtual ~GroveNode();

    /// Returns the type of the node.
    NodeType        nodeType() const;

    /// Returns name of the node.
    SString         nodeName() const;

    /*! Create (possibly deep) copy of a node, setting new parent, if needed.
     *  Please note that with this method you can only safely copy nodes
     *  which are not section nodes and whose chidlren does not contain
     *  any sections or entity references. If in doubt, use copyAsFragment()
     *  instead.
     *  \a futureParent is used to determine proper context when copying special
     *  nodes (entity refs etc), and is optional for other node types.
     */
    GroveNode   cloneNode(const bool deep,
                          const GroveNode& futureParent = GroveNode()) const;

    /*! Take nodes from current node to the \a endNode inclusive and move
     *  them into created DocumentFragment. Nodes must be on the same
     *  level (be the children of the same parent) and must not cross
     *  grove section boundaries. If any of these conditions occur, this
     *  function will do nothing and return 0.
     */
    GroveDocumentFragment takeAsFragment(const GroveNode& endNode =
                                            GroveNode());

    /*! Same as takeAsFragment(), but does deep copy of the nodes instead
     *  of moving them to DocumentFragment.
     */
    GroveDocumentFragment copyAsFragment(const GroveNode& endNode =
                                            GroveNode()) const;

    /// Returns governing grove.
    Grove       grove() const;

    /// Returns GroveSectionRoot for this node
    GroveSectionRoot getGSR() const;

    /// Change governing GSR for the current node and it's children
    void        setGSR(const GroveSectionRoot&);

    /// Returns pointer to the governing document
    GroveDocument document() const;

    /// Register a NodeWatcher on a node. If visitor is already there,
    /// it is not added, but it's bitmask is OR'ed.
    void        registerNodeWatcher(GroveNodeWatcher* nv, short bitmask = ~0);

    /// De-register a NodeWatcher
    void        deregisterNodeWatcher(GroveNodeWatcher* nv);

    /// Clean up node from all node visitors
    void        deregisterAllNodeWatchers();

    /// Dumps node
    void        dump() const;

    XTREENODE_WRAP_DECL(GroveNode)

    /// Downcasts  to GroveSectionRoot
    GroveSectionRoot        asGroveSectionRoot() const;

    /// Downcasts  to GroveDocument
    GroveDocument           asGroveDocument() const;

    /// Downcasts  to GroveDocumentFragment
    GroveDocumentFragment   asGroveDocumentFragment() const;

    /// Downcasts  to GroveNodeWithNamespace
    GroveNodeWithNamespace  asGroveNodeWithNamespace() const;

    /// Downcasts  to GroveElement
    GroveElement            asGroveElement() const;

    /// Downcasts  to GroveAttr
    GroveAttr               asGroveAttr() const;

    /// Downcasts  to GroveText
    GroveText               asGroveText() const;

    /// Downcasts  to GroveComment
    GroveComment            asGroveComment() const;

    /// Downcasts  to GrovePi
    GrovePi                 asGrovePi() const;

    /// Downcasts  to GroveErs (Entity Reference Start node)
    GroveErs                asGroveErs() const;

    /// Downcasts  to GroveEre (Entity Reference End node)
    GroveEre                asGroveEre() const;

    /// Check whether the content of this node is read-only
    bool                    isReadOnly() const;

    /// Set the read-only status for the content of node
    void                    setReadOnly(bool v, bool recursive = true);

    /// Get current entity context
    GroveErs                getErs() const;

#ifdef DOX_PYTHON_ONLY
    /// Returns the python list of children
    PY_List children();
#endif

};

////////////////////////////////////////////////////////////////////////

/// Base class for grove sections, e.g. Document and DocumentFragment
class SAPI_EXPIMP GroveSectionRoot : public GroveNode {
public:
    GroveSectionRoot(NodeType t);
    GroveSectionRoot(SernaApiBase*);
    virtual ~GroveSectionRoot();

    /// Set new governing grove for GroveSectionRoot
    void    setGrove(const Grove& g);

    /// Get pointer to the governing grove
    Grove   grove() const;

    /// Get access to the root ERS
    GroveErs ersRoot() const;

    /// Get access to the entity reference table
    GroveEntityReferenceTable ert() const;

    /*! Register node visitor with this document section root. See
     * GroveWatcherBase class description for details.
     */
    void    registerWatcher(GroveWatcher* v);

    /// De-register grove visitor from this document section root.
    void    deregisterWatcher(GroveWatcher* v);
};

/// XML document node
class SAPI_EXPIMP GroveDocument : public GroveSectionRoot {
public:
    GroveDocument(SernaApiBase*);
    virtual ~GroveDocument();

    /// Obtain pointer to the document element
    GroveElement   documentElement() const;
};

/// Allows to manupalte on the fragments of the grove.
class SAPI_EXPIMP GroveDocumentFragment : public GroveSectionRoot {
public:
    GroveDocumentFragment(SernaApiBase* = 0);

    /// Saves grove fragment into String.
    bool    saveAsXmlString(SString& saveTo,
                            int flags = Grove::GS_DEF_STRFLAGS,
                            const GroveStripInfo& = GroveStripInfo()) const;
    
    virtual ~GroveDocumentFragment();
};

///////////////////////////////////////////////////////////////////////

/// Base class for GroveElement and GroveAttr
class SAPI_EXPIMP GroveNodeWithNamespace : public GroveNode {
public:
    GroveNodeWithNamespace(SernaApiBase*);
    virtual ~GroveNodeWithNamespace();

    /// Returns local name (without namespace) of an element or attribute.
    SString             localName() const;

    /// Returns namespace prefix for current element or attribute. Returns
    /// empty string if there is no namespace prefix defined for this node.
    SString             xmlNsPrefix() const;

    /// Returns string corresponding to namespace URI
    SString             xmlNsUri() const;

    /// Attributes only: returns URI of XML namespace of a parent element.
    SString             xmlElNsUri() const;

    /// Get namespace URI by xmlns prefix in given context
    SString             getXmlNsByPrefix(const SString& prefix) const;

    /// Get the nearest defined prefix which defines \a ns. If not
    /// found, returns null string.
    SString             getPrefixByXmlNs(const SString& uri) const;

    /// Add prefix/URI pair mapping to the current node.
    void                addToPrefixMap(const SString& prefix,
                                       const SString& nsUri);

    /// Remove prefix from namespace prefix map of a current node
    void                eraseFromPrefixMap(const SString& prefix);
};

////////////////////////////////////////////////////////////////////////

/// Representation of the list of XML element attributes
class SAPI_EXPIMP GroveElementAttrList {
public:
    GroveAttr   getAttribute(const SString& name) const;

    /// Remove attribute by it's name
    bool        removeAttribute(const SString& name);

    /// Set (override) attribute
    void        setAttribute(const GroveAttr&);

    XLIST_WRAP_DECL(GroveAttr)

#ifdef BUILD_SAPI
    GroveElementAttrList(void*);
#endif // BUILD_SAPI

private:
    void*       pvt_;
};

/// XML element node
class SAPI_EXPIMP GroveElement : public GroveNodeWithNamespace {
public:
    GroveElement(SernaApiBase*);
    GroveElement(const SString& elementName);
    virtual ~GroveElement();

    /// Sets new element name
    void            setName(const SString&);

    /// Element Attributes list
    GroveElementAttrList attrs() const;
};

////////////////////////////////////////////////////////////////////////

/// XML element attribute node
class SAPI_EXPIMP GroveAttr : public GroveNodeWithNamespace {
public:
    GroveAttr(SernaApiBase*);
    GroveAttr(const SString& attributeName, 
              const SString& value = SString());
    virtual ~GroveAttr();

    /*! This enum defines possible attribute specification methods.
     *  SPECIFIED means that attribute value was explicitly specified
     *  in document instance; 
     *  DEFAULTED means that attribute value
     *  was augmented by default value from dtd/schema;
     *  for CURRENT see SGML CURRENT feature description.
     */
    enum Defaulted  { SPECIFIED, DEFAULTED, CURRENT };

    /// Attribute value type
    enum AttrType   { INVALID, IMPLIED, CDATA, TOKENIZED };

    /// ID class of the attribute. Note that IDREFS is not yet supported.
    enum IdClass    { NOT_ID,  IS_ID, IS_IDREF, IS_IDREFS };

    /// Returns parent element
    GroveElement     element() const;

    /// Check whether value of this attribute was explicitly specified
    /// (not defaulted via dtd/prolog/schema)
    bool            specified() const;

    /// A string value of an attribute. For tokenized values, this is
    /// a concatenation of tokens with single blank as a separator.
    SString         value() const;

    /// Build string attribute value (which can be accessed via value()
    /// member function) from attribute node children.
    void            build();

    /// Sets CDATA-only attribute value.
    void            setValue(const SString& val);

    /// Sets the new name of an attribute. This function also performs
    /// name parsing and namespace processing, if necessary.
    void            setName(const SString&);

    /// True if attribute is tokenized.
    bool            tokenized() const;

    /// Returns defaulting type of attribute (see enum Defaulted)
    Defaulted       defaulted() const;

    /// Attribute value type
    AttrType        type() const;

    /// Set attribute specification (defaulting) type
    void            setDefaulted(const Defaulted d);

    /// Set attribute value type.
    void            setType(const AttrType t);

    /// Returns ID class
    IdClass         idClass() const;

    /// Set new ID class. Use with caution.
    void            setIdClass(IdClass idc);
};

////////////////////////////////////////////////////////////////////////

/// Text node of the XML grove.
class SAPI_EXPIMP GroveText : public GroveNode {
public:
    GroveText(SernaApiBase*);
    GroveText(const SString& value);
    virtual ~GroveText();

    /// Get text value as string
    SString         data() const;

    /// Set new text value
    void            setData(const SString& s);

    /// Check whether content of this node is empty (does not contain
    /// any non-whitespace characters)
    bool            isEmpty() const;
};

////////////////////////////////////////////////////////////////////////

/// XML comment node
class SAPI_EXPIMP GroveComment : public GroveNode {
public:
    GroveComment(SernaApiBase*);
    GroveComment(const SString& comment);
    virtual ~GroveComment();

    /// Current comment contents (without SGML comment markup characters).
    SString         comment() const;

    /// Use this function to alter the comment.
    void            setComment(const SString& s);
};

////////////////////////////////////////////////////////////////////////

/// XML processing instruction node
class SAPI_EXPIMP GrovePi : public GroveNode {
public:
    GrovePi(SernaApiBase*);
    GrovePi(const SString& target, const SString& data);

    /// PI target
    SString         target() const;
    /// Sets PI target
    void            setTarget(const SString& t);

    /// PI data
    SString         data() const;

    /// Use this function to alter PI data.
    void            setData(const SString& v);
};

////////////////////////////////////////////////////////////////////////

/// Entity Reference Start node
class SAPI_EXPIMP GroveErs : public GroveNode {
public:
    GroveErs(SernaApiBase*);
    
    /// Returns entity declaration
    GroveEntityDecl     entityDecl() const;

    /// Returns corresponding entity reference end node
    GroveEre            ere() const;

    /// Returns parent entity ERS
    GroveErs            parentErs() const;

    /// Returns next reference of the same entity
    GroveErs            nextErs() const;

    /// Returns first nested entity reference of the current entity
    GroveErs            childErs() const;
};

class SAPI_EXPIMP GroveEre : public GroveNode {
public:
    GroveEre(SernaApiBase*);
    
    /// Returns corresponding entity reference start node
    GroveErs            ers() const;
};

} // namespace SernaApi

#endif // SAPI_GROVE_NODE_H
