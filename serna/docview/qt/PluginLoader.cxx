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
#include "docview/PluginLoader.h"
#include "docview/SernaDoc.h"

#include "utils/Config.h"
#include "utils/config_defaults.h"
#include "utils/DocSrcInfo.h"

#include "common/SernaApiBase.h"
#include "common/XList.h"
#include "common/XTreePolicies.h"
#include "common/PropertyTree.h"
#include "common/Singleton.h"
#include "common/StringTokenizer.h"
#include "common/StringCvt.h"
#include "common/Url.h"
#include "common/PathName.h"
#include "proputils/PropertyTreeSaver.h"

#include <QApplication>
#include <QLibrary>
#include <QDir>
#include <QStringList>
#include <QObject>

#include <memory>
#include <set>

// START_IGNORE_LITERALS
const char* const PluginLoader::PLUGIN_NAME = "name";
const char* const PluginLoader::PLUGIN_DESC = "shortdesc";
const char* const PluginLoader::LOAD_FOR    = "load-for";
const char* const PluginLoader::PLUGIN_DLL  = "dll";
const char* const PluginLoader::RESOLVED_DLL = "resolved-dll";
const char* const PluginLoader::PLUGIN_VENDOR = "vendor";
const char* const PluginLoader::PLUGIN_DATA = "data";
const char* const PluginLoader::IS_DISABLED = "disabled";
const char* const PluginLoader::PRELOAD_DLL = "preload-dll";
const char* const PluginLoader::RESOLVED_PATH = "resolved-path";
const char* const PluginLoader::SPD_FILE_NAME = "spd-file-name";
// STOP_IGNORE_LITERALS

using namespace Common;

namespace {

class PluginHandle : public XListItem<PluginHandle,
                                      XTreeNodeRefCounted<PluginHandle> >,
                     public RefCounted<> {
public:
    PluginHandle(PropertyNode* pluginProps,
                 SernaApiBase* pluginObj = 0,
                 SernaDoc* sernaDoc = 0)
        : pluginProps_(pluginProps),
          pluginObject_(pluginObj),
          sernaDoc_(sernaDoc) {}

    PropertyNodePtr         pluginProps_;
    SernaApiBase*           pluginObject_;
    SernaDoc*               sernaDoc_;
};

class PluginLoaderImpl : public PluginLoader {
public:
    PluginLoaderImpl();

    Common::PropertyNode*   getPluginProps() const {
        return pluginDesc_.pointer();
    }
    virtual bool            isLoaded(PropertyNode* pluginProps,
                                     const SernaDoc* doc) const;
    virtual bool            isLoaded(const String& pluginName,
                                     const SernaDoc* doc) const;
    virtual bool            isEnabled(PropertyNode* pluginProps) const;


    void                    load(PropertyNode* pluginProps, SernaDoc* doc);
    void                    load(const String& pluginName, SernaDoc* doc);
    void                    loadFor(const String& docClass, SernaDoc* doc);
    void                    checkedLoad(PropertyNode* pn, SernaDoc* doc);
    void                    unloadPlugins(SernaDoc* doc);

    const String&           errorMessage() const { return errMsg_; }

    virtual const SernaApiBase* getPlugin(PropertyNode* pluginProps,
                                          const SernaDoc* doc) const;
private:
    typedef std::set<String> SSet;
    bool                    doLoad(PropertyNode* desc, SernaDoc* doc);
    void                    process_plugins(const String&, SSet&);

    XList<PluginHandle, XTreeNodeRefCounted<PluginHandle> > handles_;
    PropertyNodePtr         pluginDesc_;
    String                  errMsg_;
};

} // namespace

