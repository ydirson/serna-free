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
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/*! \file
 */

#include "common/asserts.h"
#include "common/PtrSet.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

#include "xs/xs_defs.h"
#include "xs/XsMessages.h"
#include "xs/Origin.h"
#include "xs/Schema.h"

#ifdef _MSC_VER
# include "xs/XsType.h"
#endif

#ifdef __SUNPRO_CC
# include "Schema.h"
# include "XsDataImpl.h"
#endif

#include "xs/complex/Connectors.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/complex/ContentToken.h"
#include "xs/complex/InputTokenSequence.h"
#include "xs/complex/FixupSet.h"
#include "xs/complex/AllMatcher.h"
#include "xs/components/XsElementImpl.h"
#include "xs/components/XsGroupImpl.h"
#include "xs/XsNodeExt.h"
#include "common/PropertyTree.h"
#include "grove/Nodes.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

USING_COMMON_NS
USING_GROVE_NAMESPACE
XS_NAMESPACE_BEGIN

class AllMatcherImpl : public AllMatcher {
public:
    typedef GROVE_NAMESPACE::ExpandedName ExpandedName;
    typedef std::map<ExpandedName, ElementParticle*> ParticleMap;

    virtual bool    match(Schema* s,
                          InputTokenSequence& iseq,
                          MatchState& ms,
                          PropertyNode* unmatched = 0) const;

    virtual void    listFirstElems(PropertyNode* el, 
                                   const GroveLib::Element*) const;
    virtual const XsElement*
                    getFirstElem(const GroveLib::ExpandedName& n) const;


    void    addElem(ElementParticle* p)
    {
        pmap_[p->element()->constCred()->expandedName()] = p;
    }
    AllMatcherImpl(AllConnector* ac)
        : ac_(ac) {}

private:
    friend class AllXsNodeExt;
    typedef std::set<const ElementParticle*> ParticleSet;

    AllConnector* ac_;
    ParticleMap   pmap_;
};

Matcher* AllConnector::buildAllMatcher(Schema* s)
{
    AllMatcherImpl* ami = new AllMatcherImpl(this);

    for (ulong i = 0; i < children_.size(); ++i) {
        ElementParticle* p = children_[i]->asElementParticle();
        if (0 == p) {
            s->mstream() << XsMessages::onlyElemsInAll
                << Message::L_ERROR << XSP_ORIGIN(children_[i].pointer());
            continue;
        }
        ami->addElem(p);
    }
    return ami;
}

// For ALL connector, all elements are possible to be the first
// in sequence
void AllMatcherImpl::listFirstElems(PropertyNode* el, 
                                    const GroveLib::Element*) const
{
    ParticleMap::const_iterator pi = pmap_.begin();
    for (; pi != pmap_.end(); ++pi)
        el->appendChild(pi->second->makeElemProperty());
}

bool AllMatcherImpl::match(Schema* s,
                           InputTokenSequence& iseq,
                           MatchState& ms,
                           PropertyNode* unmatched) const
{
    ParticleSet pset;
    ulong errors = 0;
    ParticleMap::const_iterator pmi;
    ParticleSet::const_iterator psi;

    for (ulong i = 0; i < iseq.size(); ++i) {
        pmi = pmap_.find(iseq[i].token->ename());
        if (pmi == pmap_.end()) {
            ++errors;
            if (ms.quickcheck)
                continue;
            s->mstream() << XsMessages::unexpectedToken
                << Message::L_ERROR << iseq[i].token->format()
                << DVXS_ORIGIN(iseq[i].element, ac_);
            continue;
        }
        ms.matchedTokens++;
        iseq[i].matchedParticle = pmi->second;
        psi = pset.find(pmi->second);
        if (psi != pset.end() || !pmi->second->maxOccur()) {
            ++errors;
            if (!ms.quickcheck) {
                s->mstream() << XsMessages::multipleElemsInAll
                    << Message::L_ERROR << iseq[i].token->format()
                    << DVXS_ORIGIN(iseq[i].element, ac_);
            }
            continue;
        }
        pset.insert(pmi->second);
    }
    for (pmi = pmap_.begin(); pmi != pmap_.end(); ++pmi) {
        psi = pset.find(pmi->second);
        if (psi == pset.end()) {
            if (unmatched)
                unmatched->appendChild(pmi->second->makeElemProperty());
            if (pmi->second->minOccur() > 0) {
                if (!ms.editMode)
                    ++errors;
                if (!ms.quickcheck && !ms.editMode) {
                    s->mstream() << XsMessages::missedAllElem
                        << Message::L_ERROR << XSP_ORIGIN(ac_)
                        << pmi->second->element()->constCred()->format();
                }
                if (ms.editMode)
                    iseq.insertBefore(pmi->second, iseq.size());
            }
        }
    }
    return (0 == errors);
}

