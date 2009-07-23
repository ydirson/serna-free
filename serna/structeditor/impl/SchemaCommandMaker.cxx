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
#include "structeditor/se_defs.h"
#include "structeditor/impl/SchemaCommandMaker.h"
#include "structeditor/impl/debug_se.h"
#include "groveeditor/GrovePos.h"
#include "grove/Node.h"
#include "grove/SectionSyncher.h"
#include "xs/Schema.h"
#include "common/PropertyTree.h"
#include "common/prtti.h"

using namespace GroveEditor;
using namespace Common;

namespace {

class RemoveNode;

class NodeContextHolder : public Command {
public:
    NodeContextHolder(const GrovePos& pos, GroveLib::Node* node)
        : pos_(pos), node_(node) {}

    const GroveLib::Node* node() const { return node_.pointer(); }
    virtual GroveLib::Node* posHint() const = 0;
    const GrovePos&       pos() const { return pos_; }

    PRTTI_DECL(RemoveNode);

protected:
    GrovePos            pos_;
    GroveLib::NodePtr   node_;
};

//
// Simple versions of GroveEditor commands - they do not attempt to
// split/join elements etc. and do not throw exceptions.
//
class InsertNode : public NodeContextHolder {
public:
    InsertNode(const GrovePos& pos,
               GroveLib::Node* node)
        : NodeContextHolder(pos, node) {}

protected:
    virtual GroveLib::Node* posHint() const 
    {
        return isExecuted() ? node_.pointer() : 0;
    }
    virtual void    doExecute()
    {
        if (pos_.before())
            pos_.before()->insertBefore(node_.pointer());
        else
            pos_.node()->appendChild(node_.pointer());
    }
    virtual void    doUnexecute()
    {
        node_->remove();
    }
};

class RemoveNode : public NodeContextHolder {
public:
    RemoveNode(GroveLib::Node* node)
        : NodeContextHolder(GrovePos(node->parent(),
            node->nextSibling()), node) {}

    virtual GroveLib::Node* posHint() const { return 0; }
    PRTTI_DECL(RemoveNode);

protected:
    virtual void    doExecute()
    {
        node_->remove();
    }
    virtual void    doUnexecute()
    {
        if (pos_.before())
            pos_.before()->insertBefore(node_.pointer());
        else
            pos_.node()->appendChild(node_.pointer());
    }
};

class SetAttributes : public NodeContextHolder {
public:
    SetAttributes(GroveLib::Element* elem,
                  const PropertyNode* specs)
        : NodeContextHolder(GrovePos(elem), elem)
    {
        if (0 == specs)
            return;
        const PropertyNode* pn = specs->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            if (pn->getString().isNull())
                continue;
            GroveLib::Attr* attr = new GroveLib::Attr(pn->name());
            if (pn->getSafeProperty("type")->getString() == "ID")
                attr->setIdClass(GroveLib::Attr::IS_ID);
            else if (pn->getSafeProperty("type")->getString() == "IDREF")
                attr->setIdClass(GroveLib::Attr::IS_IDREF);
            attr->setValue(pn->getString());
            attrs_.push_back(attr);
        }
    }
    virtual GroveLib::Node* posHint() const { return 0; }
    
protected:
    virtual void doExecute()
    {
        GroveLib::Element* elem = 
            static_cast<GroveLib::Element*>(pos_.node());
        for (uint i = 0; i < attrs_.size(); ++i) 
            elem->attrs().appendChild(attrs_[i].pointer());
    }
    virtual void doUnexecute()
    {
        for (uint i = 0; i < attrs_.size(); ++i) 
            attrs_[i]->remove();
    }
    
private:
    Common::Vector<GroveLib::AttrPtr> attrs_;
};

} // end of unnamed namespace

/////////////////////////////////////////////////////////////////

void SchemaCommandMaker::insertNode(GroveLib::Node* before,
                                    GroveLib::Node* parent,
                                    GroveLib::Node* node)
{
    DDBG << "SCMInsertNode: " << parent->nodeName()
        << "(" << parent << ")/" << node->nodeName()
        << "(" << node << ")\n";
    GrovePos gp(parent, before);
    GrovePosList gpl;
    get_pos_list(gp, gpl);
    InsertNode* in = new InsertNode(gp, node);
    executeAndAdd(in);
    for (uint i = 1; i < gpl.size(); ++i) {
        GroveLib::Node* n = node->cloneNode(true, gpl[i].node());
        DDBG << "Replicated insert: " << n->nodeName() << std::endl;
        if (node->nodeExt())
            n->setNodeExt(node->nodeExt()->copy());
        executeAndAdd(new InsertNode(gpl[i], n));
    }
}

void SchemaCommandMaker::removeNode(GroveLib::Node* node)
{
    DDBG << "SCMRemoveNode: " << node->parent()->nodeName()
        << "(" << node->parent() << ")/" << node->nodeName()
        << "(" << node << ")\n";
    NodePosList npl;
    get_pos_list(node, npl);
    executeAndAdd(new RemoveNode(node));
    for (uint i = 1; i < npl.size(); ++i)
        executeAndAdd(new RemoveNode(npl[i].pointer()));
}

void SchemaCommandMaker::setAttributes(GroveLib::Element* elem,
                                       const Common::PropertyNode* specs)
{
    DBG(SE.TEST) << "SCMSetAttributes: " << elem->nodeName() << "\n";
    if (0 == specs)
        return;
    DBG_IF(SE.TEST) specs->dump();
    const PropertyNode* pn = specs->firstChild();
    for (; pn; pn = pn->nextSibling()) 
        if (!pn->getString().isNull()) 
            break;
    if (0 == pn)
        return; // empty specs only
    NodePosList npl;
    get_pos_list(elem, npl);
    executeAndAdd(new SetAttributes(elem, specs));
    for (uint i = 1; i < npl.size(); ++i)
        executeAndAdd(new SetAttributes(ELEMENT_CAST(npl[i].pointer()), specs));
}

GroveLib::Node* SchemaCommandMaker::posHint() const
{
    Command* cmd = firstChild();
    GroveLib::Node* pos_hint = 0;
    for (; cmd; cmd = cmd->nextSibling()) {
        const NodeContextHolder* ch =
            static_cast<const NodeContextHolder*>(cmd);
        if (ch->posHint()) { 
            pos_hint = ch->posHint();
            break;
        }
    }   
    while (pos_hint && pos_hint->firstChild())
        pos_hint = pos_hint->firstChild();
    return pos_hint;
}

SchemaCommandMaker* SchemaCommandMaker::make(Schema* s)
{
    SchemaCommandMaker* m = new SchemaCommandMaker;
    s->setCommandMaker(m);
    return m;
}

SchemaCommandMaker::SchemaCommandMaker()
{
    setAutoUnexecute(false);
}

SchemaCommandMaker::~SchemaCommandMaker()
{
}

PRTTI_BASE_STUB(NodeContextHolder, RemoveNode)
PRTTI_IMPL(RemoveNode)
