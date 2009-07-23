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
#ifndef STRUCTEDITOR_ATTRIBUTE_ITEM_H_
#define STRUCTEDITOR_ATTRIBUTE_ITEM_H_

#include "structeditor/se_defs.h"
#include "utils/SernaUiItems.h"
#include "ui/LiquidItem.h"

class ElementContext;
class AttrPropertyModel;
class StructEditor;

/////////////////////////////////////////////////////////////////////////////

class STRUCTEDITOR_EXPIMP AttributeItem : public Sui::LiquidItem {
public:
    AttributeItem(PropertyNode* properties);
    virtual ~AttributeItem();

    static AttributeItem*   findInstance(const Sui::Item* from);
    virtual String          itemClass() const {return Sui::CHANGE_ATTRIBUTES;}

    void                    showAttribute(const Common::String& attr);
    AttrPropertyModel*      attrModel() const;

protected:  
    virtual void            visibilityChanged(bool isVisible);
    virtual void            escapePressed();
    virtual void            inserted();
    virtual void            removed();
    virtual QWidget*        makeWidget(QWidget* parent, Type type);
    virtual const char*     selfTypeId() const
        { return typeid(AttributeItem).name(); }
    StructEditor*           structEditor() const;

private:
    Common::OwnerPtr<ElementContext> elementContext_;
};

#endif // STRUCTEDITOR_ATTRIBUTE_ITEM_H_
