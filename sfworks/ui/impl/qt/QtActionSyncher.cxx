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
#include "ui/UiItemWatcher.h"
#include "ui/UiAction.h"
#include "ui/UiDocument.h"
#include "ui/ActionSet.h"
#include "ui/IconProvider.h"
#include "ui/impl/ui_debug.h"
#include "ui/impl/ShortcutAction.h"
#include "ui/impl/qt/QtActionSyncher.h"

#include <QList>
#include <QAction>
#include <QPointer>
#include <QMetaEnum>
#include <QShortcut>

using namespace Common;

namespace Sui {

// important: because qAction() creates ActionSyncher lazily, we assume
// that all properties are in place before qAction() is called for the
// first time. So empty property tree may be passed to PropertyTreeHolder
// in constructor. (apg)

class QtActionSyncher : public QObject,
                        public Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    QtActionSyncher(Item& item, QAction*);
    ~QtActionSyncher();

    virtual void    propertyChanged(PropertyNode* property);
    QAction*        qAction() { return qAction_; }

public slots:
    void            toggled(bool v);
    void            triggered(bool);
    void            scheduledDispatch();

private:
    QtActionSyncher(const QtActionSyncher&);
    QtActionSyncher& operator=(const QtActionSyncher&);

    QPointer<QAction>   qAction_;
    Item&               pth_;
};

extern UI_EXPIMP bool is_mac_app_menu;

static void set_menu_role(QAction* action, const String& value)
{
    if (is_mac_app_menu && !value.isEmpty()) {
        const QMetaObject* mo = action->metaObject();
        action->setMenuRole((QAction::MenuRole)(mo->enumerator
            (mo->indexOfEnumerator(NOTR("MenuRole")))).
                keyToValue(value.latin1().c_str()));
    } else
        action->setMenuRole(QAction::NoRole);
}

QtActionItem::QtActionItem(Action* action, PropertyNode* props)
    : Item(action, props)
{
}

void QtActionItem::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    if (actionSyncher_)
        actionSyncher_->propertyChanged(prop);
    Item::propertyChanged(prop);
}

QAction* QtActionItem::qAction() 
{
    if (actionSyncher_.isNull())
        actionSyncher_ = new QtActionSyncher(*this, 0);
    return actionSyncher_->qAction();
}

void QtActionItem::setQAction(QAction* a)
{
    actionSyncher_ = new QtActionSyncher(*this, a);
}

bool QtActionItem::insertQAction(QWidget* parent_widget)
{
    if (0 == parent_widget)
        parent_widget = parentWidget();
    if (0 == parent_widget)
        return false;
    Item* item = Item::nextSibling();
    QtActionItem* qitem = 0;
    for (; item; item = item->Item::nextSibling()) {
        qitem = dynamic_cast<QtActionItem*>(item);
        if (qitem)
            break;
    }
    if (0 == qitem)
        parent_widget->addAction(qAction());
    else
        parent_widget->insertAction(qitem->qAction(), qAction());
    return true;
}

bool QtActionItem::removeQAction()
{
    if (parentWidget() && qAction()) {
        parentWidget()->removeAction(qAction());
        return true;
    }
    return false;
}

QtActionItem::~QtActionItem()
{
}

/////////////////////////////////////////////////////////////////////

QtActionSyncher::QtActionSyncher(Item& pth, QAction* a)
    : pth_(pth) 
{
    if (pth.action() && !pth.getBool(NOTR("own-qaction")) &&
        (pth_.parent() && pth_.parent()->itemClass() != CONTEXT_MENU)) {
        qAction_ = pth.action()->qAction();
        if (qAction_)
            return;
        qAction_ = a ? a : new QAction(this);
        pth.action()->setQAction(qAction_);
    } else
        qAction_ = a ? a : new QAction(this);
    qAction_->setText(pth_.getTranslated(INSCRIPTION));
    if (!pth_.get(ICON).isEmpty())
        qAction_->setIcon(icon_provider().getIconSet(pth_.get(ICON)));
    qAction_->setToolTip(pth_.getTranslated(TOOLTIP));
    if (!pth_.getTranslated(ACCEL).isEmpty()) 
        qAction_->setShortcut(QKeySequence(pth_.getTranslated(ACCEL)));
    qAction_->setCheckable(pth_.getBool(IS_TOGGLEABLE));
    if (qAction_->isCheckable()) 
        qAction_->setChecked(pth_.getBool(IS_TOGGLED));
    qAction_->setEnabled(pth_.getBool(IS_ENABLED));
    qAction_->setVisible(pth_.getBool(IS_VISIBLE));
    set_menu_role(qAction_, pth_.get(MENU_ROLE));
    if (pth_.getBool(IS_SEPARATOR))
        qAction_->setSeparator(true);
    connect(qAction(), SIGNAL(triggered(bool)),
        this, SLOT(triggered(bool)), Qt::QueuedConnection);
    connect(qAction(), SIGNAL(toggled(bool)),
        this, SLOT(toggled(bool)),pth_.getBool(IS_TOGGLEABLE) ?
            Qt::DirectConnection : Qt::QueuedConnection);
}

