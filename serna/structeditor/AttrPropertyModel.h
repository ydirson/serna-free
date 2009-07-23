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
#ifndef STRUCTEDITOR_ATTR_PROPERTY_MODEL_H_
#define STRUCTEDITOR_ATTR_PROPERTY_MODEL_H_

#include "common/PropertyTree.h"
#include <QPointer>

/////////////////////////////////////////////////////////////////////////////

extern bool is_ns_map(const Common::String& qname);

namespace GroveLib {
    class Node;
    class Element;
    class ElementMatcher;
}

class HelpHandle;

class AttributeContext {
public:
    AttributeContext();
    AttributeContext(const Common::String& elementName, 
                     const Common::String& topSysid, 
                     const GroveLib::Node* nsContext);
    virtual ~AttributeContext();
    
    const GroveLib::Node*   nsContext() const;
    Common::String          elementName() const;
    Common::String          topSysid() const;    

    bool                    isHiddenAttr(const Common::String& attrName) const;
    Common::PropertyNodePtr getAttrHelp(const Common::String& attrName) const;
    
public:
    virtual bool            addAttribute(const Common::String& name, 
                                         const Common::String& value);
    virtual bool            renameAttribute(const Common::String& attrName,
                                            const Common::String& newName,
                                            const Common::String& value);
    virtual bool            changeAttribute(const Common::String& attrName,
                                            const Common::String& newValue);
    virtual bool            removeAttribute(const Common::String& attrName);

protected:
    virtual const GroveLib::Element*    element() const;

    void    init(const GroveLib::ElementMatcher* patternMatcher,
                 const HelpHandle* helpHandle);

protected:
    Common::String          elementName_;
    const Common::String    topSysid_;
    const GroveLib::Node*   nsContext_;

    Common::OwnerPtr<const GroveLib::ElementMatcher> patternMatcher_;
    const HelpHandle*       helpHandle_;
};

/////////////////////////////////////////////////////////////////////////////

class QAbstractItemModel;
class QAbstractItemView;
class ElementAttrModel;

extern const int ATTR_NAME_COLUMN;
extern const int ATTR_VALUE_COLUMN;
extern const int ATTR_TYPE_COLUMN;

class AttrPropertyModel {
public:
    typedef Common::PropertyNodePtr PropertyPtr;
    
    enum AttrRole {
        DEFAULT_VALUE_ROLE = Qt::UserRole + 1,  
        ELEMENT_NAME_ROLE
    };
    enum STATE {
        IDLE = 0,
        EDIT_NAME,
        EDIT_VALUE
    };    
    
    AttrPropertyModel(AttributeContext* attrContext);
    virtual ~AttrPropertyModel() {};

    static AttrPropertyModel*   makeInstance(AttributeContext* context);

    virtual void        addAttribute() = 0;
    virtual void        removeAttribute() {};

    const AttributeContext* attributeContext() const { return attrContext_; }

    bool                isEditing();
    virtual void        reset() = 0;
    void                setView(QAbstractItemView* view);
    QAbstractItemView*  view() const;

    virtual void        update(const PropertyPtr& attrs, 
                               const PropertyPtr& specs,
                               const PropertyPtr& nsMap);
    void                sort(int column, bool isAscending);    
    virtual void        selectAttribute(const Common::String& /*attrName*/)= 0;
    
protected:
    bool                isValidName(const Common::String& attrName) const;
    Common::PropertyNode*   findByQname(const PropertyPtr& prop,
                                        const Common::String& attrName) const;

protected:
    AttributeContext*       attrContext_;

    Common::PropertyNodePtr attrs_;
    Common::PropertyNodePtr specs_;
    Common::PropertyNodePtr nsMap_;

    STATE                   state_;
    QPointer<QAbstractItemView> view_;    
};

#endif // STRUCTEDITOR_ATTR_PROPERTY_MODEL_H_
