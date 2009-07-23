// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class UiItem : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/UiItem.h"
using namespace SernaApi;
%End
public:
    
    SString         name() const;
    UiAction        action() const;
    SString         itemClass() const;
    SString         widgetClass() const;

    PropertyNode    property(const SString&) const;
    PropertyNode    itemProps(bool = 0);
    PropertyNode    currActionProp() const;

    SString         get(const SString&) const;
    SString         getTranslated(const SString&) const;
    bool            getBool(const SString&) const;
    int             getInt(const SString&) const;
    double          getDouble(const SString&) const;

    void            set(const SString&, const SString&);
    void            setBool(const SString&, bool);
    void            setInt(const SString&, int);
    void            setDouble(const SString&, double);
    
    void            setVisible(bool);
    bool            isVisible() const;

    QWidget*        widget() const;

    void            attach(bool = false);
    void            detach(bool = false);
    void            dispatch();
    void            grabFocus() const;
    void            releaseFocus() const;
    void            showContextMenu(int, int);
    
    UiAction        findAction(const SString&) const;
    UiItem          findItemByName(const SString&) const;
    UiItem          findItemByClass(const SString&) const;
    UiItem          findItemByAction(const UiAction&) const;


// copy-paste section: XTreeNode portion
    
    UiItem          firstChild() const;
    UiItem          lastChild() const;
    UiItem          getChild(unsigned long) const;
    UiItem          root() const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const UiItem&);
    void            removeAllChildren();
    
    UiItem          nextSibling() const;
    UiItem          prevSibling() const;
    UiItem          parent() const;
    unsigned int    siblingIndex() const;
    void            insertBefore(const UiItem&);
    void            insertAfter(const UiItem&);
    void            remove();
    void            removeGroup(const UiItem&);
    SString         getTreelocString(const UiItem& = UiItem()) const; 
    UiItem          getByTreelocString(const SString&) const;
   
    static SString  translate(const char*, const SString&);
 
    SernaDoc        asSernaDoc() const;
    
    bool operator==(const UiItem&) const;
    bool operator!=(const UiItem&) const;

    UiItem(const UiItem&);
    virtual ~UiItem();
};

class UiLiquidItemBase : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/app/UiItem.h"
using namespace SernaApi;
%End
public:
    virtual ~UiLiquidItemBase(); 

    UiItem      item() const;

    enum Type { WIDGET          = 0x01,
                UNDOCKED_TOOL   = 0x02,
                VERTICAL_TOOL   = 0x04,
                HORIZONTAL_TOOL = 0x08 };

    virtual SString     itemClass() const = 0;
    virtual SString     widgetClass() const;
    virtual void        showContextMenu(int, int);
    virtual void        releaseFocus() const;
    virtual void        grabFocus() const;
    
    virtual bool        doAttach();
    virtual bool        doDetach();
    virtual void        propertyChanged(const PropertyNode&);
    virtual void        escapePressed();
    virtual void        widgetFocusChanged(bool);

    virtual QWidget*    makeWidget(QWidget*, Type) = 0 /Factory/;
};

