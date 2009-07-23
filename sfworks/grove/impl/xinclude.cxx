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
#include "grove/grove_defs.h"
#include "grove/xinclude.h"
#include "grove/Grove.h"
#include "grove/SectionNodes.h"
#include "grove/SectionSyncher.h"
#include "grove/EntityDecl.h"
#include "grove/GroveBuilder.h"
#include "grove/Nodes.h"
#include "grove/XmlNs.h"
#include "grove/IdManager.h"
#include "grove/EntityReferenceTable.h"
#include "grove/grove_trace.h"
#include "grove/Origin.h"
#include "grove/XmlPredefNs.h"
#include "grove/XmlValidator.h"
#include "common/PropertyTree.h"
#include "dav/DavManager.h"
#include "dav/IoStream.h"
#include "GroveMessages.h"
#include <set> 
#include <map> 

#define NODE_ORIGIN(n) CompositeOrigin(new NodeOrigin(n), 0)

using namespace Common;

namespace GroveLib {

// sync data is needed to preserve pointers to the source/target (xincluded)
// sections until we can determine proper processing order (most nested must
// be processed first).
struct SyncData {
    SyncData(EntityReferenceStart* src, EntityReferenceStart* dst)
        : sourceErs_(src), targetErs_(dst) {}
        
    EntityReferenceStart* sourceErs_;
    EntityReferenceStart* targetErs_;

