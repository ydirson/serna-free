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
#include "xslt/impl/Tokenizer.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/xsltParser.hpp"
#ifdef USE_XPATH3
# include "xpath/impl3/Tokenizer.h"
# include "xpath/xpathParser.hpp"
# include "xpath/impl3/ExprImpl.h"
#else
# include "xpath/impl/Tokenizer.h"
# include "xpath/impl/xpathParser.hpp"
# include "xpath/impl/ExprImpl.h"
#endif // USE_XPATH3

USING_COMMON_NS;

#define YYEOF 0

namespace Xslt {

void xpp_error(const char* s);

#define LRETURN(tokname) { *v = xtv; return tokname; }
#define MAP2(tokname, newname) \
    case tokname : LRETURN(newname)

int XsltTokenizer::getToken(TokenValue* v)
{
    Xpath::TokenValue xtv;
    int xptok = xpt_.getToken(&xtv);
    if (xptok <= 0)
        return xptok;
    switch(xptok) {
        MAP2(LITERAL, XT_LITERAL);      MAP2(OPSLASH, XT_OPSLASH);
        MAP2(OP2SLASH, XT_OP2SLASH);    MAP2(AXISNAME, XT_AXISNAME);
        MAP2(NTEST_NODE, XT_NTEST_NODE);MAP2(NTEST_COMMENT, XT_NTEST_COMMENT);
        MAP2(NTEST_TEXT, XT_NTEST_TEXT);MAP2(NTEST_PI, XT_NTEST_PI);
        MAP2(NCNAME, XT_NCNAME);        MAP2(OPUNION, XT_BAR);
        MAP2(OPDCOLON, XT_OPDCOLON);

        case FUNCTIONNAME: {
            if (xtv.asString() == "id")
                LRETURN(XT_IDFUNC);
            if (xtv.asString() == "key")
                LRETURN(XT_KEYFUNC);
            xpp_error("Only 'id' and 'key' functions allowed"
                      " in pattern expressions");
            return YYEOF;
        }
        case '[': {  // predicate start
            int plevel = 1;
            Xpath::XpathTokenizer xpt2;
            while (plevel > 0) {
                Xpath::TokenValue xtv2;
                int xptok2 = xpt_.getToken(&xtv2);
                if (xptok2 <= 0) {
                    xpp_error("Unexpected end of predicate expression");
                    return YYEOF;
                }
                if (xptok2 == '[')
                    ++plevel;
                else if (xptok2 == ']')
                    --plevel;
                if (plevel > 0)
                    xpt2.pushToken(xptok2, xtv2);
            }
            // we need to manually increase refcnt here because otherwise
            // expression will be deleted on return before we have chance
            // to assign it to the new RefCntPtr. This is because we cannot
            // use RefCntPtr's in the %union
            RefCntPtr<Xpath::ExprImpl> rexpr = 
                xpt2.makeExpr(resolver_, funcFactory_);
            rexpr->incRefCnt();
            v->expr = rexpr.pointer();
            return XT_XPATHEXPR;
        }
        default:
            break;
    }
    *v = xtv;
    if (xptok < 255)
        return xptok;
    xpp_error("Bad token in pattern expression");
    return YYEOF;
}

} // namespace Xpath
