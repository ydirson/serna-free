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
#include "sapi/sapi_defs.h"
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/app/impl/StructEditorData.h"

#include "docview/SernaDoc.h"
#include "docview/DocumentPlugin.h"
#include "common/OwnerPtr.h"
#include "common/Vector.h"
#include "grove/Node.h"
#include "utils/Version.h"
#include <map>

#define DP  static_cast<DocumentPluginWrap*>(getRep())

namespace {

typedef std::map<Common::String, SernaApi::PluginEventExecutorMaker>
    SapiExecutorMakerMap;

typedef std::map<Common::String, SernaApi::UiItemMaker*> SapiUiItemMakerMap;

class DocumentPluginWrap : public ::DocumentPlugin {
public:
    DocumentPluginWrap(SernaApiBase* rep,
                       SernaApiBase* props,
                       SernaApi::DocumentPlugin* plugin)
        : DocumentPlugin(rep, props), plugin_(plugin) {}

    virtual void postInit() { plugin_->postInit(); }
    virtual bool preClose() { return plugin_->preClose(); }
    virtual void aboutToSave() { plugin_->aboutToSave(); }
    virtual void newDocumentGrove() { plugin_->newDocumentGrove(); }
    virtual void beforeTransform() { plugin_->beforeTransform(); }
    virtual void buildPluginInterface()
    {
        plugin_->buildPluginInterface();
    }
    SernaApi::DocumentPlugin* plugin_;
    SapiExecutorMakerMap      makerMap_;
    SapiUiItemMakerMap        itemMakers_;

    virtual ~DocumentPluginWrap() 
    {
        // remove UI item makers, and deregister corresponding item makers
        // from Sui::Document
        SapiUiItemMakerMap::iterator i = itemMakers_.begin();
        SernaDoc* sd = static_cast<SernaDoc*>(plugin_->sernaDoc().getRep());
        for (; i != itemMakers_.end(); ++i) {
            sd->removeItemMaker(i->first);
            delete i->second;
        }
    }
};

class UiExecutorWrap : public ::ActionExecutor {
public:
    UiExecutorWrap(Sui::Action* cmd, SernaApi::UiEventExecutor* uee)
        : ActionExecutor(cmd), uee_(uee) {}

    virtual void execute() { uee_->execute(); }

private:
    Common::OwnerPtr<SernaApi::UiEventExecutor> uee_;
};

class DirectPluginUiExecutor : public ::ActionExecutor {
public:
    DirectPluginUiExecutor(Sui::Action* cmd, SernaApi::DocumentPlugin* dp)
        : ActionExecutor(cmd), plugin_(dp) {}
    virtual void execute()
    {
        plugin_->executeUiEvent(uiAction()->get("commandEvent"),
                SernaApi::UiAction(uiAction()));
    }
private:
    SernaApi::DocumentPlugin* plugin_;
};

} // namespace

namespace SernaApi {

//////////////////////////////////////////////////////////////////////

DocumentPlugin::DocumentPlugin(SernaApiBase* rep, SernaApiBase* props)
    : sd_(0)
{
    setRep(new DocumentPluginWrap(rep, props, this));
}

DocumentPlugin::DocumentPlugin(long rep, long props)
    : sd_(0)
{
    setRep(new DocumentPluginWrap((SernaApiBase*)rep,
        (SernaApiBase*) props, this));
}

DocumentPlugin::DocumentPlugin(unsigned long vptr)
    : sd_(0)
{
    //std::cerr << "altcons\n";
    setRep(new DocumentPluginWrap(((SernaApiBase**)vptr)[0],
        ((SernaApiBase**)vptr)[1], this));
    ((void**)vptr)[2] = this;
}

SString DocumentPlugin::pluginName() const
{
    return SString(DP->pluginName());
}

PropertyNode DocumentPlugin::pluginProperties() const
{
    return PropertyNode(const_cast<Common::PropertyNode*>
        (DP->pluginProperties()));
}

void DocumentPlugin::buildPluginExecutors(bool autoRegister)
{
    if (autoRegister) {
        const Common::PropertyNode* pn =
            DP->pluginProperties()->getProperty("ui/uiActions");
        if (pn)
            pn = pn->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            Common::String name =
                pn->getSafeProperty("commandEvent")->getString();
            if (!name.isEmpty())
                registerExecutor(name);
        }
    }
    DP->buildPluginExecutors();
}

