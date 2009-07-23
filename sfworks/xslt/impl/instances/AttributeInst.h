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

#ifndef _XSLT_IMPL_INSTANCES_ATTRIBUTE_H_
#define _XSLT_IMPL_INSTANCES_ATTRIBUTE_H_

#include "xslt/xslt_defs.h"
#include "xslt/impl/instructions/Attribute.h"
#include "xslt/impl/instances/ResultMakingInstance.h"

namespace Xslt {

class AttributeResult;

class AttributeInstBase : public ResultMakingInstance {
public:
    AttributeInstBase(const AttributeCommon* attr, const InstanceInit& init,
                      Instance* p);

    virtual ~AttributeInstBase();

    // skip AttributeInstance for the purposes of resultContext()
    const InstanceResult*   firstResult() const { return 0; }
    const InstanceResult*   lastResult() const { return 0; }

    virtual const COMMON_NS::String& value() const = 0;
    virtual const COMMON_NS::String  qName() const = 0;
    virtual void  update() { setAttribute(); }

    void    setAttribute();

private:
    void            saveOldAttr();
    void            restoreOldAttr();
    AttributeResult* resultAttr() const;

    GroveLib::AttrPtr       oldAttr_;
};

///////////////////////////////////////////////

class AvtNameAttributeInstance {
public:
    AvtNameAttributeInstance(const AttributeAvtNameProvider*,
                             Instance* self);
    virtual ~AvtNameAttributeInstance() {}

    const COMMON_NS::String qName() const;
    void    setWatchers(COMMON_NS::SubscriberPtrWatcher*);
    void    updateAvtContext(Instance* i);

protected:
    Xpath::ValueHolderPtr   nameInst_;
    Xpath::ValueHolderPtr   namespaceInst_;
};

class SimpleNameAttributeInstance {
public:
    SimpleNameAttributeInstance(const AttributeSimpleNameProvider* np)
        : nameProvider_(np) {}
    virtual ~SimpleNameAttributeInstance() {}

    const COMMON_NS::String qName() const;
    void    setWatchers(COMMON_NS::SubscriberPtrWatcher*) {}

private:
    const AttributeSimpleNameProvider* nameProvider_;
};

///////////////////////////////////////////////

class ConstValueAttributeInstance {
public:
    ConstValueAttributeInstance(const AttributeConstValueProvider* vp)
        : valueProvider_(vp) {}
    virtual ~ConstValueAttributeInstance() {}

    const COMMON_NS::String& value() const;
    void    setWatchers(COMMON_NS::SubscriberPtrWatcher*) {}

private:
    const AttributeConstValueProvider* valueProvider_;
};

class ExprValueAttributeInstance {
public:
    ExprValueAttributeInstance(const AttributeExprValueProvider*,
                               Instance* self);
    virtual ~ExprValueAttributeInstance() {}

    const COMMON_NS::String& value() const;
    void    setWatchers(COMMON_NS::SubscriberPtrWatcher*);
    void    updateValueContext(Instance*);

private:
    Xpath::ValueHolderPtr   valueInst_;
};

class RtfValueAttributeInstance {
public:
    RtfValueAttributeInstance(const AttributeRtfValueProvider*,
                              Instance* self);
    virtual ~RtfValueAttributeInstance() {}

