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

#ifndef PROLOG_NODES_H_
#define PROLOG_NODES_H_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "common/safecast.h"
#include "common/RefCntPtr.h"
#include "grove/GroveSection.h"
#include "grove/GroveSectionRoot.h"

GROVE_NAMESPACE_BEGIN

class EntityDecl;

// heading is a subtree that goes before the prolog
class GROVE_EXPIMP DocumentHeading : public GroveSectionRoot {
public:
    DocumentHeading()
        : GroveSectionRoot(DOCUMENT_HEADING) {}
    virtual const String& nodeName() const;

    GROVE_OALLOC(DocHeading);
};

class GROVE_EXPIMP DocumentProlog : public GroveSectionRoot {
public:
    virtual const String& nodeName() const;

    DocumentProlog()
        : GroveSectionRoot(DOCUMENT_PROLOG_NODE) {}
        
    virtual void saveAsXml(GroveSaverContext&, int) const;
    
    GROVE_OALLOC(DocProlog);
};

//
// NOTE: In future, it is possible to modify these structures in
// order to build full-blown DTD editor. One idea is that DTD must
// be converted to document in some "DTD for DTD", from which later
// DTD may be generated.
// Another issue is that DTD should be created using GUI mechanisms
// different from ones used for generic document construction.
//
class GROVE_EXPIMP EntityDeclNode : public Node {
public:
    virtual const String& nodeName() const;

    EntityDecl*     entityDecl() const { return entityDecl_.pointer(); }
    void            setDecl(EntityDecl*);

    EntityDeclNode()
        : Node(ENTITY_DECL_NODE) {}
    
    virtual void saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(EntityDN);

private:
    virtual Node*           copy(Node*) const;
    virtual void            dumpInherited() const;

    COMMON_NS::RefCntPtr<EntityDecl>   entityDecl_;
};

class GROVE_EXPIMP AttrDeclNode : public Node {
public:
    virtual const String& nodeName() const;

    enum DeclaredValue {
        cdata,      name,       number,     nmtoken,    nutoken,
        entity,     idref,      names,      numbers,    nmtokens,
        nutokens,   entities,   idrefs,     id,         notation,
        nameTokenGroup
    };

    enum DefaultValueType {
        required,   current,    implied,    conref,     defaulted,
        fixed
    };
    class AttrDef;
    class AttrDefGroup;

    /*! A base class for attribute definitions. Subclasses may represent
     *  either single attribute definition or attribute group.
     */
    class GROVE_EXPIMP AttrDefBase : public COMMON_NS::RefCounted<>, public NamedBase {
    public:
        virtual void dump() const = 0;

        PRTTI_DECL(AttrDef);
        PRTTI_DECL(AttrDefGroup);
        virtual ~AttrDefBase() {}
    };
    /*! Single attribute definition
     */
    class GROVE_EXPIMP AttrDef : public AttrDefBase {
    public:
        AttrDef()
            : dv_(cdata), deftype_(implied) {}

        DeclaredValue       declaredValue() const { return dv_; }
        DefaultValueType    defaultValueType() const { return deftype_; }
        const String&       defaultValue() const { return defvalue_; }
        ulong               nAllowedValues() const;
        const String&       allowedValue(ulong idx) const;
        void                dump() const;

        void                setDeclaredValue(DeclaredValue dv) { dv_ = dv; }
        void                setDeftype(DefaultValueType tv) { deftype_ = tv; }
        void                setDefvalue(const String& v) { defvalue_ = v; }
        void                appendAllowedValue(const String& av)
        {
            allowedValues_.push_back(av);
        }
        PRTTI_DECL(AttrDef);
        GROVE_OALLOC(AttrDef);

    private:
        DeclaredValue       dv_;
        DefaultValueType    deftype_;
        String              defvalue_;
        COMMON_NS::Vector<String>      allowedValues_;
    };
    /*! A set of attribute definitions (may be either ATTLIST set of
     *  attributes or arbitrary attribute group).
     */
    class GROVE_EXPIMP AttrDefGroup : public AttrDefBase {
    public:
        typedef COMMON_NS::Vector<COMMON_NS::RefCntPtr<AttrDefBase> > attrDefList;

        ulong               nDecls() const { return attrDefs_.size(); }
        const AttrDefBase*  decl(ulong i) const
        {
            return attrDefs_[i].pointer();
        }
        void                appendAttrDef(AttrDefBase* d)
        {
            attrDefs_.push_back(d);
        }
        void                dump() const;

        PRTTI_DECL(AttrDefGroup);
        GROVE_OALLOC(AttrDefGroup);

    private:
        attrDefList attrDefs_;
    };

    AttrDeclNode()
        : Node(ATTR_DECL_NODE) {}
    virtual ~AttrDeclNode();

    ulong               nElements() const;
    const String&       element(ulong idx) const;
    const String&       originalDecl() const;
    const AttrDefGroup& attrs() const { return attrGroup_; }

    AttrDefGroup&       attrs() { return attrGroup_; }
    void                appendElement(const String& sv);
    void                setOriginalDecl(const String&);
    
    virtual void saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(AttrDN);

private:
    virtual Node*       copy(Node*) const;
    virtual void        dumpInherited() const;

    String              originalDecl_;
    COMMON_NS::Vector<String>      elements_;
    AttrDefGroup        attrGroup_;
};

/*
*/
class GROVE_EXPIMP ElementDeclNode : public Node {
public:
    virtual const String& nodeName() const;

    enum ContentType {
        modelElt,   any,    empty,  cdata,  mixed
    };

    struct GROVE_EXPIMP ContentToken {
    public:
        enum Connector {
            leaf, sequence, choice, all
        };
        enum Occurence {
            none, opt, plus, rep
        };

        ContentToken();
        ~ContentToken();

        ContentToken*   deep_copy() const;
        void            dump() const;

    public:
        char            connector;
        char            occurs;
        String          token;
        COMMON_NS::Vector<ContentToken*> subexps;
    };

    ElementDeclNode()
        : Node(ELEMENT_DECL_NODE), model_(0) {}
    virtual ~ElementDeclNode();

    ContentToken*       content() const;
    ContentType         contentType() const;
    ulong               nElements() const;
    const String&       element(ulong idx) const;
    const String&       originalDecl() const;
    ulong               nInclusions() const;
    const String&       inclusion(ulong idx) const;
    ulong               nExclusions() const;
    const String&       exclusion(ulong idx) const;

    void                setContent(ContentToken* t);
    void                setContentType(ContentType t);
    void                appendInclusion(const String&);
    void                appendExclusion(const String&);
    void                appendElement(const String& sv);
    void                setOriginalDecl(const String& sv);
    
    virtual void saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(ElementDN);

private:
    virtual Node*       copy(Node*) const;
    virtual void        dumpInherited() const;

private:
    String              originalDecl_;
    COMMON_NS::Vector<String>      elements_;
    COMMON_NS::Vector<String>      inclusions_;
    COMMON_NS::Vector<String>      exclusions_;
    ContentToken*       model_;
    ContentType         ctype_;
};

/*! Representation of whitespaces in prolog.
 */
class GROVE_EXPIMP SSepNode : public Node {
public:
    SSepNode()
        : Node(SSEP_NODE) {}

    virtual const   String& nodeName() const;
    const String&   data() const { return sepdata_; }
    void            setData(const String& s) { sepdata_ = s; }
    
    virtual void saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(SSepN);

private:
    virtual Node*   copy(Node*) const;
    virtual void    dumpInherited() const;

    String          sepdata_;
};

GROVE_NAMESPACE_END

#endif // PROLOG_NODES_H_
