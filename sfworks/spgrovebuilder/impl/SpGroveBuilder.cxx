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
#include "SpUtils.h"
#include "SpExtIdResolver.h"
#include "spgrovebuilder/SpGroveBuilder.h"

#include "common/String.h"
#include "common/StrdupNew.h"
#include "common/safecast.h"
#include "common/Vector.h"
#include "common/OwnerPtr.h"

#include "grove/grove_defs.h"
#include "grove/grove_trace.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/Grove.h"
#include "grove/EntityDecl.h"
#include "grove/EntityDeclSet.h"
#include "grove/XmlNs.h"
#include "grove/EntityReferenceTable.h"
#include "grove/XNodePtr.h"
#include "grove/IdManager.h"
#include "grove/GroveUtils.h"
#include "grove/RedlineUtils.h"
#include "grove/xinclude.h"
#include "grove/MatchPattern.h"
#include "../grove/GroveMessages.h"

#include "catmgr/CatalogManager.h"

#include "sp/config.h"
#include "sp/Boolean.h"
#include "sp/ParserApp.h"
#include "sp/ExtendEntityManager.h"
#include "sp/ExternalIdResolver.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <list>
#include <map>

#include "stdio.h"

// grove debugging
#include "sp/HashTable.cxx"

USING_COMMON_NS
GROVE_NAMESPACE_BEGIN

void make_entity_manager(Sp::ParserApp*, int locksId);
void make_mem_storage_manager(Sp::ParserApp*);

using CatMgr::CatalogManager;

SpGroveBuilder::SpGroveBuilder(int f)
  : GroveBuilder(f)
{
}

SpGroveBuilder::~SpGroveBuilder()
{
}

// note: set options directly via processOption, then call run() with files
class SpGroveApp : public Sp::ParserApp {
public:
    SpGroveApp(Grove* g, Sp::ExternalIdResolver* resolver)
        : Sp::ParserApp(0, resolver), cancel_(0), grove_(g)
    {
        make_entity_manager(this, 0);
        //    (g->groveBuilder()->flags() & GroveBuilder::doLocks)
        //        ? (int) g : 0);
        make_mem_storage_manager(this);
    }
    SP_NAMESPACE_SCOPE ParserOptions& options() { return options_; }

    /*! Run the grove application. Parse flags etc.*/
    unsigned int    run(int nFiles, char* const* files, const String& ssysid);
private:
    Sp::ErrorCountEventHandler *makeEventHandler() { return 0; }
    sig_atomic_t    cancel_;
    Grove*          grove_;
};

/*
*/
class SpEventHandler : public Sp::ErrorCountEventHandler {
public:
    SpEventHandler(SpGroveApp* app, Grove* g, const char* fn);
    virtual ~SpEventHandler();

    void        sgmlDecl(Sp::SgmlDeclEvent*);
    void        message(Sp::MessageEvent *);
    void        startElement(Sp::StartElementEvent*);
    void        endElement(Sp::EndElementEvent*);
    void        commentDecl(Sp::CommentDeclEvent*);
    void        endProlog(Sp::EndPrologEvent*);
    void        data(Sp::DataEvent*);
    void        startDtd(Sp::StartDtdEvent*);
    void        endDtd(Sp::EndDtdEvent*);
    void        entityDecl(Sp::EntityDeclEvent*);
    void        entityStart(Sp::EntityStartEvent*);
    void        entityEnd(Sp::EntityEndEvent*);
    void        externalDataEntity(Sp::ExternalDataEntityEvent*);
    void        sdataEntity(Sp::SdataEntityEvent*);
    void        subdocEntity(Sp::SubdocEntityEvent*);
    void        entityDefaulted(Sp::EntityDefaultedEvent*);
    void        markedSectionStart(Sp::MarkedSectionStartEvent *event);
    void        markedSectionEnd(Sp::MarkedSectionEndEvent *event);
    void        nonSgmlChar(Sp::NonSgmlCharEvent*);
    void        ignoredChars(Sp::IgnoredCharsEvent*);
    void        pi(Sp::PiEvent*);
    void        notationDecl(Sp::NotationDeclEvent*);
    void        elementDecl(Sp::ElementDeclEvent*);
    void        attlistDecl(Sp::AttlistDeclEvent*);
    void        sSep(Sp::SSepEvent*);

    //
    void        finalize();
    uint        numberOfElements() const { return numberOfElements_; }

private:
    // methods
    void        setNodeExt(Node* n, const Sp::Location& loc);
    void        setElementAttributes(Element*, const Sp::AttributeList&,
                                               const Sp::Location&);
    void        setNotationOnAttribute(Attr*, const Sp::Notation&);
    void        setEntityOnAttribute(Attr*, const Sp::Entity&);
    EntityDecl* addEntity(const Sp::Entity&, const Sp::EntityDecl*);
    void        setExternalId(ExternalId*, const Sp::ExternalId&);
    void        simpleEntityRef(const Sp::Entity& spe, Node* mid, Node* n);
    EntityDecl* getDecl(bool isParam, const Sp::StringC&);
    bool        isNotSDE(const Sp::Entity*);
    void        set_document(GroveSectionRoot*);
    void        restoreMarkup(String&, const Sp::Markup&, String* cs = 0);
    ElementDeclNode::ContentToken* build_model(ElementDeclNode::ContentToken*,
                                               const Sp::ContentToken* sptok,
                                               bool recurse) const;
private:
    // data
    SpGroveApp*                     app_;
    Grove*                          grove_;
    GroveSectionRoot*               document_;
    Node*                           n_;         // current node
    Text*                           text_;      // current Text node
    EntityReferenceStart*           ce_;        // current entity
    MarkedSectionStart*             mss_;       // current marked section
    const EntityReferenceStart*     prologCeRoot_;
    Common::OwnerPtr<RedlineParserTool> redlineTool_;
    ulong                           sdecnt_;    // SD entity level count
    bool                            defaultEntityPushed_;
    Common::Vector<Element*>        xincludes_;
    bool                            isXml_;
    bool                            processIds_;
    bool                            processXinclude_;
    bool                            preserveSSep_;
    bool                            want_esect_;
    bool                            want_comments_;
    bool                            want_ms_;
    bool                            no_attrgroups_;
    bool                            lineInfo_;
    bool                            skip_exs_;
    bool                            skipNextSSep_;
    uint                            numberOfElements_;

    Sp::OutputCharStream*           origStream_;
    Sp::StrOutputCharStream         strStream_;
    Sp::ConstPtr<Sp::Syntax>        syntax_;
    Sp::HashTable<Sp::StringC,
        Sp::Vector<Sp::StringC> >   adefs_;

    typedef std::map<String, RefCntPtr<AttrDeclNode::AttrDefGroup> >
        attributeGroupSet;
    attributeGroupSet   agset_;

    COMMON_NS::MessageStream        msg_stream_;
};

unsigned int SpGroveApp::run(int nFiles, 
                             char* const* files,
                             const String& ssysid)
{
    Sp::StringC sysid;
    if (!makeSystemId(nFiles, files, sysid))
        return 1;
    initParser(sysid);
    cancel_ = 0;

    // We use last file name as a top sysid for a grove. Maybe this
    // is ugly, but what's better?

    SpEventHandler handler(this, grove_, files[nFiles - 1]);
    parseAll(parser_, handler, &cancel_);
    if (!ssysid.isEmpty())
        grove_->setTopSysid(ssysid);
    handler.finalize();
    grove_->groveBuilder()->setNumberOfElements(handler.numberOfElements());
    XmlNs::makePredefinedNamespaces(grove_);
    return handler.errorCount();
}

SpEventHandler::SpEventHandler(SpGroveApp* spg_app, Grove* g, const char* sysid)
    : app_(spg_app),
      grove_(g),
      text_(0),
      sdecnt_(0),
      skipNextSSep_(0),
      numberOfElements_(0),
      msg_stream_(GroveMessages::getFacility(),
                  g->groveBuilder()->getMessenger())
{
    grove_->setTopSysid(from_utf8(sysid));
    origStream_ = app_->releaseMessageStream();
    app_->setMessageStream(&strStream_);
    set_document(grove_->heading());
    prologCeRoot_ = grove_->prolog()->ers();
    isXml_ = !(g->groveBuilder()->flags() & GroveBuilder::SGML);
    defaultEntityPushed_ = false;
    processIds_ =
        !(g->groveBuilder()->flags() & GroveBuilder::noIdProcessing);
    preserveSSep_ =
        !(g->groveBuilder()->flags() & GroveBuilder::noSSep);
    want_esect_ =
        !(g->groveBuilder()->flags() & GroveBuilder::noEntitySections);
    want_comments_ =
        !(g->groveBuilder()->flags() & GroveBuilder::noComments);
    want_ms_ =
        !(g->groveBuilder()->flags() & GroveBuilder::noMarkedSections);
    no_attrgroups_ =
        !!(g->groveBuilder()->flags() & GroveBuilder::noAttrGroups) | !isXml_;
    lineInfo_ =
        (g->groveBuilder()->flags() & GroveBuilder::lineInfo);
    skip_exs_ =
        !!(g->groveBuilder()->flags() & GroveBuilder::noExtSubsetDecls);
    redlineTool_ = new RedlineParserTool(grove_->document()->rss());
    processXinclude_ =
        (g->groveBuilder()->flags() & GroveBuilder::processXinclude);
}

