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
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/UiAction.h"
#include "sapi/common/OwnerPtr.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/common/SimpleWatcher.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/app/StructEditor.h"
#include "sapi/app/UiItem.h"
#include "sapi/app/Config.h"

#include "sapi/grove/Grove.h"
#include "sapi/grove/GroveIdManager.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/grove/GroveNodes.h"
#include <iostream>

#define NOTR(x) x

class LinkVoyagerPlugin;
using namespace SernaApi;

// Following statement defines executor 'GoByLink' 
// which will be executed when corresponding menu item is selected
SAPI_PLUGIN_EXECUTOR_IMPL(GoByLink, LinkVoyagerPlugin)

// A plugin instance class
class LinkVoyagerPlugin : public SernaApi::DocumentPlugin {
public:
    // Instance of this class reacts to double-click events
    class DblClickWatcher : public SimpleWatcher {
    public:
        virtual bool notifyChanged();
        
        DblClickWatcher(SernaApi::DocumentPlugin* plugin)
            : plugin_(plugin) {}
    private:
        SernaApi::DocumentPlugin* plugin_;
    };

    DblClickWatcher* watcher() const { return watcher_.pointer(); }

    LinkVoyagerPlugin(SernaApiBase* ssd, SernaApiBase* properties, char**);

private:
   OwnerPtr<DblClickWatcher> watcher_;
};

LinkVoyagerPlugin::LinkVoyagerPlugin(SernaApiBase* ssd, 
                           SernaApiBase* properties, char**)
    : DocumentPlugin(ssd, properties)
{
    StructEditor se = sernaDoc().structEditor();
    
    if (se.isNull()) // not in WYSYWIG mode
        return; 
    
    // Register GoByLink executor
    SAPI_REGISTER_UI_EXECUTOR(GoByLink);
    
    // Build registered executors 
    buildPluginExecutors();

    // Create and set double click tracker, 
    // which will be notified on Serna's double click events
    watcher_ = new DblClickWatcher(this); 
    se.setDoubleClickWatcher(watcher());
}

SAPI_DEFINE_PLUGIN_CLASS(LinkVoyagerPlugin)

///////////////////////////////////////////////////////////////////////

// some literal constants 
// START_IGNORE_LITERALS
static const char* HANDLER_LIST       = "file-handler-list";
static const char* HANDLER_LIST_TEMP  = "#file-handler-list";
static const char* FILE_HANDLER       = "file-handler";
static const char* APP_EXT            = "extension";
static const char* APP_PATH           = "executable";

// this function calls external viewer with parameter 'file'
static void call_external_viewer(SString file, DocumentPlugin* plugin)
{
    PropertyNode browser_args("");
    browser_args.makeDescendant("url", file);
    plugin->executeCommandEvent("LaunchBrowser", browser_args);
}
// STOP_IGNORE_LITERALS

// This function is called when double-click event occurs. When we return
// true, this means that we pass double-click event further; when we return
// false, this means interception of the event.
bool LinkVoyagerPlugin::DblClickWatcher::notifyChanged() 
{
    // Get current position in the source XML tree (grove)
    GrovePos pos = plugin_->sernaDoc().structEditor().getSrcPos();
    
    if (pos.isNull())
        return true;        // no valid position - do nothing
        
    // Check current position. If it is a text node, take its parent.
    GroveNode node = pos.node();
    if (GrovePos::TEXT_POS == pos.type())
        node = node.parent();
    
    // Checks that node is ulink or link; if not, return.
    if (node.nodeName() != NOTR("ulink") && node.nodeName() != NOTR("link"))
        return true;
    
    // does it have linkend or url?
    GroveAttr attr = node.asGroveElement().attrs().
        getAttribute(node.nodeName() == NOTR("link") ? NOTR("linkend") : NOTR("url"));
    if (attr.isNull() || attr.value().isEmpty())
        return true;
        
    SString target = attr.value();
    // For link element
    if (node.nodeName() == NOTR("link")) {
        // This is an ID reference. Fetch target position from ID table
        GroveElement elem = node.grove().idManager().lookupElement(target);
        if (elem.isNull())
            return true;
            
        // Scroll/set cursor to the target position
        StructEditor se = plugin_->sernaDoc().structEditor();
        se.setCursorBySrcPos(GrovePos(elem), se.getFoPos().node());
        return false;
    }
    // For ulink element: if it is local .xml file, open it in Serna,
    // otherwise launch viewer application.
    if (NOTR(".xml") == target.right(4) && target.find(NOTR("http:")) != 0) {
        PropertyNode ptn(NOTR("doc-src-info"));
        ptn.makeDescendant(NOTR("doc-src")).setString(target);
        plugin_->executeCommandEvent(NOTR("OpenDocumentWithDsi"), ptn);
    }
    else 
        call_external_viewer(target, plugin_);
    return false;
}

void GoByLink::execute()
{
    // When command executed by selecting corresponding menu item,
    // emulate doubleclick.
    plugin()->watcher()->notifyChanged();
}

