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
#ifndef DOCVIEW_PLUGIN_LOADER_H_
#define DOCVIEW_PLUGIN_LOADER_H_

#include "docview/dv_defs.h"
#include "common/SernaApiBase.h"
#include "common/PropertyTree.h"

class SernaDoc;

class DOCVIEW_EXPIMP PluginLoader : public SernaApiBase {
public:
    /// Plugin property names. There are children of plugin property node.
    /// Plugins are resolved as $SERNA_PLUGINS/{PLUGIN_NAME}.spd
    static const char* const PLUGIN_NAME;   // default = basename of .spd file
    static const char* const PLUGIN_DESC;
    static const char* const LOAD_FOR;
    static const char* const PLUGIN_DLL;    // path to DLL
    static const char* const RESOLVED_DLL;  // resolved at startup
    static const char* const PLUGIN_VENDOR;
    static const char* const PLUGIN_DATA;
    static const char* const IS_DISABLED;   // taken from config
    static const char* const PRELOAD_DLL;   // load/execute _init from dll
    static const char* const RESOLVED_PATH; // resolved path to plugin dir
    static const char* const SPD_FILE_NAME; // .spd file name

    /// Accessors to plugin descriptors. Children of returned property
    /// are "serna-plugin" property subtrees.
    virtual Common::PropertyNode* getPluginProps() const = 0;

    /// Load plugin with given properties
    virtual void    load(Common::PropertyNode* pluginProps, SernaDoc* doc) = 0;

    /// Load all plugins with particular name
    virtual void    load(const Common::String& name, SernaDoc* doc) = 0;

    /// Load plugin for document class
    /// (StructEditor, PlainEditor, EmptyDocument)
    virtual void    loadFor(const Common::String& docClass, SernaDoc* doc) = 0;

    /// Unload plugins for given SernaDoc
    virtual void    unloadPlugins(SernaDoc* doc) = 0;

    virtual bool    isEnabled(Common::PropertyNode* pluginProps) const = 0;

    /// Returns true if specific plugin is loaded
    virtual bool    isLoaded(Common::PropertyNode* pluginProps,
                             const SernaDoc* doc) const = 0;

    /// Returns true if any plugin with given name is loaded
    virtual bool    isLoaded(const Common::String& pluginName,
                             const SernaDoc* doc) const = 0;
    virtual const SernaApiBase*   getPlugin(Common::PropertyNode* pluginProps,
                                            const SernaDoc* doc) const = 0;

    virtual const Common::String& errorMessage() const = 0;

    virtual ~PluginLoader() {}
};

DOCVIEW_EXPIMP PluginLoader& pluginLoader();

#endif // DOCVIEW_PLUGIN_LOADER_H_

