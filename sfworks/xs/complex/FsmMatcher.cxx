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
#include "xs/XsMessages.h"
#include "xs/complex/FsmMatcher.h"
#include "xs/complex/Connectors.h"
#include "xs/complex/InputTokenSequence.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/ChoiceParticle.h"
#include "xs/complex/GeneratedParticle.h"
#include "xs/XsGroup.h"
#include "xs/Schema.h"
#include "xs/XsNodeExt.h"
#include "common/SubAllocator.h"
#include "common/SmallStlAllocator.h"
#include <set>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#define MAX_DTRAN   10000 // maximum reasonable constructed DFA size

//#define XS_FSM_DEBUG
//#define XS_DFA_DEBUG

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

struct ContentTokenKF {
    bool operator()(const RefCntPtr<ContentToken>& t1,
                    const RefCntPtr<ContentToken>& t2) const
    {
        return t1->less(t2.pointer());
    }
};

class FsmMatcher::Alphabet :
    public std::set<RefCntPtr<ContentToken>, ContentTokenKF> {};

/*! A class which represent an NFA building block, which it iself an
 *  NFA. Complete NFA is kept in NfaHolder because it needs some
 *  information common for all sub-NFA's (e.g. alphabet, counters data)
 */
class Nfa : public RefCounted<> {
public:
    class NfaState;

    /// Structure which reperesents single NFA transition.
    struct NfaTrans {
        XS_OALLOC(NfaTrans);

        RefCntPtr<ContentToken> token;  // zero token means e-jump
        NfaState*     nstate;           // next state

        void        dump() const;
        NfaTrans()
            : nstate(0) {}
    };
    /*! This structure represents single NFA state.
     */
    class NfaState : public RefCounted<> {
    public:
        XS_OALLOC(NfaState);

        NfaTrans* addETrans(NfaState* ns) { return addTrans(0, ns); }
        NfaTrans* addTrans(ContentToken* tok, NfaState* ns)
        {
            trans[tsize].token = tok;
            trans[tsize].nstate = ns;
            return &trans[tsize++];
        }
        void join_trans(const NfaState& other)
        {
            for (uint i = 0; i < other.tsize; ++i) {
                const NfaTrans& nt = other.trans[i];
                addTrans(nt.token.pointer(), nt.nstate);
            }
        }
        void        dump() const;
        uint        tsize;          // number of transitions
        NfaTrans    trans[2];

#ifdef XS_FSM_DEBUG
        NfaState() : tsize(0) { state_id = ++state_ids_; }
        uint state_id;  // for dump only
        static uint state_ids_;
#else // XS_FSM_DEBUG
        NfaState() : tsize(0) {}
#endif // XS_FSM_DEBUG
    };

    // make extra transitions for ?, * quantifiers
    void    make_occurences(bool omittable, bool repeatable);

    // join other NFA into current one as sequence
    void    seq_join(const Nfa* other);

    // join other NFA into current one as choice
    void    choice_join(const Nfa* other);

    void    optimize();
    void    dump() const;
    void    dumpAsDot(std::ostream& os) const;

    Nfa(FsmMatcher* top, ContentToken* t = 0);

    typedef NfaState* NSP;
#if !(defined(_MSC_VER) && _MSC_VER <= 1200)
    typedef FixedStlAllocator<NSP, SingleThreaded<> > NfaStateAllocator;
    typedef std::set<NSP, std::less<NSP>, NfaStateAllocator> NSPSet;
#else
    typedef std::set<NSP> NSPSet;
#endif
    //!
private:
    friend class FsmMatcher::DfaBuilder;

    // copy (re-use) states from other NFA
    void    copy_states(const NfaState* except, const Nfa* other);

    // allocate new state
    NfaState* new_state()
    {
        NfaState* ns = new NfaState;
        states_.insert(ns);
        return ns;
    }

    typedef RefCntPtr<NfaState>   NfaStatePtr;
    typedef std::set<NfaStatePtr> NfaStateSet;
    typedef NSPSet                visitedSet;
    typedef visitedSet            candidateSet;

    // used for NFA optimization
    static bool isSingle(NfaState* s)
    {
        return s->tsize == 1 && !s->trans[0].token;
    }
    static void optimize_dfs(NfaState* s, NfaTrans* nt,
                             visitedSet& vs, candidateSet& cs);
    NfaStateSet states_;
    NfaState*   startState_;
    NfaState*   endState_;
};

////////////////////////////////////////////////////////////

class FsmMatcher::DfaTrans {
public:
    XS_OALLOC(DfaTrans);

    DfaState*       nstate;
    ContentToken*   token;

    DfaTrans(DfaState* s, ContentToken* t)
        : nstate(s), token(t) {}
};

class FsmMatcher::DfaState {
public:
    XS_OALLOC(DfaState);
    typedef std::vector<DfaTrans> dfaTranList;
    typedef Nfa::NSPSet States;

    DfaState(uint stateid = 0) : state_id(stateid) {}
    DfaState(Nfa::NfaState* s) : state_id(0) { states.insert(s); }

    void dump() const;
    String name() const;

    States      states;
    dfaTranList trans;
    uint        state_id;
};

class FsmMatcher::Dfa {
public:
    typedef std::set<DfaState*>   dTranSet;
    typedef DfaState::dfaTranList dfaTranList;

    Dfa()
        : startState_(0), endToken_(0) {}
    ~Dfa();

