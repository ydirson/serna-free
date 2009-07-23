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
#ifndef XSLT_ATTRIBUTE_VALUE_TEMPLATE_H
#define XSLT_ATTRIBUTE_VALUE_TEMPLATE_H

#include <list>
#include "common/String.h"
#include "grove/Nodes.h"
#include "xpath/impl3/ExprImpl.h"
#include "xpath/FunctionFactory.h"
#include "xslt/xslt_defs.h"
#include "xslt/Exception.h"

COMMON_NS_BEGIN
class MessageStream;
COMMON_NS_END

namespace Xslt {

class Instruction;

class AttributeValueTemplate {
public:
    XSLT_OALLOC(AttributeValueTemplate);
    typedef std::list<COMMON_NS::String>    StringList;
    typedef StringList::const_iterator      StringIterator;

    AttributeValueTemplate(const GroveLib::Attr* origin,
                           const Xpath::FunctionFactory* fact);
    //!
    const GroveLib::Attr*   origin() const {return origin_;}
    //!
    const COMMON_NS::String&           name() const {return name_;}
    //!
    const COMMON_NS::String&           value() const {return value_;}
    //!
    const Xpath::ExprImplPtr&          valueExpr() const {return valueExpr_;}

    bool  isAvt() const;

private:
    const GroveLib::Attr*       origin_;
    const COMMON_NS::String&    name_;
    const COMMON_NS::String&    value_;
    Xpath::ExprImplPtr          valueExpr_;
};

}

#endif //XSLT_ATTRIBUTE_VALUE_TEMPLATE_H
