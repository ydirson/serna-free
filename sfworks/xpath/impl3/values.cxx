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
#include "xpath/impl3/FpMake.h"
#include "xpath/impl3/BooleanValue.h"
#include "xpath/impl3/StringValue.h"
#include "xpath/impl3/NumericValue.h"
#include "xpath/impl3/NodeSetValue.h"
#include "xpath/impl3/ExprInst.h"
#include "xpath/ConstValueHolder.h"
#include "grove/Nodes.h"
#include "grove/NodeVisitor.h"

USING_COMMON_NS;

namespace Xpath {

typedef ExprInst Visitor;

namespace {

    bool equals(const ValueImpl& left, const ValueImpl& right,
                ValueImpl::Visitor* visitor) {
        return (left.eq(right, visitor));
    }
    bool notEquals(const ValueImpl& left, const ValueImpl& right,
                   Visitor* visitor) {
        return (left.notEq(right, visitor));
    }
    bool less(const ValueImpl& left, const ValueImpl& right,
              Visitor* visitor) {
        return (left.lt(right, visitor));
    }
    bool lessOrEquals(const ValueImpl& left, const ValueImpl& right,
                      Visitor* visitor) {
        return (left.ltEq(right, visitor));
    }
    bool greater(const ValueImpl& left, const ValueImpl& right,
                 Visitor* visitor) {
        return (left.gt(right, visitor));
    }
    bool greaterOrEquals(const ValueImpl& left, const ValueImpl& right,
                         Visitor* visitor) {
        return (left.gtEq(right, visitor));
    }
}

NodeSetValue::NodeSetValue(NodeSet& nodeSet)
    : nodeSet_(nodeSet)
{
}

bool NodeSetValue::isThere(bool (*compare)(const ValueImpl&,
                                           const ValueImpl&,
                                           Visitor* visitor),
                           const ValueImpl& right,
                           Visitor* visitor) const
{
    const NodeSetItem* n = nodeSet_.realFirst();
    while (n) {
        ConstValueImplPtr left =
            new StringValue(elementToString(n->node(), visitor));
        if (compare(*left, right, visitor))
            return true;
        n = n->realNext();
    }
    return false;
}

bool NodeSetValue::isEqual(const Value& val) const
{
    if (NODESET == val.type())
        return (nodeSet_ == val.getNodeSet());
    return false;
}

bool NodeSetValue::eq(const ValueImpl& val, Visitor* visitor) const
{
    Type t = val.type();
    if (BOOLEAN == t)
        return (val.eq(*this, visitor));
    if (STRING == t && !nodeSet_.first())
        return val.getString().isEmpty();
    return isThere(equals, val, visitor);
}

bool NodeSetValue::notEq(const ValueImpl& val, Visitor* visitor) const
{
    Type t = val.type();
    if (BOOLEAN == t)
        return (val.notEq(*this, visitor));
    if (STRING == t && !nodeSet_.first())
        return !val.getString().isEmpty();
    return isThere(notEquals, val, visitor);
}

bool NodeSetValue::lt(const ValueImpl& val, Visitor* visitor) const
{
    if (BOOLEAN == val.type())
        return (val.gt(*this, visitor));
    return isThere(less, val, visitor);
}

bool NodeSetValue::ltEq(const ValueImpl& val, Visitor* visitor) const
{
    if (BOOLEAN == val.type())
        return (val.gtEq(*this, visitor));
    return isThere(lessOrEquals, val, visitor);
}

bool NodeSetValue::gt(const ValueImpl& val, Visitor* visitor) const
{
    if (BOOLEAN == val.type())
        return (val.lt(*this, visitor));
    return isThere(greater, val, visitor);
}

bool NodeSetValue::gtEq(const ValueImpl& val, Visitor* visitor) const
{
    if (BOOLEAN == val.type())
        return (val.ltEq(*this, visitor));
    return isThere(greaterOrEquals, val, visitor);
}

bool NodeSetValue::getBool() const
{
    throw Xpath::Exception(XpathMessages::retrievalBoolFromSet);
}

const String& NodeSetValue::getString() const
{
    throw Xpath::Exception(XpathMessages::retrievalStrFromSet);
}

double  NodeSetValue::getDouble() const
{
    throw Xpath::Exception(XpathMessages::retrievalDblFromSet);
}

ConstValueImplPtr NodeSetValue::toBooleanValue(Visitor*) const
{
    return BooleanValue::instance(0 != nodeSet_.contextSize());
}

ConstValueImplPtr NodeSetValue::toNumericValue(Visitor* visitor) const
{
    // The nodeset is first converted to string as if by a call to the
    // string function and then converted in the same way as a string argument
    return to(Value::STRING, visitor)->to(Value::NUMERIC, visitor);
}

String NodeSetValue::elementToString(GroveLib::Node* node, Visitor* visitor)
{
    if (visitor)
        visitor->registerConvVisitor(node,
            GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
            GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED|
            GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);

    String str;
    for (GroveLib::Node* cn = node->firstChild(); cn; cn = cn->nextSibling()) {
        if (cn->nodeType() == GroveLib::Node::TEXT_NODE) {
            if (visitor)
                visitor->registerConvVisitor(cn,
                    GroveLib::NodeVisitor::NOTIFY_TEXT_CHANGED|
                    GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);
            str += static_cast<const GroveLib::Text*>(cn)->data();
        }
        else
            if (cn->nodeType() == GroveLib::Node::ELEMENT_NODE)
                str += elementToString(cn, visitor);
    }
    return str;
}

ConstValueImplPtr NodeSetValue::toStringValue(Visitor* visitor) const
{
    const NodeSetItem* n = nodeSet_.first();
    if (n) {
        GroveLib::Node* node = n->node();
        switch (node->nodeType()) {
            case GroveLib::Node::ELEMENT_NODE :
            case GroveLib::Node::DOCUMENT_NODE :
            case GroveLib::Node::DOCUMENT_FRAGMENT_NODE :
                {
                    return new StringValue(elementToString(node, visitor));
                    break;
                }
            case GroveLib::Node::ATTRIBUTE_NODE :
                {
                    GroveLib::Attr* attr = static_cast<GroveLib::Attr*>(node);
                    if (visitor)
                        visitor->registerConvVisitor(attr->element(),
                            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_ADDED|
                            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_REMOVED|
                            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED);
                    return new StringValue(attr->value());
                }
            case GroveLib::Node::TEXT_NODE :
                {
                    if (visitor)
                        visitor->registerConvVisitor(node,
                            GroveLib::NodeVisitor::NOTIFY_TEXT_CHANGED|
                            GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);
                    return new StringValue
                        (static_cast<const GroveLib::Text*>(node)->data());
                    break;
                }
            case GroveLib::Node::PI_NODE :
                {
                    return new StringValue(PI_CAST(node)->data());
                    break;
                }
            case GroveLib::Node::COMMENT_NODE :
                {
                    return new StringValue(COMMENT_CAST(node)->comment());
                    break;
                }
            default :
                return new StringValue("");
        }
    }
    return new StringValue("");
}

ConstValueImplPtr NodeSetValue::toNodeSetValue(Visitor*) const
{
    return this;
}

void NodeSetValue::dump() const
{
    DDINDENT;
    DDBG << "NodeSetValue:" << std::endl;
    nodeSet_.dump();
}

class NodePtrSetValue : public NodeSetValue {
public:
    NodePtrSetValue(NodeSet& nset)
        : NodeSetValue(nset) {}
    ~NodePtrSetValue()
    {
        for (NodeSetItem* n = getNodeSet().first(); n; n = n->next()) {
            if (n->node()->decRefCnt() <= 0)
                delete n->node();
        }
    }
};

Value* Value::makeNodeSet(const NodeSet& nset)
{
    NodeSet nset_copy;
    nset.copyTo(nset_copy);
    for (NodeSetItem* n = nset_copy.first(); n; n = n->next())
        n->node()->incRefCnt();
    return new NodePtrSetValue(nset_copy);
}

ConstValueHolder* ConstValueHolder::makeNodeSet(const NodeSet& nset)
{
    ConstValueHolder* vh = new ConstValueHolder;
    vh->value_ = Value::makeNodeSet(nset);
    return vh;
}

///////////////////////////////////////////////////////////////////////////

BooleanValue::BooleanValue(bool val)
    : value_(val)
{
}

bool BooleanValue::isEqual(const Value& val) const
{
    if (BOOLEAN == val.type())
        return (value_ == val.getBool());
    return false;
}

bool BooleanValue::eq(const ValueImpl& val, Visitor* visitor) const
{
    return value_ == val.to(Value::BOOLEAN, visitor)->getBool();
}

bool BooleanValue::notEq(const ValueImpl& val, Visitor* visitor) const
{
    return value_ != val.to(Value::BOOLEAN, visitor)->getBool();
}

bool BooleanValue::lt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (value_ < val.to(Value::BOOLEAN, visitor)->getBool());
    return (to(Value::NUMERIC, visitor)->getDouble() <
            val.to(Value::NUMERIC, visitor)->getDouble());
}

