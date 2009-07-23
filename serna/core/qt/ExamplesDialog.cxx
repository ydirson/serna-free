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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "core/core_defs.h"
#include "common/String.h"
#include "common/PropertyTree.h"
#include "common/PathName.h"
#include "docview/SernaDoc.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "utils/Properties.h"
#include "common/PropertyTreeEventData.h"
#include "ui/IconProvider.h"
#include "ui/UiPropertySyncher.h"

#include "ExamplesDialogBase.hpp"

#include <QPixmap>

using namespace Common;

class ExamplesDialogImpl : public QDialog,
                           public Ui::ExamplesDialogBase {
public:
    ExamplesDialogImpl(QWidget* parent);
    bool    getParam(Common::String& path);

private:
    Sui::PropertySyncherPtr dontShowSyncher_;
};

ExamplesDialogImpl::ExamplesDialogImpl(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);

    QPixmap pic_read(Sui::icon_provider().getPixmap(NOTR("example_read")));
    QPixmap pic_unread(Sui::icon_provider().getPixmap(NOTR("document")));

    PropertyNode* pinfo = config().root()->makeDescendant(Examples::EXAMPLES);
    dontShowSyncher_ =
        new Sui::ButtonSyncher(pinfo->
        makeDescendant(Examples::DONT_SHOW_ON_START), showCheckBox_);
    examplesListView_->setColumnCount(1);
    examplesListView_->setItemHidden(examplesListView_->headerItem(), 1);
    for (PropertyNode* pn = pinfo->firstChild(); pn; pn = pn->nextSibling()) {
        if (pn->name() != Examples::EXAMPLE)
            continue;
        QTreeWidgetItem* inscr_item = new QTreeWidgetItem;
        inscr_item->setText(0,
            pn->getSafeProperty(Examples::INSCRIPTION)->getString());
        inscr_item->setToolTip(0,
            pn->getSafeProperty(Examples::EXAMPLE_FILE)->getString());
        inscr_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        PropertyNode* read = pn->getProperty(Examples::READ);
        if (read && read->getBool())
            inscr_item->setIcon(0, pic_read);
        else
            inscr_item->setIcon(0, pic_unread);
        examplesListView_->addTopLevelItem(inscr_item);
    }
    examplesListView_->setCurrentItem(examplesListView_->topLevelItem(0));
}
                 
bool ExamplesDialogImpl::getParam(String& path)
{
    if (QDialog::Accepted != exec()) 
        return false;
    PropertyNode* pn = config().root()->
        makeDescendant(Examples::EXAMPLES)->firstChild();
    const QTreeWidgetItem* cur_item = examplesListView_->currentItem();
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() != Examples::EXAMPLE)
            continue;
        if (cur_item->text(0) == pn->getSafeProperty(
            Examples::INSCRIPTION)->getString()) {
                path = pn->getSafeProperty(Examples::EXAMPLE_FILE)->getString();
                return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ExamplesDialog, SernaDoc)

bool ExamplesDialog::doExecute(SernaDoc* se, EventData* result)
{
    String path;
    if (ExamplesDialogImpl(se->widget(0)).getParam(path)) {
        PropertyNode* result_root =
            static_cast<PropertyTreeEventData*>(result)->root();
        result_root->makeDescendant(Examples::EXAMPLE_FILE)->setString(path);
        return true;
    }
    return false;
}
