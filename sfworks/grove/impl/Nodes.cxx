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
#include "grove/grove_trace.h"
#include "grove/grove_defs.h"
#include "common/String.h"
#include "common/safecast.h"
#include "common/asserts.h"
#include "common/Url.h"

#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"
#include "grove/PrologNodes.h"
#include "grove/ChoiceNode.h"
#include "grove/Grove.h"
#include "grove/EntityReferenceTable.h"
#include "grove/EntityDeclSet.h"
#include "grove/XmlNs.h"
#include "grove/GroveUtils.h"
#include "grove/impl/gsutils.h"
#include "grove/xinclude.h"
#include <set>

using namespace Common;

namespace GroveLib {

typedef GroveSaverContext GSC;

////// DocumentFragment

const String& DocumentFragment::nodeName() const
{
    static String n("#document-fragment");
    return n;
}

Node* DocumentFragment::copy(Node* newParent) const
{
    DocumentFragment* f = new DocumentFragment;
    f->assign(this, newParent);
    f->setGrove(grove());
    f->setGSR(f);
    return f;
}

bool DocumentFragment::saveAsXmlString(Common::String& saveTo,
                                       int flags, 
                                       const StripInfo* si) const
{
    GroveSaverContext gsc(si, grove(), flags);
    gsc.setOutputString();
    saveAsXml(gsc, 0);
    saveTo = gsc.getSavedString();
    return true;
}

///////////////////////////////////////////////////////////////////////

const String& Document::nodeName() const
{
    static String n("#document");
    return n;
}

Element* Document::documentElement() const
{
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() == ELEMENT_NODE)
            return static_cast<Element*>(n);
    }
    return 0;
}

Node* Document::copy(Node*) const
{
    RT_MSG_ABORT("Attempt to copy document node");
    return 0;
}

///////////////////////////////////////////////////////////////////////

Attr* AttrList::getAttribute(const String& name) const
{
    for (Attr* a = firstChild(); a; a = a->nextSibling())
        if (a->name() == name)
            return a;
    return 0;
}

bool AttrList::removeAttribute(const String& name)
{
    Attr* attr = getAttribute(name);
    if (attr) {
        attr->remove();
        return true;
    }
    return false;
}

void AttrList::setAttribute(Attr* attr)
{
    Attr* oldattr = getAttribute(attr->name());
    if (oldattr)
        oldattr->remove();
    appendChild(attr);
}

////////////////////////////////////////////////////////////////////////

// Determine whether we have pure element model and whether
// current element is empty at all
static int make_elem_child_flags(const GroveSaverContext& gsc,
                                 const Element* e,
                                 int cFlags)
{
    int child_flags = cFlags; // inherit
    child_flags &= ~(GSC::HAS_CONTENT|GSC::MIXED_CONTENT|GSC::HAS_EREFS);
    if (gsc.stripInfo()) {
        if (gsc.stripInfo()->checkStrip(e))
            child_flags |= GSC::STRIP_ALLOWED;
        else
            child_flags &= ~GSC::STRIP_ALLOWED;
    } else if ((child_flags & GSC::MIXED_CONTENT) && 
        (gsc.flags() & Grove::GS_INDENT)) 
            child_flags |= GSC::STRIP_ALLOWED;
     else
        child_flags &= ~GSC::STRIP_ALLOWED;
    for (const Node* n = e->firstChild(); n; ) {
        switch (n->nodeType()) {
            case Node::TEXT_NODE: {
                const Text* t = CONST_TEXT_CAST(n);
                const bool strip_allowed = child_flags & GSC::STRIP_ALLOWED;
                const bool has_preserved_spaces = t->data().length() &&
                    !strip_allowed;
                if (!t->isEmpty() || has_preserved_spaces) 
                    child_flags |= GSC::HAS_CONTENT|GSC::MIXED_CONTENT;
                break;
            }
            case Node::ENTITY_REF_START_NODE: {
                    child_flags |= GSC::HAS_EREFS;
                    break;
            }
            case Node::ELEMENT_NODE: 
            case Node::REDLINE_START_NODE:
            case Node::COMMENT_NODE:
            case Node::PI_NODE:
                    child_flags |= GSC::HAS_CONTENT;
                    break;
            default:
                break;
        }
        n = n->nextSibling();
    }
    return child_flags;
}

