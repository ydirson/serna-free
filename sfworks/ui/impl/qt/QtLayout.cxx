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
#include "ui/impl/qt/QtLayout.h"
#include "ui/impl/ui_debug.h"

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>

using namespace Common;

static const char* ORIENTATION  = "orientation";
static const char* VERTICAL     = "vertical";
static const char* HORIZONTAL   = "horizontal";

static const char* MARGIN       = "margin";
static const char* SPACING      = "spacing";

static const char* ROW_NUM      = "row-num";
static const char* COL_NUM      = "col-num";

static const char* ROW          = "row";
static const char* COL          = "col";

static const char* ROW_SPAN     = "row-span";
static const char* COL_SPAN     = "col-span";

////////////////////////////////////////////////////////////////////////////

namespace Sui {
    
CUSTOM_ITEM_MAKER(Layout, QtLayout)

QtLayout::QtLayout(Action*, PropertyNode* props)
    : Item(0, props),
      layout_(0),
      widget_(0)
{
    PropertyNode* orientation_specs = itemProps()->makeDescendant(
        ITEM_PROP_SPECS + String('/') + ORIENTATION);
    orientation_specs->appendChild(new PropertyNode(HORIZONTAL));
    orientation_specs->appendChild(new PropertyNode(VERTICAL));

    itemProps()->makeDescendant(ORIENTATION, VERTICAL, false);
    itemProps()->makeDescendant(MARGIN,  "0", false);
    itemProps()->makeDescendant(SPACING, "2", false);
}

QBoxLayout::Direction QtLayout::direction() const
{
    if (HORIZONTAL == itemProps()->getSafeProperty(ORIENTATION)->getString())
        return QBoxLayout::LeftToRight;
    return QBoxLayout::TopToBottom;
}

bool QtLayout::doAttach()
{
    QWidget* parent_widget = parent()->widget(this);
    DDBG << "QtLayout attach to: " << parent_widget << std::endl;
    DBG_IF(UI.TEST) dump(false);
    if (parent_widget->layout()) {
        widget_ = new QWidget(parent_widget);
        widget_->setObjectName(NOTR("widget_") + get(NAME));
        widget_->show();
        parent_widget = widget_;
    }
    layout_ = new QBoxLayout(parent_widget, direction());
    layout_->setObjectName(NOTR("layout_") + get(NAME));
    layout_->setMargin(getInt(MARGIN));
    layout_->setSpacing(getInt(SPACING));
    layout_->setAutoAdd(true);
    return true;
}

bool QtLayout::doDetach()
{
    if (layout_) {
        DBG(UI.TEST) << "QtLayout: detach: " << std::endl;
        DBG_IF(UI.TEST) dump(false);
        for (Item* i = firstChild(); i; i = i->nextSibling())
            i->detach(false);
        delete widget_;
        delete layout_;
    }
    return true;
}

QWidget* QtLayout::widget(const Item*) const
{
    if (!widget_.isNull())
        return widget_;
    if (!parent())
        return 0;
    return parent()->widget(this);
}

void QtLayout::propertyChanged(PropertyNode* prop)
{
    Item::propertyChanged(prop);
    if (layout_ && prop) {
        if (ORIENTATION == prop->name())
            layout_->setDirection(direction());
    }
}

////////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(GridLayout, QtGridLayout)

QtGridLayout::QtGridLayout(Action*, PropertyNode* props)
    : Item(0, props),
      layout_(0),
      widget_(new QWidget)
{
    widget_->setObjectName(NOTR("widget_") + get(NAME));
    itemProps()->makeDescendant(MARGIN, "11", false);
    itemProps()->makeDescendant(SPACING, "6", false);

    int row_num = itemProps()->makeDescendant(ROW_NUM)->getInt();
    if (1 > row_num)    
        row_num = 1;
    int col_num = itemProps()->makeDescendant(COL_NUM)->getInt();
    if (1 > col_num)    
        col_num = 1;
    layout_ = new QGridLayout(widget_, row_num, col_num);
    layout_->setObjectName(NOTR("layout_") + get(NAME));
    layout_->setMargin(getInt(MARGIN));
    layout_->setSpacing(getInt(SPACING));
}

QWidget* QtGridLayout::widget(const Item*) const
{
    return widget_;
}

////////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(GridWidget, QtGridWidget)

QtGridWidget::QtGridWidget(Action*, PropertyNode* props)
    : Item(0, props),
      widget_(new QWidget)
{
    widget_->setObjectName(NOTR("widget_") + get(NAME));
    itemProps()->makeDescendant(ROW, "1", false);
    itemProps()->makeDescendant(COL, "1", false);
    itemProps()->makeDescendant(ROW_SPAN, "1", false);
    itemProps()->makeDescendant(COL_SPAN, "1", false);

    QLayout* layout = new QVBoxLayout(widget_);
    layout->setAutoAdd(true);
    layout->setObjectName(NOTR("layout_") + get(NAME));
}

bool QtGridWidget::doAttach()
{
    if (GRID_LAYOUT == parent()->itemClass()) {
        QWidget* parent_widget = parent()->widget(this);
        DDBG << "QtGridWidget attach to: " << parent_widget << std::endl;
        DBG_IF(UI.TEST) dump(false);
        widget_->setParent(parent_widget);
        widget_->setVisible(getBool(IS_VISIBLE));
        
        gridLayout_ = static_cast<QtGridLayout*>(parent())->layout();
        int row = getInt(ROW);
        int col = getInt(COL);
        int row_span = getInt(ROW_SPAN);
        int col_span = getInt(COL_SPAN);
        gridLayout_->addMultiCellWidget(widget_, row, row + row_span - 1, 
                                        col, col + col_span - 1);
    }
    return true;
}

bool QtGridWidget::doDetach()
{
    if (widget_ && gridLayout_) {
        DBG(UI.TEST) << "QtGridWidget: detach" << std::endl;
        DBG_IF(UI.TEST) dump(false);
        gridLayout_->remove(widget_);
        widget_->setParent(0);
    }
    return true;
}

QWidget* QtGridWidget::widget(const Item*) const
{
    return widget_;
}

/////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(Stretch, QtStretch)

QtStretch::QtStretch(Action* action, PropertyNode* props)
    : Item(action, props),
      stretch_(itemProps()->makeDescendant(STRETCH, "0", false)->getInt()),
      spacer_(new QWidget)
{
    spacer_->setObjectName(get(NAME));
}

bool QtStretch::doAttach()
{
    QWidget* parent_widget = parent()->widget(this);
    spacer_->setParent(parent_widget);
    spacer_->setVisible(getBool(IS_VISIBLE));
    if (parent() && LAYOUT == parent()->itemClass()) {
        QtLayout* qt_layout = static_cast<QtLayout*>(parent());
        if (QBoxLayout::LeftToRight == qt_layout->direction())
            spacer_->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Minimum);
        else
            spacer_->setSizePolicy(QSizePolicy::Minimum,
                                   QSizePolicy::Expanding);
        qt_layout->layout()->setStretchFactor(spacer_, stretch_);
    }
    return true;
}

QWidget* QtStretch::widget(const Item*) const
{
    return spacer_;
}

} // namespace Sui
