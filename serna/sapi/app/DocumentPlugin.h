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
#ifndef SAPI_DOCUMENT_PLUGIN_H_
#define SAPI_DOCUMENT_PLUGIN_H_

#include "sapi/app/UiDocument.h"

namespace SernaApi {

class DocumentPlugin;
class SernaDoc;
class GroveNode;
class StructEditorData;

/// Execution of User Interface (UI) commands.
/*! UiEventExecutor is responsible for execution of
   User Interface (UI) commands, like menu, buttons and so on.*/
class SAPI_EXPIMP UiEventExecutor : public SernaApiBase {
public:
    UiEventExecutor(const UiAction& uic)
        : uic_(uic) {}
    ~UiEventExecutor();

    /// Returns UI command, emited by buttons, menu items and widgets.
    UiAction uiAction() const { return uic_; }

    /// implement this method in subclass to control execution process.
    virtual void execute() = 0;

private:
    UiAction uic_;
};

typedef UiEventExecutor* (*PluginEventExecutorMaker)
    (const UiAction& cmd, DocumentPlugin& plugin);

/// The base class for any Serna plugin.
class SAPI_EXPIMP DocumentPlugin : public SernaApiBase,
                                   public RefCountedWrappedObject {
public:
    DocumentPlugin(SernaApiBase* sernaDoc, SernaApiBase* pluginProperties);
    virtual ~DocumentPlugin();

    /// Returns pointer to the SernaDoc instance corresponding to the
    /// current document
    SernaDoc        sernaDoc() const;

    /// Returns plugin name (taken from PLUGIN_NAME property)
    SString         pluginName() const;

    /// Returns root of the property tree corresponding to the plugin
    /// data (section "data" in the .spd file)
    PropertyNode    pluginProperties() const;

    /// Build all plugin executors from the plugin file description
    /// (taken from .spd file). If \a autoRegister is TRUE, then all
    /// UI actions described in ui/uiActions will be automatically
    /// registered for local execution (via executeUiEvent()).
    void            buildPluginExecutors(bool autoRegister = false);

    /// Registers maker-function for plugin UI executors
    void            registerExecutor(const SString& name,
                                     PluginEventExecutorMaker = 0);

    /// This function builds plugin interface from the plugin definition
    /// file. It is called automatically when document UI is being constructed
    /// or re-loaded by the Customizer. At this point document is not yet
    /// in the UI tree.
    virtual void    buildPluginInterface();

    /// called after building the interface, only once (when document
    /// is opened). When postInit() is called, document is already in the
    /// UI tree.
    virtual void    postInit() {}

    /// called on close operation, to check whether document can be closed
    /// (for example to check if there is no unsaved data)
    virtual bool    preClose() { return true; }

    /// called just before document is saved.
    virtual void    aboutToSave() {}

    /// called when new document is created.
    virtual void    newDocumentGrove() {}

    /// called just before XSLT transform on main document
    virtual void    beforeTransform() {}

    /// Reimplement this to execute UI commands which was registered with
    /// null executor maker
    virtual void    executeUiEvent(const SString& cmdEvent, const UiAction&);

    /// Create single UI item with specified properties
    UiItem          makeUiItem(const SString& name,
                               const PropertyNode& properties) const;

    /// Build UI item with specified name from the ui section of the
    /// plugin description file (.spd)
    UiItem          buildUiItem(const SString& itemName) const;

    /// Event executors
    bool            executeCommandEvent(const SString& name) const;
    /// Event executors
    bool            executeCommandEvent(const SString& name,
                                        const PropertyNode& in) const;
    /// Event executors
    bool            executeCommandEvent(const SString& name,
                                        const PropertyNode& in,
                                        const PropertyNode& out) const;

    /// Returns propery tree for the given plugin suffix
    PropertyNode        getPropTree(const SString& suffix) const;

    /// Creates and registers property tree for the given plugin suffix.
    /// Once property tree is registered, it will be automatically read
    /// (and saved, when possible) in suffix file on document close/save events.
    PropertyNode        registerPropTree(const SString& suffix,
                                         const SString& propName = SString());

    /// Returns version string of the Serna application, e.g. '3.6.0'
    static SString      sernaVersion();

    // -----------------------------------------------------------------

    // for internal use only - don't use
    DocumentPlugin(long, long);
    DocumentPlugin(unsigned long);

#ifndef BUILD_SAPI
private:
#endif
    StructEditorData* sd_;
};

/// The base class for Event Executors.
class SAPI_EXPIMP UiEventExecutorBase : public UiEventExecutor {
public:
    UiEventExecutorBase(const UiAction& cmd,
                        DocumentPlugin& dp)
        : UiEventExecutor(cmd), docPlugin_(&dp) {}
    virtual ~UiEventExecutorBase() {}

protected:
    DocumentPlugin* docPlugin_;
};

/// For internal use only.
template <class PluginType> class UiEventExecutorImpl :
                                  public UiEventExecutorBase {
public:
    UiEventExecutorImpl(const UiAction& cmd, DocumentPlugin& dp)
        : UiEventExecutorBase(cmd, dp) {}
    /// Returns plugin object
    PluginType* plugin() const { return static_cast<PluginType*>(docPlugin_); }
};

template <class T> UiEventExecutor*
    makePluginUiExecutor(const UiAction& cmd, DocumentPlugin& dp);

#define SAPI_PLUGIN_EXECUTOR_IMPL(ExecutorName, PluginType) \
class ExecutorName :  public SernaApi::UiEventExecutorImpl<PluginType> { \
public:                                           \
    typedef SernaApi::UiEventExecutorImpl<PluginType> UIT; \
    ExecutorName(const SernaApi::UiAction& cmd, \
                SernaApi::DocumentPlugin& plugin) \
        : UIT(cmd, plugin) {} \
    virtual void execute(); \
}; \
namespace SernaApi { \
template <> SernaApi::UiEventExecutor* \
makePluginUiExecutor<ExecutorName>(const SernaApi::UiAction& cmd,\
                                   SernaApi::DocumentPlugin& dp) \
    { return new ExecutorName(cmd, dp); } \
} // namespace

#define SAPI_REGISTER_UI_EXECUTOR(ExecutorName) \
  registerExecutor(#ExecutorName, SernaApi::makePluginUiExecutor<ExecutorName>)

template <class PluginClass> SernaApiBase*
sapi_init_plugin_aux(SernaApiBase* props, SernaApiBase* sernaDoc, char** err)
{
    try {
        return new PluginClass(props, sernaDoc, err);
    }
    catch (...) {}
    return 0;
}

#if defined(PLUGIN_EXPORT)
# undef PLUGIN_EXPORT
#endif

#if defined(__APPLE__)
# define PLUGIN_EXPORT __attribute__((visibility("default")))
#else
# define PLUGIN_EXPORT
#endif

#define SAPI_DEFINE_PLUGIN_CLASS(className) \
extern "C" SAPI_EXPORT PLUGIN_EXPORT SernaApiBase* \
init_serna_plugin(SernaApiBase* props,\
                  SernaApiBase* sernaDoc, char** err) \
{ return SernaApi::sapi_init_plugin_aux<className>(props, sernaDoc, err); }

} // namespace SernaApi

#endif // SAPI_DOCUMENT_PLUGIN_H_
