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
#ifndef SAPI_UI_ITEM_H_
#define SAPI_UI_ITEM_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/common/xtn_wrap.h"
#include "sapi/app/UiAction.h"

class QWidget;

namespace SernaApi {

class SernaDoc;
class UiLiquidItemBase;
    
/// Base class for the elements of the user interface
class SAPI_EXPIMP UiItem : public RefCountedWrappedObject {
public:
    UiItem(SernaApiBase* = 0);
    virtual ~UiItem();

    /// Returns the name of item instance
    SString         name() const;
    /// Returns the assotiated Action
    UiAction        action() const;
    /// Returns type of item instance
    SString         itemClass() const;
    SString         widgetClass() const;
    
    /// Returns widget made by this item
    QWidget*        widget() const;

    /// Returns requested property from this item, 
    /// or from its action if property does not exist in this item
    PropertyNode    property(const SString& prop) const;
    /// Updates item properties (if asked) and returns them
    PropertyNode    itemProps(bool update = false);
    /// For MultiAction items - returns the current subaction root property
    PropertyNode    currActionProp() const;


    /// Returns string property value
    SString         get(const SString& propName) const;  
    /// Returns string property value
    SString         getTranslated(const SString& propName) const;  
    /// Returns boolean property value
    bool            getBool(const SString& propName) const;
    /// Returns integer property value
    int             getInt(const SString& propName) const;
    /// Returns double property value
    double          getDouble(const SString& propName) const;


    /// Sets string property value
    void            set(const SString& propName, const SString& value);
    /// Sets boolean property value
    void            setBool(const SString& propName, bool value);
    /// Sets integer property value
    void            setInt(const SString& propName, int value);
    /// Sets double property value
    void            setDouble(const SString& propName, double value);


    /// Sets visible state.
    void            setVisible(bool);
    /// Returns visible state.
    bool            isVisible() const;

    /// Attaches UI item
    void            attach(bool recursive = false);
    /// Detaches UI item
    void            detach(bool recursive = false);

    /// Dispatch Item command if any
    void            dispatch();
    /// Set the focus to the widget corresponding to this UI item
    void            grabFocus() const;
    /// Set the focus back to the document editor
    void            releaseFocus() const;

    /// Finds UI command by name.
    UiAction        findAction(const SString& name) const;
    /// Finds UiItem by name.
    UiItem          findItemByName(const SString& name) const;
    /// Look up  UiItem by class.
    UiItem          findItemByClass(const SString& name) const;
    /// Finds UiItem by UiActions.
    UiItem          findItemByAction(const UiAction& name) const;

    /// Shows context menu for this liquid item at global pos (x,y)
    void            showContextMenu(int x, int y);

    /// Translate string \a str in given context.
    static SString  translate(const char* context, const SString& str);

    XTREENODE_WRAP_DECL(UiItem)

    /// Downcasts UiItem to SernaDoc
    SernaDoc        asSernaDoc() const;

    /// For top-level items: append to documentItem(), attach and show
//    void            attachAndShow();
};

/// Implementations of custom liquid items should inherit from this class.
class SAPI_EXPIMP UiLiquidItemBase : public SimpleWrappedObject {
public:
    virtual ~UiLiquidItemBase(); 

    UiItem      item() const;       // corresponding UI item
    
    enum Type { WIDGET          = 0x01,
                UNDOCKED_TOOL   = 0x02,
                VERTICAL_TOOL   = 0x04,
                HORIZONTAL_TOOL = 0x08 };

    virtual SString     itemClass() const = 0;
    virtual SString     widgetClass() const;

    /// Shows context menu for this liquid item at global pos (x,y)
    virtual void        showContextMenu(int x, int y);
    /// Releases input focus and passes it back to the editor
    virtual void        releaseFocus() const;
    /// Set the focus to the widget corresponding to this UI item
    virtual void        grabFocus() const;
    
    /// may be reimplemented for custom attach
    virtual bool        doAttach();
    /// may be reimplemented for custom detach
    virtual bool        doDetach();
    /// Called when item property changes
    virtual void        propertyChanged(const PropertyNode& prop);
    /// invoked when escape button is pressed
    virtual void        escapePressed();
    /// May be reimplemented in custom item to indicate active focus
    virtual void        widgetFocusChanged(bool);

    /// Widget factory - must be implemented in custom item, and
    /// should return QWidget instance
    virtual QWidget*    makeWidget(QWidget* parent, Type type) = 0;

    UiLiquidItemBase(SernaApiBase* = 0);
    
private:
    friend class LiquidItemWrap;
    //UiLiquidItemBase(const UiLiquidItemBase&);
    //UiLiquidItemBase& operator=(const UiLiquidItemBase&);
};

} // namespace SernaApi

#endif // SAPI_UI_ITEM_H_