    const COMMON_NS::String& value() const;
    void    setWatchers(COMMON_NS::SubscriberPtrWatcher*);
    GroveLib::Node* rtf_root() const;

private:
    Xpath::ValueHolderPtr   rtf_;
    Xpath::ValueHolderPtr   valueInst_;
    Xpath::NodeSetItem      nsi_;
};

////////////////////////////////////////

class SimpleConstAttributeInst : public AttributeInstBase,
                                 public SimpleNameAttributeInstance,
                                 public ConstValueAttributeInstance {
public:
    GROVE_OALLOC(SimpleConstAttributeInst);

    SimpleConstAttributeInst(const SimpleConstAttribute*,
                             const InstanceInit& init, Instance* p);
    virtual ~SimpleConstAttributeInst() {}
    virtual const COMMON_NS::String qName() const
    {
        return SimpleNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return ConstValueAttributeInstance::value();
    }
};

class AvtConstAttributeInst : public AttributeInstBase,
                              public AvtNameAttributeInstance,
                              public ConstValueAttributeInstance,
                              public COMMON_NS::SubscriberPtrWatcher {
public:
    GROVE_OALLOC(AvtConstAttributeInst);

    AvtConstAttributeInst(const AvtConstAttribute*,
                          const InstanceInit& init, Instance* p);
    virtual ~AvtConstAttributeInst() {}
    virtual void notifyChanged(const COMMON_NS::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual void  updateContext(int utype);
    virtual const COMMON_NS::String qName() const
    {
        return AvtNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return ConstValueAttributeInstance::value();
    }
};

//

class SimpleExprAttributeInst : public AttributeInstBase,
                                public SimpleNameAttributeInstance,
                                public ExprValueAttributeInstance,
                                public COMMON_NS::SubscriberPtrWatcher {
public:
    GROVE_OALLOC(SimpleExprAttributeInst);

    SimpleExprAttributeInst(const SimpleExprAttribute*,
                            const InstanceInit& init, Instance* p);
    virtual ~SimpleExprAttributeInst() {}
    virtual void notifyChanged(const COMMON_NS::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual void updateContext(int utype);
    virtual const COMMON_NS::String qName() const
    {
        return SimpleNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return ExprValueAttributeInstance::value();
    }
};

class AvtExprAttributeInst : public AttributeInstBase,
                             public AvtNameAttributeInstance,
                             public ExprValueAttributeInstance,
                             public COMMON_NS::SubscriberPtrWatcher {
public:
    GROVE_OALLOC(AvtExprAttributeInst);

    AvtExprAttributeInst(const AvtExprAttribute*,
                         const InstanceInit& init, Instance* p);
    virtual void notifyChanged(const COMMON_NS::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual void  updateContext(int utype);
    virtual const COMMON_NS::String qName() const
    {
        return AvtNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return ExprValueAttributeInstance::value();
    }
};

//

class SimpleRtfAttributeInst : public AttributeInstBase,
                               public SimpleNameAttributeInstance,
                               public RtfValueAttributeInstance,
                               public COMMON_NS::SubscriberPtrWatcher {
public:
    GROVE_OALLOC(SimpleRtfAttributeInst);

    SimpleRtfAttributeInst(const SimpleRtfAttribute*,
                           const InstanceInit& init, Instance* p);
    virtual ~SimpleRtfAttributeInst() {}
    virtual GroveLib::Node* rtfRoot() const { return rtf_root(); }
    virtual void notifyChanged(const COMMON_NS::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual const COMMON_NS::String qName() const
    {
        return SimpleNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return RtfValueAttributeInstance::value();
    }
};

class AvtRtfAttributeInst : public AttributeInstBase,
                            public AvtNameAttributeInstance,
                            public RtfValueAttributeInstance,
                            public COMMON_NS::SubscriberPtrWatcher {
public:
    GROVE_OALLOC(AvtRtfAttributeInst);

    AvtRtfAttributeInst(const AvtRtfAttribute*,
                        const InstanceInit& init, Instance* p);
    virtual GroveLib::Node* rtfRoot() const { return rtf_root(); }
    virtual void notifyChanged(const COMMON_NS::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual void updateContext(int utype);
    virtual const COMMON_NS::String qName() const
    {
        return AvtNameAttributeInstance::qName();
    }
    virtual const COMMON_NS::String& value() const
    {
        return RtfValueAttributeInstance::value();
    }
    virtual ~AvtRtfAttributeInst() {}
};

} // namespace Xslt

#endif // _XSLT_IMPL_INSTANCES_ATTRIBUTE_H_
