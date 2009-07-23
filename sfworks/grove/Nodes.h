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
#ifndef GROVE_NODES_H_
#define GROVE_NODES_H_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "common/common_defs.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/Visitor.h"
#include "grove/GroveSection.h"
#include "grove/EntityDecl.h"
#include "grove/NodeWithNamespace.h"
#include "grove/GroveSectionRoot.h"
#include "grove/XNodePtr.h"
#include "grove/Decls.h"

GROVE_NAMESPACE_BEGIN

class GroveSaverContext;

/*! This node is a top-level node in a document instance (along with
    DocumentProlog, which is a top-level node for a document instance
    prolog).
 */
class GROVE_EXPIMP Document : public GroveSectionRoot {
public:
    Document()
        : GroveSectionRoot(DOCUMENT_NODE) {}

    virtual const String& nodeName() const;

    /// Obtain pointer to the document element
    Element*   documentElement() const;
    
    GROVE_OALLOC(Document);

private:
    Document(const Document&);
    Document& operator=(const Document&);
    virtual Node* copy(Node*) const;
};

/////////////////////////////////////////////////////////////////////

/*! Fragment of a document, which has it's own section tables
 *  and entity reference table. This class solely uses interface
 *  of GroveSectionRoot, so refer there for details.
 */
class GROVE_EXPIMP DocumentFragment : public GroveSectionRoot {
public:
    virtual const String& nodeName() const;

    DocumentFragment()
        : GroveSectionRoot(DOCUMENT_FRAGMENT_NODE) {}
    
    bool    saveAsXmlString(String& saveTo,
                            int flags = Grove::GS_DEF_STRFLAGS, 
                            const StripInfo* si = 0) const;

    GROVE_OALLOC(DocumentFragment);

private:
    DocumentFragment(const DocumentFragment&);
    DocumentFragment& operator=(const DocumentFragment&);
    virtual Node* copy(Node*) const;
};

/////////////////////////////////////////////////////////////////////

class AttrList;

/*! This class represents XML/SGML Element.
 */
class GROVE_EXPIMP Element : public NodeWithNamespace {
public:
    GROVE_OALLOC(Element);

    Element(const String& elementName)
        : NodeWithNamespace(ELEMENT_NODE)
    {
        setName(elementName);
    }
    virtual ~Element();

    /// Attribute list
    const AttrList&     attrs() const;
    AttrList&           attrs();

    /// Reset GSR value for attributes (used by deep copy)
    void                setAttributesGSR();

    void                dumpInherited() const;

    static Element*     calculateAttrParent(XLPT* p)
    {
        // 16 is to prevent gcc from warning (it is optimized to nil)
        static const size_t offs = ((size_t)&((Element*)16)->attrs_) - 16;
        return (Element*)((char*)p - offs);
    }
    
    virtual void        saveAsXml(GroveSaverContext&, int) const;
    virtual int         memSize() const;

protected:
    virtual Node*            copy(Node*) const;
    friend class Node;
    friend class Attr;

private:
    void    notifyAttributeAdded(Attr* a) const;
    void    notifyAttributeRemoved(Attr* a) const;
    void    notifyAttributeChanged(Attr* a) const;

    XLPT    attrs_;
};

/////////////////////////////////////////////////////////////////////

/*! Parsed attribute value can be accessed via value() member, but if
 *  attribute is tokenized or contain entity references, it may have
 *  children of types ERS/ERE and Text. Use setValue carefully and only
 *  for pure cdata values, because setting attribute value via setValue()
 *  will destroy attribute children.
 *
 *  After modifying Attr children, build() function must be called to
 *  re-build cdata representation of Attr value.
 */
class GROVE_EXPIMP Attr : public NodeWithNamespace {
public:
    /*! This enum defines possible attribute specification methods.
     *  SPECIFIED means that attribute value was explicitly specified
     *  in document instance; DEFAULTED means that attribute value
     *  was augmented by default value from dtd/schema; for CURRENT
     *  see SGML CURRENT feature description.
     */
    enum Defaulted  { SPECIFIED, DEFAULTED, CURRENT };

