// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveStripInfo : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/GroveStripInfo.h"
using namespace SernaApi;
%End

public:
    GroveStripInfo();
    bool checkStrip(const GroveElement&) const;
    void strip(const GroveNode&) const;
};
