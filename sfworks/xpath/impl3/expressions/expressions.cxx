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
#include "common/String.h"
#include "xpath/impl3/debug.h"
#include "xpath/ExprContext.h"
#include "xpath/VarBindings.h"
#include "xpath/impl3/ExprInst.h"
#include "xpath/impl3/expressions/AxisExpr.h"
#include "xpath/impl3/expressions/BinaryExpr.h"
#include "xpath/impl3/expressions/ConstExpr.h"
#include "xpath/impl3/expressions/FunctionExpr.h"
#include "xpath/impl3/expressions/NodeTestExpr.h"
#include "xpath/impl3/expressions/PredicateExpr.h"
#include "xpath/impl3/expressions/SlashExpr.h"
#include "xpath/impl3/expressions/UnaryExpr.h"
#include "xpath/impl3/expressions/VarExpr.h"
#include "xpath/NodeSet.h"
#include "grove/Nodes.h"
#include "grove/ChoiceNode.h"
#include "grove/XmlNs.h"

using namespace Common;

namespace Xpath {

int ExprImpl::contextDependency() const
{
    if (dep_type_ == UNDEF_DEP)
        dep_type_ = dep_type();
    return dep_type_;
}

bool ExprImpl::isNumeric() const
{
    return false;
}

static const char* axis_names [] = {
        "ancestor",
        "ancestor-or-self",
        "attribute",
        "child",
        "descendant",
        "descendant-or-self",
        "following",
        "following-sibling",
        "namespace",
        "parent",
        "preceding",
        "preceding-sibling",
        "root",
        "self",
        "LASTNAME"
    };

AxisExpr::AxisExpr(const NodeTestExprPtr& nodeTestExpr)
    : nodeTestExpr_(nodeTestExpr)
{
    if (!nodeTestExpr_.isNull())
        nodeTestExpr_->setParent(this);
}

AxisExpr::Name AxisExpr::toName(const String& strName)
{
    for (int n = FIRSTNAME; n < LASTNAME; n++)
        if (strName == axis_names[n])
            return AxisExpr::Name(n);
    return LASTNAME;
}

ExprImplPtr AxisExpr::make(const String& name, const NodeTestExprPtr& nt)
{
    Name n = toName(name);
    if (n == LASTNAME)
        return 0;
    return make(n, nt);
}

ExprImplPtr AxisExpr::make(Name n, const NodeTestExprPtr& nt)
{
    switch (n) {
        case ANCESTOR:
            return new AxisAncestorExpr(nt);
        case ANCESTOR_OR_SELF:
            return new AxisAosExpr(nt);
        case ATTRIBUTE:
            return new AxisAttributeExpr(nt);
        case CHILD:
            return new AxisChildExpr(nt);
        case DESCENDANT:
            return new AxisDescendantExpr(nt);
        case DESCENDANT_OR_SELF:
            return new AxisDosExpr(nt);
        case FOLLOWING:
            return new AxisFollowingExpr(nt);
        case FOLLOWING_SIBLING:
            return new AxisFsExpr(nt);
        case PARENT:
            return new AxisParentExpr(nt);
        case PRECEDING:
            return new AxisPrecedingExpr(nt);
        case PRECEDING_SIBLING:
            return new AxisPsExpr(nt);
        case ROOT:
            return new AxisRootExpr(nt);
        case SELF:
            return new AxisSelfExpr(nt);
        default:
            return 0;
    }
}

void AxisExpr::makeNodeTestChain(ExprImpl*& prev)
{
    if (nodeTestExpr_)
        nodeTestExpr_->makeNodeTestChain(prev);
    else
        prev = 0;   // no need to check - one of node tests always true
}

void AxisExpr::check_same_axis(const AxisExprPtr& other) const
{
    if (name() != other->name())
        throw Xpath::Exception(XpathMessages::invExprJoin);
}

AxisExprPtr AxisExpr::joinOr(const AxisExprPtr& other)
{
    check_same_axis(other);
    if (!other->nodeTestExpr())
        return this;
    if (nodeTestExpr_.isNull())
        nodeTestExpr_ = other->nodeTestExpr();
    else
        nodeTestExpr_ = new OrNodeTestExpr(nodeTestExpr_,
            other->nodeTestExpr());
    return this;
}

AxisExprPtr AxisExpr::joinAnd(const AxisExprPtr& other)
{
    check_same_axis(other);
    if (!other->nodeTestExpr())
        return this;
    if (nodeTestExpr_.isNull())
        nodeTestExpr_ = other->nodeTestExpr();
    else
        nodeTestExpr_ = new AndNodeTestExpr(nodeTestExpr_,
            other->nodeTestExpr());
    return this;
}

AxisExprPtr AxisExpr::joinNot()
{
    if (!nodeTestExpr_.isNull())
        nodeTestExpr_ = new NotNodeTestExpr(nodeTestExpr_);
    return this;
}

double AxisExpr::priority() const
{
    if (nodeTestExpr_.isNull())
        return 0.5;  // XSLT[5.5/5]
    return nodeTestExpr()->priority();
}

bool AxisExpr::operator==(const ExprImpl& other) const
{
    if (other.type() != type())
        return false;
    const AxisExpr& axis = static_cast<const AxisExpr&>(other);
    if (axis.name() != name())
        return false;
    if (nodeTestExpr_.isNull()) {
        if (axis.nodeTestExpr_.isNull())
            return true;
        return false;
    }
    if (axis.nodeTestExpr_.isNull())
        return false;
    return *nodeTestExpr_ == *axis.nodeTestExpr_;
}

void AxisExpr::dump() const
{
    DDINDENT;
    DDBG << "AxisExpr: name:" << axis_names[name()] << std::endl;
    if (!nodeTestExpr_.isNull())
        nodeTestExpr_->dump();
}

//////////////////////////////////////////////////

static const char* binary_names [] = {
    "add",
    "and",
    "bar",
    "div",
    "eq",
    "gt_eq",
    "gt",
    "lt_eq",
    "lt",
    "mod",
    "mul",
    "not_eq",
    "or",
    "sub",
    "LASTNAME"
};

BinaryExpr::BinaryExpr(const ExprImplPtr& left, const ExprImplPtr& right)
    : left_(left),
      right_(right)
{
    if (left_.isNull())
        throw Xpath::Exception(XpathMessages::invExprLeftOpBin);
    if (right_.isNull())
        throw Xpath::Exception(XpathMessages::invExprRightOpBin);
    left_->setParent(this);
    right_->setParent(this);
}

bool BinaryExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const BinaryExpr& binary = static_cast<const BinaryExpr&>(other);
        return ((binary.left() == left()) &&
                (binary.right() == right()));
    }
    return false;
}