    /// Attribute value type
    enum AttrType   { INVALID, IMPLIED, CDATA, TOKENIZED };
    
    /// IdClass is set for IDs/IDREF's
    enum IdClass    { NOT_ID,  IS_ID, IS_IDREF, IS_IDREFS };

    /// IdSubClass is to determine flavor of scoped ID
    enum IdSubClass { NOT_SCOPED_ID, ID_SCOPE_DEF, SCOPED_ID, SCOPED_IDREF };

    Attr(const String& attributeName)
        : NodeWithNamespace(ATTRIBUTE_NODE), defaulted_(DEFAULTED),
          type_(CDATA), idClass_(NOT_ID), idSubClass_(NOT_SCOPED_ID),
          duplicateId_(false),
          isEnumerated_(false)
    {
        setName(attributeName);
        nsMapList().clear();
    }
    /// Get pointer to the parent element
    Element* element() const
    {
        if (parent_)
            return Element::calculateAttrParent(parent_);
        return 0;
    }
    /*! Check whether value of this attribute was explicitly specified
     *  (not defaulted via dtd/prolog/schema)
     */
    bool            specified() const { return defaulted_ == SPECIFIED; }

    /*! A string value of an attribute. For tokenized values, this is
     * a concatenation of tokens with single blank as a separator.
     */
    const String& value() const { return parsedValue_; }

    /*! Build string attribute value (which can be accessed via value()
     * member function) from attribute node children.
     */
    void            build();

    /// Sets CDATA-only attribute value.
    void            setValue(const String& val);

    /// True if attribute is tokenized.
    bool            tokenized() const { return type_ == TOKENIZED; }

    /// Returns defaulting type of attribute (see enum Defaulted)
    Defaulted       defaulted() const { return (Defaulted)defaulted_; }

    /// Attribute value type
    AttrType        type() const { return (AttrType)type_; }

    /// Set attribute specification (defaulting) type
    void            setDefaulted(const Defaulted d) { defaulted_ = d; }

    /// Set attribute value type.
    void            setType(const AttrType t) { type_ = t; }

    /*! This function MUST be called by user after altering any attribute
     *  data because it's the only way by which grove visitors may know
     *  about attribute changes.
     */
    void            notifyChange();

    /// True if this attribute has an associated notation.
    bool            hasNotation() const;

    /// True if this attribute has entity name[s] as it's value.
    bool            hasEntities() const;

    IdClass         idClass() const { return (IdClass)idClass_; }
    void            setIdClass(IdClass idc) { idClass_ = idc; }
    IdSubClass      idSubClass() const { return (IdSubClass)idSubClass_; }
    void            setIdSubClass(IdSubClass idc) { idSubClass_ = idc; }

    bool            isDuplicateId() const { return duplicateId_; }
    void            setDuplicateId(bool v) { duplicateId_ = v; }

    /// set by validator if attribute has enumerated value
    bool            isEnumerated() const { return isEnumerated_; }
    void            setEnumerated(bool v) { isEnumerated_ = v; }

    /// Set new value on attribute without notification
    void            setValueNoNotify(const String&);

    void            dumpInherited() const;

    bool            operator<(const Attr& a) const
    {
        return value() < a.value();
    }
    GROVE_OALLOC(Attr);

    REDECLARE_XLISTITEM_INTERFACE_BASE(Node, Attr, XLPT)
    
    virtual void    saveAsXml(GroveSaverContext&, int) const;
    virtual int     memSize() const;

private:
    virtual Node*   copy(Node*) const;
    String                      parsedValue_;
    unsigned int                defaulted_ : 2;
    unsigned int                type_ : 2;
    unsigned int                idClass_ : 2;
    unsigned int                idSubClass_ : 2;
    unsigned int                duplicateId_ : 1;
    unsigned int                isEnumerated_ : 1;
};

