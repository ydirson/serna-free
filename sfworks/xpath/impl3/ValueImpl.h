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
/*! \file
 */

#ifndef XPATH_VALUE_IMPL_H
#define XPATH_VALUE_IMPL_H

#include "xpath/xpath_defs.h"
#include "xpath/Value.h"

namespace Xpath {

class NodeSet;
class ValueImpl;
class ExprInst;

typedef COMMON_NS::RefCntPtr<const ValueImpl> ConstValueImplPtr;

class XPATH_EXPIMP ValueImpl : public Value {
public:
    XPATH_OALLOC(ValueImpl);

    typedef ExprInst Visitor;

    virtual bool eq(const ValueImpl& val, Visitor* visitor) const = 0;
    virtual bool notEq(const ValueImpl& val, Visitor* visitor) const = 0;
    virtual bool lt(const ValueImpl& val, Visitor* visitor) const = 0;
    virtual bool ltEq(const ValueImpl& val, Visitor* visitor) const = 0;
    virtual bool gt(const ValueImpl& val, Visitor* visitor) const = 0;
    virtual bool gtEq(const ValueImpl& val, Visitor* visitor) const = 0;

    bool    operator==(const Value& val) const {
        return eq(static_cast<const ValueImpl&>(val), 0);
    }
    bool    operator!=(const Value& val) const {
        return notEq(static_cast<const ValueImpl&>(val), 0);
    }
    bool    operator<(const Value& val) const {
        return lt(static_cast<const ValueImpl&>(val), 0);
    }
    bool    operator<=(const Value& val) const {
        return ltEq(static_cast<const ValueImpl&>(val), 0);
    }
    bool    operator>(const Value& val) const {
        return gt(static_cast<const ValueImpl&>(val), 0);
    }
    bool    operator>=(const Value& val) const {
        return gtEq(static_cast<const ValueImpl&>(val), 0);
    }

    ConstValuePtr               to(Type type) const;
    ConstValueImplPtr           to(Type type, Visitor* visitor) const;

    static const ConstValueImplPtr& getUndefined();

protected:
    virtual ConstValueImplPtr   toBooleanValue(Visitor* visitor) const = 0;
    virtual ConstValueImplPtr   toNumericValue(Visitor* visitor) const = 0;
    virtual ConstValueImplPtr   toStringValue(Visitor* visitor) const = 0;
    virtual ConstValueImplPtr   toNodeSetValue(Visitor* visitor) const = 0;
};

} // namespace Xpath

#endif // XPATH_VALUE_IMPL_H
