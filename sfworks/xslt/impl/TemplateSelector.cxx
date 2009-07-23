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
#include "xslt/ResultOrigin.h"
#include "xslt/impl/instances/ApplicatingInstance.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/TemplateSelector.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/BuiltInTemplate.h"
#include "xslt/impl/instances/TemplateInst.h"
#include "xslt/impl/debug.h"
#include "grove/ChoiceNode.h"
#include "grove/udata.h"

// name for builtin template used for folding
static const char* SERNA_FOLD_TMPL = "serna.fold.template";
static const char* SERNA_RSS_TMPL  = "serna.redline.start.template";
static const char* SERNA_RSE_TMPL  = "serna.redline.end.template";

using namespace Common;

namespace Xslt {

TemplateSelector::TemplateSelector(const Xpath::NodeSetItem& nsi,
                                   const ApplicatingInstance* appi,
                                   const GroveLib::Node* resultParent)
    : nsi_(&nsi),
      applicatingInst_(appi),
      selected_(0),
      templateInstance_(0),
      isModified_(false),
      isFolded_(isFolded(resultParent))
{
    if (isFolded_ || nsi.node()->isGroveSectionNode() ||
        (nsi.node()->udata() & GroveLib::XSLT_GEN_BIT))
            return;
    choose_template();
}

static inline double match_priority(const Template* t,
                                    const PatternInstPtr& pinst)
{
    if (t->isDefaultPriority())
        return pinst->matchPriority();
    return t->priority();
}

void TemplateSelector::choose_template()
{
    TemplateProvider::TemplateIterator ti =
        applicatingInst_->templateProvider()->
            getFirstTemplate(applicatingInst_->mode(),
                applicatingInst_->importsOnly());

    const Template* delayed_template = 0;
    double delayed_prio = 0;

    DBG(XSLT.TSEL) << "TemplateSelector: choose_template() entered\n";
    for (;;) {
        const Template* t = ti.nextTemplate();
        if (0 == t)
            break;
        DBG(XSLT.TSEL) << "+ tsel nsi & template: " << std::endl;
        DBG_EXEC(XSLT.TSEL, (nsi_->node()->dump(), t->dump()));
        if (!t->isApplicable(*nsi_))
            continue;
        DBG(XSLT.TSEL) << "tsel Applicable OK" << std::endl;
        if (delayed_template && delayed_prio >= t->priority()) {
            selected_ = delayed_template;
            break;
        }
        PatternInstPtr pinst(0,
            static_cast<PatternInst*>(t->matchExpr()->makeInst(*nsi_, *this)));
        if (pinst.isNull())
            throw Xslt::Exception(XsltMessages::instanceInit);
        if (pinst->value()->getBool()) { // matces
            DBG(XSLT.TSEL) << "tsel: template matched: mprio "
                << match_priority(t, pinst) << " tprio " << t->priority()
                << std::endl;
            if (delayed_template && delayed_prio >= match_priority(t, pinst)) {
                selected_ = delayed_template;
                DBG(XSLT.TSEL) << "tsel: delayed selected" << std::endl;
                delayed_template = 0;
                break;
            }
            if (match_priority(t, pinst) >= t->priority()) {
                selected_ = t;
                DBG(XSLT.TSEL) << "tsel: primary template selected"
                    << std::endl;
                delayed_template = 0;
                break;
            } else {
                delayed_template = t;
                delayed_prio = match_priority(t, pinst);
            }
        }
    }
    if (delayed_template)
        selected_ = delayed_template;
}

void TemplateSelector::update()
{
    isFolded_ = isFolded(applicatingInst_->resultContext().parent());
    DBG(XSLT.UPDATE) << "TemplateSelector::update() called, fold="
        << isFolded_ << std::endl;
    DBG(XSLT.UPDATE) << "TemplateSelector:: context node="
        << nsi_->node()->nodeName() << " (" << nsi_ << ")" << std::endl;
    selected_ = 0;
    deregisterFromAllNodes();
    wlist_.destroyAll();
    if (isFolded_)
        return;
    choose_template();
}

const Template* TemplateSelector::selectedTemplate() const
{
    if (selected_)
        return selected_;
    return getBuiltinTemplate();
}

void TemplateSelector::setChanged()
{
    DBG(XSLT.UPDATE) << "TemplateSelector:: received CHANGED event\n";
    if (!isModified()) {
        SubscriberPtrPublisher::notifyChanged();
        setModified(true);
    }
}
    
bool TemplateSelector::watchFor(SubscriberPtrPublisher* p) const
{
    wlist_.push_front(new TsValueWatcher
        (const_cast<TemplateSelector*>(this), p));
    return true;
}

void TemplateSelector::registerVisitor(const GroveLib::Node* node,
                                       GroveLib::NodeVisitor*,
                                       short mask) const
{
    const_cast<GroveLib::Node*>(node)->
        registerNodeVisitor(const_cast<TemplateSelector*>(this), mask);
}

Xpath::ValueHolder*
TemplateSelector::getVariable(const COMMON_NS::String& v) const
{
    return applicatingInst_->getVariable(v);
}

Common::String TemplateSelector::contextString() const
{
    return applicatingInst_->contextString();
}

void TemplateSelector::nodeDestroyed(const GroveLib::Node*)
{
}

void TemplateSelector::childInserted(const GroveLib::Node*)
{
    nodeChanged();
}

void TemplateSelector::childRemoved (const GroveLib::Node*,
                                     const GroveLib::Node*)
{
    nodeChanged();
}

void TemplateSelector::attributeChanged(const GroveLib::Attr*)
{
    nodeChanged();
}

void TemplateSelector::attributeRemoved(const GroveLib::Element*,
                                        const GroveLib::Attr*)
{
    nodeChanged();
}

void TemplateSelector::attributeAdded(const GroveLib::Attr*)
{
    nodeChanged();
}

void TemplateSelector::textChanged(const GroveLib::Text*)
{
    nodeChanged();
}

bool TemplateSelector::isFolded(const GroveLib::Node* resultNode) const
{
    if (!applicatingInst_->canFold())
        return false;
    const GroveLib::Node* gsr = resultNode->getGSR();
    if (!gsr || gsr->nodeType() != GroveLib::Node::DOCUMENT_NODE)
        return false;   // result is RTF -- no folding
    // if nsi_ within source document, return bit immediately
    if (nsi_->node()->getGSR()->nodeType() == GroveLib::Node::DOCUMENT_NODE)
        return GroveLib::is_node_folded(nsi_->node());
    const GroveLib::Node* origin = resultOrigin(nsi_->node());
    return origin && GroveLib::is_node_folded(origin);
}

static inline bool can_omit_ers(const GroveLib::GroveSectionStart* gss)
{
    const GroveLib::EntityDecl* const ed = CONST_ERS_CAST(gss)->entityDecl();
    if (0 == ed)
        return true;
    return ed->isSingleCharEntity();
}

const Template* TemplateSelector::getBuiltinTemplate() const
{
    const BuiltinTemplateProvider& tp =
        applicatingInst_->styleContext().topStylesheet()->builtinTemplates();

    if (isFolded_) {
        const Template* fold_template =
            applicatingInst_->templateProvider()->getTemplate(SERNA_FOLD_TMPL);
        if (fold_template)
            return fold_template;
        else
            return tp.getFoldTemplate();
    }
    if (nsi_->node()->udata() & GroveLib::XSLT_GEN_BIT) 
        return tp.getGenCopyTemplate();
    switch (nsi_->node()->nodeType()) {
        case GroveLib::Node::CHOICE_NODE:
            return tp.getChoiceTemplate();

        case GroveLib::Node::ELEMENT_NODE:
        case GroveLib::Node::DOCUMENT_NODE:
        case GroveLib::Node::DOCUMENT_FRAGMENT_NODE:
            return tp.getElementTemplate();

        case GroveLib::Node::TEXT_NODE: 
            return (nsi_->node()->nodeName()[1] == 't')
                ? tp.getTextTemplate() 
                : tp.getTextChoiceTemplate();

        case GroveLib::Node::ATTRIBUTE_NODE:
            return tp.getTextAndAttrTemplate();

        case GroveLib::Node::COMMENT_NODE:
            return tp.getCommentTemplate();

        case GroveLib::Node::PI_NODE:
            return tp.getPiTemplate();
        
        case GroveLib::Node::ENTITY_REF_START_NODE:
            return can_omit_ers(CONST_ERS_CAST(nsi_->node()))
                ? tp.getDummyTemplate() : tp.getSectionTemplate();
             
        case GroveLib::Node::ENTITY_REF_END_NODE: 
            return can_omit_ers(CONST_ERE_CAST(nsi_->node())->getSectStart())
                ? tp.getDummyTemplate() : tp.getSectionTemplate();
        
        case GroveLib::Node::REDLINE_START_NODE: {
            const Template* rss_template = applicatingInst_->
                templateProvider()->getTemplate(SERNA_RSS_TMPL);
            return rss_template ? rss_template : tp.getSectionTemplate();
        }
        case GroveLib::Node::REDLINE_END_NODE: {
            const Template* rse_template = applicatingInst_->
                templateProvider()->getTemplate(SERNA_RSE_TMPL);
            return rse_template ? rse_template : tp.getSectionTemplate();
        }
        case GroveLib::Node::MARKED_SECTION_START_NODE:
        case GroveLib::Node::MARKED_SECTION_END_NODE:
            return tp.getSectionTemplate();

        default:
            DDBG << "No built-in template for type"
                << nsi_->node()->nodeType() << "nodename <"
                << nsi_->node()->nodeName() << ">" << std::endl;
            return tp.getDummyTemplate();
    }
}

TemplateSelector::~TemplateSelector()
{
    wlist_.destroyAll();
}

} // namespace Xslt