SpEventHandler::~SpEventHandler()
{
    if (origStream_) {
        app_->releaseMessageStream();
        app_->setMessageStream(origStream_);
    }
}

void SpEventHandler::set_document(GroveSectionRoot* gsr)
{
    document_ = gsr;
    ce_  = document_->ers();
    mss_ = document_->mss();
    n_ = document_;
}

void SpEventHandler::finalize()
{
    if (redlineTool_)
        redlineTool_->finalize();
    process_xinclude_elements(xincludes_);
}

static bool isBuiltinEntity(const String& en)
{
    if (en == "apos" || en == "amp" || en == "lt" ||
        en == "gt"  || en == "quot")
            return true;
    return false;
}

void SpEventHandler::sgmlDecl(Sp::SgmlDeclEvent* event)
{
    // define sd entities
    syntax_ = event->instanceSyntaxPointer();
    for (ulong i = 0; i < syntax_->nEntities(); ++i) {
        InternalEntityDecl* ie = new InternalEntityDecl;
        ie->setEntityName(SpUtils::makeString(syntax_->entityName(i)));
        ie->setDeclType(EntityDecl::internalGeneralEntity);
        ie->setDataType(EntityDecl::sgml);
        ie->setDeclOrigin(EntityDecl::sd);
        String content(syntax_->entityChar(i));
        ie->setContent(content);
        grove_->entityDecls()->insertDecl(ie);
    }
    // XML/SP HCRO kludge: SP doesn't take it correctly from SD
    if (isXml_) {
        Sp::StringC hcro;
        hcro += '&'; hcro += '#'; hcro += 'x';
        const_cast<Sp::Syntax*>(syntax_.pointer())->
            setDelimGeneral(Sp::Syntax::dHCRO, hcro);
    }
    delete event;
}

void SpEventHandler::message(Sp::MessageEvent* event)
{
    OwnerPtr<Sp::MessageEvent> evp(event);
    const Sp::Message& msg = event->message();
    String formatted_msg = SpUtils::extractFormattedMessage(msg);
    if (msg_stream_.getMessenger()) {
        PlainOrigin* src_loc =
            new PlainOrigin(SpUtils::extractSourceLocation(msg.loc));
        msg_stream_ << GroveMessages::ParserMessage
                    << CompositeOrigin(src_loc)
                    << formatted_msg << Message::L_ERROR;
    }
}

void SpEventHandler::setNodeExt(Node* n, const Sp::Location& loc)
{
    if (lineInfo_) {
        PlainOrigin src_loc = SpUtils::extractSourceLocation(loc);
        n->setNodeExt(new LineLocExt(src_loc));
    }
}

void SpEventHandler::startElement(Sp::StartElementEvent* event)
{
    numberOfElements_++;
    text_ = 0;
    Element* nn = new Element(SpUtils::makeString(event->name()));
    n_->appendChild(nn);
    n_ = nn;
    setNodeExt(n_, event->location());
    if (event->attributes().size() > 0)
        setElementAttributes(nn, event->attributes(),event->location());
    DBG(GROVE.BUILDER) << "Start element: " << nn->nodeName() << std::endl;
    delete event;
}

void SpEventHandler::endElement(Sp::EndElementEvent* event)
{
    text_ = 0;
    DBG(GROVE.BUILDER) << "End element: " << n_->nodeName() << std::endl;
    if (processXinclude_) {
        Element* e = static_cast<Element*>(n_);
        if (is_xinclude_element(e))
            xincludes_.push_back(e);
    }
    if (redlineTool_)
        redlineTool_->processElementEnd(n_);
    if (n_->parent())
        n_ = n_->parent();
    delete event;
}

#define SKIP_EXS(ev) \
    if (skip_exs_ && ce_ != prologCeRoot_) { delete ev; return; }

void SpEventHandler::commentDecl(Sp::CommentDeclEvent* event)
{
    static const char arb_comment[] = "ArborText, Inc., 20";
    if (!want_comments_) {
        delete event;
        return;
    }
    Comment* nn = new Comment;
    n_->appendChild(nn);
    text_ = 0;
    setNodeExt(nn, event->location());
    Sp::StringC cs;

    for (Sp::MarkupIter iter(event->markup()); iter.valid(); iter.advance()) {
        if (iter.type() == Sp::Markup::comment) {
            cs.assign(iter.charsPointer(), iter.charsLength());
            nn->appendComment(SpUtils::makeString(cs));
        }
    }
    if (nn->comment().left(sizeof(arb_comment) - 1) == arb_comment)
        nn->remove();
    delete event;
}

void SpEventHandler::endProlog(Sp::EndPrologEvent* event)
{
    DBG(GROVE.BUILDER) << "prolog end reached" << std::endl;
    GroveSectionStart* gs = prologCeRoot_->getFirstSect();
    for (; gs && gs->getNextSect(); gs = gs->getNextSect())
            ;
    Node* n = 0;
    if (gs)
        n = gs->getSectEnd();
    if (n)
        n = n->nextSibling();   // skip ERE
    while (n) {
        NodePtr np = n;
        n = n->nextSibling();
        np->remove();
        grove_->document()->appendChild(np.pointer());
    }
    set_document(grove_->document());
    // check for defined default entity, and if nonexistent,
    // define implicit default entity
    Sp::Dtd& dtd = const_cast<Sp::Dtd&>(event->dtd());
    if (dtd.defaultEntity().isNull()) {
        DBG(GROVE.BUILDER) << "defining new default entity" << std::endl;
        Sp::Text text;
        Sp::StringC ename(app_->systemCharset().execToDesc("#DEFAULT"));
        Sp::Entity* ne = new Sp::InternalTextEntity(ename,
            Sp::EntityDecl::generalEntity, event->location(), text,
            Sp::InternalTextEntity::none);
        ne->setDeclIn(dtd.namePointer(), dtd.isBase());
        dtd.setDefaultEntity(ne);
        defaultEntityPushed_ = true;
    }
    delete event;
}

void SpEventHandler::notationDecl(Sp::NotationDeclEvent* event)
{
    const Sp::Notation* spnot = event->notationPointer().pointer();
    EntityDeclSet& nds = grove_->notations();
    Notation* mynot = new Notation;
    String sm, cs;
    restoreMarkup(sm, event->markup(), &cs);
    mynot->setOriginalDecl(sm);
    if (cs.length() > 0)
        mynot->setComment(cs);
    mynot->setEntityName(SpUtils::makeString(spnot->name()));
    DBG(GROVE.BUILDER) << "NOTATION DECL: " << mynot->name() << std::endl;
    mynot->setDeclType(EntityDecl::notation);
    mynot->setDataType(EntityDecl::ndata);
    if (!spnot->defLocation().origin().isNull() &&
        spnot->defLocation().origin()->entity())
        mynot->setDeclOrigin(EntityDecl::dtd);
    else
        mynot->setDeclOrigin(EntityDecl::prolog);
    setExternalId(&mynot->externalId(), spnot->externalId());
    nds.insertDecl(mynot);
    EntityDeclNode* nd = new EntityDeclNode;
    setNodeExt(nd, event->location());
    nd->setDecl(mynot);
    n_->appendChild(nd);
    delete event;
}

void SpEventHandler::entityDefaulted(Sp::EntityDefaultedEvent* event)
{
    DBG(GROVE.BUILDER) << "entity defaulted : "
        << SpUtils::makeString(event->entity().name()) << std::endl;

    EntityDecl* e = addEntity(event->entity(),
                              event->location().origin()->entityDecl());
    if (isXml_ || defaultEntityPushed_) {
        e->setDeclType(EntityDecl::internalGeneralEntity);
        e->setDeclOrigin(EntityDecl::invalidDeclOrigin);
    } else 
        e->setDeclType(EntityDecl::defaulted);
    delete event;
}

void SpEventHandler::startDtd(Sp::StartDtdEvent* event)
{
    set_document(grove_->prolog());
    DBG(GROVE.BUILDER) << "dtd start reached: " <<
        SpUtils::makeString(event->name()) << std::endl;
    if (grove_->doctypeName().isEmpty())
        grove_->setDoctypeName(SpUtils::makeString(event->name()));
    if (!event->entity().isNull())
        addEntity(*event->entity().pointer(), 0);
    delete event;
}

void SpEventHandler::endDtd(Sp::EndDtdEvent* event)
{
    DBG(GROVE.BUILDER) << "dtd end reached" << std::endl;
    skipNextSSep_ = true;
    delete event;
}

void SpEventHandler::data(Sp::DataEvent* event)
{
    String cdata;
    SpUtils::assignString(cdata, event->data(), event->dataLength());
    DBG(GROVE.BUILDER) << "data: " << cdata << std::endl;
    if (cdata.length() > 0) {
        if (event->entity() && isNotSDE(event->entity())) {
            text_ = new Text;
            text_->setData(cdata);
            setNodeExt(text_, event->location());
            simpleEntityRef(*event->entity(), text_, n_);
            text_ = 0;
        } else {
            if (text_) {
                text_->appendData(cdata);
            } else {
                text_ = new Text;
                text_->setData(cdata);
                setNodeExt(text_, event->location());
                n_->appendChild(text_);
            }
        }
    }
    delete event;
}

