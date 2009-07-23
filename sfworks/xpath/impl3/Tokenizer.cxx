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
#include "xpath/xpath_defs.h"
#include "xpath/impl3/Tokenizer.h"
#include "xpath/Exception.h"
#include "xpath/xpathParser.hpp"
#include "common/String.h"
#include "xpath/impl3/debug.h"

#define YYEOF 0

USING_COMMON_NS;

namespace Xpath {

static inline bool ncchar(const Char& c)
{
    return c.isLetter() || c.isDigit() || c.unicode() == '.'
        || c.unicode() == '-' || c.unicode() == '_';
}

struct TokTable {
    const char* name;
    int         len;
    int         token;
};

static TokTable op_table[] = {
    { "and",      3,  OPAND    },
    { "mod",      3,  OPMOD    },
    { "div",      3,  OPDIV    },
    { "or",       2,  OPOR     },
    { "//",       2,  OP2SLASH },
    { "!=",       2,  OPNEQ    },
    { "<=",       2,  OPLTEQ   },
    { ">=",       2,  OPGTEQ   },
    { "::",       2,  OPDCOLON },
    { "..",       2,  T2DOT    },
    { "|",        1,  OPUNION  },
    { "/",        1,  OPSLASH  },
    { "+",        1,  OPPLUS   },
    { "-",        1,  OPMINUS  },
    { "=",        1,  OPEQ     },
    { "<",        1,  OPLT     },
    { ">",        1,  OPGT     },
    { 0,          0,  0 }
};

static TokTable nt_table[] = {
    { "processing-instruction", 22, NTEST_PI },
    { "comment",    7,              NTEST_COMMENT },
    { "node",       4,              NTEST_NODE },
    { "text",       4,              NTEST_TEXT },
    { 0,            0, 0 }
};

static int getTok(const TokTable* t, const Char* cp,
                  const Char* ce, int* tlen = 0, bool* exact = 0)
{
    int rlen = ce - cp;
    const Char* ic;
    const char* nc;
    int oplen;
    for (const TokTable* op = t; op->name; ++op) {
        oplen = op->len;
        if (oplen <= rlen) {
            ic = cp;
            nc = op->name;
            while (oplen > 0 && *ic++ == *nc++)
                --oplen;
            if (oplen)
                continue;
            if (tlen)
                *tlen = op->len;
            if (exact)
                *exact = (op->len == rlen);
            return op->token;
        }
    }
    return 0;
}

#define LRETURN(t) { prevToken_ = t; return prevToken_; }

int XpathTokenizer::getToken(TokenValue* v)
{
    if (tokidx_ < tokstack_.size()) {
        *v = tokstack_[tokidx_].tokval;
        return tokstack_[tokidx_++].tok;
    }
    while (cp_ < ce_ && cp_->isSpace())
        ++cp_;
    if (cp_ >= ce_)
        return YYEOF;
    int op = -1;
    bool ncname_is_operator = false;
    switch (prevToken_) {
        case YYEOF:
        case OPAND:
        case OPOR:
        case OPMOD:
        case OPDIV:
        case OPMUL:
        case OPUNION:
        case OPSLASH:
        case OP2SLASH:
        case OPPLUS:
        case OPMINUS:
        case OPEQ:
        case OPNEQ:
        case OPLT:
        case OPLTEQ:
        case OPGT:
        case OPGTEQ:
        case OPDCOLON:
        case '@':
        case '(':
        case '[':
        case '$':
        case ':':
        case ',':
            break;
        default:
            if (*cp_ == '*') {
                cp_++;
                LRETURN(OPMUL);
            }
            ncname_is_operator = true;
            break;
    }
    if (cp_->isLetter() || *cp_ == '_') { // possibly ncname start
        const Char* s = cp_;
        while (s < ce_ && ncchar(*s))
            ++s;
        // lookahead check - if function name
        if ((s + 2) < ce_ && s[0] == ':' && s[1] != ':') {
            const QChar* ss = s + 2;
            for (; ss < ce_ && ncchar(*ss); ++ss)
                ;
            for (; ss < ce_ && ss->isSpace(); ++ss)
                ;
            if (ss < ce_ && *ss == '(')
                s = ss;
        }
        v->sval.sstart = cp_;
        v->sval.send  = s;
        DBG(XPATH.PARSER) << "tokenizer: parsing ncname: <"
            << String(cp_, s - cp_) << "> is-op="
            << ncname_is_operator << std::endl;
        if (ncname_is_operator) {
            op = getTok(op_table, cp_, s);
            if (op > 0) {
                cp_ = s;
                DBG(XPATH.PARSER) << "tokenizer: returning OP"
                    << std::endl;
                LRETURN(op);
            }
            throw Xpath::Exception(XpathMessages::parserOpExpected);
            LRETURN(YYEOF);
        }
        if (s >= ce_) {
            cp_ = ce_;
            DBG(XPATH.PARSER) << "tokenizer: returning NCNAME" << std::endl;
            LRETURN(NCNAME);
        }
        const Char* s1 = s;
        while(s1 < ce_ && s1->isSpace())
            ++s1;
        cp_ = s1;
        if (s1 >= ce_)
            return YYEOF;
        if (*s1 == '(') {
            bool exact = false;
            op = getTok(nt_table, v->sval.sstart, s1, 0, &exact);
            if (op > 0 && exact) {
                DBG(XPATH.PARSER) << "tokenizer: retuning NTEST"
                    << String(v->sval.sstart, ce_ - v->sval.sstart)
                    << std::endl;
                LRETURN(op);
            }
            DBG(XPATH.PARSER) << "tokenizer: returning FUNCNAME"
                << String(v->sval.sstart, v->sval.send - v->sval.sstart)
                << std::endl;
            LRETURN(FUNCTIONNAME);
        } else if (*s1 == ':') {
            if (++s1 >= ce_)
                LRETURN(':');
            if (*s1 == ':')
                LRETURN(AXISNAME);
        }
        DBG(XPATH.PARSER) << "tokenizer: returning NCNAME(2)" << std::endl;
        LRETURN(NCNAME);
    }
    if (cp_->isDigit()) {
        const Char* s = cp_;
        while (s < ce_ && s->isDigit())
            ++s;
        if (s >= ce_) {
            v->nval = String(cp_, s - cp_, true).toDouble();
            cp_ = s;
            LRETURN(TNUMBER);
        }
        if (*s == '.') {
            ++s;
            while (s < ce_ && s->isDigit())
                ++s;
        }
        v->nval = String(cp_, s - cp_, true).toDouble();
        cp_ = s;
        LRETURN(TNUMBER);
    }
    if (*cp_ == '\'' || *cp_ == '"') {
        Char quot = *cp_++;
        const Char* s = cp_;
        while (s < ce_ && *s != quot)
            ++s;
        if (s >= ce_) { // unterminated string
            throw Xpath::Exception(XpathMessages::parserBadStr);
            return YYEOF;
        }
        v->sval.sstart = cp_;
        v->sval.send   = s;
        cp_ = ++s;
        LRETURN(LITERAL);
    }
    int tlen = 0;
    op = getTok(op_table, cp_, ce_, &tlen);
    if (op > 0) {
        cp_ += tlen;
        LRETURN(op);
    }
    LRETURN(cp_++->unicode());
}

void XpathTokenizer::pushToken(int tok, TokenValue tv)
{
    tokvalpair tokp;
    tokp.tok    = tok;
    tokp.tokval = tv;
    tokstack_.push_back(tokp);
}

/////////////////////////////////////////////////////////////////////

String TokenValue::asString() const
{
    return String(sval.sstart, sval.send - sval.sstart);
}

} // namespace Xpath
