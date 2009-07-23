// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
#ifndef SAPI_PLUGIN_LOADER_H_
#define SAPI_PLUGIN_LOADER_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/common/WrappedObject.h"

namespace SernaApi {
;
class PropertyNode;
class SernaDoc;
class DocumentPlugin;
    
/// Provides information about registered plugins
class SAPI_EXPIMP PluginLoader  : public SimpleWrappedObject {
public:
    PluginLoader(SernaApiBase* = 0);
    ~PluginLoader();

    /// Plugin property names. There are children of plugin property node.
    /// Plugins are resolved as $SERNA_PLUGINS/{PLUGIN_NAME}.spd
    static const SString PLUGIN_NAME;   // default = basename of .spd file
    static const SString PLUGIN_DESC;
    static const SString LOAD_FOR;
    static const SString PLUGIN_DLL;    // path to DLL
    static const SString RESOLVED_DLL;  // resolved at startup
    static const SString PLUGIN_VENDOR;
    static const SString PLUGIN_DATA;
    static const SString IS_DISABLED;   // taken from config
    static const SString PRELOAD_DLL;   // load/execute _init from dll
    static const SString RESOLVED_PATH; // resolved path to plugin dir
    static const SString SPD_FILE_NAME; // .spd file name

    /// Accessors to plugin descriptors. Children of returned property
    /// are "serna-plugin" property subtrees.
    PropertyNode        getPluginProps() const;

    /// Returns true if requested plugin is enabled in Serna
    bool                isEnabled(const PropertyNode& pluginProps) const;

    /// Returns true if requested plugin is loaded for given document
    bool                isLoaded(const PropertyNode& pluginProps, 
                                 const SernaDoc& doc) const;

    /// Returns reference to PluginLoader instance
    static PluginLoader instance();

    PluginLoader(const PluginLoader&);
    PluginLoader& operator=(const PluginLoader&);
};

} // namespace SernaApi

#endif // SAPI_PLUGIN_LOADER_H_