const XsElement*
AllMatcherImpl::getFirstElem(const GroveLib::ExpandedName& n) const
{
    ParticleMap::const_iterator pmi = pmap_.find(n);
    if (pmap_.end() == pmi)
        return 0;
    return pmi->second->element();
}

bool AllMatcher::exactMatch(Schema* s,
                            InputTokenSequence& iseq,
                            MatchState& ms) const
{
    return match(s, iseq, ms, 0);
}

void AllXsNodeExt::getElemListAppend(PropertyNode* el,
                                     const GROVE_NAMESPACE::Node* pnode) const
{
    Node* lastc = pnode->lastChild();
    while (lastc && lastc->nodeType() != Node::ELEMENT_NODE)
        lastc = lastc->prevSibling();
    // elem has no children? use element type
    if (0 == lastc) {
        const Matcher* m = getMatcher();
        if (m)
            m->listFirstElems(el);
        return;
    }
    NodeExt* ext = lastc->nodeExt();
    if (!ext)
        return;
    XsNodeExt* xs_ext = ext->asXsNodeExt();
    if (!xs_ext)
        return;
    if (xs_ext->asChoiceXsNodeExt()) {
        PropertyNode* pn =
            xs_ext->asChoiceXsNodeExt()->elist()->firstChild();
        for (; pn; pn = pn->nextSibling())
            el->appendChild(pn->copy(true));
        return;
    }
    if (xs_ext->asDfaXsNodeExt()) {
        xs_ext->asDfaXsNodeExt()->getElemListAppend(el, pnode);
        return;
    }
    AllXsNodeExt* allExt = xs_ext->asAllXsNodeExt();
    if (0 == allExt)
        return;
    ext = pnode->nodeExt();
    if (0 == ext)
        return;
    xs_ext = ext->asXsNodeExt();
    if (0 == xs_ext)
        return;
    const AllMatcherImpl* m =
        dynamic_cast<const AllMatcherImpl*>(xs_ext->getMatcher());
    if (0 == m)
        return;
    typedef AllMatcherImpl::ParticleMap ParticleMap;
    typedef std::set<ExpandedName> NameSet;
    NameSet current;
    ParticleMap::const_iterator pmi;
    NameSet::const_iterator psi;
    for (Node* n = pnode->firstChild(); n; n = n->nextSibling()) {
        if (Node::ELEMENT_NODE == n->nodeType())
            current.insert(ELEMENT_CAST(n)->expandedName());
    }
    for (pmi = m->pmap_.begin(); pmi !=  m->pmap_.end(); ++pmi) {
        psi = current.find(pmi->first);
        if (psi == current.end())
            el->appendChild(pmi->second->makeElemProperty());
    }
}

void AllXsNodeExt::getElemListReplace(PropertyNode* el,
                              const GROVE_NAMESPACE::Element* n) const
{
    getElemListAppend(el, n->parent());
}

void AllXsNodeExt::getElemListBefore(PropertyNode* el,
                                  const GROVE_NAMESPACE::Node* n) const
{
    getElemListAppend(el, n->parent());
}

int AllConnector::checkSplit(const String& tokName, XsElement*& e) const
{
    int maxcnt = 0;
    for (ulong i = 0; i  < children_.size(); ++i)
        maxcnt += children_[i]->checkSplit(tokName, e);
    return maxcnt;
}

bool AllConnector::makeSkeleton(Schema* s,
                                GROVE_NAMESPACE::Node* attachTo,
                                GROVE_NAMESPACE::Element* pe,
                                FixupSet* elemSet,
                                ulong) const
{
    bool ok = true;
    if (minOccur() == 0)
        return ok;
    for (uint i = 0; i < children_.size(); ++i)
        ok = children_[i]->makeSkeleton(s, attachTo, pe, elemSet) && ok;
    return ok;
}