    void  dumpAsDot(std::ostream& os) const;
    void  finalize(Schema*);

private:
    friend class DfaBuilder;
    friend class FsmMatcher;

    void    add_dtran(DfaState* from, ContentToken* tok, DfaState* to)
    {
        dTran_.insert(from);
        from->trans.push_back(DfaTrans(to, tok));
    }
    dTranSet        dTran_;
    DfaState*       startState_;
    ContentToken*   endToken_;
};

class FsmMatcher::DfaBuilder {
public:
    void    build(Schema*, const Particle*, Nfa*, Alphabet&, Dfa*);

    DfaBuilder()
        : dtranCount_(0), current_stateid_(0) {}

private:
    typedef Nfa::NfaState NfaState;
    typedef std::list<DfaState*>  dfaStateStack;
    typedef Dfa::dfaTranList dfaTranList;

    void        add_dtran(DfaState* from, ContentToken* tok, DfaState* to);
    DfaState    move(const DfaState* from, ContentToken* t);
    DfaState*   eclosure(const DfaState& T);
    DfaState*   find_dstate(const DfaState* ds) const;
    DfaState*   new_state();

    dfaStateStack   unmarkedStates_;
    dfaStateStack   markedStates_;
    ulong           dtranCount_;
    ulong           current_stateid_;
};

class FsmMatcher::DfaPathGen {
public:
    typedef DfaState::dfaTranList dfaTranList;
    typedef std::list<const DfaTrans*> dfaPath;

    struct StateAndTrans {
        const DfaState* state;
        const DfaTrans* trans;

        StateAndTrans(const DfaState* s = 0, const DfaTrans* dt = 0)
            : state(s), trans(dt) {}
    };
    bool    find(const DfaState* startState,
                 const ContentToken* token,
                 dfaPath& tl);

private:
    typedef std::set<const DfaState*> colorSet;
    typedef std::map<const DfaState*, StateAndTrans> predGraph;
    typedef std::list<const DfaState*> Queue;

    bool make_predpath(dfaPath& tl, const DfaState* s, const DfaState* v);

    colorSet    color_;
    predGraph   pg_;
    Queue       q_;
};

////////////////////////////////////////////////////////////

#ifdef XS_FSM_DEBUG
uint Nfa::NfaState::state_ids_ = 0;
#endif // XS_FSM_DEBUG

// Create an NFA for a single token (or single e-transition)
Nfa::Nfa(FsmMatcher* top, ContentToken* tok)
{
    startState_ = new_state();
    endState_   = new_state();
    startState_->addTrans(tok, endState_);
    if (tok)
        top->alphabet_->insert(tok);
}

// merge states from another nfa except specified one
void Nfa::copy_states(const NfaState* except, const Nfa* other)
{
    NfaStateSet::const_iterator it = other->states_.begin();
    for (; it != other->states_.end(); ++it) {
        NfaState* os = it->pointer();
        if (os != except)
            states_.insert(os);
    }
}

void Nfa::seq_join(const Nfa* other)
{
    copy_states(other->startState_, other);
    endState_->join_trans(*other->startState_);
    endState_ = other->endState_;
}

void Nfa::choice_join(const Nfa* other)
{
    copy_states(0, other);
    NfaState* is = new_state();
    NfaState* es = new_state();
    is->addETrans(startState_);
    is->addETrans(other->startState_);
    endState_->addETrans(es);
    other->endState_->addETrans(es);
    startState_ = is;
    endState_ = es;
}

void Nfa::make_occurences(bool omittable, bool repeatable)
{
    NfaState* is = new_state();
    NfaState* es = new_state();
    is->addETrans(startState_);
    endState_->addETrans(es);
    if (omittable)
        is->addETrans(es);
    if (repeatable)
        endState_->addETrans(startState_);
    startState_ = is;
    endState_ = es;
}

/////////////////////////////////////////////////////
//
// NFA optimization: remove e-states with single e-transitions from the NFA
//

void Nfa::optimize_dfs(NfaState* s, NfaTrans* nt,
                                   visitedSet& vs, candidateSet& cs)
{
    if (vs.find(s) != vs.end())
        return;
    vs.insert(s);
    if (isSingle(s)) {
        vs.erase(s);
        cs.insert(s);
        NfaState* ns = s->trans[0].nstate;
        for (; ns && isSingle(ns); ns = ns->trans[0].nstate)
            ;
        if (ns && nt)
            nt->nstate = ns;
    }
    for (uint i = 0; i < s->tsize; ++i)
        optimize_dfs(s->trans[i].nstate, &s->trans[i], vs, cs);
}

void Nfa::optimize()
{
    visitedSet vs;
    candidateSet cs;
    for (uint i = 0; i < startState_->tsize; ++i)
        optimize_dfs(startState_->trans[i].nstate,
                     &startState_->trans[i], vs, cs);
    for (candidateSet::iterator it = cs.begin(); it != cs.end(); ++it)
        states_.erase(*it);
}

void Nfa::NfaTrans::dump() const
{
#ifdef XS_FSM_DEBUG
    if (!token)
        std::cerr << "#";
    else
        std::cerr << token->format();
    std::cerr << "/";
    if (nstate)
        std::cerr << nstate->state_id;
    else
        std::cerr << "Z";
    std::cerr << " ";
#endif // XS_FSM_DEBUG
}

