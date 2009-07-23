// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/common/WrappedObject.h"
using namespace SernaApi;
%End
public:
    bool    isNull() const;

    int __nonzero__();
%MethodCode
        sipRes = !sipCpp->isNull();
%End
    int __int__();
%MethodCode
        sipRes = (int)sipCpp->getRep();
%End
    bool operator==(const SimpleWrappedObject&) const;
    bool operator!=(const SimpleWrappedObject&) const;
};
