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
#include "xslt/xslt_defs.h"
#include "xslt/Result.h"
#include "xslt/impl/ResultTreeFragment.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/SortableNodeSet.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/BuiltInTemplate.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instances/instances.h"
#include "xslt/impl/instances/TemplateInst.h"
#include "xslt/impl/instances/VariableInst.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/debug.h"
#include "xslt/TopParamValueHolder.h"

#include "xpath/Engine.h"
#include "xpath/NodeSet.h"
#include "xpath/Expr.h"

#include "grove/Nodes.h"
#include "grove/udata.h"

namespace Xslt {

USING_COMMON_NS

ApplyImportsInstance::ApplyImportsInstance(const ApplyImports* applyImports,
                                           const InstanceInit& init,
                                           Instance* p)
    : ApplicatingInstance(applyImports, applyImports, init, p, true)
{
    buildSubInstances(init.resultContext_);
    apply(init.resultContext_);
}

void ApplyImportsInstance::dump() const
{
    DDBG << "ApplyImportsInstance: " << this << std::endl;
    ApplicatingInstance::dump();
}

const String& ApplyImportsInstance::mode() const
{
    const Instance* ti = parent()->currentTemplate();
    if (0 == ti || ti->instruction()->type() != Instruction::TEMPLATE)
        return String::null();
    return static_cast<const Template*>(ti->instruction())->mode();
}

Instance* ApplyImports::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ApplyImportsInstance(this, init, p);
}

////////////////////////////////////////////////////

ApplyTemplatesInstance::
  ApplyTemplatesInstance(const ApplyTemplates* applyTemplates,
                         const InstanceInit& init, Instance* p)
    : ApplicatingInstance(applyTemplates, applyTemplates, init, p, false)
{
    buildSubInstances(init.resultContext_);
    apply(init.resultContext_);
}

const String& ApplyTemplatesInstance::mode() const
{
    return static_cast<const ApplyTemplates*>(instruction())->mode();
}

void ApplyTemplatesInstance::dump() const
{
    DDBG << "ApplyTemplatesInstance: " << this << std::endl;
    ApplicatingInstance::dump();
}

Instance* ApplyTemplates::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ApplyTemplatesInstance(this, init, p);
}

/////////////////////////////////////////////////

AttrValueTemplateInstance::
  AttrValueTemplateInstance(AttributeValueTemplate* attr,
                            GroveLib::Element* elem,
                            const Xpath::NodeSetItem& nsi,
                            const Xpath::ExprContext& exprctx)
    : attrValue_(this, attr->valueExpr()->makeInst(nsi, exprctx)),
      isChanged_(false)
{
    attrResult_ = new AttributeResult(attr->name(),
                                      attrValue_->value()->getString(),
                                      attr->origin(), 0);
    elem->attrs().setAttribute(attrResult_.pointer());
}

void AttrValueTemplateInstance::notifyChanged
                                (const COMMON_NS::SubscriberPtrBase* ptr)
{
    if (&attrValue_ == ptr) {
        isChanged_ = true;
        SubscriberPtrPublisher::notifyChanged();
    }
}

void AttrValueTemplateInstance::forceUpdate()
{
    notifyChanged(&attrValue_);
}

void AttrValueTemplateInstance::update()
{
    if (isChanged_) {
        attrResult_->setValue(attrValue_->value()->getString());
        isChanged_ = false;
    }
}

////////////////////////////////////////////////////

Instance* DummyTemplate::makeInst(const InstanceInit& init, Instance* p) const
{
    return new DummyTemplateInstance(this, init, p);
}

DummyTemplateInstance::DummyTemplateInstance(const Instruction* instruction,
                                             const InstanceInit& init,
                                             Instance* p)
    : Instance(instruction, init, p)
{
}

void DummyTemplateInstance::dump() const
{
    DDBG << "DummyTemplateInstance: " << this << std::endl;
}

/////////////////////////////////////////////////////

Instance* ElementTemplate::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ElementTemplateInstance(this, init, p);
}

ElementTemplateInstance::
  ElementTemplateInstance(const ElementTemplate* elementTemplate,
                          const InstanceInit& init, Instance* p)
    : ApplicatingInstance(elementTemplate, elementTemplate, init, p, false),
      template_(elementTemplate)
{
    apply(init.resultContext_);
}

void ElementTemplateInstance::dump() const
{
    DDBG << "ElementTemplateInstance: " << this << std::endl;
    ApplicatingInstance::dump();
}

/////////////////////////////////////////////////

