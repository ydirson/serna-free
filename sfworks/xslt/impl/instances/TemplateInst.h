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

#ifndef XSLT_IMPL_TEMPLATE_INST_H
#define XSLT_IMPL_TEMPLATE_INST_H

#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
#include "xslt/impl/Instance.h"
#include "xslt/impl/PatternInst.h"
#include "xslt/impl/instances/ResultMakingInstance.h"
#include "xslt/impl/TemplateSelector.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "grove/NodeVisitor.h"

namespace Xslt {

class Template;
class TextAndAttrTemplate;
class ElementTemplate;
class SectionTemplate;
class TextTemplate;

// Common base for both builtin and usual templates
class TemplateInstanceBase {
public:
    virtual ~TemplateInstanceBase() {}

    void setSelector(TemplateSelector* ts, ApplicatingInstance* watcher)
    {
        templateSelector_ = ts;
        templateSelector_.setWatcher(watcher);
    }
    TemplateSelector*       templateSelector() const
    {
        return templateSelector_.pointer();
    }
private:
    TemplateSelectorPtr templateSelector_;
};

///////////////////////////////////////////////////////////

class TemplateInstance : public Instance,
                                     public TemplateInstanceBase {
public:
    XSLT_OALLOC(TemplateInstance);

    TemplateInstance(const Template* templateInstr,
                     const InstanceInit& init, Instance* p);
    virtual ~TemplateInstance();

    //! Reimplemented from Instance
    //bool                    doesLoop(const Instruction* instruction,
    //                                 const Xpath::NodeSetItem& context,
    //                                 COMMON_NS::String& loop) const;
    void                    update() {}
    void                    dump() const;
    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
    virtual const Instance* asConstTemplateInst() const { return this; }
    virtual void            updateContext(int contextUpdateType);

protected:
    COMMON_NS::String       signature() const;

private:
    const Template*         template_;
    //const Instance*         upperTemplate_;
};

///////////////////////////////////////////////////////////

class ElementTemplate;
class DummyTemplate;

class DummyTemplateInstance : public Instance,
                                          public TemplateInstanceBase {
public:
    XSLT_OALLOC(DummyTemplateInstance);

    DummyTemplateInstance(const Instruction*, const InstanceInit& init, Instance* p);

    virtual const InstanceResult* firstResult() const { return 0; }
    virtual const InstanceResult* lastResult() const { return 0; }
    virtual void            disposeResult() {}
    virtual void            buildSubInstances(const ResultContext&) {}
    void                    update() {};
    void                    dump() const;
    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }

};

///////////////////////////////////////////////////////////

class ElementTemplateInstance : public ApplicatingInstance,
                                            public TemplateInstanceBase {
public:
    XSLT_OALLOC(ElementTemplateInstance);

    ElementTemplateInstance(const ElementTemplate* elementTemplate,
                            const InstanceInit& init, Instance* p);
    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }

    void                    dump() const;

private:
    const ElementTemplate*  template_;
};

///////////////////////////////////////////////////////////

class TextAndAttrTemplateInstance : public ResultMakingInstance,
            public COMMON_NS::SubscriberPtrWatcher,
            public TemplateInstanceBase {
public:
    XSLT_OALLOC(TextAndAttrTemplateInstance);

    TextAndAttrTemplateInstance(const TextAndAttrTemplate* textAndAttrTemplate,
                                const InstanceInit& init, Instance* p);
    virtual ~TextAndAttrTemplateInstance();

    // Processes the notification about resoure`s change
    void                    notifyChanged(const COMMON_NS::SubscriberPtrBase*);
    // Processes the notification about resource`s removal
    void                    update();
    virtual void            updateContext(int);
    void                    dump() const;
    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }

private:
    const TextAndAttrTemplate*  template_;
    Xpath::ValueHolderPtr       selectInst_;
};

