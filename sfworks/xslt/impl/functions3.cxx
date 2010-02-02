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
#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
#include "xpath/NodeSet.h"
#include "xslt/impl/FunctionExprImpl.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/Instruction.h"
#include "xpath/XpathFunctionFactory.h"
#include "xpath/impl3/BooleanValue.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/Instance.h"
#include "grove/Nodes.h"

USING_COMMON_NS;

namespace Xslt {

using Xpath::FunctionExpr;
using Xpath::FunctionArgExpr;

template <XsltFunctionId::Id T> FunctionExpr* 
func_maker(const Common::String& funcName, const FunctionArgExpr* args) 
{
    return new FunctionExprImpl<T>(funcName, args);
}

extern const char xslt_ext_uri[];

#define FIM(id) func_maker<XsltFunctionId::id>

static Xpath::FunctionFactory::FunctionMakerInfo func_table[] = {
    { "current",            "", FIM(Current),   0, 0 },
    { "document",           "", FIM(Document),  1, 2 },
    { "element-available",  "", FIM(ElementAvailable),  1, 1 },
    { "function-available", "", FIM(FunctionAvailable), 1, 1 },
    { "format-number",      "", FIM(FormatNumber), 2, 3 },
    { "generate-id",        "", FIM(GenerateId),   0, 1 },
    { "key",                "", FIM(Key),          2, 2 },
    { "system-property",    "", FIM(SystemProperty), 1, 1 },
    // extensions
    { "docbook-orderedlist-itemnumber", 
        xslt_ext_uri, FIM(DbkOrdlistItemnumber),   1, 99 },
    { "cals-attribute", 
        xslt_ext_uri, FIM(CalsAttribute),          1, 99 },
    { 0,                    0,  0,                 0, 0 }
};

/////////////////////////////////////////////////////////////

const Xpath::FunctionFactory::FunctionMakerInfo* 
XsltFunctionFactory::findFunction(const Common::String& localName, 
                                  const Common::String& uri) const
{
    static Xpath::XpathFunctionFactory xpf;
    const FunctionMakerInfo* fi = xpf.findFunction(localName, uri);
    if (fi)
        return fi;
    for (fi = func_table; fi->name; ++fi) 
        if (localName == fi->name && uri == fi->uri)
            return fi;
    return 0; 
}

/////////////////////////////////////////////////////////////

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::Current>::eval(const Xpath::NodeSetItem&,
                                            Xpath::ExprInst& ei) const
{
    Xpath::NodeSet node_set;
    const Instance& ci = static_cast<const Instance&>(ei.exprContext());
    node_set += ci.currentTemplate()->nsiContext().node();
    return new Xpath::NodeSetValue(node_set);
}

///////////////////////////////////////////////////////////////

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::FunctionAvailable>::eval
    (const Xpath::NodeSetItem& nsi, Xpath::ExprInst& ei) const
{        
    Xpath::FuncArgEvaluator fargs(nsi, ei, this);
    const Instance& ci = static_cast<const Instance&>(ei.exprContext());
    GroveLib::QualifiedName qn;
    qn.parse(fargs.arg(0)->to(Xpath::Value::STRING, &ei)->getString());
    String uri = qn.prefix().isEmpty() ? GroveLib::XmlNs::defaultNs()
        : ci.element()->getXmlNsByPrefix(qn.prefix());
    if (uri.isNull() && qn.prefix() == "xse")
        uri = xslt_ext_uri;
    return Xpath::BooleanValue::instance(XsltFunctionFactory().
        findFunction(qn.localName(), uri) ||
        ei.exprContext().getExternalFunction(qn.localName(), uri));
}

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::ElementAvailable>::eval(const Xpath::NodeSetItem&,
                                                     Xpath::ExprInst& ei) const
{
    (void) ei;
    return Xpath::BooleanValue::instance(true);
}

///////////////////////////////////////////////////////////////

// NOT IMPLEMENTED
template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::FormatNumber>::eval(const Xpath::NodeSetItem&,
                                                 Xpath::ExprInst&) const
{
    Xpath::NodeSet node_set;
    return new Xpath::NodeSetValue(node_set);
}

///////////////////////////////////////////////////////////////

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::GenerateId>::eval(const Xpath::NodeSetItem& nsi,
                                               Xpath::ExprInst& ei) const
{
    Xpath::FuncArgEvaluator fargs(nsi, ei, this);
    const GroveLib::Node* n = 0;
    if (fargs.argNum() == 0)
        n = nsi.node();
    else if (fargs.argNum() == 1) {
        const Xpath::NodeSet& ns =
            fargs.arg(0)->to(Xpath::Value::NODESET)->getNodeSet();
        if (ns.contextSize() == 0)
            return new Xpath::StringValue("");
        n = ns.first()->node();
    } else
        throw Xslt::Exception(XsltMessages::xpathEvalArgs,
                              ei.exprContext().contextString());
    return new Xpath::StringValue("X" + String::number((intptr_t)n, 16));
}

///////////////////////////////////////////////////////////////

// NOT IMPLEMENTED
template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::Key>::eval(const Xpath::NodeSetItem&,
                                        Xpath::ExprInst&) const
{
    Xpath::NodeSet node_set;
    return new Xpath::NodeSetValue(node_set);
}

///////////////////////////////////////////////////////////////

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::SystemProperty>::eval
    (const Xpath::NodeSetItem& nsi, Xpath::ExprInst& ei) const
{
    Xpath::FuncArgEvaluator fargs(nsi, ei, this);
    String arg = fargs.arg(0)->to(Xpath::Value::STRING, &ei)->getString();
    if (arg == "xsl:version")
        return new Xpath::NumericValue(1.0);
    else if (arg == "xsl:vendor")
        return new Xpath::StringValue("Syntext Inc.");
    else if (arg == "xsl:vendor-url")
        return new Xpath::StringValue("http://www.syntext.com");
    else
        return new Xpath::StringValue("unknown-property");
}

} // namespace Xslt
#endif // USE_XPATH3