void QtActionSyncher::toggled(bool v)
{
    Action* action = pth_.action();
    if (!action || !qAction_)
        return;
    if (action->getBool(IS_TOGGLED) == v)
        return;
    // for exclusive multi-actions, prevent de-toggling
    if (action->parent() && !v && action->parent()->getBool(IS_TOGGLEABLE) &&
        action->parent()->activeSubAction() == action) {
            qAction()->blockSignals(true);
            qAction()->setChecked(true);
            qAction()->blockSignals(false);
    } else {
        qAction()->blockSignals(true);
        action->setBool(IS_TOGGLED, v);
        qAction()->blockSignals(false);
        pth_.dispatch();
    }
}

void QtActionSyncher::triggered(bool)
{
    Action* action = pth_.action();             
    if (!action)
        return;
    if (!action->getBool(IS_TOGGLEABLE)) {
        bool r = QMetaObject::invokeMethod(this, NOTR("scheduledDispatch"),
                                           Qt::QueuedConnection);
        DBG(UI.CMD) << "ActionImpl '" << action->get(NAME) 
                    << "', this = " << this
                    << ", dispatch scheduled, invokeMethod returned " << r
                    << std::endl;
        (void) r;
    }
}

void QtActionSyncher::scheduledDispatch()
{
    DBG(UI.CMD) << "ActionImpl '" << pth_.get(NAME) << "', this = " << this
                << " about to perform scheduled dispatch" << std::endl;
    pth_.dispatch();
}

QtActionSyncher::~QtActionSyncher()
{
    if (!qAction_)
        return;
    QList<QWidget*> wlist = qAction_->associatedWidgets();
    QList<QWidget*>::iterator it = wlist.begin();
    for (; it != wlist.end(); ++it) 
        (*it)->removeAction(qAction_);
}

void QtActionSyncher::propertyChanged(PropertyNode* property)
{
    if (!property->parent() || !qAction_)
        return;
    DBG(UI.PROP) << "PropTreeHolder(qact="
        << &qAction_ << "): prop <" << property->name()
        << "> parent-name:<" 
        << property->parent()->getSafeProperty(NAME)->getString() 
        << "> changed, new value=<" << property->getString() << ">\n";
    if (INSCRIPTION == property->name()) 
        return qAction_->setText(pth_.getTranslated(INSCRIPTION));
    if (ICON == property->name()) 
        return (void) qAction_->setIcon(icon_provider().
            getIconSet(pth_.get(ICON)));
    if (TOOLTIP == property->name())
        return qAction_->setToolTip(pth_.getTranslated(TOOLTIP));
    if (ACCEL == property->name())
        return qAction_->setShortcut(QKeySequence(pth_.getTranslated(ACCEL)));
    if (IS_TOGGLEABLE == property->name())
        return qAction_->setCheckable(pth_.getBool(IS_TOGGLEABLE));
    if (IS_TOGGLED == property->name() && qAction_->isCheckable() &&
        qAction_->isChecked() != pth_.getBool(IS_TOGGLED))
            return qAction_->setChecked(pth_.getBool(IS_TOGGLED));
    if (IS_ENABLED == property->name())
        return qAction_->setEnabled(pth_.getBool(IS_ENABLED));
    if (IS_VISIBLE == property->name())
        return qAction_->setVisible(pth_.getBool(IS_VISIBLE));
    if (IS_SEPARATOR == property->name())
        return qAction_->setSeparator(pth_.getBool(IS_SEPARATOR));
    if (MENU_ROLE == property->name())
        return set_menu_role(qAction_, pth_.get(MENU_ROLE));
}

} // namespace Sui

#include "moc/QtActionSyncher.moc"