void SpEventHandler::nonSgmlChar(Sp::NonSgmlCharEvent* event)
{
    DBG(GROVE.BUILDER) << "non-sgml-char-event\n";
    String cdata;
    Sp::Char ch = event->character();
    SpUtils::assignString(cdata, &ch, 1);
    if (text_) {
        text_->appendData(cdata);
    } else {
        text_ = new Text;
        text_->setData(cdata);
        setNodeExt(text_, event->location());
        n_->appendChild(text_);
    }
    delete event;
}

void SpEventHandler::sdataEntity(Sp::SdataEntityEvent* event)
{
    String cdata;
    SpUtils::assignString(cdata, event->data(), event->dataLength());
    if (text_) {
        text_->appendData(cdata);
    } else {
        text_ = new Text;
        text_->setData(cdata);
        setNodeExt(text_, event->location());
        n_->appendChild(text_);
    }
    delete event;
}

void SpEventHandler::entityDecl(Sp::EntityDeclEvent* event)
{
    EntityDecl* e =
        addEntity(event->entity(), event->location().origin()->entityDecl());
    String sm, cs;
    restoreMarkup(sm, event->markup(), &cs);
    e->setOriginalDecl(sm);
    if (cs.length() > 0)
        e->setComment(cs);
    if (document_ == grove_->prolog()) {
        EntityDeclNode* n = new EntityDeclNode;
        setNodeExt(n, event->location());
        n->setDecl(e);
        n_->appendChild(n);
        e->setEntityDeclNode(n);
    }
    delete event;
}

void SpEventHandler::entityStart(Sp::EntityStartEvent* event)
{
    text_ = 0;
    DBG(GROVE.BUILDER) << "Entity Start Event, Entity name: "
        << SpUtils::makeString(event->entity()->name()) << std::endl;
    if (!want_esect_) {
        delete event;
        return;
    }
    const Sp::Entity* se = event->entity();
    if (se->asExternalEntity() &&
        !se->asExternalEntity()->systemIdPointer() &&
        !se->asExternalEntity()->effectiveSystemIdPointer()) {
            DBG(GROVE.BUILDER) << "Simple entity ref\n";
            simpleEntityRef(*se, 0, n_);
            delete event;
            return;
    }
    EntityDecl* decl;
    bool isParam = (se->declType() == Sp::EntityDecl::parameterEntity);
    if (se->declType() == Sp::EntityDecl::doctype)
        decl = const_cast<ExternalEntityDecl*>(grove_->doctypeEntity());
    else
        decl = getDecl(isParam, se->name());
    RT_ASSERT(0 != decl);
    if (decl->declOrigin() != EntityDecl::sd) {
        DBG(GROVE.BUILDER) << "decl name = " << decl->name() << std::endl;
        EntityReferenceStart* s = new EntityReferenceStart;
        ce_->appendSect(s);
        ce_ = s;
        s->setDecl(decl);
        n_->appendChild(s);
        if (isParam)
            grove_->parameterErt().addEntityRef(s);
        else
            document_->ert()->addEntityRef(s);
    }
    else {
        sdecnt_++;
        DBG(GROVE.BUILDER) <<"*** syntax instance entity!\n";
    }
    DBG(GROVE.BUILDER) << "Entity start event done\n";
    delete event;
}

void SpEventHandler::entityEnd(Sp::EntityEndEvent* event)
{
    if (want_esect_) {
        if (0 == sdecnt_) {
            text_ = 0;
            DBG(GROVE.BUILDER) << "Entity End Event\n";
            if (ce_->entityDecl() && ce_->entityDecl()->declType() ==
                EntityDecl::externalGeneralEntity) {
                    Node* ces = ce_->nextSibling();
                    if (ces && ces->nodeType() == Node::TEXT_NODE) {
                        Text* t = static_cast<Text*>(ces);
                        if (t->data() == "\n")
                            t->remove();
                    }
            }
            EntityReferenceEnd* ee = new EntityReferenceEnd;
            ee->setSectStart(ce_);
            ce_->setSectEnd(ee);
            ce_ = static_cast<EntityReferenceStart*>(ce_->getSectParent());
            n_->appendChild(ee);
            // TODO: adjust() - or check that erefs on the same level
        }
        else
            --sdecnt_;
    }
    delete event;
}

void SpEventHandler::externalDataEntity(Sp::ExternalDataEntityEvent* event)
{
    text_ = 0;
    DBG(GROVE.BUILDER) << "ExternalDataEntity Event, Entity name: "
        << SpUtils::makeString(event->entity()->name()) << std::endl;
    simpleEntityRef(*event->entity(), 0, n_);
    delete event;
}

void SpEventHandler::subdocEntity(Sp::SubdocEntityEvent* event)
{
    text_ = 0;
    DBG(GROVE.BUILDER) << "SubdocEntityEvent, Entity name: "
        << SpUtils::makeString(event->entity()->name()) << std::endl;
    simpleEntityRef(*event->entity(), 0, n_);
    delete event;
}

void SpEventHandler::markedSectionStart(Sp::MarkedSectionStartEvent *event)
{
    text_ = 0;
    if (!want_ms_) {
        delete event;
        return;
    }
    MarkedSectionStart* s = new MarkedSectionStart;
    s->setSectParent(mss_);
    mss_->appendSect(s);
    n_->appendChild(s);
    mss_ = s;
    int depth = 0;
    for (Sp::MarkupIter iter(event->markup()); iter.valid(); iter.advance()) {
        switch (iter.type()) {
            case Sp::Markup::reservedName: {
                if (depth)
                    break;
                switch (iter.reservedName()) {
                    case Sp::Syntax::rTEMP:
                        s->setType(MarkedSectionStart::temp);
                        break;
                    case Sp::Syntax::rIGNORE:
                        s->setType(MarkedSectionStart::ignore);
                        break;
                    case Sp::Syntax::rINCLUDE:
                        s->setType(MarkedSectionStart::include);
                        break;
                    case Sp::Syntax::rRCDATA:
                        s->setType(MarkedSectionStart::rcdata);
                        break;
                    case Sp::Syntax::rCDATA:
                        s->setType(MarkedSectionStart::cdata);
                        break;
                    default:
                        RT_MSG_ABORT("bad marked section type");
                        break;
                }
                break;
            }
            case Sp::Markup::entityStart: {
                if (depth || !s->entityDecl()) {
                    ++depth;
                    break;
                }
                DBG(GROVE.BUILDER) << "msect: " << SpUtils::makeString(iter.
                    entityOrigin()->entity()->name()) << std::endl;
                EntityDecl* decl =
                    getDecl(true, iter.entityOrigin()->entity()->name());
                RT_ASSERT(0 != decl);
                s->setDecl(decl);
                s->setType(MarkedSectionStart::entityRef);
                depth++;
                break;
            }
            case Sp::Markup::entityEnd:
                depth--;
                break;
            default:
                break;
        }
    }
    switch (event->status()) {
        case Sp::MarkedSectionEvent::include:
            s->setStatus(MarkedSectionStart::Included);
            break;
        case Sp::MarkedSectionEvent::rcdata:
            s->setStatus(MarkedSectionStart::Rcdata);
            break;
        case Sp::MarkedSectionEvent::cdata:
            s->setStatus(MarkedSectionStart::Cdata);
            break;
        case Sp::MarkedSectionEvent::ignore:
            s->setStatus(MarkedSectionStart::Ignored);
            break;
        default:
            break;
    }
    delete event;
}

void SpEventHandler::markedSectionEnd(Sp::MarkedSectionEndEvent *event)
{
    text_ = 0;
    if (!want_ms_) {
        delete event;
        return;
    }
    MarkedSectionEnd* me = new MarkedSectionEnd;
    me->setSectStart(mss_);
    mss_->setSectEnd(me);
    mss_ = static_cast<MarkedSectionStart*>(mss_->getSectParent());
    n_->appendChild(me);
    // TODO: ms->adjust();
    delete event;
}

void SpEventHandler::ignoredChars(Sp::IgnoredCharsEvent* event)
{
    DBG(GROVE.BUILDER) <<"Ignored chars\n";
    if (want_ms_) {
        String sv;
        SpUtils::assignString(sv, event->data(), event->dataLength());
        mss_->appendIgnoredChars(sv);
    }
    delete event;
}

