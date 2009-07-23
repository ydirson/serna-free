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

#include "xpath/impl3/debug.h"
#include "xpath/impl3/ValueImpl.h"
#include "grove/Decls.h"

using namespace Common;

namespace Xpath {

class XPATH_EXPIMP UndefinedValue : public ValueImpl {
public:
    XPATH_OALLOC(UndefinedValue);
    bool                isEqual(const Value& val) const { return val.type() ==
                                                                     type(); }

    bool                eq(const ValueImpl& val, Visitor* visitor) const;
    bool                notEq(const ValueImpl& val, Visitor* visitor) const;
    bool                lt(const ValueImpl& val, Visitor* visitor) const;
    bool                ltEq(const ValueImpl& val, Visitor* visitor) const;
    bool                gt(const ValueImpl& val, Visitor* visitor) const;
    bool                gtEq(const ValueImpl& val, Visitor* visitor) const;

    Type                type() const {return UNDEFINED;}
    bool                getBool() const;
    double              getDouble() const;
    const String&       getString() const;
    const NodeSet&      getNodeSet() const;

    void                dump() const;
protected:
    ConstValueImplPtr   toBooleanValue(Visitor* visitor) const;
    ConstValueImplPtr   toNumericValue(Visitor* visitor) const;
    ConstValueImplPtr   toStringValue(Visitor* visitor) const;
    ConstValueImplPtr   toNodeSetValue(Visitor* visitor) const;
};

bool UndefinedValue::eq(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::notEq(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::lt(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::ltEq(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::gt(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::gtEq(const ValueImpl&, Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationComp);
    return false;
}

bool UndefinedValue::getBool() const
{
    throw Xpath::Exception(XpathMessages::retrievalBoolUndef);
}

const String& UndefinedValue::getString() const
{
    throw Xpath::Exception(XpathMessages::retrievalStrUndef);
}

double UndefinedValue::getDouble() const
{
    throw Xpath::Exception(XpathMessages::retrievalDblUndef);
}

const NodeSet& UndefinedValue::getNodeSet() const
{
    throw Xpath::Exception(XpathMessages::retrievalSetUndef);
}

ConstValueImplPtr UndefinedValue::toBooleanValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationConv);
    return 0;
}

ConstValueImplPtr UndefinedValue::toNumericValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationConv);
    return 0;
}

ConstValueImplPtr UndefinedValue::toStringValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationConv);
    return 0;
}

ConstValueImplPtr UndefinedValue::toNodeSetValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::operationConv);
    return 0;
}


void UndefinedValue::dump() const
{
    DDINDENT;
    DDBG << "UndefinedValue" << std::endl;
}

ConstValuePtr ValueImpl::to(Type type) const
{
    return to(type, 0).pointer();
}

ConstValueImplPtr ValueImpl::to(Type type, Visitor* visitor) const
{
    switch (type) {
        case UNDEFINED :
            return getUndefined();
        case BOOLEAN :
            return toBooleanValue(visitor);
        case STRING :
            return toStringValue(visitor);
        case NUMERIC :
            return toNumericValue(visitor);
        case NODESET :
            return toNodeSetValue(visitor);
        default:
            return 0;
    }
    return 0;
}

const ConstValueImplPtr& ValueImpl::getUndefined()
{
    static ConstValueImplPtr undefined_value(new UndefinedValue);
    return undefined_value;
}

} // namespace Xpath