bool BooleanValue::ltEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (value_ <= val.to(Value::BOOLEAN, visitor)->getBool());
    return (to(Value::NUMERIC, visitor)->getDouble() <=
            val.to(Value::NUMERIC, visitor)->getDouble());
}

bool BooleanValue::gt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (value_ > val.to(Value::BOOLEAN, visitor)->getBool());
    return (to(Value::NUMERIC, visitor)->getDouble() >
            val.to(Value::NUMERIC, visitor)->getDouble());
}

bool BooleanValue::gtEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (value_ >= val.to(Value::BOOLEAN, visitor)->getBool());
    return (to(Value::NUMERIC, visitor)->getDouble() >=
            val.to(Value::NUMERIC, visitor)->getDouble());
}

const String& BooleanValue::getString() const
{
    throw Xpath::Exception(XpathMessages::retrievalStrFromBool);
}

double BooleanValue::getDouble() const
{
    throw Xpath::Exception(XpathMessages::retrievalDblFromBool);
}

const NodeSet& BooleanValue::getNodeSet() const
{
    throw Xpath::Exception(XpathMessages::retrievalSetFromBool);
}

ConstValueImplPtr BooleanValue::toBooleanValue(Visitor*) const
{
    return this;
}

ConstValueImplPtr BooleanValue::toNumericValue(Visitor*) const
{
    return new NumericValue((value_) ? 1 : 0);
}

