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
#include "debug_cc.h"
#include "customcontent.h"
#include "CustomContentDialog.h"
#include "CustomContentListDialog.h"

#include "structeditor/StructEditor.h"
#include "docview/SernaDoc.h"
#include "utils/HelpAssistant.h"
#include "utils/tr.h"
#include "docutils/doctags.h"

#include "common/PathName.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventData.h"
#include "proputils/PropertyTreeSaver.h"
#include "ui/IconProvider.h"
#include "ui/ActionSet.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QShortcut>

using namespace Common;
using namespace ContentProps;

////////////////////////////////////////////////////////////////////////////

class ContentListItem : public QTreeWidgetItem {
public:
    ContentListItem(QTreeWidget* parent, Sui::Action* action)
        : QTreeWidgetItem(parent),
          action_(action)
        {
            update();
        }
    Sui::Action*     uiAction() { return action_; }
    void            update();
private:
    Sui::Action*     action_;
};

void ContentListItem::update()
{
    setIcon(0, Sui::icon_provider().getPixmap(action_->get(ICON)));
    setText(1, action_->get(NAME));
    setText(2, QKeySequence(action_->getTranslated(HOT_KEY)));
    setText(3, action_->getTranslated(INSCRIPTION));
}

////////////////////////////////////////////////////////////////////////////

CustomContentListDialog::CustomContentListDialog(StructEditor* se,
                                                 CustomContentPlugin* plugin)
    : QDialog(se->sernaDoc()->widget(0)),
      se_(se),
      plugin_(plugin)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
        SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));
    QHeaderView& header = *contentListView_->header();
    header.setStretchLastSection(true);
    header.setResizeMode(0, QHeaderView::ResizeToContents);
    header.setResizeMode(1, QHeaderView::ResizeToContents);
    header.setResizeMode(2, QHeaderView::ResizeToContents);
    header.setResizeMode(3, QHeaderView::Stretch);
    contentListView_->setAllColumnsShowFocus(true);
    for (ActionIterator i = plugin_->actionList().begin();
         i != plugin_->actionList().end(); i++)
        new ContentListItem(contentListView_, (*i).pointer());
    if (contentListView_->topLevelItemCount()) {
        contentListView_->setCurrentItem(contentListView_->topLevelItem(0));
        contentListView_->setItemSelected
            (contentListView_->topLevelItem(0), true);
    }
}

void CustomContentListDialog::on_helpButton__clicked()
{
    helpAssistant().show(DOCTAG(EDIT_CUSTOM_CONTENT_D));
}

void CustomContentListDialog::on_contentListView__itemSelectionChanged()
{
    const bool is_on = !!contentListView_->selectedItems().size();
    editButton_->setEnabled(is_on);
    deleteButton_->setEnabled(is_on);
}

void CustomContentListDialog::editContent()
{
    ContentListItem* item =
        dynamic_cast<ContentListItem*>(contentListView_->currentItem());
    if (!item || !item->uiAction())
        return;
    Sui::Action* action = item->uiAction();
    String action_name = action->get(Sui::NAME);
    const bool has_button = action->getBool(MAKE_TOOL_BUTTON);
    if (QDialog::Accepted ==
        CustomContentDialog(plugin_, action->properties(), this, se_).exec()) {
        if (action_name != action->get(Sui::NAME))
            se_->sernaDoc()->actionSet()->remapAction(action_name, action);
        item->update();
        if (has_button != action->getBool(MAKE_TOOL_BUTTON)) 
            plugin_->makeContentItems(action);
    }
}

class NewCustomContent;

void CustomContentListDialog::on_newButton__clicked()
{
    String name = plugin_->getValidName(NOTR("customContent"));
    if (name.isEmpty())
        name = NOTR("CustomContent");
    if (!plugin_->isValidName(name)) 
        name = plugin_->getValidName(name);
    String inscription = tr("Insert %0").arg(name);

    PropertyNodePtr content = new PropertyNode(CONTENT_ITEM);
    content->makeDescendant(NAME)->setString(name);
    content->makeDescendant(INSCRIPTION)->setString(inscription);
    content->makeDescendant(Sui::TOOLTIP)->setString(inscription);
    content->makeDescendant(ICON)->setString(NOTR("custom_content_1"));
    content->makeDescendant(MAKE_TOOL_BUTTON)->setBool(true);
    content->makeDescendant(
        CONTEXT_SPECS + String("/") + CONTENT + String("/") + TEXT);
    if (QDialog::Accepted == CustomContentDialog(plugin_, content.pointer(),
                                                 this, se_).exec()) {
        Sui::Action* action = plugin_->addContent(content.pointer());
        if (action)
            new ContentListItem(contentListView_, action);
    }
}

void CustomContentListDialog::on_deleteButton__clicked()
{
    ContentListItem* item =
        dynamic_cast<ContentListItem*>(contentListView_->currentItem());
    if (!item || !item->uiAction())
        return;
    if (0 == se_->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING, tr("Delete Custom Content"),
            tr("Are you sure you want to delete custom content?"),
            tr("&Delete"), tr("&Cancel"))) {
        Sui::Action* action = item->uiAction();
        delete item;
        plugin_->deleteContent(action);
    }
}