    bool    operator<(const SyncData& other) const
    {
        // reverse comparsion, we need deepest first
        return node_depth(targetErs_) > node_depth(other.targetErs_); 
    }
    static bool node_depth(const Node* n)
    {
        uint depth = 0;
        for (; n; n = parentNode(n))
            ++depth;
        return depth;
    }
};

typedef std::multiset<SyncData> SyncDataSet;

#define RES_MSG_STREAM(xd) \
    MessageListMessenger res_messenger(xd->messageList());\
    res_messenger.incRefCnt();\
    MessageStream rstream(GroveMessages::getFacility(), &res_messenger)

/////////////////////////////////////////////////////////////////////

// Parse XPTR element scheme, and get the range (if any).
// (at this point, use only pure ID)
Node* xptr_element(Grove* grove, const String& expr)
{
    if (expr.isEmpty()) 
        return grove->document()->documentElement();
    if (0 == grove->idManager())
        return false;
    return grove->idManager()->lookupElement(expr);
}

static bool same_grove_url(const Grove* grove, const String& url)
{
    return Url(grove->topSysid()) == Url(url);
}

static EntityReferenceStart* get_target_sect(Grove* grove,
                                             EntityReferenceStart* sourceErs,
                                             bool disableBackrefs = false)
{   
    XincludeDecl* xd = static_cast<XincludeDecl*>(sourceErs->entityDecl());
    RES_MSG_STREAM(xd);
    Node* target = xptr_element(grove, xd->expr());
    if (!target) {
        rstream << GroveMessages::xiUnresolvedExpr << xd->expr()
                << NODE_ORIGIN(sourceErs);
        return 0;
    }
    if (xinclude_check_recursion(sourceErs, target)) {
        rstream << GroveMessages::xiRecursiveInclusion
                << NODE_ORIGIN(sourceErs);
        return 0;
    }
    Node* ps = target->prevSibling();
    EntityReferenceStart* target_ers = 0;
    if (ps && ps->nodeType() == Node::ENTITY_REF_START_NODE) {
        EntityReferenceStart* target_ers = ERS_CAST(ps);
        if (target_ers->entityDecl()->declType() == EntityDecl::xinclude) {
            DBG(GROVE.XINCLUDE) << "Returning existing section\n";
            // update declaration (so it will be the target one)
            sourceErs->ert()->removeEntityRef(sourceErs);
            sourceErs->setDecl(target_ers->entityDecl());
            sourceErs->ert()->addEntityRef(sourceErs);
            return target_ers;                              
        }
    }
    if (disableBackrefs)
        return 0;
    target_ers = new EntityReferenceStart;
    target_ers->setDecl(xd);
    xd->setReferenceErs(target_ers);
    xd->setParentGrove(sourceErs->grove());
    GroveSectionStart::convertToSection(Node::ENTITY_SECTION, target,
        target, target_ers);
    DBG(GROVE.XINCLUDE) << "Created new target section\n";
    //DBG_IF(GROVE.XINCLUDE) target_ers->dumpSectionInfo();
    return target_ers;
}

void sync_with_xinclude_sect(EntityReferenceStart* sourceSect,
                             EntityReferenceStart* targetSect)
{
    if (0 == sourceSect)
        return;
    XincludeDecl* xd = static_cast<XincludeDecl*>(sourceSect->entityDecl());
    if (0 == targetSect || sourceSect == targetSect) {
        xd->setFallback(true);
        return;
    }
    DocumentFragmentPtr fragment;
    if (targetSect->nextSibling() != targetSect->getSectEnd())
        fragment = targetSect->nextSibling()->copyAsFragment
            (targetSect->getSectEnd()->prevSibling());
    else
        fragment = new DocumentFragment;
    if (fragment.isNull() || !fragment->firstChild()) {
        xd->setFallback(true);
        return;         // fallback remains
    } 
    else
        xd->setFallback(false);
    sync_gsr_rss(fragment.pointer());
    if (sourceSect->nextSibling() != sourceSect->getSectEnd())
        sourceSect->nextSibling()->removeGroup
            (sourceSect->getSectEnd()->prevSibling());
    IdManager* id_mgr = sourceSect->grove()->root()->idManager();
    if (id_mgr && !id_mgr->isEnabled()) {
        for (Node* n = fragment->firstChild(); n; n = n->nextSibling())
            id_mgr->childInserted(n);
    }
    sourceSect->insertAfter(fragment.pointer());
    xd->setParentGrove(sourceSect->grove());
    xd->setReferenceErs(targetSect);
}

GROVE_EXPIMP Grove* find_grove(Grove* g, const String& path)
{
    DBG(GROVE.XINCLUDE) << "find_grove try: g->topSysid=" << g->topSysid()
        << ", path=<" << path << ">\n";
    if (same_grove_url(g, path))
        return g;
    for (g = g->firstChild(); g; g = g->nextSibling())
        if (find_grove(g, path))
            return g;
    return 0;
}

GROVE_EXPIMP Grove* xinclude_find_grove(EntityReferenceStart* xers)
{
    XincludeDecl* decl = static_cast<XincludeDecl*>(xers->entityDecl());
    //! TODO: check grove type (text or not)
    return find_grove(xers->grove()->groveBuilder()->rootGrove(), 
                      CONST_ERS_CAST(xers->getSectParent())->xmlBase(
                          decl->url()));
}

GROVE_EXPIMP Text* get_grove_text(const Grove* grove)
{
    GroveLib::Node* n = grove->document()->firstChild();
    for (; n; n = n->nextSibling()) 
        if (n->nodeType() == GroveLib::Node::TEXT_NODE) 
            return static_cast<Text*>(n);
    return 0;
}

static Grove* make_text_grove(const EntityReferenceStart* xers,
                              Text** result_text, bool* ok)
{
    *ok = false;
    XincludeDecl* decl = static_cast<XincludeDecl*>(xers->entityDecl());
    RES_MSG_STREAM(decl);
    Encodings::Encoding enc = xers->grove()->groveBuilder()->getEncoding();
    if (!decl->encodingString().isEmpty()) {
        enc = Encodings::encodingByName(decl->encodingString());
        if (enc == Encodings::XML)
            enc = Encodings::UTF_8;
    }
    Dav::IoStream ios;
    Grove* grove = new Grove;
    grove->setGroveBuilder(xers->grove()->groveBuilder()->copy());
    grove->groveBuilder()->setEncoding(enc);
    grove->topDecl()->setEncoding(enc);
    grove->topDecl()->setDataType(EntityDecl::cdata);
    grove->setTopSysid(static_cast<const EntityReferenceStart*>
        (xers->getSectParent())->xmlBase(decl->url()));
    *result_text = new Text;
    grove->document()->appendChild(*result_text);
    if (Dav::DavManager::instance().open(grove->topSysid(),
        Dav::DAV_OPEN_READ, ios) != Dav::DAV_RESULT_OK) {
            rstream << GroveMessages::xiCannotOpen 
                << decl->url() << NODE_ORIGIN(xers)
                << Dav::DavManager::instance().lastError() 
                << (xers->firstChild() 
                    ? Message::L_WARNING : Message::L_ERROR);
            return grove;
    }
    ios.setEncoding(enc);
    String istr;
    ios >> istr;
    (*result_text)->setData(istr);
    *ok = true;
    return grove;
}

static void process_text_xinclude(EntityReferenceStart* xers,
                                  SyncDataSet& sds)
{
    
    XincludeDecl* decl = static_cast<XincludeDecl*>(xers->entityDecl());
    RES_MSG_STREAM(decl);
    DBG(GROVE.XINCLUDE) << "Processint TEXT xinclude, URL = "
        << xers->xmlBase(decl->url()) << std::endl;
    // try to find the text grove with the same path
    Grove* gp = xinclude_find_grove(xers);
    if (gp) {
        DBG(GROVE.XINCLUDE) << "Found existing TEXT grove\n";
        EntityReferenceStart* target_ers = 
            static_cast<EntityReferenceStart*>
                (gp->document()->ers()->getFirstSect());
        // update declaration (so it will be the target one)
        xers->ert()->removeEntityRef(xers);
        xers->setDecl(target_ers->entityDecl());
        xers->ert()->addEntityRef(xers);
        sds.insert(SyncData(xers, target_ers));
        return;
    }
    Text* text = 0;
    bool ok = false;
    GrovePtr grove = make_text_grove(xers, &text, &ok);
    EntityReferenceStart* target_ers = new EntityReferenceStart;
    target_ers->setDecl(decl);
    GroveSectionStart::convertToSection(Node::ENTITY_SECTION, 
        text, text, target_ers);
    if (!ok) {
        decl->setFallback(true);
        text->remove();
    }
    sds.insert(SyncData(xers, target_ers));
    decl->setReferenceErs(target_ers);
    decl->setParentGrove(xers->grove());
    xers->grove()->groveBuilder()->rootGrove()->appendChild(grove.pointer());
}

static void process_xinclude_section(EntityReferenceStart* xers,
                                     SyncDataSet& sds)
{
    XincludeDecl* decl = static_cast<XincludeDecl*>(xers->entityDecl());
    RES_MSG_STREAM(decl);
    if (decl->url().isEmpty()) {
        if (decl->expr().isEmpty()) {
            rstream << GroveMessages::xiUrlExprEmpty 
                     << NODE_ORIGIN(xers)
                     << Message::L_ERROR;
            return;
        } 
        // url is empty = same grove
        sds.insert(SyncData(xers, get_target_sect(xers->grove(), xers)));
        return;    
    }
    if (decl->dataType() == EntityDecl::cdata) {
        process_text_xinclude(xers, sds);
        return;
    }
    const EntityReferenceStart* baseref = 
        CONST_ERS_CAST(xers->getSectParent());
    DBG(GROVE.XINCLUDE) << "Processing XML/XINCLUDE, URL = "
        << baseref->xmlBase(decl->url()) << std::endl;
    GroveBuilder* xgb = xers->grove()->groveBuilder();
    Grove* gp = xinclude_find_grove(xers);
    if (gp) {
        DBG(GROVE.XINCLUDE) << "Found existing XML grove\n";
        sds.insert(SyncData(xers, get_target_sect(gp, xers,
            xgb->flags() & GroveBuilder::disableXincludeBackrefs)));
        return;
    }
    String grove_path = baseref->xmlBase(decl->url());
    if (grove_path == baseref->grove()->topSysid()) {
        rstream << GroveMessages::xiRecursiveInclusion 
                << NODE_ORIGIN(xers) << Message::L_ERROR;
        decl->setFallback(true);
        return;
    }
    Grove* rootGrove = xgb->rootGrove();
    GroveBuilder* gb = xgb->copy();
    gb->setMessenger(new MessageListMessenger(decl->messageList()));
    gb->setEncoding(Encodings::XML);
    GrovePtr grove = gb->buildGroveFromFile(grove_path, false);
    if (0 == grove || !grove->document()->documentElement()) {
        rstream << GroveMessages::xiNoDocElement << decl->url()
                << NODE_ORIGIN(xers) << Message::L_ERROR;
        decl->setFallback(true);
        return;
    }
    const XmlValidatorProvider* xml_validator_provider = 
        rootGrove->groveBuilder()->getValidatorProvider();
    XmlValidatorPtr xml_validator; 
    if (xml_validator_provider)
        xml_validator = xml_validator_provider->getValidator(grove.pointer());
    if (xml_validator)
        xml_validator->validate(grove->document()->documentElement(),
            XmlValidator::OPEN_MODE|XmlValidator::XINCLUDE);
    rootGrove->appendChild(grove.pointer());
    sds.insert(SyncData(xers, get_target_sect(grove.pointer(), xers)));
}

GROVE_EXPIMP bool is_xinclude_element(const Element* e)
{
    return e->localName() == "include" && e->xmlNsUri() == XmlNs::xincludeNs();
}

// Make section in the source grove (replace xi:include with the
// xinclude entity reference)
EntityReferenceStart* xinclude_make_xers(Element* e)
{
    EntityReferenceStart* ers = new EntityReferenceStart;
    XincludeDecl* xi_decl     = new XincludeDecl;
    ers->setDecl(xi_decl);
    const Attr* a = e->attrs().getAttribute("href");
    if (a)
        xi_decl->setUrl(a->value());
    a = e->attrs().getAttribute("parse");
    if (a) {
        if (a->value() == "xml")
            xi_decl->setDataType(EntityDecl::sgml);
        else if (a->value() == "text")
            xi_decl->setDataType(EntityDecl::cdata);
        else
            xi_decl->setDataType(EntityDecl::invalidDataType);
    }
    a = e->attrs().getAttribute("xpointer");
    if (a)
        xi_decl->setOriginalDecl(a->value());
    a = e->attrs().getAttribute("encoding");
    if (a)
        xi_decl->setEncodingString(a->value());
    GroveSectionStart::convertToSection(Node::ENTITY_SECTION, e, e, ers);
    Node* n = e->firstChild();
    const Element* fallback_elem = 0;
    for (; n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE)
            continue;
        fallback_elem = CONST_ELEMENT_CAST(n);
        if (fallback_elem->localName() == "fallback" &&
            fallback_elem->xmlNsUri() == XmlNs::xincludeNs())
                break;
    }
    NodePtr np = e;
    e->remove();
    ers->appendChild(e);
    if (fallback_elem && fallback_elem->firstChild()) {
        DocumentFragmentPtr fragment = fallback_elem->firstChild()->
            copyAsFragment(fallback_elem->lastChild());
        if (fragment)
            ers->insertAfter(fragment.pointer());
    }
    return ers;
}

