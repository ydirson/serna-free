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
#include "ui/UiProps.h"
#include "ui/UiAction.h"
#include "ui/UiActionGroup.h"
#include "common/RefCntPtr.h"
#include <set>

using namespace Common;

namespace Sui {

class ActionGroup::ActionHolder : public std::set<RefCntPtr<Action> > {};

ActionGroup::ActionGroup()
    : actions_(new ActionHolder)
{
}

ActionGroup::~ActionGroup()
{
}

void ActionGroup::addAction(Action* action)
{
    actions_->insert(action);
}

void ActionGroup::removeAction(Action* action)
{
    ActionHolder::iterator it = actions_->find(action);
    if (it == actions_->end())
        return;
    actions_->erase(it);
}

void ActionGroup::setActionProp(const String& name, bool value) const
{
    ActionHolder::iterator it = actions_->begin();
    for (; it != actions_->end(); ++it) 
        (*it)->property(name)->setBool(value);
}

void ActionGroup::setEnabled(bool isOn) const
{
    setActionProp(IS_ENABLED, isOn);
}

void ActionGroup::setToggled(bool isOn) const
{
    setActionProp(IS_TOGGLED, isOn);
}

void ActionGroup::setItemVisible(bool isOn) const
{
    setActionProp(IS_VISIBLE, isOn);
}

} // namespace Sui