void SpEventHandler::pi(Sp::PiEvent* event)
{
    String sv;
    SpUtils::assignString(sv, event->data(), event->dataLength());
    RefCntPtr<ProcessingInstruction> pi = new ProcessingInstruction;
    setNodeExt(pi.pointer(), event->location());
    const Char* sp = sv.unicode();
    const Char* ep = sp + sv.length();
    const Char* s = sp;
    while (s < ep && !s->isSpace())
        ++s;
    if (s != ep) {
        pi->setTarget(String(sp, s - sp));
        while (s < ep && s->isSpace())
            ++s;
        pi->setData(String(s, ep - s));
    } else
        pi->setTarget(sv);
    DBG(GROVE.BUILDER) << "PI: " << pi->target() << " "
        << pi->data() << std::endl;
    if (event->entity())
        pi->setEntityName(SpUtils::makeString(event->entity()->name()));

    // Set encoding on entity
    if (pi->target() == "xml") {
        skipNextSSep_ = true;
        static const char enc_var[] = "encoding=";
        int idx = pi->data().find(enc_var);
        if (idx >= 0) {
            const Char* cp = pi->data().unicode() + idx +
                sizeof(enc_var) - 1;
            const Char* ce = pi->data().unicode() + pi->data().length();
            String enc;
            if (cp < ce && (*cp == '"' || *cp == '\'')) {
                ++cp;
                while (cp < ce && *cp != '"' && *cp != '\'')
                    enc += *cp++;
            }
            if (!enc.isEmpty()) {
                EntityReferenceStart* ers = ce_;
                Encodings::Encoding encoding =
                    Encodings::encodingByName(enc.upper());
                while (ers && ers->entityDecl() &&
                    !ers->entityDecl()->asExternalEntityDecl())
                        ers = static_cast<EntityReferenceStart*>
                            (ers->getSectParent());
                if (ers && ers->entityDecl())
                    ers->entityDecl()->asExternalEntityDecl()->setEncoding
                        (encoding);
                else
                    grove_->topDecl()->setEncoding(encoding);
            }
        }
    } else {
        n_->appendChild(pi.pointer());
        if (redlineTool_)
            redlineTool_->processPi(pi.pointer());
    }
    text_ = 0;
    delete event;
}

void SpEventHandler::elementDecl(Sp::ElementDeclEvent* event)
{
    SKIP_EXS(event);
    ElementDeclNode* ed = new ElementDeclNode;
    setNodeExt(ed, event->location());
    String origDecl;
    restoreMarkup(origDecl, event->markup());
    ed->setOriginalDecl(origDecl);
    DBG(GROVE.BUILDER) << "elementDecl: " << ed->originalDecl() << std::endl;
    for (ulong elemIdx = 0; elemIdx < event->elements().size(); ++elemIdx)
        ed->appendElement(SpUtils::makeString
            (event->elements()[elemIdx]->name()));
    const Sp::ElementDefinition* d = event->elements()[0]->definition();
    switch (d->declaredContent()) {
        case Sp::ElementDefinition::any:
            ed->setContentType(ElementDeclNode::any);
            break;
        case Sp::ElementDefinition::cdata:
        case Sp::ElementDefinition::rcdata:
            ed->setContentType(ElementDeclNode::cdata);
            break;
        case Sp::ElementDefinition::empty:
            ed->setContentType(ElementDeclNode::empty);
            break;
        case Sp::ElementDefinition::modelGroup: {
            bool hasPcdata = d->compiledModelGroup()->containsPcdata();
            const Sp::ModelGroup* smg = d->compiledModelGroup()->modelGroup();
            if (hasPcdata) {
                ed->setContentType(ElementDeclNode::mixed);
                ed->setContent(build_model(new ElementDeclNode::ContentToken,
                    smg, false));
            }
            else {
                ed->setContentType(ElementDeclNode::modelElt);
                ed->setContent(build_model(0, smg, true));
            }
            break;
        }
        default:
            RT_MSG_ABORT("");
    }
    for (ulong i = 0; i < d->nInclusions(); ++i)
        ed->appendInclusion(SpUtils::makeString(d->inclusion(i)->name()));
    for (ulong i = 0; i < d->nExclusions(); ++i)
        ed->appendExclusion(SpUtils::makeString(d->exclusion(i)->name()));
    n_->appendChild(ed);
    delete event;
}

ElementDeclNode::ContentToken*
SpEventHandler::build_model(ElementDeclNode::ContentToken* ct,
                            const Sp::ContentToken* sptok,
                            bool recurse) const
{
    if (!ct)
        ct = new ElementDeclNode::ContentToken;
    ElementDeclNode::ContentToken::Occurence occurs;
    switch (sptok->occurrenceIndicator()) {
        case Sp::ContentToken::opt:
            occurs = ElementDeclNode::ContentToken::opt;
            break;
        case Sp::ContentToken::plus:
            occurs = ElementDeclNode::ContentToken::plus;
            break;
        case Sp::ContentToken::rep:
            occurs = ElementDeclNode::ContentToken::rep;
            break;
        default:
            occurs = ElementDeclNode::ContentToken::none;
            break;
    }
    const Sp::LeafContentToken* leaf = sptok->asLeafContentToken();
    if (leaf) {
        if (leaf->elementType()) {
            if (recurse) {
                ct->token = SpUtils::makeString(leaf->elementType()->name());
                ct->connector = ElementDeclNode::ContentToken::leaf;
                ct->occurs    = occurs;
            }
            else {
                ElementDeclNode::ContentToken* newct =
                    new ElementDeclNode::ContentToken;
                newct->token =
                    SpUtils::makeString(leaf->elementType()->name());
                newct->occurs = occurs;
                ct->subexps.push_back(newct);
            }
        }
        return ct;
    }
    if (recurse)
        ct->occurs = occurs;
    const Sp::ModelGroup* mg = sptok->asModelGroup();
    switch (mg->connector()) {
        case Sp::ModelGroup::andConnector:
            ct->connector = ElementDeclNode::ContentToken::all;
            break;
        case Sp::ModelGroup::orConnector:
            ct->connector = ElementDeclNode::ContentToken::choice;
            break;
        case Sp::ModelGroup::seqConnector:
            ct->connector = ElementDeclNode::ContentToken::sequence;
            break;
        default:
            ct->connector = ElementDeclNode::ContentToken::leaf;
    }
    if (recurse) {
        for (ulong i = 0; i < mg->nMembers(); ++i)
            ct->subexps.push_back(build_model(0, &mg->member(i), recurse));
    }
    else {
        for (ulong i = 0; i < mg->nMembers(); ++i)
            build_model(ct, &mg->member(i), false);
    }
    return ct;
}

// Auxilliary attribute definition list parser. Used only in case if
// there's nested parameter entity references in attribute list (SP itself
// loses information about entity nesting).
class SpAttlistParser {
public:
    enum TokenType {
        UNKN, END, LITERAL, VTYPE, NAME, EREF, ERS, ERE, COMMENT, SKIP
    };
    struct Token {
        TokenType   type;
        String      value;
        Token(TokenType t = UNKN, const String& v = String())
            : type(t), value(v) {}
    };
    typedef Vector<Token> AttTokenList;
    Token       getToken();
    static void parse_attlist(const EntityDeclSet& eds,
                              AttTokenList& atl, const String& source);
    static void linearize(const EntityDeclSet& eds,
                          AttTokenList& atl, const String& source);

    SpAttlistParser(const Char* from, const Char* to)
        : ds_(from), ce_(to), cp_(ds_) {}

private:
    static bool is_xchar(const Char c)
    {
        return c.isLetterOrNumber() || c == '.' || c == '-' || c == '_'
            || c == '#' || c == ':' || c == '$';
    }
    const Char*   ds_;    // data start
    const Char*   ce_;    // data end
    const Char*   cp_;    // current position
};

SpAttlistParser::Token SpAttlistParser::getToken()
{
    const Char* p;
    while (cp_ < ce_ && cp_->isSpace())
        ++cp_;
    if (cp_ >= ce_)
        return Token(END);
    if (*cp_ == '%') {
        p = ++cp_;
        while (*cp_ != ';' && cp_ < ce_)
            ++cp_;
        if (cp_ >= ce_)
            return Token(UNKN);
        return Token(EREF, String(p, cp_++ - p));
    }
    if (*cp_ == '"' || *cp_ == '\'') {
        Char qend = *cp_;
        for (++cp_; cp_ < ce_ && *cp_ != qend; ++cp_)
            ;
        ++cp_;
        return Token(LITERAL);
    }
    if (*cp_ == '(') {
        for (++cp_; cp_ < ce_ && *cp_ != ')'; ++cp_)
            ;
        ++cp_;
        return Token(VTYPE);
    }
    if (*cp_ == '-' && &cp_[3] < ce_ && cp_[1] == '-') {
        cp_ += 2;
        for (; &cp_[1] < ce_ && *cp_ != '-' && cp_[1] != '-'; ++cp_)
            ;
        if (&cp_[1] >= ce_)
            return END;
        return Token(COMMENT);
    }
    p = cp_;
    while (is_xchar(*cp_))
        ++cp_;
    if (p != cp_)
        return Token(NAME, String(p, cp_ - p));
    ++cp_;
    return UNKN;
}

#define GET_NEXT(from, tok) \
    { tok = te; for (ct = from + 1; ct < te && \
          (ct->type == ERS || ct->type == ERE); ++ct) ; \
     if (ct >= te) break; \
     tok = ct; }

