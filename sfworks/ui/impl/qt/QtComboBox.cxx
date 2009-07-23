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
#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItems.h"
#include "ui/UiAction.h"
#include "ui/IconProvider.h"
#include "ui/impl/ui_debug.h"

#include <QComboBox>
#include <QPixmap>
#include <QVariant>
#include <QToolTip>
#include <QToolBar>
#include <QPointer>
#include <QLineEdit>

using namespace Common;

////////////////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(Sui::Action*);

namespace Sui {

static const char* IS_EDITABLE      = "is-editable";
static const char* INSERTION_POLICY = "insertion-policy";
static const char* NO_INSERTION     = "no-insertion";

class ComboBox : public QObject, 
                 public Item,
                 public ActionWatcher {
    Q_OBJECT
public:
    ComboBox(Action* action, PropertyNode* properties);
    ~ComboBox();

    virtual Common::String  itemClass()   const { return COMBO_BOX; }
    virtual Common::String  widgetClass() const { return TERMINAL_WIDGET; }

    virtual QWidget*        widget(const Item* child) const;
    Action*                 comboAction(int) const;
    int                     comboIndex(Action*) const;

public slots:
    void                    activated(int);
    void                    editingFinished();

protected:
    virtual void            childInserted(Action* action);
    virtual void            childRemoved(Action* parent, Action* action);

    void                    updateItem(PropertyNode* prop);
    virtual bool            doAttach();
    virtual bool            doDetach();
    void                    fillComboBox();
    virtual void            actionPropChanged(PropertyNode*, Action*);

protected:
    QPointer<QComboBox>     comboBox_;
};

CUSTOM_ITEM_MAKER(ComboBox, ComboBox)

////////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox(Action* action, PropertyNode* props)
    : Item(action, props)
{
    itemProps()->makeDescendant(IS_EDITABLE, "false", false);
    itemProps()->makeDescendant(INSERTION_POLICY, NO_INSERTION, false);
    if (action)
        action->addWatcher(this);
}

ComboBox::~ComboBox()
{
    delete comboBox_;
}

void ComboBox::fillComboBox()
{
    if (!action())
        return;
    Action* subact = action()->firstChild();
    for (; subact; subact = subact->nextSibling()) {
        if (subact->getBool("is-hidden"))
            continue;
        comboBox_->addItem(
            icon_provider().getPixmap(subact->get(ICON)),
            subact->getTranslated(INSCRIPTION),
            qVariantFromValue(subact));
    }
    if (action()->activeSubAction()) {
        int idx = comboIndex(action()->activeSubAction());
        if (idx >= 0)
            comboBox_->setCurrentIndex(idx);
        else
            comboBox_->setCurrentIndex(0);
    }
}

void ComboBox::childInserted(Action* action)
{
    if (!comboBox_)
        return;
    Action* next = action->nextSibling();
    for (; next && next->getBool("is-hidden"); next = next->nextSibling())
        ;
    if (next)
        comboBox_->insertItem(comboIndex(next),
            icon_provider().getPixmap(action->get(ICON)),
            action->getTranslated(INSCRIPTION), qVariantFromValue(action));
    else {
        comboBox_->addItem(icon_provider().getPixmap(action->get(ICON)),
            action->getTranslated(INSCRIPTION), qVariantFromValue(action));
    }
    if (comboBox_->currentIndex() < 0)
        comboBox_->setCurrentIndex(0);
}

void ComboBox::childRemoved(Action*, Action* action)
{
    if (!comboBox_)
        return;
    int idx = comboIndex(action);
    if (idx >= 0) 
        comboBox_->removeItem(idx);
}

Action* ComboBox::comboAction(int index) const
{
    if (index < 0)
        return 0;
    return comboBox_->itemData(index).value<Action*>();
}

int ComboBox::comboIndex(Action* act) const
{
    for (int i = 0; i < comboBox_->count(); ++i) {
        Action* action = comboBox_->itemData(i).value<Action*>();
        if (action == act)
            return i;
    }
    return -1;
}

void ComboBox::actionPropChanged(PropertyNode* prop,
                                 Action* subAction)
{
    DBG(UI.DYN) << "ComboBox: actionPropChanged, assub=" 
        << (action()->activeSubAction() ? 
            action()->activeSubAction()->get(NAME) : String("NONE"))
        << ", subact=" << (subAction ? subAction->get(NAME) : String("NONE"))
        << ", lock=" << isLockedNotify() 
        << ", prop=" << prop->name() << std::endl;
    if (isLockedNotify() || !comboBox_)
        return;
    if (0 == subAction) {
        Item::actionPropChanged(prop, 0);
        if (CURRENT_ACTION == prop->name()) 
            comboBox_->setCurrentIndex(comboIndex(action()->activeSubAction()));
        if (TOOLTIP == prop->name())
            comboBox_->setToolTip(action()->getTranslated(TOOLTIP));
        if (INSCRIPTION == prop->name())
            comboBox_->setCurrentText(action()->getTranslated(INSCRIPTION));
        return;
    }
    int idx = comboIndex(subAction);
    if (idx < 0)
        return;
    if (ICON == prop->name())
        comboBox_->setItemIcon(idx, 
            icon_provider().getPixmap(subAction->get(ICON)));
    if (INSCRIPTION == prop->name())
        comboBox_->setItemText(idx, subAction->getTranslated(INSCRIPTION));
    if (subAction->parent()->getBool(IS_TOGGLEABLE) &&
        subAction->getBool(IS_TOGGLED))
            comboBox_->setCurrentIndex(idx);
}

void ComboBox::editingFinished()
{
    QString text(comboBox_->lineEdit()->text());
    if (action()->get(INSCRIPTION) != text) {
        action()->set(INSCRIPTION, text);
        Action* custom_act = action()->getSubAction(NOTR("custom"));
        if (custom_act)
            dispatch(custom_act);
        else
            dispatch();
    }
}

void ComboBox::activated(int idx)
{
    Action* current = comboAction(idx);
    if (current) {
        action()->set(INSCRIPTION, current->get(INSCRIPTION));
        dispatch(current);
    }
}

bool ComboBox::doAttach()
{
    QToolBar* toolbar = dynamic_cast<QToolBar*>(parentWidget());
    comboBox_ = new QComboBox(toolbar ? 0 : parentWidget());
    comboBox_->setObjectName(get(NAME));
    comboBox_->setWindowTitle(getTranslated(INSCRIPTION));
    comboBox_->setDuplicatesEnabled(false);
    comboBox_->setToolTip(getTranslated(Sui::TOOLTIP));
    comboBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    fillComboBox();
    connect(comboBox_, SIGNAL(activated(int)), SLOT(activated(int)));
    if (toolbar) 
        toolbar->addWidget(comboBox_);
    comboBox_->setEnabled(getBool(IS_ENABLED));
    comboBox_->setEditable(getBool(IS_EDITABLE));
    comboBox_->setInsertPolicy(QComboBox::NoInsert);
    if (comboBox_->isEditable()) {
        comboBox_->setAutoCompletion(false);
        connect(comboBox_->lineEdit(), SIGNAL(editingFinished()),
            SLOT(editingFinished()));
    }
    return true;
}

bool ComboBox::doDetach()
{
    delete comboBox_;
    return true;
}

QWidget* ComboBox::widget(const Item*) const
{
    return comboBox_;
}

} // namespace Sui

#include "moc/QtComboBox.moc"