GROVE_EXPIMP void process_xinclude_elements(const Vector<Element*>& ev)
{
    SyncDataSet sds;
    for (uint i = 0; i < ev.size(); ++i) {
        Element* e = ev[i];
        DBG_IF(GROVE.XINCLUDE) {
            DDBG << "Processing XINCLUDE: "; 
            e->dump();
        }
        process_xinclude_section(xinclude_make_xers(e), sds);
    }
    SyncDataSet::iterator it = sds.begin();
    for (; it != sds.end(); ++it)
        sync_with_xinclude_sect(it->sourceErs_, it->targetErs_);
}

static void sync_attr(Element* elem, const String& attName, const String& value)
{
    Attr* a = elem->attrs().getAttribute(attName);
    if (value.isEmpty()) {
        if (a)
            a->remove();
        return;
    } 
    if (a) {
        if (a->value() != value)
            a->setValue(value);
    } 
    else {
        a = new Attr(attName);
        a->setValue(value);
        elem->attrs().appendChild(a);
    }
}

void xinclude_fixup_ref(const EntityReferenceStart* xers)
{
    Element* xi_elem = ELEMENT_CAST(xers->firstChild());
    const XincludeDecl* decl = 
        static_cast<const XincludeDecl*>(xers->entityDecl());
    if (xi_elem->xmlNsUri() != W3C_XINCLUDE_NAMESPACE) {
        xi_elem->setName("xi:include");
        xi_elem->addToPrefixMap("xi", W3C_XINCLUDE_NAMESPACE);
    }
    sync_attr(xi_elem, "href", decl->url());
    sync_attr(xi_elem, "xpointer", decl->expr());
}

