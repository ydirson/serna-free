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
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/Grove.h"
#include "sapi/common/impl/xtn_wrap_impl.h"
#include "sapi/grove/GroveIdManager.h"
#include "sapi/grove/CatalogManager.h"
#include "sapi/grove/GroveEntity.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/ChoiceNode.h"
#include "grove/Grove.h"
#include "grove/GroveBuilder.h"
#include "grove/StripInfo.h"
#include "grove/IdManager.h"
#include "grove/SectionSyncher.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/udata.h"
#include "spgrovebuilder/SpGroveBuilder.h"
#include "catmgr/CatalogManager.h"
#include "common/safecast.h"
#include "common/PathName.h"

#include <iostream>
#include <typeinfo>

namespace SernaApi {

#define MY_IMPL   SAFE_CAST(GroveLib::Node*, getRep())
#define NODE_IMPL(node) SAFE_CAST(GroveLib::Node*, node.getRep())
#define NNS_IMPL   SAFE_CAST(GroveLib::NodeWithNamespace*, getRep())
#define ELEM_IMPL  SAFE_CAST(GroveLib::Element*, getRep())
#define ATTR_IMPL  SAFE_CAST(GroveLib::Attr*, getRep())
#define TEXT_IMPL  SAFE_CAST(GroveLib::Text*, getRep())
#define PI_IMPL    SAFE_CAST(GroveLib::ProcessingInstruction*, getRep())

GroveNode::GroveNode(SernaApiBase* rep)
    : RefCountedWrappedObject(rep)
{
}

XTREENODE_WRAP_IMPL(GroveNode, GroveLib::Node)

GroveNode::NodeType GroveNode::nodeType() const
{
    if (getRep())
        return (NodeType) MY_IMPL->nodeType();
    return UNDEFINED_NODE;
}

SString GroveNode::nodeName() const
{
    if (getRep())
        return MY_IMPL->nodeName();
    return SString();
}

void GroveNode::dump() const
{
    if (getRep())
        GroveLib::Node::dumpSubtree(MY_IMPL);
}

GroveNode::~GroveNode()
{
}

GroveNode GroveNode::cloneNode(const bool deep,
                               const GroveNode& futureParent) const
{
    if (getRep())
        return MY_IMPL->cloneNode(deep, NODE_IMPL(futureParent));
    return 0;
}

GroveDocumentFragment GroveNode::takeAsFragment(const GroveNode& endNode)
{
    if (getRep())
        return MY_IMPL->takeAsFragment(NODE_IMPL(endNode));
    return 0;
}

GroveDocumentFragment GroveNode::copyAsFragment(const GroveNode& endNode) const
{
    if (getRep())
        return MY_IMPL->copyAsFragment(NODE_IMPL(endNode));
    return 0;
}

Grove GroveNode::grove() const
{
    if (getRep())
        return MY_IMPL->grove();
    return 0;
}

GroveSectionRoot GroveNode::getGSR() const
{
    if (getRep())
        return MY_IMPL->getGSR();
    return 0;
}

void GroveNode::setGSR(const GroveSectionRoot& gsr)
{
    MY_IMPL->setGSR(static_cast<GroveLib::GroveSectionRoot*>(gsr.getRep()));
}

GroveDocument GroveNode::document() const
{
    if (getRep())
        return MY_IMPL->document();
    return 0;
}

void GroveNode::deregisterAllNodeWatchers()
{
    MY_IMPL->deregisterAllNodeVisitors();
}

GroveSectionRoot GroveNode::asGroveSectionRoot() const
{
    return dynamic_cast<GroveLib::GroveSectionRoot*>(getRep());
    
}
   
GroveDocument GroveNode::asGroveDocument() const
{
    if (getRep() && MY_IMPL->nodeType() == GroveLib::Node::DOCUMENT_NODE)
        return getRep();
    return 0;
}

GroveDocumentFragment GroveNode::asGroveDocumentFragment() const
{
    if (getRep() && 
        MY_IMPL->nodeType() == GroveLib::Node::DOCUMENT_FRAGMENT_NODE)
            return getRep();
    return 0;
}

GroveNodeWithNamespace GroveNode::asGroveNodeWithNamespace() const
{
    return dynamic_cast<GroveLib::NodeWithNamespace*>(getRep());
}

GroveElement GroveNode::asGroveElement() const
{
    if (getRep() && MY_IMPL->nodeType() == GroveLib::Node::ELEMENT_NODE)
        return getRep();
    return 0;
}

GroveAttr GroveNode::asGroveAttr() const
{
    if (getRep() && MY_IMPL->nodeType() == GroveLib::Node::ATTRIBUTE_NODE)
        return getRep();
    return 0;
}

GroveText GroveNode::asGroveText() const
{
    if (!getRep())
        return 0;
    switch (MY_IMPL->nodeType()) {
        case GroveLib::Node::TEXT_NODE:
            return getRep();
        default:
            return 0;
    }
}

GroveComment GroveNode::asGroveComment() const
{
    if (getRep() && MY_IMPL->nodeType() == GroveLib::Node::COMMENT_NODE)
        return getRep();
    return 0;
}

GrovePi GroveNode::asGrovePi() const
{
    if (getRep() && MY_IMPL->nodeType() == GroveLib::Node::PI_NODE)
        return getRep();
    return 0;
}


GroveErs GroveNode::asGroveErs() const
{
    return (getRep() && MY_IMPL->nodeType() == 
        GroveLib::Node::ENTITY_REF_START_NODE) ? getRep() : 0;
}

GroveEre GroveNode::asGroveEre() const
{
    return (getRep() && MY_IMPL->nodeType() == 
        GroveLib::Node::ENTITY_REF_END_NODE) ? getRep() : 0;
}

bool GroveNode::isReadOnly() const
{
    if (!getRep() || !MY_IMPL->nodeExt())
        return false;
    const GroveLib::LineLocExt* ext = MY_IMPL->nodeExt()->asConstLineLocExt();
    if (0 == ext)
        return false;
    return ext->isReadOnly();
}

static void set_read_only(GroveLib::Node* n, bool v, bool recursive)
{
    if (!n || !n->nodeExt())
        return;
    GroveLib::LineLocExt* ext = n->nodeExt()->asLineLocExt();
    if (ext)
        ext->setReadOnly(v);
    if (!recursive)
        return;
    n = n->firstChild();
    for (; n; n = n->nextSibling())
        set_read_only(n, v, recursive);
}

void GroveNode::setReadOnly(bool v, bool recursive)
{
    if (getRep())
        set_read_only(MY_IMPL, v, recursive);
}

GroveErs GroveNode::getErs() const
{
    if (!getRep())
        return 0;
    return get_ers(MY_IMPL);    
}

///////////////////////////////////////////////////////////////////////////////

#define SELF_GSR SAFE_CAST(GroveLib::GroveSectionRoot*, getRep())

GroveSectionRoot::GroveSectionRoot(GroveNode::NodeType t)
    : GroveNode(new GroveLib::GroveSectionRoot((GroveLib::Node::NodeType)t))
{
}

GroveSectionRoot::GroveSectionRoot(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveSectionRoot::~GroveSectionRoot()
{
}

void GroveSectionRoot::setGrove(const Grove& g)
{
    if (getRep())
        SELF_GSR->setGrove(static_cast<GroveLib::Grove*>(g.getRep()));
}

Grove GroveSectionRoot::grove() const
{
    return getRep() ? SELF_GSR->grove() : 0;
}

GroveErs GroveSectionRoot::ersRoot() const
{
    return getRep() ? SELF_GSR->ers() : 0;
}

GroveEntityReferenceTable GroveSectionRoot::ert() const
{
    return getRep() ? SELF_GSR->ert() : 0;
}

///////////////////////////////////////////////////////////////////////////////

GroveDocument::GroveDocument(SernaApiBase* rep)
    : GroveSectionRoot(rep)
{
}

GroveDocument::~GroveDocument()
{
}

GroveElement GroveDocument::documentElement() const
{
    if (getRep())
        return SAFE_CAST(GroveLib::Document*, getRep())->documentElement();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

GroveDocumentFragment::GroveDocumentFragment(SernaApiBase* rep)
    : GroveSectionRoot(rep ? rep : new GroveLib::DocumentFragment)
{
}
    
bool GroveDocumentFragment::saveAsXmlString(SString& saveTo,
                                            int flags,
                                            const GroveStripInfo& si) const
{
    if (!getRep())
        return false;
    Common::String save_to;
    bool ok = static_cast<const GroveLib::DocumentFragment*>(getRep())->
        saveAsXmlString(save_to, flags, 
            static_cast<const GroveLib::StripInfo*>(si.getRep()));
    saveTo = save_to;
    return ok;
}

GroveDocumentFragment::~GroveDocumentFragment()
{
}

///////////////////////////////////////////////////////////////////////////////

GroveNodeWithNamespace::GroveNodeWithNamespace(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveNodeWithNamespace::~GroveNodeWithNamespace()
{
}

SString GroveNodeWithNamespace::localName() const
{
    if (getRep())
        return NNS_IMPL->localName();
    return SString();
}

SString GroveNodeWithNamespace::xmlNsPrefix() const
{
    if (getRep())
        return NNS_IMPL->xmlNsPrefix();
    return SString();
}

SString GroveNodeWithNamespace::xmlNsUri() const
{
    if (getRep())
        return NNS_IMPL->xmlNsUri();
    return SString();
}

SString GroveNodeWithNamespace::xmlElNsUri() const
{
    if (getRep())
        return NNS_IMPL->xmlElNsUri();
    return SString();
}

SString GroveNodeWithNamespace::getXmlNsByPrefix(const SString& prefix) const
{
    if (getRep())
        return NNS_IMPL->getXmlNsByPrefix(prefix);
    return SString();
}

SString GroveNodeWithNamespace::getPrefixByXmlNs(const SString& uri) const
{
    if (getRep())
        return NNS_IMPL->getPrefixByXmlNs(uri);
    return SString();
}

void GroveNodeWithNamespace::addToPrefixMap(const SString& prefix,
                       const SString& nsUri)
{
    if (getRep())
        NNS_IMPL->addToPrefixMap(prefix, nsUri);
}
void GroveNodeWithNamespace::eraseFromPrefixMap(const SString& prefix)
{
    if (getRep())
        NNS_IMPL->eraseFromPrefixMap(prefix);
}

///////////////////////////////////////////////////////////////////////////////

#define ATTL_SELF ((::GroveLib::AttrList*) pvt_)

GroveElementAttrList::GroveElementAttrList(void* attlist)
{
    pvt_ = attlist;
}

GroveAttr GroveElementAttrList::getAttribute(const SString& name) const
{
    return pvt_ ? ATTL_SELF->getAttribute(name) : GroveAttr(0);
}

bool GroveElementAttrList::removeAttribute(const SString& name)
{
    return pvt_ ? ATTL_SELF->removeAttribute(name) : false;
}

void GroveElementAttrList::setAttribute(const GroveAttr& attr)
{
    if (pvt_) 
        ATTL_SELF->setAttribute(static_cast<GroveLib::Attr*>(attr.getRep()));
}

GroveAttr GroveElementAttrList::firstChild() const
{
    return pvt_ ? ATTL_SELF->firstChild() : GroveAttr(0);
}

GroveAttr GroveElementAttrList::lastChild() const
{
    return pvt_ ? ATTL_SELF->lastChild() : GroveAttr(0);
}

GroveAttr GroveElementAttrList::getChild(int n) const
{
    return pvt_ ? ATTL_SELF->getChild(n) : GroveAttr(0);
}

int GroveElementAttrList::countChildren() const
{
    return pvt_ ? ATTL_SELF->countChildren() : 0;
}

void GroveElementAttrList::appendChild(const GroveAttr& attr)
{
    if (pvt_)
        ATTL_SELF->appendChild(static_cast<GroveLib::Attr*>(attr.getRep()));
}

void GroveElementAttrList::removeAllChildren()
{
    if (pvt_)
        ATTL_SELF->removeAllChildren();
}

///////////////////////////////////////////////////////////////////////////////

GroveElement::GroveElement(SernaApiBase* rep)
    : GroveNodeWithNamespace(rep)
{
}

GroveElement::GroveElement(const SString& elementName)
    : GroveNodeWithNamespace(new GroveLib::Element(elementName))
{
}

GroveElement::~GroveElement()
{
}

void GroveElement::setName(const SString& name)
{
    ELEM_IMPL->setName(name);
}

GroveElementAttrList GroveElement::attrs() const
{
    if (getRep())
        return &(ELEM_IMPL->attrs());
    return 0;
}

void GroveElement::setCollapsed(bool v)
{
    if (getRep())
        GroveLib::set_node_fold_state(ELEM_IMPL, v);
}

///////////////////////////////////////////////////////////////////////////////

GroveAttr::GroveAttr(SernaApiBase* rep)
    : GroveNodeWithNamespace(rep)
{
}

GroveAttr::GroveAttr(const SString& attributeName,
                     const SString& value)
    : GroveNodeWithNamespace(new GroveLib::Attr(attributeName))
{
    if (!value.isNull())
        ATTR_IMPL->setValue(value);
}

GroveAttr::~GroveAttr()
{
}

GroveElement GroveAttr::element() const
{
    if (getRep())
        return ATTR_IMPL->element();
    return 0;
}

bool GroveAttr::specified() const
{
    if (getRep())
        return ATTR_IMPL->specified();
    return 0;
}

SString GroveAttr::value() const
{
    if (getRep())
        return ATTR_IMPL->value();
    return SString();
}

void GroveAttr::build()
{
    ATTR_IMPL->build();
}

void GroveAttr::setValue(const SString& val)
{
    ATTR_IMPL->setValue(val);
}

void GroveAttr::setName(const SString& name)
{
    ATTR_IMPL->setName(name);
}

bool GroveAttr::tokenized() const
{
    if (getRep())
        return ATTR_IMPL->tokenized();
    return 0;
}

GroveAttr::Defaulted GroveAttr::defaulted() const
{
    if (getRep())
        return (Defaulted) ATTR_IMPL->defaulted();
    return CURRENT;
}

GroveAttr::AttrType GroveAttr::type() const
{
    if (getRep())
        return (AttrType) ATTR_IMPL->type();
    return INVALID;
}

void GroveAttr::setDefaulted(const GroveAttr::Defaulted d)
{
    ATTR_IMPL->setDefaulted((GroveLib::Attr::Defaulted)d);
}

void GroveAttr::setType(const GroveAttr::AttrType t)
{
    ATTR_IMPL->setType((GroveLib::Attr::AttrType)t);
}

GroveAttr::IdClass GroveAttr::idClass() const
{
    if (getRep())
        return (IdClass) ATTR_IMPL->idClass();
    return NOT_ID;
}

void GroveAttr::setIdClass(GroveAttr::IdClass idc)
{
    ATTR_IMPL->setIdClass((GroveLib::Attr::IdClass)idc);
}

///////////////////////////////////////////////////////////////////////////////

GroveText::GroveText(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveText::GroveText(const SString& value)
    : GroveNode(new GroveLib::Text(value))
{
}

GroveText::~GroveText()
{
}

SString GroveText::data() const
{
    if (getRep())
        return TEXT_IMPL->data();
    return SString();
}

void GroveText::setData(const SString& s)
{
    TEXT_IMPL->setData(s);
}

bool GroveText::isEmpty() const
{
    if (getRep())
        return TEXT_IMPL->isEmpty();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

GroveComment::GroveComment(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveComment::GroveComment(const SString& comment)
    : GroveNode(new GroveLib::Comment(comment))
{
}

GroveComment::~GroveComment()
{
}

SString GroveComment::comment() const
{
    if (getRep())
        return SAFE_CAST(GroveLib::Comment*, getRep())->comment();
    return SString();
}

void GroveComment::setComment(const SString& s)
{
    SAFE_CAST(GroveLib::Comment*, getRep())->setComment(s);
}

///////////////////////////////////////////////////////////////////////////////

#define SELF_ERS static_cast<GroveLib::EntityReferenceStart*>(getRep())

GroveErs::GroveErs(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveEntityDecl GroveErs::entityDecl() const
{
    return getRep() ? SELF_ERS->entityDecl() : 0;
}

GroveEre GroveErs::ere() const
{
    return getRep() ? SELF_ERS->getSectEnd() : 0;
}

GroveErs GroveErs::parentErs() const
{
    return getRep() ? SELF_ERS->getSectParent() : 0;
}

GroveErs GroveErs::nextErs() const
{
    return getRep() ? SELF_ERS->getNextSect() : 0;
}

GroveErs GroveErs::childErs() const
{
    return getRep() ? SELF_ERS->getFirstSect() : 0;
}

#define SELF_ERE static_cast<GroveLib::EntityReferenceEnd*>(getRep())

GroveEre::GroveEre(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GroveErs GroveEre::ers() const
{
    return getRep() ? SELF_ERE->getSectStart() : 0;
}

///////////////////////////////////////////////////////////////////////////////

GrovePi::GrovePi(SernaApiBase* rep)
    : GroveNode(rep)
{
}

GrovePi::GrovePi(const SString& target, const SString& data)
    : GroveNode(new GroveLib::ProcessingInstruction)
{
    PI_IMPL->setTarget(target);
    PI_IMPL->setData(data);
}

SString GrovePi::target() const
{
    if (getRep())
        return PI_IMPL->target();
    return SString();
}

void GrovePi::setTarget(const SString& t)
{
    PI_IMPL->setTarget(t);
}

SString GrovePi::data() const
{
    if (getRep())
        return PI_IMPL->data();
    return SString();
}

void GrovePi::setData(const SString& v)
{
    PI_IMPL->setData(v);
}

///////////////////////////////////////////////////////////////////////////////

#define GROVE_IMPL   SAFE_CAST(GroveLib::Grove*, getRep())

Grove::Grove(SernaApiBase* rep)
    : RefCountedWrappedObject(rep)
{
}

Grove::Grove(const SString& rootElementName)
    : RefCountedWrappedObject()
{
    GroveLib::GrovePtr grove = new GroveLib::Grove();
    grove->document()->appendChild(new GroveLib::Element(rootElementName));
    setRep(grove.pointer());
}

bool Grove::saveAsXmlFile(int flags, 
                          const GroveStripInfo& si,
                          const SString& saveAs)
{
    if (!getRep())
        return false;
    return GROVE_IMPL->saveAsXmlFile(flags, 
        static_cast<GroveLib::StripInfo*>(si.getRep()), saveAs);    
}

bool Grove::saveAsXmlString(SString& saveTo,
                            int flags, 
                            const GroveStripInfo& si)
{
    if (!getRep())
        return false;
    Common::String save_to;
    bool ok = GROVE_IMPL->saveAsXmlString(save_to, flags,
        static_cast<GroveLib::StripInfo*>(si.getRep()));
    saveTo = save_to;
    return ok;
}

Grove::~Grove()
{
}

GroveDocument Grove::document() const
{
    if (getRep())
        return GROVE_IMPL->document();
    return 0;
}

GroveIdManager Grove::idManager()
{
    if (getRep())
        return GROVE_IMPL->idManager();
    return 0;
}

SString Grove::doctypeName() const
{
    if (getRep())
        return GROVE_IMPL->doctypeName();
    return SString();
}

SString Grove::topSysid() const
{
    if (getRep())
        return GROVE_IMPL->topSysid();
    return SString();
}

GroveEntityDeclSet Grove::entityDecls() const
{
    return getRep() ? GROVE_IMPL->entityDecls() : 0;
}

GroveEntityDeclSet Grove::notations() const
{
    return getRep() ? &GROVE_IMPL->notations() : 0;
}

GroveEntityDeclSet Grove::parameterEntityDecls() const
{
    return getRep() ? &GROVE_IMPL->parameterEntityDecls() : 0;
}

SernaApi::Grove Grove::buildGroveFromString(const SString& s,
                                  GroveBuilderFlags flags,
                                  bool dtdValidate,
                                  const CatalogManager& catMgr)
{
    GroveLib::GroveBuilder* builder = new GroveLib::SpGroveBuilder(flags);
    if (catMgr.getRep())
        builder->setCatalogManager(static_cast<CatMgr::CatalogManager*>
            (catMgr.getRep()));
    GroveLib::GrovePtr grove = builder->buildGrove(s, 
        Common::String(), dtdValidate);
    if (grove && grove->document() && grove->document()->documentElement())
        return grove.pointer();
    return Grove();
}

SernaApi::Grove Grove::buildGroveFromFile(const SString& fn,
                                GroveBuilderFlags flags,
                                bool dtdValidate,
                                const CatalogManager& catMgr)
{
    GroveLib::GroveBuilder* builder = new GroveLib::SpGroveBuilder(flags);
    if (catMgr.getRep()) {
        CatMgr::CatalogManager* cmgr =
            static_cast<CatMgr::CatalogManager*>(catMgr.getRep());
        builder->setCatalogManager(cmgr);
    }
    GroveLib::GrovePtr grove = builder->buildGroveFromFile(fn, dtdValidate);
    if (grove && grove->document() && grove->document()->documentElement())
        return grove.pointer();
    return Grove();
}

CatalogManager::CatalogManager(SernaApiBase* b)
    : SimpleWrappedObject(b)
{
}

SString CatalogManager::getCatalogsList() const
{
    Common::Vector<Common::String> pathVec;
    SString pathstr;
    if (!getRep())
        return pathstr;
    static_cast<CatMgr::CatalogManager*>
        (getRep())->getCatalogList(pathVec);
    for (uint i = 0; i < pathVec.size(); ++i) {
        pathstr += pathVec[i];
        if ((i + 1) < pathVec.size())
            pathstr += Common::PathName::PATH_SEP;
    }
    return pathstr;
}

///////////////////////////////////////////////////////////////////////////////


} // namespace SernaApi