static QFileInfoList get_spd_list(const QDir& plugins_dir)
{
    QFileInfoList result;
    QFileInfoList pdirs = plugins_dir.entryInfoList(QDir::Dirs);
    if (pdirs.empty())
        return result;
    QFileInfoList::const_iterator dit = pdirs.begin();
    for (; dit != pdirs.end(); ++dit) {
        if (dit->isHidden())
            continue;
        QDir pdir(dit->absFilePath());
        QFileInfoList spds = pdir.entryInfoList(NOTR("*.spd"), QDir::Files);
        if (spds.empty())
            continue;
        QFileInfoList::const_iterator pit = spds.begin();
        for (; pit != spds.end(); ++pit)
            result.append(QFileInfo(*pit));
    }
    return result;
}

static void preload_dlls(PropertyNode* pn)
{
#if defined(__linux__) || defined(__sun__)
    pn = pn->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() != PluginLoader::PRELOAD_DLL)
            continue;
        QLibrary* lib = new QLibrary(qApp);
        lib->setFileName(pn->getString());
        (void) lib->resolve(NOTR("_init"));
    }
#endif
}

PluginLoaderImpl::PluginLoaderImpl()
    : pluginDesc_(new PropertyNode(NOTR("plugin-list")))
{
    static char path_sep[] = { PathName::PATH_SEP, 0 };
    String plugins_dir = config().root()->getString("vars/plugins");
    if (plugins_dir.isEmpty())
        plugins_dir = config().getDataDir() + NOTR("/plugins");
    String addtl_plugins_dir(config().root()->
        getString("vars/ext_plugins"));
    SSet processed_dirs;    
    process_plugins(plugins_dir, processed_dirs);
    for (StringTokenizer st(addtl_plugins_dir, path_sep); st; ) {
        String add_dir(st.next());
        if (add_dir.isEmpty())
            continue;
        process_plugins(add_dir, processed_dirs);
    }
}

void PluginLoaderImpl::process_plugins(const String& dir, SSet& processed)
{
    QDir plugins_dir(dir);
    if (!plugins_dir.exists() || processed.find(dir) != processed.end())
        return;
    processed.insert(dir);
    QFileInfoList spds(get_spd_list(plugins_dir));
    QFileInfoList::const_iterator sit = spds.begin();
    for (String spd_path; sit != spds.end(); ++sit) {
        PropertyNodePtr proot(new PropertyNode);
        PropUtils::PropertyTreeSaver pts(
            proot.pointer(), NOTR("serna-plugin"));
        spd_path.assign(from_string<ustring>((*sit).absFilePath()));
        if (!pts.readPropertyTree(spd_path) || !proot)
            continue;
        proot->makeDescendant(RESOLVED_PATH, (*sit).dirPath(true));
        proot->makeDescendant(SPD_FILE_NAME, (*sit).fileName());
        String data_fn = proot->getSafeProperty(PLUGIN_DLL)->getString();
        data_fn = config().resolveResource(String(), data_fn, spd_path);
        if (!data_fn.isEmpty())
            proot->makeDescendant(RESOLVED_DLL)->setString(data_fn);
        proot->makeDescendant(PLUGIN_NAME, (*sit).baseName(), false);
        proot->setName(proot->getProperty(PLUGIN_NAME)->getString());
        pluginDesc_->appendChild(proot.pointer());
    }
    if (pluginDesc_->empty())
        return;

    const PropertyNode* ptn(config().root()->
                            getSafeProperty("app/disabled-plugins"));
    StringTokenizer st(ptn->getString());
    while (st) {
        String disabled_name(st.next());
        PropertyNode* pn = pluginDesc_->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            if (pn->getSafeProperty(PLUGIN_NAME)->getString() == disabled_name)
                pn->makeDescendant(IS_DISABLED)->setBool(true);
        }
    }
}

