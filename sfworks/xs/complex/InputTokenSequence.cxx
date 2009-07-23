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
#include "xs/xs_defs.h"
#include "xs/Schema.h"
#include "xs/complex/InputTokenSequence.h"
#include "xs/complex/GeneratedParticle.h"
#include "xs/XsNodeExt.h"
#include "xs/XsMessages.h"
#include "xs/Origin.h"
#include "grove/ValidationCommandsMaker.h"
#include "grove/Nodes.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

InputTokenSequence::token_entry::token_entry(QnameToken* tok,
                                             GROVE_NAMESPACE::Element* ep)
    : token(tok), element(ep)
{
}


bool InputTokenSequence::build(Schema* s,
                               GROVE_NAMESPACE::Element* parent,
                               int flags, 
                               GroveLib::ValidationCommandsMaker* cm)
{
    bool ok = true;
    parent_ = parent;
    commandMaker_ = cm;
    GROVE_NAMESPACE::Node* n = parent->firstChild();
    for (; n; n = n->nextSibling()) {
        if (!(flags & cdataAllowed) && n->nodeType() == Node::TEXT_NODE) {
            if (!TEXT_CAST(n)->data().stripWhiteSpace().isEmpty()) {
                s->mstream() << XsMessages::nonEmptyTextChunk
                             << Message::L_ERROR
                             << SRC_ORIGIN(parent);
                ok = false;
            }
        }
        if (n->nodeType() == GroveLib::Node::CHOICE_NODE) {
            choiceList_.push_back(static_cast<GroveLib::ChoiceNode*>(n));
            continue;
        }
        // ignore all other node types
        if (n->nodeType() != GROVE_NAMESPACE::Node::ELEMENT_NODE) 
            continue;
        append(ELEMENT_CAST(n));
    }
    return ok;
}

void InputTokenSequence::append(GROVE_NAMESPACE::Element* elem)
{
    entries_.push_back(new token_entry(new InputToken(elem), elem));
}

void InputTokenSequence::insertBefore(Particle* p, ulong idx,
                                      const FsmMatcher::DfaState* s)
{
    token_entry* t = new token_entry(0, 0);
    ElementParticle* elem_part = p->asElementParticle();
    if (elem_part)
        t->matchedParticle = new GeneratedParticle(elem_part,s);
    else
        t->matchedParticle = p;
    if (idx >= entries_.size())
        entries_.push_back(t);
    else
        entries_.insert(&entries_[idx], 1, t);
}

InputTokenSequence::InputTokenSequence()
    : commandMaker_(0)
{
    entries_.reserve(1024); // general speed-up
}

InputTokenSequence::~InputTokenSequence()
{
    if (commandMaker_) {
        GroveLib::ChoiceNodeList::iterator it = choiceList_.begin();
        for (; it != choiceList_.end(); ++it) 
            commandMaker_->removeNode(it);
    }
    for (ulong i = 0; i < entries_.size(); ++i)
        delete entries_[i];
}

void InputTokenSequence::dump() const
{
#ifdef XS_DEBUG
    std::cerr << NOTR("ISEQ: ");
    for (ulong i = 0; i < size(); ++i) {
        const token_entry& te = *entries_[i];
        if (te.token)
            std::cerr << NOTR("INP: ") << te.token->format() << std::endl;
        else {
            std::cerr << NOTR("PART: "); te.matchedParticle->dump(0);
        }
    }
#endif // XS_DEBUG
}

XS_NAMESPACE_END
