// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class XpathNodeSet {

%TypeHeaderCode
#include "sapi/grove/XpathUtils.h"
using namespace SernaApi;
%End
public:
    void            push_back(const GroveNode&);
    int             size();
    void            erase(int);
    GroveNode       firstNode() const;
    GroveNode       lastNode() const;

GroveNode   operator[](int);
%MethodCode
        if ((a0 = sipConvertFromSequenceIndex(a0, sipCpp->size())) < 0)
                sipIsErr = 1;
        else
                sipRes = new GroveNode((*sipCpp)[a0]);
%End

    SIP_PYLIST  list() const;   
%MethodCode
        if ((sipRes = PyList_New(sipCpp->size())) == NULL)
            sipIsErr = 1;
        else {
            for (unsigned int i = 0; i < sipCpp->size(); ++i) {
                PyObject* inst = sipBuildResult(&sipIsErr, "N",
                    new GroveNode((*sipCpp)[i].getRep()), sipClass_GroveNode); 
                PyList_SetItem(sipRes, i, inst);
            }
        }
%End
    int __nonzero__();
%MethodCode
        sipRes = !!sipCpp->size();
%End
};

class XpathValue : SimpleWrappedObject {
public:
    XpathValue() /Default/;
    XpathValue(const SString&);
    XpathValue(double);
    XpathValue(const XpathNodeSet&);
    ~XpathValue();

    SString         getString() const;
    double          getDouble() const;
    XpathNodeSet    getNodeSet() const;
    bool            getBool() const;
    
    void            setString(const SString&);
    void            setDouble(double);
    void            setBool(bool);
    void            setNodeSet(const XpathNodeSet&);

    void            notifyChanged();
};

class XpathExpr : SimpleWrappedObject {
public:
    XpathExpr(const SString&,
              const GroveNodeWithNamespace& = GroveNodeWithNamespace(0));
    XpathValue eval(const GroveNode&) const;
};

class XpathValueList {

%TypeHeaderCode
#include "sapi/grove/XpathUtils.h"
using namespace SernaApi;
%End
public:
    void            push_back(const XpathValue&);
    int             size();
    void            erase(int);

XpathValue   operator[](int);
%MethodCode
        if ((a0 = sipConvertFromSequenceIndex(a0, sipCpp->size())) < 0)
                sipIsErr = 1;
        else
                sipRes = new XpathValue((*sipCpp)[a0]);
%End

    SIP_PYLIST  list() const;   
%MethodCode
        if ((sipRes = PyList_New(sipCpp->size())) == NULL)
            sipIsErr = 1;
        else {
            for (unsigned int i = 0; i < sipCpp->size(); ++i) {
                PyObject* inst = sipBuildResult(&sipIsErr, "N",
                    new XpathValue((*sipCpp)[i].getRep()), 
                        sipClass_XpathValue); 
                PyList_SetItem(sipRes, i, inst);
            }
        }
%End
    int __nonzero__();
%MethodCode
        sipRes = !!sipCpp->size();
%End
};

class XsltExternalFunction : SimpleWrappedObject {
public:
    virtual ~XsltExternalFunction();
    virtual XpathValue eval(const XpathValueList&) const;

protected:
    XsltExternalFunction(const SString&, const SString&);
};

