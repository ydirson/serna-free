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
#ifndef SAPI_UI_COMMAND_H_
#define SAPI_UI_COMMAND_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/common/WrappedObject.h"
#include "sapi/common/xtn_wrap.h"

namespace SernaApi {

class PropertyNode;

/// UiAction is class for UI commands, emited by buttons, menu items and widgets.
class SAPI_EXPIMP UiAction : public RefCountedWrappedObject {
public:
    UiAction(SernaApiBase* = 0);
    virtual ~UiAction();

    /// Command name
    SString         name() const;
    /// Returns actin property by name
    PropertyNode    property(const SString& prop) const;
    /// Properties of the action
    PropertyNode    properties() const;

    /// Accesor to concrete property value.
    SString         get(const SString& propName) const;
    /// Accesor to concrete property value.
    SString         getTranslated(const SString& propName) const;
    /// Returns boolean property value
    bool            getBool(const SString& propName) const;
    /// Returns integer property value
    int             getInt(const SString& propName) const;
    /// Returns double property value
    double          getDouble(const SString& propName) const;

    /// Set property with name and value.
    void            set(const SString& propName, const SString& value);
    /// Sets boolean property value
    void            setBool(const SString& propName, bool value);
    /// Sets integer property value
    void            setInt(const SString& propName, int value);
    /// Sets double property value
    void            setDouble(const SString& propName, double value);

    /// Returns enabled state.
    bool            isEnabled() const;
    /// Change enabled state.
    void            setEnabled(bool);  
    /// Returns true if command can be toggled.
    bool            isToggleable() const;
    /// Returns toggled state.
    bool            isToggledOn() const;
    /// Sets toggled state.
    void            setToggled(bool);                            

    /// Dispatches the command.
    void            dispatch() const;
    /// Dumps command properties.
    void            dump() const;

    XTREENODE_WRAP_DECL(UiAction)
};

} // namespace SernaApi

#endif // SAPI_UI_COMMAND_H_