void BinaryExpr::makeNodeTestChain(ExprImpl*& prev)
{
    left_->makeNodeTestChain(prev);
    right_->makeNodeTestChain(prev);
}

int BinaryExpr::dep_type() const
{
    return left_->contextDependency() | right_->contextDependency();
}

bool BinaryExpr::isNumeric() const
{
    switch (name()) {
        case ADD:
        case DIV:
        case MOD:
        case MUL:
        case SUB:
            return true;
        default:;
    }
    return false;
}

ExprImplPtr BinaryExpr::optimize_binary(int opt)
{
    ExprImplPtr e = left_->optimize(opt);
    if (e != left_)
        left_ = e;
    e = right_->optimize(opt);
    if (e != right_)
        right_ = e;
    return this;
}

void BinaryExpr::dump() const
{
    DDINDENT;
    DDBG << "BinaryExpr: operation=" << binary_names[name()] << std::endl;
    DDBG << "left operand:" << std::endl;
    left_->dump();
    DDBG << "right operand:" << std::endl;
    right_->dump();
}

//////////////////////////////////////////////

ConstExpr::ConstExpr(ConstValueImplPtr value)
    : value_(value)
{
}

bool ConstExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const ConstExpr& cexpr = static_cast<const ConstExpr&>(other);
        return (cexpr.value()->eq(*value_, 0));
    }
    return false;
}

bool ConstExpr::isNumeric() const
{
    return value_->type() == Value::NUMERIC;
}

ConstValueImplPtr ConstExpr::eval(const NodeSetItem&, ExprInst&) const
{
    return value_;
}

void ConstExpr::dump() const
{
    DDINDENT;
    DDBG << "ConstExpr: refers to" << std::endl;
    value_->dump();
}

////////////////////////////////////////////////

