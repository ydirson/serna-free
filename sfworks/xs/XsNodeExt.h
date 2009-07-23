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
#ifndef XS_NODEEXT_H_
#define XS_NODEEXT_H_

#include "xs/xs_defs.h"
#include "grove/Nodes.h"
#include "grove/ChoiceNode.h"
#include "grove/NodeExt.h"
#include "common/prtti.h"
#include "xs/XsElement.h"
#include "xs/XsAttribute.h"
#include "xs/complex/FsmMatcher.h"

typedef GROVE_NAMESPACE::LineLocExt XsNodeExtBase;

XS_NAMESPACE_BEGIN

class DfaXsNodeExt;
class Component;
class Matcher;
class ChoiceParticle;
class AllXsNodeExt;
class ChoiceXsNodeExt;

XS_NAMESPACE_END

/*! Extension by which XSV augments nodes (if requested)
 */
class XS_EXPIMP XsNodeExt : public XsNodeExtBase {
public:
    typedef GROVE_NAMESPACE::Node Node; // shortcut
    typedef GROVE_NAMESPACE::Element Element;

    enum ExtType {
        UNKNOWN, DFA, ALL, SIMPLETYPE, ATTR, CHOICEEXT
    };
    virtual ExtType type() const { return UNKNOWN; }
    
    // returns true if content model is ANY
    static bool  getList(Common::PropertyNode* to, Node* node,
                         Node* nparent= 0, bool replace = false);
    //! returns true if list has at least one required attribute
    static bool  getAttrs(Common::PropertyNode* to, Node* node);
    static bool  getAttrs(Common::PropertyNode* to, 
                          const Xs::XsElement* xe, 
                          const GroveLib::Element* grove);
   //! Returns true if node contans simpletype value, fill list into 'to' node
   //  STCE - Simple Type Containing Value
    static bool  getSTCEvalue(Common::PropertyNode* to, Node* node);

    static const XS_NAMESPACE::XsElement*   xsElement(const Node* n);
    static const XS_NAMESPACE::XsAttribute* xsAttribute(const Node* n);
    const XS_NAMESPACE::Matcher*            getMatcher() const;

    // Returns XsElement for DFA, ALL, SIMPLETYPE
    virtual const XS_NAMESPACE::XsElement* xsElement() const { return 0; }

    // Returns xsAttribute for ATTR
    virtual const XS_NAMESPACE::XsAttribute* xsAttribute() const { return 0; }

    // Augments node
    static void set(Element*, const XS_NAMESPACE::XsElement*);
    static void set(Node*, const XS_NAMESPACE::XsAttribute*);
    static void set(Node*, const XS_NAMESPACE::FsmMatcher::DfaState*);
    static void set(Node*, XS_NAMESPACE::ChoiceParticle*);

    static const XS_NAMESPACE::XsElement*
        getNextXsElem(const GroveLib::Node* parent,
                      const GroveLib::Node* node,
                      const GroveLib::ExpandedName& n);
    PRTTI_DECL(XsNodeExt);
    PRTTI_DECL_NS(DfaXsNodeExt, XS_NAMESPACE);
    PRTTI_DECL_NS(AllXsNodeExt, XS_NAMESPACE);
    PRTTI_DECL_NS(ChoiceXsNodeExt, XS_NAMESPACE);
    PRTTI_DECL_NS(LineLocExt, GroveLib);

    void   setComponent(const XS_NAMESPACE::Component* c) { component_ = c; }
    virtual GroveLib::NodeExt* copy() const;
    bool   isAnyTypeExt() const
        { return component_ && elem()->isAnyType(); }

    XsNodeExt(const XS_NAMESPACE::Component* c)
        : component_(c) {}

    virtual ~XsNodeExt() {}

protected:
    const XS_NAMESPACE::XsElement* elem() const
    {
        return static_cast<const XS_NAMESPACE::XsElement*>(component_);
    }
    const XS_NAMESPACE::XsAttribute* attr() const
    {
        return static_cast<const XS_NAMESPACE::XsAttribute*>(component_);
    }
    void do_dump(int indent, const char*) const;

