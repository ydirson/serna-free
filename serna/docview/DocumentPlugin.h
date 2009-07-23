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
#ifndef SERNA_DOCUMENT_PLUGIN_H_
#define SERNA_DOCUMENT_PLUGIN_H_

#include "docview/dv_defs.h"
#include "docview/SernaDoc.h"

#include "common/SernaApiRefCounted.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/Vector.h"
#include "common/i18n_translate.h"

class DocumentPlugin;
class MessageTreeNode;

typedef ActionExecutor* (*PluginUiExecutorMaker)
    (Sui::Action* cmd, DocumentPlugin* plugin);

////////////////////////////////////////////////////////////////////////////

class DOCVIEW_EXPIMP DocumentPlugin : public Common::SernaApiRefCounted,
                                      public Common::CommandEventContext {
public:
    typedef Common::String          String;
    typedef Common::PropertyNode    PropertyNode;

    DocumentPlugin(SernaApiBase* sernaDoc, SernaApiBase* properties);
    virtual ~DocumentPlugin();

    /// Returns plugin name (taken from PLUGIN_NAME property)
    const String&       pluginName() const { return pluginName_; }
    //! Properties shared between all plugin instances
    const PropertyNode* pluginProperties() const
    {
        return pluginProperties_.pointer();
    }
    SernaDoc*           sernaDoc() const { return sernaDoc_; }

    ActionExecutor*     buildPluginExecutor(const PropertyNode* prop);
    void                buildPluginExecutors();

    /// This function builds plugin interface from the plugin definition
    /// file. It is called automatically when document UI is being constructed
    /// or re-loaded by the Customizer. At this point document is not yet
    /// in the UI tree.
    virtual void        buildPluginInterface();

    /// called after building the interface, only once (when document
    /// is opened). When postInit() is called, document is already in the
    /// UI tree.
    virtual void        postInit();

    /// called on close operation, to check whether document can be closed
    /// (for example to check if there is no unsaved data)
    virtual bool        preClose() { return true; }

    /// called just before document is saved.
    virtual void        aboutToSave() {}

    /// called when creating a new document
    virtual void        newDocumentGrove() {}

    /// called just before XSLT transform
    virtual void        beforeTransform() {}

    void                registerExecutor(const String& name,
                                         PluginUiExecutorMaker);

    /// Finds and returns registered executor by name
    ActionExecutor*     findPluginExecutor(const String& name) const;

    /// Convinience function to find plugin command (with plugin name
    /// prefix added automatically)
    Sui::Action*         findPluginUiAction(const String& name) const;

    /// Convinience function to create UI items with plugin origin set
    Sui::ItemPtr         makeUiItem(const String& name,
                                   PropertyNode* properties);
    /// Event executors
    bool                executeCommandEvent(const String& name);
    bool                executeCommandEvent(const String& name,
                                            PropertyNode* in,
                                            PropertyNode* out = 0);
    const PropertyNode* getPropTree(const String& suffix) const;
    PropertyNode*       registerPropTree(const String& suffix,
                                         const String& propName = String());
    void                getPropTreeSuffixes(PropertyNode* suffixList) const;
    virtual void        saveProperties(const String& path);
    virtual void        restoreProperties(const PropertyNode*);

private:
    virtual const char* selfTypeId() const
    {
        return typeid(DocumentPlugin).name();
    }
private:
    class ExecutorMakerMap;
    class ExecutorMap;
    SernaDoc*                               sernaDoc_;
    Common::PropertyNodePtr                 pluginProperties_;
    Common::OwnerPtr<ExecutorMakerMap>      executorMakerMap_;
    Common::OwnerPtr<ExecutorMap>           executorMap_;
    Common::Vector<Sui::ActionPtr>           emptyActions_;
    Common::String                          pluginName_;
    MessageTreeNode*                        messageTree_;

    class PropTreeMap;
    Common::OwnerPtr<PropTreeMap>           suffixMap_;
};

////////////////////////////////////////////////////////////////////////////

class DOCVIEW_EXPIMP PluginUiExecutorBase : public ActionExecutor {
public:
    PluginUiExecutorBase(Sui::Action* cmd, DocumentPlugin* dp)
        : ActionExecutor(cmd),
          docPlugin_(dp) {}

    virtual ~PluginUiExecutorBase();

protected:
    DocumentPlugin* docPlugin_;
};

////////////////////////////////////////////////////////////////////////////

template <class PluginType> class PluginUiExecutor :
                                  public PluginUiExecutorBase {
public:
    PluginUiExecutor(Sui::Action* cmd, DocumentPlugin* dp)
        : PluginUiExecutorBase(cmd, dp) {}

    PluginType* plugin() const { return static_cast<PluginType*>(docPlugin_); }
};

template <class T>
ActionExecutor* makeUiPluginExecutor(Sui::Action* cmd, DocumentPlugin* dp);

////////////////////////////////////////////////////////////////////////////

#define SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(ExecutorName, PluginType) \
class ExecutorName :  public PluginUiExecutor<PluginType> { \
public:                                           \
    ExecutorName(Sui::Action* cmd, DocumentPlugin* plugin) \
        : PluginUiExecutor<PluginType>(cmd, plugin) {} \
    virtual void execute(); \
    DECLARE_I18N_TRANSLATE_FUNCTIONS(ExecutorName) \
}; \
template <> ActionExecutor* \
makeUiPluginExecutor<ExecutorName>(Sui::Action* cmd, DocumentPlugin* dp) \
    { return new ExecutorName(cmd, dp); }

////////////////////////////////////////////////////////////////////////////

#define REGISTER_UI_EXECUTOR(ExecutorName) \
    registerExecutor(#ExecutorName, makeUiPluginExecutor<ExecutorName>)

template<class PluginClass> SernaApiBase*
init_serna_plugin_aux(SernaApiBase* props, SernaApiBase* sernaDoc, char** err)
{
    *err = 0;
    return new PluginClass(props, sernaDoc, err);
}

////////////////////////////////////////////////////////////////////////////
#if defined(PLUGIN_EXPORT)
# undef PLUGIN_EXPORT
#endif

#if defined(__APPLE__)
# define PLUGIN_EXPORT __attribute__((visibility("default")))
#else
# define PLUGIN_EXPORT
#endif

#define DEFINE_PLUGIN_CLASS(className) \
extern "C" COMMON_EXPORT PLUGIN_EXPORT SernaApiBase* \
init_serna_plugin(SernaApiBase* props, SernaApiBase* sernaDoc, char** err) \
{ return init_serna_plugin_aux<className>(props, sernaDoc, err); }

#endif // SERNA_DOCUMENT_PLUGIN_H_
