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
#include "xslt/impl/debug.h"
#include "xslt/impl/instructions/Number.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xpath/Engine.h"
#include "common/String.h"
#include "grove/Nodes.h"

USING_COMMON_NS;

namespace Xslt {

NumberConverter::NumberConverter(const String& formatString)
{
    String format(formatString);
    if (format.isEmpty())
        format = "1";
    String token(String::null());
    FormatTokenType type(LAST);

    //TODO: replace all occurences of string by chars
    // (String=char currently works bad in common)
    for (ulong c = 0; c < format.length(); c++) {
        const Char& ch(format.at(c));
        switch (ch.unicode()) {
            case '1' : {
                if (NUMERIC == type) {
                    token.append("1");
                    appendToken(token, type);
                }
                else {
                    appendToken(token, type);
                    token = "1";
                    type = NUMERIC;
                    appendToken(token, type);
                }
                break;
            }
            case 'a' : {
                appendToken(token, type);
                token = "a";
                type = ALPHABETIC;
                appendToken(token, type);
                break;
            }
            case 'A' : {
                appendToken(token, type);
                token = "A";
                type = ALPHABETIC;
                appendToken(token, type);
                break;
            }
            case 'i' : {
                appendToken(token, type);
                token = "i";
                type = ROMAN;
                appendToken(token, type);
                break;
            }
            case 'I' : {
                appendToken(token, type);
                token = "I";
                type = ROMAN;
                appendToken(token, type);
                break;
            }
            default: {
                if (ch.isDigit()) {
                    if (NUMERIC == type)
                        token.append(ch);
                    else {
                        appendToken(token, type);
                        token = QString(ch);
                        type = NUMERIC;
                    }
                }
                else {
                    if (SEPARATOR == type)
                        token.append(ch);
                    else {
                        appendToken(token, type);
                        token = QString(ch);
                        type = SEPARATOR;
                    }
                }
                break;
            }
        }
    }
    appendToken(token, type);
    //for (uint i = 0; i < tokenList_.size(); ++i) {
    //    DDBG << "Token: <" << tokenList_[i].token_
    //        << "> type: " << tokenList_[i].type_ << std::endl;
    //}
}

void NumberConverter::appendToken(String& token, FormatTokenType& type)
{
    if (!token.isEmpty())
        tokenList_.push_back(FormatToken(token, type));
    token = String::null();
    type = LAST;
}

void NumberConverter::toString(const NumberList& numList,
                               COMMON_NS::String& str) const
{
    const FormatToken* token = &tokenList_[0];
    const FormatToken* tokenEnd = &tokenList_[tokenList_.size()];
    const FormatToken* lastToken = token;

    if (0 == numList.size()) {
        str = "";
        return;
    }
    for (uint numIdx = 0; numIdx < numList.size(); ++numIdx) {
        if (numIdx && (token >= (tokenEnd - 1) || SEPARATOR != token->type_))
            str += '.';
        while (token < (tokenEnd - 1) && SEPARATOR == token->type_) {
            str += token->token_;
            ++token;
            if (token < tokenEnd && SEPARATOR != token->type_)
                lastToken = token;
        }
        uint num = numList[numIdx];
        const String& tok = lastToken->token_;
        switch (lastToken->type_) {
            case NUMERIC : {
                str += toDecimal(num);
                break;
            }
            case ALPHABETIC : {
                str += toLiteral(num, tok.upper() == tok);
                break;
            }
            case ROMAN : {
                str += toRoman(num, tok.upper() == tok);
                break;
            }
            default:
                break;
        }
        ++token;
    }
    if (SEPARATOR == (tokenEnd - 1)->type_)
        str += (tokenEnd - 1)->token_;
}

String NumberConverter::format() const
{
    String str;
    for (TokenList::const_iterator i = tokenList_.begin();
         i != tokenList_.end(); i++)
        if (SEPARATOR != (*i).type_)
            str += '[' + (*i).token_ + ']';
        else
            str += (*i).token_;
    return str;
}

String NumberConverter::toDecimal(ulong num) const
{
    String str;
    str.setNum(num);
    return str;
}

String NumberConverter::toRoman(ulong num, bool upper) const
{
    String alphabet = "ivxlcdm  "; // 1 5 10 50 100 500 1000 ...
    String str;
    String temp;
    temp.setNum(num);
    int len = temp.length();
    for( int i = 0; i < len; i++) {
        int pos = (len - i - 1)*2;
        int n = temp[i].digitValue();
        if (1 <= n && n <= 3 )
            while (n-- > 0)
                str += alphabet[pos];
        if (n == 4)
            str += alphabet[pos];
        if (n == 4 || n == 5)
            str += alphabet[pos + 1];
        if (6 <= n && n <= 8) {
            str += alphabet[pos + 1];
            n -= 5;
            while (n-- > 0)
                str += alphabet[pos];
        }
        if (9 == n) {
            str += alphabet[pos];
            str +=alphabet[pos + 2];
        }
    }
    if (upper)
        str = str.upper();
    return str;
}

String NumberConverter::toLiteral(ulong num, bool upper) const
{
    String str;
    String alphabet = "abcdefghijklmnopqrstuvwxyz";
    int alphabet_len = alphabet.length();

    while (num > 0) {
        int mod = num % alphabet_len;
        num = num / alphabet_len;
        if (0 == mod) {
            mod = alphabet_len;
            num--;
        }
        str = alphabet[mod - 1] + str;
    }
    if (upper)
        str = str.upper();
    return str;
}

///////////////////////////////////////////////////////////////////////
    
static String get_attr(const GroveLib::Element* elem, 
                       const Common::String& name)
{
    const GroveLib::Attr* a = elem->attrs().getAttribute(name);
    return a ? a->value() : String::null();
}

NumberInstructionBase::NumberInstructionBase(const GroveLib::Element* elem,
                                             const Xpath::FunctionFactory* fact)
    : element_(elem),
      errorId_(0),
      level_(SINGLE), 
      count_(get_attr(elem, "count")),
      from_(get_attr(elem, "from")),
      value_(get_attr(elem, "value")), 
      lang_(get_attr(elem, "lang")),
      separator_(get_attr(elem, "grouping-separator")), 
      groupingSize_(3),
      functionFactory_(fact)
{
    static XsltFunctionFactory xff;
    if (functionFactory_ == 0)
        functionFactory_ = &xff;
    const GroveLib::Attr* a = elem->attrs().getAttribute("format");
    if (a) {
        formatAvt_ = new AttributeValueTemplate(a, functionFactory_);
        if (!formatAvt_->isAvt() || formatAvt_->valueExpr().isNull()) {
            format_ = new NumberConverter(formatAvt_->value());
            formatAvt_.clear();
        }
    } else
        format_ = new NumberConverter(String::null());
    String str(get_attr(elem, "letter-value"));
    if (!str.isEmpty()) {
        if ("alphabetic" == str)
            letterValue_ = ALPHABETIC;
        else if ("traditional" == str)
            letterValue_ = TRADITIONAL;
        else {
            error(XsltMessages::instrNumberLetter);
            return;
        }
    }
    str = get_attr(elem, "level");
    if (!str.isEmpty()) {
        if ("any" == str)
            level_ = ANY;
        else if ("multiple" == str)
            level_ = MULTIPLE;
        else if ("single" == str)
            level_ = SINGLE;
        else {
            error(XsltMessages::instrNumberLevel);
            return;
        }
    }
    if (!value_.isNull()) {
        if (!count_.isNull() || !from_.isNull()) {
            error(XsltMessages::instrNumberCombine);
            return;
        }
        try {
            valueExpr_ = Xpath::Engine::makeExpr
                ("number(" + value_ + ")", elem, functionFactory_);
        }
        catch (Xpath::Exception& e) {
            error(XsltMessages::instrXpath);
            return;
        }
    }
    str = get_attr(elem, "grouping-size");
    if (!str.isNull())
        groupingSize_ = str.toInt();
    if (!count_.isNull())
        countPattern_ = PatternExpr::makePattern(count_, 
            elem, functionFactory_);
    if (!from_.isNull())
        fromPattern_ = PatternExpr::makePattern(from_, elem, functionFactory_);
}

String NumberInstructionBase::levelToString() const
{
    switch (level_) {
        case SINGLE :
            return "single";
        case MULTIPLE :
            return "multiple";
        case ANY :
        default:
            return "any";
    };
}

void NumberInstructionBase::dump() const
{
    if (!value_.isEmpty()) {
        DDBG << "SimpleNumber: value=" << value_ << " format "
            << format_->format() << std::endl;
        return;
    }
    DDBG << "Number: level=" << levelToString()
        << " count=" << count_ << " from=" << from_ << std::endl;
    {
        DDINDENT;
        DDBG << " format=" << format_->format()
            << " lang=" << lang_ << "letter-value="
            << ((ALPHABETIC == letterValue_) ? "ALPHABETIC" : "TRADITIONAL")
            << std::endl;
        else {
            if (!countPattern_.isNull()) {
                DDBG << "Count parsed:" << std::endl;
                countPattern_->dump();
            }
            if (!fromPattern_.isNull()) {
                DDBG << "From parsed:" << std::endl;
                fromPattern_->dump();
            }
        }
    }
}

NumberInstructionBase::~NumberInstructionBase()
{
}

/////////////////////////////////////////////////////////////////////////

Number::Number(const GroveLib::Element* elem, const Stylesheet* context,
               Instruction* p)
    : Instruction(elem, context, p),
      NumberInstructionBase(elem, context->topStylesheet()->functionFactory())
{
    if (errorId())
        throw Xslt::Exception((XsltMessages::Messages) errorId(), 
            contextString());
}

void Number::dump() const
{
    NumberInstructionBase::dump();
    Instruction::dump();
}

Instance* Number::makeInst(const InstanceInit& init, Instance* p) const
{
    return valueExpr().isNull() ? 
        makeNumberInst(init, p) : makeSimpleNumberInst(init, p);
}

} // namespace Xslt
