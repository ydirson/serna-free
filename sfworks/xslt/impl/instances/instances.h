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

#ifndef XSLT_INSTANCES_H
#define XSLT_INSTANCES_H

#include "xslt/xslt_defs.h"
#include "common/RefCntPtr.h"
#include "common/SubscriberPtr.h"
#include "common/Vector.h"
#include "grove/Nodes.h"

#include "xpath/ValueHolder.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"

#include "xslt/impl/instances/ResultMakingInstance.h"
#include "xslt/impl/instances/ApplicatingInstance.h"
#include "xslt/impl/instructions/Number.h"

namespace Xslt {

class Instruction;
class ApplyImports;

class ApplyImportsInstance : public ApplicatingInstance {
public:
    XSLT_OALLOC(ApplyImportsInstance);

    ApplyImportsInstance(const ApplyImports* applyImports,
                         const InstanceInit& init, Instance* p);
    virtual const Common::String& mode() const;
    
    virtual ~ApplyImportsInstance() {}

    void                dump() const;
};

///////////////////////////////////////////////////////////

class ApplyTemplates;

class ApplyTemplatesInstance : public ApplicatingInstance {
public:
    XSLT_OALLOC(ApplyTemplatesInstance);

    ApplyTemplatesInstance(const ApplyTemplates* applyTemplates,
                           const InstanceInit& init, Instance* p);

    virtual const Common::String& mode() const;

    virtual ~ApplyTemplatesInstance() {}

    void                    dump() const;
};

///////////////////////////////////////////////////////////

class Attribute;

class AttributeInstance : public ResultMakingInstance,
                          public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(AttributeInstance);

    AttributeInstance(const Attribute* attr, const InstanceInit& init,
                      Instance* p);
    virtual ~AttributeInstance();

    // skip AttributeInstance for the purposes of resultContext()
    const InstanceResult*   firstResult() const { return 0; }
    const InstanceResult*   lastResult() const { return 0; }

    //! Processes the notification about resoure`s change
    void                notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    //! Processes the notification about resource`s removal
    void                update();
    void                dump() const;

    //! Reimplemented from Instance
    virtual void        updateContext(int);

protected:
    virtual GroveLib::Node* rtfRoot() const;
    GroveLib::Node*     rtf_root() const;
    bool                isValidName() const;
    COMMON_NS::String   qName() const;
    void                setAttribute();

private:
    Xpath::ValueHolderPtr   nameInst_;
    Xpath::ValueHolderPtr   namespaceInst_;
    Xpath::ValueHolderPtr   rtf_;
    Xpath::ValueHolderPtr   valueInst_;
    Xpath::NodeSetItem      nsi_;
    GroveLib::AttrPtr       oldAttr_;
};

///////////////////////////////////////////////////////////

class AttributeResult;
class AttributeValueTemplate;

class AttrValueTemplateInstance :
                    public COMMON_NS::RefCounted<>,
                    public COMMON_NS::SubscriberPtrWatcher,
                    public COMMON_NS::SubscriberPtrPublisher {
public:
    XSLT_OALLOC(AttrValueTemplateInstance);

    typedef COMMON_NS::RefCntPtr<AttributeResult>   AttributeResultPtr;

    AttrValueTemplateInstance(AttributeValueTemplate* attr,
                              GroveLib::Element* elem,
                              const Xpath::NodeSetItem& nsi,
                              const Xpath::ExprContext& ectx);
    virtual ~AttrValueTemplateInstance() {}

    //! Processes the notification about resoure`s change
    void                notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    // Processes the notification about resource`s removal
    void                update();
    Xpath::ValueHolderPtr&  attrValue() { return attrValue_; }
    void                forceUpdate();

private:
    AttributeResultPtr      attrResult_;
    Xpath::ValueHolderPtr   attrValue_;
    bool                    isChanged_;
};

typedef COMMON_NS::SubscriberPtr<AttrValueTemplateInstance> AttrValuePtr;

///////////////////////////////////////////////////////////

class CallTemplate;

class CallTemplateInstance : public Instance {
public:
    XSLT_OALLOC(CallTemplateInstance);

    CallTemplateInstance(const CallTemplate* callTemplate,
                         const InstanceInit& init, Instance* p);
    virtual bool         mayHaveWithParams() const { return true; }
    void    update() {}

private:
    COMMON_NS::RefCntPtr<Instance>  templateInst_;
};

///////////////////////////////////////////////////////////

class ChosenInstance : public Instance {
public:
    XSLT_OALLOC(ChosenInstance);

    ChosenInstance(const Instruction* chosen,
                   const InstanceInit& init, Instance* p);
    virtual ~ChosenInstance() {}

    void    update() {}
    void    dump() const;
};

///////////////////////////////////////////////////////////

class Choose;
class ResultContext;
class When;

class ChooseInstance : public Instance,
                       public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(ChooseInstance);

    struct WhenValue {
        WhenValue() : whenInstr_(0) {}
        Xpath::ValueHolderPtr   valuePtr_;
        const When*             whenInstr_;
    };
    typedef Common::Vector<WhenValue> WhenVector;

    ChooseInstance(const Choose* choose,
                   const InstanceInit& init, Instance* p);
    virtual ~ChooseInstance() {}

    // Processes the notification about resoure`s change
    void                notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    // Processes the notification about resource`s removal
    void                update();
    void                updateContext(int);
    void                dump() const;

protected:
    void                choose_(const ResultContext* resultContext);

private:
    WhenVector          whenVector_;
    const Instruction*  chosen_;
};

///////////////////////////////////////////////////////////

class Copy;