void Nfa::NfaState::dump() const
{
#ifdef XS_FSM_DEBUG
    for (uint i = 0; i < tsize; ++i)
        trans[i].dump();
#endif // XS_FSM_DEBUG
}

void Nfa::dump() const
{
#ifdef XS_FSM_DEBUG
    std::cerr << NOTR("NFA states:\n");
    NfaStateSet::const_iterator it = states_.begin();
    for (; it != states_.end(); ++it) {
        const NfaState* s = it->pointer();
        std::cerr << s->state_id << ": ";
        s->dump();
        if (s == startState_)
            std::cerr << NOTR("[Start]");
        if (s == endState_)
            std::cerr << NOTR("[End]");
        std::cerr << "\n";
    }
#endif // XS_FSM_DEBUG
}

void Nfa::dumpAsDot(std::ostream& os) const
{
#ifdef XS_FSM_DEBUG
    os << NOTR("digraph NFA {\n");
    NfaStateSet::const_iterator it = states_.begin();
    for (; it != states_.end(); ++it) {
        const NfaState* s = it->pointer();
        os << NOTR("    ") << s->state_id << NOTR(" [label=\"") << s->state_id;
        if (s == startState_)
            os << NOTR("[Start]");
        if (s == endState_)
            os << NOTR("[End]");
        os << "\"];\n";
        for (uint i = 0; i < s->tsize; ++i) {
            const NfaTrans& t = s->trans[i];
            os << "\t" << s->state_id << NOTR(" -> ");
            if (t.nstate)
                os << t.nstate->state_id;
            else
                os << NOTR("ZERO");
            os << "[label=\"";
            if (t.token)
                os << t.token->format();
            os << "\"];\n";
        }
    }
    os << "}\n";
#else // XS_FSM_DEBUG
    (void) os;
#endif // XS_FSM_DEBUG
}

/////////////////////////////////////////////////////////////////

void FsmMatcher::DfaBuilder::build(Schema* s, const Particle* p, Nfa* nfa,
                                   Alphabet& alphabet, Dfa* dfa)
{
#ifdef XS_DFA_DEBUG
    std::cerr << NOTR("Alphabet: ");
    Alphabet::const_iterator a = alphabet.begin();
    for (; a != alphabet.end(); ++a)
        std::cerr << (*a)->format() << ' ';
    std::cerr << std::endl;
#endif // XS_DFA_DEBUG
    unmarkedStates_.push_back(eclosure(DfaState(nfa->startState_)));
    dfa->startState_ = unmarkedStates_.back();
    DfaState* U = 0, *R = 0;
    for (;;) {
        if (unmarkedStates_.empty())     // while unmarked state in Dstates
            break;
        DfaState* T = unmarkedStates_.back();
        unmarkedStates_.pop_back();
        markedStates_.push_back(T);      // mark T
        Alphabet::const_iterator ai = alphabet.begin();
        for (; ai != alphabet.end(); ++ai) {
            DfaState ts = move(T, ai->pointer());
            if (ts.states.empty())
                continue;
            U = eclosure(ts);
            R = find_dstate(U);
            if (!R) {
                unmarkedStates_.push_back(U);
            } else {
                delete U;
                U = R;
            }
            if (++dtranCount_ >= MAX_DTRAN) {
                if (dtranCount_ == MAX_DTRAN) {
                    s->mstream() << XsMessages::tooBigDfa << uint32(MAX_DTRAN)
                        << XSP_ORIGIN(p) << Message::L_ERROR;
                    dtranCount_ = MAX_DTRAN + 1;
                }
                return;
            }
            dfa->add_dtran(T, ai->pointer(), U);
        }
    }
}

FsmMatcher::DfaState
FsmMatcher::DfaBuilder::move(const DfaState* from, ContentToken* t)
{
    DfaState ds;
    DfaState::States::const_iterator it = from->states.begin();
    for (; it != from->states.end(); ++it) {
        NfaState* s = *it;
        for (uint i = 0; i < s->tsize; ++i) {
            const ContentToken* tok = s->trans[i].token.pointer();
            if (tok && tok->dataaddr() == t->dataaddr())
                ds.states.insert(s->trans[i].nstate);
        }
    }
    return ds;
}

FsmMatcher::DfaState* FsmMatcher::DfaBuilder::eclosure(const DfaState& T)
{
#if !(defined(_MSC_VER) && _MSC_VER <= 1200)
    typedef std::list<Nfa::NSP, Nfa::NfaStateAllocator> stateStack;
#else
    typedef std::list<Nfa::NSP> stateStack;
#endif
    stateStack stack;
    DfaState::States::const_iterator it = T.states.begin();
    for (; it != T.states.end(); ++it)
        stack.push_back(*it);
    DfaState* ecl = new_state();
    uint state_id = ecl->state_id;
    *ecl = T;
    ecl->state_id = state_id;
    while (!stack.empty()) {
        NfaState* t = stack.back();
        stack.pop_back();
        for (uint i = 0; i < t->tsize; ++i) {
            if (t->trans[i].token)
                continue;
            NfaState* u = t->trans[i].nstate;
            if (ecl->states.find(u) == ecl->states.end()) {
                ecl->states.insert(u);
                stack.push_back(u);
            }
        }
    }
    return ecl;
}

