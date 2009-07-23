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

#ifndef XSLT_NUMBER_INST_H
#define XSLT_NUMBER_INST_H

#include "xslt/xslt_defs.h"
#include "xslt/NumberClient.h"
#include "xslt/impl/Instance.h"
#include "xslt/impl/PatternInst.h"
#include "xslt/impl/instances/ResultMakingInstance.h"
#include "xslt/impl/instructions/Number.h"
#include "xslt/TemplateSelectorBase.h"
#include "xpath/NodeSet.h"
#include "grove/NodeVisitor.h"
#include "common/SubscriberPtr.h"
#include "common/CDList.h"

namespace Xslt {

class NumberLevelCounter;

class NumberNodeCounter : public Common::XListItem<NumberNodeCounter> {
public:
    XSLT_OALLOC(NumberNodeCounter);
    typedef Common::XList<NodeCounterWatcher> ClientList;

    NumberNodeCounter(GroveLib::Node* node, bool simple, bool modified)
        : node_(node), pos_(1), 
          simple_(simple), matches_(true), modified_(modified) {}
    
    void                    destroy();  // must be called instead of delete
    NumberLevelCounter*     levelCounter() const;
    bool                    matches() const { return matches_; }
    uint                    pos() const { return pos_; }
    void                    setPos(uint pos) { pos_ = pos; }
    void                    setModified();
    bool                    isModified() const { return modified_; } 
    void                    notifyClients();
    void                    notifyFirstClient();
    void                    setNotModified() { modified_ = false; }
    void                    addWatcher(NodeCounterWatcher* w);
    GroveLib::Node*         node() const { return node_; }

protected:
    friend class NodeCounterWatcher;
    ClientList      clients_;
    GroveLib::Node* node_;
    uint            pos_      : 28;
    uint            simple_   : 1;
    uint            matches_  : 1;
    uint            modified_ : 1;
};

typedef Common::XList<NumberNodeCounter> NodeCounterList;

class WatchedNodeCounter : public NumberNodeCounter,
                           public TemplateSelectorBase {
public:
    XSLT_OALLOC(WatchedNodeCounter);
    WatchedNodeCounter(const PatternExprImpl&,
                       GroveLib::Node* node,
                       const Xpath::ExprContext&,
                       bool modified);
    void notifyChanged(const Common::SubscriberPtrBase*);
};

///////////////////////////////////////////////////////////////

class NumberLevelCounter : public Common::RefCounted<>, 
                           public Common::XListItem<NumberLevelCounter>,
                           public GroveLib::NodeVisitor {
public:
    XSLT_OALLOC(NumberLvlCounter);
    
    NumberLevelCounter(const GroveLib::Node* parent,
                       const Xpath::ExprContext& exprContext,
                       const PatternExprImplPtr& pattern);
    ~NumberLevelCounter();
    
    const PatternExprImpl&  countPattern() const { return *countPattern_; }
    void                    setModified() { modified_ = true; } 
    const GroveLib::Node*   parent() const { return parent_; }
    NumberNodeCounter*      getNodeCounter(const GroveLib::Node*) const;
    void                    update();

    // Reimplemented from NodeVisitor
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node* node);
    virtual void    childRemoved (const GroveLib::Node* node,
                                  const GroveLib::Node* child);
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}
    virtual void    textChanged(const GroveLib::Text*) {}

private:
    friend class NumberNodeCounter;

    const GroveLib::Node*       parent_;
    PatternExprImplPtr          countPattern_;
    NodeCounterList             counters_;
    bool                        modified_;
};

inline NumberLevelCounter* NumberNodeCounter::levelCounter() const
{
    return XLIST_CONTAINING_PARENT_CLASS(NumberLevelCounter, counters_);
}
    
inline NumberNodeCounter* NodeCounterWatcher::nodeCounter() const
{
    return XLIST_CONTAINING_PARENT_CLASS(NumberNodeCounter, clients_);
}
    
inline void NumberNodeCounter::setModified()
{
    if (!modified_) {
        modified_ = true;
        levelCounter()->setModified();
        notifyClients();
    }
}
    
