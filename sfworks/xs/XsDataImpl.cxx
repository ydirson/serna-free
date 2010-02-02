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
#include "xs/xs_defs.h"
#include "xs/XsDataImpl.h"
#include "xs/SchemaNamespaces.h"
#include "xs/Component.h"
#include "xs/ComponentRef.h"
#include "xs/ComponentSpace.h"
#include "xs/ImportMap.h"
#include "xs/XsMessages.h"
#include "xs/parser/SchemaParser.h"
#include "xs/ExtPythonScript.h"
#include "common/ThreadingPolicies.h"
#include "common/Url.h"
#include "complex/Particle.h"
#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include <map>
#include <set>

using namespace std;

USING_COMMON_NS
USING_GROVE_NAMESPACE
XS_NAMESPACE_BEGIN

class SubstGroupMap : public multimap<GROVE_NAMESPACE::ExpandedName,
                                      XsElementImpl*> {};

void XsDataImpl::insertSubstElement(GROVE_NAMESPACE::ExpandedName name,
                                    XsElementImpl* elem)
{
    substMap_->insert(SubstGroupMap::value_type(name, elem));
}

typedef Vector<XsElementImpl*> XsElemVec;
typedef set<const XsElementImpl*> VisitedSet;
typedef multimap<GROVE_NAMESPACE::ExpandedName,
                 XsElementImpl*> SubstMap;

void do_subst_group(const ExpandedName& name, XsElemVec& grp, 
                    SubstMap& smap, VisitedSet& vs)
{
    typedef SubstMap::iterator MI;
    pair<MI, MI> g = smap.equal_range(name);
    for (MI p = g.first; p != g.second; ++p) {
        if (vs.find(p->second) != vs.end())
            continue;
        vs.insert(p->second);
        grp.push_back(p->second);
        ExpandedName ename;
        if (p->second->substGroup(ename))
            do_subst_group(ename, grp, smap, vs); 
    }
}

void XsDataImpl::substGroup(GROVE_NAMESPACE::ExpandedName name,
                            Vector<XsElementImpl*>& grp)
{
    VisitedSet vs;
    do_subst_group(name, grp, *substMap_, vs);
}

static void join_subst_groups(Schema* mySchema)
{
    Vector<Schema*> ischemas;
    mySchema->xsi()->importMap()->importedSchemas(ischemas);
    for (uint i = 0; i < ischemas.size(); ++i) {
        Schema* s = ischemas[i];
        if (s->xsi() == mySchema->xsi())
            continue;
        s->parse2();
        mySchema->xsi()->joinSubstMap(s->xsi());
    }
}

void XsDataImpl::joinSubstMap(XsDataImpl* other)
{
    SubstGroupMap::iterator it = other->substMap_->begin();
    for (; it != other->substMap_->end(); ++it)
        substMap_->insert(SubstGroupMap::value_type(it->first, it->second));
}

void XsDataImpl::parseSchema(GROVE_NAMESPACE::Element* root, Schema* s)
{
    if (isParsed_) {
        s->mstream() << XsMessages::schemaAlreadyParsed << Message::L_WARNING;
        return;
    }
    if (0 == root) {
        s->mstream() << XsMessages::noSchema << Message::L_ERROR;
        return;
    }
    schemaNs_    = W3C_SCHEMA_NAMESPACE;
    if (!isSchemaNode(root)) {
        s->mstream() << XsMessages::noW3Cns  << Message::L_WARNING
            << schemaNs_;
        return;
    }
    schemaRoot_  = root;
    schemaGrove_ = root->grove();
    schemaExtNs_ = SCHEMA_EXTENSIONS_NAMESPACE;
    antlr::RefAST ast(new GroveAst(root, s));
    try {
        SchemaParser parser(s);
        parser.xmlSchema(ast);
    }
    catch (exception&) {
        s->mstream() << XsMessages::unknownParserException << Message::L_FATAL;
    }
    isParsed_ = true;

    // Process redefine for all component spaces
    for (int i = Component::type; i < (int)Component::MAX; ++i)
        getOspaceByType((Component::ComponentType)i)->processRedefine(s);
    
}

void XsDataImpl::parse2(Schema* s)
{
    if (substParsed_)
        return;
    substParsed_ = true;
    importMap()->loadAll();
    join_subst_groups(s);
#if 0
    GROVE_NAMESPACE::ExpandedName new_grp;
    typedef SubstGroupMap::iterator MI;
    MI it = substMap_->begin();
    bool skip = true;
    for(; it != substMap_->end(); ++it) {
        if (!(new_grp == it->first)) {
            if (!skip) {
                if (Component* comp = elementSpace().
                       lookupBase(it->first.localName(), it->first.uri())) {
                    XsElementImpl* elem = static_cast<XsElementImpl*>(comp);
                    substMap_->insert(SubstGroupMap::value_type
                        (it->first, elem));
                    elem->substitutionGroup_ = it->first;
                }
            }
            skip = false;
            new_grp = it->first;
        }
        if (it->second->constCred()->expandedName() == it->first)
            skip = true;
    }
#endif // 
}