bool can_remove_xinclude_section(const Node* n)
{
    if (0 == n)
        return true;
    const EntityReferenceStart* ers = 0;
    const EntityReferenceStart* ref_ers = 0;
    switch (n->nodeType()) {
        case Node::ENTITY_REF_START_NODE:
            ers = CONST_ERS_CAST(n); break;
        case Node::ENTITY_REF_END_NODE:
            ers = CONST_ERS_CAST(CONST_ERE_CAST(n)->getSectStart());
            break;
        default:
            break;
    }
    if (0 == ers || ers->entityDecl()->declType() != EntityDecl::xinclude)
        return true;
    const EntityReferenceStart* source_ers = ers;
    ref_ers = static_cast<const XincludeDecl*>(ers->entityDecl())->
        referenceErs();
    if (0 == ref_ers)
        return true;
    ers = CONST_ERS_CAST(ers->getSectParent());
    bool nested_xinclude = false;
    for (; ers && ers->getSectParent();
         ers = CONST_ERS_CAST(ers->getSectParent())) {
        if (ers->entityDecl()->declType() == EntityDecl::xinclude) {
            nested_xinclude = static_cast<const XincludeDecl*>
                (ers->entityDecl())->referenceErs()->grove() ==
                ref_ers->grove();
            break;
        }
    }
    if (nested_xinclude) {
        const EntityReferenceTable::ErtEntry* ee = 
            n->getGSR()->ert()->lookup(source_ers->entityDecl());
        if (ee->numOfRefs() > 1)
            return false;
    }
    return true;
}