void Element::saveAsXml(GroveSaverContext& gsc, int cFlags) const
{
    int child_flags = make_elem_child_flags(gsc, this, cFlags);
    const bool can_indent = gsc.flags() & Grove::GS_INDENT;
    const bool can_indent_self = can_indent && 
        !(cFlags & GSC::MIXED_CONTENT) && (cFlags & GSC::STRIP_ALLOWED);
    const bool can_indent_content = 
        can_indent && !(child_flags & GSC::MIXED_CONTENT) &&
        (child_flags & GSC::STRIP_ALLOWED);
    const Element* local_root = gsc.localRoot();
    typedef std::set<String> PrefixSet;
    PrefixSet pset;
    if (can_indent_self) 
        gsc.printIndent();
    gsc.adjustIndent(1);
    String gi;
    if (xmlNsPrefix().isEmpty()) 
        gi = localName();
    else {
        gi = xmlNsPrefix() + ':' + localName();
        if (local_root)
            pset.insert(xmlNsPrefix());
    }
    gsc.os() << '<' << gi << dumpXmlNsMap(
        (gsc.flags() & Grove::GS_SAVE_DEFATTRS));
    for (Attr* a = attrs().firstChild(); a; a = a->nextSibling()) {
        if (!a->xmlNsPrefix().isEmpty() && local_root)
            pset.insert(a->xmlNsPrefix());
        if (a->defaulted() == Attr::DEFAULTED && 
            !(gsc.flags() & Grove::GS_SAVE_DEFATTRS))
                continue;
        gsc.os() << ' ';
        a->saveAsXml(gsc, child_flags);
    }
    if (local_root) {
        PrefixSet::const_iterator it = pset.begin();
        for (; it != pset.end(); ++it) {
            const Node* map_elem = this;
            const Node* lparent = parentNode(local_root);
            bool prefix_found = false;
            for (; map_elem && map_elem != lparent &&
                   map_elem->nodeType() == Node::ELEMENT_NODE; 
                   map_elem = map_elem->parent())
                if (!CONST_ELEMENT_CAST(map_elem)->
                    lookupPrefixMap(*it).isNull()) {
                        prefix_found = true;
                        break;
                }
            if (prefix_found)
                continue;
            String uri = local_root->getXmlNsByPrefix(*it);
            if (!uri.isNull())
                gsc.os() << XmlNsMapItem(*it, uri).asDeclString();
        }
    }   
    if (child_flags & (GSC::HAS_CONTENT|GSC::HAS_EREFS))
        gsc.os() << '>';
    else
        gsc.os() << "/>";
    if (can_indent_content && (child_flags & GSC::HAS_CONTENT)) 
        gsc.os() << CR_ENDL;
    if (child_flags & (GSC::HAS_CONTENT|GSC::HAS_EREFS)) {
        for (const Node* n = firstChild(); n; ) {
            n->saveAsXml(gsc, child_flags);
            if (n->nodeType() != ENTITY_REF_START_NODE)
                n = n->nextSibling();
            else
                n = CONST_ERS_CAST(n)->getSectEnd();
        }
        if (can_indent_content && (child_flags & GSC::HAS_CONTENT)) 
            gsc.printIndent(-1);
        gsc.os() << "</" << gi << '>';
    }
    if (can_indent_self) 
        gsc.os() << CR_ENDL;
    gsc.adjustIndent(-1);
}

Node* Element::copy(Node* newParent) const
{
    Element* e = new Element(name());
    e->assign(this, newParent);
    copyNsMappings(e);
    for (const Attr* a = attrs().firstChild(); a; a = a->nextSibling())
        e->attrs().appendChild(static_cast<Attr*>(a->cloneNode(true, e)));
    return e;
}

void Element::dumpInherited() const
{
    DBG_IF(GROVE.TEST) {
        dumpNs();
        if (attrs().firstChild()) {
            DDBG << "Attributes:" << std::endl;
            for (const Attr* a = attrs().firstChild(); a; a = a->nextSibling())
                dumpSubtree(a);
        }
    }
}