FsmMatcher::DfaState*
FsmMatcher::DfaBuilder::find_dstate(const DfaState* ds) const
{
    dfaStateStack::const_iterator it = unmarkedStates_.begin();
    for (; it != unmarkedStates_.end(); ++it)
        if ((*it)->states == ds->states)
            return *it;
    it = markedStates_.begin();
    for (; it != markedStates_.end(); ++it)
        if ((*it)->states == ds->states)
            return *it;
    return 0;
}

FsmMatcher::DfaState* FsmMatcher::DfaBuilder::new_state()
{
    return new DfaState(current_stateid_++);
}

#ifdef XS_FSM_DEBUG
static void dump_state(std::ostream& os, const FsmMatcher::DfaState* ds)
{
    FsmMatcher::Dfa::dfaTranList::const_iterator it2 = ds->trans.begin();
    for (; it2 != ds->trans.end(); ++it2) {
        os << '"' << ds->state_id << '"' << " -> "
           << '"' << it2->nstate->state_id << '"'
            << " [label=\"" << it2->token->format();
        os << "\"];\n";                                          
    }
}
#endif

void FsmMatcher::Dfa::dumpAsDot(std::ostream& os) const
{
#ifdef XS_FSM_DEBUG
    dTranSet::const_iterator it = dTran_.begin();
    os << NOTR("digraph G {\n");
    os << '"' << startState_->state_id << NOTR("\" [shape=box];\n");
    for (; it != dTran_.end(); ++it) 
        dump_state(os, *it);
    os << "}\n";
#else // XS_FSM_DEBUG
    (void) os;
#endif // XS_FSM_DEBUG
}



// Check overlapping tokens in each transition; release NFA state sets
void FsmMatcher::Dfa::finalize(Schema* s)
{
    dTranSet::iterator it = dTran_.begin();
    dfaTranList::iterator ti, ti2;
    for (; it != dTran_.end(); ++it) {
        (*it)->states.clear();
        for (ti = (*it)->trans.begin(); ti != (*it)->trans.end(); ++ti) {
            ti2 = ti;
            for (++ti2; ti2 != (*it)->trans.end(); ++ti2) {
                if (ti->token->match(ti2->token))
                    s->mstream() << XsMessages::tokensOverlap
                        << Message::L_ERROR
                        << ti->token->format()
                        << ti2->token->format()
                        << XSP_ORIGIN(ti->token->particle());
            }
        }
    }
}

void FsmMatcher::DfaState::dump() const
{
#ifdef XS_FSM_DEBUG
    std::cerr << NOTR("DFA state: ") << name() << std::endl;
#endif // XS_FSM_DEBUG
}

String FsmMatcher::DfaState::name() const
{
    String n;
#ifdef XS_FSM_DEBUG
    States::const_iterator it = states.begin();
    for (; it != states.end(); ++it) {
        n += String::number((*it)->state_id);
        n += ' ';
    }
#endif // XS_FSM_DEBUG
    return n;
}

FsmMatcher::Dfa::~Dfa()
{
    dTranSet::iterator it = dTran_.begin();
    for (; it != dTran_.end(); ++it)
        delete *it;
}

#ifdef XS_FSM_DEBUG

void FsmMatcher::nfa_dumpAsDot(const char* ofn)
{
    std::ofstream ofs(ofn);
    if (nfa_)
        nfa_->dumpAsDot(ofs);
}

void FsmMatcher::dfa_dumpAsDot(const char* ofn)
{
    std::ofstream ofs(ofn);
    if (dfa_)
        dfa_->dumpAsDot(ofs);
}

#endif // XS_FSM_DEBUG

/////////////////////////////////////////////////////////////////

FsmMatcher::FsmMatcher(Schema* s, Connector* top)
{
    alphabet_ = new Alphabet;
    topConnector_ = top;
    dfa_ = new Dfa;
    nfa_ = top->build_nfa(s, this);
    dfa_->endToken_ = new SeqEndToken(top);
    nfa_->seq_join(new Nfa(this, dfa_->endToken_));
    nfa_->optimize();
    DfaBuilder db;
    db.build(s, top, nfa_.pointer(), *alphabet_, dfa_.pointer());
    dfa_->finalize(s);
    //nfa_ = 0;
}

FsmMatcher::~FsmMatcher()
{
}

/////////////////////////////////////////////////////////////////

RefCntPtr<Nfa>
Particle::quantize_nfa(Schema* s, FsmMatcher* top, Particle* child)
{
    if (minOccur() == 1 && maxOccur() == 1)
        return child->build_nfa(s, top);
    RefCntPtr<Nfa> nfa;
    if (minOccur() == 0) {
        if (maxOccur() == 1) {
            nfa = child->build_nfa(s, top);
            nfa->make_occurences(1, 0);
            return nfa;
        }
        if (maxOccur() == unbounded) {
            nfa = child->build_nfa(s, top);
            nfa->make_occurences(1, 1);
            return nfa;
        }
    }
    uint i;
    nfa = new Nfa(top);
    RefCntPtr<Nfa> nfa2;
    for (i = 0; i < minOccur(); ++i)
        nfa->seq_join(child->build_nfa(s, top).pointer());
    if (maxOccur() == unbounded) {
        nfa->make_occurences(0, 1);
        return nfa;
    }
    for (i = minOccur(); i < maxOccur(); ++i) {
        nfa2 = child->build_nfa(s, top);
        nfa2->make_occurences(1, 0);
        nfa->seq_join(nfa2.pointer());
    }
    return nfa;
}


