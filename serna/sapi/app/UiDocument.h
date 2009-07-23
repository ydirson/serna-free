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
#ifndef SAPI_UI_DOCUMENT_H_
#define SAPI_UI_DOCUMENT_H_

#include "sapi/app/UiItem.h"
#include "sapi/app/UiAction.h"

namespace SernaApi {

class PropertyNode;

/// Set of UI commands (UiActions)
class SAPI_EXPIMP UiActionSet : public SimpleWrappedObject {
public:
    UiActionSet(SernaApiBase* = 0);
    ~UiActionSet();

    /// Lookup appropriate command by name
    UiAction   findAction(const SString&) const;

    /// Deletes command
    void       removeAction(const UiAction&);

    /// UiAction factory
    UiAction   makeAction(const PropertyNode&);

    /// UiAction factory
    UiAction   makeAction(const SString& name,
                          const SString& inscription = SString(),
                          const SString& icon = SString(),
                          const SString& tooltip = SString(),
                          const SString& whatsThis = SString(),
                          const SString& accel = SString(),
                          bool isTogglable = false);
};

// Maker functor for creating custom UI items
class SAPI_EXPIMP UiItemMaker : public RefCountedWrappedObject {
public:
    
    /// Creates liquid item and appends it to the document item
    static UiItem   makeLiquidItem(const PropertyNode& properties,
                                   UiLiquidItemBase*);
    virtual UiItem makeItem(const UiAction& action, 
                            const PropertyNode& properties) = 0;
    virtual ~UiItemMaker();

protected:
    UiItemMaker();

private:
    UiItemMaker(const UiItemMaker&);
    UiItemMaker& operator=(const UiItemMaker&);
};
   
/// Aggregates the components of the document representation UI
class SAPI_EXPIMP UiDocument : public UiItem {
public:
    /// Returns UiActionSet - set of UI commands.
    UiActionSet  actionSet();

    /// Add maker for the custom UI items. \a acceptableIn is a 
    /// whitespace-separated list of item classes where \a itemClass
    /// should be allowed as a child.
    PropertyNode addItemMaker(const SString& itemClass,
                              UiItemMaker*  itemMaker,
                              const SString& acceptableIn = SString());
    /// UiItem factory
    UiItem   makeItem(const SString& itemClass,
                      const PropertyNode& properties) const;

    /// Shortcut or makeItem(), append to document, attach and show
    void     makeAndShow(const SString& itemClass,
                         const PropertyNode& properties);

    UiDocument(SernaApiBase* = 0);
    virtual ~UiDocument();
};

} // namespace SernaApi

#endif // SAPI_UI_DOCUMENT_H_
