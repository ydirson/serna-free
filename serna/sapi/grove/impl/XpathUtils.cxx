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
#include "sapi/grove/XpathUtils.h"
#include "grove/Nodes.h"
#include "xpath/Value.h"
#include "xpath/NodeSet.h"
#include "xpath/ConstValueHolder.h"
#include "xpath/ExprContext.h"
#include "xpath/Expr.h"
#include "xpath/Engine.h"
#include "xslt/ExternalFunction.h"
#include "common/asserts.h"
#include "xpath/impl3/FpMake.h"

namespace SernaApi {

GroveNode XpathNodeSet::firstNode() const
{
    if (0 == size())
        return GroveNode();
    return operator[](0);
}
    
GroveNode XpathNodeSet::lastNode() const
{
    if (0 == size())
        return GroveNode();
    return operator[](size() - 1);
}

class XpathValue;

class XpathValueHolder : public Xpath::ValueHolder {
public:
    using SubscriberPtrPublisher::notifyChanged;

    XpathValueHolder(Xpath::Value* xpv)
        : value_(xpv) {}
    XpathValueHolder(const Xpath::ConstValuePtr& value)
        : value_(value) {}
    Xpath::ConstValuePtr value() const
    {
        return value_;
    }
    void setValue(const Xpath::ConstValuePtr& value)
    {
        value_ = value;
        notifyChanged();
    }
private:
    Xpath::ConstValuePtr    value_;
};
    
#define XV_REP(rep)         static_cast<XpathValueHolder*>(rep.getRep())
#define XV_SELF             XV_REP((*this))
#define NODE_REP(n)         static_cast<GroveLib::Node*>(n)
#define XVREP_INIT(fp, v)   RefCountedWrappedObject\
    (new XpathValueHolder(Xpath::Value::fp(v)))

XpathValue::XpathValue(const SString& s)
    : XVREP_INIT(makeString, s)    
{
}

XpathValue::XpathValue(double v)
    : XVREP_INIT(makeDouble, v)
{
}

XpathValue::XpathValue(bool v)
    : XVREP_INIT(makeBool, v)
{
}

XpathValue::XpathValue(SernaApiBase* value)
    : RefCountedWrappedObject
        (value ? new XpathValueHolder(static_cast<Xpath::Value*>(value)) : 0)
{
}

static void fill_node_set(Xpath::NodeSet& xp_nset, const XpathNodeSet& nset)
{
    for (unsigned int i = 0; i < nset.size(); ++i)
        xp_nset += NODE_REP(nset[i].getRep());
}

XpathValue::XpathValue(const XpathNodeSet& nset)    
{
    Xpath::NodeSet xp_nset;
    fill_node_set(xp_nset, nset);
    setRep(new XpathValueHolder(Xpath::Value::makeNodeSet(xp_nset)));
}

SString XpathValue::getString() const
{
    if (!getRep())
        return SString();
    return XV_SELF->value()->to(Xpath::Value::STRING)->getString();        
}

double XpathValue::getDouble() const
{
    if (!getRep())
        return Xpath::getNaN();
    return XV_SELF->value()->to(Xpath::Value::NUMERIC)->getDouble();        
}

bool XpathValue::getBool() const
{
    if (!getRep())
        return false;
    return XV_SELF->value()->to(Xpath::Value::BOOLEAN)->getBool();        
}

XpathNodeSet XpathValue::getNodeSet() const
{
    XpathNodeSet nset;
    if (!getRep())
        return nset;
    if (XV_SELF->value()->type() != Xpath::Value::NODESET)
        return nset;
    Xpath::NodeSetItem* nsi = XV_SELF->value()->getNodeSet().first();
    for (; nsi; nsi = nsi->next())
        nset.push_back(nsi->node());
    return nset;
}

#define SET_VALUE(v) if (!getRep()) setRep(new XpathValueHolder(v)); \
    else XV_SELF->setValue(v)

void XpathValue::setString(const SString& s)
{
    SET_VALUE(Xpath::Value::makeString(s));
}

void XpathValue::setDouble(double v)
{
    SET_VALUE(Xpath::Value::makeDouble(v));
}

void XpathValue::setBool(bool v)
{
    SET_VALUE(Xpath::Value::makeBool(v));
}

void XpathValue::setNodeSet(const XpathNodeSet& nset)
{
    Xpath::NodeSet xp_nset;
    fill_node_set(xp_nset, nset);
    SET_VALUE(Xpath::Value::makeNodeSet(xp_nset));
}

void XpathValue::notifyChanged()
{
    if (getRep())
        XV_SELF->notifyChanged();
}

XpathValue::~XpathValue()
{
}

/////////////////////////////////////////////////////////////

XpathExpr::XpathExpr(const SString& expr, const GroveNodeWithNamespace& nsRes)
    : RefCountedWrappedObject(0)
{
    try {
        Xpath::ExprPtr expr_ptr = Xpath::Engine::makeExpr(expr, 
            static_cast<GroveLib::NodeWithNamespace*>(nsRes.getRep())); 
        setRep(expr_ptr.pointer());
    } catch (...) {}
}
    
XpathValue XpathExpr::eval(const GroveNode& context) const
{
    if (!context.getRep() || !getRep()) 
        return XpathValue(/*(SernaApiBase*)0*/);
    static Xpath::ExprContext expr_context;
    Xpath::NodeSetItem nsi(NODE_REP(context.getRep()));
    RT_ASSERT(dynamic_cast<Xpath::Expr*>(getRep()));
    Common::RefCntPtr<Xpath::ValueHolder> vh = 
        static_cast<Xpath::Expr*>(getRep())->makeInst(nsi, expr_context);
    // const-cast is because of refcnt + passing via SernaApiBase
    return XpathValue(const_cast<Xpath::Value*>(vh->value().pointer()));
}

/////////////////////////////////////////////////////////////

class XsltFuncWrap : public Xslt::ExternalFunction {
public:
    XsltFuncWrap(const Common::String& name,
                 const Common::String& uri, XsltExternalFunction* xf)
        : ExternalFunction(name, uri), xf_(xf) {}
        
    virtual Xpath::ValueHolder* eval(const Xpath::ConstValuePtrList& vl) const 
    {
        // copy value list
        XpathValueList xpl;
        xpl.reserve(vl.size());
        for (unsigned int i = 0; i < vl.size(); ++i) 
            xpl.push_back(const_cast<Xpath::Value*>(vl[i].pointer()));
        XpathValue xv = xf_->eval(xpl);
        Xpath::ValueHolder* vh = XV_REP(xv);
        xv.SimpleWrappedObject::setRep(0); // keep object but set refcnt=0
        vh->decRefCnt();
        return vh;
    }
private:
    XsltExternalFunction* xf_;
};

XsltExternalFunction::XsltExternalFunction(const SString& name, 
                                           const SString& nsUri)
    : RefCountedWrappedObject(new XsltFuncWrap(name, nsUri, this))
{
}

XpathValue XsltExternalFunction::eval(const XpathValueList&) const
{
    return XpathValue("");
}

XsltExternalFunction::~XsltExternalFunction()
{
}

} // namespace SernaApi
