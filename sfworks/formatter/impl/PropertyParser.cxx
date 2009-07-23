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
#include "common/String.h"
#include "formatter/Exception.h"
#include "formatter/impl/PropertyParser.h"
#include "formatter/impl/debug.h"

#include <exception>
#include <iostream>
#include <map>

namespace Formatter
{

// paramers are: literal function name, name in class, argument types array
#define REGISTER_FUNCTIONS \
    REGISTER_FUNCTION(min, \
                      minValue, \
                      minMaxArgTypes) \
    REGISTER_FUNCTION(max, \
                      maxValue, \
                      minMaxArgTypes) \
    REGISTER_FUNCTION(body-start, \
                      bodyStart, \
                      bodyStartArgTypes) \
    REGISTER_FUNCTION(label-end, \
                      labelEnd, \
                      labelEndArgTypes) \
    REGISTER_FUNCTION(proportional-column-width, \
                      proportionalColumnWidth, \
                      proportionalArgTypes) \
    REGISTER_FUNCTION(inherited-property-value, \
                      inheritedPropertyValue, \
                      inheritedPropertyArgTypes)

USING_COMMON_NS;

static const CType CM_PER_INCH = 2.54;
static const CType MM_PER_INCH = 25.4;
static const CType PT_PER_INCH = 72.;
static const CType PC_PER_INCH = PT_PER_INCH / 12.;

typedef bool (ParserContext::* FuncPtr)(const FunctionArgList& argList,
                                        ValueTypePair& returnValue,
                                        const PropertyContext* propContext,
const Allocation& alloc) const;

struct FuncPtrTypePair {
    FuncPtr                 func;
    const Value::ValueType* types;