TextAndAttrTemplateInstance::
  TextAndAttrTemplateInstance(const TextAndAttrTemplate* textAndAttrTemplate,
                              const InstanceInit& init, Instance* p)
    : ResultMakingInstance(textAndAttrTemplate, init, p),
      template_(textAndAttrTemplate),
      selectInst_(this,
        textAndAttrTemplate->selectExpr().
            makeInst(init.context_, exprContext()))
{
    result_.insertBefore(new TextResult(selectInst_->value()->getString(),
        init.context_.node(), this), init.resultContext_.node_);
}

TextAndAttrTemplateInstance::~TextAndAttrTemplateInstance()
{
}

void TextAndAttrTemplateInstance::notifyChanged
                                    (const COMMON_NS::SubscriberPtrBase* ptr)
{
    if (ptr == &selectInst_)
        setModified();
}

void TextAndAttrTemplateInstance::updateContext(int utype)
{
    updateChildContext(utype);
}

void TextAndAttrTemplateInstance::update()
{
    if (GroveLib::Node::TEXT_NODE == result_.node()->nodeType())
        static_cast<GroveLib::Text*>(result_.node())->
            setData(selectInst_->value()->getString());
}

void TextAndAttrTemplateInstance::dump() const
{
    DDBG << "TextAndAttrTemplateInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* TextAndAttrTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new TextAndAttrTemplateInstance(this, init, p);
}

/////////////////////////////////////////////////

TextTemplateInstance::
  TextTemplateInstance(const TextTemplate* textAndAttrTemplate,
                       const InstanceInit& init, Instance* p)
    : ResultMakingInstance(textAndAttrTemplate, init, p)
{
    GroveLib::Text* src = static_cast<GroveLib::Text*>(init.context_.node());
    result_.insertBefore
        (new TextResult(static_cast<GroveLib::Text*>(src)->data(), src, this),
            init.resultContext_.node_);
    src->registerNodeVisitor(this);
}

TextTemplateInstance::~TextTemplateInstance()
{
}

void TextTemplateInstance::textChanged(const GroveLib::Text*)
{
    setModified();
}

void TextTemplateInstance::update()
{
    if (GroveLib::Node::TEXT_NODE == result_.node()->nodeType()) {
        TextResult* res = static_cast<TextResult*>(result_.node());
        res->setData(static_cast<const GroveLib::Text*>(res->origin())->data());
    }
}

void TextTemplateInstance::dump() const
{
    DDBG << "TextTemplateInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* TextTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new TextTemplateInstance(this, init, p);
}

////////////////////////////////////////////////////

static String entity_tn(const GroveLib::EntityReferenceStart* ers)
{
    const GroveLib::EntityDecl* ed = ers->entityDecl();
    switch (ed->declType()) {
        case GroveLib::EntityDecl::internalGeneralEntity:
            return "internal-entity";
        case GroveLib::EntityDecl::externalGeneralEntity:
            return "external-entity";
        case GroveLib::EntityDecl::xinclude: 
            return static_cast<const GroveLib::XincludeDecl*>(ed)->isFallback()
                ? "xinclude-fallback" : "xinclude";
        default:
            return "unknown";
    }
}

SectionTemplateInstance::SectionTemplateInstance(const SectionTemplate* t,
                                                 const InstanceInit& init,
                                                 Instance* p)
    : ResultMakingInstance(t, init, p)
{
    GroveLib::Node* cn = nsiContext().node();
    ElementResult* elem_res = 
        new ElementResult("se:section", cn, this);
    elem_res->setUdata(GroveLib::XSLT_GEN_BIT);
    const bool isStart = !(cn->nodeType() & GroveLib::Node::SECT_END_BIT);
    String sect_type;
    switch (cn->nodeType()) {
        case GroveLib::Node::ENTITY_REF_START_NODE: 
            sect_type = entity_tn(CONST_ERS_CAST(cn));
            break;
        case GroveLib::Node::ENTITY_REF_END_NODE: {
            const GroveLib::EntityReferenceEnd* ere = CONST_ERE_CAST(cn);
            if (ere->getSectStart())
                sect_type = entity_tn(CONST_ERS_CAST(ere->getSectStart()));
            break;
        }
        case GroveLib::Node::REDLINE_START_NODE: 
        case GroveLib::Node::REDLINE_END_NODE: 
            sect_type = "redline";
            break;

        case GroveLib::Node::MARKED_SECTION_START_NODE:
        case GroveLib::Node::MARKED_SECTION_END_NODE:
            sect_type = "marked-section";
            break;

        default:
            sect_type = "unknown";
            break;
    }
    elem_res->attrs().appendChild(new AttributeResult("type", 
        sect_type, 0, this));
    elem_res->attrs().appendChild(new AttributeResult("kind", 
        isStart ? "start" : "end", 0, this));
    result_.insertBefore(elem_res, init.resultContext_.node_);
}

void SectionTemplateInstance::dump() const
{
    DDBG << "SectionTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* SectionTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new SectionTemplateInstance(this, init, p);
}