int Element::memSize() const
{
    int s = NodeWithNamespace::node_size() + sizeof(*this);
    for (const Attr* a = attrs().firstChild(); a; a = a->nextSibling())
        s += a->memSize();
    return s;
}

Element::~Element()
{
}

////////////////////////////////////////////////////////////////////////
    
void Comment::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << "<!--" << comment() << "-->";    
}

const String& Comment::nodeName() const
{
    static String n("#comment");
    return n;
}

int Comment::memSize() const
{
    return node_size() + sizeof(*this) + str_mem(comment_);
}

Node* Comment::copy(Node* newParent) const
{
    Comment* c = new Comment;
    c->assign(this, newParent);
    c->comment_ = comment_;
    return c;
}

void Comment::dumpInherited() const
{
    DDBG << "COMMENT:" << comment_ << std::endl;
}

//////////////////////////////////////////////////////////////////////

static const unsigned short NBSP_CHAR = 0xA0;

inline static bool is_space(const Char& c)
{
    return c.isSpace() && c.unicode() != NBSP_CHAR; // nbsp
}

void Text::saveAsXml(GroveSaverContext& gsc, int cFlags) const
{
    String stripped;
    bool strip_allowed = cFlags & GSC::STRIP_ALLOWED;
    stripped.reserve(data().length() + 1024);
    const Char* cp = data().unicode();
    const Char* ce = cp + data().length();
    const Node* n = prevSibling();
    if (strip_allowed && n && n->nodeType() == TEXT_NODE) {
        const Text* t = CONST_TEXT_CAST(n);
        const String& data = t->data();
        if (!data.isEmpty() && is_space(data[data.length() - 1]))
            while (cp < ce && is_space(*cp))
                ++cp;
    }
    while (cp < ce) {
        if (strip_allowed && is_space(*cp)) {
            stripped += ' ';
            while (cp < ce && is_space(*cp))
                ++cp;
            continue;
        }
        if (gsc.cdataLevel()) { 
            if (cp->unicode() != NBSP_CHAR)
                stripped += *cp;
            else
                stripped += "&#160;";
            ++cp;    
        } else {
            switch (cp->unicode()) {
                case '<':
                    stripped += "&lt;";   break;
                case '>':                 
                    stripped += "&gt;";   break;
                case '&':
                    stripped += "&amp;";  break;
                case '"':
                    stripped += "&quot;"; break;
                case '\'':
                    stripped += "&apos;"; break;
                case NBSP_CHAR: 
                    stripped += "&#160;"; break;
                case '%':
                    if (cFlags & GroveSaverContext::INT_ENTITY)
                        stripped += "&#37;";
                    else
                        stripped += *cp;
                    break;
                default:
                    stripped += *cp;
                    break;
            }
            ++cp;
        }
    }
    gsc.os() << stripped; 
}
    
bool Text::isEmpty() const
{
    const Char* cp = data().unicode();
    const Char* ce = cp + data().length();
    for (; cp < ce; ++cp)
        if (!is_space(*cp))
            return false;
    return true;
}

const String& Text::nodeName() const
{
    static String n("#text");
    return n;
}

Node* Text::copy(Node* newParent) const
{
    Text* t = new Text;
    t->assign(this, newParent);
    t->setData(data());
    return t;
}

int Text::memSize() const
{
    return node_size() + sizeof(*this) + str_mem(data_);
}

void Text::dumpInherited() const
{
    DDBG << "text value:<" << data() << '>' << std::endl;
}

//////////////////////////////////////////////////////////////////////
    