ExtPythonScript* XsDataImpl::pyScript()
{
    return script_;
}

void XsDataImpl::init(Schema *s)
{
    // set target namespace for all ospaces
    for (ulong i = 1; i < Component::MAX; ++i) {
        ComponentSpaceBase* csb =
            getOspaceByType((Component::ComponentType)i);
        if (0 == csb)
            continue;
        csb->setLns(targetNs_);
    }
    importMap_->appendMySchema(W3C_SCHEMA_NAMESPACE, s);
    importMap_->appendMySchema(SCHEMA_EXTENSIONS_NAMESPACE, s);
    importMap_->appendMySchema(targetNs_, s);
    initAtomicTypes();
}

bool XsDataImpl::isSchemaNode(const GROVE_NAMESPACE::NodeWithNamespace* n) const
{
    if (n->nodeType() == GroveLib::Node::ATTRIBUTE_NODE &&
        n->xmlNsUri().isEmpty() && n->xmlElNsUri() == schemaNs_)
            return true;
    return (n->xmlNsUri() == schemaNs_);
}

bool
XsDataImpl::isSchemaExtNode(const GROVE_NAMESPACE::NodeWithNamespace* n) const
{
    if (n->nodeType() == GroveLib::Node::ATTRIBUTE_NODE &&
        n->xmlNsUri().isEmpty() && n->xmlElNsUri() == schemaExtNs_)
            return true;
    return (n->xmlNsUri() == schemaExtNs_);
}

const String& XsDataImpl::targetNsUri() const
{
    return targetNs_;
}

NcnCred XsDataImpl::makeCred(const String& name, bool pvt) const
{
    if (pvt)
        return NcnCred(name, "pns" + String::number((intptr_t)this));
    return NcnCred(name, targetNs_);
}

static bool check_within_redef(const GroveLib::Element* elem)
{
    while (parentNode(elem)) {
        if (elem->localName() == "redefine")
            return true;
        elem = ELEMENT_CAST(parentNode(elem));
    }
    return false;
}

ComponentRefBase* XsDataImpl::makeRefBase(Schema* s,
                                          GROVE_NAMESPACE::Element* elem,
                                          const String& name,
                                          Component::ComponentType cid)
{
    bool withinRedef = check_within_redef(elem);
    GROVE_NAMESPACE::QualifiedName qname;
    String xns;
    qname.parse(name);
    if (qname.isQualified()) {
        xns = elem->getXmlNsByPrefix(qname.prefix());
        if (xns.isNull()) {
            s->mstream() << XsMessages::undeclaredSchemaPrefix
                << Message::L_ERROR << qname.prefix() << XSN_ORIGIN(elem);
        }
    }
    else
        xns = elem->getXmlNsByPrefix(GROVE_NAMESPACE::XmlNs::defaultNs());
    if ((schemaNs_ == xns) || (targetNs_ == xns))
        return new ComponentRefBase(s, qname.localName(), xns, cid,
            withinRedef);
    if (!importMap_->isMapped(xns)) {
        s->mstream() << XsMessages::namespaceNotImported  << Message::L_ERROR
           << xns << XSN_ORIGIN(elem);
    }
    return new ComponentRefBase(s, qname.localName(), xns, cid, withinRedef);
}

bool XsDataImpl::putPieceIntoTable(Piece* p)
{
    if (pieceIdt_.end() == pieceIdt_.find(p->id()))
        return false;
    pieceIdt_.insert(p);
    return true;
}

bool XsDataImpl::putComponentIntoTable(Component* c)
{
    if (componentIdt_.end() == componentIdt_.find(c->id()))
        return false;
    componentIdt_.insert(c);
    return true;
}

XsDataImpl::XsDataImpl(Schema *s)
    : isParsed_(false), substParsed_(false)
{
    typeSpace_      = new TypeSpace;
    attributeSpace_ = new AttributeSpace;
    groupSpace_     = new GroupSpace;
    attributeGroupSpace_ = new AttributeGroupSpace;
    elementSpace_   = new ElementSpace;
    notationSpace_  = new NotationSpace;
    identityConstraintSpace_ = new IdentityConstraintSpace;
    importMap_      = new ImportMap(s);
#ifdef USE_PYTHON
    script_ = ExtPythonScript::make();
#endif
    substMap_ = new SubstGroupMap;
}