SectionTemplateInstance::~SectionTemplateInstance()
{
}

////////////////////////////////////////////////////

ChoiceTemplateInstance::ChoiceTemplateInstance(const ChoiceTemplate* t,
                                               const InstanceInit& init,
                                               Instance* p)
    : ResultMakingInstance(t, init, p)
{
    ElementResult* elem_res =
        new ElementResult("se:choice", nsiContext().node(), this);
    elem_res->setUdata(GroveLib::XSLT_GEN_BIT);
    result_.insertBefore(elem_res, init.resultContext_.node_);
}

void ChoiceTemplateInstance::dump() const
{
    DDBG << "ChoiceTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* ChoiceTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new ChoiceTemplateInstance(this, init, p);
}

ChoiceTemplateInstance::~ChoiceTemplateInstance()
{
}

////////////////////////////////////////////////////

TextChoiceTemplateInstance::TextChoiceTemplateInstance(
                                               const TextChoiceTemplate* t,
                                               const InstanceInit& init,
                                               Instance* p)
    : ResultMakingInstance(t, init, p),
      valueInst_(this, t->valueExpr().makeInst(init.context_, exprContext()))
{
    const char* rn = (nsiContext().node()->nodeName() == "#choice-simple")
        ? "se:line-edit" : "se:combo-box";
    ElementResult* elem_res = new ElementResult(rn, nsiContext().node(), this);
    elem_res->attrs().appendChild(new AttributeResult("is-editable",
        "true", 0, this));
    elem_res->attrs().appendChild(new AttributeResult("value", 
        valueInst_->value()->getString(), 0, this)); 
    elem_res->setUdata(GroveLib::XSLT_GEN_BIT);
    result_.insertBefore(elem_res, init.resultContext_.node_);
}

void 
TextChoiceTemplateInstance::notifyChanged(const Common::SubscriberPtrBase*)
{
    setModified();
}

void TextChoiceTemplateInstance::update()
{
    static_cast<ElementResult*>(result_.node())->attrs().
        getAttribute("value")->setValue(valueInst_->value()->getString());
}

void TextChoiceTemplateInstance::dump() const
{
    DDBG << "TextChoiceTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* TextChoiceTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new TextChoiceTemplateInstance(this, init, p);
}

TextChoiceTemplateInstance::~TextChoiceTemplateInstance()
{
}

////////////////////////////////////////////////////

FoldTemplateInstance::FoldTemplateInstance(const FoldTemplate* t,
                                           const InstanceInit& init,
                                               Instance* p)
    : ResultMakingInstance(t, init, p)
{
    ElementResult* result = 
        new ElementResult("se:fold", nsiContext().node(), this);
    result->setUdata(GroveLib::XSLT_GEN_BIT);
    result_.insertBefore(result, init.resultContext_.node_);
    result->attrs().appendChild(new AttributeResult("se:fold", "true",
        0, this));
}

