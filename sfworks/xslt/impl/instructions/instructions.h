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

#ifndef XSLT_INSTRUCTIONS_H
#define XSLT_INSTRUCTIONS_H

#include "xpath/Expr.h"
#include "xslt/xslt_defs.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/instructions/Sort.h"
#include "xslt/impl/instructions/Number.h"
#include "xslt/impl/instructions/ApplicatingInstruction.h"
#include "xslt/impl/instructions/Attribute.h"
#include "common/prtti.h"
#include <list>

namespace Xslt {

class ApplyImports : public Instruction,
                                 public ApplicatingInstruction {
public:
    XSLT_OALLOC(ApplyImports);

    ApplyImports(const GroveLib::Element* element,
                 const Stylesheet* context, Instruction* p);
    virtual ~ApplyImports() {}

    Type                type() const { return APPLY_IMPORTS; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    virtual void        dump() const;

    PRTTI_DECL(ApplicatingInstruction);

protected:
    virtual bool allowsChild(Type) const { return false; }

private:
    Xpath::ExprPtr      selectExpr_;
};

class ApplyTemplates : public Instruction,
                                   public ApplicatingInstruction {
public:
    XSLT_OALLOC(ApplyTemplates);

    ApplyTemplates(const GroveLib::Element* element,
                   const Stylesheet* context, Instruction* p);
    virtual ~ApplyTemplates() {}

    const COMMON_NS::String& mode() const { return mode_; }
    Type                type() const { return APPLY_TEMPLATES; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

    PRTTI_DECL(ApplicatingInstruction);

protected:
    virtual bool allowsChild(Type type) const;

private:
    COMMON_NS::String   mode_;
};

// A proxy class which allows using attributes defined elsewhere
class UseAttributeInstruction : public Instruction {
public:
    XSLT_OALLOC(UseAttribute);

    UseAttributeInstruction(const Instruction* attribute)
        : Instruction(attribute->element(), attribute->styleContext(), 0),
          attribute_(attribute) {}

    Type                type() const { return USE_ATTRIBUTE; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const
    {
        return attribute_->makeInst(init, p);
    }
    virtual void        dump() const { attribute_->dump(); }

private:
    virtual bool allowsChild(Type) const { return false; }
    const Instruction* attribute_;
};

class UseAttributeSet {
public:
    class AttrSet;
    class UseSet;

    UseAttributeSet(const GroveLib::Element* element, Instruction* p);

    // Creates individual UseAttributeInstruction's out of AttributeSet.
    // Note that this function must be called AFTER buildSubInstructions()
    void        createAttributes() const;

private:
    COMMON_NS::String   useString_;
    Instruction*        p_;
};

class AttributeSet : public Instruction {
public:
    typedef UseAttributeSet::AttrSet AttrSet;
    typedef UseAttributeSet::UseSet  UseSet;

    XSLT_OALLOC(AttributeSet);

    AttributeSet(const GroveLib::Element* element,
                 const Stylesheet* context, Instruction* p);
    virtual ~AttributeSet();

    Type    type() const { return ATTRIBUTE_SET; }
    virtual void dump() const;

    const COMMON_NS::String& name() const { return name_; }

    // for internal use
    void    merge(const AttributeSet* other) const;
    void    collect_attributes(AttrSet&, UseSet&) const;
private:
    AttributeSet(const AttributeSet&);

protected:
    class ChildAttrMap;
    virtual bool allowsChild(Type type) const { return (ATTRIBUTE == type); }

    COMMON_NS::String name_;
    COMMON_NS::OwnerPtr<UseSet> useSet_;
    COMMON_NS::OwnerPtr<ChildAttrMap> childMap_;
};

////////////////////////////////////////////////////////////////////////

class Template;

class CallTemplate : public Instruction {
public:
    XSLT_OALLOC(CallTemplate);

    CallTemplate(const GroveLib::Element* element,
                 const Stylesheet* context, Instruction* p);
    virtual ~CallTemplate() {}

    const COMMON_NS::String& name() const { return name_; }
    Type                type() const { return CALL_TEMPLATE; }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;
    void                resolve();
    const Template*     callTemplate() const { return template_; }

protected:
    bool                allowsChild(Type type) const;

private:
    const COMMON_NS::String name_;
    const Template*     template_;
};

class When : public Instruction  {
public:
    XSLT_OALLOC(When);

    When(const GroveLib::Element* element,
         const Stylesheet* context, Instruction* p);
    virtual ~When() {};

    const Xpath::ExprPtr& testExpr() const { return testExpr_; }
    Type                type() const { return WHEN; }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;

private:
    const COMMON_NS::String test_;
    Xpath::ExprPtr          testExpr_;
};

class Otherwise : public Instruction  {
public:
    XSLT_OALLOC(Otherwise);
    Otherwise(const GroveLib::Element* element,
              const Stylesheet* context, Instruction* p);
    virtual ~Otherwise() {};

    Type                type() const { return OTHERWISE; }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;
};

class Choose : public Instruction {
public:
    XSLT_OALLOC(Choose);

    typedef std::list<const When*>      WhenList;
    typedef WhenList::const_iterator    WhenIterator;

    Choose(const GroveLib::Element* element,
           const Stylesheet* context, Instruction* p);
    virtual ~Choose() {};

    Type                type() const { return CHOOSE; }
    const WhenList&     whenList() const { return whenList_; }
    const Instruction*  otherwise() const;
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;
    void                addChild(Instruction* child);

private:
    WhenList            whenList_;
};

class Copy : public Instruction,
                         public UseAttributeSet {
public:
    XSLT_OALLOC(Copy);

    Copy(const GroveLib::Element* element,
         const Stylesheet* context, Instruction* p);
    virtual ~Copy() {};

    Type            type() const { return COPY; }
    Instance*       makeInst(const InstanceInit& init, Instance* p) const;
    void            dump() const;

protected:
    bool            allowsChild(Type type) const;
};

class DecimalFormat : public Instruction {
public:
    XSLT_OALLOC(DecimalFormat);

    DecimalFormat(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~DecimalFormat() {}

    Type            type() const { return DECIMAL_FORMAT; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const {return false;}
};

class ProcessingInstruction : public Instruction {
public:
    XSLT_OALLOC(ProcessingInstruction);

    ProcessingInstruction(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~ProcessingInstruction() {}

    Type            type() const { return PROCESSING_INSTRUCTION; }
    void            dump() const;
    Instance*       makeInst(const InstanceInit& init, Instance* p) const;

protected:
    bool            allowsChild(Type) const {return false;}
};

class Comment : public Instruction {
public:
    XSLT_OALLOC(Comment);

    Comment(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~Comment() {}

    Type            type() const { return COMMENT; }
    void            dump() const;
    Instance*       makeInst(const InstanceInit& init, Instance* p) const;

protected:
    bool            allowsChild(Type) const {return false;}
};

class Element : public Instruction,
                public UseAttributeSet {
public:
    XSLT_OALLOC(Element);

    typedef AttributeValueTemplate  AttrTemplate;
    typedef COMMON_NS::OwnerPtr<AttrTemplate>  AttrTemplateOwner;

    Element(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~Element() {};

    Type                type() const { return ELEMENT; }
    const AttrTemplate* name() const { return name_.pointer(); }
    const AttrTemplate* ns() const { return namespace_.pointer(); }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;

private:
    AttrTemplateOwner   name_;
    AttrTemplateOwner   namespace_;
};

class Fallback : public Instruction {
public:
    XSLT_OALLOC(Fallback);

    Fallback(const GroveLib::Element* element,
             const Stylesheet* context, Instruction* p);
    virtual ~Fallback() {}

    Type            type() const { return FALLBACK; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const { return false; }
};

class ForEach : public Instruction {
public:
    XSLT_OALLOC(ForEach);

    ForEach(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~ForEach();

    Type                type() const { return FOR_EACH; }
    const Xpath::Expr&  selectExpr() const { return *selectExpr_.pointer(); }
    const Sort::List&   sortSpecs() const { return sortList_; }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;
    void                addChild(Instruction* child);

private:
    Sort::List          sortList_;
    const COMMON_NS::String select_;
    Xpath::ExprPtr      selectExpr_;
};

class If : public Instruction {
public:
    XSLT_OALLOC(If);

    If(const GroveLib::Element* element,
       const Stylesheet* context, Instruction* p);
    virtual ~If() {};

    Type                type() const { return IF; }
    const Xpath::Expr&  testExpr() const { return *testExpr_.pointer(); }
    Instance*           makeInst(const InstanceInit& init, Instance* p) const;
    void                dump() const;

protected:
    bool                allowsChild(Type type) const;

private:
    const COMMON_NS::String test_;
    Xpath::ExprPtr      testExpr_;
};

class Ignored : public Instruction {
public:
    XSLT_OALLOC(Ignored);

    Ignored(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~Ignored() {}

    Type            type() const { return IGNORED_INSTRUCTION; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const {return false;}
};

//
// Provides common functionality for Import and Include
//
class IncludeBase : public Instruction {
public:
    IncludeBase(const GroveLib::Element* element,
                const Stylesheet* context, Instruction* p);
    virtual ~IncludeBase() {}

protected:
    COMMON_NS::String uriRef_;
};

class Import : public IncludeBase {
public:
    XSLT_OALLOC(Import);

    Import(const GroveLib::Element* element,
           const Stylesheet* context, Instruction* p);
    virtual ~Import() {}

    Type    type() const { return IMPORT; }
    void    dump() const;

protected:
    bool    allowsChild(Type) const { return false; }
};

class Include : public IncludeBase {
public:
    XSLT_OALLOC(Include);

    Include(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~Include() {}

    Type    type() const { return INCLUDE; }
    void    dump() const;

protected:
    bool    allowsChild(Type) const { return false; }
};

class Key : public Instruction {
public:
    XSLT_OALLOC(Key);

    Key(const GroveLib::Element* element,
        const Stylesheet* context, Instruction* p);
    virtual ~Key() {}

    Type            type() const { return KEY; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const {return false;}
};

//////////////////////////////////////////////////////////////////////

//! Auxilliary which keeps a set of namespace mappings

class LiteralResult : public Instruction,
                      public UseAttributeSet {
public:
    XSLT_OALLOC(LiteralResult);

    typedef std::list<AttributeValueTemplate*>  AttrValueList;
    typedef AttrValueList::const_iterator       AttrValueIterator;

    LiteralResult(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~LiteralResult();

    Type                    type() const { return LITERAL_RESULT_ELEMENT; }
    const COMMON_NS::String& elementName() const;
    const AttrValueList&    attributes() const { return attributes_; }
    Instance*               makeInst(const InstanceInit& init,
                                     Instance* p) const;
    void                    dump() const;
    static void             copyPrefixesTo(const GroveLib::Element* from,
                                           GroveLib::Element* to);
protected:
    bool                    allowsChild(Type type) const;
    AttrValueList           attributes_;
};

class LiteralResultText : public Instruction {
public:
    XSLT_OALLOC(LiteralResultText);
    LiteralResultText(const GroveLib::Text* text,
                      const Stylesheet* context, Instruction* p);
    virtual ~LiteralResultText() {};

    Type                    type() const { return LITERAL_RESULT_TEXT; }
    const COMMON_NS::String& text() const { return node_->data(); }
    const GroveLib::Text*   node() const { return node_; }
    Instance*               makeInst(const InstanceInit& init,
                                     Instance* p) const;
    void                    dump() const;

protected:
    bool                    allowsChild(Type) const {return false;}

private:
    const GroveLib::Text*   node_;
};

class NamespaceAlias : public Instruction {
public:
    XSLT_OALLOC(NamespaceAlias);

    NamespaceAlias(const GroveLib::Element* element,
                   const Stylesheet* context, Instruction* p);
    virtual ~NamespaceAlias() {}

    Type            type() const { return NAMESPACE_ALIAS; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const { return false; }
};

class Output : public Instruction {
public:
    XSLT_OALLOC(Output);

    Output(const GroveLib::Element* element,
           const Stylesheet* context, Instruction* p);
    virtual ~Output() {}

    Type            type() const { return OUTPUT; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const { return false; }
};

class PreserveSpace : public Instruction {
public:
    XSLT_OALLOC(PreserveSpace);

    PreserveSpace(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~PreserveSpace() {}

    const COMMON_NS::String& elements() const { return elements_; }
    Type            type() const { return PRESERVE_SPACE; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const { return false; }
    COMMON_NS::String elements_;
};

class StripSpace : public Instruction {
public:
    XSLT_OALLOC(StripSpace);

    StripSpace(const GroveLib::Element* element,
               const Stylesheet* context, Instruction* p);
    virtual ~StripSpace() {}

    const COMMON_NS::String& elements() const { return elements_; }
    Type            type() const { return STRIP_SPACE; }
    void            dump() const;

protected:
    bool            allowsChild(Type) const { return false; }
    COMMON_NS::String elements_;
};

class Text : public Instruction {
public:
    XSLT_OALLOC(Text);

    Text(const GroveLib::Element* element,
         const Stylesheet* context, Instruction* p);
    virtual ~Text() {}

    Type            type() const { return TEXT; }
    const COMMON_NS::String& text() const { return text_; }
    Instance*       makeInst(const InstanceInit& init, Instance* p) const;
    void            dump() const;

protected:
    bool            allowsChild(Type) const {return false;}

private:
    COMMON_NS::String text_;
};

class Unknown : public Instruction {
public:
    XSLT_OALLOC(Unknown);

    Unknown(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~Unknown() {}

    Type            type() const { return UNKNOWN_INSTRUCTION; }
    void            dump() const;

protected:
    bool            allowsChild(Type type) const { return FALLBACK == type; }
};

class ValueOf : public Instruction {
public:
    XSLT_OALLOC(ValueOf);

    ValueOf(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~ValueOf() {}

    const Xpath::ExprPtr&   selectExpr() const { return selectExpr_; }
    Type                    type() const { return VALUE_OF; }
    Instance*               makeInst(const InstanceInit& init,
                                     Instance* p) const;
    void                    dump() const;

protected:
    bool                    allowsChild(Type) const {return false;}

private:
    COMMON_NS::String       select_;
    Xpath::ExprPtr          selectExpr_;
};

class Message : public Instruction {
public:
    XSLT_OALLOC(Message);

    Message(const GroveLib::Element* element,
            const Stylesheet* context, Instruction* p);
    virtual ~Message() {}

    Type                    type() const { return MESSAGE; }
    Instance*               makeInst(const InstanceInit& init,
                                     Instance* p) const;
    const Xpath::ExprPtr&   messageExpr() const { return messageExpr_; }
    bool                    terminate() const { return terminate_; }
    void                    dump() const;

protected:
    bool                    allowsChild(Type type) const;
    Xpath::ExprPtr          messageExpr_;
    bool                    terminate_;
};

class Param;

class Variable : public Instruction {
public:
    XSLT_OALLOC(Variable);

    Variable(const GroveLib::Element* element,
             const Stylesheet* context, Instruction* p);
    virtual ~Variable() {}

    const COMMON_NS::String& name() const { return name_; }
    const Xpath::ExprPtr&   varExpr() const { return varExpr_; }
    void                    setConstString(const Common::String&) const;
    Type                    type() const { return VARIABLE; }
    Instance*               makeInst(const InstanceInit& init,
                                     Instance* p) const;
    void                    dump() const;

protected:
    bool                    allowsChild(Type type) const;

private:
    COMMON_NS::String       name_;
    Xpath::ExprPtr          varExpr_;
};

class WithParam : public Variable {
public:
    XSLT_OALLOC(WithParam);

    WithParam(const GroveLib::Element* element,
              const Stylesheet* context, Instruction* p);
    virtual ~WithParam() {}

    Type            type() const { return WITH_PARAM; }
    Instance*       makeInst(const InstanceInit& init,
                             Instance* p) const;
    void            dump() const;
};

class Param : public Variable {
public:
    XSLT_OALLOC(Param);

    Param(const GroveLib::Element* element,
          const Stylesheet* context, Instruction* p);
    virtual ~Param() {}

    Type            type() const { return PARAM; }
    Instance*       makeInst(const InstanceInit& init, Instance* p) const;
    void            dump() const;
};

class TopParam : public Param {
public:
    XSLT_OALLOC(TopParam);

    TopParam(const GroveLib::Element* element,
             const Stylesheet* context, Instruction* p);
    virtual ~TopParam() {}

    const Common::String& valueType() const { return valueType_; }
    const Common::String& annotation() const { return annotation_; }

    Instance*       makeInst(const InstanceInit& init, Instance* p) const;

private:
    Common::String  valueType_;
    Common::String  annotation_;
};

} // namespace Xslt

#endif // XSLT_INSTRUCTIONS_H
