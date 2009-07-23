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
#include "csl/csl_defs.h"
#include "csl/SubInstance.h"
#include "csl/Instance.h"
#include "csl/RootInstance.h"
#include "csl/Instructions.h"
#include "xpath/Expr.h"
#include "xpath/ValueHolder.h"
#include "xpath/Value.h"
#include "xpath/NodeSet.h"
#include "xslt/NumberClient.h"
#include "csl/csl_debug.h"

using namespace Common;

namespace Csl {
    
void SubInstance::setModified()
{
    if (!isModified_) {
        isModified_ = true;
        parentInst()->subinstChanged();
    }
}

Instance* SubInstance::parentInst() const
{
    return parent() ? 
        XLIST_CONTAINING_PARENT_CLASS(Instance, subInstances_) : 0;
}

//////////////////////////////////////////////////////////////////

class TextInstance : public SubInstance {
public:
    CSL_OALLOC(TextInst);
    TextInstance(const Text* text, const Instance*)
    {
        result_ = text->text();
    }
    virtual void doUpdate() {}
    virtual void dump() const
    {
        DDBG << "TextInstance(" << this << "), result=<" 
            << result_ << ")\n";
    }
};
    
SubInstance* Text::makeInst(const Instance* inst) const 
{
    return new TextInstance(this, inst);
}
    
//////////////////////////////////////////////////////////////////

class CollectTextInstance : public SubInstance,
                            public GroveLib::NodeVisitor {
public:
    CSL_OALLOC(CollectTextInst);
    CollectTextInstance(const CollectText* ct, const Instance* instance) 
        : maxLength_(ct->maxLength()), 
          firstNodeOnly_(ct->firstNodeOnly()),
          selectExpr_(this, ct->selectExpr().makeInst(instance->nsi(), *this))
    {
        collectText();
    }
    void         collectText();
    virtual void doUpdate()
    {
        DDBG << "CollectInst: Update\n";
        collectText();
    }
    virtual void dump() const
    {
        DDBG << "CollectTextInstance (" << this << "), result=<"
            << result_ << ">\n";
    }
    void check_mod(const GroveLib::Node* n)
    {
        if (n->nodeType() == GroveLib::Node::TEXT_NODE ||
            n->nodeType() == GroveLib::Node::ELEMENT_NODE)
                setModified();
    }
    virtual void nodeDestroyed(const GroveLib::Node*) {}
    virtual void childInserted(const GroveLib::Node* n) { check_mod(n); }
    virtual void childRemoved(const GroveLib::Node*,
                              const GroveLib::Node* n) { check_mod(n); }
    virtual void attributeChanged(const GroveLib::Attr*) {}
    virtual void attributeRemoved(const GroveLib::Element*,
                                  const GroveLib::Attr*) {}
    virtual void attributeAdded(const GroveLib::Attr*) {}
    virtual void textChanged(const GroveLib::Text*) { setModified(); }
    virtual void genericNotify(const GroveLib::Node*, void*) {}

private:
    bool                  add_text(GroveLib::Node* n);
    uint                  maxLength_ : 24;
    bool                  firstNodeOnly_;
    Xpath::ValueHolderPtr selectExpr_;
};

void CollectTextInstance::collectText()
{
    const Xpath::Value* const value = selectExpr_->value().pointer();
    switch (value->type()) {
        case Xpath::Value::STRING:
            result_ = value->getString();
            return;
        case Xpath::Value::NODESET:
            break;
        default:
            result_ = String::null();
            return;
    }
    result_ = String();
    const Xpath::NodeSet& nset = value->getNodeSet();
    const Xpath::NodeSetItem* nsi = nset.first();
    if (firstNodeOnly_ && nsi) {
        GroveLib::Node* n = nsi->node();
        if (n->nodeType() == GroveLib::Node::ELEMENT_NODE) {
            n->registerNodeVisitor(this, 
                GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
                GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED);
            n = n->firstChild();
        }
        if (0 == n)
            return;
        if (n->nodeType() == GroveLib::Node::TEXT_NODE) 
            add_text(n);
        return;
    }    
    for (; nsi; nsi = nsi->next()) 
        if (add_text(nsi->node()))
            return;
}

bool CollectTextInstance::add_text(GroveLib::Node* n)
{
    switch (n->nodeType()) {
        case GroveLib::Node::ELEMENT_NODE: {
            n->registerNodeVisitor(this, 
                GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
                GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED);
            if (n->prevSibling() && 
                n->prevSibling()->nodeType() == GroveLib::Node::ELEMENT_NODE)
                    result_.append(' ');
            GroveLib::Node* cn = n->firstChild();
            for (; cn; cn = cn->nextSibling())
                if (add_text(cn))
                    return true;
            return false;
         }
         case GroveLib::Node::TEXT_NODE: {
            const GroveLib::Text* const text =
                static_cast<const GroveLib::Text*>(n);
            int residue = maxLength_ - result_.length();
            if (residue <= 0)
                return true;
            bool finished = false;
            if (residue > (int)text->data().length())
                residue = text->data().length();
            else
                finished = true;
            result_.append(text->data().unicode(), residue);
            n->registerNodeVisitor(this,
                GroveLib::NodeVisitor::NOTIFY_TEXT_CHANGED);
            return finished;
        }
        default:
            return false;
    }
}

SubInstance* CollectText::makeInst(const Instance* inst) const 
{
    return new CollectTextInstance(this, inst);
}
    
//////////////////////////////////////////////////////////////////

class ValueOfInstance : public SubInstance {
public:
    CSL_OALLOC(ValueOfInst);
    ValueOfInstance(const ValueOf* valueof, const Instance* instance) 
        : maxLength_(valueof->maxLength()), selectExpr_(this, 
            valueof->selectExpr().makeInst(instance->nsi(), *this))
    {
        doUpdate();
    }
    virtual void doUpdate();
    virtual void dump() const
    {
        DDBG << "ValueOfInstance (" << this << "), result=<"
            << result_ << ">\n";
    }
private:
    uint                  maxLength_;
    Xpath::ValueHolderPtr selectExpr_;
};

void ValueOfInstance::doUpdate()
{
    result_ = selectExpr_->value()->getString().left(maxLength_);
}

SubInstance* ValueOf::makeInst(const Instance* inst) const 
{
    return new ValueOfInstance(this, inst);
}
    
//////////////////////////////////////////////////////////////////

class NumberInstance : public SubInstance,
                       public Xslt::NumberClient {
public:
    CSL_OALLOC(NumberInst);
    NumberInstance(const Number* number, const Instance* instance)
        : NumberClient(number, instance->nsi().node(), 
            instance->rootInstance()->numberCache(), *this),
          instruction_(number)
    {
        doUpdate();
    }
    virtual void  doUpdate();
    virtual void  dump() const;
    virtual void  numberChanged() 
    {
        if (parentInst())
            setModified();
    }
    virtual const Xslt::NumberInstructionBase& numberInstruction() const 
    {
        return *instruction_;
    }
private:
    const Number* instruction_;
};

void NumberInstance::doUpdate()
{
    result_ = toString();
}

void NumberInstance::dump() const
{
    DDBG << "NumberInstance (" << this << "), result=<"
        << result_ << ">\n";
}

class AnyNumberInstance : public SubInstance,
                          public Xslt::AnyNumberClient {
public:
    CSL_OALLOC(AnyNumberInst);
    AnyNumberInstance(const Number* number, const Instance* instance)
        : AnyNumberClient(number, instance->nsi().node(), 
            instance->rootInstance()->numberCache(), *this),
          instruction_(number)
    {
        doUpdate();
    }
    virtual void  doUpdate();
    virtual void  dump() const;
    virtual void  numberChanged() 
    {
        if (parentInst())
            setModified();
    }
    virtual const Xslt::NumberInstructionBase& numberInstruction() const 
    {
        return *instruction_;
    }
private:
    const Number* instruction_;
};

void AnyNumberInstance::doUpdate()
{
    if (parentInst())
        clientUpdate(parentInst()->nsi().node(), *this);
    result_ = toString();
}

void AnyNumberInstance::dump() const
{
    DDBG << "AnyNumberInstance (" << this << "), result=<"
        << result_ << ">\n";
}

SubInstance* Number::makeInst(const Instance* inst) const 
{
    if (level() == ANY)
        return new AnyNumberInstance(this, inst);
    return new NumberInstance(this, inst);
}

//////////////////////////////////////////////////////////////////

} // namespace Csl
