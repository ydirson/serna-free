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
#ifndef UI_PROPERTY_EDITOR_H_
#define UI_PROPERTY_EDITOR_H_

#include "ui/ui_defs.h"
#include "common/PropertyTree.h"
#include "common/OwnerPtr.h"

#include <QtAbstractPropertyManager>
#include <QtTreePropertyBrowser>

namespace Sui {

class UI_EXPIMP PropertyEditor : public QtTreePropertyBrowser,
                                 public Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    PropertyEditor(QWidget* parent);
    virtual ~PropertyEditor();

    void    init(Common::PropertyNode* root, bool use_top = false);

    QtAbstractPropertyManager*
        managerForName(const Common::String& name) const;

    // reimplement his to return correct qtpropertymanager for property node
    virtual QtAbstractPropertyManager*
        managerForProperty(const Common::PropertyNode*) const;

    // reimplement this for custom qtproperty settings (eg. enable/disable)
    virtual void    setPropValue(QtProperty*, const Common::PropertyNode*);

    // reimplement this if you want enums with icons
    virtual QIcon   valueIcon(const Common::PropertyNode*,
                              const Common::String& iconName) const;

    // returns corresponding property node for qproperty
    Common::PropertyNode* qpropValue(const QtProperty*) const;

    // called when value of QtProperty had been changed. Default
    // implementation of this function sets value of corresponding
    // property node.
    virtual void    valueChanged(QtProperty*);

private slots:
    void            value_changed(QtProperty*);

private:
    class PropertyManagerSet;
    class QPropToNodeMap;
    class NodeToQPropMap;
    template <class MC, class FC> void add_manager(const Common::String&);
    QtProperty* build_properties(Common::PropertyNode*, QtProperty* qpp);
    void        set_watchers(Common::PropertyNode*);
    virtual void    propertyChanged(Common::PropertyNode*);

    Common::OwnerPtr<PropertyManagerSet> propertyManagers_;
    Common::OwnerPtr<QPropToNodeMap>     nodeMap_;
    Common::OwnerPtr<NodeToQPropMap>     qpropMap_;
    Common::PropertyNodePtr              root_;
    bool                                 valueLock_;
};

} // namespace Sui

#endif // UI_PROPERTY_EDITOR_H_