PredicateExpr::PredicateExpr(const ExprImplPtr& left, const ExprImplPtr& pred)
    : left_(left),
      predicate_(pred)
{
    if (left_.isNull())
        throw Xpath::Exception(XpathMessages::invExprLeftOpPred);
    if (predicate_.isNull())
        throw Xpath::Exception(XpathMessages::invExprRightOpPred);
    left_->setParent(this);
    predicate_->setParent(this);
}

void PredicateExpr::makeNodeTestChain(ExprImpl*& prev)
{
    left_->makeNodeTestChain(prev);
    predicate_->makeNodeTestChain(prev);
}

bool PredicateExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const PredicateExpr& filter = static_cast<const PredicateExpr&>(other);
        return ((filter.left() == left()) &&
                (filter.predicate() == predicate()));
    }
    return false;
}

int PredicateExpr::dep_type() const
{
    int dt = left_->contextDependency();
    dt |= predicate_->contextDependency();
    if (static_cast<const ExprImpl*>(predicate_.pointer())->isNumeric())
        dt |= POS_DEP;
    return dt;
}

static inline bool check_star_nt(const AxisExpr* aexpr)
{
    return aexpr->nodeTestExpr()->name() == NodeTestExpr::NAME_NODE_TEST &&
        static_cast<const NameNodeTestExpr*>
            (aexpr->nodeTestExpr())->localName() == "*";
}

ExprImplPtr PredicateExpr::optimize(int opt)
{
    ExprImplPtr e = left_->optimize(opt);
    if (e != left_)
        left_ = e;
    e = predicate_->optimize(opt);
    if (e != predicate_)
        predicate_ = e;
    if (left_->type() != AXIS)
        return this;
    AxisExpr* aexpr = static_cast<AxisExpr*>(left_.pointer());
    if (predicate_->type() == AXIS) {
        AxisExpr* pred = static_cast<AxisExpr*>(predicate_.pointer());
        if (aexpr->name() != AxisExpr::CHILD)
            return this;
        if (pred->name() != AxisExpr::SELF || !aexpr->nodeTestExpr())
            return this;
        // *[self::foo]
        if (check_star_nt(aexpr))
            return new AxisChildExpr(pred->nodeTestExpr());
        return new AxisChildExpr(new AndNodeTestExpr(aexpr->nodeTestExpr(),
            pred->nodeTestExpr()));
    } else if (predicate_->type() == FUNCTION) {
        const FunctionExpr* fe = static_cast<const FunctionExpr*>(&*predicate_);
        if (fe->functionId() != FunctionExpr::DitaContains || 
            !aexpr->nodeTestExpr() || !check_star_nt(aexpr)) 
             return this;
        return AxisExpr::make(aexpr->name(), new DitaContainsTestExpr(fe));
    }   
    return this;
}

void PredicateExpr::dump() const
{
    DDINDENT;
    DDBG << "PredicateExpr:" << std::endl;
    DDBG << "left operand:" << std::endl;
    left_->dump();
    DDBG << "predicate:" << std::endl;
    predicate_->dump();
}

/////////////////////////////////////////////////

FunctionExpr::FunctionExpr(const Common::String& fname, 
                           const FunctionArgExpr* args)
    : funcName_(fname)
{
    if (args)
        args->linearize(args_);
}

bool FunctionExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const FunctionExpr& func = static_cast<const FunctionExpr&>(other);
        if (funcName_ != func.funcName_)
            return false;
        if (func.args().size() != args_.size())
            return false;
        for (size_t i = 0; i < args_.size(); i++)
            if (func.args()[i] != args_[i])
                return false;
        return true;
    }
    return false;
}

void FunctionExpr::makeNodeTestChain(ExprImpl*& prev)
{
    for (uint i = 0; i < args_.size(); ++i)
        args_[i]->makeNodeTestChain(prev);
}

int FunctionExpr::dep_type() const
{
    int dt = NO_DEP;
    for (uint i = 0; i < args_.size(); ++i)
        dt |= args_[i]->contextDependency();
    if (funcName_ == "position")
        dt |= POS_DEP;
    else if (funcName_ == "last")
        dt |= SIZE_DEP;
    return dt;
}

bool FunctionExpr::isNumeric() const
{
    return funcName_ == "position" || funcName_ == "last"
        || funcName_ == "ceiling"  || funcName_ == "count"
        || funcName_ == "floor"    || funcName_ == "number"
        || funcName_ == "round"    || funcName_ == "string-length";
}

