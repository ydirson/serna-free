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
#include "xslt/PatternExpr.h"
#include "xslt/Result.h"
#include "xslt/impl/instructions/Number.h"
#include "xslt/impl/instances/NumberInst.h"
#include "xslt/impl/instances/SharedData.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/patterns.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "common/Singleton.h"
#include "common/XTreeIterator.h"
#include <set>

using namespace Common;

namespace Xslt {

struct PatternMatch {
    XSLT_OALLOC(PatternMatch);
    PatternMatch(GroveLib::Node* node,
                 const PatternExpr& pattern,
                 const Xpath::ExprContext& ectx)
        : nsi_(node)
    {
          pinst_ = pattern.makeInst(nsi_, ectx);
    }
    Xpath::NodeSetItem      nsi_;
    Xpath::ValueHolderPtr   pinst_;
};

class PatternMatchList : public OwnerVector<PatternMatch> {};

static inline NumberNodeCounter*
make_node_counter(const PatternExprImpl& pattern,
                  GroveLib::Node* node,
                  const Xpath::ExprContext& exprContext,
                  bool modified)
{
    OwnerPtr<WatchedNodeCounter> wcnt(new WatchedNodeCounter
        (pattern, node, exprContext, modified));
    if (!wcnt->hasWatchers())
        return new NumberNodeCounter(node, true, modified);
    return wcnt.release();
}

// returns number of closest previous match
uint NodeCounterWatcher::getPos() const
{
    if (!parent())
        return 0;
    nodeCounter()->levelCounter()->update();
    for (NumberNodeCounter* nc = nodeCounter(); nc; nc = nc->prevSibling())
        if (nc->matches())
            return nc->pos();
    return 0; // we are first, or no other matches
}

NodeCounterWatcher::~NodeCounterWatcher()
{
    if (!parent())
        return;
    NumberLevelCounter* const lc = nodeCounter()->levelCounter();
    if (!lc->decRefCnt())
        delete lc;
}

NumberLevelCounter::NumberLevelCounter(const GroveLib::Node* parent,
                                       const Xpath::ExprContext& exprContext,
                                       const PatternExprImplPtr& pattern)
    : parent_(parent),
      countPattern_(pattern),
      modified_(false)
{
    DBG(XSLT.NUMBER) << "NumberLevelCounter cons: " << this
        << ", parent: " << std::endl;
    DBG_EXEC(XSLT.NUMBER, parent->dump());

    GroveLib::Node* n = parent->firstChild();
    uint pos = 0;
    for (; n; n = n->nextSibling()) {
        if (pattern->isApplicable(n)) {
            OwnerPtr<NumberNodeCounter> counter(
                make_node_counter(*pattern, n, exprContext, false));
            if (counter->matches())
                ++pos;
            counter->setPos(pos);
            counters_.appendChild(counter.release());
            DBG(XSLT.NUMBER) << "LevelCounter: Created match, pos="
                << pos << " for node: " << n->nodeName() << std::endl;
        }
    }
    const_cast<GroveLib::Node*>(parent)->registerNodeVisitor(this,
        GroveLib::GroveVisitor::NOTIFY_CHILD_INSERTED|
        GroveLib::GroveVisitor::NOTIFY_CHILD_REMOVED);
}

NumberNodeCounter*
NumberLevelCounter::getNodeCounter(const GroveLib::Node* n) const
{
    NumberNodeCounter* nc = counters_.firstChild();
    for (; nc; nc = nc->nextSibling())
        if (nc->node() == n)
            return nc;
    return 0;
}

NumberLevelCounter::~NumberLevelCounter()
{
    DBG(XSLT.NUMBER) << "~NumberLevelCounter: " << this << std::endl;
    while (counters_.firstChild())
        counters_.firstChild()->destroy();
}

void NumberLevelCounter::childInserted(const GroveLib::Node* node)
{
    if (!countPattern().isApplicable(node))
        return;
    DBG(XSLT.NUMBER) << "LevelCounter(" << this << "): got appl INS node: "
        << node << ": " << node->nodeName() << std::endl;
    // find position for insertion
    setModified();
    OwnerPtr<NumberNodeCounter> counter(make_node_counter(countPattern(),
        const_cast<GroveLib::Node*>(node), Xpath::ExprContext(), true));
    NumberNodeCounter* const first_counter = counters_.firstChild();
    if (!node->nextSibling()) {
        if (first_counter)
            first_counter->notifyFirstClient();
        counters_.appendChild(counter.release());
        return;
    }
    if (!node->prevSibling()) {
        if (first_counter) {
            first_counter->insertBefore(counter.release());
            first_counter->notifyFirstClient();
            DBG(XSLT.NUMBER) << "Notifying clients of nc="
                << first_counter << std::endl;
        } else
            counters_.appendChild(counter.release());
        return;
    }
    const GroveLib::Node* cn = node->parent()->firstChild();
    NumberNodeCounter* cp = counters_.firstChild();
    while (cp) {        // don't forget to update counters
        for (;;) {
            if (0 == cn)
                break;
            if (cn == cp->node()) {
                cn = cn->nextSibling();
                cp = cp->nextSibling();
                break;
            }
            if (cn == node) {
                DBG(XSLT.NUMBER) << "Number: NodeCounter inserted before "
                    << cp->node()->nodeName() << std::endl;
                cp->insertBefore(counter.release());
                cp->notifyFirstClient();
                return;
            }
            cn = cn->nextSibling();
        }
        if (0 == cn)
            break;
    }
    DBG(XSLT.NUMBER) << "Number: NodeCounter appended\n";
    counters_.appendChild(counter.release());
    if (first_counter)
        first_counter->notifyFirstClient();
}

void NumberLevelCounter::childRemoved(const GroveLib::Node*,
                                      const GroveLib::Node* child)
{
    if (!countPattern().isApplicable(child))
        return;
    DBG(XSLT.NUMBER) << "NumberLevelCounter: appl REMOVED node: "
        << child << ": " << child->nodeName() << std::endl;
    NumberNodeCounter* nc = counters_.firstChild();
    for (; nc; nc = nc->nextSibling()) {
        if (nc->node() == child) {
            if (nc->prevSibling())
                nc->prevSibling()->setModified();
            else if (nc->nextSibling())
                nc->nextSibling()->setModified();
            else
                setModified();
            nc->destroy();
            return;
        }
    }
}

// notifies all clients starting from first modified NC
void NumberLevelCounter::update()
{
    if (!modified_)
        return;
    modified_ = false;
    NumberNodeCounter* nc = counters_.firstChild();
    bool needsUpdate = false;
    uint pos = 0;
    for (; nc; nc = nc->nextSibling()) {
        if (nc->matches())
            ++pos;
        if (nc->isModified() || nc->pos() != pos)
            needsUpdate = true;
        if (needsUpdate) {
            nc->setNotModified();
            nc->setPos(pos);
            nc->notifyClients();
        }
    }
}

///////////////////////////////////////////////////////////////////////

void NumberNodeCounter::destroy()
{
    if (simple_)
        delete this;
    else
        delete static_cast<WatchedNodeCounter*>(this);
}

WatchedNodeCounter::WatchedNodeCounter(const PatternExprImpl& pattern,
                                       GroveLib::Node* node,
                                       const Xpath::ExprContext& ectx,
                                       bool modified)
    : NumberNodeCounter(node, false, modified)
{
    if (!modified) {
        Xpath::NodeSetItem nsi(node);
        Xpath::ValueHolderPtr vh(0, pattern.makeInst(nsi, ectx));
        matches_ = vh->value()->getBool();
    } else
        matches_ = false;
}

void WatchedNodeCounter::notifyChanged(const SubscriberPtrBase*)
{
    setModified();
}

///////////////////////////////////////////////////////////////////////

static const PatternExprImpl*
matched_alt_expr(const Xpath::ValueHolderPtr& vp)
{
    RT_ASSERT(dynamic_cast<const PatternInst*>(vp.pointer()));
    return static_cast<const PatternExprImpl*>(
        &static_cast<const PatternInst&>(*vp).expr());
}

////////////////////////////////////////////////////////////////////

NumberFormatItem* NumberCache::getFormat(const Common::String& str)
{
    NumberFormatItem* fmt_item = formats_.firstChild();
    for (; fmt_item; fmt_item = fmt_item->nextSibling())
        if (fmt_item->format() == str)
            return fmt_item;
    fmt_item = new NumberFormatItem(str);
    formats_.appendChild(fmt_item);
    return fmt_item;
}

void NumberCache::getCounterWatcher(NodeCounterWatcher* watchp,
                                    const PatternExprImpl* pattern,
                                    GroveLib::Node* node,
                                    const Xpath::ExprContext& ectx)
{
    // 1. get/create appropriate level counter
    NumberLevelCounter* lcnt = levelCounters_.firstChild();
    for (; lcnt; lcnt = lcnt->nextSibling()) {
        if (lcnt->parent() != node->parent())
            continue;
        if (!(lcnt->countPattern() == *pattern))
            continue;
        DBG(XSLT.NUMBER) << "NumberCache: found existing LC for patternExpr";
        DBG_EXEC(XSLT.NUMBER, lcnt->countPattern().dump());
        break;
    }
    if (0 == lcnt) {
        DBG(XSLT.NUMBER) << "NumberCache: created LC for patternExpr:";
        DBG_EXEC(XSLT.NUMBER, pattern->dump());
        lcnt = new NumberLevelCounter(node->parent(), ectx,
            const_cast<PatternExprImpl*>(pattern));
        levelCounters_.appendChild(lcnt);
    }
    NumberNodeCounter* nc = lcnt->getNodeCounter(node);
    DBG(XSLT.NUMBER) << "NumberInst: watcher added for nc " << nc << std::endl;
    if (!nc) {
        lcnt->childInserted(node);
        nc = lcnt->getNodeCounter(node);
    }
    if (nc)
        nc->addWatcher(watchp);
}

NumberCache::NumberCache()
{
}

NumberCache::~NumberCache()
{
}

////////////////////////////////////////////////////////////////////

static bool descendant_of(const GroveLib::Node* n,
                          const GroveLib::Node* p)
{
    while (n && n != p) {
        if (n->parent() == p)
            return true;
        n = n->parent();
    }
    return false;
}

// todo: implement correct handling of expanded names. now only local
// names are considered (for simplicity)
static PatternExprPtr make_count_pattern(const GroveLib::Node* cn,
                                         Common::String& patString,
                                         const NumberInstructionBase* instr)
{
    switch (cn->nodeType()) {
        case GroveLib::Node::ATTRIBUTE_NODE:
            patString = "@" + cn->nodeName();
            break;
        case GroveLib::Node::ELEMENT_NODE:
            patString = cn->nodeName();
            break;
        case GroveLib::Node::COMMENT_NODE:
            patString = "comment()";
            break;
        case GroveLib::Node::PI_NODE: {
            const GroveLib::ProcessingInstruction* pi =
                static_cast<const GroveLib::ProcessingInstruction*>(cn);
            patString = "processing-instruction(";
            if (!pi->target().isEmpty())
                patString += "'" + pi->target() + "'";
            patString += ")";
            break;
        }
        case GroveLib::Node::TEXT_NODE:
            patString = "text()";
            break;

        default:
            patString = "node()";
            break;
    }
    return PatternExpr::makePattern(patString,
        instr->element(), instr->functionFactory());
}

static inline void select_ancestors(GroveLib::Node* cn,
                                    const PatternExpr& pe,
                                    const Xpath::ExprContext& econtext,
                                    PatternMatchList& froml,
                                    bool first_only)
{
    for (; cn->parent(); cn = cn->parent()) {
        if (!pe.isApplicable(cn))
            continue;
        OwnerPtr<PatternMatch> pm(new PatternMatch(cn, pe, econtext));
        if (!pm->pinst_->value()->getBool())
            continue;
        froml.push_back(pm.release());
        DBG(XSLT.NUMBER) << "NumberInst/selectAnc: created match for node: "
            << cn->nodeName() << std::endl;
        if (first_only && froml.size())
            break;
    }
}

NumberClientBase::NumberClientBase(const NumberInstructionBase* instr,
                                   const GroveLib::Node* context)
{
    String countPatternString;
    if (instr->countPattern().isNull())
        countPattern_ = make_count_pattern(context,
            countPatternString, instr); // "sample"
    else {
        countPattern_ = instr->countPattern();
        countPatternString = instr->countPatternString();
    }
    DBG(XSLT.NUMBER) << "NumberInst: pattern-string <" << countPatternString
        << ">" << std::endl;
}

NumberClient::NumberClient(const NumberInstructionBase* instr,
                           GroveLib::Node* context,
                           NumberCache* cache,
                           const Xpath::ExprContext& econtext)
    : NumberClientBase(instr, context)
{
    if (0 == cache)
        cache = &SingletonHolder<NumberCache>::instance(); // for debug
    const GroveLib::Node* from = 0;
    if (!instr->fromPattern().isNull()) {
        PatternMatchList froml;
        froml.reserve(16);
        select_ancestors(context, *instr->fromPattern(), econtext, froml, true);
        if (!froml.size())
            return;
        from = froml[0]->nsi_.node();
        DBG(XSLT.NUMBER) << "NumberInst::selectedFrom " <<
            from->nodeName() << std::endl;
    }
    PatternMatchList cvec;
    Vector<PatternMatch*> mv;
    cvec.reserve(64);
    select_ancestors(context, *countPattern_, econtext, cvec,
        instr->level() == Number::SINGLE);
    mv.reserve(cvec.size());
    for (uint i = 0; i < cvec.size(); ++i) {
        if (from && !descendant_of(cvec[i]->nsi_.node(), from))
            continue;
        mv.push_back(cvec[i]);
    }
    levels_.resize(mv.size());
    int mvidx = mv.size() - 1;
    for (uint i = 0; i < mv.size(); ++i, --mvidx) {
        levels_[i].setClient(this);
        cache->getCounterWatcher(&levels_[i],
            matched_alt_expr(mv[mvidx]->pinst_),
            mv[mvidx]->nsi_.node(), econtext);
    }
    if (levels_.size() == 0)
        return;
    DBG(XSLT.NUMBER) << "Created " << levels_.size() << " LC's\n";
}

String NumberClient::toString() const
{
    NumberConverter::NumberList numberList;
    numberList.resize(levels_.size());
    for (uint i = 0; i < levels_.size(); ++i)
        numberList[i] = levels_[i].getPos();
    String result;
    numberFormat()->toString(numberList, result);
    return result;
}

const NumberConverter* NumberClientBase::numberFormat() const
{
    return numberInstruction().numberFormat();
}

/////////////////////////////////////////////////////////////////////

NumberInstBase::NumberInstBase(const Number* number,
                       const InstanceInit& init, Instance* p)
    : ResultMakingInstance(number, init, p),
      NumberClient(number, nsiContext().node(),
        &topStylesheetInst()->sharedData().numberCache(), exprContext())
{
}

void NumberInstBase::update()
{
    if (GroveLib::Node::TEXT_NODE != result_.node()->nodeType())
        return;
    GroveLib::Text& t = *static_cast<GroveLib::Text*>(result_.node());
    String result = toString();
    if (result != t.data())
        t.setData(result);
}

const NumberInstructionBase& NumberInstBase::numberInstruction() const
{
    return *static_cast<const Number*>(instruction());
}

void NumberInstBase::numberChanged()
{
    DBG(XSLT.NUMBER) << "numberChanged, inst=" << this << std::endl;
    setModified();
}

NumberInstBase::~NumberInstBase()
{
    disposeResult();
    DBG(XSLT.NUMBER) << "NumberInst::~NumberInst\n";
}

void NumberInstBase::dump() const
{
    DDBG << "NumberInst: " << this << std::endl;
    Instance::dump();
}

Instance* Number::makeNumberInst(const InstanceInit& init, Instance* p) const
{
    if (level() == ANY)
        return new AnyNumberInst(this, init, p);
    if (formatAvt())
        return new NumberInstAvt(this, init, p);
    else
        return new NumberInst(this, init, p);
}

NumberInst::NumberInst(const Number* number,
                       const InstanceInit& init, Instance* p)
    : NumberInstBase(number, init, p)
{
    result_.insertBefore(new TextResult(toString(), 0, this),
        init.resultContext_.node_);
}

//////////////////////////////////////////////////////////////////////

NumberInstAvt::NumberInstAvt(const Number* number,
                             const InstanceInit& init, Instance* p)
    : NumberInstBase(number, init, p),
      formatAvt_(this, number->formatAvt()->valueExpr()->
        makeInst(nsiContext(), exprContext()))
{
    result_.insertBefore(new TextResult(toString(), 0, this),
        init.resultContext_.node_);
}

const NumberConverter* NumberInstAvt::numberFormat() const
{
    if (format_.isNull()) {
        NumberCache* cache = &topStylesheetInst()->sharedData().numberCache();
        format_ = cache->getFormat(formatAvt_->value()->getString());
    }
    return format_.pointer();
}

void NumberInstAvt::notifyChanged(const Common::SubscriberPtrBase*)
{
    format_ = 0;
    setModified();
}

////////////////////////////////////////////////////////////////////

AnyNumberInstBase::AnyNumberInstBase(const Number* number,
                                     const InstanceInit& init, Instance* p)
    : ResultMakingInstance(number, init, p),
      AnyNumberClient(number, nsiContext().node(),
        &topStylesheetInst()->sharedData().numberCache(), exprContext())
{
    DBG(XSLT.NUMBER) << "AnyNumberInstBase cons: " << this
        << ", num=" << toString() << std::endl;
    if (expr_entry_->isModified())
        expr_entry_->update();
}


void AnyNumberInstBase::update()
{
    clientUpdate(nsiContext().node(), exprContext());
    if (GroveLib::Node::TEXT_NODE != result_.node()->nodeType())
        return;
    GroveLib::Text& t = *static_cast<GroveLib::Text*>(result_.node());
    String result = toString();
    if (result != t.data())
        t.setData(toString());
}

const NumberInstructionBase& AnyNumberInstBase::numberInstruction() const
{
    return *static_cast<const Number*>(instruction());
}

void AnyNumberInstBase::numberChanged()
{
    DBG(XSLT.NUMBER) << "anyNumberChanged, inst=" << this << std::endl;
    setModified();
}

AnyNumberInstBase::~AnyNumberInstBase()
{
    disposeResult();
    DBG(XSLT.NUMBER) << "~AnyNumberInstBase: " << this << "\n";
}

void AnyNumberInstBase::dump() const
{
    DDBG << "AnyNumberInst: " << this << std::endl;
    Instance::dump();
}

AnyNumberInst::AnyNumberInst(const Number* number,
                             const InstanceInit& init, Instance* p)
    : AnyNumberInstBase(number, init, p)
{
    result_.insertBefore(new TextResult(toString(), 0, this),
        init.resultContext_.node_);
}

AnyNumberInstAvt::AnyNumberInstAvt(const Number* number,
                                  const InstanceInit& init, Instance* p)
    : AnyNumberInstBase(number, init, p),
      formatAvt_(this, number->formatAvt()->valueExpr()->
        makeInst(nsiContext(), exprContext()))
{
    result_.insertBefore(new TextResult(toString(), 0, this),
        init.resultContext_.node_);
}

const NumberConverter* AnyNumberInstAvt::numberFormat() const
{
    if (format_.isNull()) {
        NumberCache* cache = &topStylesheetInst()->sharedData().numberCache();
        format_ = cache->getFormat(formatAvt_->value()->getString());
    }
    return format_.pointer();
}

void AnyNumberInstAvt::notifyChanged(const Common::SubscriberPtrBase*)
{
    format_ = 0;
    setModified();
}

AnyNumberInstAvt::~AnyNumberInstAvt()
{
}

///////////////////////////////////////////////////////////////////////

static inline bool expr_eq(const PatternExprPtr& e1, const PatternExprPtr& e2)
{
    if (e2.isNull())
        return false;
    return static_cast<const PatternExprImpl&>(*e1) ==
        static_cast<const PatternExprImpl&>(*e2);
}

AnyNumberExprEntry*
NumberCache::getAnyExprEntry(const PatternExprPtr& from,
                             const PatternExprPtr& count,
                             GroveLib::GroveSectionRoot* gsr)
{
    AnyNumberExprEntry* ae = anyCounters_.firstChild();
    for (; ae; ae = ae->nextSibling()) {
        if (!from.isNull() && !expr_eq(from, ae->fromExpr()))
            continue;
        if (expr_eq(count, ae->countExpr())) {
            DBG(XSLT.NUMBER) << "NumberCache(" << this
                << "): found anyExprEntry: ";
            DBG_EXEC(XSLT.NUMBER, ae->countExpr()->dump());
            return ae;
        }
    }
    DBG(XSLT.NUMBER) << "NumberCache(" << this
        << "): Creating any-entry for: ";
    DBG_EXEC(XSLT.NUMBER, count->dump());
    ae = new AnyNumberExprEntry(from, count);
    anyCounters_.appendChild(ae);
    ae->build(gsr);
    return ae;
}

/////////////////////////////////////////////////////////////////////

typedef Common::XTreeWalkIterator<GroveLib::Node> DocOrderIterator;

AnyNumberClient::AnyNumberClient(const NumberInstructionBase* instr,
                           GroveLib::Node* context,
                           NumberCache* cache,
                           const Xpath::ExprContext& econtext)
    : NumberClientBase(instr, context)
{
    if (countPattern_.isNull())
        return;
    if (0 == cache)
        cache = &SingletonHolder<NumberCache>::instance();
    expr_entry_ = cache->getAnyExprEntry(instr->fromPattern(),
        countPattern_, context->getGSR());
    init(context, econtext, *instr);
}

void AnyNumberClient::init(GroveLib::Node* context,
                           const Xpath::ExprContext& econtext,
                           const NumberInstructionBase& instr)
{
    DBG(XSLT.NUMBER) << "AnyNumberClient: init(" << this << ")\n";
    PatternExprPtr from_pattern(instr.fromPattern());
    XTreeDfsIterator<GroveLib::Node> dit(context);   // go upwards
    Xpath::ValueHolderPtr cvp;
    for (; dit.node(); --dit) {
        DBG(XSLT.NUMBER) << "AnyNumberClient: traverse: "
            << dit.node()->nodeName() << " (" << dit.node() << ")" << std::endl;
        Xpath::NodeSetItem nsi(dit.node());
        if (countPattern_->isApplicable(dit.node())) {
            cvp = countPattern_->makeInst(nsi, econtext);
            if (cvp && cvp->value()->getBool()) {
                AnyCounter* counter = expr_entry_->findCounter(dit.node());
                if (counter) {
                    counter->clients().appendChild(this);
                    DBG(XSLT.NUMBER)
                        << "AnyNumberClient: adding self to client list of "
                        << counter << std::endl;
                    return;
                }
                // need to create/insert new counter here?
                DBG(XSLT.NUMBER) << "AnyNumberClient: counter not found\n";
                break;
            }
        }
        if (!from_pattern.isNull() && from_pattern->isApplicable(dit.node())) {
            cvp = from_pattern->makeInst(nsi, econtext);
            if (cvp && cvp->value()->getBool())
                break;
        }
    }
    DBG(XSLT.NUMBER) << "AnyNumberClient: attaching as zero-client\n";
    // no matches - attach to first counter
    expr_entry_->counters().firstChild()->clients().appendChild(this);
}

void AnyNumberClient::clientUpdate(GroveLib::Node* node,
                                   const Xpath::ExprContext& ectx)
{
    expr_entry_->update();
    if (ALI::parent())
        ALI::remove();
    init(node, ectx, numberInstruction());
}

///////////////////////////////////////////////////////////////////////

class AnyNumberExprEntry::RemovedNodes :
    public std::set<const GroveLib::Node*> {};

AnyNumberExprEntry::AnyNumberExprEntry(const PatternExprPtr& fromExpr,
                                       const PatternExprPtr& countExpr)
        : fromExpr_(fromExpr),
          countExpr_(countExpr),
          anyWatcher_(*this),
          removedNodes_(new RemovedNodes),
          gsr_(0),
          isModified_(false),
          isChanged_(false)
{
}

AnyCounter* AnyNumberExprEntry::findCounter(const GroveLib::Node* node) const
{
    AnyCounter* counter = counters_.firstChild();
    for (; counter; counter = counter->nextSibling())
        if (counter->node() == node)
            return counter;
    return 0;
}

struct AnyNumberExprEntry::BuildContext {
    BuildContext()
        : count(0), counter(0) {}
    uint            count;
    AnyCounter*     counter;
    void            dump(const char* str) const
    {
        DBG(XSLT.NUMBER) << "AnyNumber: " << str << ", BCTX(count=" << count;
        if (counter) {
            DBG(XSLT.NUMBER) << ", node=" << counter->node()->nodeName()
                << ", cpos=" << counter->pos();
        } else
            DBG(XSLT.NUMBER) << ",NULL-CNT";
        DBG(XSLT.NUMBER) << ")\n";
    }
};

inline bool AnyNumberExprEntry::matches(const PatternExprPtr& expr,
                                        const Xpath::NodeSetItem& nsi) const
{
    if (!expr->isApplicable(nsi.node()))
        return false;
    Xpath::ValueHolderPtr vh(0, expr->makeInst(nsi, *this));
    return vh->value()->getBool();
}


void AnyNumberExprEntry::build_dfs(GroveLib::Node* node, BuildContext& bctx)
{
    Xpath::NodeSetItem nsi(node);
    if (!fromExpr().isNull() && matches(fromExpr(), nsi))
        bctx.count = 0;
    if (matches(countExpr(), nsi))
        counters_.appendChild(new AnyCounter(nsi.node(), ++bctx.count));
    for (node = node->firstChild(); node; node = node->nextSibling())
        build_dfs(node, bctx);
}

inline void AnyNumberExprEntry::process_children(GroveLib::Node* node,
                                                 BuildContext& bctx)
{
    for (node = node->firstChild(); node; node = node->nextSibling())
        update_dfs(node, bctx);
}

static void reconnect_to_prev(AnyCounter*& acnt)
{
    AnyCounter* next = acnt->nextSibling();
    AnyCounter* prev = acnt->prevSibling();
    AnyNumberClient* client;
    while ((client = acnt->clients().firstChild())) {
        DBG(XSLT.NUMBER) << "Reconnecting client: " << client << std::endl;
        client->numberChanged();
        client->ALI::remove();
        prev->clients().appendChild(client);
    }
    delete acnt;
    acnt = next;
}

void AnyNumberExprEntry::update_dfs(GroveLib::Node* node, BuildContext& bctx)
{
    Xpath::NodeSetItem nsi(node);
    if (!fromExpr().isNull() && matches(fromExpr(), nsi))
        bctx.count = 0;
    const bool matched = matches(countExpr(), nsi);
    if (0 == bctx.counter) { // means last...
        if (matched) {
            DBG(XSLT.NUMBER) << "AnyUpd: match, clearing lastchild\n";
            counters_.lastChild()->numberChanged();
            counters_.lastChild()->clients().removeAllChildren();
            counters_.appendChild(new AnyCounter(nsi.node(), ++bctx.count));
            DBG(XSLT.NUMBER) << "No counter, append, node="
                << node->nodeName() << std::endl;
        }
        return process_children(node, bctx);
    }
    if (bctx.counter->node() == node) {
        if (matched) {
            DBG_IF(XSLT.NUMBER) bctx.dump("AnyUpd: eq nodes, match");
            bctx.counter->setPos(++bctx.count);
            bctx.counter = bctx.counter->nextSibling();
            return process_children(node, bctx);
        }
        // do not match -> remove current counter
        DBG_IF(XSLT.NUMBER) bctx.dump("eq node, no match");
        reconnect_to_prev(bctx.counter);
        if (bctx.counter)
            bctx.counter->setPos(bctx.count);
        return process_children(node, bctx);
    }
    if (matched) {
        AnyCounter* prev = bctx.counter->prevSibling();
        if (prev) {
            DBG(XSLT.NUMBER) << "AnyUpd: match, clearing prev\n";
            prev->numberChanged();
            prev->clients().removeAllChildren();
        }
        bctx.counter->insertBefore(new AnyCounter(nsi.node(), ++bctx.count));
        DBG(XSLT.NUMBER) << "AnyUpd: insertBefore, cnt="
            << bctx.count << std::endl;
    } else
        bctx.counter->setPos(bctx.count);
    process_children(node, bctx);
}

void AnyNumberExprEntry::update()
{
    if (!isModified_)
        return;
    isModified_ = false;
    DBG_IF(XSLT.NUMBER) {
        DBG(XSLT.NUMBER) << "BEFORE UPDATE:\n";
        AnyCounter* cnt = counters_.firstChild();
        for (; cnt; cnt = cnt->nextSibling()) {
            DBG(XSLT.NUMBER) << "    " << cnt->siblingIndex() << ": "
                << "node=" << cnt->node()->nodeName()
                << ", pos=" << cnt->pos()
                << ", nclients=" << cnt->clients().countChildren()
                << std::endl;
        }
    }
    if (removedNodes_->begin() != removedNodes_->end()) {
        DBG(XSLT.NUMBER) << "NumberAny: processing removed nodes\n";
        AnyCounter* acnt = counters_.firstChild()->nextSibling();
        uint removed_nodes = 0;
        while (acnt) {
            if (removedNodes_->find(acnt->node()) == removedNodes_->end()) {
                acnt->setPos(acnt->pos() - removed_nodes);
                acnt = acnt->nextSibling();
            } else {
                reconnect_to_prev(acnt);
                ++removed_nodes;
            }
        }
        removedNodes_->clear();
    }
    // process node removal here
    if (isChanged_) {
        DBG(XSLT.NUMBER) << "NumberAny: update_dfs\n";
        AnyNumberClient* client =
            counters_.firstChild()->clients().firstChild();
        for (; client; client = client->ALI::nextSibling())
            client->numberChanged();
        BuildContext bctx;
        bctx.counter = counters_.firstChild()->nextSibling();
        deregisterWatchers();
        GroveLib::Node* node = gsr_->firstChild();
        for (; node; node = node->nextSibling())
            update_dfs(gsr_, bctx);
        isChanged_ = false;
    }
}

void AnyNumberExprEntry::build(GroveLib::GroveSectionRoot* gsr)
{
    gsr_ = gsr;
    counters_.appendChild(new AnyCounter(gsr, 0));
    BuildContext bctx;
    build_dfs(gsr, bctx);
    anyWatcher_.setGSR(gsr);
}

static inline void notify_clients(const AnyCounterList& counters)
{
    AnyCounter* counter = counters.firstChild();
    for (; counter; counter = counter->nextSibling())
        if (counter->clients().firstChild())
            //counter->clients().firstChild()->numberChanged();
            break;
    if (counter)
        counter->clients().firstChild()->numberChanged();
}

void AnyNumberExprEntry::addRemovedNode(const GroveLib::Node* node)
{
    removedNodes_->insert(node);
    if (isModified_)
        return;
    isModified_ = true;
    notify_clients(counters_);
}

void AnyNumberExprEntry::notifyChanged(const SubscriberPtrBase*)
{
    isChanged_ = true;
    if (isModified_)
        return;
    isModified_ = true;
    notify_clients(counters_);
}

AnyNumberExprEntry::~AnyNumberExprEntry()
{
    DBG(XSLT.NUMBER) << "~AnyNumberEntry: " << this << std::endl;
}

void AnyNumberExprEntry::AnyWatcher::setGSR(GroveLib::GroveSectionRoot* gsr)
{
    gsr->registerVisitor(this);
}

AnyNumberExprEntry::AnyWatcher::~AnyWatcher()
{
    if (entry_.gsr())
        entry_.gsr()->deregisterVisitor(this);
}

inline static bool is_applicable(const AnyNumberExprEntry& e,
                                 const GroveLib::Node* n)
{
    if (!e.fromExpr().isNull() && e.fromExpr()->isApplicable(n))
        return true;
    if (e.countExpr()->isApplicable(n))
        return true;
    return false;
}

inline static bool is_applicable_rec(const AnyNumberExprEntry& e,
                                     const GroveLib::Node* n)
{
    if (is_applicable(e, n))
        return true;
    for (n = n->firstChild(); n; n = n->nextSibling())
        if (is_applicable_rec(e, n))
            return true;
    return false;
}

void AnyNumberExprEntry::AnyWatcher::childInserted(const GroveLib::Node* n)
{
    if (is_applicable_rec(entry_, n)) {
        DBG(XSLT.NUMBER) << "NumberAny: appl child inserted: "
            << n->nodeName() << std::endl;
        entry_.notifyChanged(0);
    }
}

inline static void process_remove_rec(AnyNumberExprEntry& e,
                                      const GroveLib::Node* n)
{
    if (is_applicable(e, n)) {
        DBG(XSLT.NUMBER) << "NumberAny: appl child removed: "
            << n->nodeName() << std::endl;
        e.addRemovedNode(n);
    }
    for (n = n->firstChild(); n; n = n->nextSibling())
        process_remove_rec(e, n);
}

void AnyNumberExprEntry::AnyWatcher::childRemoved(const GroveLib::Node*,
                                                  const GroveLib::Node* n)
{
    process_remove_rec(entry_, n);
}

/////////////////////////////////////////////////////////////////////

String AnyNumberClient::toString() const
{
    NumberConverter::NumberList numberList;
    String result;
    if (ALI::parent()) {
        const AnyCounter* const counter =
            XLIST_CONTAINING_PARENT_CLASS2(AnyCounter, clients_, ALI);
        numberList.push_back(counter->pos());
        numberFormat()->toString(numberList, result);
    } else
        result = NOTR("0");
    return result;
}

static inline bool notify_client(AnyNumberClient* client)
{
    if (!client || NumberInstructionBase::CT_XSLT !=
        client->numberInstruction().clientType())
            return false;
    client->numberChanged();
    DBG(XSLT.NUMBER) << "AnyNumber:: passed notification to "
        << client << std::endl;
    return true;
}

AnyNumberClient::~AnyNumberClient()
{
    DBG(XSLT.NUMBER) << "~AnyNumberClient: " << this << std::endl;
    if (!expr_entry_ || !expr_entry_->isModified() || !ALI::parent())
        return;
    DBG(XSLT.NUMBER) << "AnyNumberClient(" << this << "): pasing modstate\n";
    // the following is the trick to preserve modstate - somebody
    // should update expr_entry, even if modified instance is deleted.
    // Note that modstate should be updated only for clients of same type.
    AnyNumberClient* c = 0;
    for (c = ALI::nextSibling(); c; c = c->ALI::nextSibling())
        if (notify_client(c))
            return;
    for (c = ALI::prevSibling(); c; c = c->ALI::prevSibling())
        if (notify_client(c))
            return;
    AnyCounter* my_counter =
        XLIST_CONTAINING_PARENT_CLASS2(AnyCounter, clients_, ALI);
    AnyCounter* cnt = my_counter->nextSibling();
    for (; cnt; cnt = cnt->nextSibling())
        if (notify_client(cnt->clients().firstChild()))
            return;
    cnt = my_counter->prevSibling();
    for (; cnt; cnt = cnt->prevSibling())
        if (notify_client(cnt->clients().firstChild()))
            return;
}

} // namespace Xslt
