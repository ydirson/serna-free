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

#include "xs/xs_defs.h"
#include "grove/Node.h"
#include "xs/XsNodeExt.h"
#include "xs/XsType.h"
#include "xs/Schema.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/Connector.h"
#include "xs/complex/ChoiceParticle.h"
#include "xs/complex/SimpleContent.h"
#include "xs/components/XsElementImpl.h"
#include "xs/components/AttributeSet.h"
#include "xs/datatypes/DerivationTypes.h"
#include <iostream>

USING_GROVE_NAMESPACE
USING_XS_NAMESPACE

namespace {

template <class T> T* ext_set(Node* n, const Component* c)
{
    // Avoid reallocating nodeExt, if possible
    NodeExt* ext = n->nodeExt();
    EntityReferenceStart* ers = 0;
    uint line = 0, col = 0;
    if (ext) {
        XsNodeExt* xne = ext->asXsNodeExt();
        if (xne && int(xne->type()) == int(T::ExtTypeValue)) {
            xne->setComponent(c);
            return static_cast<T*>(ext);
        }
        LineLocExt* lle = ext->asLineLocExt();
        if (lle) {
            ers  = lle->getErs();
            line = lle->line();
            col  = lle->column();
        } else {
            EntityNodeExt* ene = ext->asEntityNodeExt();
            if (ene)
                ers = ene->getErs();
        }
    }
    T* new_ext = new T(c);
    new_ext->setLineInfo(line, col);
    new_ext->setErs(ers);
    n->setNodeExt(new_ext);
    return new_ext;
}

inline void copy_props(Common::PropertyNode* to,
                       const Common::PropertyNode* from)
{
    Common::PropertyNode* pn = from->firstChild();
    for (; pn; pn = pn->nextSibling())
        to->appendChild(pn->copy(true));
}

} // namespace

void XsNodeExt::set(Element* elem, const XsElement* xse)
{
    if (elem->parent() && elem->parent()->nodeExt()) {
        XsNodeExt* ext = elem->parent()->nodeExt()->asXsNodeExt();
        if (!ext)
            return;
        const XsElement* xe = ext->xsElement();
        if (!xe || !xe->isValid())
            return;
        const XsType* xst = xe->xstype();
        if (xst && XsType::unknown != xst->typeClass()) {
            const XsComplexTypeImpl* xcti = xst->asConstXsComplexTypeImpl();
            if (xcti) {
                const ComplexContent* cc =
                    xcti->content()->asConstComplexContent();
                if (cc) {
                    const Connector* conn = cc->connector();
                    if (conn && conn->type() == Connector::ALL) {
                        ext_set<AllXsNodeExt>(elem, xse);
                        return;
                    }
                }
            }
        }
    }
    ext_set<DfaXsNodeExt>(elem, xse);
}

void XsNodeExt::set(Node* n, const XsAttribute* xsa)
{
    ext_set<AttrXsNodeExt>(n, xsa);
}

void XsNodeExt::set(Node* n, const FsmMatcher::DfaState* ds)
{
    ext_set<DfaXsNodeExt>(n, 0)->setDfaState(ds);
}

GroveLib::NodeExt* XsNodeExt::copy() const
{
    return new XsNodeExt(component_);
}

void XsNodeExt::set(Node* n, XS_NAMESPACE::ChoiceParticle* cp)
{
    ChoiceXsNodeExt* ext = ext_set<ChoiceXsNodeExt>(n, 0);
    ext->setChoiceParticle(cp);
    ChoiceNode* cn = static_cast<GroveLib::ChoiceNode*>(n);
    cn->elemList_.setRoot(const_cast<Common::PropertyNode*>(ext->elist()));
}

bool XsNodeExt::getList(Common::PropertyNode* list, Node* node,
                        Node* nparent, bool replace)
{
    Element* parent;
    if (!node)
        parent = ELEMENT_CAST(nparent);
    else
        parent = ELEMENT_CAST(node->parent());
    while (node && node->nodeType() != Node::ELEMENT_NODE)
        node = node->nextSibling();
    if (node && node->nodeType() == GroveLib::Node::CHOICE_NODE) {
        copy_props(list,
            static_cast<const GroveLib::ChoiceNode*>(node)->elemList());
        return false;
    }
    bool append = false;
    if (!node) {
        if (!parent)
            return false;
        node = parent;
        append = true;
    }
    if (parent && !parent->parent())
        append = true;

    XsListNodeExt* xslst = 0;
    NodeExt* ext = node->nodeExt();
    if (!ext)
        return false;
    XsNodeExt* xs_ext = ext->asXsNodeExt();
    if (!xs_ext)
        return false;
    if (xs_ext->isAnyTypeExt())
        return true;
    ChoiceXsNodeExt* ch_ext = xs_ext->asChoiceXsNodeExt();
    DfaXsNodeExt* dfa_ext = xs_ext->asDfaXsNodeExt();
    AllXsNodeExt* all_ext = xs_ext->asAllXsNodeExt();
    if (dfa_ext)
        xslst = static_cast<XsListNodeExt*>(dfa_ext);
    else if (all_ext)
        xslst = static_cast<XsListNodeExt*>(all_ext);
    else if (ch_ext) 
        copy_props(list, ch_ext->elist());
    if (!xslst)
        return false;
    if (replace)
        xslst->getElemListReplace(list, ELEMENT_CAST(node));
    else if (append)
        xslst->getElemListAppend(list, ELEMENT_CAST(node));
    else
        xslst->getElemListBefore(list, ELEMENT_CAST(node));
    return false;
}