void Attr::saveAsXml(GroveSaverContext& gsc, int cFlags) const
{
    if (!xmlNsPrefix().isEmpty() && element())
        gsc.os() << xmlNsPrefix() << ':';
    gsc.os() << localName() << '=';
    String s;
    if (gsc.flags() & Grove::GS_EXPAND_ENTITIES) {
        const Char* cp = value().unicode();
        const Char* ce = cp + value().length();
        for (; cp < ce; ++cp) {
            if (*cp == '&')
                s += "&amp;";
            else
                s += *cp;
        }
        gsc.os() << quoteString(s, cFlags & GSC::ESC_ATTVAL); 
        return;
    }
    Dav::IoStream aos(s);
    for (const Node* n = firstChild(); n; ) {
        switch (n->nodeType()) {
            case Node::TEXT_NODE: {
                const Text* t  = CONST_TEXT_CAST(n);
                const Char* cp = t->data().unicode();
                const Char* ce = cp + t->data().length();
                for (; cp < ce; ++cp)
                    if (*cp == '&')
                        aos << "&amp;";
                    else if (*cp == '<')
                        aos << "&lt;";
                    else
                        aos << QChar(*cp);
                n = n->nextSibling();
                continue;
            }
            case Node::ENTITY_REF_START_NODE: {
                const EntityReferenceStart* ers = 
                    static_cast<const EntityReferenceStart*>(n);
                aos << "&" << ers->entityDecl()->name() << ';';
                n = ers->getSectEnd();
                continue;
            }
            default:
                n = n->nextSibling();
                continue;
        }
    }
    gsc.os() << quoteString(s, cFlags & GSC::ESC_ATTVAL); 
}

void Attr::notifyChange()
{
    Element* elem = element();
    if (elem)
        elem->notifyAttributeChanged(this);
}

void Attr::setValue(const String& v)
{
    setValueNoNotify(v);
    notifyChange();
}

void Attr::setValueNoNotify(const String& v)
{
    removeAllChildren();
    parsedValue_ = v;
    defaulted_   = SPECIFIED;
    type_        = CDATA;
    Text* t = new Text;
    t->setData(v);
    appendChild(t);
}

Node* Attr::copy(Node* newParent) const
{
    Attr* a = new Attr(name());
    if (newParent)
        a->gsr_ = newParent->getGSR();
    else
        a->gsr_ = 0;
    a->defaulted_   = defaulted_;
    a->type_        = type_;
    a->parsedValue_ = parsedValue_;
    a->idClass_     = idClass_;
    a->idSubClass_  = idSubClass_;
    a->isEnumerated_ = isEnumerated_;
    return a;
}

//
// builds parsed attribute value.
// TODO: entities support (ie possible children of Attr is not only
//       Text, but also EntityReferenceStart and EntityReferenceEnd).
//
void Attr::build()
{
    if (type_ != CDATA)
        return;
    parsedValue_.truncate(0);
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() == TEXT_NODE) {
            parsedValue_ += static_cast<Text*>(n)->data();
            if (tokenized() && n != firstChild())
                parsedValue_ += ' ';
        }
    }
    defaulted_ = SPECIFIED;
}

void Attr::dumpInherited() const
{
#ifndef _NDEBUG
    static const char* idClassName[] = {
        "NOT-ID", "ID", "IDREF", "IDREFS"
    };
    static const char* idSubClassName[] = {
        "", "<SCOPE-DEF>", "<SCOPED-ID>", "<SCOPED-IDREF>"
    };
    DBG_IF(GROVE.TEST) {
        int idc = idClass();
        if (idc > (int)IS_IDREFS)
            idc = 0;
        DDBG << "Attr type:" << static_cast<int>(type_)
             << " def:" << int(defaulted_)
             << " idClass: " << idClassName[idc]
             << idSubClassName[idSubClass_]
             << " value:" << abr(parsedValue_) << std::endl;
        dumpNs();
    }
#endif // _NDEBUG
}

int Attr::memSize() const
{
    return sizeof(*this) + NodeWithNamespace::node_size() + 
        str_mem(parsedValue_);
}

///////////////////////////////////////////////////////////////////////
    
void ProcessingInstruction::saveAsXml(GroveSaverContext& gsc, int) const
{
    if (target().isEmpty())
        return;
    gsc.os() << "<?" << target();
    if (!data().isEmpty())
        gsc.os() << ' ' << data();
    if (target()[0] != '?')
        gsc.os() << '?';
    gsc.os() << '>';
}

const String& ProcessingInstruction::nodeName() const
{
    static String n("#processing-instruction");
    return n;
}

