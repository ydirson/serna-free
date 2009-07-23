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
#ifndef XPATH_IMPL_TOKENIZER_H_
#define XPATH_IMPL_TOKENIZER_H_

#include "xpath/xpath_defs.h"
#include "common/String.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/Vector.h"
#include "grove/XmlName.h"

namespace Xslt {
    class PatternExprImpl;
}
namespace Xpath {

class ExprImpl;
class FunctionFactory;

class PQname : public GROVE_NAMESPACE::QualifiedName,
               public COMMON_NS::RefCounted<> {
public:
    PQname(const COMMON_NS::String& name)
        : QualifiedName(name) {}
    PQname(const COMMON_NS::String& name, const COMMON_NS::String& pref)
        : QualifiedName(name, pref) {}
};

// Defines semantic value of tokens.
union XPATH_EXPIMP TokenValue {
    double nval;             // numeric value
    struct Sval {            // string value
        const COMMON_NS::Char* sstart;
        const COMMON_NS::Char* send;
    } sval;
    ExprImpl* expr;
    PQname* qname;
    Xslt::PatternExprImpl* pexpr;           // for XSLT pattern grammar only
    COMMON_NS::String asString() const; // returns string value as String
};

class XPATH_EXPIMP XpathTokenizer {
public:
    XpathTokenizer(const COMMON_NS::String& sv)
        : sv_(sv), prevToken_(0), tokidx_(0)
    {
        cp_ = sv_.unicode();
        ce_ = cp_ + sv_.length();
        tokstack_.reserve(16);
    }
    XpathTokenizer()
        : cp_(0), ce_(0), prevToken_(0), tokidx_(0) {}

    int                            getToken(TokenValue* v);
    COMMON_NS::RefCntPtr<ExprImpl>
        makeExpr(const GroveLib::NodeWithNamespace* nsRes,
                 const FunctionFactory* f);
    void                           pushToken(int, TokenValue);

private:
    struct tokvalpair { int tok; TokenValue tokval; };
    COMMON_NS::Vector<tokvalpair> tokstack_;
    COMMON_NS::String      sv_;
    const COMMON_NS::Char* cp_;
    const COMMON_NS::Char* ce_;
    int  prevToken_;
    uint tokidx_;
};

} // namespace Xpath

#endif // XPATH_IMPL_TOKENIZER_H_
