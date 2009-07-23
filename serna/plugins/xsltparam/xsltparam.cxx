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
#include "xsltparam.h"
#include "qt/XsltParamsDialog.h"
#include "docview/DocumentPlugin.h"
#include "structeditor/StructEditor.h"
#include "proputils/PropertyTreeSaver.h"
#include "common/PropertyTreeEventData.h"
#include "common/PackedPtrSet.h"
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"
#include "ui/UiProps.h"

#include <iostream>

using namespace Common;
using namespace XsltParam;

namespace XsltParam
{
// START_IGNORE_LITERALS
const char* const PARAMS_ROOT   = "parameterSets";
const char* const PARAM_SET     = "paramSet";
const char* const PARAMS        = "params";
const char* const DESCRIPTION   = "description";
const char* const ANNOTATION    = "annotation";
const char* const NAME          = "name";
const char* const TYPE          = "type";
const char* const VALUE         = "value";
const char* const CURRENT       = "current";
const char* const GUESSED_TYPE  = "guessed-type";
const char* const SUFFIX        = ".xsltp";
// STOP_IGNORE_LITERALS
const char* const DEF_COMBO     = QT_TR_NOOP("Default View");
}

class XsltParamPlugin;

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(ShowXsltParamsDialog, XsltParamPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(XsltParamAction,      XsltParamPlugin)

///////////////////////////////////////////////////////////////

class XsltParamPlugin : public DocumentPlugin {
public:
    XsltParamPlugin(SernaApiBase* ssd, SernaApiBase* properties, char**)
        : DocumentPlugin(ssd, properties)
    {
        REGISTER_UI_EXECUTOR(ShowXsltParamsDialog);
        REGISTER_UI_EXECUTOR(XsltParamAction);
        buildPluginExecutors();
        params_ = registerPropTree(SUFFIX, PARAMS_ROOT);
    }
    virtual void    buildPluginInterface();
    virtual void    postInit();
    void            buildDynamicMenu();
    PropertyNode*   params() { return params_.pointer(); }
    PropertyNode*   defaultParams() { return defaultParams_.pointer(); }
    StructEditor*   structEditor() const { return structEditor_; }

private:
    StructEditor*   structEditor_;
    PropertyNodePtr params_;
    PropertyNodePtr defaultParams_;
};

class GetXsltParams;
class SetXsltParams;

void XsltParamPlugin::postInit()
{
    // activate current parameter set
    PropertyNode* param = params_->firstChild();
    for (; param && !param->getProperty(CURRENT); param = param->nextSibling())
        ;
    if (param) {
        PropertyTreeEventData ed(param->makeDescendant(PARAMS));
        makeCommand<SetXsltParams>(&ed)->execute(structEditor(), &ed);
    }
    buildDynamicMenu();
}

void ShowXsltParamsDialog::execute()
{
    XsltParamsDialog xsltp_dialog(plugin()->structEditor(),
        plugin()->params(), plugin()->defaultParams());
    if (QDialog::Accepted == xsltp_dialog.exec())
        plugin()->buildDynamicMenu();
}

void XsltParamAction::execute()
{
    String name(uiAction()->activeSubAction()->get(Sui::NAME));
    PropertyNode* cur_set = plugin()->params()->firstChild();
    bool done = false;
    for (; cur_set; cur_set = cur_set->nextSibling()) {
        PropertyNode* pnode = cur_set->getProperty(CURRENT);
        if (!pnode)
            continue;
        if (name == cur_set->getString(NAME))
            done = true;
        else
            pnode->remove();
    }
    if (done)
        return;
    cur_set = plugin()->params()->firstChild();
    for (; cur_set; cur_set = cur_set->nextSibling()) {
        if (name == cur_set->getString(NAME)) {
            cur_set->makeDescendant(CURRENT);
            break;
        }
    }
    PropertyNodePtr new_params = plugin()->defaultParams()->copy(true);
    if (cur_set)
        new_params->merge(cur_set->getProperty(PARAMS), true);
    PropertyTreeEventData ed(new_params.pointer());
    makeCommand<SetXsltParams>(&ed)->execute(plugin()->structEditor(), &ed);
    plugin()->structEditor()->grabFocus();
}

void XsltParamPlugin::buildPluginInterface()
{
    structEditor_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    if (!structEditor_)
        return;
    // obtain existing (default) xslt parameters
    PropertyTreeEventData pd;
    makeCommand<GetXsltParams>()->execute(structEditor(), &pd);
    defaultParams_ = pd.root();
    DocumentPlugin::buildPluginInterface();
}

static void make_param_action(Sui::Action* top_action, 
                              const String& inscr)
{
    PropertyNodePtr prop(new PropertyNode(Sui::ACTION));
    prop->makeDescendant(Sui::INSCRIPTION, inscr);
    prop->makeDescendant(Sui::NAME, inscr);
    Sui::Action* sub_action = Sui::Action::make(prop.pointer());
    sub_action->setEnabled(true);
    top_action->appendChild(sub_action);
}

void XsltParamPlugin::buildDynamicMenu()
{
    Sui::Action* xsltp_action = findPluginUiAction(NOTR("selectXsltParam"));
    xsltp_action->removeAllChildren();
    make_param_action(xsltp_action, tr(DEF_COMBO));
    PropertyNode* prop = params()->firstChild();
    int current_action = 1, active_action = 0;
    for (; prop; prop = prop->nextSibling(), current_action++) {
        make_param_action(xsltp_action,
            prop->getSafeProperty(NAME)->getString());
        if (prop->getProperty(CURRENT))
            active_action = current_action;
    }
    xsltp_action->setEnabled(false); // reset state of late-attached action
    xsltp_action->setEnabled(xsltp_action->countChildren() > 1);
    xsltp_action->setInt(Sui::CURRENT_ACTION, active_action);
}

DEFINE_PLUGIN_CLASS(XsltParamPlugin)