void AllConnector::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
        std::cerr << ' ';
    std::cerr << NOTR("AllConnector:") << std::endl;
    dumpChildren(indent + 2);
#endif // XS_DEBUG
}

PRTTI_IMPL(AllConnector);

/////////////////////////////////////////////////////////////

int ChoiceConnector::checkSplit(const String& tokName, XsElement*& e) const
{
    int maxcnt = 0;
    for (ulong i = 0; i < children_.size(); ++i) {
        int cnt = children_[i]->checkSplit(tokName, e);
        if (maxcnt < cnt)
            maxcnt = cnt;
    }
    return maxcnt;
}

bool ChoiceConnector::makeSkeleton(Schema* s,
                                   GROVE_NAMESPACE::Node* attachTo,
                                   GROVE_NAMESPACE::Element* pe,
                                   FixupSet* elemSet,
                                   ulong occurs) const
{
    bool ok = true;
    ulong minocc = ~0;
    Particle* child = 0;
    ulong i;
    for (i = 0; i < children_.size(); ++i) {
        if (children_[i]->minOccur() < minocc) {
            child  = children_[i].pointer();
            minocc = child->minOccur();
        }
    }
    if (!child || 0 == minocc)
        return true;            // nothing to do
    if (0 == occurs)
        occurs = minOccur();
    for (i = 0; i < occurs; ++i)
        ok = child->makeSkeleton(s, attachTo, pe, elemSet) && ok;
    return ok;
}

void ChoiceConnector::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("CHOICE: ") << minOccur() << '/' << maxOccur() << std::endl;
    dumpChildren(indent + 2);
#endif // XS_DEBUG
}

PRTTI_IMPL(ChoiceConnector)

///////////////////////////////////////////////////////////

void GroupConnector::setGroupRef(const XsGroupInst& grp)
{
    grp_ = grp;
}

int GroupConnector::checkSplit(const String& tokName, XsElement*& e) const
{
    Connector* c = grp_->connector(0);
    if (0 == c)
        return 0;
    return c->checkSplit(tokName, e);
}
    
void GroupConnector::makeSubst(Schema* s)
{
    Connector* c = grp_->connector(0);
    if (c)
        c->makeSubst(s);
}

bool GroupConnector::makeSkeleton(Schema* s,
                                  GROVE_NAMESPACE::Node* attachTo,
                                  GROVE_NAMESPACE::Element* pe,
                                  FixupSet* elemSet,
                                  ulong occurs) const
{
    Connector* c = grp_->connector(s);
    if (0 == c)
        return false;
    return c->makeSkeleton(s, attachTo, pe, elemSet, occurs);
}

Connector*  GroupConnector::connector()
{
    return grp_->connector(0);
}

void GroupConnector::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
        std::cerr << ' ';
    std::cerr << NOTR("GROUPCONNECTOR: ") << minOccur()
              << '/' << maxOccur() << std::endl;
    grp_->dump(indent + 2);
#endif // XS_DEBUG
}

PRTTI_IMPL(GroupConnector);

////////////////////////////////////////////////////////////

int SequenceConnector::checkSplit(const String& tokName, XsElement*& e) const
{
    int maxcnt = 0;
    for (ulong i = 0; i < children_.size(); ++i)
        maxcnt += children_[i]->checkSplit(tokName, e);
    return maxcnt;
}

bool SequenceConnector::makeSkeleton(Schema* s,
                                     GROVE_NAMESPACE::Node* attachTo,
                                     GROVE_NAMESPACE::Element* pe,
                                     FixupSet* elemSet,
                                     ulong occurs) const
{
    // sequentally attach children skeletons to pe minOccurs times
    bool ok = true;
    if (0 == occurs)
        occurs = minOccur();
    for (ulong nocc = 0; nocc < occurs; ++nocc) {
        for (ulong i = 0; i < children_.size(); ++i)
            ok = children_[i]->makeSkeleton(s, attachTo, pe, elemSet) && ok;
    }
    return ok;
}

void SequenceConnector::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("SEQUENCE: ") << minOccur() << '/' << maxOccur() << std::endl;
    dumpChildren(indent + 2);
#endif // XS_DEBUG
}

PRTTI_IMPL(SequenceConnector);

XS_NAMESPACE_END
