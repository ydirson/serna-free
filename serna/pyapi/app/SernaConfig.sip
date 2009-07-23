// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class SernaConfig {
   
%TypeHeaderCode
#include "sapi/app/Config.h"
using namespace SernaApi;
%End
 
public:
    static PropertyNode root();
    static SString      getProperty(const SString&);
    static SString      getHomeDir();
    static SString      getDataDir();
    static SString      getConfigDir();
    static SString      resolveResource(const SString&,
            const SString&, const SString&,
            const PropertyNode& = PropertyNode());
};