void FunctionExpr::dump() const
{

    DDINDENT;
    DDBG << "FunctionExpr: " << funcName_ << std::endl;
    for (size_t i = 0; i < args_.size(); i++) {
        DDBG << "argument N" << i << ":" << std::endl;
        args_[i]->dump();
    }
}

///////////////////////////////////////////

void NodeTestExpr::makeNodeTestChain(ExprImpl*& prev)
{
    if (0 == prev)
        return;
    prev->nextNodeTest_ = this;
    prev = this;
}

bool NodeTestExpr::operator==(const ExprImpl& other) const
{
    if (other.type() != type())
        return false;
    if (static_cast<const NodeTestExpr&>(other).name() != name())
        return false;
    return true;
}

/////////////////////////////////////////////

bool LangNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return n->nodeType() == GroveLib::Node::ATTRIBUTE_NODE
        && n->nodeName() == "xml:lang";
}

double LangNodeTestExpr::priority() const
{
    return 0;           // XSLT[5.5/2]
}

void LangNodeTestExpr::dump() const
{

    DDINDENT;
    DDBG << "LangNodeTestExpr (xml:lang)";
}

/////////////////////////////////////////////

DitaContainsTestExpr::DitaContainsTestExpr(const String& test)
    : test_(test)
{
}

DitaContainsTestExpr::DitaContainsTestExpr(const FunctionExpr* fe)
{
    RT_ASSERT(fe->functionId() == FunctionExpr::DitaContains);
    test_ = static_cast<const ConstExpr*>(&*fe->args()[0])->
        value()->getString();
}

bool DitaContainsTestExpr::nodeTest(const GroveLib::Node* n) const
{
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
        return false;
    const GroveLib::Attr* a = 
        CONST_ELEMENT_CAST(n)->attrs().getAttribute(NOTR("class"));
    if (0 == a)
        return false;
    return a->value().contains(test_);
}

double DitaContainsTestExpr::priority() const
{
    return 0; // XSLT[5.5/2]
}

bool DitaContainsTestExpr::operator==(const ExprImpl& other) const
{
    if (NodeTestExpr::operator==(other)) {
        const DitaContainsTestExpr& test =
            static_cast<const DitaContainsTestExpr&>(other);
        return (test.test_ == test_);
    }
    return false;
}

void DitaContainsTestExpr::dump() const
{
    DDINDENT;
    DDBG << "DitaContainsTestExpr: " << test_ << std::endl;
}

/////////////////////////////////////////////

bool NameNodeTestExpr::operator==(const ExprImpl& other) const
{
    if (NodeTestExpr::operator==(other)) {
        const NameNodeTestExpr& test =
            static_cast<const NameNodeTestExpr&>(other);
        return (test.uri() == uri() &&
                test.localName() == localName());
    }
    return false;
}

static inline bool ns_match(const COMMON_NS::String& pLocalName,
                            const COMMON_NS::String& pUri,
                            const COMMON_NS::String& localName,
                            const COMMON_NS::String& uri)
{
    if ("*" == pLocalName) {
        if (pUri.isNull())
            return true;
        else
            return pUri == uri;
    }
    if (pLocalName != localName)
        return false;
    if (!pUri.isNull())
        return pUri == uri;
    return true;
}

NameNodeTestExpr::NameNodeTestExpr(const GroveLib::QualifiedName& qname,
                                   const GroveLib::NodeWithNamespace* resolver)
{
    setLocalName(qname.localName());
    if (!resolver || qname.prefix().isEmpty()) {
        setUri(COMMON_NS::String::null());
        return;
    }
    setUri(resolver->getXmlNsByPrefix(qname.prefix()));
    if (uri().isNull())
        throw Xpath::Exception(XpathMessages::invExprNs, qname.prefix());
}

bool NameNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    typedef GroveLib::NodeWithNamespace NsNode;
    GroveLib::Node::NodeType nt = n->nodeType();
    if (GroveLib::Node::CHOICE_NODE == nt) {
        const GroveLib::ChoiceNode* cn =
            static_cast<const GroveLib::ChoiceNode*>(n);
        const PropertyNode* pn = cn->elemList()->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            if (ns_match(localName(), uri(), pn->name(), pn->getString()))
                return true;
        }
        return false;
    }
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE &&
        n->nodeType() != GroveLib::Node::ATTRIBUTE_NODE)
            return false;   // not applicable
    const NsNode* node = static_cast<const NsNode*>(n);
    return ns_match(localName(), uri(), node->localName(), node->xmlNsUri());
}