    const XS_NAMESPACE::Component* component_;
};

XS_NAMESPACE_BEGIN

/*! An interface for NodeExt's which can provide list of elements
 */
class XsListNodeExt {
public:
    virtual void getElemListAppend(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const = 0;
    virtual void getElemListReplace(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Element* n) const = 0;
    virtual void getElemListBefore(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const = 0;
    virtual ~XsListNodeExt() {}
};

///////////////////////////////////////////////////////////////////

/*! Extension for nodes which are validated by DFA
 */
class DfaXsNodeExt : public XsNodeExt, public XsListNodeExt {
public:
    enum { ExtTypeValue = DFA };
    virtual ExtType type() const { return DFA; }

    const FsmMatcher::DfaState* dfaState() const { return dfaState_; }

    void    setDfaState(const FsmMatcher::DfaState* s) { dfaState_ = s; }
    virtual const XsElement* xsElement() const { return elem(); }

    // implementation of functions from ListNodeExt
    virtual void getElemListAppend(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const;
    virtual void getElemListReplace(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Element* n) const;
    virtual void getElemListBefore(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const;

    const XsElement* getNextElem(const GroveLib::ExpandedName& cur,
                                 const GroveLib::ExpandedName& n) const;

    virtual void dump(int indent = 0) const;
    virtual GroveLib::NodeExt* copy() const;

    DfaXsNodeExt(const Component* c, const FsmMatcher::DfaState* s = 0)
        : XsNodeExt(c), dfaState_(s) {}

    PRTTI_DECL(DfaXsNodeExt);

private:
    const FsmMatcher::DfaState* dfaState_;
};

class AllXsNodeExt : public XsNodeExt, public XsListNodeExt {
public:
    enum { ExtTypeValue = ALL };
    virtual ExtType          type() const { return ALL; }
    virtual const XsElement* xsElement() const { return elem(); }

    // implementation of functions from ListNodeExt
    virtual void getElemListAppend(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const;
    virtual void getElemListReplace(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Element* n) const;
    virtual void getElemListBefore(Common::PropertyNode* el,
                                const GROVE_NAMESPACE::Node* n) const;
    virtual void dump(int indent = 0) const;
    virtual GroveLib::NodeExt* copy() const;

    AllXsNodeExt(const Component* c)
        : XsNodeExt(c) {}

    PRTTI_DECL(AllXsNodeExt);
};

class SimpleTypeXsNodeExt : public XsNodeExt {
public:
    enum { ExtTypeValue = SIMPLETYPE };
    virtual ExtType           type() const { return SIMPLETYPE; }
    virtual const XsElement*  xsElement() const { return elem(); }
    virtual void dump(int indent = 0) const;
    virtual GroveLib::NodeExt* copy() const;

    SimpleTypeXsNodeExt(const Component* c)
        : XsNodeExt(c) {}
};

class AttrXsNodeExt : public XsNodeExt {
public:
    enum { ExtTypeValue = ATTR };
    virtual ExtType            type() const { return ATTR; }
    virtual const XsAttribute* xsAttribute() const { return attr(); }
    virtual void dump(int indent = 0) const;
    virtual GroveLib::NodeExt* copy() const;

    AttrXsNodeExt(const Component* c)
        : XsNodeExt(c) {}
};

class ChoiceXsNodeExt : public XsNodeExt {
public:
    enum { ExtTypeValue = CHOICEEXT };
    virtual ExtType            type() const { return CHOICEEXT; }
    virtual const Common::PropertyNode* elist() const;
    virtual void  dump(int indent = 0) const;
    void          setChoiceParticle(ChoiceParticle* p);
    virtual GroveLib::NodeExt* copy() const;

    ChoiceXsNodeExt(const Component*);
    virtual ~ChoiceXsNodeExt();

    PRTTI_DECL(ChoiceXsNodeExt);

private:
    RefCntPtr<ChoiceParticle> cp_;
};

XS_NAMESPACE_END

#endif // XS_NODEEXT_H_