inline void NumberNodeCounter::notifyFirstClient()
{
    if (clients_.firstChild())
        clients_.firstChild()->numberChanged();
}

inline void NumberNodeCounter::notifyClients()
{
    modified_ = false;
    NodeCounterWatcher* w = clients_.firstChild();
    for (; w; w = w->nextSibling())
        w->numberChanged();
}

inline void NumberNodeCounter::addWatcher(NodeCounterWatcher* w)
{
    clients_.appendChild(w);
    levelCounter()->incRefCnt();
}

// todo: pass nsi& to NumberClient, not just node

///////////////////////////////////////////////////////////////

class AnyNumberInst;

typedef Common::XList<AnyNumberClient> AnyClientList;

class AnyCounter : public Common::XListItem<AnyCounter> {
public:
    XSLT_OALLOC(AnyCounter);

    AnyCounter(const GroveLib::Node* node, uint pos)
        : node_(node), pos_(pos) {}
    const GroveLib::Node* node() const { return node_; }
    void                  numberChanged();
    AnyClientList&        clients() { return clients_; }
    uint                  pos() const { return pos_; }
    void                  setPos(uint pos);

private:
    friend class AnyNumberClient;

    const GroveLib::Node*   node_;
    uint                    pos_;
    AnyClientList           clients_;
};

typedef Common::OwnedXList<AnyCounter> AnyCounterList;

class AnyNumberExprEntry : public Common::RefCounted<>,
                           public Common::XListItem<AnyNumberExprEntry>,
                           public TemplateSelectorBase {
public:
    XSLT_OALLOC(AnyNumberExprEntry);
    class AnyWatcher : public GroveLib::GroveVisitorBase {
    public:
        XSLT_OALLOC(AnyWatcher);
        AnyWatcher(AnyNumberExprEntry& e)
            : entry_(e) {}
        ~AnyWatcher();

        void setGSR(GroveLib::GroveSectionRoot*);

        virtual void childInserted(const GroveLib::Node*);
        virtual void childRemoved(const GroveLib::Node*,
                                   const GroveLib::Node*);
        virtual void nodeDestroyed(const GroveLib::Node*) {}
        virtual void attributeChanged(const GroveLib::Attr*) {}
        virtual void attributeRemoved(const GroveLib::Element*,
                                      const GroveLib::Attr*) {}
        virtual void attributeAdded(const GroveLib::Attr*) {}
        virtual void textChanged(const GroveLib::Text*) {}

    private:
        AnyNumberExprEntry&         entry_;
    };
    AnyNumberExprEntry(const PatternExprPtr& fromExpr,
                       const PatternExprPtr& countExpr);
    ~AnyNumberExprEntry();

    const PatternExprPtr& fromExpr() const { return fromExpr_; }
    const PatternExprPtr& countExpr() const { return countExpr_; }

    AnyCounter*         findCounter(const GroveLib::Node* node) const;
    AnyCounterList&     counters() { return counters_; }

    void  build(GroveLib::GroveSectionRoot*);
    void  update();
    
    virtual void    notifyChanged(const Common::SubscriberPtrBase*);
    GroveLib::GroveSectionRoot* gsr() const { return gsr_; }
    bool            isModified() const { return isModified_; }
    void            addRemovedNode(const GroveLib::Node*);


private:
    AnyNumberExprEntry(const AnyNumberExprEntry&);
    AnyNumberExprEntry& operator=(const AnyNumberExprEntry&);

    class BuildContext;
    class RemovedNodes;

    void    build_dfs(GroveLib::Node*,  BuildContext&);
    void    update_dfs(GroveLib::Node*, BuildContext&);
    void    process_children(GroveLib::Node*, BuildContext&);
    bool    matches(const PatternExprPtr& expr,
                    const Xpath::NodeSetItem& nsi) const;

    PatternExprPtr              fromExpr_;
    PatternExprPtr              countExpr_;
    AnyCounterList              counters_;
    AnyWatcher                  anyWatcher_;
    Common::OwnerPtr<RemovedNodes> removedNodes_;
    GroveLib::GroveSectionRoot* gsr_;
    bool                        isModified_;
    bool                        isChanged_;
};

