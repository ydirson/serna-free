// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveIdManager : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/GroveIdManager.h"
using namespace SernaApi;
%End

public:
    void            getIdList(PropertyNode&) const;
    GroveElement    lookupElement(const SString&) const;
private:
    GroveIdManager();
};

