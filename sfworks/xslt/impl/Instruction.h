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

#ifndef XSLT_INSTRUCTION_H
#define XSLT_INSTRUCTION_H

#include "common/String.h"
#include "common/RefCounted.h"
#include "common/XTreeNode.h"
#include "common/prtti.h"
#include "common/SList.h"
#include "xslt/xslt_defs.h"
#include "xslt/Exception.h"
#include "xslt/PatternExpr.h"

namespace GroveLib
{
    class Node;
    class Element;
}

namespace Xslt {

class Stylesheet;
class Instance;
class InstanceInit;
class ApplicatingInstruction;

class Instruction : public Common::RefCounted<>,
                    public Common::XTreeNode<Instruction,
                    Common::XTreeNodeRefCounted<Instruction> > {
public:

    enum Type {
        FIRST_INSTRUCTION = 0,
        // Top level instructions
        STYLESHEET = FIRST_INSTRUCTION,
        ATTRIBUTE_SET,
        DECIMAL_FORMAT,
        IMPORT,
        INCLUDE,
        KEY,
        NAMESPACE_ALIAS,
        OUTPUT,
        PARAM,
        PRESERVE_SPACE,
        STRIP_SPACE,
        TEMPLATE,
        VARIABLE,
        // Other instructions
        APPLY_IMPORTS,
        APPLY_TEMPLATES,
        ATTRIBUTE,
        UNPROCESSED_ATTRIBUTE,
        USE_ATTRIBUTE,
        CALL_TEMPLATE,
        CHOOSE,
        COMMENT,
        COPY,
        COPY_OF,
        ELEMENT,
        FALLBACK,
        FOR_EACH,
        IF,
        MESSAGE,
        NUMBER,
        OTHERWISE,
        PROCESSING_INSTRUCTION,
        SORT,
        TEXT,
        VALUE_OF,
        WHEN,
        WITH_PARAM,
        CALS_PROCESS_TGROUP,
        LITERAL_RESULT_ELEMENT,
        LITERAL_RESULT_TEXT,
        CALS_TABLE_GROUP,
        CALS_TABLE_ROW,
        CALS_TABLE_CELL,
        CALS_TABLE_CELLATTRS,
        GENERATED_COPY,
        IGNORED_INSTRUCTION,
        UNKNOWN_INSTRUCTION,
        LAST_INSTRUCTION
    };

    Instruction(const GroveLib::Element* element,
                const Stylesheet* styleContext, Instruction* p);
    virtual ~Instruction();

    //! Some sub-instructions do additional checks in addChild
    virtual void            addChild(Instruction* child);
    //!
    virtual Instance*       makeInst(const InstanceInit& init,
                                     Instance* p) const;
    //!
    virtual Type            type() const = 0;
    //!
    const GroveLib::Element* element() const;
    //!
    virtual void            dump() const;
    //!
    const Stylesheet*       styleContext() const { return stylesheet_; }
    //
    void                    buildSubInstructions(bool isTopLevel = false);
    //
    void                    warning(const GroveLib::Node* node,
                                    const Common::String& msg) const;

    // used only by stylesheet combiner
    void                    setStyleContext(const Stylesheet*);

    Common::String          contextString() const;

    /// Compile pattern expression. May throw XSLT exceptions.
    PatternExprPtr          makePattern(const Common::String&) const;
    Xpath::ExprPtr          makeExpr(const Common::String&) const;

    PRTTI_DECL(ApplicatingInstruction);

protected:
    //!
    virtual bool            allowsChild(Type type) const = 0;
    //!
    Common::String       name(const GroveLib::Node* node) const;
    //!
    Common::String       attr(const Common::String& name,
                                 bool isRequired = false,
                                 bool valMustBeNcn = false) const;
    void                    checkAttributes();

private:
    struct AttrItem : public Common::SListItem<AttrItem> {
        Common::String attr;
        AttrItem(const Common::String& n) : attr(n) {}
    };
    typedef Common::SList<AttrItem> AttrList;

    mutable AttrList         attrList_;
    const GroveLib::Element* element_;
    const Stylesheet*        stylesheet_;
};

} // namespace Xslt

#endif // XSLT_INSTRUCTION_H
