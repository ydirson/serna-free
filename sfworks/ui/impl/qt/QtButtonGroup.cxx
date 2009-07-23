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
#include "ui/impl/qt/QtButtonGroup.h"
#include "ui/impl/ui_debug.h"

#include <QButtonGroup>
#include <QLayout>

using namespace Common;

static const char* TITLE = "title";

////////////////////////////////////////////////////////////////////////////

namespace Sui {

CUSTOM_ITEM_MAKER(ButtonGroup, QtButtonGroup)

QtButtonGroup::QtButtonGroup(Action*, PropertyNode* props)
    : Sui::Item(0, props),
      group_(new QButtonGroup(0))
{
    //group_->setTitle(itemProps()->makeDescendant(TITLE)->getString());
    //group_->setColumnLayout(0, Qt::Vertical);
    //group_->layout()->setSpacing(6);
    //group_->layout()->setMargin(11);
    //group_->layout()->setAutoAdd(true);
    group_->setObjectName(get(NAME));
}

QtButtonGroup::~QtButtonGroup()
{
    DDBG << "~QtButtonGroup: " << this << std::endl;
}

QWidget* QtButtonGroup::widget(const Item*) const
{
    return 0;//group_;
}

} // namespace Sui