void SpAttlistParser::parse_attlist(const EntityDeclSet& eds,
                                    AttTokenList& tl,
                                    const String& decl)
{
    linearize(eds, tl, decl);
    SpAttlistParser::Token *ct, *t =  &tl[0];
    SpAttlistParser::Token* te = t + tl.size();
    SpAttlistParser::Token *t1 = te, *t2 = te, *t3 = te;
    while (t < te) {
        while (t < te && (t->type == ERS || t->type == ERE) )
            ++t;
        if (t >= te)
            break;
        if (t->type != NAME) {
            t->type = SKIP;
            ++t;
            continue;
        }
        GET_NEXT(t, t1);
        if (t1->type == NAME) {
            if (t1->value == "CDATA" || t1->value == "ID"
                || t1->value == "IDREFS" || t1->value == "ENTITY"
                || t1->value == "ENTITIES" || t1->value == "NMTOKEN"
                || t1->value == "IDREF" || t1->value == "NMTOKENS") {
                    t1->type = SKIP;
                    GET_NEXT(t1, t2);
                    if (t2->value == "#FIXED") {
                        GET_NEXT(t2, t3);
                        t2->type = SKIP; t3->type = SKIP;
                        t = t3 + 1;
                    } else {
                        t2->type = SKIP;
                        t = t2 + 1;
                        continue;
                    }
            }
            if (t1->value == "NOTATION") {
                t1->type = SKIP;
                GET_NEXT(t1, t2);
                if (t2->type != VTYPE) {
                    t = t2;
                    continue;
                }
                t2->type = SKIP;
                GET_NEXT(t2, t3);
                if (t3->type != NAME && t3->type != LITERAL) {
                    t = t3;
                    continue;
                }
                t3->type = SKIP;
                t = t3 + 1;
                continue;
            } else {
                t = t1;
                continue;
            }
        } else if (t1->type == VTYPE) {
            GET_NEXT(t1, t2);
            t1->type = SKIP;
            t2->type = SKIP;
            t = t2 + 1;
            continue;
        } else
            ++t;
    }
}

void SpAttlistParser::linearize(const EntityDeclSet& eds,
                                AttTokenList& tl,
                                const String& decl)
{
    const Char* cp = decl.unicode();
    const Char* ce = cp + decl.length() - 1;
    cp += 9; // beginning: <!ENTITY ...
    while (cp < ce && *cp != '%')
        ++cp;
    if (cp >= ce)
        return;
    ++cp;
    while (cp < ce && cp->isSpace())
        ++cp;
    if (cp >= ce)
        return;
    while (cp < ce && is_xchar(*cp))
        ++cp;
    while (cp < ce && cp->isSpace())
        ++cp;
    if (cp >= ce)
        return;
    if (*cp == '\'' || *cp == '"') {
        --ce;
        ++cp;
    }
    if (cp >= ce)
        return;
    SpAttlistParser atp(cp, ce);
    for (;;) {
        Token t = atp.getToken();
        if (t.type == END)
            return;
        if (t.type == EREF) {
            EntityDecl* med = eds.lookupDecl(t.value);
            if (0 == med)
                continue;
            tl.push_back(Token(ERS, t.value));
            linearize(eds, tl, med->originalDecl());
            tl.push_back(Token(ERE, t.value));
            continue;
        }
        tl.push_back(t);
    }
}

//
// The following mess is actually a workaround for attlist
// decls in XML mode. SP doesn't correctly handle multiple ATTLISTS
// for the same element names.
//
void SpEventHandler::attlistDecl(Sp::AttlistDeclEvent* event)
{
    SKIP_EXS(event);
    AttrDeclNode* adn = new AttrDeclNode;
    setNodeExt(adn, event->location());
    String origDecl;
    restoreMarkup(origDecl, event->markup());
    adn->setOriginalDecl(origDecl);
    for (ulong i = 0; i < event->elements().size(); ++i)
        adn->appendElement(SpUtils::makeString
            (event->elements()[i]->name()));
    const Sp::ElementType* spt = event->elements()[0];
    const Sp::AttributeDefinitionList* adl = spt->attributeDefTemp();
    typedef Sp::Vector<Sp::StringC> nameVec;
    nameVec deflist;
    for (ulong i = 0; i < adl->size(); ++i) {
        const Sp::StringC& an = adl->def(i)->name();
        nameVec* spv = const_cast<nameVec*>(adefs_.lookup(spt->name()));
        if (spv) {
            ulong j;
            for (j = 0; j < spv->size(); ++j)
                if ((*spv)[j] == an)
                    break;
            if (j == spv->size()) {
                deflist.push_back(an);
                spv->push_back(an);
            }
        }
        else {
            deflist.push_back(an);
            nameVec nv;
            nv.push_back(an);
            adefs_.insert(spt->name(), nv);
        }
    }
    typedef std::map<String, RefCntPtr<AttrDeclNode::AttrDef> > adefMap;
    adefMap adefs;

    for (ulong i = 0; i < deflist.size(); ++i) {
        const Sp::AttributeDefinition* spdef = 0;
        for (ulong j = 0; j < adl->size(); ++j) {
            if (adl->def(j)->name() == deflist[i]) {
                spdef = adl->def(j);
                break;
            }
        }
        RT_ASSERT(spdef);
        Sp::AttributeDefinitionDesc sad;
        spdef->getDesc(sad);

        RefCntPtr<AttrDeclNode::AttrDef> adef = new AttrDeclNode::AttrDef;
        adef->setName(SpUtils::makeString(deflist[i]));
        adefs[adef->name()] = adef;

        // This is ugly, but to depend on enum on 3rd component
        // is even worse, and building indexed table is no better.
        switch (sad.declaredValue) {
            case Sp::AttributeDefinitionDesc::cdata:
                adef->setDeclaredValue(AttrDeclNode::cdata);    break;
            case Sp::AttributeDefinitionDesc::name:
                adef->setDeclaredValue(AttrDeclNode::name);     break;
            case Sp::AttributeDefinitionDesc::number:
                adef->setDeclaredValue(AttrDeclNode::number);   break;
            case Sp::AttributeDefinitionDesc::nmtoken:
                adef->setDeclaredValue(AttrDeclNode::nmtoken);  break;
            case Sp::AttributeDefinitionDesc::nutoken:
                adef->setDeclaredValue(AttrDeclNode::nutoken);  break;
            case Sp::AttributeDefinitionDesc::entity:
                adef->setDeclaredValue(AttrDeclNode::entity);   break;
            case Sp::AttributeDefinitionDesc::idref:
                adef->setDeclaredValue(AttrDeclNode::idref);    break;
            case Sp::AttributeDefinitionDesc::names:
                adef->setDeclaredValue(AttrDeclNode::names);    break;
            case Sp::AttributeDefinitionDesc::numbers:
                adef->setDeclaredValue(AttrDeclNode::numbers);  break;
            case Sp::AttributeDefinitionDesc::nmtokens:
                adef->setDeclaredValue(AttrDeclNode::nmtokens); break;
            case Sp::AttributeDefinitionDesc::nutokens:
                adef->setDeclaredValue(AttrDeclNode::nutokens); break;
            case Sp::AttributeDefinitionDesc::entities:
                adef->setDeclaredValue(AttrDeclNode::entities); break;
            case Sp::AttributeDefinitionDesc::idrefs:
                adef->setDeclaredValue(AttrDeclNode::idrefs);   break;
            case Sp::AttributeDefinitionDesc::id:
                adef->setDeclaredValue(AttrDeclNode::id);       break;
            case Sp::AttributeDefinitionDesc::notation:
                adef->setDeclaredValue(AttrDeclNode::notation); break;
            case Sp::AttributeDefinitionDesc::nameTokenGroup:
                adef->setDeclaredValue(AttrDeclNode::nameTokenGroup); break;
            default:
                break;
        }
        switch (sad.defaultValueType) {
            case Sp::AttributeDefinitionDesc::required:
                adef->setDeftype(AttrDeclNode::required);    break;
            case Sp::AttributeDefinitionDesc::current:
                adef->setDeftype(AttrDeclNode::current);     break;
            case Sp::AttributeDefinitionDesc::implied:
                adef->setDeftype(AttrDeclNode::implied);     break;
            case Sp::AttributeDefinitionDesc::conref:
                adef->setDeftype(AttrDeclNode::conref);      break;
            case Sp::AttributeDefinitionDesc::defaulted:
                adef->setDeftype(AttrDeclNode::defaulted);   break;
            case Sp::AttributeDefinitionDesc::fixed:
                adef->setDeftype(AttrDeclNode::fixed);       break;
            default:
                break;
        }
        for (ulong j = 0; j < sad.allowedValues.size(); ++j)
            adef->appendAllowedValue(SpUtils::makeString(sad.allowedValues[j]));
        if (!sad.defaultValue.isNull()) {
            const Sp::Text* text = sad.defaultValue->text();
            if (text)
                adef->setDefvalue(SpUtils::makeString(text->string()));
        }
    }
    adefMap::iterator aditer;
    if (no_attrgroups_) {
        for (aditer = adefs.begin(); aditer != adefs.end(); ++aditer)
            adn->attrs().appendAttrDef(aditer->second.pointer());
        n_->appendChild(adn);
        delete event;
        return;
    }
    Sp::MarkupIter iter(event->markup());
    AttrDeclNode::AttrDefGroup* attachTo = &adn->attrs();
    std::list<AttrDeclNode::AttrDefGroup*> attachStack;
    attachStack.push_back(attachTo);   // initially, attach to top group
    const Sp::EntityDecl* spe = 0;
    bool hadElemName = false;
    for (; iter.valid(); iter.advance()) {
        switch(iter.type()) {
            case Sp::Markup::entityStart: {
                spe = iter.entityOrigin()->entity();
                String ename = SpUtils::makeString(spe->name());
                attributeGroupSet::iterator agiter = agset_.find(ename);
                if (agiter != agset_.end()) {
                    attachTo->appendAttrDef(agiter->second.pointer());
                    while (iter.type() != Sp::Markup::entityEnd
                        && iter.valid())
                            iter.advance();
                    continue;
                }
                EntityDecl* med =
                    grove_->parameterEntityDecls().lookupDecl(ename);
                assert(med);
                if (0 == hadElemName)
                    continue;
                SpAttlistParser::AttTokenList tv;
                SpAttlistParser::parse_attlist(grove_->parameterEntityDecls(),
                    tv, med->originalDecl());
                AttrDeclNode::AttrDefGroup* pa = attachTo;
                attachTo = new AttrDeclNode::AttrDefGroup;
                attachTo->setName(ename);
                agset_[ename] = attachTo;
                attachStack.push_back(attachTo);
                pa->appendAttrDef(attachTo);
//#define ATTLIST_PSEQ_DEBUG
#ifdef ATTLIST_PSEQ_DEBUG
                std::cerr << "PSEQ " << ename << ":\n";
                for (uint k = 0; k < tv.size(); ++k) {
                    const SpAttlistParser::Token& t = tv[k];
                    if (t.type == SpAttlistParser::SKIP)
                        std::cerr << "\tSKIP";
                    else if (t.type == SpAttlistParser::NAME)
                        std::cerr << "\tNAME";
                    else if (t.type == SpAttlistParser::ERS)
                        std::cerr << "\tERS";
                    if (t.type == SpAttlistParser::ERE)
                        std::cerr << "\tERE";
                    std::cerr << "(" << t.value << ")\n";
                }
                std::cerr << endl;
#endif // ATTLIST_PSEQ_DEBUG
                for (uint i = 0; i < tv.size(); ++i) {
                    const SpAttlistParser::Token& t = tv[i];
                    switch (t.type) {
                        case SpAttlistParser::NAME: {
                            aditer = adefs.find(t.value);
                            if (aditer != adefs.end())
                                attachTo->appendAttrDef
                                    (aditer->second.pointer());
                            continue;
                        }
                        case SpAttlistParser::ERS: {
                            pa = attachTo;
                            agiter = agset_.find(t.value);
                            if (agiter == agset_.end()) {
                                attachTo = new AttrDeclNode::AttrDefGroup;
                                attachTo->setName(t.value);
                                agset_[t.value] = attachTo;
                                attachStack.push_back(attachTo);
                                pa->appendAttrDef(attachTo);
                            } else {
                                attachTo->appendAttrDef
                                    (agiter->second.pointer());
                                while (i < tv.size() &&
                                    (tv[i].type != SpAttlistParser::ERE ||
                                    tv[i].value != t.value))
                                        ++i;
                            }
                            continue;
                        }
                        case SpAttlistParser::ERE: {
                            attachStack.pop_back();
                            attachTo = attachStack.back();
                            continue;
                        }
                        default:
                            break;
                    }
                }
                while (iter.type() != Sp::Markup::entityEnd
                    && iter.valid())
                        iter.advance();
                attachStack.pop_back();
                attachTo = attachStack.back();
                break;
            }
            case Sp::Markup::delimiter: {
                // skip braces (groups)
                if (iter.delimGeneral() == Sp::Syntax::dGRPO)
                    while (iter.type() != Sp::Markup::delimiter
                        && iter.delimGeneral() != Sp::Syntax::dGRPC
                        && iter.valid())
                            iter.advance();
                break;
            }
            case Sp::Markup::name: {
                if (!hadElemName) {
                    hadElemName = true;
                    break;
                }
                String sv;
                SpUtils::assignString(sv, iter.charsPointer(),
                                      iter.charsLength());
                aditer = adefs.find(sv);
                if (aditer != adefs.end())
                    attachTo->appendAttrDef(aditer->second.pointer());
                break;
            }
            default:
                ;
        }
    }
    n_->appendChild(adn);
    delete event;
}

