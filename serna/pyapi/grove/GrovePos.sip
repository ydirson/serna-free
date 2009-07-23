// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GrovePos {

%TypeHeaderCode
#include "sapi/grove/GrovePos.h"
using namespace SernaApi;
%End

public:
    enum Type { UNKNOWN_POS, ELEMENT_POS, TEXT_POS };
    GrovePos();
    GrovePos(const GroveNode&);
    GrovePos(const GroveNode&, const GroveNode&);
    GrovePos(const GroveText&, long);
    GrovePos(const SString&, const GroveNode&);
    SString         asString() const;
    bool            isNull() const;
    Type            type() const;
    GroveNode       node() const;
    GroveNode       before() const;
    GroveText       text() const;
    long            idx() const;
    GroveErs        getErs() const;
    void            dump() const;
    
    int __nonzero__();
%MethodCode
        sipRes = !sipCpp->isNull();
%End

    GrovePos(const GrovePos&);
};

