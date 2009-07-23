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
#ifndef XSLT_NUMBER_CLIENT_H_
#define XSLT_NUMBER_CLIENT_H_

#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
#include "xpath/ExprContext.h"
#include "xpath/FunctionFactory.h"
#include "common/XList.h"
#include "common/Vector.h"

namespace Xslt {

class NumberNodeCounter;
class NumberClient;
class NumberCache;
class NumberInstructionBase;
class AttributeValueTemplate;
class NumberConverter;

class XSLT_EXPIMP NodeCounterWatcher :
    public Common::XListItem<NodeCounterWatcher> {
public:
    NumberNodeCounter* nodeCounter() const;
    void               setClient(NumberClient* client) { client_ = client; }
    uint               getPos() const;
    void               numberChanged() const;
    ~NodeCounterWatcher();

private:
    NumberClient*      client_;
};

class XSLT_EXPIMP NumberClientBase {
public:
    NumberClientBase(const NumberInstructionBase* instr,
                     const GroveLib::Node* context);
    virtual ~NumberClientBase() {}

    virtual void numberChanged() = 0;
    virtual const NumberInstructionBase& numberInstruction() const = 0;
    virtual const NumberConverter* numberFormat() const;

protected:
    PatternExprPtr          countPattern_;

private:
    NumberClientBase(const NumberClientBase&);
    NumberClientBase& operator=(const NumberClientBase&);
};

class AnyNumberExprEntry;

class XSLT_EXPIMP NumberClient : public NumberClientBase {
public:

    NumberClient(const NumberInstructionBase* instr,
                 GroveLib::Node* context,
                 NumberCache* cache,
                 const Xpath::ExprContext&);

    virtual ~NumberClient() {}

protected:
    Common::String      toString() const;

private:
    typedef Common::Vector<NodeCounterWatcher> NumberLevelVec;
    NumberLevelVec          levels_;
};

inline void NodeCounterWatcher::numberChanged() const
{
    client_->numberChanged();
}

//////////////////////////////////////////////////////////////////////

class XSLT_EXPIMP AnyNumberClient : public NumberClientBase,
                                    public Common::XListItem<AnyNumberClient> {
public:
    typedef Common::XListItem<AnyNumberClient> ALI;

    AnyNumberClient(const NumberInstructionBase* instr,
                    GroveLib::Node* context,
                    NumberCache* cache,
                    const Xpath::ExprContext&);
    virtual ~AnyNumberClient();

protected:
    void                clientUpdate(GroveLib::Node*,
                                     const Xpath::ExprContext&);
    Common::String      toString() const;
    void                init(GroveLib::Node*,
                             const Xpath::ExprContext&,
                             const NumberInstructionBase&);

    Common::RefCntPtr<AnyNumberExprEntry> expr_entry_;
};

//////////////////////////////////////////////////////////////////////

class NumberConverter;

class XSLT_EXPIMP NumberInstructionBase {
public:
    enum Level {
        SINGLE, MULTIPLE, ANY
    };
    enum LetterValue {
        ALPHABETIC, TRADITIONAL
    };
    enum ClientType {
        CT_XSLT, CT_USER_DEFINED
    };
    NumberInstructionBase(const GroveLib::Element* element,
                          const Xpath::FunctionFactory* fact = 0);
    virtual ~NumberInstructionBase();

    Level                 level() const { return level_; }
    const Xpath::ExprPtr& valueExpr() const { return valueExpr_; }

    const PatternExprPtr& countPattern() const { return countPattern_; }
    const Common::String& countPatternString() const { return count_; }

    const PatternExprPtr& fromPattern() const { return fromPattern_; }
    const Common::String& fromPatternString() const { return from_; }
    //!

    int   errorId() const { return errorId_; }
    void  dump() const;
    const GroveLib::Element* element() const { return element_; }
    const AttributeValueTemplate* formatAvt() const
        { return formatAvt_.pointer(); }
    const NumberConverter* numberFormat() const
        { return format_.pointer(); }
    const Xpath::FunctionFactory* functionFactory() const
        { return functionFactory_; }

    virtual int clientType() const { return CT_USER_DEFINED; }

private:
    NumberInstructionBase(const NumberInstructionBase&);
    NumberInstructionBase& operator=(const NumberInstructionBase&);

    Common::String          levelToString() const;
    void                    error(int v) { errorId_ = v; }

    const GroveLib::Element* element_;
    Common::OwnerPtr<AttributeValueTemplate> formatAvt_;
    Common::OwnerPtr<NumberConverter>        format_;
    int                     errorId_;
    Level                   level_;
    const Common::String    count_;
    PatternExprPtr          countPattern_;
    const Common::String    from_;
    PatternExprPtr          fromPattern_;
    const Common::String    value_;
    Xpath::ExprPtr          valueExpr_;
    const Common::String    lang_;
    LetterValue             letterValue_;
    const Common::String    separator_;
    ulong                   groupingSize_;
    const Xpath::FunctionFactory* functionFactory_;
};

} // namespace Xslt

#endif // XSLT_NUMBER_CLIENT_H_