    FuncPtrTypePair(FuncPtr fp, const Value::ValueType* t)
        : func(fp), types(t) {}
    FuncPtrTypePair() {}
};

typedef std::map<String, FuncPtrTypePair> FunctionMap;
static FunctionMap* funcMap = 0;

#define REGISTER_FUNCTION(funcname, fimplname, types) \
    (*funcMap)[#funcname] = \
        FuncPtrTypePair((FuncPtr)&ParserContext::fimplname, \
            ParserContext::types);

static inline bool ncchar(const Char& c)
{
    return c.isLetter() || c.isDigit() || c == '.'
        || c == '-' || c == '_';
}

bool PropertyParser::parseExpr(Token& t)
{
    if (!nextToken() || !expr(t)) {
        errMsg_ = "Expression string is empty";
        return false;
    }
    if (static_cast<int>(lastTok_.type_) != Token::T_FINISH) {
        errMsg_ = "Extra tokens in expression";
        return false;
    }
    return true;
}

bool PropertyParser::parseNumeric(const String& expr,
                                  const PropertyContext* prc,
                                  CType& result,
                                  bool& isRelative)
{
    Token t;
    cp_ = expr.unicode();
    ce_ = cp_ + expr.length();
    isRelative_   = &isRelative;
    prc_ = prc;
    try {
        if (!parseExpr(t))
            return false;
        if (static_cast<int>(t.type_) == Token::T_NCNAME &&
            !resolveToken(t.value_.asString(), t))
                throw Formatter::Exception(XslMessages::parseToken);
        if (static_cast<int>(t.type_) != Token::T_NUMERIC) {
            errMsg_ = "Numeric value expected";
            return false;
        }
        result = t.value_.nval_;
        return true;
    } catch (Formatter::Exception& e) {
        errMsg_ = e.what();
    }
    return false;
}

bool PropertyParser::parseColor(const String& expr,
                                const PropertyContext* prc,
                                uint& result)
{
    Token t;
    cp_ = expr.unicode();
    ce_ = cp_ + expr.length();
    prc_ = prc;
    try {
        if (!parseExpr(t))
            return false;
        if (static_cast<int>(t.type_) == Token::T_NCNAME &&
            !resolveToken(t.value_.asString(), t))
                throw Formatter::Exception(XslMessages::parseToken);

        switch (t.type_) {
            case Token::T_COLOR:
                result = t.value_.ival_;
                break;

            case Token::T_NUMERIC:
                result = uint(t.value_.nval_);
                break;

            default:
                errMsg_ = "Numeric or color value expected";
                return false;
        }
        return true;
    } catch (Formatter::Exception& e) {
        errMsg_ = e.what();
    }
    return false;
}

bool PropertyParser::parseLiteral(const String& expr,
                                  const PropertyContext* prc,
                                  CString& result)
{
    //TODO: remove patch
    result = expr;
    return true;

    Token t;
    cp_ = expr.unicode();
    ce_ = cp_ + expr.length();
    prc_ = prc;
    try {
        if (!parseExpr(t))
            return false;
        if (static_cast<int>(t.type_) == Token::T_NCNAME)
            result = t.value_.asString();
        else {
            errMsg_ = "Literal value expected";
            return false;
        }
        return true;
    }
    catch (Formatter::Exception& e) {
        errMsg_ = e.what();
    }
    return false;
}

bool PropertyParser::hasMoreTokens()
{
    while (cp_ < ce_ && cp_->isSpace())
        ++cp_;
    if (cp_ >= ce_)
        return false;
    return true;
}

bool PropertyParser::nextToken()
{
    if (!hasMoreTokens()) {
        lastTok_.type_ = Token::T_FINISH;
        return false;
    }
    DDBG << "considering '" << String(*cp_) << "'\n";
    switch (cp_->unicode()) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': {
            const Char* s = cp_;
            if (*s == '.')
                ++s;
            while (s < ce_ && s->isDigit())
                ++s;
            if (s >= ce_) {
                lastTok_.value_.nval_ = String(cp_, s - cp_, true).toDouble();
                lastTok_.type_ = Token::T_NUMERIC;
                cp_ = s;
                return true;
            }
            if (*s == '.') {
                ++s;
                while (s < ce_ && s->isDigit())
                    ++s;
            }
            lastTok_.value_.nval_ = String(cp_, s - cp_, true).toDouble();
            lastTok_.type_ = Token::T_NUMERIC;
            cp_ = s;
            if (cp_ >= ce_)
                return true;
            while (s < ce_ && s->isSpace())
                ++s;
            if (s >= ce_)
                return true;
            switch (s->unicode()) {
                case '%':   // percent
                    cp_ = ++s;
                    lastTok_.value_.nval_ =
                        (percentBase_ * lastTok_.value_.nval_) / 100.0;
                    *isRelative_ = true;
                    break;

                case 'c':   // cm
                    if (++s < ce_ && *s == 'm') {
                        lastTok_.value_.nval_ *= pc_.dpi() / CM_PER_INCH;
                        cp_ = ++s;
                    }
                    break;

                case 'm':   // mm
                    if (++s < ce_ && *s == 'm') {
                        lastTok_.value_.nval_ *= pc_.dpi() / MM_PER_INCH;
                        cp_ = ++s;
                    }
                    break;

                case 'i':   // in
                    if (++s < ce_ && *s == 'n') {
                        lastTok_.value_.nval_ *= pc_.dpi();
                        cp_ = ++s;
                    }
                    break;

                case 'e':   // em
                    if (++s < ce_ && *s == 'm') {
                        lastTok_.value_.nval_ *=
                            pc_.fontSize(prc_->propertyName());
                        cp_ = ++s;
                    }
                    break;

                case 'p':   // pt, pc, px
                    if (++s >= ce_)
                        break;
                    switch (s->unicode()) {
                        case 't':   // pt
                            lastTok_.value_.nval_ *= pc_.dpi() / PT_PER_INCH;
                            cp_ = ++s;
                            break;

                        case 'c':
                            lastTok_.value_.nval_ *= pc_.dpi() / PC_PER_INCH;
                            cp_ = ++s;

                            break;
                        case 'x':
                            cp_ = ++s;
                            break;

                        default:
                            break;
                    }
                }
            return true;
        }
        case '#': { // color
            const Char* s = ++cp_;
            while (s < ce_ && s->isSpace())
                ++s;
            uint rv = 0;
            bool pc = false;
            while (s < ce_) {
                rv <<= 4;
                switch (s->unicode()) {
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        rv |= (s->unicode() & 0xF);
                        pc = true;
                        break;

                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                        rv |= (s->unicode() - 'a' + 10);
                        pc = true;
                        break;

                    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                        rv |= (s->unicode() - 'A' + 10);
                        pc = true;
                        break;

                    default:
                        goto out1;
                }
                ++s;
            }
out1:       if (!pc)
                throw Formatter::Exception(XslMessages::parseColorNumber);
            cp_ = s;
            lastTok_.type_ = Token::T_COLOR;
            lastTok_.value_.ival_ = rv;
            return true;
        }
        case '+': case '-': case '*': case '/':
        case '(': case ')': case ',': {
            lastTok_.type_ = cp_++->unicode();
            return true;
        }
        default:
            break;
    }
    if (cp_->isLetter() || *cp_ == '_') { // NCNAME start
        const Char* s = cp_;
        while (s < ce_ && ncchar(*s))
            ++s;
        lastTok_.value_.sval_.sstart_ = cp_;
        lastTok_.value_.sval_.ssend_  = s;
        cp_ = s;
        while (s < ce_ && s->isSpace())
            ++s;
        if (s < ce_ && *s == '(') {
            cp_ = ++s;
            lastTok_.type_ = Token::T_FUNCTION;
            return true;
        }
        cp_ = s;
        lastTok_.type_ = Token::T_NCNAME;
        return true;
    }
    throw Formatter::Exception(XslMessages::parseChar);
}

