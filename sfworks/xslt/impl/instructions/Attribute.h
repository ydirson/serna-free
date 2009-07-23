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
#ifndef XSLT_IMPL_INSTRUCTIONS_ATTRIBUTE_H
#define XSLT_IMPL_INSTRUCTIONS_ATTRIBUTE_H

#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/Instruction.h"

namespace Xslt {

class AttributeAvtNameProvider {
public:
    AttributeAvtNameProvider(AttributeValueTemplate* name,
                             AttributeValueTemplate* ns)
        : name_(name), namespace_(ns) {}

    const COMMON_NS::String& nameString() const { return name_->value(); }
    const COMMON_NS::String& nsString() const { return namespace_->value(); }
    const AttributeValueTemplate* name() const { return name_.pointer(); }
    const AttributeValueTemplate* ns() const { return namespace_.pointer(); }

    void  dump_np() const;

private:
    COMMON_NS::OwnerPtr<AttributeValueTemplate> name_;
    COMMON_NS::OwnerPtr<AttributeValueTemplate> namespace_;
};

class AttributeSimpleNameProvider {
public:
    AttributeSimpleNameProvider(AttributeValueTemplate* name,
                                AttributeValueTemplate* ns)
        : name_(name->value())
    {
        if (ns)
            namespace_ = ns->value();
    }
    const COMMON_NS::String& nameString() const { return name_; }
    const COMMON_NS::String& nsString() const { return namespace_; }

    void  dump_np() const;

private:
    COMMON_NS::String   name_;
    COMMON_NS::String   namespace_;
};

/////////////////////////////////

class AttributeConstValueProvider {
public:
    AttributeConstValueProvider(const COMMON_NS::String& value)
        : value_(value) {}

    const COMMON_NS::String& value() const { return value_; }
    void  dump_vp() const;

private:
    COMMON_NS::String   value_;
};

class AttributeExprValueProvider {
public:
    AttributeExprValueProvider(const Xpath::ExprPtr& valueExpr)
        : valueExpr_(valueExpr) {}

    const Xpath::ExprPtr valueExpr() const { return valueExpr_; }
    void  dump_vp() const;

private:
    Xpath::ExprPtr      valueExpr_;
};

class AttributeRtfValueProvider {
public:
    AttributeRtfValueProvider(const Xpath::ExprPtr& valueExpr)
        : valueExpr_(valueExpr) {}

    const Xpath::ExprPtr valueExpr() const { return valueExpr_; }
    void  dump_vp() const;

private:
    Xpath::ExprPtr      valueExpr_;
};

////////////////////////////////

class AttributeCommon : public Instruction {
public:
    AttributeCommon(const Instruction* i)
        : Instruction(i->element(), i->styleContext(), 0) {}

    AttributeCommon(const GroveLib::Element* element,
              const Stylesheet* context, Instruction* p)
        : Instruction(element, context, p) {}

    virtual ~AttributeCommon() {}

    virtual const COMMON_NS::String& nameString() const = 0;

private:
    virtual bool allowsChild(Type) const { return false; } // unused
};

template <class NP, class VP>
class AttributeTemplate : public AttributeCommon, public NP, public VP {
    using VP::dump_vp;
    using NP::dump_np;
public:
    XSLT_OALLOC(Attribute);

    AttributeTemplate(const Instruction* i,
                      AttributeValueTemplate* name,
                      AttributeValueTemplate* ns)
        : AttributeCommon(i), NP(name, ns), VP(i->makeExpr("string(.)")) {}

    AttributeTemplate(const Instruction* i,
                      AttributeValueTemplate* name,
                      AttributeValueTemplate* ns,
                      const COMMON_NS::String& value)
        : AttributeCommon(i), NP(name, ns), VP(value) {}

    AttributeTemplate(const Instruction* i,
                      AttributeValueTemplate* name,
                      AttributeValueTemplate* ns,
                      const Xpath::ExprPtr& valueExpr)
        : AttributeCommon(i), NP(name, ns), VP(valueExpr) {}

    virtual ~AttributeTemplate() {}

    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    virtual const COMMON_NS::String& nameString() const
    {
        return NP::nameString();
    }
    virtual void        dump() const
    {
        dump_vp();
        dump_np();
        Instruction::dump();
    }
    Type    type() const { return ATTRIBUTE; }
};

/// A base instruction - just a holder

class Attribute : public AttributeCommon {
public:
    XSLT_OALLOC(Attribute);

    Attribute(const GroveLib::Element* element,
              const Stylesheet* context, Instruction* p);
    virtual ~Attribute() {}

    virtual const COMMON_NS::String& nameString() const;    // for attributeSet

    AttributeCommon* remakeSelf();
    virtual bool allowsChild(Type type) const;
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    Type    type() const { return UNPROCESSED_ATTRIBUTE; }
};

typedef AttributeTemplate<AttributeSimpleNameProvider,
                          AttributeConstValueProvider> SimpleConstAttribute;
typedef AttributeTemplate<AttributeSimpleNameProvider,
                          AttributeRtfValueProvider>   SimpleRtfAttribute;
typedef AttributeTemplate<AttributeSimpleNameProvider,
                          AttributeExprValueProvider>  SimpleExprAttribute;
typedef AttributeTemplate<AttributeAvtNameProvider,
                          AttributeRtfValueProvider>   AvtRtfAttribute;
typedef AttributeTemplate<AttributeAvtNameProvider,
                          AttributeConstValueProvider> AvtConstAttribute;
typedef AttributeTemplate<AttributeAvtNameProvider,
                          AttributeExprValueProvider>  AvtExprAttribute;

} // namespace Xslt

#endif // XSLT_IMPL_INSTRUCTIONS_ATTRIBUTE_H

