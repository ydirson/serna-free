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

#ifndef XSLT_NUMBER_H
#define XSLT_NUMBER_H

#include "xslt/xslt_defs.h"
#include "xslt/NumberClient.h"
#include "xslt/impl/Instruction.h"
#include "common/Vector.h"

namespace Xslt {

class NumberConverter {
public:
    XSLT_OALLOC(NumberConverter);

    enum FormatTokenType {
        SEPARATOR, NUMERIC, ALPHABETIC, ROMAN, LAST
    };
    struct FormatToken {
        FormatToken(const COMMON_NS::String& token, FormatTokenType type)
            : token_(token), type_(type) {}

        COMMON_NS::String token_;
        FormatTokenType   type_;
    };
    typedef COMMON_NS::Vector<FormatToken>  TokenList;
    typedef COMMON_NS::Vector<ulong>        NumberList;

    NumberConverter(const COMMON_NS::String& formatString);

    void                   toString(const NumberList& numList,
                                    COMMON_NS::String& rs) const;
    COMMON_NS::String      format() const;

protected:
    COMMON_NS::String      toDecimal(ulong num) const;
    COMMON_NS::String      toRoman(ulong num, bool upper) const;
    COMMON_NS::String      toLiteral(ulong num, bool upper) const;
    void appendToken(COMMON_NS::String& token, FormatTokenType& type);

private:
    TokenList   tokenList_;
};

/*! \brief Number
 */
class Number : public Instruction, public NumberInstructionBase {
public:
    XSLT_OALLOC(Number);
    Number(const GroveLib::Element* element,
           const Stylesheet* context, Instruction* p);
    virtual ~Number() {}

    Type                type() const { return NUMBER; }

    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    Instance*   makeNumberInst(const InstanceInit&, Instance*) const;
    Instance*   makeSimpleNumberInst(const InstanceInit&, Instance*) const;
    virtual int clientType() const { return CT_XSLT; }
    
    virtual void    dump() const;

private:
    virtual bool    allowsChild(Type) const { return false; }
};

} // namespace Xslt

#endif // XSLT_NUMBER_H
