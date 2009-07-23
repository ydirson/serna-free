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
#include "grove/ElementMatcher.h"
#include "grove/grove_trace.h"
#include "grove/Nodes.h"
#include "common/Vector.h"
#include "common/PropertyTree.h"
#include "common/StringTokenizer.h"
#include <algorithm>

using namespace Common;

namespace GroveLib {

typedef RefCntPtr<ElementMatcher::PatternNode> PatternNodePtr;
typedef Vector<PatternNodePtr> PatternNodeList;

class ElementMatcher::PatternNode : public RefCounted<> {
public:
    PatternNode(const String& qname = "#root", bool isTerminal = false)
        : qname_(qname), isTerminal_(isTerminal) {}

    PatternNodeList& children() { return children_; }
    const String&   qname() const { return qname_; }
    bool            isTerminal() const    { return isTerminal_; }
    void            setTerminal(bool v) { isTerminal_ = v; }
    void            dump() const;

private:
    String          qname_;
    PatternNodeList children_;
    bool            isTerminal_;
};

class PatternNodeCmp {
public:
    bool operator()(const PatternNodePtr& node1,
                    const String& qname)
    {
        if (node1->qname() == "*")
            return false;
        if (qname == "*")
            return true;
        return node1->qname() < qname;
    }
};

static void add_pattern(ElementMatcher::PatternNode* pn, const String& pattern)
{
    Vector<String> tokens;
    tokens.reserve(32);
    for (StringTokenizer st(pattern, "/"); st;) {
        String tok = st.next();
        if (!tok.isEmpty())
            tokens.push_back(tok);
    }
    if (!tokens.size())
        return;
    String& tok = tokens[tokens.size() - 1];
    if (tok[0] == '@')
        tok = tok.mid(1);
    for (int i = tokens.size() - 1; i >= 0; --i) {
        PatternNodePtr newptn =
            new ElementMatcher::PatternNode(tokens[i], !i);
        PatternNodeList& nl = pn->children();
        if (0 == nl.size()) {
            nl.push_back(newptn);
            pn = newptn.pointer();
            continue;
        }
        PatternNodeList::iterator it =
            std::lower_bound(nl.begin(), nl.end(), newptn->qname(),
                PatternNodeCmp());
        if (it != nl.end() && (*it)->qname() == newptn->qname()) {
            pn = it->pointer();
            if (0 == i)
                pn->setTerminal(true);
            continue;
        }
        nl.insert(it, newptn);
        pn = newptn.pointer();
    }
}

static int match(ElementMatcher::PatternNode* pn, const String& firstName,
                 const Element* context)
{
    int matchDepth = 0;
    const String* currentName = &firstName;
    for (; pn; ) {
        PatternNodeList& nl = pn->children();
        if (!nl.size())
            break;
        PatternNodeList::iterator it = std::lower_bound(nl.begin(),
            nl.end(), *currentName, PatternNodeCmp());
        if (it == nl.end())
            break;
        if ((*it)->qname() != *currentName) {
            if ((*it)->qname() != "*")
                break;
            --matchDepth;
        }
        matchDepth += 2;
        pn = it->pointer();
        if (!context)
            break;
        currentName = &context->nodeName();
        const Node* parent = parentNode(context);
        if (!parent || parent->nodeType() != Node::ELEMENT_NODE)
            context = 0;
        else
            context = CONST_ELEMENT_CAST(parent);
    }
    if (pn->isTerminal())
        return matchDepth;
    return 0;
}

void ElementMatcher::addPattern(const String& pattern)
{
    PatternNode* root = 0;
    for (StringTokenizer st(pattern); st;) {
        bool hasAttr = false;
        bool disable = false;
        String tok = st.next().stripWhiteSpace();
        if (tok.isEmpty())
            continue;
        if (tok.find('@') >= 0)
            hasAttr = true;
        if (tok[0] == '!') {
            tok = tok.mid(1);
            disable = true;
        }
        if (hasAttr) {
            if (disable)
                root = attrsDisable_.pointer();
            else
                root = attrsEnable_.pointer();
        } else {
            if (disable)
                root = elemsDisable_.pointer();
            else
                root = elemsEnable_.pointer();
        }
        add_pattern(root, tok);
    }
}

bool ElementMatcher::matchElement(const Element* elem) const
{
    const Element* pelem = 0;
    if (parentNode(elem) &&
        parentNode(elem)->nodeType() == Node::ELEMENT_NODE)
            pelem = CONST_ELEMENT_CAST(parentNode(elem));
    return match(elemsEnable_.pointer(), elem->nodeName(), pelem) >
        match(elemsDisable_.pointer(), elem->nodeName(), pelem);
}

bool ElementMatcher::matchElement(const Element* parent,
                                  const Common::String& elemQname) const
{
    return match(elemsEnable_.pointer(), elemQname, parent) >
        match(elemsDisable_.pointer(), elemQname, parent);
}

bool ElementMatcher::matchAttr(const Attr* attr) const
{
    return match(attrsEnable_.pointer(), attr->nodeName(), attr->element()) >
        match(attrsDisable_.pointer(), attr->nodeName(), attr->element());
}

bool ElementMatcher::matchAttr(const Element* parent,
                               const Common::String& attrQname) const
{
    return match(attrsEnable_.pointer(), attrQname, parent) >
        match(attrsDisable_.pointer(), attrQname, parent);
}

ElementMatcher::ElementMatcher(const String& pattern)
    : elemsEnable_(new PatternNode),
      elemsDisable_(new PatternNode),
      attrsEnable_(new PatternNode),
      attrsDisable_(new PatternNode)
{
    addPattern(pattern);
}

ElementMatcher::~ElementMatcher()
{
}

void ElementMatcher::dump() const
{
    DDBG << "ElementMatcher:\n";
    DDINDENT;
    if (elemsEnable_->children().size()) {
        DDBG << "ElemsEnable:\n";
        DDINDENT;
        elemsEnable_->dump();
    }
    if (elemsDisable_->children().size()) {
        DDBG << "ElemsDisable:\n";
        DDINDENT;
        elemsDisable_->dump();
    }
    if (attrsEnable_->children().size()) {
        DDBG << "AttrsEnable:\n";
        DDINDENT;
        attrsEnable_->dump();
    }
    if (attrsDisable_->children().size()) {
        DDBG << "AttrsDisable:\n";
        DDINDENT;
        attrsDisable_->dump();
    }
}

void ElementMatcher::PatternNode::dump() const
{
#ifndef NDEBUG
    DDBG << qname() << ": " << int(isTerminal_) << std::endl;
    DDINDENT;
    for (uint i = 0; i < children_.size(); ++i)
        children_[i]->dump();
#endif // NDEBUG
}

} // namespace GroveLib