bool PropertyParser::expr(Token& tv)
{
    Token t;
    if (term(t)) {
        if (hasMoreTokens() && additiveExpr(t, tv))
            return true;
        tv = t;
        return true;
    }
    return false;
}

bool PropertyParser::term(Token& rv)
{
    Token t, t1;
    if (!unaryExpr(t))
        return false;
    if (hasMoreTokens() && mulExpr(t, t1))
        rv = t1;
    else
        rv = t;
    return true;
}

CType PropertyParser::nextTerm()
{
    Token t;
    if (!nextToken())
        throw Formatter::Exception(XslMessages::parseExpr);
    if (!term(t))
        throw Formatter::Exception(XslMessages::parseTerm);
    if (static_cast<int>(t.type_) != Token::T_NUMERIC) {
        t.value_.nval_ = convertToNumeric(t);
        t.type_ = Token::T_NUMERIC;
    }
    return t.value_.nval_;
}

bool PropertyParser::additiveExpr(const Token& iv, Token& rv)
{
    CType dv;
    if (static_cast<int>(iv.type_) != Token::T_NUMERIC)
        dv = convertToNumeric(iv);
    else
        dv = iv.value_.nval_;
    bool hadMatch = false;
    for (;;) {
        switch (lastTok_.type_) {
            case '+': {
                dv += nextTerm();
                hadMatch = true;
                continue;
            }
            case '-': {
                dv -= nextTerm();
                hadMatch = true;
                continue;
            }
            default:
                rv.type_ = Token::T_NUMERIC;
                rv.value_.nval_ = dv;
                return hadMatch;
        }
    }
}

bool PropertyParser::mulExpr(const Token& iv, Token& rv)
{
    CType dv;
    if (static_cast<int>(iv.type_) != Token::T_NUMERIC)
        dv = convertToNumeric(iv);
    else
        dv = iv.value_.nval_;
    bool hadMatch = false;
    for (;;) {
        switch (lastTok_.type_) {
            case '*': {
                dv *= nextTerm();
                hadMatch = true;
                continue;
            }
            case '/': {
                CType v = nextTerm();
                if (0 == v)
                    throw Formatter::Exception(XslMessages::parseNaN);
                dv /= v;
                hadMatch = true;
                continue;
            }
            default:
                rv.type_ = Token::T_NUMERIC;
                rv.value_.nval_ = dv;
                return hadMatch;
        }
    }
}

bool PropertyParser::unaryExpr(Token& rv)
{
    Token t;
    if (lastTok_.type_ == '-') {
        if (!factor(t, false))
            return false;
        if (static_cast<int>(t.type_) != Token::T_NUMERIC)
            t.value_.nval_ = convertToNumeric(t);
        rv.type_ = Token::T_NUMERIC;
        rv.value_.nval_ = -t.value_.nval_;
        return true;
    }
    if (!factor(t, true))
        return false;
    rv = t;
    return true;
}

