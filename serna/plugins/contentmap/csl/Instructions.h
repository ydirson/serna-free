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
#ifndef CSL_INSTRUCTIONS_H_
#define CSL_INSTRUCTIONS_H_

#include "csl/csl_defs.h"
#include "common/XList.h"
#include "common/XTreePolicies.h"
#include "grove/Nodes.h"
#include "xslt/NumberClient.h"

namespace Xpath {
    class Expr;
}

namespace Csl {

class Instruction;
class SubInstance;
class Instance;

typedef Common::XTreeNodeRefCounted<Instruction> InstructionTreePolicy;

class Instruction : public Common::RefCounted<>,
                    public Common::XListItem<Instruction,
                                             InstructionTreePolicy> {
public:
    enum Type {
        TEXT, NUMBER, COLLECT_TEXT, VALUE_OF
    };
    virtual const Type   type() const = 0;
    virtual SubInstance* makeInst(const Instance*) const = 0;
    virtual void         dump() const = 0;
    
    Instruction() {}
    virtual ~Instruction() {}
    
private:
    Instruction(const Instruction&);
    Instruction& operator=(const Instruction&);
};

class InstructionList : 
    public Common::XList<Instruction, InstructionTreePolicy> {};

//////////////////////////////////////////////////////////////////////

class Text : public Instruction {
public:
    CSL_OALLOC(TextInstr);
    
    Text(const GroveLib::Element* elem);
    virtual ~Text() {}

    virtual const Type    type() const { return TEXT; }
    const Common::String& text() const { return text_; }
    virtual SubInstance*  makeInst(const Instance*) const;
    virtual void          dump() const;

private:
    Common::String text_;
};

//////////////////////////////////////////////////////////////////////

class CollectText : public Instruction {
public:
    CSL_OALLOC(CollectTextInstr);
    
    CollectText(const GroveLib::Element* elem);
    virtual ~CollectText() {}

    virtual const Type   type() const { return COLLECT_TEXT; }
    const Xpath::Expr&   selectExpr() const { return *selectExpr_; }
    uint                 maxLength() const { return maxLength_; }
    virtual SubInstance* makeInst(const Instance*) const;
    bool                 firstNodeOnly() const { return firstNodeOnly_; }
    virtual void         dump() const;

private:
    Common::RefCntPtr<Xpath::Expr>  selectExpr_;   
    uint                            maxLength_;
    bool                            firstNodeOnly_;
};

//////////////////////////////////////////////////////////////////////

class ValueOf : public CollectText {
public:
    CSL_OALLOC(ValueOfInstr);
    
    ValueOf(const GroveLib::Element* elem)
        : CollectText(elem) {}
    virtual ~ValueOf() {}

    virtual const Type   type() const { return VALUE_OF; }
    virtual SubInstance* makeInst(const Instance*) const;
    virtual void         dump() const;
};    

//////////////////////////////////////////////////////////////////////

class Number : public Instruction,
               public Xslt::NumberInstructionBase {
public:
    Number(const GroveLib::Element* elem);
    virtual const Type type() const { return NUMBER; }
    virtual SubInstance* makeInst(const Instance*) const;
    virtual void         dump() const;
};

} // namespace Csl

#endif // CSL_INSTRUCTIONS_H_

