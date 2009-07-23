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
#include <iostream>
#include "xpath/ExprContext.h"
#include "xpath/Expr.h"
#include "xpath/NodeSet.h"
#include "xpath/ConstValueHolder.h"
#include "xpath/ExternalFunction.h"
#include "xpath/XpathFunctionFactory.h"
#include "xpath/impl3/NumericValue.h"
#include "xpath/impl3/BooleanValue.h"
#include "xpath/impl3/NodeSetValue.h"
#include "xpath/impl3/StringValue.h"
#include "xpath/impl3/ExprInst.h"
#include "xpath/impl3/expressions/NodeTestExpr.h"
#include "xpath/impl3/expressions/ConstExpr.h"
#include "xpath/impl3/debug.h"
#include "grove/Nodes.h"
#include "grove/IdManager.h"
#include "grove/EntityDeclSet.h"
#include "grove/RedlineUtils.h"
#include "common/StringTokenizer.h"
#include <math.h>

#include "xpath/impl3/expressions/AxisExpr.h"
#include "xpath/impl3/expressions/SlashExpr.h"
#include "xpath/impl3/expressions/FunctionExpr.h"

using namespace Common;

namespace Xpath {

template <FunctionExpr::FunctionId F> class FunctionExprImpl :
                                          public FunctionExpr {
public:
    FunctionExprImpl(const COMMON_NS::String& funcName,
        const FunctionArgExpr* args = 0)
            : FunctionExpr(funcName, args) {}
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const;
    virtual FunctionId        functionId() const { return F; }
};

template <FunctionExpr::FunctionId F> class OptimizedFunctionExprImpl :
                                          public FunctionExpr {
public:
    OptimizedFunctionExprImpl(const COMMON_NS::String& funcName,
        const FunctionArgExpr* args = 0)
            : FunctionExpr(funcName, args) {}
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const;
    virtual ExprImplPtr       optimize(int);
    virtual FunctionId        functionId() const { return F; }
};

template <> class FunctionExprImpl<FunctionExpr::Lang> : public FunctionExpr {
public:
    FunctionExprImpl(const COMMON_NS::String& funcName,
        const FunctionArgExpr* args = 0)
            : FunctionExpr(funcName, args) {}
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const;
    virtual FunctionId        functionId() const { return Lang; }
    virtual void    makeNodeTestChain(ExprImpl*& prev)
    {
        nte_.makeNodeTestChain(prev);
    }

private:
    LangNodeTestExpr nte_;
};

class ExternalFunctionImpl : public FunctionExpr {
public:
    ExternalFunctionImpl(const Common::String& funcName,
                         const Common::String& ns,
                         const FunctionArgExpr* args)
        : FunctionExpr(funcName, args), ns_(ns)
    {
    }
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const;
    virtual FunctionId        functionId() const { return ExternalFunction; }

private:
    Common::String ns_;
};

template <FunctionExpr::FunctionId T> FunctionExpr* 
func_maker(const Common::String& funcName, const FunctionArgExpr* args) 
{
    return new FunctionExprImpl<T>(funcName, args);
}

template <FunctionExpr::FunctionId T> FunctionExpr* 
opt_func_maker(const Common::String& funcName, const FunctionArgExpr* args) 
{
    return new OptimizedFunctionExprImpl<T>(funcName, args);
}

#define FIM(id) func_maker<FunctionExpr::id>
#define FOM(id) opt_func_maker<FunctionExpr::id>

#define EXSLT_URI "http://exslt.org/common"

FunctionFactory::FunctionMakerInfo func_table[] = {
    { "boolean",            "", FIM(Boolean),    1, 1 },
    { "ceiling",            "", FIM(Ceiling),    1, 1 },
    { "concat",             "", FIM(Concat),     2, 999999 },
    { "contains",           "", FOM(Contains),   2, 2 },
    { "count",              "", FIM(Count),      1, 1 },
    { "false",              "", FIM(False),      0, 0 },
    { "floor",              "", FIM(Floor),      1, 1 },
    { "id",                 "", FIM(Id),         1, 2 },
    { "lang",               "", FIM(Lang),       1, 1 },
    { "last",               "", FIM(Last),       0, 0 },
    { "level",              "", FIM(Level),      0, 0 }, // extension
    { "local-name",         "", FIM(LocalName),  0, 1 },
    { "name",               "", FIM(Name),       0, 1 },
    { "normalize-space",    "", FIM(NormalizeSpace), 0, 1 },
    { "not",                "", FOM(Not),        1, 1 },
    { "namespace-uri",      "", FIM(NsUri),      0, 1 },
    { "number",             "", FIM(Number),     0, 1 },
    { "position",           "", FIM(Position),   0, 0 },
    { "round",              "", FIM(Round),      1, 1 },
    { "starts-with",        "", FIM(StartWith),  2, 2 },
    { "string",             "", FIM(StringFunc), 0, 1 },
    { "string-length",      "", FIM(StringLength),    0, 1 },
    { "substring-after",    "", FIM(SubstringAfter),  0, 2 },
    { "substring-before",   "", FIM(SubstringBefore), 0, 2 },
    { "substring",          "", FIM(Substring),   2, 3 },
    { "sum",                "", FIM(Sum),         1, 1 },
    { "translate",          "", FIM(Translate),   3, 3 },
    { "true",               "", FIM(True),        0, 0 },
    { "unparsed-entity-uri", "", FIM(UnparsedEntityUri), 1, 1},
    { "redline",            SERNA_XPATH_EXTENSIONS_URI,
                                 FIM(Redline),    0, 1 },
    { "node-set",           EXSLT_URI, 
                                 FIM(NodeSetFunc), 1, 1 },
    { 0,                "", 0, 0, 0 }
};

const FunctionFactory::FunctionMakerInfo* 
XpathFunctionFactory::findFunction(const Common::String& localname, 
                                   const Common::String& uri) const
{
    const FunctionMakerInfo* fi = func_table;
    for (; fi->name; ++fi) 
        if (localname == fi->name && uri == fi->uri)
            return fi;
    return 0; 
}

FunctionExpr*
FunctionFactory::makeFunction(const Common::String& funcName,
                              const FunctionArgExpr* alist,
                              const GroveLib::NodeWithNamespace* nsres) const
{
    static const char ext_uri[] = 
        "http://www.syntext.com/Extensions/XSLT-1.0"; 
    GroveLib::QualifiedName qname;
    qname.parse(funcName);
    String uri = qname.prefix().isEmpty() ? GroveLib::XmlNs::defaultNs() 
        : nsres->getXmlNsByPrefix(qname.prefix());
    if (uri.isNull() && qname.prefix() == "xse")
        uri = ext_uri;
    const FunctionMakerInfo* fi = findFunction(qname.localName(), uri);
    if (fi) {
        OwnerPtr<FunctionExpr> fe((*fi->maker)(funcName, alist));
        if (fe->args().size() < fi->minArgs || fe->args().size() > fi->maxArgs)
            throw Xpath::Exception(XpathMessages::evalNArgs, fe->name());
        return fe.release();
    }
    if (uri.isEmpty() || uri == ext_uri)
        throw Xpath::Exception(XpathMessages::parserUknFunc, funcName);
    DDBG << "XPath:: makeExternalFunction: " << funcName << std::endl;
    return new ExternalFunctionImpl(qname.localName(), uri, alist);
}

///////////////////////////////////////////

ConstValueImplPtr
ExternalFunctionImpl::eval(const NodeSetItem& context, ExprInst& ei) const
{
    const ExternalFunctionBase* efb =
        ei.exprContext().getExternalFunction(name(), ns_);
    if (0 == efb)
        throw Xpath::Exception(XpathMessages::evalNoExtFunc,
              ns_.isEmpty() ? ns_ : "{" + ns_ + "}",
              name(), ei.exprContext().contextString());
    FuncArgEvaluator fargs(context, ei, this);
    ConstValuePtrList computed_args;
    computed_args.reserve(fargs.argNum());
    for (uint i = 0; i < fargs.argNum(); ++i)
        computed_args.push_back(fargs.arg(i).pointer());
    RefCntPtr<ValueHolder> result = efb->eval(computed_args);
    ei.appendSubInst(new ValueHolderWatcher(result.pointer()));
    return static_cast<const ValueImpl*>(result->value().pointer());
}

///////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Boolean>::eval(const NodeSetItem& context,
                                            ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return fargs.arg(0)->to(Value::BOOLEAN, &ei);
}

