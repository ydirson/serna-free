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

#ifndef XSLT_INSTANCE_H
#define XSLT_INSTANCE_H

#ifdef _MSC_VER
# pragma warning(disable : 4786)
#endif

#include "xslt/xslt_defs.h"
#include "common/OwnerPtr.h"
#include "common/String.h"
#include "common/XTreeNode.h"
#include "common/SubscriberPtr.h"

#include "xpath/ValueHolder.h"
#include "xpath/ExprContext.h"

#include "xslt/Exception.h"
#include "xslt/impl/ResultContext.h"
#include "xslt/impl/InstanceResult.h"
#include "xslt/impl/InstanceInit.h"

namespace Xslt {

class Instruction;
class TopStylesheetInst;
class Stylesheet;
class TemplateInstanceBase;
class ApplicatingInstance;
class Instance;

typedef COMMON_NS::XTreeNode<Instance,
    COMMON_NS::XTreeNodeRefCounted<Instance> > InstanceTreeBase;

/*! \brief InstructionInst
 */
class Instance : public COMMON_NS::RefCounted<>,
                             public InstanceTreeBase,
                             public Xpath::ExprContext {
public:
    XSLT_OALLOC(Instance);

    Instance(const Instruction* instruction,
             const InstanceInit& init, Instance* parent)
        : instruction_(instruction), nsi_(&init.context_),
          isModified_(false), skipResultContext_(false),
          context_dep_(UNDEF_DEP)
    {
        if (parent) {
            parent->appendChild(this);
            topInst_ = parent->topInst_;
        } else
            set_root_top();
    }
    virtual ~Instance();

    // Expression NSI context
    const Xpath::NodeSetItem& nsiContext() const { return *nsi_; }

    enum ContextUpdateType {
        SIZE_CHANGED = 01, POS_CHANGED = 02, UNDEF_DEP = 07 // <- three bits
    };
    virtual void updateContext(int contextUpdateType);

    // Instruction style context
    const Stylesheet& styleContext() const;

    // First result of current instance
    virtual const InstanceResult* firstResult() const;

    // Last result of current instance
    virtual const InstanceResult* lastResult() const;

    // Non-recursive - first result of instance
    virtual const InstanceResult* selfResult() const { return 0; }

    // Dispose result of an instruction
    virtual void            disposeResult();

    // Returns context for result. This is a first result produced
    // by following instances, or last result of preceding instances.
    virtual ResultContext   resultContext() const;

    //!
    virtual void            dump() const;
    //!
    const Instruction*      instruction() const { return instruction_; }
    //!
    const GroveLib::Element* element() const;
    //!
    virtual bool            doesLoop(const Instruction* instruction,
                                     const Xpath::NodeSetItem& context,
                                     COMMON_NS::String& loop) const;
    //! Sets the modification flag and registers itself in registry
    void                    setModified(bool isModified = true);
    //!
    virtual void            update() = 0;
    //! Reimplemented from Xpath::VarBindings
    Xpath::ValueHolder*       getVariable(const COMMON_NS::String& name) const;

    const Xpath::ExprContext& exprContext() const { return *this; }

    const TopStylesheetInst*  topStylesheetInst() const { return topInst_; }
    const Instance*           currentTemplate() const;

    virtual TemplateInstanceBase* asTemplateInstBase();
    virtual const ApplicatingInstance* asConstApplicatingInstance() const;
    virtual COMMON_NS::String contextString() const;
    virtual const Xpath::ExternalFunctionBase* getExternalFunction
        (const Common::String&, const Common::String&) const;

/// note: we don't update sibling indexes on remove
    void    appendChild(Instance* i)
    {
        InstanceTreeBase::appendChild(i);
        i->setSiblingIndex();
    }
    void    insertBefore(Instance* i)
    {
        InstanceTreeBase::insertBefore(i);
        i->setSiblingIndex();
        for (i = this; i; i = i->nextSibling())
            ++i->siblingIndex_;
    }
    // get treeloc in instance tree
    void    getTreeloc(COMMON_NS::TreelocRep& tl, bool append = false) const;

    void    setSiblingIndex()
    {
        const Instance* ps = prevSibling();
        if (ps)
            siblingIndex_ = ps->siblingIndex_ + 1;
        else
            siblingIndex_ = 0;
    }
    uint    instanceSiblingIndex() const { return siblingIndex_; }
    bool    updateExpr(Xpath::ValueHolderPtr& vp);
    void    buildSubInstances(const ResultContext& before,
                                      const Xpath::NodeSetItem* ctx = 0);
    void    setSkipResultContext(bool v) { skipResultContext_ = v; }

    /// force template selector to reselect template matched to \a origin
    /// such a template must be an ancestor of current instance.
    void    notifyTemplateStateChange(const GroveLib::Node* origin) const;

protected:
    virtual bool    mayHaveWithParams() const { return false; }
    virtual GroveLib::Node* rtfRoot() const { return 0; }
    virtual const Instance* asConstTemplateInst() const { return 0; }

    void            removeSubInstances() { removeAllChildren(); }

    void            updateChildContext(int contextUpdateType);
    void            getResult(ResultContext&) const;
    void            set_root_top();

protected:
    TopStylesheetInst*  topInst_;
    const Instruction*  instruction_;
    const Xpath::NodeSetItem* nsi_;
    uint                siblingIndex_ : 24;
    uint                isModified_   : 1;
    uint                skipResultContext_ : 1;
    uint                context_dep_ : 3;
};

} // namespace Xslt

#endif // XSLT_INSTANCE_H