RefCntPtr<Nfa> AllConnector::build_nfa(Schema*, FsmMatcher*)
{
    RT_MSG_ABORT("AllConnector:: build_nfa?");
    return 0;
}

RefCntPtr<Nfa> ChoiceConnector::build_single_nfa(Schema* s, FsmMatcher* top)
{
    RefCntPtr<Nfa> nfa = children_[0]->build_nfa(s, top);
    for (ulong i = 1; i < children_.size(); ++i)
        nfa->choice_join(children_[i]->build_nfa(s,top).pointer());
    return nfa;
}

RefCntPtr<Nfa> ChoiceConnector::build_nfa(Schema* s, FsmMatcher* top)
{
    if (!children_.size())
        return new Nfa(top);
    if (minOccur() == 1 && maxOccur() == 1)
        return build_single_nfa(s, top);
    RefCntPtr<Nfa> nfa;
    if (minOccur() == 0) {
        nfa = build_single_nfa(s, top);
        if (maxOccur() == 1) {
            nfa->make_occurences(1, 0);
            return nfa;
        }
        if (maxOccur() == unbounded) {
            nfa->make_occurences(1, 1);
            return nfa;
        }
    }
    uint i;
    nfa = new Nfa(top);
    RefCntPtr<Nfa> nfa2;
    for (i = 0; i < minOccur(); ++i)
        nfa->seq_join(build_single_nfa(s, top).pointer());
    if (maxOccur() == unbounded) {
        nfa->make_occurences(0, 1);
        return nfa;
    }
    for (i = minOccur(); i < maxOccur(); ++i) {
        nfa2 = build_single_nfa(s, top);
        nfa2->make_occurences(1, 0);
        nfa->seq_join(nfa2.pointer());
    }
    return nfa;
}

RefCntPtr<Nfa> GroupConnector::build_nfa(Schema* s, FsmMatcher* top)
{
    Connector* c = grp_->connector(0);
    if (0 == c)
        return new Nfa(top);
    return quantize_nfa(s, top, c);
}

RefCntPtr<Nfa> SequenceConnector::build_single_nfa(Schema* s, FsmMatcher* top)
{
    RefCntPtr<Nfa> nfa = children_[0]->build_nfa(s, top);
    for (ulong i = 1; i < children_.size(); ++i)
        nfa->seq_join(children_[i]->build_nfa(s, top).pointer());
    return nfa;
}

RefCntPtr<Nfa> SequenceConnector::build_nfa(Schema* s, FsmMatcher* top)
{
    if (!children_.size())
        return new Nfa(top);
    RefCntPtr<Nfa> nfa = build_single_nfa(s, top);

    if (minOccur() == 0) {
        if (maxOccur() == 1) {
            nfa->make_occurences(1, 0);
            return nfa;
        }
        if (maxOccur() == unbounded) {
            nfa->make_occurences(1, 1);
            return nfa;
        }
    }
    uint i;
    RefCntPtr<Nfa> nfa2;
    for (i = 1; i < minOccur(); ++i) {
        nfa2 = build_single_nfa(s, top);
        nfa->seq_join(nfa2.pointer());
    }
    if (maxOccur() == unbounded) {
        nfa->make_occurences(0, 1);
        return nfa;
    }
    for (i = minOccur(); i < maxOccur(); ++i) {
        nfa2 = build_single_nfa(s, top);
        nfa2->make_occurences(1, 0);
        nfa->seq_join(nfa2.pointer());
    }
    return nfa;
}

RefCntPtr<Nfa> WildcardParticle::build_nfa(Schema*, FsmMatcher* top)
{
    return buildTokenNfa(top, new WildcardToken(wildcard(), this));
}

RefCntPtr<Nfa> GeneratedParticle::build_nfa(Schema*, FsmMatcher*)
{
    RT_MSG_ABORT("GeneratedParticle:: build_nfa?");
    return 0;
}

RefCntPtr<Nfa> Particle::buildTokenNfa(FsmMatcher* top,
                                       ContentToken* tok)
{
    if (minOccur() == 1 && maxOccur() == 1)
        return new Nfa(top, tok);
    RefCntPtr<Nfa> nfa;
    if (minOccur() == 0) {
        if (maxOccur() == 1) {
            nfa = new Nfa(top, tok);
            nfa->make_occurences(1, 0);
            return nfa;
        }
        if (maxOccur() == Particle::unbounded) {
            nfa = new Nfa(top, tok);
            nfa->make_occurences(1, 1);
            return nfa;
        }
    }
    uint i;
    nfa = new Nfa(top);
    RefCntPtr<Nfa> nfa2;
    for (i = 0; i < minOccur(); ++i)
        nfa->seq_join(new Nfa(top, tok));
    if (maxOccur() == Particle::unbounded) {
        nfa->make_occurences(0, 1);
        return nfa;
    }
    for (i = minOccur(); i < maxOccur(); ++i) {
        nfa2 = new Nfa(top, tok);
        nfa2->make_occurences(1, 0);
        nfa->seq_join(nfa2.pointer());
    }
    return nfa;
}

RefCntPtr<Nfa> ElementParticle::build_nfa(Schema*, FsmMatcher* top)
{
    return buildTokenNfa(top, token().pointer());
}

RefCntPtr<Nfa> ChoiceParticle::build_nfa(Schema*, FsmMatcher*)
{
    return 0;
}