class GROVE_EXPIMP AttrList : public Node::XLPT {
public:
    /// Get attribute by it's GI
    Attr*       getAttribute(const Common::String& name) const;
    /// Remove attribute by it's GI
    bool        removeAttribute(const Common::String& name);
    /// Set (override) attribute
    void        setAttribute(Attr*);

    REDECLARE_XLIST_INTERFACE_BASE(Node::XLPT, Attr, Attr)
};

/////////////////////////////////////////////////////////////////////

/*! Public interface for the text nodes. The text may consist not only
    of characters, but also be a SDATA entity reference or non-SGML data.
 */
class GROVE_EXPIMP Text : public Node {
public:
    Text()
        : Node(TEXT_NODE) {}

    Text(const String& value)
        : Node(TEXT_NODE), data_(value) {}

    virtual const String& nodeName() const;

    /// Get text value as string
    const String&   data() const { return data_; }

    /// Set new text value
    void            setData(const String& s)
    {
        data_ = s;
        notifyChanged();
    }

    /// Append new text
    void            appendData(const String& s)
    {
        data_ += s;
        notifyChanged();
    }
    /*! Check whether content of this node is empty (does not contain
     *  any non-whitespace characters)
     */
    bool            isEmpty() const;
    
    void            dumpInherited() const;

    GROVE_OALLOC(Text);

    virtual void    saveAsXml(GroveSaverContext&, int) const;
    virtual int     memSize() const;

private:
    void            notifyChanged();
    virtual Node*   copy(Node*) const;
    String          data_;
};

/////////////////////////////////////////////////////////////////////

/*! This node keeps markup comments.
 */
class GROVE_EXPIMP Comment : public Node {
public:
    Comment()
        : Node(COMMENT_NODE) {}

    Comment(const String& comment)
        : Node(COMMENT_NODE), comment_(comment) {}

    virtual const String&   nodeName() const;

    /// Current comment contents (without SGML comment markup characters).
    const String&           comment() const { return comment_; }

    /// Use this function to alter the comments.
    void                    setComment(const String& s) { comment_ = s; }

    /// Append new character string to current comment node.
    void                    appendComment(const String& s) { comment_ += s; }
    
    virtual void            saveAsXml(GroveSaverContext&, int) const;
    virtual int             memSize() const;

    GROVE_OALLOC(Comment);

private:
    virtual Node*           copy(Node*) const;
    void                    dumpInherited() const;
    String                  comment_;
};

/////////////////////////////////////////////////////////////////////

/*! This node represents XML/SGML PI.
 */
class GROVE_EXPIMP ProcessingInstruction : public Node {
public:
    ProcessingInstruction()
        : Node(PI_NODE) {}

    virtual const String&   nodeName() const;

    //! PI target
    const String&           target() const { return target_; }
    void                    setTarget(const String& t) { target_ = t; }

    //! PI data
    const String&           data() const { return data_; }

    //! Use this function to alter PI data.
    void                    setData(const String& v) { data_ = v; }

    //! Name of PI entity, if applicable.
    const String&           entityName() const { return entityName_; }

    //! Sets an PI entity name.
    void                    setEntityName(const String& v) { entityName_ = v; }
    
    virtual void            saveAsXml(GroveSaverContext&, int) const;
    virtual int             memSize() const;

    GROVE_OALLOC(PI);

private:
    virtual Node*           copy(Node*) const;
    void                    dumpInherited() const;

    String                  data_;
    String                  entityName_;
    String                  target_;
};

//////////////////////////////////////////////////////////

inline AttrList& Element::attrs()
{
    return static_cast<AttrList&>(attrs_);
}

inline const AttrList& Element::attrs() const
{
    return static_cast<const AttrList&>(attrs_);
}

GROVE_NAMESPACE_END

inline GroveLib::Node* parentNode(const GroveLib::Node* n)
{
    return n->nodeType() != GroveLib::Node::ATTRIBUTE_NODE ? n->parent()
        : static_cast<const GroveLib::Attr*>(n)->element();
}

#endif // GROVE_NODES_H_
