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
#include "common/String.h"
#include "common/Url.h"
#include "common/Message.h"

#include "grove/grove_trace.h"
#include "grove/EntityDecl.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"
#include "grove/EntityDeclSet.h"

#include "stdio.h"

using namespace Common;

namespace GroveLib {

EntityDecl::EntityDecl()
    : declNode_(0), declType_(invalidDeclType), dataType_(invalidDataType),
      declOrigin_(invalidDeclOrigin),
      declModified_(0), contentModified_(0), readOnly_(0)
{
}

const String& EntityDecl::originalDecl() const
{
    return originalDecl_;
}

void EntityDecl::setOriginalDecl(const String& odecl)
{
    originalDecl_ = odecl;
}

const String& EntityDecl::comment() const
{
    return comment_;
}

void EntityDecl::setComment(const String& cs)
{
    comment_ = cs;
}

EntityDecl::~EntityDecl()
{
}

void EntityDeclExt::assignExt(const EntityDeclExt& ext)
{
    extid_ = ext.extid_;
}

/*! EntityDecl assignment operator, which allows to assign EntityDecl's
 *  by value.
 */
EntityDecl& EntityDecl::operator=(const EntityDecl& d)
{
    declType_   = d.declType_;
    dataType_   = d.dataType_;
    declOrigin_ = d.declOrigin_;
    origin_     = d.origin_;
    declModified_ = d.declModified_;
    contentModified_ = d.contentModified_;
    NamedBase::operator=(d.name());
    return *this;
}

/*! Deep copy of EntityDecl. This function is required because many
    classes inherit from base EntityDecl.
 */
EntityDecl* EntityDecl::copy() const
{
    EntityDecl* d = new EntityDecl;
    *d = *this;
    return d;
}

/*! Deep-copy implementation for Notation.
 */
EntityDecl* Notation::copy() const
{
    Notation* d = new Notation;
    d->EntityDecl::operator=(*this);
    d->assignExt(*this);
    return d;
}

/*! Deep-copy implementation for ExternalEntityDecl.
 */
EntityDecl* ExternalEntityDecl::copy() const
{
    ExternalEntityDecl* d = new ExternalEntityDecl;
    d->EntityDecl::operator=(*this);
    d->assignExt(*this);
    d->notationName_ = notationName_;
    d->encoding_ = encoding_;
    return d;
}

String EntityDecl::asAnnotationString() const
{
    return "&" + name();
}

#if defined(_WIN32)
# define snprintf _snprintf
#endif

DocumentFragment*
ExternalEntityDecl::parse(const EntityReferenceStart* refErs,
                          bool* isReadonly) const
{
    Vector<String> flist;
    String start_tag("<ENTITY-TOP-ELEMENT>");
    String end_tag("</ENTITY-TOP-ELEMENT>");
    String path = refErs->xmlBase(sysid());

    char buf[32];
    int len = snprintf(&buf[0], sizeof(buf), "mem://%p,%x",
                       &start_tag, sizeof(Char) * start_tag.size());
    flist.push_back(from_latin1(&buf[0], len));

    flist.push_back(path);

    len = snprintf(&buf[0], sizeof(buf), "mem://%p,%x",
                   &end_tag, sizeof(Char) * end_tag.size());
    flist.push_back(from_latin1(&buf[0], len));

    GroveBuilder* gb = refErs->grove()->groveBuilder()->copy();
    gb->setEncoding(Encodings::XML);
    GrovePtr entity_grove = gb->buildGrove(flist, path);
    if (entity_grove.isNull())
        return 0;
    if (isReadonly)
        *isReadonly = entity_grove->topDecl()->isReadOnly();
    Node* top_element = entity_grove->document()->documentElement();
    if (!top_element)
        return 0;
    Node* first_child = top_element->firstChild();
    const_cast<ExternalEntityDecl*>(this)->setEncoding
        (entity_grove->topDecl()->encoding());
    if (first_child) 
        return first_child->takeAsFragment(top_element->lastChild());
    return 0;
}

DocumentFragment*
InternalEntityDecl::parse(const EntityReferenceStart* refErs,
                          bool* isReadonly) const
{
    if (isReadonly)
        *isReadonly = false;
    // in SD entities, there is no escaping, so we use them directly
    if (dataType() != EntityDecl::sgml || EntityDecl::sd == declOrigin()) {
        Text* t = new Text;
        t->setData(content());
        DocumentFragment* df = new DocumentFragment;
        df->appendChild(t);
        return df;
    }
    String is = "<ENTITY-TOP-ELEMENT>";
    is += content();
    is += "</ENTITY-TOP-ELEMENT>";
    GrovePtr entity_grove =
        refErs->grove()->groveBuilder()->copy()->buildGrove(is, 
            refErs->xmlBase());
   if (entity_grove.isNull())
       return 0;
    Node* top_element = entity_grove->document()->documentElement();
    if (!top_element || !top_element->firstChild())
        return 0;
    return top_element->firstChild()->takeAsFragment(top_element->lastChild());
}

/*! Deep-copy implementation for InternalEntityDecl.
 */
EntityDecl* InternalEntityDecl::copy() const
{
    InternalEntityDecl* d = new InternalEntityDecl;
    d->EntityDecl::operator=(*this);
    d->content_ = content_;
    return d;
}

void InternalEntityDecl::rebuildContent(Grove* grove)
{
    const EntityDecl* ed = grove->entityDecls()->lookupDecl(name());
    if (0 == ed) {
        setContentModified(false);
        return;
    }
    const EntityReferenceTable::ErtEntry* ee = 0;
    if (grove->parent())  // this is a included subgrove
        ee = grove->parent()->document()->ert()->lookup(ed);
    else
        ee = grove->document()->ert()->lookup(ed);
    if (0 == ee) {
        setContentModified(false);
        return;
    }
    content_.truncate(0);
    ee->node(0)->saveAsXmlString(content_, Grove::GS_SAVE_CONTENT|
        Grove::GS_FORCE_SAVE|Grove::GS_DONTSAVEDEFENC, 0);
}

String ExternalEntityDecl::entityPath(const Grove* grove) const
{
    Vector<const ExternalEntityDecl*> entityPath;
    entityPath.reserve(128);
    const EntityDecl* origin = originEntityDecl();
    entityPath.push_back(this);
    while (origin) {
        const ExternalEntityDecl* ext_origin =
            origin->asConstExternalEntityDecl();
        if (ext_origin)
            entityPath.push_back(ext_origin);
        origin = origin->originEntityDecl();
    }
    Url path(grove->topSysid());
    for (int i = int(entityPath.size()) - 1; i >= 0; --i)
        path = path.combinePath2Path(entityPath[i]->sysid());
    return path;
}

////////////////////////////////////////////////////////////////////
//
// The following functions are used for dumping entity definitions
// in XML/SGML prolog format. Some debug dumps is also here.
//
////////////////////////////////////////////////////////////////////

void EntityDeclExt::add_specific_decl(String& s) const
{
    if (!sysid().isEmpty()) {
        s += " SYSTEM \"";
        s += sysid();
        s += '"';
    }
    if (!pubid().pubid().isEmpty()) {
        s += " PUBLIC \"";
        s += pubid().pubid();
        s += '"';
    }
}

Common::String EntityDecl::asDeclString() const
{
    return String();
}

Common::String Notation::asDeclString() const
{
    String s;
    if (declOrigin_ == special)
        return s;
    s = "<!NOTATION ";
    s += name();
    add_specific_decl(s);
    s += comment_;
    s += '>';
    return s;
}

Common::String InternalEntityDecl::asDeclString() const
{
    String s;
    if (entity_prolog(s))
        return s;
    s += " '";
    s += content();
    s += '\'';
    s += comment_;
    s += '>';
    return s;
}

Common::String ExternalEntityDecl::asDeclString() const
{
    String s;
    if (entity_prolog(s))
        return s;
    add_specific_decl(s);
    if (!notationName_.isEmpty()) {
        s += " NDATA ";
        s += notationName_;
    }
    s += comment_;
    s += '>';
    return s;
}

bool EntityDecl::entity_prolog(String& s) const
{
    if (declOrigin_ == special) {
        s.truncate(0);
        return true;
    }
    s = "<!ENTITY ";
    if (isParameterEntity())
        s += "% ";
    s += name();
    return false;
}
    
bool EntityDecl::isSingleCharEntity() const
{
    if (declType() != internalGeneralEntity)
        return false;
    if (declOrigin() != dtd && declOrigin() != sd)
        return false;
    if (name() == "lt" || name() == "amp" || name() == "gt")
        return true;
    return 1 == static_cast<const GroveLib::InternalEntityDecl*>
        (this)->content().length(); // single char, declared in dtd
}

#ifdef GROVE_DEBUG
void EntityDecl::dump() const
{
    using std::endl;
    DDINDENT;

    DDBG << "entity_decl:<" << name() << "> decl_type:" << int(declType_)
         << " data_type:" << int(dataType_) << endl;

    {
        DINDENT(12);
        DDBG << "origin:" << origin_.pointer()
             << " decl_modified:" << (declModified_ ? 'y' : 'n')
             << " cont_modified:" << (contentModified_ ? 'y' : 'n') << endl;

        DDBG << "declOrigin:" << declOrigin_ << endl;

        const InternalEntityDecl* ie = asConstInternalEntityDecl();
        if (ie)
            DDBG << "internal:<" << ie->content() << '>' << endl;

        const ExternalEntityDecl* ee = asConstExternalEntityDecl();
        if (ee)
            DDBG << "external sysid:<" << ee->sysid()
                 << "> pubid:<" << ee->pubid().pubid() << '>'
                 << " encoding: " << Encodings::encodingName(ee->encoding())
                 << endl;
    }
    DDBG << "OriginalDecl: " << originalDecl() << endl;
}
#else
void EntityDecl::dump() const
{
}
#endif // GROVE_DEBUG

EntityDeclExt::~EntityDeclExt()
{
}

Notation::~Notation()
{
}

InternalEntityDecl::~InternalEntityDecl()
{
}

ExternalEntityDecl::ExternalEntityDecl()
    : encoding_(Encodings::XML)
{
}

ExternalEntityDecl::~ExternalEntityDecl()
{
}

XincludeDecl::XincludeDecl()
    : refErs_(0), parentGrove_(0), isFallback_(false)
{
    declType_ = xinclude;
    dataType_ = sgml;
    setName("XI:" + String::number((intptr_t)this));
}

String XincludeDecl::asAnnotationString() const
{
    String annotation = Url(url())[Url::FILENAME];
    if (!expr().isEmpty())
        annotation += "#" + expr();
    return annotation;
}

XincludeDecl::~XincludeDecl()
{
}

//
// Pseudo-RTTI stubs and definitions.
//
PRTTI_BASE_STUB(EntityDecl, Notation)
PRTTI_BASE_STUB(EntityDecl, InternalEntityDecl)
PRTTI_BASE_STUB(EntityDecl, ExternalEntityDecl)
PRTTI_BASE_STUB(EntityDecl, XincludeDecl)
PRTTI_IMPL(Notation)
PRTTI_IMPL(InternalEntityDecl)
PRTTI_IMPL(ExternalEntityDecl)
PRTTI_IMPL(XincludeDecl)

} // namespace GroveLib