void FoldTemplateInstance::dump() const
{
    DDBG << "FoldTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* FoldTemplate::makeInst(const InstanceInit& init, Instance* p) const
{
    return new FoldTemplateInstance(this, init, p);
}

FoldTemplateInstance::~FoldTemplateInstance()
{
}

//////////////////////////////////////////////////

GenCopyTemplateInstance::GenCopyTemplateInstance(const GenCopyTemplate* copy,
                                                 const InstanceInit& init,
                                                 Instance* p)
    : ResultMakingInstance(copy, init, p)
{
    
    GroveLib::Node* node = init.context_.node();
    if (node->nodeType() != GroveLib::Node::ELEMENT_NODE)
        return;
    const GroveLib::Element* elem = CONST_ELEMENT_CAST(node);
    ElementResult* res = new ElementResult(elem->nodeName(), node, this);
    res->setUdata(node->udata());   // propagate "generated" bit
    const GroveLib::Attr* a = elem->attrs().firstChild();
    for (; a; a = a->nextSibling())
        res->attrs().appendChild
            (static_cast<GroveLib::Attr*>(a->cloneNode(true)));
    result_.insertBefore(res, init.resultContext_.node_);
}

void GenCopyTemplateInstance::dump() const
{
    DDBG << "GenCopyTemplateInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* GenCopyTemplate::makeInst(const InstanceInit& init, Instance* p) const
{
    return new GenCopyTemplateInstance(this, init, p);
}

////////////////////////////////////////////////////

CommentTemplateInstance::CommentTemplateInstance(const CommentTemplate* t,
                                               const InstanceInit& init,
                                               Instance* p)
    : ResultMakingInstance(t, init, p)
{
    result_.insertBefore(new ElementResult("se:comment",
        nsiContext().node(), this), init.resultContext_.node_);
}

void CommentTemplateInstance::dump() const
{
    DDBG << "CommentTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* CommentTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new CommentTemplateInstance(this, init, p);
}

CommentTemplateInstance::~CommentTemplateInstance()
{
}

////////////////////////////////////////////////////

PiTemplateInstance::PiTemplateInstance(const PiTemplate* t,
                                               const InstanceInit& init,
                                               Instance* p)
    : ResultMakingInstance(t, init, p)
{
    result_.insertBefore(new ElementResult("se:pi",
        nsiContext().node(), this), init.resultContext_.node_);
}

void PiTemplateInstance::dump() const
{
    DDBG << "PiTemplateInstance: " << this << std::endl;
    ResultMakingInstance::dump();
}

Instance* PiTemplate::makeInst(const InstanceInit& init,
                                        Instance* p) const
{
    return new PiTemplateInstance(this, init, p);
}

PiTemplateInstance::~PiTemplateInstance()
{
}

/////////////////////////////////////////////////////////////////

CallTemplateInstance::CallTemplateInstance(const CallTemplate* instr,
                                           const InstanceInit& init,
                                           Instance* p)
    : Instance(instr, init, p)
{

    buildSubInstances(init.resultContext_);
    templateInst_ = instr->callTemplate()->makeInst(init, this);
}

Instance* CallTemplate::makeInst(const InstanceInit& init, Instance* p) const
{
    return new CallTemplateInstance(this, init, p);
}

////////////////////////////////////////////////////

ChosenInstance::ChosenInstance(const Instruction* instruction,
                               const InstanceInit& init, Instance* p)
    : Instance(instruction, init, p)
{
    buildSubInstances(init.resultContext_);
}

void ChosenInstance::dump() const
{
    Instance::dump();
}

ChooseInstance::ChooseInstance(const Choose* choose,
                               const InstanceInit& init, Instance* p)
    : Instance(choose, init, p),
      chosen_(0)
{
    whenVector_.resize(choose->whenList().size());
    int pos = 0;
    for (Choose::WhenIterator i = choose->whenList().begin();
         i != choose->whenList().end(); i++, pos++) {
        whenVector_[pos].whenInstr_ = *i;
        whenVector_[pos].valuePtr_.setWatcher(this);
        whenVector_[pos].valuePtr_ =
            (*i)->testExpr()->makeInst(init.context_, exprContext());
    }
    choose_(&init.resultContext_);
}

void ChooseInstance::choose_(const ResultContext* rc)
{
    const Instruction* new_chosen = 0;
    for (uint i = 0; i < whenVector_.size(); i++)
        if (whenVector_[i].valuePtr_->value()->getBool()) {
            new_chosen = whenVector_[i].whenInstr_;
            break;
        }

    if (0 == new_chosen)
        new_chosen = static_cast<const Choose*>(instruction())->otherwise();

    if (chosen_ != new_chosen) {
        disposeResult();
        removeSubInstances();
        chosen_ = new_chosen;
        if (chosen_) {
            try {
                chosen_->makeInst(InstanceInit(nsiContext(),
                    rc ? *rc : resultContext()), this);
            }
            catch (Xslt::Exception& e) {
                throw;
            }
        }
    }
}

void ChooseInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void ChooseInstance::update()
{
    choose_(0);
}

void ChooseInstance::updateContext(int utype)
{
    for (uint i = 0; i < whenVector_.size(); i++)
        updateExpr(whenVector_[i].valuePtr_);
    updateChildContext(utype);
}

Instance* Choose::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ChooseInstance(this, init, p);
}

void ChooseInstance::dump() const
{
    DDBG << "Choose: " << this << std::endl;
    Instance::dump();
}

Instance* When::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ChosenInstance(this, init, p);
}

Instance* Otherwise::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ChosenInstance(this, init, p);
}

//////////////////////////////////////////////////

CopyInstance::CopyInstance(const Copy* copy,
                           const InstanceInit& init,
                           Instance* p)
    : ResultMakingInstance(copy, init, p),
      changeType_(NO_CHANGE)
{
    GroveLib::Node* node = init.context_.node();
    switch (node->nodeType()) {
        case GroveLib::Node::TEXT_NODE: {
            const GroveLib::Text* text = static_cast<GroveLib::Text*>(node);
            result_.insertBefore(new TextResult
                (text->data(), text, this),init.resultContext_.node_);
            node->registerNodeVisitor(this);
            break;
        }
        case GroveLib::Node::ELEMENT_NODE: {
            if (!node->parent())
                break;
            result_.insertBefore(new ElementResult
                (node->nodeName(), node, this),
                 init.resultContext_.node_);
            node->registerNodeVisitor(this);
            break;
        }
        case GroveLib::Node::ATTRIBUTE_NODE: {
            const GroveLib::Attr* attr = static_cast<GroveLib::Attr*>(node);
            result_.insertBefore(new AttributeResult
                (attr->nodeName(), attr->value(), attr, this), 0);
            attr->element()->registerNodeVisitor(this);
            break;
        }
        default:
            break;
    };
    if (GroveLib::Node::ATTRIBUTE_NODE != node->nodeType()) {
        if (result_.node()) {
            ResultContext result_context(result_.node(), 0);
            buildSubInstances(result_context);
        }
        else
            buildSubInstances(init.resultContext_);
    }
}