class NumberFormatItem : public Common::RefCounted<>,
                         public Common::XListItem<NumberFormatItem>,
                         public NumberConverter {
public:
    XSLT_OALLOC(NumberFormatItem);
    NumberFormatItem(const Common::String& str)
        : NumberConverter(str) {}
};

class NumberCache {
public:
    void    getCounterWatcher(NodeCounterWatcher*,
                              const PatternExprImpl* patternExpr,
                              GroveLib::Node* node,
                              const Xpath::ExprContext& exprContext);
    AnyNumberExprEntry* getAnyExprEntry(const PatternExprPtr& from,
                                        const PatternExprPtr& count,
                                        GroveLib::GroveSectionRoot*);
    NumberFormatItem* getFormat(const Common::String&);

    NumberCache();
    ~NumberCache();

private:
    NumberCache(const NumberCache&);
    NumberCache& operator=(const NumberCache&);

    Common::XList<NumberLevelCounter> levelCounters_;
    Common::XList<AnyNumberExprEntry> anyCounters_;
    Common::XList<NumberFormatItem> formats_;
};

///////////////////////////////////////////////////////////////

class NumberInstBase : public ResultMakingInstance,
                       public NumberClient {
public:
    NumberInstBase(const Number* number, const InstanceInit& init, Instance* p);
    virtual ~NumberInstBase();
    
    virtual void    numberChanged();
    virtual void    update();
    virtual void    dump() const;
    virtual const NumberInstructionBase& numberInstruction() const;
};

class NumberInst : public NumberInstBase {
public:
    XSLT_OALLOC(NumberInst);

    NumberInst(const Number* number, const InstanceInit& init, Instance* p);
    virtual ~NumberInst() {}
};

class NumberInstAvt : public NumberInstBase, 
                      public Common::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(NumberInstAvt);

    NumberInstAvt(const Number* number, const InstanceInit& init, Instance* p);
    virtual const NumberConverter* numberFormat() const;
    void          notifyChanged(const Common::SubscriberPtrBase*);

private:
    Xpath::ValueHolderPtr formatAvt_;
    mutable Common::RefCntPtr<NumberFormatItem> format_;
};

///////////////////////////////////////////////////////////////

class AnyNumberInstBase : public ResultMakingInstance,
                          public AnyNumberClient {
public:
    AnyNumberInstBase(const Number* number,
                      const InstanceInit& init, Instance* p);
    virtual ~AnyNumberInstBase();
    
    virtual void    numberChanged();
    virtual void    update();
    virtual void    dump() const;
    virtual const   NumberInstructionBase& numberInstruction() const;
};

class AnyNumberInst : public AnyNumberInstBase {
public:
    XSLT_OALLOC(AnyNumberInst);

    AnyNumberInst(const Number* number, const InstanceInit& init, Instance* p);
    virtual ~AnyNumberInst() {}
};

class AnyNumberInstAvt : public AnyNumberInstBase,
                         public Common::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(AnyNumberInstAvt);

    AnyNumberInstAvt(const Number* number,
                     const InstanceInit& init, Instance* p);
    virtual ~AnyNumberInstAvt();

    virtual const NumberConverter* numberFormat() const;
    void          notifyChanged(const Common::SubscriberPtrBase*);

private:
    Xpath::ValueHolderPtr formatAvt_;
    mutable Common::RefCntPtr<NumberFormatItem> format_;
};

inline void AnyCounter::numberChanged()
{
    AnyNumberClient* inst = clients_.firstChild();
    for (; inst; inst = inst->nextSibling())
        inst->numberChanged();
}

inline void AnyCounter::setPos(uint pos)
{
    if (pos != pos_) {
        pos_ = pos;
        numberChanged();
    }
}
    
} // namespace Xslt

#endif // XSLT_NUMBER_INST_H