ConstValueImplPtr BooleanValue::toStringValue(Visitor*) const
{
    return new StringValue((value_) ? "true" : "false");
}

ConstValueImplPtr BooleanValue::toNodeSetValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::conversionStrToSet);
}

const ConstValueImplPtr& BooleanValue::instance(bool val)
{
    static ConstValueImplPtr true_value(new BooleanValue(true));
    static ConstValueImplPtr false_value(new BooleanValue(false));

    return val ? true_value : false_value;
}

void BooleanValue::dump() const
{
    DDINDENT;
    DDBG << "BooleanValue: " << (value_ ? "true" : "false") << std::endl;
}

ConstValueHolder* ConstValueHolder::makeBool(bool v)
{
    ConstValueHolder* vh = new ConstValueHolder;
    vh->value_ = BooleanValue::instance(v);
    return vh;
}

Value* Value::makeBool(bool v)
{
    return new BooleanValue(v);
}

///////////////////////////////////////////////////////////////////////////

NumericValue::NumericValue(double num)
    : number_(num)
{
}

bool NumericValue::isEqual(const Value& val) const
{
    if (NUMERIC == val.type()) {
        const NumericValue* num =
            static_cast<const NumericValue*>(val.to(Value::NUMERIC).pointer());

                bool num_isnan = num->isNan() || num->isNegInfinity() ||
                    num->isPosInfinity() || isNan() ||
                    isNegInfinity() || isPosInfinity();
                bool ok = (num->isNan() && isNan());
                bool ok1 = (num->isNegInfinity() && isNegInfinity());
        bool ok2 = (num->isPosInfinity() && isPosInfinity());
                bool ok3 = (!num_isnan) && (num->getDouble() == number_);
                return (ok || ok1 || ok2 || ok3);
/*        return ((num->isNan() && isNan()) ||
                (num->isNegInfinity() && isNegInfinity()) ||
                (num->isPosInfinity() && isPosInfinity()) ||
                (num->getDouble() == number_));
*/
    }
    return false;
}

