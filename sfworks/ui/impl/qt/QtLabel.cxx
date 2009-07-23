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
#include "ui/IconProvider.h"
#include "ui/impl/qt/QtLabel.h"
#include "ui/impl/ui_debug.h"
#include "common/ScopeGuard.h"

#include <QLabel>
#include <QVariant>
#include <QLayout>

using namespace Common;

namespace Sui {

/////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(Label, QtLabel)

static const char* SIZE_POLICY  = "size-policy";
static const char* IGNORED      = "ignored";

static void set_label_color(QLabel* label, const PropertyNode* prop)
{
    if (!prop)
        return;
    QString color_str(prop->getString());
    QWidget* parent = label->parentWidget();
    if ("light" == color_str) {
        if (parent)
            label->setPaletteBackgroundColor(
                parent->paletteBackgroundColor().light());
        return;
    }
    if (color_str.isEmpty()) {
        if (parent)
            label->setPaletteBackgroundColor(
                parent->paletteBackgroundColor());
        return;
    }
    QColor color(color_str);
    if (color.isValid()) {
        QPalette palette;
        palette.setColor(label->backgroundRole(), Qt::green);
        label->setPalette(palette);
    }
}

QtLabel::QtLabel(Action* action, PropertyNode* props)
    : Item(action, props),
      stretch_(getInt(STRETCH)),
      label_(new QLabel(getTranslated(INSCRIPTION), 0))
{
    UI_LOCK_PROP_GUARD
    label_->setObjectName(get(NAME));

    PropertyNode* size_specs = itemProps()->makeDescendant(
        Sui::ITEM_PROP_SPECS + String('/') + SIZE_POLICY);
    size_specs->appendChild(new PropertyNode(IGNORED));

    String tooltip = getTranslated(TOOLTIP);
    if (!tooltip.isEmpty())
        label_->setToolTip(tooltip + ' ');

    label_->setIndent(3);
    label_->setAutoFillBackground(true);
    PropertyNode* size_policy = itemProps()->getProperty(SIZE_POLICY);
    if (size_policy && IGNORED == size_policy->getString())
        label_->setSizePolicy(QSizePolicy::Ignored,
                              label_->sizePolicy().verData());
    if (Item::property("width"))
        label_->setFixedWidth(getInt("width"));
    if (Item::property("height"))
        label_->setFixedHeight(getInt("height"));
    if (Item::property("frameShape"))
        label_->setProperty("frameShape", 
            QVariant(QString(get("frameShape"))));
    if (Item::property("frameShadow"))
        label_->setProperty("frameShadow", 
            QVariant(QString(get("frameShadow"))));
    
    set_label_color(label_, itemProps()->makeDescendant("color"));
    PropertyNode* color_spec = 
        itemProps()->makeDescendant(ITEM_PROP_SPECS + String("/color"));
    QStringList list = QColor::colorNames();
    list.sort();
    for (QStringList::iterator i = list.begin(); i != list.end(); i++)
        color_spec->appendChild(new PropertyNode(*i));
}

QWidget* QtLabel::widget(const Item*) const
{
    return label_;
}

void QtLabel::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    Item::propertyChanged(prop);
    if (label_.isNull() || !prop) 
        return;
    if (INSCRIPTION == prop->name()) {
        if (prop->getString().isEmpty())
            label_->setPixmap(icon_provider().getPixmap(get(ICON)));
        else 
            label_->setText(getTranslated(prop) + ' ');
    }
    if ("color" == prop->name())
        set_label_color(label_, prop);
    if (TOOLTIP == prop->name()) {
        if (!prop->getString().isEmpty())
            label_->setToolTip(getTranslated(prop) + ' ');
        else
            label_->setToolTip(QString());
    }
}

bool QtLabel::doAttach()
{
    if (!Item::doAttach())
        return false;
    set_label_color(label_, Item::property("color"));
    QWidget* parent_widget = Item::parent()->widget(this);
    if (parent_widget && parent_widget->layout()) {
        parent_widget->layout()->addWidget(label_);
        QBoxLayout* box_layout = 
            dynamic_cast<QBoxLayout*>(parent_widget->layout());
        if (box_layout) 
            box_layout->setStretchFactor(label_, stretch_);
    }
    return true;
}

} // namespace Sui