Node* ProcessingInstruction::copy(Node* newParent) const
{
    ProcessingInstruction* pi = new ProcessingInstruction;
    pi->assign(this, newParent);
    pi->data_   = data_;
    pi->target_ = target_;
    pi->entityName_ = entityName_;
    return pi;
}

int ProcessingInstruction::memSize() const
{
    return node_size() + sizeof(*this) + str_mem(data_) + str_mem(target_)
        + str_mem(entityName_);
}

void ProcessingInstruction::dumpInherited() const
{
    DDBG << "PI: target <" << target() << "> ent <" << entityName()
        << "> value:<" << data() << '>' << std::endl;
}

//////////////////////////////////////////////////////////////////////////

bool EntityReferenceStart::saveAsXmlFile(int flags, 
                                         const StripInfo* si,
                                         const Common::String& saveAs)
{
    flags &= ~(Grove::GS_SAVE_PROLOG);
    if (entityDecl()->declType() != EntityDecl::externalGeneralEntity)
        return false;
    GroveSaverContext gsc(si, grove(), flags);
    if (!gsc.setOutputUrl(saveAs,
        entityDecl()->asConstExternalEntityDecl()->encoding()))
            return false;
    saveAsXml(gsc, flags|GroveSaverContext::SAVE_ECONTENT);
    return true;
}

bool EntityReferenceStart::saveAsXmlString(Common::String& saveTo,
                                          int flags, 
                                          const StripInfo* si)
{
    flags &= ~(Grove::GS_SAVE_PROLOG|Grove::GS_SAVE_ENTITIES);
    GroveSaverContext gsc(si, grove(), flags);
    gsc.setOutputString();
    if (entityDecl()->declType() == EntityDecl::internalGeneralEntity)
        flags |= GroveSaverContext::INT_ENTITY;
    saveAsXml(gsc, flags|GroveSaverContext::SAVE_ECONTENT);
    saveTo = gsc.getSavedString();
    return true;
}