RefCntPtr<Nfa> ChoiceParticle::quantize_nfa(Schema*, FsmMatcher*, Particle*)
{
    return 0;
}

//////////////////////////////////////////////////////////////////////

bool FsmMatcher::exactMatch(Schema* schema,
                            InputTokenSequence& iseq,
                            MatchState& ms) const
{
    return match(schema, iseq, ms, 0, 0);
}

bool FsmMatcher::match(Schema* schema, InputTokenSequence& iseq,
                       MatchState& ms, const DfaState* startState,
                       const DfaState** endState) const
{
    ulong errors = 0;
    ulong iPos = ms.iseqStart;
    ulong iLen = iseq.size();
    const DfaState* state = startState ? startState : dfa_->startState_;
    const ContentToken* ct;
    const Dfa::dfaTranList* dl = &state->trans;
    Dfa::dfaTranList::const_iterator ti = dl->begin();
    const DfaTrans* endTrans = 0;

    while (iPos < iLen) {
        if (iPos >= ms.stopAt) {
            if (endState)
                *endState = state;
            return (0 == errors);
        }
        ct = iseq[iPos].token.pointer();
        endTrans = 0;
        dl = &state->trans;
        for (ti = dl->begin(); ti != dl->end(); ++ti) {
            if (ti->token->type() == ContentToken::SEQ_END_TOKEN) {
                endTrans = &(*ti);
                continue;
            }
            if (ct->match(ti->token)) {
                iseq[iPos].matchedParticle = ti->token->particle();
                if (ms.editMode) 
                    XsNodeExt::set(iseq[iPos].element, state);
                state = ti->nstate;
                ++iPos;
                ++ms.matchedTokens;
                goto next;
            }
        }
        if (ms.quickcheck) {
            if (endState)
                *endState = state;
            return false;
        }
        // no match: unexpected token
        schema->mstream() << XsMessages::unexpectedToken
            << Message::L_ERROR << ct->format()
            << DVXS_ORIGIN(iseq[iPos].element, topConnector_);
        ++errors;
        {
            DfaPathGen pgen;
            DfaPathGen::dfaPath path;
            if (pgen.find(state, ct, path) && path.begin() != path.end()) {
                 String possible_tokens;
                 dl = &state->trans;
                 ti = dl->begin();
                 for (; ti != dl->end(); ++ti)
                    if (ti->token->type() != ContentToken::SEQ_END_TOKEN)
                        possible_tokens += ti->token->format() + ' ';
                // in edit mode, find this token and set correct
                // DFA state for it
                if (ms.editMode) {
                    DfaPathGen::dfaPath::reverse_iterator pp =
                        path.rbegin();
                    XsNodeExt::set(iseq[iPos].element,(*pp)->nstate);
                } else
                    schema->mstream() << XsMessages::possibleTokensAre
                        << Message::L_INFO << possible_tokens
                        << DVXS_ORIGIN(iseq[iPos].element, topConnector_);
                state = path.back()->nstate;
            } else
                ++iPos;
        }
next:;
    }
    if (endState)
        *endState = state;
    if (ms.quickcheck)
        return true;
    if (endTrans)
        return (0 == errors);
    for (; ti != dl->end(); ++ti) {
        if (ti->token->type() == ContentToken::SEQ_END_TOKEN)
            return (0 == errors);
    }
    dl = &state->trans;
    for (ti = dl->begin(); ti != dl->end(); ++ti) {
        if (ti->token->type() == ContentToken::SEQ_END_TOKEN)
            return (0 == errors);
    }
    if (ms.editMode && !errors) {
        while (state->trans.size() == 1 &&
            state->trans[0].token->type() == ContentToken::QNAME_TOKEN) {
                iseq.insertBefore(state->trans[0].token->particle(),
                    iPos++, state);
                state = state->trans[0].nstate;
        }
        dl = &state->trans;
        for (ti = dl->begin(); ti != dl->end(); ++ti)
            if (ti->token->type() == ContentToken::SEQ_END_TOKEN)
                break;
        if (ti != dl->end())
            return true;
        RefCntPtr<ChoiceParticle> dpp =
            new ChoiceParticle(topConnector_->origin());
        int added_elems = 0;
        for (ti = dl->begin(); ti != dl->end(); ++ti)
            if (ti->token->type() == ContentToken::QNAME_TOKEN) {
                PropertyNode* pn = ti->token->particle()->makeElemProperty();
                if (pn)
                    dpp->addElem(pn);
                ++added_elems;
            }
        if (added_elems > 0)
            iseq.insertBefore(dpp.pointer(), iPos++, state);
        return true;
    } else
        schema->mstream() << XsMessages::prematureEndOfSeq
            << Message::L_ERROR << DVXS_ORIGIN(iseq.parent(), topConnector_);
    return false;
}

// Returns next state for given input token itok
static const FsmMatcher::DfaState*
next_state(const FsmMatcher::DfaState* state, const ContentToken& itok)
{
    const FsmMatcher::Dfa::dfaTranList& dl = state->trans;
    FsmMatcher::Dfa::dfaTranList::const_iterator ti = dl.begin();
    for (; ti != dl.end(); ++ti) {
        if (ti->token->type() == ContentToken::SEQ_END_TOKEN)
            continue;
        if (ti->token->match(&itok))
            return ti->nstate;
    }
    return 0;
}