bool NumericValue::eq(const ValueImpl& val, Visitor* visitor) const
{
    switch (val.type()) {
        case NODESET :
        case BOOLEAN :
            return (val.eq(*this, visitor));
        default:
            return isEqual(*val.to(Value::NUMERIC, visitor).pointer());
    }
}

bool NumericValue::notEq(const ValueImpl& val, Visitor* visitor) const
{
    switch (val.type()) {
        case NODESET :
        case BOOLEAN :
            return (val.notEq(*this, visitor));
        default:
            return !isEqual(*val.to(Value::NUMERIC, visitor).pointer());
    }
}

bool NumericValue::lt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.gt(*this, visitor));
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
    if (isNaN(number_) || isNaN(y))
        return false;
    return (number_ < y);
}

bool NumericValue::ltEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.gtEq(*this, visitor));
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
    if (isNaN(number_) || isNaN(y))
        return false;
    return (number_ <= y);
}

bool NumericValue::gt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.lt(*this, visitor));
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
    if (isNaN(number_) || isNaN(y))
        return false;
    return (number_ > y);
}

bool NumericValue::gtEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.ltEq(*this, visitor));
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
    if (isNaN(number_) || isNaN(y))
        return false;
    return (number_ >= y);
}

bool NumericValue::getBool() const
{
    throw Xpath::Exception(XpathMessages::retrievalBoolFromNum);
}

const String& NumericValue::getString() const
{
    throw Xpath::Exception(XpathMessages::retrievalStrFromNum);
}

const NodeSet& NumericValue::getNodeSet() const
{
    throw Xpath::Exception(XpathMessages::retrievalSetFromNum);
}

ConstValueImplPtr NumericValue::toBooleanValue(Visitor*) const
{
    return BooleanValue::instance(!isNan() && (0 != number_));
}

ConstValueImplPtr NumericValue::toNumericValue(Visitor*) const
{
    return this;
}

static const char*
dbltostr(const double& dbl)
{
    if (isNaN(dbl))
        return "NaN";
    else
        if (getInf() == dbl)
            return "Infinity";
        else
            if (getNegInf() == dbl)
                return "-Infinity";
    return 0;
}

ConstValueImplPtr NumericValue::toStringValue(Visitor*) const
{
        const char* p = dbltostr(number_);
        if (0 != p)
            return new StringValue(p);
        String str;
        str.setNum(number_);
        return new StringValue(str);
}

ConstValueImplPtr NumericValue::toNodeSetValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::conversionNumToSet);
}

bool NumericValue::isNan() const
{
    return isNaN(number_);
}

bool NumericValue::isInfinity() const
{
    return (isPosInfinity() || isNegInfinity());
}

bool NumericValue::isPosInfinity() const
{
    return (number_ == getInf());
}

bool NumericValue::isNegInfinity() const
{
    return (number_ == getNegInf());
}

ConstValueHolder* ConstValueHolder::makeDouble(double v)
{
    ConstValueHolder* vh = new ConstValueHolder;
    vh->value_ = new NumericValue(v);
    return vh;
}