bool XsNodeExt::getAttrs(Common::PropertyNode* to,
                         const XsElement* xe, const Element* elem)
{
    if (0 == xe)
        return false;
    const XsType* xst = xe->xstype();
    if (!xst || XsType::unknown == xst->typeClass())
        return false;
    const XsComplexTypeImpl* xcti = xst->asConstXsComplexTypeImpl();
    if (!xcti)
        return false;
    const XsContent* xc = xcti->content();
    if (!xc)
        return false;
    Common::RefCntPtr<AttributeSet> attrset = xc->attributeSet();
    Common::Vector<XsAttribute*> attlist;
    bool ok = attrset->attrList(attlist);
    for (uint i = 0; i < attlist.size(); ++i)
        to->appendChild(attlist[i]->makeSpec(elem));
    return ok;
}

bool XsNodeExt::getAttrs(Common::PropertyNode* to, Node* node)
{
    if (!node || node->nodeType() != Node::ELEMENT_NODE ||
         !node->nodeExt())
        return false;
    XsNodeExt* ext = node->nodeExt()->asXsNodeExt();
    if (0 == ext)
        return false;
    const XsElement* xe = ext->xsElement();
    return getAttrs(to, xe, static_cast<Element*>(node));
}

//get SimpleTypeContaining Element Value
bool XsNodeExt::getSTCEvalue(Common::PropertyNode* to, Node* node)
{
    if (!node || node->nodeType() != Node::ELEMENT_NODE ||
         !node->nodeExt())
        return false;
    XsNodeExt* ext = node->nodeExt()->asXsNodeExt();
    if (0 == ext)
        return false;
    const XsElement* xe = ext->xsElement();
    if (!xe->isValid())
        return false;
    const XsElementImpl* xei = static_cast<const XsElementImpl*>(xe);
    if (0 == xe)
        return false;
    const XsType* xst = xei->xstype();
    if (!xst || XsType::unknown == xst->typeClass())
        return false;
    const XsSimpleTypeImpl* xsti = xst->getSimpleType();
    if (0 == xsti)
        return false;
    xei->getPossibleEnumlist(to);
    if (!to->getProperty(Xs::ELEM_VALUE_DEFAULT)) {
        String src, result;
        xsti->validate(0, node, src, &result);
        if (!result.isEmpty())
            to->makeDescendant(Xs::ELEM_VALUE_DEFAULT)->setString(result);
    }
    return true;
}

const XsElement* XsNodeExt::xsElement(const Node* n)
{
    if (n && n->nodeExt()) {
        const XsNodeExt* xne = n->nodeExt()->asConstXsNodeExt();
        if (xne)
            return xne->xsElement();
    }
    return 0;
}

const XsAttribute* XsNodeExt::xsAttribute(const Node* n)
{
    if (n && n->nodeExt()) {
        const XsNodeExt* xne = n->nodeExt()->asConstXsNodeExt();
        if (xne)
            return xne->xsAttribute();
    }
    return 0;
}

const XS_NAMESPACE::Matcher* XsNodeExt::getMatcher() const
{
    if (type() != DFA && type() != ALL)
        return 0;     // node is not validated or of a bad typ
    const XsElement* xselem = xsElement();
    if (0 == xselem)
        return 0;
    const XsType* xst = xselem->xstype();
    if (!xst)
        return 0;
    const XsComplexTypeImpl* xcti = xst->asConstXsComplexTypeImpl();
    if (!xcti)
        return 0;
    const XsContent* xc = xcti->content();
    if (!xc)
        return 0;
    const ComplexContent* cc = xc->asConstComplexContent();
    if (!cc || !cc->connector())
        return 0;
    return cc->matcher(0, String::null());
}