double NameNodeTestExpr::priority() const
{
    if ("*" == localName())
        return -0.25;   // XSLT[5.5/3]
    return 0;           // XSLT[5.5/2]
}

void NameNodeTestExpr::dump() const
{

    DDINDENT;
    if (uri().isEmpty())
        DDBG << "NameNodeTestExpr: " << localName() << std::endl;
    else
        DDBG << "NameNodeTestExpr: " << uri() << ":"
             << localName() << std::endl;
}

/////////////////////////////////////////////////

bool NodeNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    // NodeTest is true for all nodes, except document element
    return n->nodeType() != GroveLib::Node::DOCUMENT_NODE;
}

double NodeNodeTestExpr::priority() const
{
    return -0.5;    // XSLT[5.5/4]
}

void NodeNodeTestExpr::dump() const
{
    DDINDENT;
    DDBG << "Node()NodeTestExpr" << std::endl;
}

/////////////////////////////////////////////////

bool TextNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return n->nodeType() == GroveLib::Node::TEXT_NODE;
}

double TextNodeTestExpr::priority() const
{
    return -0.5; // XSLT[5.5/4]
}

void TextNodeTestExpr::dump() const
{
    DDINDENT;
    DDBG << "Text()NodeTestExpr" << std::endl;
}

/////////////////////////////////////////////////

bool CommentNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return n->nodeType() == GroveLib::Node::COMMENT_NODE;
}

double CommentNodeTestExpr::priority() const
{
    return -0.5; // XSLT[5.5/4]
}

void CommentNodeTestExpr::dump() const
{
    DDINDENT;
    DDBG << "Comment()NodeTestExpr" << std::endl;
}

/////////////////////////////////////////////////

PiNodeTestExpr::PiNodeTestExpr(const String& piname)
    : pi_name_(piname) {}

NodeTestExpr* PiNodeTestExpr::make(const String& piname)
{
    if (piname.left(3) != "se:")
        return new PiNodeTestExpr(piname);
    String ext = piname.mid(3);
    if ("choice" == ext)
        return new NodeTypeTestExpr(GroveLib::Node::CHOICE_NODE);
    if ("element" == ext)
        return new NodeTypeTestExpr(GroveLib::Node::ELEMENT_NODE);
    if ("choice-simple" == ext || "choice-enum" == ext)
        return new TextChoiceTestExpr(ext);
    if ("enum-attr" == ext)
        return new EnumAttrTestExpr;
    return new PiNodeTestExpr(piname);
}

bool PiNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    if (n->nodeType() != GroveLib::Node::PI_NODE)
        return false;
    if (pi_name_.isNull())
        return true;
    String pn_name =
        static_cast<const GroveLib::ProcessingInstruction*>(n)->target();
    return pn_name == pi_name_;
}

double PiNodeTestExpr::priority() const
{
    if (pi_name_.isEmpty())
        return -0.5;    // XSLT[5.5/4]
    else
        return 0;       // XSLT[5.5/2]
}

void PiNodeTestExpr::dump() const
{
    DDINDENT;
    DDBG << "Pi()NodeTestExpr: <" << pi_name_ << ">" << std::endl;
}

/////////////////////////////////////////////////

NodeTypeTestExpr::NodeTypeTestExpr(GroveLib::Node::NodeType t)
    : type_(t) {}

bool NodeTypeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return n->nodeType() == type_;
}

double NodeTypeTestExpr::priority() const
{
    return -0.5;
}

bool NodeTypeTestExpr::operator==(const ExprImpl& other) const
{
    if (NodeTestExpr::operator==(other)) {
        const NodeTypeTestExpr& test =
            static_cast<const NodeTypeTestExpr&>(other);
        return (test.type_ == type_);
    }
    return false;
}

void NodeTypeTestExpr::dump() const
{
    DDINDENT;
    DDBG << "NodeTypeTestExpr: <" << int(type_) << ">\n";
}

/////////////////////////////////////////////////