/////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Ceiling>::eval(const NodeSetItem& context,
                                            ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return new NumericValue(ceil(fargs.arg(0)->
        to(Value::NUMERIC, &ei)->getDouble()));
}

/////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Concat>::eval(const NodeSetItem& context,
                                           ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    if (fargs.argNum() < 2)
        throw Xpath::Exception(XpathMessages::evalConcat,
              ei.exprContext().contextString());
    String concat;
    for (size_t c = 0; c < fargs.argNum(); c++)
        concat += fargs.arg(c)->to(Value::STRING, &ei)->getString();
    return new StringValue(concat);
}

//////////////////////////////////////////////

template<> ConstValueImplPtr
OptimizedFunctionExprImpl<FunctionExpr::Contains>::eval(const NodeSetItem& nsi,
                                                      ExprInst& ei) const
{
    FuncArgEvaluator fargs(nsi, ei, this);
    return BooleanValue::instance(fargs.arg(0)->to(Value::STRING, &ei)->
        getString().contains(fargs.arg(1)->
            to(Value::STRING, &ei)->getString()));
}

template<> ExprImplPtr 
OptimizedFunctionExprImpl<FunctionExpr::Contains>::optimize(int opt)
{
    (void) FunctionExpr::optimize(opt);
    if (!(opt & FunctionFactory::OPT_DITA) || args_.size() != 2)
        return this;
    if (args_[0]->type() != AXIS || args_[1]->type() != CONST)
        return this;
    ConstValueImplPtr value = static_cast<const ConstExpr*>
        (args_[1].pointer())->value();
    const AxisExpr* ae = static_cast<const AxisExpr*>(args_[0].pointer());
    if (ae->name() != AxisExpr::ATTRIBUTE || value->type() != Value::STRING ||
        ae->nodeTestExpr()->name() != NodeTestExpr::NAME_NODE_TEST)
            return this;
    const NameNodeTestExpr* nnte = 
        static_cast<const NameNodeTestExpr*>(ae->nodeTestExpr());
    if (!nnte->uri().isEmpty() || nnte->localName() != NOTR("class"))
        return this;
    FunctionArgExpr fexpr(new ConstExpr(value), 0);
    return new FunctionExprImpl<FunctionExpr::DitaContains>
        (NOTR("ditaContains"), &fexpr);
}