const XsElement* XsNodeExt::getNextXsElem(const GroveLib::Node* parent,
                                          const GroveLib::Node* node,
                                          const GroveLib::ExpandedName& n)
{
    while (node && node->nodeType() != GroveLib::Node::ELEMENT_NODE)
        node = node->prevSibling();
    if (node) {
        const NodeExt* ne = node->nodeExt();
        if (ne) {
            const XsNodeExt* xsext = ne->asConstXsNodeExt();
            if (xsext) {
                const DfaXsNodeExt* dfaext = xsext->asConstDfaXsNodeExt();
                ExpandedName cur = static_cast<const NodeWithNamespace*>
                                  (node)->expandedName();
                if (dfaext)
                    return dfaext->getNextElem(cur, n);
            }
        }
    }
    const NodeExt* ne = parent->nodeExt();
    if (0 == ne)
        return 0;
    const XsNodeExt* xsext = ne->asConstXsNodeExt();
    if (0 == xsext)
        return 0;
    const Matcher* m = xsext->getMatcher();
    if (m)
        return m->getFirstElem(n);
    return 0;
}

//////////////////////////////////////////////////////////////

#ifdef XS_DEBUG

void XsNodeExt::do_dump(int indent, const char* name) const
{
    int i;
    String xsv_flags("");
    if (udata() & 001)
        xsv_flags += NOTR(" VALIDATED");
    if (udata() & 002)
        xsv_flags += NOTR(" VALID");
    if (udata() & 004)
        xsv_flags += NOTR(" VALIDATEDCONTENT");
    if (udata() & 010)
        xsv_flags += NOTR(" VALIDCONTENT");
    if (udata() & 020)
        xsv_flags += NOTR(" GENERATED");
    if (udata() & 040)
        xsv_flags += NOTR(" CHOICE");

    if (!xsv_flags.isEmpty()) {
        for (i = 0; i < indent; ++i)
            std::cerr << ' ';
        std::cerr << NOTR("XSV_flags: ") << (int)udata() << ',' 
            << xsv_flags << std::endl;
    }    
    for (i = 0; i < indent; ++i)
        std::cerr << ' ';
    std::cerr << "EXT-" << name << "<"
              << (component_ ? component_->constCred()->format(): "")
              << "> ";
}

#endif // XS_DEBUG

XS_NAMESPACE_BEGIN

const Common::PropertyNode* ChoiceXsNodeExt::elist() const
{
    return cp_->elist();
}

ChoiceXsNodeExt::ChoiceXsNodeExt(const Component* c)
    : XsNodeExt(c)
{
}

GroveLib::NodeExt* ChoiceXsNodeExt::copy() const
{
    ChoiceXsNodeExt* ext = new ChoiceXsNodeExt(component_);
    ext->cp_ = cp_;
    return ext;
}

void ChoiceXsNodeExt::setChoiceParticle(ChoiceParticle* p)
{
    cp_ = p;
}

void ChoiceXsNodeExt::dump(int) const
{
#ifdef XS_DEBUG
    cp_->dump(0);
#endif // XS_DEBUG
}

ChoiceXsNodeExt::~ChoiceXsNodeExt()
{
}

//////////////////////////////////////////////////////////////

GroveLib::NodeExt* DfaXsNodeExt::copy() const
{
    return new DfaXsNodeExt(component_, dfaState_);
}

void DfaXsNodeExt::dump(int indent) const
{
#ifdef XS_DEBUG
    do_dump(indent, "DFA");
    std::cout << ": DFASTATE 0x" << std::hex << dfaState() << std::endl;
#endif // XS_DEBUG
}

GroveLib::NodeExt* AllXsNodeExt::copy() const
{
    return new AllXsNodeExt(component_);
}

void AllXsNodeExt::dump(int indent) const
{
#ifdef XS_DEBUG
    do_dump(indent, "ALL");
    std::cout << std::endl;
#endif // XS_DEBUG
}

GroveLib::NodeExt* SimpleTypeXsNodeExt::copy() const
{
    return new SimpleTypeXsNodeExt(component_);
}

void SimpleTypeXsNodeExt::dump(int indent) const
{
#ifdef XS_DEBUG
    do_dump(indent, "STYPE");
    std::cout << std::endl;
#endif // XS_DEBUG
}

GroveLib::NodeExt* AttrXsNodeExt::copy() const
{
    return new AttrXsNodeExt(component_);
}

void AttrXsNodeExt::dump(int indent) const
{
#ifdef XS_DEBUG
    do_dump(indent, "ATTR");
    std::cout << std::endl;
#endif // XS_DEBUG
}

//////////////////////////////////////////////////////////////

PRTTI_IMPL(DfaXsNodeExt);
PRTTI_IMPL(AllXsNodeExt);
PRTTI_IMPL(ChoiceXsNodeExt);

XS_NAMESPACE_END

PRTTI_IMPL2_NS(XsNodeExt, LineLocExt, GroveLib);

PRTTI_IMPL(XsNodeExt);
PRTTI_BASE_STUB_NS(XsNodeExt, DfaXsNodeExt, XS_NAMESPACE);
PRTTI_BASE_STUB_NS(XsNodeExt, AllXsNodeExt, XS_NAMESPACE);
PRTTI_BASE_STUB_NS(XsNodeExt, ChoiceXsNodeExt, XS_NAMESPACE);
