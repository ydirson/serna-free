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
#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#include "xslt/xslt_defs.h"
#include "xslt/XsltResource.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/CopyOf.h"
#include "xslt/impl/instructions/Attribute.h"
#include "xslt/impl/instructions/CalsTable.h"
#include "xslt/XsltMessages.h"
#include "xslt/impl/debug.h"
#include "xpath/FunctionFactory.h"
#include "common/String.h"
#include "common/Singleton.h"
#include "common/OwnerPtr.h"
#include "common/Factory.h"
#include "grove/Nodes.h"

#include <map>

USING_COMMON_NS

namespace Xslt {

template<class T> Instruction*
make(const GroveLib::Element* elem,
     const Stylesheet* context, Instruction* p)
{
    return new T(elem, context, p);
}

static Instruction* makeParam(const GroveLib::Element* elem,
                              const Stylesheet* context, Instruction* p)
{
    return (p && p->type() == Instruction::STYLESHEET)
        ? new TopParam(elem, context, p)
        : new Param(elem, context, p);
}

template<> Instruction*
make<Stylesheet>(const GroveLib::Element*, const Stylesheet*, Instruction*)
{
    return 0;
}

typedef Instruction*
    (*InstructionMaker)(const GroveLib::Element*,
                        const Stylesheet*, Instruction*);

struct MakerItem {
    const char*         name_;
    Instruction::Type   type_;
    InstructionMaker    maker_;
};

static MakerItem makers[] = {
    // top level`s
    { "stylesheet",      Instruction::STYLESHEET,        make<Stylesheet> },
    { "transform",       Instruction::STYLESHEET,        make<Stylesheet> },
    { "import",          Instruction::IMPORT,            make<Import> },
    { "include",         Instruction::INCLUDE,           make<Include> },
    { "strip-space",     Instruction::STRIP_SPACE,       make<StripSpace> },
    { "preserve-space",  Instruction::PRESERVE_SPACE,    make<PreserveSpace> },
    { "output",          Instruction::OUTPUT,            make<Output> },
    { "key",             Instruction::KEY,               make<Key> },
    { "decimal-format",  Instruction::DECIMAL_FORMAT,    make<DecimalFormat> },
    { "namespace-alias", Instruction::NAMESPACE_ALIAS,   make<NamespaceAlias> },
    { "attribute-set",   Instruction::ATTRIBUTE_SET,     make<AttributeSet> },
    { "variable",        Instruction::VARIABLE,          make<Variable> },
    { "param",           Instruction::PARAM,             makeParam },
    { "template",        Instruction::TEMPLATE,          make<Template> },
    // others
    { "apply-imports",   Instruction::APPLY_IMPORTS,     make<ApplyImports> },
    { "apply-templates", Instruction::APPLY_TEMPLATES,   make<ApplyTemplates> },
    { "attribute",       Instruction::ATTRIBUTE,         make<Attribute> },
    { "call-template",   Instruction::CALL_TEMPLATE,     make<CallTemplate> },
    { "choose",          Instruction::CHOOSE,            make<Choose> },
    { "copy",            Instruction::COPY,              make<Copy> },
    { "copy-of",         Instruction::COPY_OF,           make<CopyOf> },
    { "element",         Instruction::ELEMENT,           make<Element> },
    { "for-each",        Instruction::FOR_EACH,          make<ForEach> },
    { "if",              Instruction::IF,                make<If> },
    { "message",         Instruction::MESSAGE,           make<Message> },
    { "number",          Instruction::NUMBER,            make<Number> },
    { "otherwise",       Instruction::OTHERWISE,         make<Otherwise> },
    { "sort",            Instruction::SORT,              make<Sort> },
    { "text",            Instruction::TEXT,              make<Text> },
    { "value-of",        Instruction::VALUE_OF,          make<ValueOf> },
    { "when",            Instruction::WHEN,              make<When> },
    { "with-param",      Instruction::WITH_PARAM,        make<WithParam> },
    { "processing-instruction",
        Instruction::PROCESSING_INSTRUCTION, make<ProcessingInstruction> },
    { "comment",
        Instruction::COMMENT,                make<Comment> },
    { "fallback",        Instruction::FALLBACK,          make<Fallback> },
    //
    {   "literal-result-element",
        Instruction::LITERAL_RESULT_ELEMENT, make<LiteralResult>
    },
    { "cals-table-group", Instruction::CALS_TABLE_GROUP, make<CalsTableGroup> },
    { "cals-table-row",   Instruction::CALS_TABLE_ROW,   make<CalsTableRow> },
    { "cals-table-cell",  Instruction::CALS_TABLE_CELL,  make<CalsTableCell> },
    // unknown
    { "unknown",         Instruction::UNKNOWN_INSTRUCTION,  make<Unknown> },
    { "ignored",         Instruction::IGNORED_INSTRUCTION,  make<Ignored> }
};

static const int MAKERS_SZ = sizeof(makers) / sizeof(makers[0]);

static inline bool is_valid_type(Instruction::Type type)
{
    return (type > 0) && (type < Instruction::LAST_INSTRUCTION);
}

class DynamicInstructionFactory : public InstructionFactory {
public:
    XSLT_OALLOC(DynamicInstructionFactory);
    typedef std::map<String, Instruction::Type> TypeMap;
    typedef TypeMap::const_iterator             TypeIterator;