void EntityReferenceStart::saveAsXml(GroveSaverContext& gsc, int cFlags) const
{
    if (entityDecl()->declType() == EntityDecl::internalParameterEntity ||
        entityDecl()->declType() == EntityDecl::externalParameterEntity) {
            gsc.os() << '%' << entityDecl()->name() << ';';
            return;
    }
    if (entityDecl()->declType() == EntityDecl::xinclude &&
        !(gsc.flags() & Grove::GS_EXPAND_ENTITIES)) {
        const XincludeDecl* xd = static_cast<const XincludeDecl*>(entityDecl());
        DBG(GROVE.SAVE) << "Saving xinclude: " << xd->url() << std::endl;
        if (0 == xd->referenceErs() || xd->isFallback()) {
            DBG(GROVE.SAVE) << "Saving xinclude: no reference ERS!\n";
            if (0 == firstChild() || firstChild()->nodeType() != ELEMENT_NODE) {
                DBG(GROVE.SAVE) << "Saving xinclude: no first child?\n";
                return; // no xinclude element
            }
            xinclude_fixup_ref(this);
            DBG(GROVE.SAVE) << "xinclude save: savng first child (1)\n";
            firstChild()->saveAsXml(gsc, cFlags);
            return;
        }
        if (xd->referenceErs() != this) {
            DBG(GROVE.SAVE) << "refers not mine: gsc.grove=" << gsc.grove()
                << ", grove=" << grove() << ", refers->grove=" 
                << xd->referenceErs()->grove() << std::endl;
            if (xd->referenceErs() && gsc.grove() != grove() && 
                xd->referenceErs()->grove() == gsc.grove())
                    return xd->referenceErs()->saveAsXml(gsc, cFlags);
            // source (parent) grove
            if (0 == firstChild() || firstChild()->nodeType() != ELEMENT_NODE)
                return; // no xinclude element
            xinclude_fixup_ref(this);
            DBG(GROVE.SAVE) << "xinclude save: savng first child (2)\n";
            firstChild()->saveAsXml(gsc, cFlags);
            return;
        } 
        cFlags |= GSC::SAVE_ECONTENT;
    }
    if ((gsc.flags() & Grove::GS_EXPAND_ENTITIES) ||
        (cFlags & GSC::SAVE_ECONTENT)) {
            cFlags &= ~GSC::SAVE_ECONTENT;
            // inclusion
            if (grove()->parent() && 
                entityDecl()->declType() == EntityDecl::xinclude) {
                DBG(GROVE.SAVE) << "Saving xinclude (3)\n";
                DBG_IF(GROVE.SAVE) {
                    Grove* pg = grove()->root();
                    dumpSubtree(pg->document());
                    for (pg = pg->firstChild(); pg; pg = pg->nextSibling()) {
                        DBG(GROVE.SAVE) << pg->topSysid() 
                            << ": -------------------------\n";
                        dumpSubtree(pg->document());
                    }
                    DBG(GROVE.SAVE) << "-------------------------\n";
                }
                const XincludeDecl* xd = 
                    static_cast<const XincludeDecl*>(entityDecl());
                const EntityReferenceTable::ErtEntry* ee = 
                    grove()->root()->document()->ert()->lookup(entityDecl());
                if (0 == ee)
                    ee = xd->parentGrove()->document()->ert()->
                        lookup(entityDecl());
                if (ee) {
                    DBG(GROVE.SAVE) << "Found reference content, pg="
                        << xd->parentGrove()->topSysid() << std::endl;
                    const Node* n = ee->node(0)->nextSibling();
                    while (n && n != ee->node(0)->getSectEnd()) {
                        if (n->nodeType() == Node::ELEMENT_NODE) {
                            const Element* lroot = gsc.localRoot();
                            gsc.setLocalRoot(CONST_ELEMENT_CAST(n));
                            n->saveAsXml(gsc, cFlags);
                            DBG_IF(GROVE.SAVE) dumpSubtree(n);
                            gsc.setLocalRoot(lroot);
                        } else
                            n->saveAsXml(gsc, cFlags);
                        if (n->nodeType() == ENTITY_REF_START_NODE) 
                            n = CONST_ERS_CAST(n)->getSectEnd();
                        else
                            n = n->nextSibling();
                    }
                    return;
                }
            }
            const Node* n = nextSibling();
            if (!(gsc.flags() & Grove::GS_EXPAND_ENTITIES))
                gsc.xmlHeader(entityDecl());
            while (n && n != getSectEnd()) {
                n->saveAsXml(gsc, cFlags);
                if (n->nodeType() == ENTITY_REF_START_NODE) 
                    n = CONST_ERS_CAST(n)->getSectEnd();
                else
                    n = n->nextSibling();
            }                    
            return;
    }
    gsc.os() << '&' << entityDecl()->name() << ';';
}

Grove* EntityReferenceStart::currentGrove() const
{
    const EntityReferenceStart* cur_ers = this;
    for (; cur_ers; cur_ers = CONST_ERS_CAST(cur_ers->getSectParent())) {
        if (cur_ers->entityDecl()->declType() != EntityDecl::xinclude) 
            continue;
        const XincludeDecl& xd =
            static_cast<const XincludeDecl&>(*cur_ers->entityDecl());
        if (xd.isFallback())
            continue;
        return xd.referenceErs()->grove();
    }
    return grove();
}

// fn = absolute - return fn
// fn = relative - return absolute path2path to fn
// fn = empty    - return absolute path to current entity
String EntityReferenceStart::xmlBase(const Common::String& fn) const
{
    Url fn_url(fn);
    if (!fn_url.isRelative())
        return fn;
    const EntityReferenceStart* ers = this;
    String sysid;
    for (; ers; ers = CONST_ERS_CAST(ers->getSectParent())) {
        const EntityDecl* ed = ers->entityDecl();
        if (ed->declType() == EntityDecl::externalGeneralEntity ||
            ed->declType() == EntityDecl::document) {
                sysid = static_cast<const ExternalEntityDecl*>(ed)->
                    entityPath(ers->currentGrove());
                break;
        }
        if (ed->declType() == EntityDecl::xinclude) {
            const XincludeDecl* xd = static_cast<const XincludeDecl*>(ed);
            if (xd->referenceErs())
                sysid = xd->referenceErs()->grove()->topSysid();
            else
                sysid = xd->url();
            break;    
        }
    }
    return Url(sysid).combinePath2Path(fn_url);
}
    