void DocumentPlugin::buildPluginInterface()
{
    DP->::DocumentPlugin::buildPluginInterface();
}

SernaDoc DocumentPlugin::sernaDoc() const
{
    return SernaDoc(DP->sernaDoc(), const_cast<DocumentPlugin*>(this));
}

UiItem DocumentPlugin::buildUiItem(const SString& itemName) const
{
    return sernaDoc().buildUiItem(pluginProperties().getProperty("ui"),
        itemName);
}

::ActionExecutor* proxy_executor_maker(Sui::Action* cmd,
                                          ::DocumentPlugin* sdp)
{
    DocumentPluginWrap* dpwrap = static_cast<DocumentPluginWrap*>(sdp);
    SapiExecutorMakerMap::const_iterator it = dpwrap->makerMap_.
        find(cmd->properties()->getSafeProperty("commandEvent")->getString());
    if (it == dpwrap->makerMap_.end())
        return 0;
    if (it->second)
        return new UiExecutorWrap(cmd, (*it->second)(cmd, *dpwrap->plugin_));
    else
        return new DirectPluginUiExecutor(cmd, dpwrap->plugin_);
}

void DocumentPlugin::registerExecutor(const SString& name,
                                      PluginEventExecutorMaker m)
{
    DP->registerExecutor(name, proxy_executor_maker);
    DP->makerMap_[name] = m;
}

void DocumentPlugin::executeUiEvent(const SString&, const UiAction&)
{
}

UiItem DocumentPlugin::makeUiItem(const SString& name,
                                  const PropertyNode& properties) const
{
    return DP->makeUiItem(name,
        static_cast<Common::PropertyNode*>(properties.getRep())).pointer();
}

bool DocumentPlugin::executeCommandEvent(const SString& name) const
{
    return DP->executeCommandEvent(name);
}

bool DocumentPlugin::executeCommandEvent(const SString& name,
                                         const PropertyNode& in) const
{
    return DP->executeCommandEvent(name,
        static_cast<Common::PropertyNode*>(in.getRep()));
}

bool DocumentPlugin::executeCommandEvent(const SString& name,
                                         const PropertyNode& in,
                                         const PropertyNode& out) const
{
    return DP->executeCommandEvent(name,
        static_cast<Common::PropertyNode*>(in.getRep()),
        static_cast<Common::PropertyNode*>(out.getRep()));
}

///////////////////////////////////////////////////////////////////////

PropertyNode DocumentPlugin::getPropTree(const SString& suffix) const
{
    const Common::PropertyNode* ptree = DP->getPropTree(suffix);
    return PropertyNode(const_cast<Common::PropertyNode*>(ptree));
}

PropertyNode DocumentPlugin::registerPropTree(const SString& suffix,
                                              const SString& propName)
{
    Common::PropertyNode* ptree = DP->registerPropTree(suffix, propName);
    return PropertyNode(ptree);
}

SString DocumentPlugin::sernaVersion()
{
    return Version::version();
}

///////////////////////////////////////////////////////////////////////

DocumentPlugin::~DocumentPlugin()
{
    if (sd_)
        delete sd_;
}

UiEventExecutor::~UiEventExecutor()
{
}

void sapi_add_item_maker(UiDocument* uidoc,
                        const SString& itemClass,
                        UiItemMaker* maker)
{
    DocumentPlugin* plugin_inst = static_cast<SernaDoc*>(uidoc)->plugin();
    if (!plugin_inst || !plugin_inst->getRep())
        return;
    DocumentPluginWrap* dwp =
        static_cast<DocumentPluginWrap*>(plugin_inst->getRep());
    SapiUiItemMakerMap::iterator i = dwp->itemMakers_.find(itemClass);
    if (i != dwp->itemMakers_.end()) {
        delete i->second;
        i->second = maker;  // redefinition
    } else
        (dwp->itemMakers_)[itemClass] = maker;
}

///////////////////////////////////////////////////////////////////////

} // namespace SernaApi
