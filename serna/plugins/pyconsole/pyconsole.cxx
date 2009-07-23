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
#include "sapi/common/PropertyNode.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/app/Config.h"

#include "QtPyDialog.h"

class PythonConsole;
class PythonConsoleWatcher;

SAPI_PLUGIN_EXECUTOR_IMPL(PythonConsoleDialog, PythonConsole)

static PythonConsoleWatcher* py_watcher = 0;

///////////////////////////////////////////////////////////////

class PythonConsoleWatcher : public SernaApi::PropertyNodeWatcher {
public:
    PythonConsoleWatcher(const SernaApi::SernaDoc& sd) : sernaDoc_(sd)
    {
        pyMessages_ = SernaApi::SernaConfig::root().
            makeDescendant("#python-messages");
        pyMessages_.addWatcher(this);
    }
    virtual void propertyChanged(const SernaApi::PropertyNode&)
    {
        if (QtPyDialog::py_dialog_)
            QtPyDialog::py_dialog_->propertyChanged(this);
        else if (QtPyDialog::pyConsoleAutoShow_)
            make_dialog();
    }
    void make_dialog()
    {
        QtPyDialog::make(pyMessages_, sernaDoc_);
    }

private:
    SernaApi::PropertyNode  pyMessages_;
    SernaApi::SernaDoc      sernaDoc_;
};

class PythonConsole : public SernaApi::DocumentPlugin {
public:
    PythonConsole(SernaApiBase* ssd, SernaApiBase* properties, char**)
        : DocumentPlugin(ssd, properties)
    {
        SAPI_REGISTER_UI_EXECUTOR(PythonConsoleDialog);
        buildPluginExecutors();
        if (0 == py_watcher)
            py_watcher = new PythonConsoleWatcher(sernaDoc());
    }
};

void PythonConsoleDialog::execute()
{
    if (QtPyDialog::py_dialog_) {
        QtPyDialog::py_dialog_->show();
        return;
    }
    py_watcher->make_dialog();
}

SAPI_DEFINE_PLUGIN_CLASS(PythonConsole)