Value* Value::makeDouble(double v)
{
    return new NumericValue(v);
}

void NumericValue::dump() const
{
    DDINDENT;
    const char* p = dbltostr(number_);
    DDBG << "NumericValue: ";
    if (0 == p)
        DDBG << number_ << std::endl;
    else
        DDBG << p << std::endl;
}

////////////////////////////////////////////////////////////////////////

StringValue::StringValue(const String& str)
    : string_(str)
{
}

bool StringValue::isEqual(const Value& val) const
{
    if (STRING == val.type())
        return (string_ == val.getString());
    return false;
}

bool StringValue::eq(const ValueImpl& val, Visitor* visitor) const
{
    switch (val.type()) {
        case NODESET :
        case BOOLEAN :
        case NUMERIC :
            return (val.eq(*this, visitor));
        default:
            return (val.to(Value::STRING, visitor)->getString() == string_);
    }
}

bool StringValue::notEq(const ValueImpl& val, Visitor* visitor) const
{
    switch (val.type()) {
        case NODESET :
        case BOOLEAN :
        case NUMERIC :
            return (val.notEq(*this, visitor));
        default:
            return (val.to(Value::STRING, visitor)->getString() != string_);
    }
}

bool StringValue::lt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.gt(*this, visitor));
    double x = to(Value::NUMERIC, visitor)->getDouble();
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
        if (isNaN(x) || isNaN(y))
                return false;
    return (x < y);
}

bool StringValue::ltEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.gtEq(*this, visitor));
    double x = to(Value::NUMERIC, visitor)->getDouble();
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
        if (isNaN(x) || isNaN(y))
                return false;
    return (x <= y);
}

bool StringValue::gt(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.lt(*this, visitor));
    double x = to(Value::NUMERIC, visitor)->getDouble();
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
        if (isNaN(x) || isNaN(y))
                return false;
    return (x > y);
}

bool StringValue::gtEq(const ValueImpl& val, Visitor* visitor) const
{
    if (NODESET == val.type())
        return (val.ltEq(*this, visitor));
    double x = to(Value::NUMERIC, visitor)->getDouble();
    double y = val.to(Value::NUMERIC, visitor)->getDouble();
        if (isNaN(x) || isNaN(y))
                return false;
    return (x >= y);
}

bool StringValue::getBool() const
{
    throw Xpath::Exception(XpathMessages::retrievalBoolFromStr);
}

double StringValue::getDouble() const
{
    throw Xpath::Exception(XpathMessages::retrievalDblFromStr);
}

const NodeSet& StringValue::getNodeSet() const
{
    throw Xpath::Exception(XpathMessages::retrievalSetFromStr);
}

ConstValueImplPtr StringValue::toBooleanValue(Visitor*) const
{
    return BooleanValue::instance(!string_.isEmpty());
}

ConstValueImplPtr StringValue::toNumericValue(Visitor*) const
{
    bool ok;
    double num = string_.toDouble(&ok);
    if (ok)
        return new NumericValue(num);
    return new NumericValue(getNaN());
}

ConstValueImplPtr StringValue::toStringValue(Visitor*) const
{
    return this;
}

ConstValueImplPtr StringValue::toNodeSetValue(Visitor*) const
{
    throw Xpath::Exception(XpathMessages::conversionStrToSet);
}

const ConstValueImplPtr& StringValue::empty()
{
    static ConstValueImplPtr empty_value(new StringValue(""));
    return empty_value;
}

ConstValueHolder* ConstValueHolder::makeString(const Common::String& s)
{
    ConstValueHolder* vh = new ConstValueHolder;
    vh->value_ = new StringValue(s);
    return vh;
}

Value* Value::makeString(const Common::String& s)
{
    return new StringValue(s);
}

void StringValue::dump() const
{
    DDINDENT;
    DDBG << "StringValue: '" << string_ << '\'' << std::endl;
}

} // namespace Xpath