void CopyInstance::update()
{
    GroveLib::Node* node = nsiContext().node();
    switch (changeType_) {
        case TEXT_CHANGED:
            {
                static_cast<GroveLib::Text*>(result_.node())->setData(
                    static_cast<GroveLib::Text*>(node)->data());
                break;
            }
        case ATTR_CHANGED:
            {
                static_cast<GroveLib::Attr*>(result_.node())->setValue(
                    static_cast<GroveLib::Attr*>(node)->value());
                break;
            }
        default:
            break;
    }
    changeType_ = NO_CHANGE;
}

void CopyInstance::textChanged(const GroveLib::Text* text)
{
    if (GroveLib::Node::TEXT_NODE == result_.node()->nodeType() &&
        text == static_cast<TextResult*>(result_.node())->origin()) {
        changeType_ = TEXT_CHANGED;
        setModified();
    }
}

void CopyInstance::attributeChanged(const GroveLib::Attr* new_attr)
{
    if (GroveLib::Node::ATTRIBUTE_NODE == result_.node()->nodeType() &&
        new_attr == static_cast<AttributeResult*>(result_.node())->origin()) {
        changeType_ = ATTR_CHANGED;
        setModified();
    }
}

void CopyInstance::dump() const
{
    DDBG << "CopyInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* Copy::makeInst(const InstanceInit& init, Instance* p) const
{
    return new CopyInstance(this, init, p);
}

//////////////////////////////////////////////////

ElementInstance::ElementInstance(const Element* element,
                                 const InstanceInit& init, Instance* p)
    : ResultMakingInstance(element, init, p),
      element_(element),
      nameInst_(this,
        element->name()->valueExpr()->makeInst(init.context_, exprContext())),
      namespaceInst_(this, 0)
{
    if (element->ns())
        namespaceInst_ =
            element->ns()->valueExpr()->makeInst(init.context_, exprContext());
    build(init.resultContext_);
}

void ElementInstance::build(const ResultContext& before)
{
    if (isValidName()) {
        ElementResult* elem = new ElementResult(qName(), nsi_->node(), this);
        result_.insertBefore(elem, before.node_);
        ResultContext result_context(result_.node(), 0);
        buildSubInstances(result_context);
    }
    else 
        buildSubInstances(before);
}

String ElementInstance::qName() const
{
    String name(nameInst_->value()->getString());
    if (!namespaceInst_.isNull())
        //! TODO: use getXmlNsByUri
        name = namespaceInst_->value()->getString() + ':' + name;
    return name;
}

bool ElementInstance::isValidName() const
{
    return (!nameInst_->value()->getString().isEmpty());
}

void ElementInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void ElementInstance::updateContext(int utype)
{
    updateExpr(nameInst_);
    if (!namespaceInst_.isNull())
        updateExpr(namespaceInst_);
    updateChildContext(utype);
}

void ElementInstance::update()
{
    if (isValidName() != (0 != result_.node())) {
        disposeResult();
        removeSubInstances();
        build(resultContext());
    }
    else
        if (result_.node())
            static_cast<ElementResult*>(result_.node())->setName(qName());
}

const InstanceResult* ElementInstance::firstResult() const
{
    if (result_.node())
        return &result_;
    return Instance::firstResult();
}

const InstanceResult* ElementInstance::lastResult() const
{
    if (result_.node())
        return &result_;
    return Instance::lastResult();
}

void ElementInstance::dump() const
{
    DDBG << "ElementInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* Element::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ElementInstance(this, init, p);
}

ForEachInstance::ForEachInstance(const ForEach* forEach,
                                 const InstanceInit& init, Instance* p)
    : Instance(forEach, init, p),
      selectInst_(this,
#ifdef XSLT_SORTING
            new SortableNodeSet(forEach->sortSpecs(),
                forEach->selectExpr(), init.context_))
#else  // XSLT_SORTING
            forEach->selectExpr().makeInst(init.context_, exprContext()))
#endif // XSLT_SORTING
{
    instantiate(init.resultContext_);
}