void PluginLoaderImpl::checkedLoad(PropertyNode* pluginProps, SernaDoc* doc)
{
    String plugin_name(pluginProps->getSafeProperty(PLUGIN_NAME)->getString());
    if (isLoaded(pluginProps, doc))
        return;
    if (doc)
        doc->showStageInfo(tr("Loading plug-in: %1").arg(plugin_name));
    if (!doLoad(pluginProps, doc)) {
        const String& errMsg = errorMessage();
        if (!errMsg.empty() && doc) {
            QString msg(tr("%1\n(Plug-in name: %2)"));
            int button = doc->showMessageBox(SernaDoc::MB_CRITICAL,
                                             tr("Cannot load plug-in"),
                                             msg.arg(errMsg).arg(plugin_name),
                                             tr("&Ok"), tr("&Disable"));
            if (1 == button) {
                pluginProps->makeDescendant(IS_DISABLED)->setBool(true);
                PropertyNode* ptn(config().root()->
                            makeDescendant("app/disabled-plugins"));
                if (ptn->getString().isEmpty())
                    ptn->setString(plugin_name);
                else
                    ptn->setString(ptn->getString() + ' ' + plugin_name);
            }
        }
    }
    if (doc)
        doc->showStageInfo();
}

void PluginLoaderImpl::load(PropertyNode* pluginProps, SernaDoc* doc)
{
    if (isLoaded(pluginProps, doc))
        return;
    if (pluginProps->getSafeProperty(IS_DISABLED)->getBool())
        return;
    checkedLoad(pluginProps, doc);
}

void PluginLoaderImpl::load(const String& pluginName, SernaDoc* doc)
{
    PropertyNode* plugin_props = pluginDesc_->firstChild();
    for (; plugin_props; plugin_props = plugin_props->nextSibling()) {
        if (plugin_props->getSafeProperty(PLUGIN_NAME)->getString()
            == pluginName)
            load(plugin_props, doc);
    }
}

void PluginLoaderImpl::loadFor(const String& docClass, SernaDoc* doc)
{
    using namespace DocSrcInfo;

    PropertyNode* pn = pluginDesc_->firstChild();
    String t_category, t_name;
    const PropertyNode* dsi = 0;
    if (doc && (dsi = doc->getDsi())) {
        t_category = dsi->getSafeProperty(TEMPLATE_CATEGORY)->getString();
        t_name     = dsi->getSafeProperty(TEMPLATE_NAME)->getString();
    }
    for (; pn; pn = pn->nextSibling()) {
        if (pn->getSafeProperty(IS_DISABLED)->getBool())
            continue;
        const PropertyNode* load_for = pn->getProperty(LOAD_FOR);
        if (0 == load_for)
            continue;
        if (!load_for->getString().isEmpty()) {
            for (StringTokenizer st(load_for->getString()); st; ) {
                if (docClass == st.next()) {
                    checkedLoad(pn, doc);
                    break;
                }
            }
            continue;
        }
        const PropertyNode* type_prop = load_for->firstChild();
        for (; type_prop; type_prop = type_prop->nextSibling()) {
            if (type_prop->name() == NOTR("mode") &&
                type_prop->getString() == docClass) {
                    checkedLoad(pn, doc);
                    break;
            } else if (type_prop->name() == NOTR("template") &&
              docClass == NOTR("wysiwyg-mode")) {
                const PropertyNode* name_prop =
                    type_prop->getProperty(TEMPLATE_NAME);
                if ((name_prop && name_prop->getString() != t_name) ||
                    t_category != type_prop->getSafeProperty
                        (TEMPLATE_CATEGORY)->getString())
                            continue;
                checkedLoad(pn, doc);
                break;
            }
        }
    }
}