class TextTemplateInstance : public ResultMakingInstance,
                                         public TemplateInstanceBase,
                                         public GroveLib::NodeVisitor {
public:
    XSLT_OALLOC(TextTemplateInstance);

    TextTemplateInstance(const TextTemplate* textTemplate,
                         const InstanceInit& init, Instance* p);
    virtual ~TextTemplateInstance();

    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*) {}
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*) {}
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}
    virtual void    textChanged(const GroveLib::Text* node);

    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }

    void                    update();
    virtual void            updateContext(int) {}
    void                    dump() const;
};

///////////////////////////////////////////////////////////

class SectionTemplateInstance : public ResultMakingInstance,
                                            public TemplateInstanceBase {
public:
    XSLT_OALLOC(SectionTemplateInstance);

    SectionTemplateInstance(const SectionTemplate* t, const InstanceInit& init,
                            Instance* p);
    virtual ~SectionTemplateInstance();

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

///////////////////////////////////////////////////////////

class ChoiceTemplate;

class ChoiceTemplateInstance : public ResultMakingInstance,
                                           public TemplateInstanceBase {
public:
    XSLT_OALLOC(ChoiceTemplateInstance);

    ChoiceTemplateInstance(const ChoiceTemplate* t, const InstanceInit& init,
                            Instance* p);
    virtual ~ChoiceTemplateInstance();

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

///////////////////////////////////////////////////////////

class TextChoiceTemplate;

class TextChoiceTemplateInstance : public ResultMakingInstance,
                                   public TemplateInstanceBase,
                                   public Common::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(TextChoiceTemplateInstance);

    TextChoiceTemplateInstance(const TextChoiceTemplate* t, 
                               const InstanceInit& init,
                               Instance* p);
    virtual ~TextChoiceTemplateInstance();

    // Processes the notification about resource`s removal
    void                    notifyChanged(const Common::SubscriberPtrBase*);
    void                    update();
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }

private:
    Xpath::ValueHolderPtr   valueInst_;
};

///////////////////////////////////////////////////////////

class CommentTemplate;

class CommentTemplateInstance : public ResultMakingInstance,
                                public TemplateInstanceBase {
public:
    XSLT_OALLOC(CommentTemplateInstance);

    CommentTemplateInstance(const CommentTemplate* t, const InstanceInit& init,
                            Instance* p);
    virtual ~CommentTemplateInstance();

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

///////////////////////////////////////////////////////////

class PiTemplate;

class PiTemplateInstance : public ResultMakingInstance,
                                           public TemplateInstanceBase {
public:
    XSLT_OALLOC(PiTemplateInstance);

    PiTemplateInstance(const PiTemplate* t, const InstanceInit& init,
                            Instance* p);
    virtual ~PiTemplateInstance();

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

///////////////////////////////////////////////////////////

class FoldTemplate;

class FoldTemplateInstance : public ResultMakingInstance,
                                         public TemplateInstanceBase {
public:
    XSLT_OALLOC(FoldTemplateInstance);

    FoldTemplateInstance(const FoldTemplate* t, const InstanceInit& init,
                         Instance* p);
    virtual const Instance* asConstTemplateInst() const { return this; }
    virtual ~FoldTemplateInstance();

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

///////////////////////////////////////////////////////////

class GenCopyTemplate;

class GenCopyTemplateInstance : public ResultMakingInstance,
                                public TemplateInstanceBase {
public:
    XSLT_OALLOC(GenCopyTemplateInstance);

    GenCopyTemplateInstance(const GenCopyTemplate* t, const InstanceInit& init,
                         Instance* p);
    virtual const Instance* asConstTemplateInst() const { return this; }

    // Processes the notification about resource`s removal
    void                    update() {}
    virtual void            updateContext(int) {}
    void                    dump() const;

    // reimplemented from Instance
    virtual TemplateInstanceBase* asTemplateInstBase() { return this; }
};

} // namespace Xslt

#endif // XSLT_IMPL_TEMPLATE_INST_H