void ForEachInstance::instantiate(const ResultContext& before)
{
    const Xpath::NodeSet& node_set = selectInst_->value()->getNodeSet();
    const Xpath::NodeSetItem* n = node_set.first();
    while (n) {
        buildSubInstances(before, n);
        n = n->next();
    }
}

void ForEachInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void ForEachInstance::updateContext(int utype)
{
    updateExpr(selectInst_);
    updateChildContext(utype);
}

void ForEachInstance::update()
{
    // TODO: Improve performance by not removing old instances
    disposeResult();
    removeSubInstances();
    instantiate(resultContext());
}

Instance* ForEach::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ForEachInstance(this, init, p);
}

///////////////////////////////////////////////////

IfInstance::IfInstance(const If* ifInstruction,
                       const InstanceInit& init, Instance* p)
    : Instance(ifInstruction, init, p),
      testInst_(this,
        ifInstruction->testExpr().makeInst(init.context_, exprContext()))
{
    prevValue_ = testInst_->value()->getBool();
    if (prevValue_)
        buildSubInstances(init.resultContext_);
}

void IfInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void IfInstance::updateContext(int utype)
{
    updateExpr(testInst_);
    updateChildContext(utype);
}

void IfInstance::update()
{
    bool newValue = testInst_->value()->getBool();
    if (newValue == prevValue_)
        return;
    prevValue_ = newValue;
    disposeResult();
    removeSubInstances();
    if (newValue)
        buildSubInstances(resultContext());
}

Instance* If::makeInst(const InstanceInit& init, Instance* p) const
{
    return new IfInstance(this, init, p);
}

LiteralResultInstance::LiteralResultInstance(const LiteralResult* literalResult,
                                             const InstanceInit& init,
                                             Instance* pinst)
    : ResultMakingInstance(literalResult, init, pinst)
{
    ElementResult* elem =
        new ElementResult(literalResult->elementName(),
            init.context_.node(), this);

    LiteralResult::copyPrefixesTo(literalResult->element(), elem);

    GroveLib::Node* p = result_.parent();
    if (!p || p->nodeType() != GroveLib::Node::ELEMENT_NODE)  // document root?
        LiteralResult::copyPrefixesTo(styleContext().element(), elem);
    const LiteralResult::AttrValueList& attrs = literalResult->attributes();
    for (LiteralResult::AttrValueIterator i = attrs.begin();
         i != attrs.end(); i++)
        attributes_.push_back(AttrValuePtr(this,
            new AttrValueTemplateInstance(*i, elem,
                nsiContext(), exprContext())));
    p = elem->node();
    p->setParent(result_.parent());
    p->setGSR(result_.parent()->getGSR());
    ResultContext result_context(p, 0);
    buildSubInstances(result_context);
    result_.insertBefore(elem, init.resultContext_.node_);
}

LiteralResultInstance::~LiteralResultInstance()
{
    attributes_.clear();
}

void LiteralResultInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void LiteralResultInstance::updateContext(int utype)
{
    DBG(XSLT.UPDATE) << "LiteralResultInstance: updateCtx " << utype
        << std::endl;
    AttrList::iterator i = attributes_.begin();
    for (; i != attributes_.end(); i++) {
        if(updateExpr(i->pointer()->attrValue()))
            i->pointer()->forceUpdate();
    }
    updateChildContext(utype);
}

void LiteralResultInstance::update()
{
    AttrList::iterator i = attributes_.begin();
    for (; i != attributes_.end(); i++)
        (*i)->update();
}

