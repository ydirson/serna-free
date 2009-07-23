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
#include "common/String.h"
#include "common/PropertyTree.h"
#include "common/CommandEvent.h"

#include "docview/SernaDoc.h"
#include "core/core_defs.h"
#include "core/debug_core.h"
#include "utils/HelpAssistant.h"
#include "utils/Config.h"
#include "ui/UiPropertySyncher.h"

#include "docutils/doctags.h"
#include "core/DebugDialogBase.hpp"

#include <QDialog>
#include <QShortcut>

using namespace Common;

class DebugDialog : public QDialog, 
                    public Ui::DebugDialogBase,
                    public Sui::PropertySyncherHolder {
    Q_OBJECT
public:
    DebugDialog(QWidget* parent, PropertyNode* params);

protected slots:
    virtual void        on_buttonHelp__clicked();

private:
    PropertyNodePtr    params_;
};

DebugDialog::DebugDialog(QWidget* parent, PropertyNode* params)
    : QDialog(parent),
      params_(params)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
            SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));
    addSyncher(new Sui::LineEditSyncher(
        params_->makeDescendant(NOTR("trace_tags_1")), mode1LineEdit_));
    addSyncher(new Sui::LineEditSyncher(
        params_->makeDescendant(NOTR("trace_tags_2")), mode2LineEdit_));
    Sui::SelectorSyncher* sel = new Sui::SelectorSyncher(
        params_->makeDescendant(NOTR("use_trace_tags")));
    sel->addSelector("0", quietRadioButton_); 
    sel->addSelector("1", mode1RadioButton_);    
    sel->addSelector("2", mode2RadioButton_);    
    addSyncher(sel);
}

void DebugDialog::on_buttonHelp__clicked()
{
    helpAssistant().show(DOCTAG(UG_PREFERENCES_D));
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(EditTraceTags, SernaDoc)
SIMPLE_COMMAND_EVENT_IMPL(SetTraceTags,  SernaDoc)

bool EditTraceTags::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTree debug_param;
    debug_param.root()->merge(config().root()->makeDescendant("debug"));
    DBG(CORE.TEST) << "Debug: input params:\n";
    DBG_IF(CORE.TEST) debug_param.root()->dump();
    DebugDialog debug_dialog(sernaDoc->widget(0), debug_param.root());
    if (debug_dialog.exec() != QDialog::Accepted)
        return false;
    config().root()->makeDescendant("debug")->merge(debug_param.root(), true);
    return makeCommand<SetTraceTags>(0)->execute(sernaDoc);
}

bool SetTraceTags::doExecute(SernaDoc*, EventData*)
{
    DBG_SET_TAGS("*", false);
    set_trace_tags();
    return true;
}

#include "moc/DebugDialog.moc"