EntityReferenceStart::EntityReferenceStart()
    : GroveSectionStart(ENTITY_REF_START_NODE)
{
}

const String& EntityReferenceStart::nodeName() const
{
    static String n("#ers-start");
    return n;
}

int EntityReferenceStart::memSize() const
{
    return node_size() + sizeof(*this);
}

void EntityReferenceStart::dumpInherited() const
{
    DBG_IF(GROVE.TEST) {
        if (!parent() && !erTable_.isNull())
            erTable_->dump();
        DDBG << "EntityRefStart (" << this << "): entity:"
             << (entityDecl() ? entityDecl()->name().utf8() : "<no entity>")
             << std::endl;
        dumpSectionInfo();
    }
}

bool EntityReferenceStart::hasLoop(const String& name) const
{
    const EntityReferenceStart* ers = this;
    while (ers) {
        const EntityDecl* ed = ers->entityDecl();
        if (ed && ed->name() == name)
            return true;
        ers = static_cast<const EntityReferenceStart*>(ers->getSectParent());
    }
    return false;
}

void EntityReferenceStart::copy_sect_start(const GroveSectionStart* other,
                                           Node* newParent)
{
    const EntityReferenceStart* n = 
        static_cast<const EntityReferenceStart*>(other);
    erTable_ = newParent->getGSR()->ert();
    decl_    = n->decl_;
    ert()->addEntityRef(this);
}

void EntityReferenceStart::detachSection()
{
    if (!entityDecl())
        return;
    if (getGSR() && getGSR()->ert())
        getGSR()->ert()->removeEntityRef(this);
}

void EntityReferenceStart::attachSection()
{
    GroveSectionRoot* const gsr = getGSR();
    if (gsr && gsr->ert()) {
        erTable_ = gsr->ert();
        gsr->ert()->addEntityRef(this);
    }
}

void EntityReferenceStart::processTakeAsFragment(GroveSectionStart* base)
{
    ert()->initErt(this);
    static_cast<EntityReferenceStart*>(base)->ert()->mergeRemove(*ert());
}

void EntityReferenceStart::processInsertFragment(GroveSectionStart* base)
{
    static_cast<EntityReferenceStart*>(base)->ert()->mergeAdd(*ert());
}

void EntityReferenceStart::initTables()
{
    erTable_ = new EntityReferenceTable;
}

EntityDeclSet* EntityReferenceStart::entityDecls() const
{
    const EntityReferenceStart* ers = this;
    for (; ers; ers = CONST_ERS_CAST(ers->getSectParent())) {
        if (ers->entityDecl()->declType() == EntityDecl::xinclude) {
            return static_cast<const XincludeDecl*>(ers->entityDecl())->
                referenceErs()->grove()->entityDecls();
        }
    }
    return grove()->entityDecls();
}

EntityReferenceStart::~EntityReferenceStart()
{
    detachSection();
}

const String& EntityReferenceEnd::nodeName() const
{
    static String n("#ers-end");
    return n;
}

int EntityReferenceEnd::memSize() const
{
    return node_size() + sizeof(*this);
}

void EntityReferenceEnd::dumpInherited() const
{
    DBG_IF(GROVE.TEST) {
        DDBG << "EntityRefEnd:" << std::endl;
        dumpSectionInfo();
    }
}

////////////////////////////////////////////////////////////////////////

void MarkedSectionStart::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << "<![";
    if (!entityDecl())
        gsc.os() << typeName();
    else
        gsc.os() << '%' << entityDecl()->name() << ';';
    gsc.os() << '[';
    if (type() == cdata)
        gsc.cdataLevel(1);
    if (type() == ignore)
        gsc.os() << ignoredChars();
}

const String& MarkedSectionStart::nodeName() const
{
    static String n("#mss-start");
    return n;
}

String MarkedSectionStart::typeName() const
{
    const char *tn[] = {
        "TEMP", "INCLUDE", "RCDATA", "CDATA", "IGNORE", "ENTITYREF"
    };
    return tn[unsigned(type_)];
}

