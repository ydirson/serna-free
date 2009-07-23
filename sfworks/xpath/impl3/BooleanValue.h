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

#ifndef XPATH_BOOLEAN_VALUE_H
#define XPATH_BOOLEAN_VALUE_H

#include "common/String.h"
#include "xpath/xpath_defs.h"
#include "xpath/impl3/ValueImpl.h"

/*!
 */
namespace Xpath {

class NodeSet;
class BooleanValue;
typedef COMMON_NS::RefCntPtr<const BooleanValue> ConstBooleanValuePtr;


class XPATH_EXPIMP BooleanValue : public ValueImpl {
public:
    XPATH_OALLOC(BooleanValue);

    Type                type() const {return BOOLEAN;}

    bool                isEqual(const Value& val) const;

    bool                eq(const ValueImpl& val, Visitor* visitor) const;
    bool                notEq(const ValueImpl& val, Visitor* visitor) const;
    bool                lt(const ValueImpl& val, Visitor* visitor) const;
    bool                ltEq(const ValueImpl& val, Visitor* visitor) const;
    bool                gt(const ValueImpl& val, Visitor* visitor) const;
    bool                gtEq(const ValueImpl& val, Visitor* visitor) const;

    bool                getBool() const {return value_;}
    double              getDouble() const;
    const COMMON_NS::String&       getString() const;
    const NodeSet&      getNodeSet() const;

    static const ConstValueImplPtr& instance(bool val);

    BooleanValue(bool val);

protected:
    ConstValueImplPtr   toBooleanValue(Visitor* visitor) const;
    ConstValueImplPtr   toNumericValue(Visitor* visitor) const;
    ConstValueImplPtr   toStringValue(Visitor* visitor) const;
    ConstValueImplPtr   toNodeSetValue(Visitor* visitor) const;

    void                dump() const;

private:
    const bool          value_;
};

} // namespace Xpath

#endif // XPATH_BOOLEAN_VALUE_H