TextChoiceTestExpr::TextChoiceTestExpr(const String& testFor)
{
    if (testFor == "choice-simple")
        compareWith_ = "#choice-simple";
    else
        compareWith_ = "#choice-enum";
}

bool TextChoiceTestExpr::operator==(const ExprImpl& other) const
{
    if (NodeTestExpr::operator==(other)) {
        const TextChoiceTestExpr& test =
            static_cast<const TextChoiceTestExpr&>(other);
        return (test.compareWith_ == compareWith_);
    }
    return false;
}


bool TextChoiceTestExpr::nodeTest(const GroveLib::Node* n) const
{
    if (n->nodeType() != GroveLib::Node::TEXT_NODE)
        return false;
    const String& node_name = n->nodeName();
    if (node_name[1] == 't') // text
        return false;
    return node_name == compareWith_;
}

double TextChoiceTestExpr::priority() const
{
    return -0.5;
}

void TextChoiceTestExpr::dump() const
{
    DDBG << "TextChoiceTestExpr: <" << compareWith_ << ">\n";
}

/////////////////////////////////////////////////

bool EnumAttrTestExpr::nodeTest(const GroveLib::Node* n) const
{
    if (n->nodeType() != GroveLib::Node::ATTRIBUTE_NODE)
        return false;
    return static_cast<const GroveLib::Attr*>(n)->isEnumerated();
}

double EnumAttrTestExpr::priority() const
{
    return -0.5;
}

void EnumAttrTestExpr::dump() const
{
    DDBG << "EnumAttrTestExpr\n";
}

/////////////////////////////////////////////////

bool OrNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return left_->nodeTest(n) || right_->nodeTest(n);
}

double OrNodeTestExpr::priority() const
{
    return 0;
}

void OrNodeTestExpr::dump() const
{
    DDBG << "OR-NODE-TEST:\n";
    DDBG << "left:\n";
    { DDINDENT; left_->dump(); }
    DDBG << "right:\n";
    { DDINDENT; right_->dump(); }
}
/////////////////////////////////////////////////

bool AndNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return left_->nodeTest(n) && right_->nodeTest(n);
}

double AndNodeTestExpr::priority() const
{
    return 0;
}

void AndNodeTestExpr::dump() const
{
    DDBG << "AND-NODE-TEST:\n";
    DDBG << "left:\n";
    { DDINDENT; left_->dump(); }
    DDBG << "right:\n";
    { DDINDENT; right_->dump(); }
}
/////////////////////////////////////////////////

bool NotNodeTestExpr::nodeTest(const GroveLib::Node* n) const
{
    return !left_->nodeTest(n);
}

double NotNodeTestExpr::priority() const
{
    return 0;
}

void NotNodeTestExpr::dump() const
{
    DDBG << "NOT-NODE-TEST:\n";
    { DDINDENT; left_->dump(); }
}

/////////////////////////////////////////////////

SlashExpr::SlashExpr(ExprImplPtr left, ExprImplPtr right)
    : left_(left),
      right_(right)
{
    if (left_.isNull())
        throw Xpath::Exception(XpathMessages::invExprLeftOpSlash);
    if (right_.isNull())
        throw Xpath::Exception(XpathMessages::invExprRightOpSlash);
}

bool SlashExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const SlashExpr& slash = static_cast<const SlashExpr&>(other);
        return ((slash.left() == left()) &&
                (slash.right() == right()));
    }
    return false;
}

void SlashExpr::makeNodeTestChain(ExprImpl*& prev)
{
    left_->makeNodeTestChain(prev);
    right_->makeNodeTestChain(prev);
}

void SlashExpr::dump() const
{
    DDINDENT;
    DDBG << "SlashExpr:" << std::endl;
    DDBG << "left operand:" << std::endl;
    left_->dump();
    DDBG << "right operand:" << std::endl;
    right_->dump();
}

//////////////////////////////////////////////////

UnaryExpr::UnaryExpr(const ExprImplPtr& left, Op op)
    : left_(left), op_(op)
{
    if (left_.isNull())
        throw Xpath::Exception(XpathMessages::invExprNoOp);
    left_->setParent(this);
}

bool UnaryExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const UnaryExpr& unary = static_cast<const UnaryExpr&>(other);
        return (unary.op_ == op_);
    }
    return false;
}