void SpEventHandler::sSep(Sp::SSepEvent* event)
{
    if (skipNextSSep_) {
        skipNextSSep_ = 0;
        delete event;
        return;
    }
    if (n_ == grove_->prolog()) {
        SKIP_EXS(event);
    } else {
        if (!preserveSSep_) {
            delete event;
            return;
        }
    }
    String sepdata;
    SpUtils::assignString(sepdata, event->data(),
        event->dataLength());
    DBG(GROVE.BUILDER) << "SSep: <" << sepdata << ">\n";
    SSepNode* ssn = new SSepNode;
    setNodeExt(ssn, event->location());
    ssn->setData(sepdata);
    n_->appendChild(ssn);
    delete event;
}

//////////////////////////////////////////////////////////////////////////
//
// Miscellanea
//
//////////////////////////////////////////////////////////////////////////

/*! Restores original ASCII-representation of markup, preserving
    entity references.
 */
void SpEventHandler::restoreMarkup(String& os,
                                   const Sp::Markup& markup,
                                   String* cs)
{
    Sp::MarkupIter iter(markup);
    os.truncate(0);
    ulong entityLevel = 0;
    for (; iter.valid(); iter.advance()) {
        switch (iter.type()) {
            case Sp::Markup::entityStart: {
                ++entityLevel;
                os += '%';
                os += SpUtils::makeString(iter.entityOrigin()->
                    entity()->name());
                os += ';';
                break;
            }
            case Sp::Markup::entityEnd: {
                --entityLevel;
                break;
            }
            case Sp::Markup::literal: {
                if (entityLevel > 0)
                    break;
                Sp::TextIter ti(iter.text());
                Sp::TextItem::Type type;
                const Sp::Char* s;
                const Sp::Location* loc;
                size_t len = 0;
                bool skiptext = false;
                int litidx = -1;
                while(ti.next(type, s, len, loc)) {
                    switch(type) {
                        case Sp::TextItem::nonSgml:
                            len = 1;
                        case Sp::TextItem::sdata:
                        case Sp::TextItem::data:
                        case Sp::TextItem::cdata: {
                            if (skiptext)
                                break;
                            String osv;
                            if (len == 1) {
                                Char c(*s);
                                if (!c.isLetterOrNumber() && !c.isPunct())
                                    osv = "&#" + String::number((unsigned)(*s))
                                               + ";";
                                    else
                                        SpUtils::assignString(osv, s, 1);
                            } else
                                SpUtils::assignString(osv, s, len);
                            os += osv;
                            break;
                        }
                        case Sp::TextItem::entityStart: {
                            if (skiptext)
                                break;
                            skiptext = true;
                            os += '%';
                            os += SpUtils::makeString
                                (loc->origin()->entity()->name());
                            os += ';';
                            break;
                        }
                        case Sp::TextItem::entityEnd: {
                            skiptext = false;
                            break;
                        }
                        case Sp::TextItem::startDelim: {
                            litidx = os.length();
                            os += '"';
                            break;
                        }
                        case Sp::TextItem::endDelim: {
                            litidx = -1;
                            os += '"';
                            break;
                        }
                        case Sp::TextItem::endDelimA: {
                            if (litidx >= 0) {
                                os.ref(litidx) = '\'';
                                litidx = -1;
                            }
                            os += '\'';
                            break;
                        }
                        default:
                            ;
                    }
                }
                break;
            }
            case Sp::Markup::comment: {
                if (entityLevel > 0)
                    break;
                String sv, osv;
                SpUtils::assignString(sv, iter.charsPointer(),
                    iter.charsLength());
                osv += "--";
                osv += sv;
                osv += "--";
                if (cs)
                    *cs = osv;
                os += osv;
                break;
            }
            case Sp::Markup::name:
            case Sp::Markup::nameToken:
            case Sp::Markup::reservedName:
            case Sp::Markup::sdReservedName:
            case Sp::Markup::number:
            case Sp::Markup::attributeValue:
            case Sp::Markup::s:
            case Sp::Markup::shortref: {
                if (entityLevel > 0)
                    break;
                String sv;
                SpUtils::assignString(sv, iter.charsPointer(),
                                      iter.charsLength());
                os += sv;
                break;
            }
            case Sp::Markup::delimiter: {
                if (entityLevel > 0)
                    break;
                os += SpUtils::makeString(syntax_->
                                          delimGeneral(iter.delimGeneral()));
                break;
            }
            default:
                break;
        }
    }
}

/*! Build attribute nodes for an element from SP' AttributeList.
 */
