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

#ifndef XSLT_BUILT_IN_TEMPLATE_H
#define XSLT_BUILT_IN_TEMPLATE_H

#include "xpath/Engine.h"
#include "xslt/xslt_defs.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/ApplicatingInstruction.h"

namespace Xslt {

class DummyTemplate : public Template {
public:
    XSLT_OALLOC(DummyTemplate);

    DummyTemplate(const Stylesheet* context);
    virtual ~DummyTemplate() {};
    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class ElementTemplate : public Template,
                                    public ApplicatingInstruction {
public:
    XSLT_OALLOC(ElementTemplate);

    ElementTemplate(const Stylesheet* context);
    virtual ~ElementTemplate() {};

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;

    PRTTI_DECL(ApplicatingInstruction);
};

class TextAndAttrTemplate : public Template {
public:
    XSLT_OALLOC(TextAndAttrTemplate);

    TextAndAttrTemplate(const Stylesheet* context);
    virtual ~TextAndAttrTemplate() {};

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
    //!
    const Xpath::Expr&  selectExpr() const { return *selectExpr_.pointer(); }

private:
    Xpath::ExprPtr      selectExpr_;
};

class TextTemplate : public Template {
public:
    XSLT_OALLOC(TextTemplate);

    TextTemplate(const Stylesheet* context);
    virtual ~TextTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class SectionTemplate : public Template {
public:
    XSLT_OALLOC(SectionTemplate);

    SectionTemplate(const Stylesheet* context);
    virtual ~SectionTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class CommentTemplate : public Template {
public:
    XSLT_OALLOC(CommentTemplate);

    CommentTemplate(const Stylesheet* context);
    virtual ~CommentTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class PiTemplate : public Template {
public:
    XSLT_OALLOC(PiTemplate);

    PiTemplate(const Stylesheet* context);
    virtual ~PiTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class ChoiceTemplate : public Template {
public:
    XSLT_OALLOC(ChoiceTemplate);

    ChoiceTemplate(const Stylesheet* context);
    virtual ~ChoiceTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class TextChoiceTemplate : public Template {
public:
    XSLT_OALLOC(TextChoiceTemplate);

    TextChoiceTemplate(const Stylesheet* context);
    virtual ~TextChoiceTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
    const Xpath::Expr& valueExpr() const { return *valueExpr_; }

private:
    Xpath::ExprPtr  valueExpr_;    
};

class FoldTemplate : public Template {
public:
    XSLT_OALLOC(FoldTemplate);

    FoldTemplate(const Stylesheet* context);
    virtual ~FoldTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class GenCopyTemplate : public Template {
public:
    XSLT_OALLOC(GenCopyTemplate);

    GenCopyTemplate(const Stylesheet* context);
    virtual ~GenCopyTemplate() {}

    //! Reimplemented from Instruction
    virtual Instance* makeInst(const InstanceInit& init, Instance* p) const;
};

class BuiltinTemplateProvider {
public:
    BuiltinTemplateProvider(const Stylesheet*);
    ~BuiltinTemplateProvider();

    Template*   getElementTemplate() const { return &elementTemplate_; }
    Template*   getTextAndAttrTemplate() const { return &textAndAttrTemplate_; }
    Template*   getTextTemplate() const { return &textTemplate_; }
    Template*   getDummyTemplate() const { return &dummyTemplate_; }
    Template*   getSectionTemplate() const { return &sectionTemplate_; }
    Template*   getCommentTemplate() const { return &commentTemplate_; }
    Template*   getPiTemplate() const { return &piTemplate_; }
    Template*   getChoiceTemplate() const { return &choiceTemplate_; }
    Template*   getFoldTemplate() const { return &foldTemplate_; }
    Template*   getGenCopyTemplate() const { return &genCopyTemplate_; }
    Template*   getTextChoiceTemplate() const { return &textChoiceTemplate_; }

private:
    mutable ElementTemplate     elementTemplate_;
    mutable TextAndAttrTemplate textAndAttrTemplate_;
    mutable TextTemplate        textTemplate_;
    mutable CommentTemplate     commentTemplate_;
    mutable PiTemplate          piTemplate_;
    mutable DummyTemplate       dummyTemplate_;
    mutable SectionTemplate     sectionTemplate_;
    mutable ChoiceTemplate      choiceTemplate_;
    mutable FoldTemplate        foldTemplate_;
    mutable GenCopyTemplate     genCopyTemplate_;
    mutable TextChoiceTemplate  textChoiceTemplate_;
};

} // namespace Xslt

#endif // XSLT_BUILT_IN_TEMPLATE_H