    //! Singleton factory constructor
    DynamicInstructionFactory();
    virtual ~DynamicInstructionFactory() {}
    //!
    Instruction::Type   getTypeOf(const GroveLib::Node* styleNode,
                                  const Stylesheet* context) const;
    //!
    TopStylesheet*      makeTopStylesheet(const GroveLib::Node* styleNode,
                                          XsltResource* xr) const;
    //!
    Stylesheet*         makeStylesheet(const GroveLib::Node* styleNode,
                                       const Stylesheet* context,
                                       const String& origin,
                                       Instruction* p) const;

    //! Makes instruction according to given node from the style grove
    Instruction*        makeInstruction(const GroveLib::Node* styleNode,
                                        Instruction::Type type,
                                        const Stylesheet* context,
                                        Instruction* p) const;
    //!
    void setProgressStream(MessageStream* pstream) { pstream_ = pstream; }

private:
    TypeMap                 typeMap_;
    static InstructionMaker makerVector_[Instruction::LAST_INSTRUCTION];
    MessageStream*          pstream_;
};

InstructionMaker
DynamicInstructionFactory::makerVector_[Instruction::LAST_INSTRUCTION];

DynamicInstructionFactory::DynamicInstructionFactory()
    : pstream_(0)
{
    for (int c = 0; c < MAKERS_SZ; ++c) {
        typeMap_[makers[c].name_] = makers[c].type_;
        makerVector_[makers[c].type_] = makers[c].maker_;
    }
}

Instruction::Type
DynamicInstructionFactory::getTypeOf(const GroveLib::Node* styleNode,
                                     const Stylesheet* context) const
{
    switch (styleNode->nodeType()) {
        case GroveLib::Node::ELEMENT_NODE : {
            const GROVE_NAMESPACE::Element* element =
                static_cast<const GroveLib::Element*>(styleNode);

            if (isXsltNsUri(element->xmlNsUri())) {
                TypeIterator i = typeMap_.find(element->localName());
                if (i != typeMap_.end())
                    return (*i).second;
                return Instruction::UNKNOWN_INSTRUCTION;
            }
            else if (context && context->isExtNode(styleNode))
                return Instruction::IGNORED_INSTRUCTION;
            else
                return Instruction::LITERAL_RESULT_ELEMENT;
        }
        case GroveLib::Node::TEXT_NODE : {
            return Instruction::LITERAL_RESULT_TEXT;
        }
        default :
            throw Xslt::Exception(XsltMessages::factoryBadNodetype);
    }
    return Instruction::UNKNOWN_INSTRUCTION; //! To keep compiler happy
}

static inline const GroveLib::Element*
as_grove_element(const GroveLib::Node* node)
{
    return static_cast<const GroveLib::Element*>(node);
}

TopStylesheet*
DynamicInstructionFactory::makeTopStylesheet(const GroveLib::Node* styleNode,
                                             XsltResource* xr) const
{
    if (GroveLib::Node::ELEMENT_NODE != styleNode->nodeType())
        throw Xslt::Exception(XsltMessages::factoryBadNode);

    const GroveLib::Element* element = as_grove_element(styleNode);
    OwnerPtr<TopStylesheet> ts(new TopStylesheet(element, xr));
    ts->buildSubInstructions(true);
    ts->combineStylesheets();
    return ts.release();
}

Stylesheet*
DynamicInstructionFactory::makeStylesheet(const GroveLib::Node* styleNode,
                                          const Stylesheet* context,
                                          const String& origin,
                                          Instruction* p) const
{
    if (GroveLib::Node::ELEMENT_NODE != styleNode->nodeType())
        throw Xslt::Exception(XsltMessages::factoryBadNode);

    const GroveLib::Element* element = as_grove_element(styleNode);
    OwnerPtr<Stylesheet> s(new Stylesheet(element, origin, p, context));
    s->buildSubInstructions(false);
    return s.release();
}

Instruction*
DynamicInstructionFactory::makeInstruction(const GroveLib::Node* styleNode,
                                           Instruction::Type type,
                                           const Stylesheet* context,
                                           Instruction* p) const
{
    using namespace GroveLib;
    const Node::NodeType ndType = styleNode->nodeType();

    if (Instruction::LITERAL_RESULT_TEXT == type && Node::TEXT_NODE == ndType) {
        const GroveLib::Text* t(static_cast<const GroveLib::Text*>(styleNode));
        return new LiteralResultText(t, context, p);
    }

    if (Node::ELEMENT_NODE != ndType)
        throw Xslt::Exception(XsltMessages::factoryBadNode);

    const GroveLib::Element* element = as_grove_element(styleNode);
    XsltMessages::Messages msg = XsltMessages::factoryErrInstr;

    if (is_valid_type(type) && 0 != makerVector_[type]) {
        if (Instruction* ins = makerVector_[type](element, context, p))
            return ins;
    }
    else
        msg = XsltMessages::factoryInvInstr;
    throw Xslt::Exception(msg, element->xmlNsPrefix(), element->localName());
}
                                   
extern const char xslt_ext_uri[] = SERNA_XPATH_EXTENSIONS_URI;
                                   
bool isXsltNsUri(const COMMON_NS::String& uri)
{
    return (("http://www.w3.org/1999/XSL/Transform" == uri) || // XSLT1.0
            ("http://www.w3.org/XSL/Transform/1.0" == uri)  || // WD 19990421
            ("http://www.w3.org/TR/WD-xsl" == uri) || // some old stuff use this
            (xslt_ext_uri == uri));
}

} // namespace Xslt

namespace Common {

template<> Xslt::InstructionFactory*
Factory<Xslt::InstructionFactory>::instance()
{
    return &SingletonHolder<Xslt::DynamicInstructionFactory>::instance();
}

} // namespace Common
