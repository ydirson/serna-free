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
#include "grove/Grove.h"
#include "grove/IdManager.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"
#include "grove/PrologNodes.h"
#include "grove/EntityReferenceTable.h"
#include "grove/GroveBuilder.h"
#include "grove/GroveVisitor.h"
#include "grove/EntityDeclSet.h"
#include "grove/impl/gsutils.h"
#include "grove/grove_trace.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "dav/DavManager.h"
#include "GroveMessages.h"
#include <map>

using namespace Common;

namespace GroveLib {

Document* Grove::document() const
{
    return document_.pointer();
}

DocumentProlog* Grove::prolog() const
{
    return prolog_.pointer();
}

void Grove::setDoctypeEntity(ExternalEntityDecl* e)
{
    dtEntity_ = e;
}

void Grove::setTopSysid(const String& sysid)
{
    top_decl_->setSysid(sysid);
}

const String& Grove::topSysid() const
{
    return top_decl_->sysid();
}

ExternalEntityDecl* Grove::topDecl() const
{
    return top_decl_.pointer();
}

void Grove::setGroveBuilder(GroveBuilder* b)
{
    builder_ = b;
}

void Grove::setIdManager(IdManager* idm)
{
    idManager_ = idm;
}

DocumentHeading* Grove::heading() const
{
    return heading_.pointer();
}

static void ensure_sections(GroveSectionRoot* gsr)
{
    for (uint i = 0; i < Node::MAX_SECTION_ID; ++i)
        gsr->ensureSectionRoot(i);
}

Grove::Grove()
    : registerVisitors_(true)
{
    builder_     = new GroveBuilder;
    prolog_      = new DocumentProlog;
    heading_     = new DocumentHeading;
    ensure_sections(prolog_.pointer());
    ensure_sections(heading_.pointer());
    prolog_->setGrove(this);
    perTable_    = new EntityReferenceTable;
    perDecls_    = new EntityDeclSet;
    notations_   = new EntityDeclSet;

    // define top document entity
    top_decl_ = new ExternalEntityDecl;
    top_decl_->setDeclType(EntityDecl::document);
    top_decl_->setDataType(EntityDecl::sgml);
    top_decl_->setDeclOrigin(EntityDecl::special);
    top_decl_->setEntityName(String("#DOCUMENT"));

    // create "document" element
    document_ = new Document;
    document_->setGrove(this);
    ensure_sections(document_.pointer());
    document_->ers()->setDecl(top_decl_.pointer());

    entityDecls_ = new EntityDeclSet;
    entityDecls()->insertDecl(top_decl_.pointer());
}

GROVE_EXPIMP void release_locks(Grove* g)
{
    // apg: http:// is because we need dispach unlock to webdav plugin
    if (g->groveBuilder()->flags() & 
        (GroveBuilder::doLocks|GroveBuilder::checkLocks)) 
            Dav::DavManager::instance().lock(Url("http://"),
                Dav::DAV_UNLOCK, (intptr_t) g);
}

Grove::~Grove()
{
    document_->setGrove(0);
    prolog_->setGrove(0);
    heading_->setGrove(0);
    release_locks(this);
}

static void save_content_and_prolog(GroveSaverContext& gsc)
{
    Grove* grove = gsc.grove();
    if (grove->topDecl()->dataType() == EntityDecl::cdata) { // text grove
        Node* n = grove->document()->firstChild();
        for (; n; n = n->nextSibling())
            if (n->nodeType() == Node::TEXT_NODE)
                break;
        if (0 == n)
            return;
        Node* ps = n->prevSibling();
        if (ps && ps->nodeType() == Node::ENTITY_REF_START_NODE) {
            const EntityReferenceStart* ers =
                static_cast<const EntityReferenceStart*>(ps);
            if (ers->entityDecl()->declType() != EntityDecl::xinclude)
                return;
            const XincludeDecl* xd =
                static_cast<const XincludeDecl*>(ers->entityDecl());
            if (xd->parentGrove()) {
                const EntityReferenceTable::ErtEntry* ee =
                    xd->parentGrove()->document()->ert()->lookup(xd);
                if (ee && ee->node(0)->nextSibling() &&
                    ee->node(0)->nextSibling()->nodeType() == Node::TEXT_NODE)
                        n = ee->node(0)->nextSibling();
            }
        }
        if (0 == n)
            return;
        gsc.os() << static_cast<const Text*>(n)->data();
        return;
    }
    if (gsc.flags() & Grove::GS_SAVE_PROLOG) {
        grove->entityDecls()->rebuildInternalEntityContents(grove);
        gsc.xmlHeader(grove->topDecl());
        grove->heading()->saveAsXml(gsc, 0);
        grove->prolog()->saveAsXml(gsc, 0);
    }
    if (gsc.flags() & Grove::GS_SAVE_CONTENT) {
        if (!(gsc.flags() & Grove::GS_SAVE_PROLOG))
            gsc.xmlHeader(grove->topDecl());
        grove->document()->saveAsXml(gsc, 0);
        gsc.os() << CR_ENDL;
    }
}

// Grove saving

bool Grove::saveAsXmlFile(int flags,
                          const StripInfo* si,
                          const Common::String& saveAs)
{
    Grove* msgGrove = this;
    if (parent())
        msgGrove = parent();
    Common::MessageStream rstream(GroveMessages::getFacility(),
        msgGrove->groveBuilder()->getMessenger());
    DDBG << "Grove: saveAsXmlFile: " << topSysid() << std::endl;
    if ((flags & GS_FORCE_SAVE) || !topDecl()->isReadOnly()) {
        GroveSaverContext gsc(si, this, flags);
        if (!gsc.setOutputUrl(saveAs)) {
            if (!parent())
                rstream << GroveMessages::cantSaveDoc
                    << (saveAs.isEmpty() ? topSysid() : saveAs)
                    << Dav::DavManager::instance().lastError()
                    << Message::L_ERROR;
            return false;
        }
        save_content_and_prolog(gsc);
        if (!gsc.close()) {
            if (!parent())
                rstream << GroveMessages::cantSaveDoc
                    << (saveAs.isEmpty() ? topSysid() : saveAs)
                    << Dav::DavManager::instance().lastError()
                    << Message::L_ERROR;
            return false;
        }
    }
    bool ok = true;
    if ((flags & GS_SAVE_ENTITIES) && !(flags & GS_EXPAND_ENTITIES)) {
        EntityReferenceTable::ErtTable::iterator ertIter =
            document()->ert()->begin();
        for (; ertIter != document()->ert()->end(); ++ertIter) {
            const ExternalEntityDecl* eed =
                (*ertIter)->decl()->asConstExternalEntityDecl();
            if (0 == eed || eed->declType() == EntityDecl::document)
                continue;
            if (!eed->isContentModified() && !(flags & GS_FORCE_SAVE))
                continue;
            if (eed->sysid().isEmpty())
                continue;
            String path((*ertIter)->node(0)->xmlBase());
            DDBG << "Saving: "  << eed->name() << ", "
                << abr(eed->sysid()) << ' ' << abr(path)
                << " encoding=" << Encodings::encodingName(eed->encoding())
                << std::endl;
            GroveSaverContext entity_gsc(si, this, flags);
            if (!entity_gsc.setOutputUrl(path, eed->encoding())) {
                rstream << GroveMessages::cantSaveEntity << path
                    << Dav::DavManager::instance().lastError()
                    << Message::L_ERROR;
                ok = false;
                continue;
            }
            (*ertIter)->node(0)->saveAsXml(entity_gsc,
                GroveSaverContext::SAVE_ECONTENT);
            if (flags & GS_RESET_MODFLAGS)
                (*ertIter)->node(0)->entityDecl()->setContentModified(false);
        }
    }
    if (flags & GS_RECURSIVE) {
        for (Grove* g = firstChild(); g; g = g->nextSibling()) {
            EntityReferenceTable* ert = g->document()->ert();
            EntityReferenceTable::ErtTable::iterator ertIter = ert->begin();
            EntityDecl* decl = 0;

            for (; ertIter != ert->end(); ++ertIter) {
                decl = (*ertIter)->decl();
                if (decl->isContentModified() || decl->isDeclModified())
                    break;
            }
            if (ertIter != ert->end()) {
                DDBG << "Subgrove modified: " << g->topSysid()
                    << ", by entity: " << decl->name() << std::endl;
                bool subok = g->saveAsXmlFile(flags, si);
                ok &= subok;
                if (!subok)
                    rstream << GroveMessages::cantSaveSubdoc << g->topSysid()
                        << Dav::DavManager::instance().lastError()
                        << Message::L_ERROR;
                if (flags & GS_RESET_MODFLAGS) {
                    decl->setContentModified(false);
                    decl->setDeclModified(false);
                }
            }
        }
    }
    return ok;
}

bool Grove::saveAsXmlString(Common::String& saveTo,
                            int flags,
                            const StripInfo* si)
{
    GroveSaverContext gsc(si, this, flags);
    gsc.setOutputString();
    save_content_and_prolog(gsc);
    saveTo = gsc.getSavedString();
    return true;
}

/////////////////////////////////////////////////////////////////

typedef std::map<String, bool> ReadonlyCheckMap;

// returns TRUE if resurce must be read-only
static bool do_lock_resource(const String& url, const Grove* grove)
{
    if (grove->groveBuilder()->flags() & GroveBuilder::doLocks)
        return Dav::DavManager::instance().lock(url, Dav::DAV_LOCK, 
            (intptr_t) grove) != Dav::DAV_RESULT_OK;
    if (grove->groveBuilder()->flags() & GroveBuilder::checkLocks)
        return Dav::DavManager::instance().lock(url, Dav::DAV_CHECK_LOCK, 
            (intptr_t) grove) == Dav::DAV_RESULT_LOCKED;
    return false;
}
    
static void set_lock_status(Grove* g, ReadonlyCheckMap& check_map)
{
    g->topDecl()->setReadOnly(do_lock_resource(g->topSysid(), g));
    EntityReferenceTable* ert = g->document()->ert();
    EntityReferenceTable::iterator it = ert->begin();
    for (; it != ert->end(); ++it) {
        String url;
        const EntityDecl* ed = (*it)->decl();
        switch (ed->declType()) {
            case EntityDecl::externalGeneralEntity:
                url = ed->asConstExternalEntityDecl()->entityPath(g);
                break;
            case EntityDecl::xinclude:
                if (static_cast<const XincludeDecl*>(ed)->isFallback())
                    continue;
                url = (*it)->node(0)->xmlBase();
                break;
            default:
                continue;
        }
        ReadonlyCheckMap::iterator mit = check_map.find(url);
        if (mit != check_map.end())
            (*it)->decl()->setReadOnly(mit->second);
        bool is_readonly = do_lock_resource(url, g);
        check_map[url] = is_readonly;
        (*it)->decl()->setReadOnly(is_readonly);
    }            
}

GROVE_EXPIMP void set_lock_status(Grove* grove)
{
    if (!(grove->groveBuilder()->flags() & 
        (GroveBuilder::doLocks|GroveBuilder::checkLocks)))
            return;
    ReadonlyCheckMap check_map;
    set_lock_status(grove, check_map);
    for (Grove* g = grove->firstChild(); g; g = g->nextSibling())
        set_lock_status(g, check_map);
}

} // namespace GroveLib
