// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class PropertyNodeWatcher : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/common/PropertyNode.h"
using namespace SernaApi;
%End

public:
    PropertyNodeWatcher();
    virtual ~PropertyNodeWatcher();
    virtual void propertyChanged(const PropertyNode&) = 0;
};    

////////////////////////////////////////////////////////////

class PropertyNode : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/common/PropertyNode.h"
using namespace SernaApi;
%End

public:
    PropertyNode();
    PropertyNode(const SString&, const SString& = SString());
    PropertyNode(const SString&, int /Constrained/);
    PropertyNode(const SString&, double);
    PropertyNode(const PropertyNode&);
  
    SString         name() const;
    void            setName(const SString&);
  
    SString         getString() const;
    bool            getBool() const;
    int             getInt() const;
    double          getDouble() const;

    void            setString(const SString&);
    void            setBool(bool);
    void            setInt(int);
    void            setDouble(double);
    
    PropertyNode    getProperty(const SString&) const;
    PropertyNode    getSafeProperty(const SString&) const;
    
    void            merge(const PropertyNode&, bool = false);
    PropertyNode    copy(bool = false) const;
   
    PropertyNode    makeDescendant(const SString&);
    PropertyNode    makeDescendant(const SString&, const SString&, bool = true);
    
    void            addWatcher(PropertyNodeWatcher*);
    void            removeWatcher(PropertyNodeWatcher*);
    void            removeAllWatchers();

    bool            save(const SString&) const;
    bool            read(const SString&, bool = false);  
    
    void            dump() const;

// copy-paste section: XTreeNode portion
    
    PropertyNode    firstChild() const;
    PropertyNode    lastChild() const;
    PropertyNode    getChild(unsigned long) const;
    PropertyNode    root() const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const PropertyNode&);
    void            removeAllChildren();
    
    PropertyNode    nextSibling() const;
    PropertyNode    prevSibling() const;
    PropertyNode    parent() const;
    unsigned int    siblingIndex() const;
    void            insertBefore(const PropertyNode&);
    void            insertAfter(const PropertyNode&);
    void            remove();
    void            removeGroup(const PropertyNode&);
    SString         getTreelocString(const PropertyNode& = PropertyNode()) const; 
    PropertyNode    getByTreelocString(const SString&) const;

    bool operator==(const PropertyNode&) const;
    bool operator!=(const PropertyNode&) const;
    
    SIP_PYLIST children() const;
%MethodCode
    if (!sipCpp || sipCpp->isNull()) 
        return PyList_New(0);
    if (!(sipRes = PyList_New(sipCpp->countChildren()))) 
        sipIsErr = 1;
    else {
        PropertyNode n = sipCpp->firstChild();
        for (unsigned int i = 0; n; ++i, n = n.nextSibling()) {
            PyObject* t = sipBuildResult(0, "M", 
                new PropertyNode(n.getRep()), sipClass_PropertyNode);
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