// Algorithm: first find last child of specified node
// - if not found: use node XsElement to determine matcher, and then
//   ask for possible first tokens from the matcher.
// - if found: use dfaState for this node to determine next dfaState,
//   and list all transitions from next dfaState (this is always correct
//   because this is the last element).
//
void DfaXsNodeExt::getElemListAppend(PropertyNode* el,
                                     const GROVE_NAMESPACE::Node* pnode) const
{
    // trying to find last element child of 'elem'
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
    GROVE_NAMESPACE::Element* e =
        static_cast<GROVE_NAMESPACE::Element*>(lastc);

    NodeExt* ext = lastc->nodeExt();
    if (!ext)
        return;
    XsNodeExt* xs_ext = ext->asXsNodeExt();
    if (!xs_ext)
        return;
    DfaXsNodeExt* dfa_ext = xs_ext->asDfaXsNodeExt();

    if (!dfa_ext) {
        if(xs_ext->asChoiceXsNodeExt()) {
            PropertyNode* pn =
                xs_ext->asChoiceXsNodeExt()->elist()->firstChild();
            for (; pn; pn = pn->nextSibling())
                el->appendChild(pn->copy(true));
            return;
        }
        else if (xs_ext->asAllXsNodeExt()) {
            xs_ext->asAllXsNodeExt()->getElemListBefore(el, lastc);
            return;
        }
    }
    if (0 == dfa_ext->dfaState())
        return;
    const FsmMatcher::DfaState* ds =
        next_state(dfa_ext->dfaState(), InputToken(e));
    if (0 == ds)
        return;
    const FsmMatcher::Dfa::dfaTranList& dl = ds->trans;
    FsmMatcher::Dfa::dfaTranList::const_iterator ti = dl.begin();
    for (; ti != dl.end(); ++ti) {
        if (ti->token->type() != ContentToken::QNAME_TOKEN)
            continue;
        PropertyNode* pn = ti->token->particle()->makeElemProperty();
        if (pn)
            el->appendChild(pn);
        //el.push_back(static_cast<const QnameToken*>(ti->token)->ename());
    }
}

//
// Algorithm: take all transitions except one which correspond to the
// current element. This will result in tokens which can replace current
// element.
//
void
DfaXsNodeExt::getElemListReplace(PropertyNode* el,
                                 const GROVE_NAMESPACE::Element* elem) const
{
    // make sure it is not root element
    if (0 == dfaState_)
        return;
    const FsmMatcher::Dfa::dfaTranList& dl = dfaState_->trans;
    FsmMatcher::Dfa::dfaTranList::const_iterator ti = dl.begin();
    InputToken itok(const_cast<GROVE_NAMESPACE::Element*>(elem));
    for (; ti != dl.end(); ++ti) {
        if (ti->token->type() != ContentToken::QNAME_TOKEN)
            continue;
        if (ti->token->match(&itok))   // everything except myself
            continue;
        PropertyNode* pn = ti->token->particle()->makeElemProperty();
        if (pn)
            el->appendChild(pn);
    }
}

// Algorithm: if specified node is the last child element of it's parent,
// and has no following element, use getElemListAppend algorithm.
// Otherwise:
// - determine next-state ("my" state)
// - if next-element (relative to the specified node) matches any
//   transitions in this state, collect tokens from transitions whose stateId
//   is less than stateId of previously determined state (this corresponds
//   to the "back references" in the DFA for multiple occurences);
// - if not, just collect all tokens.
//
// TODO: improve algorithm for handling back-references. When we insert
//       element whose transition points "back", then we can also
//       auto-generate all elements until current dfaState (stop and
//       generate ChoiceElement at the first ambiguity).
//

void DfaXsNodeExt::getElemListBefore(PropertyNode* el,
                                     const GROVE_NAMESPACE::Node* pnode) const
{
    // trying to find previos sibling element to call getElemListAfter
    Node* prev = const_cast<Node*>(pnode->prevSibling());
    while (prev && prev->nodeType() != Node::ELEMENT_NODE)
        prev = prev->prevSibling();
    const Node* next = pnode->nextSibling();
    while (next && next->nodeType() != GROVE_NAMESPACE::Node::ELEMENT_NODE)
        next = next->nextSibling();
    if (0 != prev) {
        NodeExt* ext = prev->nodeExt();
        if (!ext)
            return;
        XsNodeExt* xs_ext = ext->asXsNodeExt();
        if (!xs_ext)
            return;
        DfaXsNodeExt* dfa_ext = xs_ext->asDfaXsNodeExt();
        if (!dfa_ext && xs_ext->asChoiceXsNodeExt()) {
            PropertyNode* pn =
                xs_ext->asChoiceXsNodeExt()->elist()->firstChild();
            for (; pn; pn = pn->nextSibling())
                el->appendChild(pn->copy(true));
            return;
        }
        if (0 == dfa_ext->dfaState())
            return;
        const FsmMatcher::DfaState* ds =
            next_state(dfa_ext->dfaState(), InputToken(const_cast<Element*>
                (static_cast<const Element*>(prev))));
        if (!ds)
            return;
        const FsmMatcher::Dfa::dfaTranList* dl = &ds->trans;
        FsmMatcher::Dfa::dfaTranList::const_iterator ti = dl->begin();
        for (; ti != dl->end(); ++ti) {
            if (ti->token->type() != ContentToken::QNAME_TOKEN)
                continue;
            FsmMatcher::DfaPathGen pgen;
            FsmMatcher::DfaPathGen::dfaPath path;
            InputToken itok(const_cast<Element*>
                (static_cast<const Element*>(pnode)));
            if (next_state(ti->nstate, itok) ||
                pgen.find(ti->nstate, &itok, path)) {
                PropertyNode* pn = ti->token->particle()->makeElemProperty();
                if (pn)
                    el->appendChild(pn);
            }
        }
        return;
    }
    Element* parent = ELEMENT_CAST(pnode->parent());
    if (!parent)
        return;
    NodeExt* ext = parent->nodeExt();
    if (!ext)
        return;
    XsNodeExt* xs_ext = ext->asXsNodeExt();
    if (0 == xs_ext)
        return;
    const Matcher* m = xs_ext->getMatcher();
    if (0 == m)
        return;
    if (pnode->nodeType() != GROVE_NAMESPACE::Node::ELEMENT_NODE &&
        0 == prev && 0 == next) {
            m->listFirstElems(el);
            return;
    }
    m->listFirstElems(el, static_cast<const Element*>(pnode));
}