template <> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::DitaContains>::eval(const NodeSetItem& nsi,
                                                 ExprInst&) const
{
    if (args().size() != 1 || args()[0]->type() != CONST)
        throw Xpath::Exception(XpathMessages::evalNArgs, name());
    DitaContainsTestExpr test_expr(
        static_cast<const ConstExpr*>(&*args()[0])->value()->getString());
    return BooleanValue::instance(test_expr.nodeTest(nsi.node()));        
}

//////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Count>::eval(const NodeSetItem& context,
                                          ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return new NumericValue(fargs.arg(0)->
        to(Value::NODESET)->getNodeSet().contextSize());
}

/////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::False>::eval(const NodeSetItem&,
                                          ExprInst&) const
{
    return BooleanValue::instance(false);
}

//////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Floor>::eval(const NodeSetItem& context,
                                          ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return new NumericValue(floor(fargs.arg(0)->
        to(Value::NUMERIC, &ei)->getDouble()));
}

///////////////////////////////////////////////

static void id_collectNodes(ExprInst& ei,
                            const GroveLib::Node* contextNode,
                            const String& string,
                            NodeSet& nodeSet)
{
    GroveLib::IdManager* idm = contextNode->grove()->idManager();
    if (0 == idm)
        return;
    GroveLib::IdTableEntry* ide;
    for (StringTokenizer st(string); st; ) {
        ide = idm->findIdrefEntry(st.next());
        if (0 == ide)
            continue;
        if (ide->idNode())
            nodeSet += ide->element();
        ei.watchFor(ide);
    }
}

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Id>::eval(const NodeSetItem& context,
                                       ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    GroveLib::Node* context_node = context.node();
    if (fargs.argNum() > 1) {
        const NodeSet& nset = fargs.arg(1)->getNodeSet();
        if (nset.contextSize())
            context_node = nset.first()->node();
    }
    NodeSet result_set;
    ConstValueImplPtr arg0 = fargs.arg(0);
    if (Value::STRING == arg0->type())
        id_collectNodes(ei, context_node,
            arg0->getString(), result_set);
    else {
        const NodeSet& node_set = arg0->getNodeSet();
        for (const NodeSetItem* n = node_set.first(); n; n = n->next())
            id_collectNodes(ei, context_node,
                NodeSetValue::elementToString(n->node(), &ei), result_set);
    }
    return new NodeSetValue(result_set);
}