bool xinclude_check_recursion(const EntityReferenceStart* xers,
                              const Node* targetNode)
{
    const EntityReferenceStart* ters = CONST_ERS_CAST(
        targetNode->getPrevClosure(Node::ENTITY_REF_START_NODE));
    if (0 == ters || 0 == ters->getSectParent())
        return false;
    for (; ters && ters->getSectParent();
         ters = CONST_ERS_CAST(ters->getSectParent())) 
        if (ters->entityDecl()->declType() == EntityDecl::xinclude) 
            return xers->hasLoop(ters->entityDecl()->name());
    return false;
}

/////////////////////////////////////////////////////////////////////////

GROVE_EXPIMP bool check_has_entity(const Grove* grove, const String& path)
{
    Url url(path);
    EntityReferenceTable* ert = grove->document()->ert();
    EntityReferenceTable::iterator ei = ert->begin();
    for (; ei != ert->end(); ++ei) {
        const EntityDecl* ed = (*ei)->decl();
        if (ed->declType() != EntityDecl::externalGeneralEntity)
            continue;
        if (Url(ed->asConstExternalEntityDecl()->entityPath(grove)) == url)
            return true;
    }
    for (grove = grove->firstChild(); grove; grove = grove->nextSibling())
        if (check_has_entity(grove, path))
            return true;
    return false;
}

} // namespace GroveLib