void MarkedSectionStart::dumpInherited() const
{
    DBG_IF(GROVE.TEST) {
        DDBG << "MarkedSectionStart: type:" << int(type_)
             << " stat:" << int(status_);
        if (static_cast<int>(type_) == entityRef) {
            DDBG << " EntR:" << (entity_.isNull() ? "<NONE?>" : entity_->name())
                 << std::endl;
        } else
            DDBG << std::endl;
        dumpSectionInfo();
    }
}

void MarkedSectionStart::copy_sect_start(const GroveSectionStart* other,
                                         Node*)
{
    const MarkedSectionStart* n = static_cast<const MarkedSectionStart*>(other);
    type_   = n->type_;
    status_ = n->status_;
    entity_ = n->entity_;
}

int MarkedSectionStart::memSize() const
{
    return node_size() + sizeof(*this) + str_mem(ignoredChars_);
}

void MarkedSectionEnd::saveAsXml(GroveSaverContext& gsc, int) const
{
    const MarkedSectionStart* mss = CONST_MSS_CAST(getSectStart());
    if (mss->type() == MarkedSectionStart::cdata)
        gsc.cdataLevel(-1);
    gsc.os() << "]]>";
}

const String& MarkedSectionEnd::nodeName() const
{
    static String n("#mse-end");
    return n;
}

int MarkedSectionEnd::memSize() const
{
    return node_size() + sizeof(*this);
}

void MarkedSectionEnd::dumpInherited() const
{
    DBG_IF(GROVE.TEST) {
        DDBG << "MarkedSectionEnd:" << std::endl;
        dumpSectionInfo();
    }
}

/////////////////////////////////////////////////////////////////////////

Node* ChoiceNode::copy(Node* newParent) const
{
    ChoiceNode* n = new ChoiceNode;
    n->assign(this, newParent);
    const PropertyNode* pn = elemList_.root()->firstChild();
    for (; pn; pn = pn->nextSibling())
        n->elemList_.root()->appendChild(pn->copy(true));
    return n;
}

void ChoiceNode::setElemList(Common::PropertyNode* pt)
{
    elemList_.setRoot(pt);
}

void ChoiceNode::saveAsXml(GroveSaverContext&, int) const
{
}

void ChoiceNode::dumpInherited() const
{
    DDBG << "Choice Elements: ";
    DBG_IF(GROVE.TEST) elemList_.root()->dump();
}

////////////////////////////////////////////////////////////////////////

Node* TextChoiceNode::copy(Node* newParent) const
{
    TextChoiceNode* n = new TextChoiceNode((Type)type_);
    n->assign(this, newParent);
    n->setData(data());
    return n;
}

const String& TextChoiceNode::nodeName() const
{
    static String simple_choice_nodename("#choice-simple");
    static String enum_choice_nodename("#choice-enum");
    return (static_cast<int>(type_) == SIMPLE_CHOICE)
        ? simple_choice_nodename 
        : enum_choice_nodename;
}

GROVE_EXPIMP bool is_text_choice(const Text* t)
{
    if (t->nodeName()[1] == 't')
        return false;
    return static_cast<const TextChoiceNode*>(t)->isChoice();            
}

GROVE_EXPIMP void set_text_choice(Text* t, bool v)
{
    if (t->nodeName()[1] != 't')
        static_cast<TextChoiceNode*>(t)->setChoice(v);            
}

////////////////////////////////////////////////////////////////////////

Common::XTreeNodeCmpResult Node::comparePos(const Node* otherNode) const
{
    if (nodeType() == ATTRIBUTE_NODE) {
        if (otherNode->nodeType() == ATTRIBUTE_NODE)
            return static_cast<const Attr*>(this)->
                element()->XTN::comparePos(static_cast<const Attr*>
                    (otherNode)->element());
        return static_cast<const Attr*>(this)->
            element()->XTN::comparePos(otherNode);
    }
    if (otherNode->nodeType() == ATTRIBUTE_NODE)
        return XTN::comparePos(static_cast<const Attr*>(otherNode)->element());
    return XTN::comparePos(otherNode);
}

GROVE_NAMESPACE_END