////////////////////////////////////////////

ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Lang>::eval(const NodeSetItem& context,
                                         ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    GroveLib::Node* node = context.node();
    const GroveLib::Attr* attr = 0;
    while (node) {
        ei.registerVisitor(node,
            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED|
            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_ADDED|
            GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_REMOVED);
        if (node->nodeType() == GroveLib::Node::ELEMENT_NODE)
            attr = (static_cast<const GroveLib::Element*>(node))->
                attrs().getAttribute("xml:lang");
        if (attr)
            break;
        else
            node = parentNode(node);
    }
    if (attr) {
        String value = attr->value().lower();
        String lang  = fargs.arg(0)->
            to(Value::STRING, &ei)->getString().lower();
        return BooleanValue::instance((value == lang) ||
            (value.contains(lang + '-', false)));
    }
    else
        return BooleanValue::instance(false);
}

/////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Last>::eval(const NodeSetItem& context,
                                         ExprInst&) const
{
    return new NumericValue(context.size());
}

/////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Level>::eval(const NodeSetItem& context,
                                          ExprInst&) const
{
    const GroveLib::Node* n = parentNode(context.node());
    uint level = 0;
    while (n) {
        ++level;
        n = n->parent();
    }
    return new NumericValue(level);
}

//////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::LocalName>::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    const GroveLib::Node* node = 0;
    if (fargs.argNum() > 0) {
        const NodeSetItem* n =
            fargs.arg(0)->to(Value::NODESET)->getNodeSet().first();
        if (n)
            node = n->node();
    } else
        node = context.node();
    if (node && ((node->nodeType() == GroveLib::Node::ELEMENT_NODE) ||
        (node->nodeType() == GroveLib::Node::ATTRIBUTE_NODE)))
         return new StringValue
            (static_cast<const GroveLib::NodeWithNamespace*>(node)->
                localName());
    return StringValue::empty();
}

///////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Name>::eval(const NodeSetItem& context,
                                         ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    const GroveLib::Node* node = 0;
    if (fargs.argNum() > 0) {
        const NodeSetItem* n =
            fargs.arg(0)->to(Value::NODESET)->getNodeSet().first();
        if (n)
            node = n->node();
    } else
        node = context.node();
    if (node && ((node->nodeType() == GroveLib::Node::ELEMENT_NODE) ||
        (node->nodeType() == GroveLib::Node::ATTRIBUTE_NODE))) 
        return new StringValue(node->nodeName());
    return StringValue::empty();
}

///////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::NormalizeSpace>::eval(const NodeSetItem& context,
                                                   ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String str;
    if (fargs.argNum() == 0) {
        NodeSet node_set;
        node_set += context.node();
        str = NodeSetValue(node_set).to(Value::STRING, &ei)->getString();
    } else
        str = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    return new StringValue(str.simplifyWhiteSpace());
}

////////////////////////////////////////////////

template<> ConstValueImplPtr
OptimizedFunctionExprImpl<FunctionExpr::Not>::eval(const NodeSetItem& context,
                                        ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return BooleanValue::instance
        (!fargs.arg(0)->to(Value::BOOLEAN, &ei)->getBool());
}

template<> ExprImplPtr 
OptimizedFunctionExprImpl<FunctionExpr::Not>::optimize(int opt)
{
    (void) FunctionExpr::optimize(opt);
    if (!args_.size())
        return this;
    if (args_[0]->type() == AXIS && parent() && parent()->type() == PREDICATE)
        return static_cast<AxisExpr*>(args_[0].pointer())->joinNot().pointer();
    return this;
}

///////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::NsUri>::eval(const NodeSetItem& context,
                                          ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    const GroveLib::Node* node = 0;
    if (fargs.argNum() > 0) {
        const NodeSetItem* n =
            fargs.arg(0)->to(Value::NODESET)->getNodeSet().first();
        if (n)
            node = n->node();
    } else
        node = context.node();
    if (node && ((node->nodeType() == GroveLib::Node::ELEMENT_NODE) ||
        (node->nodeType() == GroveLib::Node::ATTRIBUTE_NODE))) {
            const GroveLib::NodeWithNamespace* ns_node =
                static_cast<const GroveLib::NodeWithNamespace*>(node);
        return new StringValue(ns_node->xmlNsUri());
    }
    return StringValue::empty();
}

