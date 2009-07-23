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
#ifndef UI_ACTION_SET_H_
#define UI_ACTION_SET_H_

#include "ui/ui_defs.h"
#include "ui/UiItem.h"
#include "ui/UiAction.h"

#include <map>

namespace Sui {

class Document;

class UI_EXPIMP ActionPred {
public:
    virtual ~ActionPred() {}
    virtual bool operator()(const Action*) const = 0;
};


class UI_EXPIMP ActionSet : public SernaApiBase, 
                            protected Common::PropertyNodeWatcher {
public:
    typedef std::map<Common::String, ActionPtr> ActionPtrMap;
    typedef Common::String String;

    ActionSet();
    ~ActionSet();

    const ActionPtrMap& actionMap() const { return actionMap_; }

    Action*         findAction(const String& name) const;
    Action*         findAction(const ActionPred& pred) const;

    void            remapAction(const String& name, Action* action);
    void            removeAction(Action* action);
    Action*         makeAction(const Common::PropertyNode*);
    Action*         makeAction(const String& name,
                               const String& inscription = String(),
                               const String& icon = String(),
                               const String& toolTip = String(),
                               const String& whatsThis = String(),
                               const String& accel = String(),
                               bool isToggleable = false);
private:
    friend class Document;

    virtual void    propertyChanged(Common::PropertyNode* property);
    void            checkAccel(Action* action);
    Action*         find_action(const String& name) const;

    ActionPtrMap    actionMap_;
    ActionPtrMap    accelMap_;
};

} // namespace Sui

#endif // UI_ACTION_SET_H_