void LiteralResultInstance::dump() const
{
    DDBG << "LiteralResultInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* LiteralResult::makeInst(const InstanceInit& init, Instance* p) const
{
    return new LiteralResultInstance(this, init, p);
}

///////////////////////////////////////////////////

LiteralTextInstance::
  LiteralTextInstance(const LiteralResultText* literalResultText,
                      const InstanceInit& init, Instance* p)
    : ResultMakingInstance(literalResultText, init, p)
{
    TextResult* text = new TextResult(literalResultText->text(), 0, this);
    result_.insertBefore(text, init.resultContext_.node_);
}

void LiteralTextInstance::dump() const
{
    DDBG << "LiteralTextInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance*
LiteralResultText::makeInst(const InstanceInit& init, Instance* p) const
{
    return new LiteralTextInstance(this, init, p);
}

//////////////////////////////////////////////////////

ParamInstance::ParamInstance(const Param* param,
                             const InstanceInit& init, Instance* p)
    : VariableInstance(param, init, p)
{
}

Instance* Param::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ParamInstance(this, init, p);
}

void ParamInstance::setTopValueHolder()
{
    RefCntPtr<Xpath::ValueHolder> vholder = variableInst_.pointer();
    variableInst_ = new TopParamValueHolder(vholder.pointer());
}

void ParamInstance::dump() const
{
    DDBG << "ParamInstance: " << this << '(' << name() << ')' << std::endl;
    Instance::dump();
}

Instance* TopParam::makeInst(const InstanceInit& init, Instance* p) const
{
    ParamInstance* pinst = new ParamInstance(this, init, p);
    if (!varExpr().isNull())
        pinst->setTopValueHolder();
    return pinst;
}

/////////////////////////////////////////////////

ResultMakingInstance::ResultMakingInstance(const Instruction* instruction,
                                           const InstanceInit& init,
                                           Instance* p)
    : Instance(instruction, init, p),
      result_(init.resultContext_.parent_)
{
}

const InstanceResult* ResultMakingInstance::firstResult() const
{
    if (result_.node())
        return &result_;
    return 0;
}

const InstanceResult* ResultMakingInstance::lastResult() const
{
    if (result_.node())
        return &result_;
    return 0;
}

const InstanceResult* ResultMakingInstance::selfResult() const
{
    if (result_.node())
        return &result_;
    return 0;
}

void ResultMakingInstance::disposeResult()
{
    result_.dispose();
    Instance::disposeResult();
}

//////////////////////////////////////////////////////

TemplateInstance::TemplateInstance(const Template* templateInstr,
                                   const InstanceInit& init, Instance* p)
    : Instance(templateInstr, init, p),
      template_(templateInstr)
{
    //if (upperTemplate_) {
    //    String loop = "    " + signature() + '\n';
    //    if (upperTemplate_->doesLoop(templateInstr, init.context_, loop)) {
    //        loop.insert(0, "Template loop detected:\n");
    //        throw InstanceError(loop, this);
    //    }
    //}
    //upperTemplate_ = p->currentTemplate();
    buildSubInstances(init.resultContext_);
}

void TemplateInstance::updateContext(int utype)
{
    // TS doesn't really needs an update.
    // was: templateSelector()->updateContext(utype);
    updateChildContext(utype);
}

TemplateInstance::~TemplateInstance()
{
}

#if 0
bool TemplateInstance::doesLoop(const Instruction* instruction,
                                const Xpath::NodeSetItem& context,
                                String& loop) const
{
    String str = "    " + signature() + '\n';
    loop.insert(0, str);
    if (Instance::doesLoop(instruction, context, loop)) {
        return true;
    }
    else
        if (upperTemplate_)
            return upperTemplate_->doesLoop(instruction, context, loop);
    return false;
}
#endif

String TemplateInstance::signature() const
{
    String dump_str("template:");
    if (!template_->match().isNull())
        dump_str += " match=" + template_->match();
    if (!template_->name().isNull())
        dump_str += " name=" + template_->name();
    if (!template_->isDefaultPriority()) {
        String num;
        num.setNum(template_->priority());
        dump_str += " priority=" + num;
    }
    if (!template_->mode().isNull())
        dump_str += " mode=" + template_->mode();
    return dump_str;
}

void TemplateInstance::dump() const
{
    DDBG << "TemplateInstance: " << this << std::endl;
    Instance::dump();
}

Instance* Template::makeInst(const InstanceInit& init, Instance* p) const
{
    return new TemplateInstance(this, init, p);
}

////////////////////////////////////////////////////////

TextInstance::TextInstance(const Text* text,
                           const InstanceInit& init,
                           Instance* p)
    : ResultMakingInstance(text, init, p)
{
    result_.insertBefore
        (new TextResult(text->text(), 0, this), init.resultContext_.node_);
}

Instance* Text::makeInst(const InstanceInit& init, Instance* p) const
{
    return new TextInstance(this, init, p);
}

////////////////////////////////////////////////////////

ValueOfInstance::ValueOfInstance(const ValueOf* valueOf,
                                 const InstanceInit& init, Instance* p)
    : ResultMakingInstance(valueOf, init, p),
      selectInst_(this,
        valueOf->selectExpr()->makeInst(init.context_, exprContext()))
{
    result_.insertBefore(new TextResult(selectInst_->value()->getString(),
        0, this), init.resultContext_.node_);
}

void ValueOfInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr)
{
    if (ptr == &selectInst_)
        setModified();
}

void ValueOfInstance::updateContext(int utype)
{
    updateExpr(selectInst_);
    updateChildContext(utype);
}

void ValueOfInstance::update()
{
    GroveLib::Node* const rn  = result_.node();
    if (GroveLib::Node::TEXT_NODE == rn->nodeType())
        static_cast<GroveLib::Text*>(rn)->
            setData(selectInst_->value()->getString());
}

void ValueOfInstance::dump() const
{
    DDBG << "ValueOfInstance: " << this << std::endl;
    result_.node()->dump();
    ResultMakingInstance::dump();
}

