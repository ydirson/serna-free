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
#include "ui/impl/ui_debug.h"
#include "ui/UiAction.h"
#include "ui/UiItems.h"
#include "ui/UiProps.h"
#include "common/StringTokenizer.h"
#include "common/ScopeGuard.h"

#include <QTimer>
#include <QLayout>
#include <QSplitter>
#include <QPointer>

#include <list>

using namespace Common;

namespace Sui {

static const char* PROPORTIONS  = "proportions";
static const char* ORIENTATION  = "orientation";
static const char* VERTICAL     = "vertical";
static const char* HORIZONTAL   = "horizontal";

class QtSplitter : public QObject, public Item {
    Q_OBJECT
public:
    QtSplitter(Action* action, PropertyNode* properties);
    virtual ~QtSplitter() {};

    virtual String      itemClass() const { return SPLITTER; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }
    virtual QWidget*    widget(const Item* child) const;

public slots:
    void                setSizes();
    void                updateSavedSizes();

protected:
    virtual bool        doAttach();

private:
    QPointer<QSplitter> splitter_;
};

///////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(Splitter, QtSplitter)

QtSplitter::QtSplitter(Action* action, PropertyNode* props)
    : Item(action, props),
      splitter_(new QSplitter(0))
{
    UI_LOCK_PROP_GUARD
    splitter_->setObjectName(get(NAME));
    PropertyNode* orientation_specs = itemProps()->makeDescendant(
        Sui::ITEM_PROP_SPECS + String('/') + ORIENTATION);
    orientation_specs->appendChild(new PropertyNode(HORIZONTAL));
    orientation_specs->appendChild(new PropertyNode(VERTICAL));

    if (VERTICAL == itemProps()->getSafeProperty(ORIENTATION)->getString())
        splitter_->setOrientation(Qt::Vertical);
    else
        splitter_->setOrientation(Qt::Horizontal);
    connect(splitter_, SIGNAL(splitterMoved(int, int)), 
        this, SLOT(updateSavedSizes()));
}

bool QtSplitter::doAttach()
{
    if (!Item::doAttach())
        return false;
    QWidget* parent_widget = Item::parent()->widget(this);
    if (!parent_widget)
        return false;
    if (0 == parent_widget->layout()) {
        QBoxLayout* layout = new QHBoxLayout(parent_widget);
        layout->addWidget(splitter_);
    } else 
        parent_widget->layout()->addWidget(splitter_);
    QTimer::singleShot(10, this, SLOT(setSizes()));
    return true;
}

QWidget* QtSplitter::widget(const Item*) const
{
    return splitter_;
}

void QtSplitter::setSizes()
{
    QList<int> sizes;
    for (StringTokenizer t(get(PROPORTIONS)); t;) 
        sizes.push_back(t.next().toInt());
    DBG(UI.SPLITTER) << "set sizes to = " << get(PROPORTIONS) << std::endl;
    splitter_->blockSignals(true);
    splitter_->setSizes(sizes);
    splitter_->blockSignals(false);
}

void QtSplitter::updateSavedSizes()
{
    DBG(UI.SPLITTER) << "QtSplitter: updateSavedSizes\n";
    if (!splitter_)
        return;
    String str;
    QList<int> list = splitter_->sizes();
    QList<int>::iterator i;
    for (i = list.begin(); i != list.end(); ++i) 
        str += String::number(*i) + ' ';
    str = str.simplifyWhiteSpace();
    DBG(UI.SPLITTER) << "QtSplitter: Stretches: " << str << std::endl;
    itemProps()->makeDescendant(PROPORTIONS)->setString(str);
}

} // namespace Sui

#include "moc/QtSplitter.moc"