// Lists possible first elements in the DFA
//
void FsmMatcher::listFirstElems(PropertyNode* el,
                                const GroveLib::Element* node) const
{
    const FsmMatcher::Dfa::dfaTranList& dl = dfa_->startState_->trans;
    FsmMatcher::Dfa::dfaTranList::const_iterator ti = dl.begin();
    if (0 == node) {
        for (; ti != dl.end(); ++ti) {
            if (ti->token->type() != ContentToken::QNAME_TOKEN)
                continue;
            PropertyNode* pn = ti->token->particle()->makeElemProperty();
            if (pn)
                el->appendChild(pn);
        }
        return;
    }
    MatchState ms;
    ms.quickcheck = true;
    InputTokenSequence iseq;
    ElementPtr ep = new GroveLib::Element(node->nodeName());
    iseq.append(ep.pointer());
    InputTokenSequence::token_entry& te = iseq[0];
    te.matchedParticle = 0;
    iseq.build(0, ELEMENT_CAST(node->parent()),
        InputTokenSequence::cdataAllowed, 0);
    for (; ti != dl.end(); ++ti) {
        if (ti->token->type() != ContentToken::QNAME_TOKEN)
            continue;
        te.token = static_cast<QnameToken*>(ti->token);
        if (exactMatch(0, iseq, ms)) {
            PropertyNode* pn = ti->token->particle()->makeElemProperty();
            if (pn)
                el->appendChild(pn);
        }
    }
}

static const FsmMatcher::DfaTrans*
get_next_trans(const FsmMatcher::DfaState* ds, const GroveLib::ExpandedName& n)
{
    const FsmMatcher::Dfa::dfaTranList& dl = ds->trans;
    FsmMatcher::Dfa::dfaTranList::const_iterator di = dl.begin();
    COMMON_NS::OwnerPtr<QnameToken> itok;
    itok = new QnameToken(n, 0);
    for (; di != dl.end(); ++di) {
        if (di->token->match(itok.pointer()))
            return &(*di);
    }
    return 0;
}

const XsElement*
DfaXsNodeExt::getNextElem(const GroveLib::ExpandedName& cur,
                          const GroveLib::ExpandedName& n) const
{
    const FsmMatcher::DfaTrans* t = get_next_trans(dfaState(), cur);
    if (0 == t)
        return 0;
    const FsmMatcher::DfaTrans* ts = get_next_trans(t->nstate, n);
    if (0 == ts)
        return 0;
    ElementParticle* ep = ts->token->particle()->asElementParticle();
    if (ep)
       return ep->element();
    return 0;
}

//////////////////////////////////////////////////////////////////

const XsElement*
FsmMatcher::getFirstElem(const GroveLib::ExpandedName& n) const
{
    const FsmMatcher::DfaTrans* ts = get_next_trans(dfa_->startState_, n);
    if (0 == ts)
        return 0;
    ElementParticle* ep = ts->token->particle()->asElementParticle();
    if (ep)
       return ep->element();
    return 0;
}

bool FsmMatcher::DfaPathGen::find(const DfaState* startState,
                                  const ContentToken* token,
                                  dfaPath& res)
{
    const DfaState* curState;
    color_.insert(startState);
    q_.push_front(startState);
    res.clear();
    while(!q_.empty()) {
        curState = q_.front();
        const dfaTranList& tl = curState->trans;
        dfaTranList::const_iterator ti = tl.begin();
        for (; ti != tl.end(); ++ti) {
            if (color_.find(ti->nstate) == color_.end()) { // not found = White
                if (ti->token->match(token))
                    goto found;
                color_.insert(ti->nstate);
                pg_[ti->nstate] = StateAndTrans(curState, &(*ti));
                q_.push_back(ti->nstate);
            }
            else if (startState == ti->nstate  &&
                     ti->token->match(token))
                return true;
        }
        q_.pop_front();
        color_.insert(curState);
    }
    return false;
found:
    return make_predpath(res, startState, curState);
}

bool FsmMatcher::DfaPathGen::make_predpath(dfaPath& res,
                                           const DfaState* s,
                                           const DfaState* v)
{
    if (s == v)
        return true;
    predGraph::const_iterator it = pg_.find(v);
    if (it == pg_.end())
        return false;
    if (!make_predpath(res, s, it->second.state))
        return false;
    res.push_back(it->second.trans);
    return true;
}

XS_NAMESPACE_END