////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Number>::eval(const NodeSetItem& context,
                                           ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    double num;
    if (fargs.argNum() == 0) {
        NodeSet node_set;
        node_set += context.node();
        num = NodeSetValue(node_set).to(Value::NUMERIC, &ei)->getDouble();
    } else
        num = fargs.arg(0)->to(Value::NUMERIC, &ei)->getDouble();
    return new NumericValue(num);
}

/////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Position>::eval(const NodeSetItem& context,
                                             ExprInst&) const
{
    return new NumericValue(context.pos());
}

////////////////////////////////////////////////////

double round(double number)
{
    return floor(number + 0.5);
}

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Round>::eval(const NodeSetItem& context,
                                          ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    ConstNumericValuePtr value =
        static_cast<const NumericValue*>
            (fargs.arg(0)->to(Value::NUMERIC, &ei).pointer());
    if (value->isNan() || value->isInfinity())
        return value.pointer();
    return new NumericValue(round(value->getDouble()));
}

/////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::StartWith>::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    return BooleanValue::instance
        (0 == fargs.arg(0)->to(Value::STRING, &ei)->
            getString().find(fargs.arg(1)->
                to(Value::STRING, &ei)->getString()));
}

/////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::StringFunc>::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String str;
    if (fargs.argNum() == 0) {
        NodeSet node_set;
        node_set += context.node();
        str = NodeSetValue(node_set).to(Value::STRING, &ei)->getString();
    } else
        str = fargs.arg(0)->to(Value::STRING, &ei)->getString();
   return new StringValue(str);
}

/////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::StringLength>::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String str;
    if (fargs.argNum() == 0) {
        NodeSet node_set;
        node_set += context.node();
        str = NodeSetValue(node_set).to(Value::STRING, &ei)->getString();
    } else
        str = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    return new NumericValue(str.length());
}

////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::SubstringAfter>::eval(const NodeSetItem& context,
                                                   ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String first  = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    String second = fargs.arg(1)->to(Value::STRING, &ei)->getString();
    int pos = first.find(second);
    if (0 <= pos)
        return new StringValue(first.mid(pos + second.length()));
    return StringValue::empty();
}

////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::SubstringBefore>::eval(const NodeSetItem& context,
                                                    ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String first  = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    String second = fargs.arg(1)->to(Value::STRING, &ei)->getString();
    int pos = first.find(second);
    if (0 <= pos)
        return new StringValue(first.left(pos));
    return StringValue::empty();
}

////////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Substring>::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String str = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    ConstNumericValuePtr from_value =
        static_cast<const NumericValue*>(fargs.arg(1)->
            to(Value::NUMERIC, &ei).pointer());
    ConstNumericValuePtr count_value = (3 > fargs.argNum()) ? 0 :
        static_cast<const NumericValue*>(fargs.arg(2)->
            to(Value::NUMERIC, &ei).pointer());

    if (from_value->isNan() || from_value->isInfinity() ||
        (!count_value.isNull() && (count_value->isNan() ||
        count_value->isNegInfinity())))
            return StringValue::empty();
    int from = int(round(from_value->getDouble()) - 1);
    int correction = 0;
    if (from < 0) {
        correction = -from;
        from = 0;
    }
    if (count_value.isNull() || count_value->isPosInfinity())
        return new StringValue(str.mid(from));
    double count = round(count_value->getDouble()) - correction;
    return new StringValue(str.mid(from, int(count)));
}

////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Sum>::eval(const NodeSetItem& context,
                                        ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    const NodeSet& node_set = fargs.arg(0)->to(Value::NODESET)->getNodeSet();
    const NodeSetItem* n = node_set.first();
    double sum = 0;
    while (n) {
        NodeSet n_set;
        n_set += n->node();
        NodeSetValue value(n_set);
        sum += value.to(Value::NUMERIC, &ei)->getDouble();
        n = n->next();
    }
    return new NumericValue(sum);
}

