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
#ifndef XSLT_TEMPLATE_SELECTOR_H
#define XSLT_TEMPLATE_SELECTOR_H

#include "xslt/xslt_defs.h"
#include "common/SubscriberPtr.h"
#include "xpath/ExprContext.h"
#include "xpath/ValueHolder.h"
#include "xpath/NodeSet.h"
#include "xslt/Exception.h"
#include "xslt/impl/PatternInst.h"
#include "xslt/impl/TemplateProvider.h"
#include "common/SList.h"

namespace Xslt {

class ApplicatingInstance;
class TemplateProvider;
class Template;
class Instance;

class TsValueWatcher : public Common::SListItem<TsValueWatcher>,
                       public Common::SubscriberPtrBase {
public:
    XSLT_OALLOC(TsValueWatcher);
    
    TsValueWatcher(Common::SubscriberPtrWatcher* watcher,
                   Common::SubscriberPtrPublisher* publisher)
        : SubscriberPtrBase(watcher, publisher) {}
};

typedef Common::SList<TsValueWatcher> TsValueWatcherList;

class TemplateSelector : public Common::RefCounted<>,
                         public Common::SubscriberPtrPublisher,
                         public Common::SubscriberPtrWatcher,
                         public GroveLib::NodeVisitor,
                         public Xpath::ExprContext {
public:
    XSLT_OALLOC(TemplateSelector);

    TemplateSelector(const Xpath::NodeSetItem& nsi,
                     const ApplicatingInstance* appi,
                     const GroveLib::Node* resultParent);
    ~TemplateSelector();

    const Template*             selectedTemplate() const;
    void                        setTemplateInstance(Instance* inst)
    {
        templateInstance_ = inst;
    }
    Instance*                   templateInstance() const
    {
        return templateInstance_;
    }
    void    nodeChanged() { setChanged(); }
    void    notifyChanged(const Common::SubscriberPtrBase*) { setChanged(); }

    void            update();
    void            dump() const;
    void            setModified(bool v) { isModified_ = v; }
    bool            isModified() { return isModified_; }
    bool            isFolded(const GroveLib::Node*) const;

    virtual void    nodeDestroyed(const GroveLib::Node*);
    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved (const GroveLib::Node* node,
                                  const GroveLib::Node* child);
    virtual void    attributeChanged(const GroveLib::Attr*);
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*);
    virtual void    attributeAdded(const GroveLib::Attr*);
    virtual void    textChanged(const GroveLib::Text*);
    virtual void    genericNotify(const GroveLib::Node*, void*) {}

    virtual Xpath::ValueHolder* getVariable(const COMMON_NS::String& v) const;
    virtual COMMON_NS::String contextString() const;
    virtual void    registerVisitor(const GroveLib::Node* node,
                                    GroveLib::NodeVisitor* visitor,
                                    short mask) const;
    virtual bool    watchFor(Common::SubscriberPtrPublisher*) const;
    void            setChanged();

private:
    void                        choose_template();
    const Template*             getBuiltinTemplate() const;

    const Xpath::NodeSetItem*   nsi_;
    const ApplicatingInstance*  applicatingInst_;
    const Template*             selected_;
    Instance*                   templateInstance_;
    mutable TsValueWatcherList  wlist_;
    bool                        isModified_;
    bool                        isFolded_;
};

typedef COMMON_NS::SubscriberPtr<TemplateSelector> TemplateSelectorPtr;

} // namespace Xslt

#endif // XSLT_TEMPLATE_SELECTOR_H