void SpEventHandler::setElementAttributes(Element* n,
                                          const Sp::AttributeList& alist,
                                          const Sp::Location& loc)
{
    const Sp::Text* text;
    const Sp::StringC* string;
    std::list<EntityReferenceStart*> esstack;
    ulong i = 0;
    ulong eskip = 0;
    RT_ASSERT(!n->attrs().firstChild());
    for (i = 0; i < alist.size(); ++i) {
        const Sp::AttributeValue* v = alist.value(i);
        if (0 == v)
            continue;
        Sp::AttributeValue::Type atype = v->info(text, string);
        if (atype == Sp::AttributeValue::implied)
            continue;  // discard implied attributes
        AttrPtr a = new Attr(SpUtils::makeString(alist.name(i)));
        n->attrs().appendChild(a.pointer());
        if (lineInfo_)
            setNodeExt(a.pointer(), loc);
        switch (atype) {
            case Sp::AttributeValue::implied:
                break; // can't happen really

            case Sp::AttributeValue::tokenized: {
                a->setValueNoNotify(SpUtils::makeString(*string));
                a->setType(Attr::TOKENIZED);
                if (alist.id(i))
                    a->setIdClass(Attr::IS_ID);
                else if (alist.idref(i))
                    a->setIdClass(Attr::IS_IDREF);
                const Sp::AttributeSemantics* sem = alist.semantics(i);
                if (sem) {
                    Sp::ConstPtr<Sp::Notation> notation = sem->notation();
                    if (!notation.isNull()) {
                        setNotationOnAttribute(a.pointer(), *notation);
                    }
                    else {
                        //for (ulong e = 0; e < sem->nEntities(); ++e)
                        //  setEntityOnAttribute(a.pointer(), *sem->entity(e));
                    }
                }
                break;
            }
            case Sp::AttributeValue::cdata: {
                a->setType(Attr::CDATA);
                Sp::TextItem::Type type;
                const Sp::Char* s;
                const Sp::Location* loc;
                size_t len;
                Sp::TextIter iter(*text);
                Text* t;
                while(iter.next(type, s, len, loc)) {
                    switch (type) {
                        case Sp::TextItem::nonSgml:
                            len = 1; // fall through
                        case Sp::TextItem::sdata:
                        case Sp::TextItem::data:
                        case Sp::TextItem::cdata: {
                            t = new Text;
                            String cdata;
                            SpUtils::assignString(cdata, s, len);
                            t->setData(cdata);
                            DBG(GROVE.BUILDER) << "attr cdata: "
                                << cdata << std::endl;
                            //const Sp::Entity* se = loc->origin()->entity();
                            //if (se && !se->asExternalEntity())
                            //    simpleEntityRef(*se, t, a.pointer());
                            //else
                                a->appendChild(t);
                            break;
                        }
                        case Sp::TextItem::entityStart: {
                            if (!want_esect_)
                                break;
                            const Sp::Entity* se = loc->origin()->entity();
                            String en = SpUtils::makeString(se->name());
                            DBG(GROVE.BUILDER) << "attr entity start: "
                                << en << std::endl;
                            if (isBuiltinEntity(en)) {
                                eskip++;
                                break;
                            }
                            EntityDecl* decl = getDecl(false, se->name());
                            RT_ASSERT(0 != decl);
                            EntityReferenceStart* es =
                                new EntityReferenceStart;
                            ce_->appendSect(es);
                            ce_ = es;
                            es->setDecl(decl);
                            a->appendChild(es);
                            document_->ert()->addEntityRef(es);
                            esstack.push_back(es);
                            break;
                        }
                        case Sp::TextItem::entityEnd: {
                            if(!want_esect_)
                                break;
                            if (eskip) {
                                eskip--;
                                break;
                            }
                            DBG(GROVE.BUILDER) << "attr entity end\n";
                            EntityReferenceEnd* ee =
                                new EntityReferenceEnd;
                            EntityReferenceStart* es = esstack.back();
                            esstack.pop_back();
                            ee->setSectStart(es);
                            es->setSectEnd(ee);
                            ce_ = static_cast<EntityReferenceStart*>
                                (ce_->getSectParent());
                            a->appendChild(ee);
                            break;
                        }
                        default:
                            break;
                    }
                }
                a->build();
                if (isXml_) {
                    String prefix;
                    if (XmlNs::parseXmlNsAtt(a->name(), prefix)) {
                        n->addToPrefixMap(prefix, a->value())->
                            setDefaulted(!alist.specified(i));
                        a->remove();
                        a = 0;
                    }
                }
            }
        }
        if (!a.isNull()) {
            if (alist.specified(i))
                a->setDefaulted(Attr::SPECIFIED);
            else
                a->setDefaulted(Attr::DEFAULTED);
        }
    }
}

/*! Add entity declaration to grove, given SP entity class and it's
    entityOrigin.
 */
EntityDecl* SpEventHandler::addEntity(const Sp::Entity& entity,
                                      const Sp::EntityDecl* entityOrigin)
{
    DBG(GROVE.BUILDER) << "ENTITY DECL: " <<
        SpUtils::makeString(entity.name()) << std::endl;

    if (entityOrigin)
        DBG(GROVE.BUILDER) << " -> origin: "
            << SpUtils::makeString(entityOrigin->name()) << std::endl;

    RefCntPtr<EntityDecl> e;
    InternalEntityDecl* iep = 0;
    ExternalEntityDecl* eep = 0;
    if (0 != entity.asExternalEntity())
        eep = new ExternalEntityDecl;
    else
        iep = new InternalEntityDecl;
    // static_casts are required by MSVC by unknown reason
    e = iep ? static_cast<EntityDecl*>(iep) : static_cast<EntityDecl*>(eep);
    e->setEntityName(SpUtils::makeString(entity.name()));
    if (0 != entityOrigin) {
        bool isparam = (entityOrigin->declType() == Sp::EntityDecl::parameterEntity ||
                        entityOrigin->declType() == Sp::EntityDecl::doctype);
        e->setDeclOrigin(EntityDecl::dtd);
        RefCntPtr<EntityDecl> oe;
        if (entityOrigin->declType() == Sp::EntityDecl::doctype)
            oe = const_cast<ExternalEntityDecl*>(grove_->doctypeEntity());
        else
            oe = getDecl(isparam, entityOrigin->name());
        RT_ASSERT(0 != oe.pointer());
        e->setOriginEntityDecl(oe.pointer());
        // TODO: Set notation here !!!!!!
    }
    e->setDeclOrigin(entityOrigin ? EntityDecl::dtd : EntityDecl::prolog);
    switch (entity.declType()) {
        case Sp::EntityDecl::parameterEntity:
            e->setDeclType(iep ? EntityDecl::internalParameterEntity :
                EntityDecl::externalParameterEntity);
            e->setDataType(EntityDecl::sgml);
            break;

        case Sp::EntityDecl::generalEntity:
        case Sp::EntityDecl::sgml:
            e->setDeclType(iep ? EntityDecl::internalGeneralEntity :
                EntityDecl::externalGeneralEntity);
            switch (entity.dataType()) {
                case Sp::EntityDecl::sgmlText:
                    e->setDataType(EntityDecl::sgml);
                    break;
                case Sp::EntityDecl::pi:
                    e->setDeclType(EntityDecl::pi);
                    e->setDataType(EntityDecl::cdata);
                    break;
                case Sp::EntityDecl::sdata:
                    e->setDataType(EntityDecl::sdata);
                    break;
                case Sp::EntityDecl::cdata:
                    e->setDataType(EntityDecl::cdata);
                    break;
                case Sp::EntityDecl::ndata:
                    e->setDataType(EntityDecl::ndata);
                    break;
                case Sp::EntityDecl::subdoc:
                    e->setDataType(EntityDecl::sgml);
                    e->setDeclType(EntityDecl::subdoc);
                    break;

                default:
                    RT_MSG_ABORT("invalid entity type in grovebuilder");
                    break; // keep compilers happy
            }
            break;
        case Sp::EntityDecl::doctype:
            e->setDeclType(EntityDecl::doctype);
            e->setDataType(EntityDecl::sgml);
            break;
        case Sp::EntityDecl::linktype:
        case Sp::EntityDecl::notation:
            break;
    }
    if (eep) {
        setExternalId(&eep->externalId(),
            entity.asExternalEntity()->externalId());
    }
    else {
        iep->setContent
            (SpUtils::makeString(entity.asInternalEntity()->string()));
    }
    if (entity.declType() == Sp::EntityDecl::parameterEntity)
        grove_->parameterEntityDecls().insertDecl(e.pointer(), false);
    else if (entity.declType() == Sp::EntityDecl::doctype)
        grove_->setDoctypeEntity(eep);
    else
        grove_->entityDecls()->insertDecl(e.pointer(), false);
    return e.pointer();
}

/*! Insert simple entity reference into grove, given SP entity \a spe
    and parent node \a n. If \a in is non-null, then insert this single
    node between EntityReferenceStart and EntityReferenceEnd.
 */
void SpEventHandler::simpleEntityRef(const Sp::Entity& spe, Node* in, Node* n)
{
    if (want_esect_) {
        EntityDecl* decl;
        if (spe.declType() == Sp::EntityDecl::doctype)
            decl = const_cast<ExternalEntityDecl*>(grove_->doctypeEntity());
        else
            decl = getDecl(false, spe.name());
        RT_ASSERT(0 != decl);
        EntityReferenceStart* es = new EntityReferenceStart;
        EntityReferenceEnd*   ee = new EntityReferenceEnd;
        ce_->appendSect(es);
        es->setSectEnd(ee);
        ee->setSectStart(es);
        es->setDecl(decl);
        n->appendChild(es);
        document_->ert()->addEntityRef(es);
        if (0 != in)
            n->appendChild(in);
        n->appendChild(ee);
    } else if (0 != in)
        n->appendChild(in);
}

