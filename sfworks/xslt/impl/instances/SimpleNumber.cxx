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
#include "xslt/impl/instances/ResultMakingInstance.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/instances/SharedData.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/instances/NumberInst.h"
#include "xslt/impl/patterns.h"
#include "xslt/impl/debug.h"
#include "common/SubscriberPtr.h"
#include <math.h>

namespace Xslt {

static ulong round_to_int(double v)
{
    const double ceiling = ceil(v);
    if (0.5 >= (ceiling - v))
        return (ulong)ceiling;
    return (ulong)(ceiling - 1.0);
}

/*! Instance of a "simple" number - with 'value' attribute only
 */
class SimpleNumberInstBase : public ResultMakingInstance,
                             public Common::SubscriberPtrWatcher {
public:
    SimpleNumberInstBase(const Number* number,
                         const InstanceInit& init, Instance* p);
    virtual ~SimpleNumberInstBase();

    virtual void    notifyChanged(const Common::SubscriberPtrBase*)
    {
        setModified();
    }
    virtual void    update();
    virtual void    dump() const;
    virtual const NumberConverter* numberFormat() const
    {
        return static_cast<const Number*>(instruction())->numberFormat();
    }
    void    toString(Common::String&) const;

private:
    Xpath::ValueHolderPtr valueInst_;
};

class  SimpleNumberInst : public SimpleNumberInstBase {
public:
    XSLT_OALLOC(SimpleNumberInst);

    SimpleNumberInst(const Number* number, 
                     const InstanceInit& init, Instance* p);
};

class  SimpleNumberInstAvt : public SimpleNumberInstBase {
public:
    XSLT_OALLOC(SimpleNumberInst);

    SimpleNumberInstAvt(const Number* number, 
                        const InstanceInit& init, Instance* p);
    virtual void    notifyChanged(const Common::SubscriberPtrBase*);
    virtual const   NumberConverter* numberFormat() const;

private:
    Xpath::ValueHolderPtr formatAvt_;
    mutable Common::RefCntPtr<NumberFormatItem> format_;
};

SimpleNumberInstBase::SimpleNumberInstBase(const Number* number,
                                           const InstanceInit& init,
                                           Instance* p)
    : ResultMakingInstance(number, init, p),
      valueInst_(this,
        number->valueExpr()->makeInst(init.context_, exprContext()))
{
}

void SimpleNumberInstBase::toString(COMMON_NS::String& s) const
{
    NumberConverter::NumberList nl;
    nl.push_back(round_to_int(valueInst_->value()->getDouble()));
    numberFormat()->toString(nl, s);
}

void SimpleNumberInstBase::update()
{
    if (GroveLib::Node::TEXT_NODE != result_.node()->nodeType()) 
        return;
    Common::String rs;
    toString(rs);
    GroveLib::Text& t = *static_cast<GroveLib::Text*>(result_.node());
    if (rs != t.data())
        t.setData(rs);
}

void SimpleNumberInstBase::dump() const
{
    DDBG << "SimpleNumberInst: " << this << std::endl;
    DBG_EXEC(XSLT.NUMBER,
        static_cast<const Number*>(instruction())->valueExpr()->dump());
    DBG_EXEC(XSLT.NUMBER, valueInst_->value()->dump());
    Instance::dump();
}

SimpleNumberInstBase::~SimpleNumberInstBase()
{
    disposeResult();
}
    
SimpleNumberInst::SimpleNumberInst(const Number* number, 
                                   const InstanceInit& init, Instance* p)
    : SimpleNumberInstBase(number, init, p)
{
    Common::String s;
    toString(s);
    result_.insertBefore(new TextResult(s, 0, this),
        init.resultContext_.node_);
}

SimpleNumberInstAvt::SimpleNumberInstAvt(const Number* number, 
                                         const InstanceInit& init, Instance* p)
    : SimpleNumberInstBase(number, init, p),
      formatAvt_(this, number->formatAvt()->valueExpr()->
        makeInst(nsiContext(), exprContext()))
{
    Common::String s;
    toString(s);
    result_.insertBefore(new TextResult(s, 0, this),
        init.resultContext_.node_);
}

const NumberConverter* SimpleNumberInstAvt::numberFormat() const
{
    if (format_.isNull()) {
        NumberCache* cache = &topStylesheetInst()->sharedData().numberCache();
        format_ = cache->getFormat(formatAvt_->value()->getString());
    }
    return format_.pointer();
}

void SimpleNumberInstAvt::notifyChanged(const Common::SubscriberPtrBase*)
{
    format_ = 0;
    setModified();
}

Instance* 
Number::makeSimpleNumberInst(const InstanceInit& init, Instance* p) const
{
    if (formatAvt())
        return new SimpleNumberInstAvt(this, init, p);
    else
        return new SimpleNumberInst(this, init, p);
}

} // namespace Xslt

