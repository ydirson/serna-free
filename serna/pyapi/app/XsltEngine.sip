// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class XsltEngine : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/grove/XpathUtils.h"
#include "sapi/app/XsltEngine.h"
using namespace SernaApi;
%End
public:
    void        registerExternalFunction(const XsltExternalFunction&);
    void        deregisterExternalFunction(const XsltExternalFunction&);
private:
    XsltEngine();
};