/*! Get entity declaration from the existing grove, given entity name
    in SP encoding.
 */
EntityDecl* SpEventHandler::getDecl(bool isParam, const Sp::StringC& sn)
{
    if (!isParam)
        return grove_->entityDecls()->lookupDecl(SpUtils::makeString(sn));
    return grove_->parameterEntityDecls().lookupDecl
        (SpUtils::makeString(sn));
}

/*! Check if there is SD-declared entity in existing grove with the same
    name with SP entity \a spe.
 */
bool SpEventHandler::isNotSDE(const Sp::Entity* spe)
{
    const EntityDecl* decl =
        grove_->entityDecls()->lookupDecl(SpUtils::makeString(spe->name()));
    if (!decl)
        return true;
    if (isBuiltinEntity(decl->name()))
        return false;
    return (decl->declOrigin() != EntityDecl::sd);
}

/*! Convert external ID from SP format into grove format.
 */
void SpEventHandler::setExternalId(ExternalId* extid,
                                   const Sp::ExternalId& se)
{
    const Sp::StringC* str = se.systemIdString();
    if (str)
        extid->setSysid(SpUtils::makeString(*str));
    str = se.publicIdString();
    if (str)
        extid->pubid().parse(SpUtils::makeString(*str));
}

/*! Set notaton for an attribute.
 *  TODO: implement it! it is rare, but ...
 */
void SpEventHandler::setNotationOnAttribute(Attr*, const Sp::Notation&)
{
}

/*! For tokenized attributes of type ENTITY, make simple entity reference(s)
    from an attribute.
 */
void SpEventHandler::setEntityOnAttribute(Attr* a, const Sp::Entity& spe)
{
    simpleEntityRef(spe, 0, a);
}

#ifndef XML_DECL_FILE
extern const char SpXmlDecl__[];
#endif // XML_DECL_FILE

#if defined(_MSC_VER)
# define PUTENV _putenv
#else
# define PUTENV putenv
#endif

#define SP_ENCODING_VAR    "SP_ENCODING="
#define SP_SEARCH_PATH_VAR "SGML_SEARCH_PATH="
#define SP_BCTF_VAR "SP_BCTF="

static void unset_env(char* var, unsigned len)
{
#if defined(__FreeBSD__)
    char tmp(var[len - 1]);
    var[len - 1] = '\0';
    unsetenv(var);
    var[len - 1] = tmp;
#else
    (void) var;
    (void) len;
#endif
}

GrovePtr SpGroveBuilder::buildGrove(const COMMON_NS::Vector<String>& files,
                                    const Common::String& prefSysid,
                                    bool dtdValidate)
{
    using namespace std;

// TODO not reentrant
    static char varbuf[128] = SP_ENCODING_VAR;
    static const int name_sz = sizeof(SP_ENCODING_VAR) - 1;
    varbuf[name_sz] = 0;
    unset_env(varbuf, name_sz);
    // this is ugly, but SP has no other interface to set this :/
    String enc = Encodings::encodingName(encoding_).lower();
    bool is_euc = ("euc-jp" == enc);
    bool is_ucs2 = ("ucs-2" == enc || "iso-10646-ucs-2" == enc
                    || "unicode" == enc || "utf-16" == enc);
    bool is_jis = ("sjis" == enc || "shift_jis" == enc);
    if (is_euc || is_ucs2 || is_jis)
        (void) strcat(varbuf, "xml");
    else
        (void) strcat(varbuf, enc.utf8().c_str());
    PUTENV(varbuf);

    static char spath[4096] = SP_SEARCH_PATH_VAR;
    static const int spath_nsz = sizeof(SP_SEARCH_PATH_VAR) - 1;
    spath[spath_nsz] = 0;
    unset_env(spath, spath_nsz);
    (void) strcat(spath, spath_.utf8().c_str());
    PUTENV(spath);

//////////////
/* TODO: right 2 byte encoding
    String enc = Encodings::encodingName(encoding_).lower();
    bool is_euc = ("euc-jp" == enc);
    bool is_ucs2 = ("ucs-2" == enc || "iso-10646-ucs-2" == enc || "unicode" == enc);
    bool is_jis = ("sjis" == enc || "shift_jis" == enc);
    static char bctf[128] = SP_BCTF_VAR;
    static const int bctf_sz = sizeof(SP_BCTF_VAR) - 1;
    bctf[spath_nsz] = 0;
#if defined(WIN32)
    PUTENV("SP_BCTF=");
#else
    unsetenv(bctf);
#endif

    if (is_euc || is_ucs2 || is_jis) {
        if (is_euc)
            (void) strcat(bctf, "euc");
        else if (is_jis)
            (void) strcat(bctf, "sjis");
        else
            (void) strcat(bctf, "fixed-2");
        PUTENV(bctf);
    }
*/
    PUTENV("SP_CHARSET_FIXED=1");
    PUTENV("SGML_CATALOG_FILES=");
    PUTENV("SP_USE_DOCUMENT_CATALOG=0");

    GrovePtr grove(new Grove);
    grove->setGroveBuilder(this);
    if (!grove->groveBuilder()->rootGrove())
        grove->groveBuilder()->setRootGrove(grove.pointer());
    bool idProcEnabled = !(flags() & noIdProcessing);
    if (idProcEnabled) {
        grove->setIdManager(new IdManager);
        if (scopeDefs_ && matchPatternFactory_) {
            grove->idManager()->setMatchPatternFactory(
                matchPatternFactory_->copy());
            grove->idManager()->setScopeDefs(scopeDefs_.pointer(),
                                             nsMap_.pointer());
        }
    }
    OwnerPtr<Sp::ExternalIdResolver> r(new SpExtIdResolver(*getCatalogManager(),
                                                           getMessenger()));
    SpGroveApp app(grove.pointer(), r.pointer());
    //
    // The following mess is necessary because JJC code requires
    // that all data passed to processOption must be still in scope
    // after calling processOption ...
    //
    if (!dtdValidate)
        app.processOption('w', "no-valid");

    if (!(flags() & SGML)) {
        app.processOption('w', "xml");
        app.processOption('w', "no-explicit-sgml-decl");
    }
    app.processOption('e', 0);
    app.processOption('g', 0);
    if (!(flags() & noProlog))
        app.options().eventsWanted.addPrologMarkup();
    if (!(flags() & noComments))
        app.options().eventsWanted.addCommentDecls();
    if (!(flags() & noMarkedSections))
        app.options().eventsWanted.addMarkedSections();
    app.options().eventsWanted.addInstanceMarkup();

    size_t argc = files.size();
    vector<char*> vargs;
    vargs.reserve(argc + 1);

    if (!(flags() & SGML)) {
#ifdef XML_DECL_FILE
        vargs.push_back(strdup_new(XML_DECL_FILE));
#else
        vargs.push_back(const_cast<char*>(SpXmlDecl__));
#endif // XML_DECL_FILE
    }
    for (size_t i = 0; i < argc; ++i) {
        const String& fpath = files[i];
        if (fpath.left(9) == "<LITERAL>") {
            nstring id("string://");
            id += fpath.right(fpath.size() - sizeof("<LITERAL>") - 1).utf8();
            vargs.push_back(strdup_new(id.c_str()));
        }
        else
            vargs.push_back(strdup_new(fpath.utf8().c_str()));
    }
    app.run(vargs.size(), &vargs[0], prefSysid);

    if (idProcEnabled) 
        grove->idManager()->enable(grove->document());

    if (getEncoding() == Encodings::XML)
        grove->groveBuilder()->setEncoding(grove->topDecl()->encoding());
    else
        grove->topDecl()->setEncoding(getEncoding());

    for (vector<char*>::iterator it = vargs.begin(); it != vargs.end(); ++it) {
#ifndef XML_DECL_FILE
        if (SpXmlDecl__ != *it)
#endif
            delete[] *it;
    }
    set_lock_status(grove.pointer());
    return grove;
}

#if defined(_WIN32)
# define snprintf _snprintf
#endif

GrovePtr SpGroveBuilder::buildGrove(const String& s, 
                                    const String& prefSysid,
                                    bool dtdValidate)
{
    Vector<String> args;
    char buf[32];
    int len = snprintf(&buf[0], sizeof(buf), "mem://%p,%x",
                       &s, sizeof(Char) * s.size());
    args.push_back(from_latin1(&buf[0], len));
//    Encodings::Encoding save_enc = getEncoding();
//    setEncoding(Encodings::UCS_2);
    GrovePtr grove_ptr(buildGrove(args, prefSysid, dtdValidate));
//    setEncoding(save_enc);
    return grove_ptr;
}

GroveBuilder* SpGroveBuilder::copy() const
{
    SpGroveBuilder* gb = new SpGroveBuilder;
    gb->GroveBuilder::operator=(*this);
    return gb;
}

GROVE_NAMESPACE_END
