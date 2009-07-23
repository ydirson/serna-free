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

#include "common/String.h"
#include "common/safecast.h"

#include "grove/grove_trace.h"
#include "grove/grove_defs.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/XNodePtr.h"
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "grove/impl/gsutils.h"
#include <set>

using Common::String;

namespace GroveLib {

//////////////////////////////////////////////////////////////////

const String& DocumentHeading::nodeName() const
{
    static String n("#document-heading");
    return n;
}

//////////////////////////////////////////////////////////////////

const String& DocumentProlog::nodeName() const
{
    static String n("#document-prolog");
    return n;
}

void DocumentProlog::saveAsXml(GroveSaverContext& gsc, int) const
{
    typedef std::list<Node*> NList;
    NList elist;
    std::set<String> neset, peset, geset;
    //
    // stage1 - store in elist all prolog entities and all entities,
    //          that was declared outside the prolog, but were modified
    for (Node* n = firstChild(); n;) {
        switch (n->nodeType()) {
            case ENTITY_DECL_NODE: {
                const EntityDecl* decl =
                    static_cast<EntityDeclNode*>(n)->entityDecl();
                if (decl->declType() == EntityDecl::doctype ||
                    decl->declType() == EntityDecl::document)
                        break;
                if (decl->declOrigin() != EntityDecl::prolog &&
                    !decl->isDeclModified())
                        break;
                EntityDecl* nd = 0;
                std::set<String>* es;
                if (decl->declType() == EntityDecl::internalParameterEntity ||
                    decl->declType() == EntityDecl::externalParameterEntity) {
                    nd = grove()->parameterEntityDecls().
                        lookupDecl(decl->name());
                    es = &peset;
                } else if (decl->declType() == EntityDecl::notation) {
                    nd = grove()->notations().lookupDecl(decl->name());
                    es = &neset;
                } else {
                    nd = grove()->entityDecls()->lookupDecl(decl->name());
                    es = &geset;
                }
                // note: theoretically we must have separate entity
                // sets for external subsets, to be fully correct
                if (nd && es->find(nd->name()) == es->end()) {
                    elist.push_back(n);
                    es->insert(nd->name());
                }
                break;
            }
            case ENTITY_REF_START_NODE: {
                const EntityReferenceStart* ers = CONST_ERS_CAST(n);
                if (ers->entityDecl()->declType() != EntityDecl::doctype) 
                    elist.push_back(n);
                n = ers->getSectEnd();
                continue;
            }
            default: {
                elist.push_back(n);
                break;
            }
        }
        n = n->nextSibling();
    }
    //
    // stage2 - traverse all entity decls in grove,
    //          add all new entity decls (those exist in declaration tree, but
    //          not present in prolog) to the beginning of elist
    EntityDeclSet::iterator iter;
    iter = grove()->notations().begin();
    Common::Vector<const EntityDecl*> decls;
    for (; iter != grove()->notations().end(); ++iter) {
        if ((*iter)->declOrigin() != EntityDecl::prolog
            && (!(*iter)->isDeclModified()))
                continue;
        if ((*iter)->declType() != EntityDecl::notation)
            continue;
        if (neset.find((*iter)->name()) != neset.end())
            continue;
        decls.push_back(iter->pointer());
    }
    iter = grove()->parameterEntityDecls().begin();
    for (; iter != grove()->parameterEntityDecls().end(); ++iter) {
        if (((*iter)->declOrigin() == EntityDecl::prolog ||
            (*iter)->isDeclModified()) &&
                (*iter)->declType() != EntityDecl::doctype &&
                peset.find((*iter)->name()) == peset.end())
                    decls.push_back(iter->pointer());
    }
    iter = grove()->entityDecls()->begin();
    for (; iter != grove()->entityDecls()->end(); ++iter) {
        if (((*iter)->declOrigin() == EntityDecl::prolog ||
            (*iter)->isDeclModified()) &&
            (geset.find((*iter)->name()) == geset.end()))
                decls.push_back(iter->pointer());
    }
    // Stage3 - actual prolog saving
    String dt = grove()->doctypeName();
    if (dt.isEmpty()) {
        Element* root = grove()->document()->documentElement();
        if (!root)
            return;
        dt = root->name();
    }
    bool hasPrologNodes = false;
    bool hasPiNodes = false;
    NList::iterator nit;
    for (nit = elist.begin(); nit != elist.end(); ++nit) {
        if ((*nit)->nodeType() == Node::SSEP_NODE ||
            (*nit)->nodeType() == Node::COMMENT_NODE)
                continue;
        if ((*nit)->nodeType() == Node::PI_NODE) {
            hasPiNodes = true;
            continue;
        }
        hasPrologNodes = true;
        break;
    }
    String dtPrefix = "<!DOCTYPE ";
    dtPrefix += dt;
    bool hadDoctype = false;
    const EntityDecl* decl = grove()->doctypeEntity();
    if (decl && decl->declType() == EntityDecl::doctype) {
        const ExternalEntityDecl* epe =
            SAFE_CAST(const ExternalEntityDecl*, decl);
        if (!epe->pubid().pubid().isEmpty()) {
            gsc.os() << dtPrefix << " PUBLIC \""
                << epe->pubid().pubid() << '"';
            hadDoctype = true;
        }
        if (!epe->sysid().isEmpty()) {
            if (!hadDoctype)
                gsc.os() << dtPrefix << " SYSTEM";
            gsc.os() << " \"" << epe->sysid() << '"';
            hadDoctype = true;
        }
    }
    if (!decls.size() && hadDoctype && !hasPrologNodes && !hasPiNodes) {
        gsc.os() << ">" << CR_ENDL;
        return;
    }
    if (hasPrologNodes || hasPiNodes || decls.size()) {
        if (!hadDoctype)
            gsc.os() << dtPrefix;
        hadDoctype = true;
        gsc.os() << " [";
    }
    // -- prolog begins here
    // save new entity declarations
    for (ulong i = 0; i < decls.size(); ++i) 
        gsc.os() << CR_ENDL << decls[i]->asDeclString();
    
    // save all other declarations
    for (nit = elist.begin(); nit != elist.end(); ++nit) 
        (*nit)->saveAsXml(gsc, 0);
    if (hadDoctype)
        gsc.os() << "]>" << CR_ENDL;
}

//////////////////////////////////////////////////////////////////

void EntityDeclNode::saveAsXml(GroveSaverContext& gsc, int) const
{
    const InternalEntityDecl* ied =
        entityDecl()->asConstInternalEntityDecl();
    if (entityDecl()->isDeclModified() || (ied && ied->isContentModified()))
        gsc.os() << entityDecl()->asDeclString();
    else
        gsc.os() << entityDecl()->originalDecl();
}

const String& EntityDeclNode::nodeName() const
{
    static String n("#entity-decl");
    return n;
}

void EntityDeclNode::setDecl(EntityDecl* decl)
{
    entityDecl_ = decl;
}

Node* EntityDeclNode::copy(Node* newParent) const
{
    EntityDeclNode* n = new EntityDeclNode;
    n->assign(this, newParent);
    n->entityDecl_ = entityDecl_;
    return n;
}

#ifdef GROVE_DEBUG
using COMMON_NS::ind;

void EntityDeclNode::dumpInherited() const
{
    DDBG << "orig_decl:" << abr(entityDecl_->originalDecl())
         << std::endl;
}
#else
void EntityDeclNode::dumpInherited() const
{
}
#endif // GROVE_DEBUG

//////////////////////////////////////////////////////////////////

void ElementDeclNode::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << originalDecl();
}