class CopyInstance : public ResultMakingInstance,
                     public GroveLib::NodeVisitor {
public:
    XSLT_OALLOC(CopyInstance);

    enum ChangeType {
        TEXT_CHANGED,
        ATTR_CHANGED,
        NO_CHANGE
    };
    CopyInstance(const Copy* copy, const InstanceInit& init, Instance* p);
    virtual ~CopyInstance() {}

    // reimplemented from NodeVisitor
    virtual void    attributeChanged(const GroveLib::Attr*);
    virtual void    textChanged(const GroveLib::Text*);
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*) {}
    virtual void    childRemoved(const GroveLib::Node*,
                                 const GroveLib::Node*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}

    void            update();
    void            dump() const;

private:
    ChangeType  changeType_;
};

class GeneratedCopy;

/// This instance is used for copying xslt-generated nodes when
/// apply-templates is applied to the RTF
class GeneratedCopyInstance : public ResultMakingInstance {
public:
    XSLT_OALLOC(GeneratedCopyInstance);
    
    GeneratedCopyInstance(const GeneratedCopy* copy,
                          const InstanceInit& init, Instance* p);
    void            dump() const;
    void            update() {}
    virtual ~GeneratedCopyInstance() {}
};

///////////////////////////////////////////////////////////

class Element;

class ElementInstance : public ResultMakingInstance,
                        public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(ElementInstance);

    ElementInstance(const Element* element,
                    const InstanceInit& init, Instance* p);
    virtual ~ElementInstance() {};

    void                dump() const;
    //! Processes the notification about resoure`s change
    void                notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    //! Processes the notification about resource`s removal
    const InstanceResult* firstResult() const;
    const InstanceResult* lastResult() const;
    void                update();
    void                updateContext(int);

protected:
    bool                isValidName() const;
    COMMON_NS::String   qName() const;
    void                build(const ResultContext& before);

private:
    const Element*          element_;
    Xpath::ValueHolderPtr   nameInst_;
    Xpath::ValueHolderPtr   namespaceInst_;
};

///////////////////////////////////////////////////////////

class ForEach;

class ForEachInstance : public Instance,
                        public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(ForEachInstance);

    ForEachInstance(const ForEach* forEach,
                    const InstanceInit& init, Instance* p);
    virtual ~ForEachInstance() {};

    // Processes the notification about resoure`s change
    void            notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    // Processes the notification about resource`s removal
    void            update();
    void            updateContext(int);

protected:
    void            instantiate(const ResultContext& before);

private:
    Xpath::ValueHolderPtr   selectInst_;
};

///////////////////////////////////////////////////////////

class If;

class IfInstance : public Instance,
                   public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(IfInstance);

    IfInstance(const If* ifInstruction, const InstanceInit& init, Instance* p);
    virtual ~IfInstance() {}

    // Processes the notification about resoure`s change
    void        notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    // Processes the notification about resource`s removal
    void        update();
    void        updateContext(int);

private:
    Xpath::ValueHolderPtr   testInst_;
    bool                    prevValue_;
};

///////////////////////////////////////////////////////////

class LiteralResult;
class AttrValueTemplateInstance;

class LiteralResultInstance : public ResultMakingInstance,
                              public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(LiteralResultInstance);
    typedef std::list<AttrValuePtr> AttrList;

    LiteralResultInstance(const LiteralResult* literalResult,
                          const InstanceInit& init, Instance* p);
    virtual ~LiteralResultInstance();

    // Processes the notification about resoure`s change
    void                    notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    // Processes the notification about resource`s removal
    void                    update();
    void                    updateContext(int);
    void                    dump() const;

private:
    AttrList                attributes_;
};

///////////////////////////////////////////////////////////

class LiteralResultText;

class LiteralTextInstance : public ResultMakingInstance {
public:
    XSLT_OALLOC(LiteralTextInstance);

    LiteralTextInstance(const LiteralResultText* literalResultText,
                        const InstanceInit& init, Instance* p);

    void    update() {};
    void    dump() const;
};

///////////////////////////////////////////////////////////

class Text;

class TextInstance : public ResultMakingInstance {
public:
    XSLT_OALLOC(TextInstance);

    TextInstance(const Text* text, const InstanceInit& init, Instance* p);
    void    update() {}
};

///////////////////////////////////////////////////////////

class ValueOf;

class ValueOfInstance : public ResultMakingInstance,
                        public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(ValueOfInstance);

    ValueOfInstance(const ValueOf* valueOf,
                    const InstanceInit& init, Instance* p);
    virtual ~ValueOfInstance() {}

    void        notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    void        update();
    void        updateContext(int);
    void        dump() const;

private:
    Xpath::ValueHolderPtr   selectInst_;
};

class MessageInstance: public Instance,
                       public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(MessageInstance);

    MessageInstance(const Message* instr,
                    const InstanceInit& init, Instance* p);
    virtual ~MessageInstance() {}

    void        notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);
    void        update();
    void        updateContext(int);
    void        dump() const;
    void        emitMessage();
    virtual GroveLib::Node* rtfRoot() const;

private:
    Xpath::ValueHolderPtr   messageInst_;
    Xpath::ValueHolderPtr   rtf_;
    Xpath::NodeSetItem      messageInstNsi_;
    COMMON_NS::String       message_;
};

class DummyInstance : public Instance {
public:
    XSLT_OALLOC(DummyInstance);

    DummyInstance(const Instruction* instr, 
                  const InstanceInit& init, Instance* p)
        : Instance(instr, init, p) {}
    virtual void    update() {}
};

///////////////////////////////////////////////////////////

} // namespace Xslt

#endif // XSLT_INSTANCES_H_