/////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Translate>::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    FuncArgEvaluator fargs(context, ei, this);
    String source = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    String dict_from = fargs.arg(1)->to(Value::STRING, &ei)->getString();
    String dict_to = fargs.arg(2)->to(Value::STRING, &ei)->getString();
    int len = dict_to.length();

    String result;
    for (size_t i = 0; i < source.length(); i++) {
        int pos = dict_from.find(source.at(i));
        if (-1 != pos) {
            if (pos < len)
                result += dict_to[pos];
        }
        else
            result += source.at(i);
    }
    return new StringValue(result);
}

/////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::True>::eval(const NodeSetItem&,
                                         ExprInst&) const
{
    return BooleanValue::instance(true);
}

/////////////////////////////////////////////////

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::UnparsedEntityUri>::eval(const NodeSetItem& ctx,
                                                      ExprInst& ei) const
{
    FuncArgEvaluator fargs(ctx, ei, this);
    String ename = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    GroveLib::Node* node = ctx.node();
    const GroveLib::EntityDecl* entity =
        node->grove()->entityDecls()->lookupDecl(ename);
    if (entity) {
        const GroveLib::ExternalEntityDecl* exd =
            entity->asConstExternalEntityDecl();
        if (exd)
            return new StringValue(exd->sysid());
    }
    return StringValue::empty();
}

/////////////////////////////////////////////////

class RssNotifier : public ExprSubInst, public GroveLib::NodeVisitor {
public:
    XPATH_OALLOC(RssNotifier);

    virtual int     type() const { return RSSWATCHER_SUBINST; }
    virtual void    exprChanged() { deregisterFromAllNodes(); }

    virtual void    childInserted(const GroveLib::Node*) {}
    virtual void    childRemoved(const GroveLib::Node*,
                                 const GroveLib::Node*) {}
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}
    virtual void    textChanged(const GroveLib::Text*) {}
    virtual void    genericNotify(const GroveLib::Node*, void* t)
    {
        if (GroveLib::RedlineData::redlineNotifyId() == t)
            exprInst().setModified();
    }
};

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::NodeSetFunc>::eval(const NodeSetItem& ctx,
                                                ExprInst& ei) const
{
    FuncArgEvaluator fargs(ctx, ei, this);
    NodeSet node_set;
    fargs.arg(0)->to(Value::NODESET)->getNodeSet().copyTo(node_set);
    return new NodeSetValue(node_set);
}

template<> ConstValueImplPtr
FunctionExprImpl<FunctionExpr::Redline>::eval(const NodeSetItem& ctx,
                                            ExprInst& ei) const
{
    FuncArgEvaluator fargs(ctx, ei, this);
    const GroveLib::Node* node = 0;
    String what = fargs.arg(0)->to(Value::STRING, &ei)->getString();
    if (fargs.argNum() > 1) {
        const NodeSetItem* n =
            fargs.arg(1)->to(Value::NODESET)->getNodeSet().first();
        if (n)
            node = n->node();
    } else
        node = ctx.node();
    if (0 == node)
        return StringValue::empty();
    GroveLib::RedlineSectionStart* rss = GroveLib::get_redline_section(node);
    if (0 == rss)
        return StringValue::empty();
    ExprSubInst* subinst = ei.findSubInst(ExprSubInst::RSSWATCHER_SUBINST);
    RssNotifier* rssn;
    if (subinst) 
        rssn = static_cast<RssNotifier*>(subinst);
    else {
        rssn = new RssNotifier;
        ei.appendSubInst(rssn);
    }
    rss->registerNodeVisitor(rssn, GroveLib::GroveVisitor::NOTIFY_GENERIC);
    String result;
    const GroveLib::RedlineData& rd = *rss->redlineData();
    if (NOTR("type") == what) {
        if (rd.redlineMask() & (1 << GroveLib::REDLINE_UNDERLINE))
            return new StringValue(NOTR("inserted"));
        else if (rd.redlineMask() & (1 << GroveLib::REDLINE_STRIKE_THRU))
            return new StringValue(NOTR("removed"));
        else
            return new StringValue(NOTR("marked"));
    } else if (NOTR("annotation") == what) 
        return new StringValue(rd.annotation());
    return StringValue::empty();
}

/////////////////////////////////////////////////

ConstValueImplPtr FuncArgEvaluator::arg(uint i)
{
    args_[i] = funcExpr_->args()[i]->eval(context_, exprInst_);
    return args_[i];
}

} // namespace Xpath