Instance* ValueOf::makeInst(const InstanceInit& init, Instance* p) const
{
    return new ValueOfInstance(this, init, p);
}

////////////////////////////////////////////////////

MessageInstance::MessageInstance(const Message* instr,
                                 const InstanceInit& init, Instance* p)
    : Instance(instr, init, p)
{
    skipResultContext_ = true;
    ResultTreeFragment* rtf = new ResultTreeFragment;
    rtf_ = rtf;
    messageInstNsi_.setNode(rtf->rtfRoot());
    buildSubInstances(ResultContext(rtf->rtfRoot(), 0));
    messageInst_ =
        instr->messageExpr()->makeInst(messageInstNsi_, exprContext());
    emitMessage();
    messageInst_.setWatcher(this);
    if (instr->terminate())
        throw Xslt::Exception(XsltMessages::instanceMsg, contextString());
}

void MessageInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    setModified();
}

void MessageInstance::emitMessage()
{
    COMMON_NS::MessageStream* m = topStylesheetInst()->mstream();
    if (!m || messageInst_->value().isNull())
        return;
    COMMON_NS::String msg = messageInst_->value()->getString();
    if (msg == message_)
        return;
    *m << XsltMessages::message << COMMON_NS::Message::L_INFO << msg;
    message_ = msg;
}

void MessageInstance::updateContext(int utype)
{
    updateChildContext(utype);
}

void MessageInstance::update()
{
    emitMessage();
}

GroveLib::Node* MessageInstance::rtfRoot() const
{
    return static_cast<const ResultTreeFragment*>(rtf_.pointer())->rtfRoot();
}

void MessageInstance::dump() const
{
    DDBG << "MessageInstance: " << this << std::endl;
    Instance::dump();
}

Instance* Message::makeInst(const InstanceInit& init, Instance* p) const
{
    return new MessageInstance(this, init, p);
}

////////////////////////////////////////////////////

WithParamInstance::WithParamInstance(const WithParam* withParam,
                                     const InstanceInit& init, Instance* p)
    : VariableInstance(withParam, init, p)
{
}

Instance* WithParam::makeInst(const InstanceInit& init, Instance* p) const
{
    return new WithParamInstance(this, init, p);
}

void WithParamInstance::dump() const
{
    DDBG << "WithParamInstance: " << this << '(' << name() << ')' << std::endl;
    Instance::dump();
}

//////////////////////////////////////////////////////

VariableInstance::VariableInstance(const Variable* var,
                                   const InstanceInit& init, Instance* p)
    : Instance(var, init, p)
{
    if (!var->varExpr().isNull()) // Note that given context is used.
        variableInst_ = var->varExpr()->makeInst(nsiContext(), exprContext());
    else {
        skipResultContext_ = true;
        variableInst_ = new ResultTreeFragment;
        buildSubInstances(ResultContext(rtf_root(), 0));
    }
}

GroveLib::Node* VariableInstance::rtfRoot() const
{
    return rtf_root();
}

GroveLib::Node* VariableInstance::rtf_root() const
{
    if (skipResultContext_)
        return static_cast<const ResultTreeFragment*>
            (variableInst_.pointer())->rtfRoot();
    const Xpath::NodeSetItem* n =
        variableInst_->value()->getNodeSet().first();
    return n ? n->node(): 0;
}

void VariableInstance::updateContext(int utype)
{
    if (!skipResultContext_ &&
        !static_cast<const Variable*>(instruction())->varExpr().isNull())
            updateExpr(variableInst_);
    updateChildContext(utype);
}

const String& VariableInstance::name() const
{
    return static_cast<const Variable*>(instruction())->name();
}

Xpath::ValueHolder* VariableInstance::getVariable() const
{
    DBG(XSLT.VARS) << "GetVariable(self): " << name() << std::endl;
    return variableInst_.pointer();
}

Xpath::ValueHolder* VariableInstance::getVariable(const String& name) const
{
    DBG(XSLT.VARS) << "VarInst::GetVariable(by name): " << name << std::endl;
    if (prevSibling())
        return prevSibling()->Instance::getVariable(name);
    return parent()->getVariable(name);
}

Instance* Variable::makeInst(const InstanceInit& init, Instance* p) const
{
    return new VariableInstance(this, init, p);
}

void VariableInstance::dump() const
{
    DDBG << "VariableInstance: " << this << '(' << name() << ')' << std::endl;
    Instance::dump();
}

///////////////////////////////////////////////////////////////

Instance* 
ProcessingInstruction::makeInst(const InstanceInit& init, Instance* p) const
{
    return new DummyInstance(this, init, p);
}

Instance* Comment::makeInst(const InstanceInit& init, Instance* p) const
{
    return new DummyInstance(this, init, p);
}

} // namespace Xslt