void UnaryExpr::makeNodeTestChain(ExprImpl*& prev)
{
    left_->makeNodeTestChain(prev);
}

int UnaryExpr::dep_type() const
{
    return left_->contextDependency();
}

bool UnaryExpr::isNumeric() const
{
    return true;
}

ExprImplPtr UnaryExpr::optimize_unary(int opt)
{
    ExprImplPtr e = left_->optimize(opt);
    if (e != left_)
        left_ = e;
    return this;
}

void UnaryExpr::dump() const
{
    DDINDENT;
    DDBG << "UnaryExpr: op<" << (int)op_ << ">" << std::endl;
    DDBG << "operand:" << std::endl;
    left_->dump();
}

////////////////////////////////////////////////////

static void normalizeNsPrefix(String& name, const GroveLib::Node* node)
{
    int pos = name.find(":");
    if (0 < pos) {
        String prefix = name.left(pos);
        if ((node->nodeType() == GroveLib::Node::ELEMENT_NODE) ||
            (node->nodeType() == GroveLib::Node::ATTRIBUTE_NODE)) {
            const GroveLib::NodeWithNamespace* temp =
                static_cast<const GroveLib::NodeWithNamespace*>(node);
            String uri = temp->getXmlNsByPrefix(prefix);
            if ((! uri.isEmpty() ) && (! uri.isNull() ))
                name = "{" + uri + "}" + name;
        }
    }
}

VarExpr::VarExpr(const String& varName)
    : varName_(varName)
{
}

bool VarExpr::operator==(const ExprImpl& other) const
{
    if (other.type() == type()) {
        const VarExpr& var = static_cast<const VarExpr&>(other);
        return (var.varName() == varName());
    }
    return false;
}

void VarExpr::dump() const
{
    DDINDENT;
    DDBG << "VarExpr: varName:" << varName_ << std::endl;
}

ConstValueImplPtr VarExpr::eval(const NodeSetItem& nsi, ExprInst& ei) const
{
    String name = varName_;
    normalizeNsPrefix(name, nsi.node());
    ValueHolder* v = ei.exprContext().getVariable(name);
    if (0 == v) {
        throw Xpath::Exception(XpathMessages::instVar, name,
                               ei.exprContext().contextString());
    }
    ei.watchFor(v);
    return static_cast<const ValueImpl*>(v->value().pointer());
}

////////////////////////////////////////////////////////////

void FunctionArgExpr::linearize(COMMON_NS::Vector<ExprImplPtr>& argv) const
{
    const FunctionArgExpr* curarg = this;
    do {
        argv.push_back(curarg->arg_);
        curarg = curarg->next_.pointer();
    } while(curarg);
}

////////////////////////////////////////////////////////////


static ExprImplPtr
join_or_axis(ExprImpl* self, ExprImplPtr& left, ExprImplPtr& right)
{
    if (left->type() == Expr::AXIS && right->type() == Expr::AXIS) {
        AxisExpr* a1 = static_cast<AxisExpr*>(left.pointer());
        AxisExpr* a2 = static_cast<AxisExpr*>(right.pointer());
        if (a1->name() == a2->name())
            return a1->joinOr(a2).pointer();
    }
    return self;
}

template<> ExprImplPtr BinaryBarExpr::optimize(int opt)
{
    (void) optimize_binary(opt);
    return join_or_axis(this, left_, right_);
}

template<> ExprImplPtr BinaryOrExpr::optimize(int opt)
{
    (void) optimize_binary(opt);
    return join_or_axis(this, left_, right_);
}

template<> ExprImplPtr BinaryAndExpr::optimize(int opt)
{
    (void) optimize_binary(opt);
    if (left_->type() == AXIS && right_->type() == AXIS) {
        AxisExpr* a1 = static_cast<AxisExpr*>(left_.pointer());
        AxisExpr* a2 = static_cast<AxisExpr*>(right_.pointer());
        if (a1->name() == a2->name())
            return a1->joinAnd(a2).pointer();
    }
    return this;
}

///////////////////////////////////////////////////////////////

ExprImplPtr FunctionExpr::optimize(int opt)
{
    for (uint i = 0; i < args_.size(); ++i) {
        ExprImplPtr e = args_[i]->optimize(opt);
        if (e != args_[i])
            args_[i] = e;
    }
    return this;
}

} // namespace Xpath

