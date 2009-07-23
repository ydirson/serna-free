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

#include "xslt/xslt_defs.h"
#include "xslt/Result.h"
#include "xslt/impl/Instance.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/debug.h"
#include "common/TreelocRep.h"
#include "common/Singleton.h"
#include "common/StringFactory.h"

USING_COMMON_NS

namespace Xslt {

class XsltResultStringFactory : public StringFactory {};

static inline String makeString(const String& s)
{
    return SingletonHolder<XsltResultStringFactory>::instance().makeString(s);
}

void XsltResult::getTreeloc(COMMON_NS::TreelocRep& tl) const
{
    if (instance_) {
        tl.resize(0);
        if (instance_->instruction()->type() == Xslt::Instruction::COPY_OF
            && copyOrigin()) { // xsl:copy-of
                const GroveLib::Node* n = copyOrigin();
                if (n->nodeType() == GroveLib::Node::ATTRIBUTE_NODE) {
                    tl += n->siblingIndex();
                    n = static_cast<const GroveLib::Attr*>(n)->element();
                }
                for (; n->parent(); n = n->parent())
                    tl += n->siblingIndex();
        }
        instance_->getTreeloc(tl, true);
    }
    else
        tl.clear();
}

void XsltResult::notifyTemplateStateChange() const
{
    if (origin_.isNull() || !instance_)
        return;
    instance_->notifyTemplateStateChange(origin_.pointer());
}

AttributeResult::AttributeResult(const COMMON_NS::String& name,
                                 const COMMON_NS::String& value,
                                 const GroveLib::Node* origin,
                                 const Xslt::Instance* inst)
    : XsltResult(origin, inst), GroveLib::Attr(makeString(name))
{
    if (!value.isNull())
        setValue(makeString(value));
}

AttributeResult::~AttributeResult()
{
}

/////////////////////////////////////////////////


ElementResult::ElementResult(const String& name,
                             const GroveLib::Node* origin,
                             const Xslt::Instance* inst)
    : XsltResult(origin, inst), Element(makeString(name))
{
}

ElementResult::~ElementResult()
{
}

////////////////////////////////////////////////


TextResult::TextResult(const String& text,
                       const GroveLib::Node* origin,
                       const Xslt::Instance* inst)
    : XsltResult(origin, inst), Text(text)
{
}

TextResult::~TextResult()
{
}

///////////////////////////////////////////////

XSLT_EXPIMP GroveLib::Node* resultOrigin(const GroveLib::Node* node)
{
    const GroveLib::Node* resultNode = node;
    while (resultNode) {
        const XsltResult* result = resultNode->asConstXsltResult();
        if (result) {
            if (!result->origin())
                return 0;
            if (result->origin()->grove())
                return const_cast<GroveLib::Node*>(result->origin());
            else
                resultNode = result->origin();
        } else
            return 0;
    }
    return 0;
}

PRTTI_IMPL(XsltResult)
PRTTI_IMPL2(AttributeResult, XsltResult)
PRTTI_IMPL2(ElementResult, XsltResult)
PRTTI_IMPL2(TextResult, XsltResult)

} // namespace Xslt

