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

#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/BuiltInTemplate.h"
#include "xslt/impl/debug.h"
#include "xpath/NodeSet.h"
#include "common/String.h"
#include "grove/Nodes.h"

USING_COMMON_NS;

namespace Xslt {

Template::Template(const GroveLib::Element* elem,
                   const Stylesheet* context,
                   Instruction* p,
                   bool  isBuiltin)
    : Instruction(elem, context, p),
      priorityNum_(0)
{
    if (isBuiltin)
        return;
    match_ = attr("match", false);
    name_  = attr("name",  false);
    mode_  = attr("mode",  false);
    priority_ = attr("priority", false);

    if (!match_.isNull())
        matchPattern_ = static_cast<PatternExprImpl*>(&*makePattern(match_));
    bool ok = false;
    priorityNum_ = priority_.toDouble(&ok);
    if (!ok)
        priorityNum_ = matchPattern_.isNull() ? 0 : matchPattern_->priority();
    buildSubInstructions();
}

bool Template::isApplicable(const Xpath::NodeSetItem& nsi) const
{
    if (matchPattern_.isNull())
        return false;
    return matchPattern_->isApplicable(nsi.node());
}

bool Template::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT :
        case INCLUDE :
        case ATTRIBUTE_SET :
        case DECIMAL_FORMAT :
        case KEY :
        case NAMESPACE_ALIAS :
        case OUTPUT :
        case PRESERVE_SPACE :
        case STRIP_SPACE :
        case SORT :
        case TEMPLATE :
            return false;
        default :
            return true;
    }
    return true;
}

void Template::dump() const
{
    if (isDefaultPriority()) {
        DDBG << "Template: match=" << match_ << " name=" << name_
            << " mode=" << mode_ << " priority=default" << std::endl;
    } else {
        DDBG << "Template: match=" << match_ << " name=" << name_
            << " mode=" << mode_ << " priority=" << priorityNum_ << std::endl;
    }
    if (!matchPattern_.isNull()) {
        DDBG << "MatchPattern:" << std::endl;
        matchPattern_->dump();
    }
    Instruction::dump();
}

/////////////////////

DummyTemplate::DummyTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

ElementTemplate::ElementTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true),
      ApplicatingInstruction(COMMON_NS::String(), context->topStylesheet())
{
}

TextAndAttrTemplate::TextAndAttrTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true),
        selectExpr_(PatternExpr::makeExpr("string(.)", 
        context->topStylesheet()->element()))
{
}

TextTemplate::TextTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

SectionTemplate::SectionTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

CommentTemplate::CommentTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

PiTemplate::PiTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

ChoiceTemplate::ChoiceTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

FoldTemplate::FoldTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

GenCopyTemplate::GenCopyTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
}

TextChoiceTemplate::TextChoiceTemplate(const Stylesheet* context)
    : Template(context->topStylesheet()->element(), context, 0, true)
{
    valueExpr_ = PatternExpr::makeExpr("string(.)",
        context->topStylesheet()->element());
}

BuiltinTemplateProvider::BuiltinTemplateProvider(const Stylesheet* ts)
    : elementTemplate_(ts), textAndAttrTemplate_(ts),
      textTemplate_(ts), commentTemplate_(ts), piTemplate_(ts),
      dummyTemplate_(ts), sectionTemplate_(ts), choiceTemplate_(ts),
      foldTemplate_(ts), genCopyTemplate_(ts), textChoiceTemplate_(ts)
{
}

BuiltinTemplateProvider::~BuiltinTemplateProvider()
{
}

} // namespace Xslt
