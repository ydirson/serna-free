// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class SernaClipboard {
   
%TypeHeaderCode
#include "sapi/app/Clipboard.h"
using namespace SernaApi;
%End
 
public:
    static void                     setText(bool, const SString&);
    static SString                  getText(bool, const GroveStripInfo&);
    static GroveDocumentFragment    getFragment(bool = false);
    static void                     setFragment(bool, GroveDocumentFragment&);
};