bool PluginLoaderImpl::doLoad(PropertyNode* pluginProps, SernaDoc* doc)
{
    String plugin_name =
        pluginProps->getSafeProperty(PLUGIN_NAME)->getString();
    PropertyNode* dll_prop = pluginProps->getProperty(RESOLVED_DLL);
    if (0 == dll_prop || dll_prop->getString().isEmpty()) {
        errMsg_ = tr("No plugin DLL specified for plugin '%1'"
                     " (or DLL not found)").arg(plugin_name);
        return false;
    }
    preload_dlls(pluginProps);
    OwnerPtr<QLibrary> dylib(new QLibrary(qApp));
    dylib->setFileName(dll_prop->getString());
    if (!dylib->load()) {
        errMsg_ = tr("DLL '%1' for the plugin '%2' cannot be loaded")
                  .arg(dylib->fileName()).arg(plugin_name);
        return false;
    }
    if (0 == doc) {  // startup mode, do not need init_serna_plugin
        dylib.release();
        return true;
    }
    typedef SernaApiBase* (*init_funcp)(SernaApiBase*, SernaApiBase*, char**);
    init_funcp fp = 0;
    try { //core in release(win) on trying resolve dynamic debug build plugin
        fp = (init_funcp) dylib->resolve(NOTR("init_serna_plugin"));
    }
    catch(...) {}
    if (0 == fp) {
        errMsg_ = tr("DLL '%1' is not found \nor does not have "
                     "init_serna_plugin function").arg(dylib->fileName());
        dylib->unload();
        return false;
    }
    char* errMsgBuf = 0;
    OwnerPtr<SernaApiBase> plugin_obj;
    QString cant_init =
        tr("Plugin '%1' cannot be initialized").arg(plugin_name);
    try {
        plugin_obj = (*fp)(doc, pluginProps, &errMsgBuf);
    }
    catch (std::exception& e) {
        errMsg_ = cant_init +
            QString(NOTR("\n(caught std::exception: %1)")).arg(e.what());
        if (errMsgBuf) {
            errMsg_ += QString(NOTR("\n[%1]")).arg(errMsgBuf);
            delete[] errMsgBuf;
        }
        return false;
    }
    catch (...) {
        errMsg_ = cant_init + NOTR(" (unknown exception)");
        if (errMsgBuf) {
            errMsg_ += QString(NOTR("\n[%1]")).arg(errMsgBuf);
            delete[] errMsgBuf;
        }
        return false;
    }
    if (0 == plugin_obj.pointer()) {
        if (0 != errMsgBuf) {
            errMsg_ = errMsgBuf;
            delete[] errMsgBuf;
        }
        else {
            if ('\0' != errMsgBuf[0])
                errMsg_ = cant_init;
            else
                errMsg_.clear();
        }
        dylib->unload();
        return false;
    }
    handles_.appendChild(new PluginHandle(
                             pluginProps, plugin_obj.pointer(), doc));
    plugin_obj.release();
    dylib.release();
    return true;
}

void PluginLoaderImpl::unloadPlugins(SernaDoc* doc)
{
    PluginHandle* ph = handles_.firstChild();
    while (ph) {
        if (ph->sernaDoc_ == doc) {
            delete ph->pluginObject_;
            PluginHandle* nextPh = ph->nextSibling();
            ph->remove();
            ph = nextPh;
        }
        else
            ph = ph->nextSibling();
    }
}

bool PluginLoaderImpl::isLoaded(PropertyNode* pluginProps,
                                const SernaDoc* doc) const
{
    return 0 != getPlugin(pluginProps, doc);
}

bool PluginLoaderImpl::isLoaded(const String& pluginName,
                                const SernaDoc* doc) const
{
    PluginHandle* handle = handles_.firstChild();
    for (; handle; handle = handle->nextSibling())
        if (handle->sernaDoc_ == doc &&
            handle->pluginProps_->getSafeProperty(PLUGIN_NAME)->getString()
            == pluginName)
            return true;
    return false;
}

const SernaApiBase* PluginLoaderImpl::getPlugin(PropertyNode* pluginProps,
                                                const SernaDoc* doc) const
{
    PluginHandle* handle = handles_.firstChild();
    for (; handle; handle = handle->nextSibling())
        if (handle->sernaDoc_ == doc &&
            handle->pluginProps_.pointer() == pluginProps)
            return handle->pluginObject_;
    return 0;
}

bool PluginLoaderImpl::isEnabled(PropertyNode* pluginProps) const
{
    return !pluginProps->getSafeProperty(IS_DISABLED)->getBool();
}

PluginLoader& pluginLoader()
{
    return SingletonHolder<PluginLoaderImpl>::instance();
}

