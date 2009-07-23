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
#include "ui/UiAction.h"
#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItems.h"
#include "ui/impl/ui_debug.h"

#include <QDialog>
#include <QEvent>
#include <QPointer>

using namespace Common;

static const char* IS_MODAL = "is-modal";
static const char* CAPTION  = "caption";
static const char* WIDTH    = "width";
static const char* HEIGHT   = "height";

////////////////////////////////////////////////////////////////////////////

namespace Sui {

class QtDialog : public QObject, public Item {
public:
    QtDialog(Action* action, PropertyNode* properties);
    virtual ~QtDialog();

    virtual String      itemClass() const { return DIALOG; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }

    virtual void        setItemVisible(bool isVisible);

    virtual QWidget*    widget(const Item* child) const;

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();
    virtual void        inserted();
    void                syncVisibleProperty(bool isVisible);
    virtual bool        eventFilter(QObject* watched, QEvent* e);

private:
    QPointer<QDialog>   dialog_;
    bool                visibilityLocked_;
};

CUSTOM_ITEM_MAKER(Dialog, QtDialog)

QtDialog::QtDialog(Action*, PropertyNode* props)
    : Sui::Item(0, props),
      dialog_(0),
      visibilityLocked_(false)
{
    itemProps()->makeDescendant(IS_MODAL, "false", false);
    itemProps()->makeDescendant(CAPTION, "Dialog", false);
}

QtDialog::~QtDialog()
{
    DDBG << "~QtDialog: " << this << std::endl;
}

void QtDialog::inserted()
{
    attach(true);
}

bool QtDialog::doAttach()
{
    if (dialog_.isNull()) {
        QWidget* parent_widget = Sui::Item::parent()->widget(this);
        DDBG << "QtDialog attach to: " << parent_widget << std::endl;
        DBG_IF(UI.TEST) dump(false);

        dialog_ = new QDialog(parent_widget);
        dialog_->setObjectName(get(NAME));
        QSize size_hint(getInt(WIDTH), getInt(HEIGHT));
        dialog_->resize(size_hint);
        dialog_->setModal(getBool(IS_MODAL));
        dialog_->setCaption(getTranslated(CAPTION));
        dialog_->installEventFilter(this);
    }
    return true;
}

bool QtDialog::doDetach()
{
    if (dialog_) {
        DBG(UI.TEST) << "QtDialog: detach: " << std::endl;
        DBG_IF(UI.TEST) dump(false);
        visibilityLocked_ = true;
        for (Item* i = firstChild(); i; i = i->nextSibling())
            i->detach(false);
        if (dialog_)
            dialog_->deleteLater();
        visibilityLocked_ = false;
    }
    return true;
}

void QtDialog::syncVisibleProperty(bool isVisible)
{
    if (visibilityLocked_)
        return;
    visibilityLocked_ = true;
    Item::property(IS_VISIBLE)->setBool((dialog_) ? isVisible : false);
    visibilityLocked_ = false;
}

void QtDialog::setItemVisible(bool isVisible)
{
    if (visibilityLocked_)
        return;
    visibilityLocked_ = true;
    if (dialog_) {
        if (isVisible) {
            if (dialog_->isModal())
                dialog_->exec();
            else
                dialog_->show();
        }
        else
            dialog_->hide();
    }
    visibilityLocked_ = false;
}

QWidget* QtDialog::widget(const Item*) const
{
    return dialog_;
}

bool QtDialog::eventFilter(QObject* watched, QEvent* e)
{
    if (watched == dialog_) {   
        if (e->type() == QEvent::Hide || e->type() == QEvent::Close) {
            if (dialog_->isModal()) {
                remove();
                return true;
            } 
            else 
                syncVisibleProperty(false);
        }
    }
    return false;
}

} // namespace Sui
