// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class PluginLoader : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/PluginLoader.h"
#include "sapi/app/SernaDoc.h"
using namespace SernaApi;
%End

public:
    static const SString PLUGIN_NAME;
    static const SString PLUGIN_DESC;
    static const SString LOAD_FOR;
    static const SString PLUGIN_DLL; 
    static const SString RESOLVED_DLL;
    static const SString PLUGIN_VENDOR;
    static const SString PLUGIN_DATA;
    static const SString IS_DISABLED; 
    static const SString PRELOAD_DLL; 
    static const SString RESOLVED_PATH;
    static const SString SPD_FILE_NAME;

    PropertyNode        getPluginProps() const;
    bool                isEnabled(const SString&) const;
    bool                isLoaded(const SString&, const SernaDoc&) const;
    static PluginLoader instance();
    PluginLoader(const PluginLoader&);
};
