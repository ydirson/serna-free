// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveNode : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End
public:
    enum NodeType {
        UNDEFINED_NODE,
        ELEMENT_NODE, ATTRIBUTE_NODE, TEXT_NODE,
        PI_NODE, COMMENT_NODE, DOCUMENT_NODE,
        DOCUMENT_FRAGMENT_NODE, MAX_DOM_NODETYPE,
        MARKED_SECTION_START_NODE, MARKED_SECTION_END_NODE,
        ENTITY_REF_START_NODE, ENTITY_REF_END_NODE,
        DOCUMENT_PROLOG_NODE, ENTITY_DECL_NODE,
        ELEMENT_DECL_NODE, ATTR_DECL_NODE,
        SSEP_NODE, CHOICE_NODE,
        MAX_NODETYPE
    };
    GroveNode();
    virtual ~GroveNode();
    NodeType        nodeType() const;
    SString         nodeName() const;
    GroveNode       cloneNode(const bool,
                              const GroveNode& = GroveNode()) const;
    GroveDocumentFragment takeAsFragment(const GroveNode& = GroveNode());
    GroveDocumentFragment copyAsFragment(const GroveNode& = GroveNode()) const;
    Grove       grove() const;
    GroveSectionRoot getGSR() const;
    void        setGSR(const GroveSectionRoot&);
    GroveDocument document() const;
    void        registerNodeWatcher(GroveNodeWatcher*, short = ~0);
    void        deregisterNodeWatcher(GroveNodeWatcher*);
    void        deregisterAllNodeWatchers();
    void        dump() const;

// copy-paste section: XTreeNode portion
    
    GroveNode    firstChild() const;
    GroveNode    lastChild() const;
    GroveNode    getChild(unsigned long) const;
    GroveNode    root() const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const GroveNode&);
    void            removeAllChildren();
    
    GroveNode       nextSibling() const;
    GroveNode       prevSibling() const;
    unsigned int    siblingIndex() const;
    void            insertBefore(const GroveNode&);
    void            insertAfter(const GroveNode&);
    GroveNode       parent() const;
    void            remove();
    void            removeGroup(const GroveNode&);
    SString         getTreelocString(const GroveNode& = GroveNode()) const; 
    GroveNode       getByTreelocString(const SString&) const;
    bool            isNull() const;
    
    GroveSectionRoot        asGroveSectionRoot() const;
    GroveDocument           asGroveDocument() const;
    GroveDocumentFragment   asGroveDocumentFragment() const;
    GroveNodeWithNamespace  asGroveNodeWithNamespace() const;
    GroveElement            asGroveElement() const;
    GroveAttr               asGroveAttr() const;
    GroveText               asGroveText() const;
    GroveComment            asGroveComment() const;
    GrovePi                 asGrovePi() const;
    GroveErs                asGroveErs() const;
    GroveEre                asGroveEre() const;
    GroveErs                getErs() const;
    
    bool            isReadOnly() const;
    void            setReadOnly(bool, bool = true);

    SIP_PYLIST children() const;
%MethodCode
    if (!sipCpp || sipCpp->isNull()) 
        return PyList_New(0);
    if (!(sipRes = PyList_New(sipCpp->countChildren()))) 
        sipIsErr = 1;
    else {
        GroveNode n = sipCpp->firstChild();
        for (unsigned int i = 0; n; ++i, n = n.nextSibling()) {
            PyObject* t = sipBuildResult(0, "M", 
                new GroveNode(n.getRep()), sipClass_GroveNode);
            if (0 == t || PyList_SetItem(sipRes, i, t) < 0) {
                Py_XDECREF(t);
                Py_DECREF(sipRes);
                sipIsErr = 1;
                break;
            }
        }
    }
%End
};

////////////////////////////////////////////////////////////////////////