bool PropertyParser::factor(Token& rv, bool useCurrent)
{
    if (!useCurrent && !nextToken())
        return false;

    switch (lastTok_.type_) {
       default:
            rv = lastTok_;
            nextToken();
            break;

        case Token::T_FUNCTION: {
            Token t, ft(lastTok_);
            FunctionArgList al;
            for (;;) {
                if (!nextToken())
                    throw Formatter::Exception(XslMessages::parseBrace);
                if (lastTok_.type_ == ')') {
                    nextToken();
                    break;
                }
                if (!expr(t))
                    throw Formatter::Exception(XslMessages::parseFunc);
                al.push_back(t);
                if (lastTok_.type_ == ')') {
                    nextToken();
                    break;
                }
                if (lastTok_.type_ == ',')
                    continue;
                throw Formatter::Exception(XslMessages::parseCommaBrace);
            }
            return evalFunc(ft.value_.asString(), al, rv);
        }
        case '(':
            if (!nextToken() || !expr(rv) || lastTok_.type_ != ')')
                throw Formatter::Exception(XslMessages::parseBraceBal);
            nextToken();
            return true;
    }
    return true;
}

CType PropertyParser::convertToNumeric(const ValueTypePair& iv) const
{
    switch (iv.type_) {
        default:
            throw Formatter::Exception(XslMessages::parseConversion);

        case Token::T_NCNAME: {
            ValueTypePair vtp;
            if (!prc_->resolveEnumToken(iv.value_.asString(), 
                                        vtp, pc_, alloc_)) {
                throw Formatter::Exception(XslMessages::parseExplicitToken,
                                     iv.value_.asString());
            }
            if (static_cast<int>(vtp.type_) == Value::V_NUMERIC)
                return vtp.value_.nval_;
            if (static_cast<int>(vtp.type_) == Value::V_COLOR)
                return vtp.value_.ival_;
            throw Formatter::Exception(XslMessages::parseTokenToNum);
        }
        case Token::T_COLOR:
            return iv.value_.ival_;
    }
}

bool PropertyParser::evalFunc(const String& funcName, FunctionArgList& al,
                              ValueTypePair& rv) const
{
    DDBG << "Evaluating function: " << funcName << std::endl;
    if (0 == funcMap) {
        funcMap = new FunctionMap;
        REGISTER_FUNCTIONS;
    }
    FunctionMap::const_iterator ci = funcMap->find(funcName);
    if (ci == funcMap->end()) {
        throw Formatter::Exception(XslMessages::parseInvFunc, funcName);
    }
    const Value::ValueType* vt = ci->second.types;
    uint i = 0;
    for (; i < al.size() && *vt; ++i, ++vt) {
        const int argtype = al[i].type_;
        if (*vt != argtype) {
            if (*vt == Value::V_NUMERIC)
                al[i].value_.nval_ = convertToNumeric(al[i]);
            else if (argtype == Token::T_NCNAME) {
                ValueTypePair vtp;
                if (!prc_->resolveEnumToken(al[i].value_.asString(), 
                                            vtp, pc_, alloc_))
                    throw Formatter::Exception(XslMessages::parseToken);
                al[i].type_ = vtp.type_;
                al[i].value_ = vtp.value_;
            } else
                throw Formatter::Exception(XslMessages::parseFuncArg);
        }
    }
    if (*vt || i < al.size())
        throw Formatter::Exception(XslMessages::parseFuncNumArg);
    return (pc_.*(ci->second.func))(al, rv, prc_, alloc_);
}

bool PropertyParser::resolveToken(const String& s, ValueTypePair& vp) const
{
    if (s == "inherit") {
        FunctionArgList al;
        ValueTypePair tvp;
        tvp.type_ = Value::V_NCNAME;
        const String& pn = prc_->propertyName();
        tvp.value_.sval_.sstart_ = pn.unicode();
        tvp.value_.sval_.ssend_  = tvp.value_.sval_.sstart_ + pn.length();
        al.push_back(tvp);
        return pc_.inheritedPropertyValue(al, vp, prc_, alloc_);
    }
    return prc_->resolveEnumToken(s, vp, pc_, alloc_);
}

bool PropertyContext::resolveEnumToken(const String&, ValueTypePair&,
                                       const ParserContext&, 
                                       const Allocation&) const
{
    return false;
}

}
