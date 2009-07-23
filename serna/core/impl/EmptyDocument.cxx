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
#include "core/EmptyDocument.h"
#include "docview/DocumentStateEventData.h"
#include "docview/PluginLoader.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"

#include "common/PropertyTreeEventData.h"
#include "common/PathName.h"
#include "ui/UiDocument.h"
#include "ui/UiAction.h"
#include "ui/ActionSet.h"

#include <qapplication.h>
#include <qmessagebox.h>

using namespace Common;

EmptyDocument::EmptyDocument(const DocBuilder* builder, bool noAx)
    : SernaDoc(builder, 0)
{
    setBool("no-ax", noAx);
    builder->buildActions(dispatcher(), actionSet());
    pluginLoader().loadFor(NOTR("no-doc"), this);
    set_trace_tags();
    buildInterface();
    DocumentStateEventData newDocEd(DocumentStateEventData::BUILD_INTERFACE);
    stateChangeFactory().dispatchEvent(&newDocEd);
    contextAction_ = actionSet()->findAction(NOTR("docContext"));
}

class OpenDocumentWithDsi;

void EmptyDocument::openUrl(const String& file)
{
    PropertyTreeEventData iparam;
    iparam.root()->makeDescendant(DocSrcInfo::DOC_PATH, file);
    makeCommand<OpenDocumentWithDsi>(&iparam)->execute(this, &iparam);
}

bool EmptyDocument::restoreView(const String& restoreFrom)
{
    if (SernaDoc::restoreView(restoreFrom)) {
        showMessageBox(MB_INFO, tr("Restart Serna"),
                       tr("Serna UI View has been restored. Please restart "
                          "Serna for GUI changes to take effect."),
                       tr("&Close"));
        return true;
    }
    return false;
}

String EmptyDocument::getLevelFile(Level level, String* comment) const
{
    switch (level) {
        case TEMPLATE_LEVEL: {
            PathName path(config().getConfigDir());
            path.append(NOTR("no_document_view"));
            if (comment)
                *comment = tr("No Document View", "SaveView for:");
            return path.name();
        }
        case BUILTIN_LEVEL: {
            PathName path(config().getDataDir());
            path.append(NOTR("ui")).append(NOTR("EmptyDocument"));
            if (comment)
                *comment = tr("Builtin No-Document View", "SaveView for:");
            return path.name();
        }
        case ORIGINAL_TEMPLATE_LEVEL: {
            PathName path(config().getDataDir());
            path.append(NOTR("ui")).append(NOTR("no_document_view"));
            if (comment)
                *comment = tr("No Document View", "SaveView for:");
            return path.name();
        }
        default:
            break;
    }
    if (comment)
        *comment = String();
    return String();
}

class ExecuteEmptyUiCmd : public CommandEvent<SernaDoc> {
public:
    ExecuteEmptyUiCmd(ActionExecutor* ue)
        : ue_(ue) {}
    virtual bool doExecute(SernaDoc*, EventData*)
    {
        ue_->execute();
        return true;
    }
private:
    ActionExecutor* ue_;
};

CommandEventPtr EmptyDocument::makeUiEventExecutor(ActionExecutor* ue)
{
    return new ExecuteEmptyUiCmd(ue);
}

Common::CommandEventPtr EmptyDocument::makeSaveEvent(PropertyNode*) const
{
    return 0;
}

static void set_action(Sui::ActionSet* actionSet,
                       Sui::ActionPtr& action,
                       const char* actionName)
{
    action = actionSet->findAction(actionName);
    if (action.isNull()) {
        QMessageBox::warning(
            qApp->activeWindow(),
            qApp->translate("EmptyDocument", "Builtin UI Action not Defined"),
            qApp->translate("EmptyDocument", "Ui Action '%0' is not defined")
            .arg(actionName));
        action = actionSet->makeAction(NOTR("NO-BUILTIN-ACTION"));
    }
}

#include "genui/EmptyDocumentBuild.cpp"
#include "genui/StructDocumentBuild.cpp"
#include "genui/PlainDocumentBuild.cpp"


