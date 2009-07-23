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
#ifndef CORE_PREFERENCES_H_
#define CORE_PREFERENCES_H_

#include "core/core_defs.h"
#include "common/PropertyTree.h"

class QWidget;

class CORE_EXPIMP PreferencesTabItem :
    public Common::SernaApiRefCounted,
    public Common::XListItem<PreferencesTabItem,
        Common::XTreeNodeRefCounted<PreferencesTabItem> >,
    public Common::PropertyTree {
    
public:
    static const char TAB_INSCRIPTION_PROP[]; // overrides winTitle, if needed
    static const char TAB_INDEX_PROP[];       // tab index (lexical order)
    
    PreferencesTabItem(const Common::String& index);

    virtual QWidget* makeTab(Common::PropertyNode*) const = 0;
    virtual ~PreferencesTabItem();
};

class CORE_EXPIMP PreferencesTabList :
                  public Common::XList<PreferencesTabItem,
                    Common::XTreeNodeRefCounted<PreferencesTabItem> > {
public:
    void                        insertTab(PreferencesTabItem*);
    static PreferencesTabList&  instance();
    ~PreferencesTabList();
};

template <class TAB> class PreferencesTabRegistrar {
public:
    PreferencesTabRegistrar()
    {
        TAB* tab = new TAB;
        tab->incRefCnt();   // make tab permanent
        PreferencesTabList::instance().insertTab(tab);
    }
};

#define REGISTER_BUILTIN_PREFERENCES_TAB(name, idx, pred) \
class name##WidgetTabItem : public PreferencesTabItem { \
public:                                                 \
    name##WidgetTabItem()                               \
        : PreferencesTabItem(#idx) {}                   \
    virtual QWidget* makeTab(PropertyNode* props) const \
    {                                                   \
        if (pred) { QWidget* w = new name##Widget(props); \
            w->setObjectName(#name "Widget"); return w; } \
        else return 0;                                  \
    }                                                   \
};                                                      \
static PreferencesTabRegistrar<name##WidgetTabItem> tab_registrar;

#endif // CORE_PREFERENCES_H_