const String& ElementDeclNode::nodeName() const
{
    static String n("#element-decl");
    return n;
}

ElementDeclNode::ContentToken* ElementDeclNode::content() const
{
    return model_;
}

void ElementDeclNode::setContent(ContentToken* m)
{
    model_ = m;
}

ElementDeclNode::ContentType ElementDeclNode::contentType() const
{
    return ctype_;
}

void ElementDeclNode::setContentType(ContentType t)
{
    ctype_ = t;
}

ulong ElementDeclNode::nElements() const
{
    return elements_.size();
}

const String& ElementDeclNode::element(ulong idx) const
{
    return elements_[idx];
}

void ElementDeclNode::appendElement(const String& elem)
{
    elements_.push_back(elem);
}

const String& ElementDeclNode::originalDecl() const
{
    return originalDecl_;
}

ulong ElementDeclNode::nInclusions() const
{
    return inclusions_.size();
}

const String& ElementDeclNode::inclusion(ulong idx) const
{
    return inclusions_[idx];
}

void ElementDeclNode::appendInclusion(const String& s)
{
    inclusions_.push_back(s);
}

ulong ElementDeclNode::nExclusions() const
{
    return exclusions_.size();
}

const String& ElementDeclNode::exclusion(ulong idx) const
{
    return exclusions_[idx];
}

