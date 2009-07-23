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
#include "core/Preferences.h"

#include "docview/SernaDoc.h"
#include "docview/PluginLoader.h"

#include "utils/Config.h"
#include "utils/Properties.h"
#include "utils/HelpAssistant.h"
#include "utils/DocSrcInfo.h"

#include "core/PreferencesDialogBase.hpp"
#include "docutils/doctags.h"

#include <QShortcut>

using namespace Common;

class PreferencesDialog : public QDialog,
                          public Common::PropertyNodeWatcher,
                          public Ui::PreferencesDialogBase {
    Q_OBJECT
public:
    PreferencesDialog(QWidget* parent, Common::PropertyNode*);

protected:
    virtual void        propertyChanged(Common::PropertyNode* property);
    virtual bool        event(QEvent* e);

protected slots:

    virtual void        on_buttonHelp__clicked();
    virtual void        on_buttonApply__clicked();

private:
    Common::PropertyNode*       property_;
};

PreferencesDialog::PreferencesDialog(QWidget* parent, PropertyNode* prop)
    : QDialog(parent, NOTR("PreferencesDialog")),
      property_(prop)
{
    setupUi(this);

    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
            SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));

    property_->root()->addWatcher(this);

    while (tabWidget_->count())
        tabWidget_->removeTab(tabWidget_->currentIndex());

    PreferencesTabItem* tab_item = PreferencesTabList::instance().firstChild();
    for (; tab_item; tab_item = tab_item->nextSibling()) {
        QWidget* tab = tab_item->makeTab(property_);
        const PropertyNode* inscr_prop = tab_item->root()->
            getProperty(PreferencesTabItem::TAB_INSCRIPTION_PROP);
        if (tab)
            tabWidget_->addTab(tab, inscr_prop ? 
                QString(inscr_prop->getString()) : tab->windowTitle());
    }
    buttonApply_->setEnabled(false);
}

void PreferencesDialog::propertyChanged(PropertyNode*)
{
    buttonApply_->setEnabled(true);
}

void PreferencesDialog::on_buttonHelp__clicked()
{
    helpAssistant().show(DOCTAG(UG_PREFERENCES_D));
}

void PreferencesDialog::on_buttonApply__clicked() 
{
    config().root()->makeDescendant(
        FileHandler::HANDLER_LIST)->removeAllChildren();
    config().root()->makeDescendant(
        SernaFileType::TYPE_LIST)->removeAllChildren();
    config().root()->merge(property_, true);
    config().update();
    config().save();
    buttonApply_->setEnabled(false);
}

bool PreferencesDialog::event(QEvent* e)
{
    if (QEvent::LanguageChange == e->type()) {
        for (int i = 0; i < tabWidget_->count(); ++i) {
            if (QWidget* qw = tabWidget_->page(i))
                tabWidget_->setTabLabel(qw, qw->caption());
        }
    }
    return QDialog::event(e);
}

#include "moc/PreferencesDialog.moc"

////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(EditPreferences, SernaDoc)

bool EditPreferences::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTree preferences;
    preferences.root()->merge(config().root());
    PropertyNode* to_remove =
        preferences.root()->getProperty(DocSrcInfo::RECENT_LIST);
    if (to_remove)
        to_remove->remove();
    PreferencesDialog dialog(sernaDoc->widget(0), preferences.root());
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return true;
}

/////////////////////////////////////////////////////////////////////////

using namespace Common;
    
const char PreferencesTabItem::TAB_INSCRIPTION_PROP[]  = NOTR("name");
const char PreferencesTabItem::TAB_INDEX_PROP[] = NOTR("index");

#define GET_PROP(ptr, x) \
    ptr->root()->getSafeProperty(PreferencesTabItem::x)->getString()

PreferencesTabItem::PreferencesTabItem(const Common::String& index)
{
    root()->makeDescendant(TAB_INDEX_PROP, index);
}

PreferencesTabItem::~PreferencesTabItem()
{
}

void PreferencesTabList::insertTab(PreferencesTabItem* item)
{
    PreferencesTabItem* i = firstChild();
    for (; i; i = i->nextSibling())
        if (GET_PROP(item, TAB_INDEX_PROP) <= GET_PROP(i, TAB_INDEX_PROP))
            break;
    if (i)
        i->insertBefore(item);
    else
        appendChild(item);
}

PreferencesTabList& PreferencesTabList::instance()
{
    return SingletonHolder<PreferencesTabList>::instance();
}

PreferencesTabList::~PreferencesTabList()
{
}