void XsDataImpl::resolveComponents()
{
    Lock(*this);
    for (ulong i = 0; i < references_.size(); ++i)
        references_[i]->resolve();
    references_.clear();
}

Schema* XsDataImpl::importedSchema(const String& uri)
{
    return importMap_->ischema(uri);
}
// The following is a big kludge. Normally, inclusion should be done
// on _component_ level (schema must be parsed first, then the tables
// must be merged). 
static void set_form_attrs(GroveLib::Element* e,
                           const String& elemDefault,
                           const String& attrDefault)
{
    String localName = e->localName();
    if (localName == "element" || localName == "attribute") {
        GroveLib::Attr* a = e->attrs().getAttribute("form");
        if (0 == a && !e->attrs().getAttribute("ref")) {
            a = new Attr("form");
            a->setValue(localName == "element" ? elemDefault : attrDefault);
            e->attrs().appendChild(a);
        }
    }
    GroveLib::Node* n = e->firstChild();
    for (; n; n = n->nextSibling()) 
        if (n->nodeType() == GroveLib::Node::ELEMENT_NODE)
            set_form_attrs(ELEMENT_CAST(n), elemDefault, attrDefault);
        
}

bool XsDataImpl::processInclude(const GroveLib::Element* originElem,
                                Schema* schema)
{
    AttributeParser attrs(originElem, schema);
    String loc;
    if (!attrs.get("schemaLocation", &loc) || loc.isEmpty()) {
        schema->mstream() << XsMessages::schemaLocationRequired
                          << COMMON_NS::Message::L_ERROR;
        return false;
    }
    RefCntPtr<Uri::GroveResource> git;
    GroveLib::Element* myTop = 0, *hisTop = 0;
    const GroveLib::Attr* nsAtt = 0;

    SchemaResourceBuilder builder(schema->getMessenger(),
        schema->pstream(), schema->getCatalogManager());
    loc = Url(originElem->grove()->topSysid()).combinePath2Path(loc);
    for (uint i = 0; i < includes_.size(); ++i)
        if (includes_[i] == loc)
            return false; // already included
    if (schema->pstream())
        *(schema->pstream()) << String("XSV: include " + loc);
    git = dynamic_cast<Uri::GroveResource*>(Uri::uriManager().
        getTopResource("grove", builder, loc));
    if (git.isNull() || !git->grove()->document()->documentElement()) {
        schema->mstream() << XsMessages::unableToIncludeSchema
                          << Message::L_ERROR  << loc;
        return false;
    }
    git->grove()->setTopSysid(loc);
    myTop  = originElem->root()->grove()->document()->documentElement();
    hisTop = git->grove()->document()->documentElement();
    nsAtt  = hisTop->attrs().getAttribute("targetNamespace");
    String hisNs("");
    if (nsAtt)
        hisNs = nsAtt->value();
    if (hisNs != targetNsUri() && !hisNs.isEmpty()) {
        schema->mstream() << XsMessages::includedNsMustBeEqual
            << Message::L_ERROR << hisNs;
        return false;
    }
    const Attr* elemFormDefaultAttr = 
        hisTop->attrs().getAttribute("elementFormDefault");
    const Attr* attrFormDefaultAttr =
        hisTop->attrs().getAttribute("attributeFormDefault");
    // todo: for optimization, call refast parser here
    if (hisTop->firstChild()) {
        GroveLib::Node* n = hisTop->firstChild();
        for (; n; n = n->nextSibling()) {
            if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
                continue;
            GroveLib::Element* e = ELEMENT_CAST(n->cloneNode(true, myTop));
            hisTop->condCopyNsMappings(e);
            e->addToPrefixMap("", targetNsUri());
            myTop->appendChild(e);
            myTop->lastChild()->setGSR(git->grove()->document());
            // set "form" attribute to explicit value
            GroveLib::Node* n2 = e->firstChild();
            for (; n2; n2 = n2->nextSibling()) {
                if (n2->nodeType() != GroveLib::Node::ELEMENT_NODE)
                    continue;
                set_form_attrs(ELEMENT_CAST(n2), 
                    elemFormDefaultAttr ? 
                        elemFormDefaultAttr->value() : String("unqualified"),
                    attrFormDefaultAttr ?
                        attrFormDefaultAttr->value() : String("unqualified"));
            }
        }
    }
    includes_.push_back(loc);
    return true;
}

XsDataImpl::~XsDataImpl()
{
        delete importMap_;
#ifdef USE_PYTHON
        delete script_;
#endif // USE_PYTHON
        delete substMap_;
}

XS_NAMESPACE_END