class GroveSectionRoot : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveSectionRoot(NodeType);
    virtual ~GroveSectionRoot();
    void    setGrove(const Grove&);
    Grove   grove() const;
    void    registerWatcher(GroveWatcher*);
    void    deregisterWatcher(GroveWatcher*);
    GroveErs ersRoot() const;
    GroveEntityReferenceTable ert() const;
};

class GroveDocument : GroveSectionRoot {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    virtual ~GroveDocument();
    GroveElement   documentElement() const;

private:
    GroveDocument();
};

class GroveDocumentFragment : GroveSectionRoot {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveDocumentFragment();
    bool    saveAsXmlString(SString&,
                            int = Grove::GS_DEF_STRFLAGS,
                            const GroveStripInfo& = GroveStripInfo()) const;
    virtual ~GroveDocumentFragment();
};

///////////////////////////////////////////////////////////////////////

class GroveNodeWithNamespace : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    virtual ~GroveNodeWithNamespace();
    SString             localName() const;
    SString             xmlNsPrefix() const;
    SString             xmlNsUri() const;
    SString             xmlElNsUri() const;
    SString             getXmlNsByPrefix(const SString&) const;
    SString             getPrefixByXmlNs(const SString&) const;
    void                addToPrefixMap(const SString&,
                                       const SString&);
    void                eraseFromPrefixMap(const SString&);

private:
    GroveNodeWithNamespace();
};

////////////////////////////////////////////////////////////////////////

class GroveAttr : GroveNodeWithNamespace {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveAttr(const SString&, const SString& = SString());
    virtual ~GroveAttr();
    enum Defaulted  { SPECIFIED, DEFAULTED, CURRENT };
    enum AttrType   { INVALID, IMPLIED, CDATA, TOKENIZED };
    enum IdClass    { NOT_ID,  IS_ID, IS_IDREF, IS_IDREFS };
    GroveElement     element() const;
    bool            specified() const;
    SString         value() const;
    void            build();
    void            setValue(const SString&);
    void            setName(const SString&);
    bool            tokenized() const;
    Defaulted       defaulted() const;
    AttrType        type() const;
    void            setDefaulted(const Defaulted);
    void            setType(const AttrType);
    IdClass         idClass() const;
    void            setIdClass(IdClass);
};

////////////////////////////////////////////////////////////////////////

class GroveElementAttrList {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveAttr   getAttribute(const SString&) const;
    bool        removeAttribute(const SString&);
    void        setAttribute(const GroveAttr&);

// copy-paste section: XTreeNode portion
    
    GroveAttr    firstChild() const;
    GroveAttr    lastChild() const;
    GroveAttr    getChild(unsigned long) const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const GroveAttr&);
    void            removeAllChildren();
};

////////////////////////////////////////////////////////////////////////

class GroveElement : GroveNodeWithNamespace {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveElement(const SString&);
    virtual ~GroveElement();
    void            setName(const SString&);
    void            setCollapsed(bool);
    GroveElementAttrList attrs() const;
};

////////////////////////////////////////////////////////////////////////

class GroveText: GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveText(const SString&);
    virtual ~GroveText();
    SString         data() const;
    void            setData(const SString&);
    bool            isEmpty() const;
};

////////////////////////////////////////////////////////////////////////

class GroveComment : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveComment(const SString&);
    virtual ~GroveComment();
    SString         comment() const;
    void            setComment(const SString&);
};

////////////////////////////////////////////////////////////////////////

class GrovePi : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GrovePi(const SString&, const SString&);
    SString         target() const;
    void            setTarget(const SString&);
    SString         data() const;
    void            setData(const SString&);
};


class GroveErs : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveEntityDecl     entityDecl() const;
    GroveEre            ere() const;
    GroveErs            parentErs() const;
    GroveErs            nextErs() const;
    GroveErs            childErs() const;

private:
    GroveErs();
};

class GroveEre : GroveNode {

%TypeHeaderCode
#include "sapi/grove/GroveNodes.h"
using namespace SernaApi;
%End

public:
    GroveErs            ers() const;

private:
    GroveEre();
};