void ElementDeclNode::appendExclusion(const String& s)
{
    exclusions_.push_back(s);
}

void ElementDeclNode::setOriginalDecl(const String& od)
{
    originalDecl_ = od;
}

Node* ElementDeclNode::copy(Node* newParent) const
{
    ElementDeclNode* n = new ElementDeclNode;
    n->assign(this, newParent);
    n->originalDecl_ = originalDecl_;
    n->elements_   = elements_;
    n->inclusions_ = inclusions_;
    n->exclusions_ = exclusions_;
    if (model_)
        n->model_ = model_->deep_copy();
    else
        n->model_ = 0;
    return n;
}

ElementDeclNode::ContentToken::ContentToken()
    : connector(leaf),
      occurs(none)
{
}

ElementDeclNode::ContentToken*
ElementDeclNode::ContentToken::deep_copy() const
{
    ContentToken* ct = new ContentToken;
    ct->connector = connector;
    ct->occurs = occurs;
    if (static_cast<int>(connector) == leaf) {
        ct->token = token;
        return ct;
    }
    for (ulong i = 0; i < subexps.size(); ++i)
        ct->subexps.push_back(subexps[i]->deep_copy());
    return ct;
}

#ifdef GROVE_DEBUG
void ElementDeclNode::ContentToken::dump() const
{
    static const char* occ_names[] = { "1", "?", "+", "*" };
    static const char* conn_types[] = { "LEAF", "SEQ", "CHOICE", "ALL" };

    DDINDENT;
    DDBG << "occur:" << occ_names[int(occurs)];

    if (static_cast<int>(connector) == leaf) {
        DDBG << " leaf:" << abr(token) << std::endl;
        return;
    }
    DDBG << " group " << abr(conn_types[unsigned(connector)]) << ':'
        << std::endl;
    for (ulong i = 0; i < subexps.size(); ++i)
        subexps[i]->dump();
}
#else
void ElementDeclNode::ContentToken::dump() const
{
}
#endif // GROVE_DEBUG

ElementDeclNode::ContentToken::~ContentToken()
{
    for (ulong i = 0; i < subexps.size(); ++i)
        delete subexps[i];
}

void ElementDeclNode::dumpInherited() const
{
#ifdef GROVE_DEBUG
//    DINDENT(4*indent);

    DDBG << "o_decl:" << abr(originalDecl()) << " c_type:" << int(ctype_)
         << std::endl << "elements:";

    ulong i = 0;
    for (i = 0; i < elements_.size(); i++)
        DDBG << abr(elements_[i]);

    DDBG << std::endl << "inclusions:";
    for (i = 0; i < inclusions_.size(); i++)
        DDBG << abr(inclusions_[i]);

    DDBG << std::endl << "exclusions:";
    for (i = 0; i < exclusions_.size(); i++)
        DDBG << abr(exclusions_[i]);

    DDBG << std::endl;
    if (model_)
        model_->dump();
#endif // GROVE_DEBUG
}

