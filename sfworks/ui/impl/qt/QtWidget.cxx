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
#include "ui/ui_defs.h"
#include "ui/UiAction.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "ui/impl/ui_debug.h"

#include <QWidget>
#include <QGridLayout>
#include <QObject>
#include <QPointer>

using namespace Common;

namespace Sui {

/////////////////////////////////////////////////////////////////////////

class QtWidget : public Item {
public:
    QtWidget(Action* action, PropertyNode* properties);
    virtual ~QtWidget() {};

    virtual String      itemClass() const { return WIDGET; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }

    virtual QWidget*    widget(const Item* child) const;

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();
    
protected:
    QPointer<QWidget>   widget_;
};

CUSTOM_ITEM_MAKER(Widget, QtWidget)

QtWidget::QtWidget(Action* action, PropertyNode* props)
    : Item(action, props),
      widget_(new QWidget)
{
    widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget_->setObjectName(get(NAME));
    QGridLayout* layout = new QGridLayout(widget_);
    layout->setMargin(0);
}

bool QtWidget::doAttach()
{
    if (!Item::doAttach())
        return false;
    QWidget* parent_widget = Item::parent()->widget(this);
    if (parent_widget && 0 == parent_widget->layout()) {
        QBoxLayout* layout = new QHBoxLayout(parent_widget);
        layout->addWidget(widget_);
    }
    else
        if (parent_widget && parent_widget->layout()) 
            parent_widget->layout()->addWidget(widget_);
    return true;
}

bool QtWidget::doDetach()
{
    if (Item::property("is_central"))
        return false;
    return Item::doDetach();
}

QWidget* QtWidget::widget(const Item*) const
{
    return widget_;
}

} // namespace Sui
