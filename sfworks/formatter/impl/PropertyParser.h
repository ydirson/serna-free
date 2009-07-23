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
#ifndef FO_PROPERTY_PARSER_H_
#define FO_PROPERTY_PARSER_H_

#include "formatter/types.h"
#include "formatter/impl/ParserContext.h"

namespace Formatter {
/*
expr:               term additiveExpr($1) { $ = $2 }
                    term                  { $ = term }
                    ;

addtitiveExpr:        '+' term { $ = $INP + $1 } additiveExpr($)
                    | '-' term { $ = $INP - $1 } additiveExpr($)
                    ;

term:               unaryExpr mulExpr($1) { $ = $2 }
                    | unaryExpr           { $ = $1 }
                    ;

mulExpr:            '*'  unaryExpr { $ = $INP * $1 }   mulExpr($)
                    '/'  unaryExpr { $ = $INP / $1 }   mulExpr($)
                    div  unaryExpr { $ = $INP div $1 } mulExpr($)
                    mod  unaryExpr { $ = $INP mod $1 } mulExpr($)
                    ;

unaryExpr:          factor        { $ = $1 }
                    | '-' factor  { $ = -$2 }
                    ;

factor:             '(' expr ')'
                    | NUMBER
                    | functionCall
                    ;
*/

class FORMATTER_EXPIMP PropertyParser {
public:
    typedef COMMON_NS::String CString;

    PropertyParser(const ParserContext& pc,
                   const Allocation& alloc, CType percentBase)
        : pc_(pc),
          alloc_(alloc),
          percentBase_(percentBase) {}

    /// Function for parsing properties with numeric values
    bool            parseNumeric(const CString& expr,
                                 const PropertyContext* prc,
                                 CType& result,
                                 bool& isRelative);

    /// Function for parsing properties with literal values
    bool            parseLiteral(const CString& expr,
                                 const PropertyContext* prc,
                                 COMMON_NS::String& result);
    /// Parse color
    bool            parseColor(const CString& expr,
                               const PropertyContext* prc,
                               uint& result);
    /// Returns property parsing error
    const CString&  errMsg() const { return errMsg_; }

    ///
    const ParserContext&    parserContext() const { return pc_; }

private:
    struct Token : public ValueTypePair {
        enum TokenType {
            T_NONE      = Value::V_NONE,
            T_NUMERIC   = Value::V_NUMERIC,
            T_NCNAME    = Value::V_NCNAME,
            T_COLOR     = Value::V_COLOR,
            T_FUNCTION,
            T_FINISH
        };
    };
    bool            parseExpr(Token& rv);
    bool            nextToken();
    bool            hasMoreTokens();
    bool            unaryExpr(Token& rv);
    bool            factor(Token& rv, bool useCurrent);
    bool            mulExpr(const Token& iv, Token& rv);
    bool            term(Token& rv);
    bool            additiveExpr(const Token& iv, Token& rv);
    bool            expr(Token& rv);
    CType           nextTerm();
    bool            evalFunc(const CString& funcName, FunctionArgList& al,
                             ValueTypePair& rv) const;
    bool            resolveToken(const CString& s, ValueTypePair& vp) const;
    CType           convertToNumeric(const ValueTypePair& iv) const;

    const ParserContext&    pc_;
    const Allocation&       alloc_;
    const CType             percentBase_;
    const COMMON_NS::Char*  cp_;
    const COMMON_NS::Char*  ce_;
    Token                   lastTok_;
    bool*                   isRelative_;
    const PropertyContext*  prc_;
    CString                 errMsg_;
};

}

#endif // FO_PROPERTY_PARSER_H_