ElementDeclNode::~ElementDeclNode()
{
    delete model_;
}

//////////////////////////////////////////////////////////////////

void AttrDeclNode::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << originalDecl();
}

const String& AttrDeclNode::nodeName() const
{
    static String n("#attr-decl");
    return n;
}

ulong AttrDeclNode::nElements() const
{
    return elements_.size();
}

const String& AttrDeclNode::element(ulong idx) const
{
    return elements_[idx];
}

const String& AttrDeclNode::originalDecl() const
{
    return originalDecl_;
}

ulong AttrDeclNode::AttrDef::nAllowedValues() const
{
    return allowedValues_.size();
}

const String& AttrDeclNode::AttrDef::allowedValue(ulong idx) const
{
    return allowedValues_[idx];
}

void AttrDeclNode::appendElement(const String& elem)
{
    elements_.push_back(elem);
}

void AttrDeclNode::setOriginalDecl(const String& s)
{
    originalDecl_ = s;
}

Node* AttrDeclNode::copy(Node* newParent) const
{
    AttrDeclNode* n = new AttrDeclNode;
    n->assign(this, newParent);
    n->elements_ = elements_;
    n->originalDecl_ = originalDecl_;
    n->attrGroup_ = attrGroup_;
    return n;
}

#ifdef GROVE_DEBUG
void AttrDeclNode::AttrDef::dump() const
{
    DDINDENT;
    DDBG << "ATTR:" << name() << " declared_v:" << int(dv_)
         << " def_type:" << int(deftype_)
         << "def_val:" << abr(defvalue_) << std::endl;
    if (allowedValues_.size() > 0) {
        DDBG << "allowed_values:";
        for (ulong i = 0; i < allowedValues_.size(); ++i)
            DDBG << abr(allowedValues_[i]);
        DDBG << std::endl;
    }
}

void AttrDeclNode::AttrDefGroup::dump() const
{
    DDINDENT;
    DDBG << "ATTR-GROUP: " << name() << std::endl;
    for (ulong di = 0; di < attrDefs_.size(); ++di) {
        DDBG << ind(4) << "Attr#" << di << std::endl;
        attrDefs_[di]->dump();
    }
}

void AttrDeclNode::dumpInherited() const
{
//    DINDENT(4*indent);
    DDBG << "Attlist:";
    for (ulong i = 0; i < elements_.size(); ++i)
        DDBG << abr(elements_[i]);
    DDBG << std::endl;
    attrGroup_.dump();
    DDBG << "orig_decl:" << abr(originalDecl_) << std::endl;
}
#else
void AttrDeclNode::AttrDef::dump() const
{
}
void AttrDeclNode::dumpInherited() const
{
}
void AttrDeclNode::AttrDefGroup::dump() const
{
}
#endif // GROVE_DEBUG

AttrDeclNode::~AttrDeclNode()
{
}

PRTTI_BASE_STUB_NS(AttrDeclNode::AttrDefBase, AttrDef, AttrDeclNode);
PRTTI_BASE_STUB_NS(AttrDeclNode::AttrDefBase, AttrDefGroup, AttrDeclNode);
PRTTI_IMPL2_NS(AttrDeclNode::AttrDef, AttrDef, AttrDeclNode);
PRTTI_IMPL2_NS(AttrDeclNode::AttrDefGroup, AttrDefGroup, AttrDeclNode);

//////////////////////////////////////////////////////////////////

void SSepNode::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << data();
}

const String& SSepNode::nodeName() const
{
    static String n("#space-sep");
    return n;
}

void SSepNode::dumpInherited() const
{
    DDBG << "ssep data:<" << sepdata_ << ">\n";
}

Node* SSepNode::copy(Node* newParent) const
{
    SSepNode* n = new SSepNode;
    n->assign(this, newParent);
    n->sepdata_  = sepdata_;
    return n;
}

} // namespace GroveLib
