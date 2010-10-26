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
#include "structeditor/se_defs.h"
#include "structeditor/StructEditor.h"
#include "structeditor/StructDocument.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/impl/SchemaCommandMaker.h"
#include "structeditor/impl/entity_utils.h"

#include "utils/Properties.h"
#include "utils/DocTemplate.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "utils/file_utils.h"
#include "utils/message_utils.h"
#include "utils/DocSrcInfo.h"
#include "utils/SernaMessages.h"
#include "utils/SernaCatMgr.h"
#include "common/PropertyTreeEventData.h"
#include "utils/ElementHelp.h"

#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/ElementMatcher.h"
#include "grove/SectionSyncher.h"
#include "grove/Origin.h"
#include "grove/udata.h"
#include "grove/xinclude.h"
#include "spgrovebuilder/SpGroveBuilder.h"

#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/XTreeIterator.h"
#include "common/DiffuseSearch.h"
#include "common/Url.h"
#include "common/StringCvt.h"
#include "common/ScopeGuard.h"

#include "catmgr/CatalogManager.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommands.h"

#include "docview/SernaDoc.h"
#include "docview/MessageView.h"
#include "docview/MessageTree.h"
#include "docview/DocumentStateEventData.h"

#include "xs/XsMessages.h"
#include "xs/Origin.h"
#include "xs/XsNodeExt.h"
#include "xs/Schema.h"
#include "xs/SchemaResource.h"
#include "xs/XsElement.h"
#include "xs/XsValidatorProvider.h"

#include "xslt/Engine.h"
#include "xslt/DocumentCache.h"
#include "dav/DavManager.h"

#include <qfile.h>
#include <qapplication.h>
#include <qfileinfo.h>

#include <map>
#include <iostream>

using namespace Common;
using namespace GroveLib;
using namespace FileUtils;
using namespace AttributesSpace;

static const int DEFAULT_LARGE_DOC_LIMIT = 100; // pages
static const int DEFAULT_ELEMS_PER_PAGE  = 30;  // elements per page

///////////////////////////////////////////////////////////////////////////

static Uri::Resource::ModtimeCheckState get_msstate(bool isEnd = false)
{
    bool state = config().root()->getSafeProperty(App::APP)->
        getSafeProperty(App::DISABLE_RESOURCE_MODTIME_CHECK)->getBool();
    if (state)
        return Uri::Resource::NEVER_CHECK;
    return isEnd ? Uri::Resource::ALWAYS_CHECK : Uri::Resource::SINGLE_CHECK;
}

bool StructEditor::newDocument(PropertyNode* dsi)
{
    Uri::ModtimeCheckGuard modtime_check_guard(get_msstate(),
                                               get_msstate(true));
    sernaDoc()->setDsi(dsi);
    createGrove(dsi);
    DocumentStateEventData evd(DocumentStateEventData::NEW_DOCUMENT_GROVE);
    sernaDoc()->stateChangeFactory().dispatchEvent(&evd);
    if (!initGrove(dsi))
        return false;
    sectionSyncher_ = new GroveLib::SectionSyncher(grove()->document());
    init();
    doTimeStamps(grove(), getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
    forceModified_ = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////

static void set_large_doc_subtree(ElementMatcher& em, Node* n)
{
    for (n = n->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE)
            continue;
        if (em.matchElement(static_cast<Element*>(n)))
            set_node_fold_state(n, true);
        set_large_doc_subtree(em, n);
    }
}

static void set_large_doc(Node* node, const PropertyNode* foldList)
{
    if (0 == foldList) {    // no fold list specified - fold first level
        for (node = node->firstChild(); node; node = node->nextSibling()) {
            if (node->nodeType() == Node::ELEMENT_NODE)
                set_node_fold_state(node, true);
        }
        return;
    }
    ElementMatcher em(foldList->getString());
    set_large_doc_subtree(em, node);
}

static void prepare_xinclude(StructEditor* se)
{
    EntityReferenceTable* ert = se->grove()->document()->ert();
    EntityReferenceTable::iterator it = ert->begin();
    for (; it != ert->end(); ++it) {
        if ((*it)->decl()->declType() != EntityDecl::xinclude)
            continue;
        const XincludeDecl* xd =
            static_cast<const XincludeDecl*>((*it)->decl());
        if (xd->isFallback()) {
            GroveLib::CompositeOrigin origin(
                new GroveLib::NodeOrigin((*it)->node(0)));
            if (xd->expr().isEmpty())
                se->messageStream() << SernaMessages::xincludeError
                    << xd->url() << origin;
            else
                se->messageStream() << SernaMessages::xincludeError2
                    << xd->url() << xd->expr() << origin;
        }
    }
}

bool StructEditor::openDocument(PropertyNode* dsi)
{
    using namespace XmlCatalogs;

    sernaDoc()->setDsi(dsi);
    const String& doc = dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    Url doc_url(doc);
    if (doc_url.isLocal()) {
        QFileInfo fi(String(doc_url.absolute()));
        if (!fi.isFile()) {
            sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Cannot open document"),
                tr("Cannot open document '%0', it is not a file").arg(doc),
                tr("&Ok"));
            return false;
        }
    }
    Uri::ModtimeCheckGuard modtime_check_guard(get_msstate(),
                                               get_msstate(true));
    setCatalogs(dsi);
    initElementHelp(dsi);
    PropertyTree res_info;
    if (Dav::DavManager::instance().getResourceInfo(doc, res_info.root()) !=
        Dav::DAV_RESULT_OK) {
            sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Cannot open document"),
                tr("Cannot open document (unable to get resource status): '%0'").arg(doc),
                tr("&Ok"));
            return false;
    }
    if (!useSchema(dsi))
        return false;
    GroveBuilder* gb = makeGroveBuilder(dsi);
    gb->setCatalogManager(uriMapper()->catalogManager());
    gb->setValidatorProvider(validatorProvider());
    PropertyNode* enc_property = dsi->makeDescendant(DocSrcInfo::ENCODING);
    if (!enc_property->getString().empty())
        gb->setEncoding(Encodings::encodingByName(enc_property->getString()));
    else
        gb->setEncoding(Encodings::XML);

    PropertyNode* cat_prop = config().root()->makeDescendant(XML_CATALOGS);
    if (!cat_prop->makeDescendant(USE_CATALOGS)->getBool())
        gb->setSearchPath(cat_prop->makeDescendant(DTD_PATH)->getString());

    gb->setMessenger(messageTree());
    set_grovebuilder_lock_flags(doc_url, gb);
    DDBG << "OpenDoc: url=" << doc << ", flags="
        << gb->flagsAsString() << "\n";

    String doc_info =
        dsi->getSafeProperty(DocSrcInfo::TEMPLATE_CATEGORY)->getString() +
        "/" + dsi->getSafeProperty(DocSrcInfo::TEMPLATE_NAME)->getString();
    if (doc_info.length() < 2)
        doc_info = tr("Parsing %1").arg(doc);
    else
        doc_info = tr("Parsing %1 (as %2)").arg(doc).arg(doc_info);
    sernaDoc()->showStageInfo(doc_info);

    grove_ = gb->buildGroveFromFile(doc, false); // DTD validation is OFF

    sernaDoc()->showStageInfo(tr("Initializing..."));

    if (grove_.isNull())
        throw StructEditorException(String(tr("Null grove built")));

    if (ExternalEntityDecl* eed = grove_->topDecl())
        enc_property->setString(Encodings::encodingName(eed->encoding()));

    bool hasXmlErrors = false;

    XTreeDocOrderIterator<MessageTreeNode> xit(messageTree());
    for (; xit.node(); ++xit) {
        if (xit.node()->getMessage() &&
            xit.node()->getMessage()->facility() == 1) { // 1 == SpGroveBuilder
                hasXmlErrors = true;
                break;
        }
    }
    doTimeStamps(grove(), getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
    if (hasXmlErrors) {
        getDsi()->makeDescendant("#has-xml-errors");
        return true;
    }
    if (0 == grove()->document()->documentElement())
        return false;

    if (!grove()->document()->documentElement()) {
        sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                             tr("Error while opening document"),
                             tr("No document element"), tr("&Ok"));
        grove_ = 0;
        return false;
    }
    PropertyNode* folding_rules = dsi->getProperty(DocSrcInfo::FOLDING_RULES);
    int largeDocSizeLimit = -1;
    String folding_mode;
    if (0 != folding_rules) {
        largeDocSizeLimit = folding_rules->
            getSafeProperty(DocSrcInfo::LARGE_DOC_LIMIT)->getInt();
        folding_mode = folding_rules->
            getSafeProperty(DocSrcInfo::FOLDING_MODE)->getString();
    }
    uint numOfElements = grove_->groveBuilder()->numberOfElements();
    for (Grove* g2 = grove_->firstChild(); g2; g2 = g2->nextSibling())
        numOfElements += g2->groveBuilder()->numberOfElements();
    DDBG << "Document size(elements): " << numOfElements
         << " limit(pages): " << largeDocSizeLimit << std::endl;
    if (0 < largeDocSizeLimit && 0 != folding_rules &&
        folding_mode != NOTR("never-fold")) {
        const PropertyNode* fold_if_large =
            folding_rules->getProperty(DocSrcInfo::FOLD_IF_LARGE);
        int elemsPerPage = folding_rules->getSafeProperty
            (DocSrcInfo::ELEMS_PER_PAGE)->getInt();
        if (elemsPerPage > 0) {
            int nPages = numOfElements/elemsPerPage;
            GroveLib::Node* docNode = grove_->document()->documentElement();
            DDBG << "Approximately " << nPages << " pages\n";
            if (nPages > largeDocSizeLimit) {
                if (folding_mode == NOTR("always-fold"))
                    set_large_doc(docNode, fold_if_large);
                else
                    switch (sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                        tr("Opening Large Document"),
                        tr("<qt><center><nobr><b>Document appears to be large"
                        ".</b></nobr><br/>"
                        "For faster operation you can open this document"
                        " with some of its content collapsed (you can always"
                        " expand it later)<br/><br/><nobr>Note: see "
                        "<b>Preferences->Folding</b></nobr> or the <b>Document "
                        "Template</b> for settings.</center></qt>"),
                        tr("&Open Collapsed"), tr("Open &Normally"),
                        tr("&Cancel"))) {
                            case 2:
                            default:
                                grove_ = 0;
                                return false;
                            case 0: {
                                DDBG << "Using large-doc mode\n";
                                set_large_doc(docNode, fold_if_large);
                                folding_rules->makeDescendant
                                    (DocSrcInfo::FOLDING_MODE)->
                                        setString(NOTR("always-fold"));
                                break;
                            }
                            case 1:
                                folding_rules->makeDescendant
                                    (DocSrcInfo::FOLDING_MODE)->
                                        setString(NOTR("never-fold"));
                                break;
                    }
            }
        }
    }
    processEntities();
    firstValidate();
    prepare_xinclude(this);
    sectionSyncher_ = new GroveLib::SectionSyncher(grove()->document());
    init();
    return true;
}

static void add_comment(GrovePtr& gptr)
{
    Node* np = gptr->heading();
    if (0 == np)
        return;

    static const char COMMENT_DATA[] =
        NOTR(" This document was created with Syntext Serna Free. ");

    String comment_str(COMMENT_DATA);

    for (np = np->firstChild(); 0 != np; np = np->nextSibling()) {
        if (Node::COMMENT_NODE != np->nodeType())
            continue;
        Comment* comment = static_cast<Comment*>(np);
        if (comment->comment() == comment_str)
            return;
    }

    CommentPtr comment(new Comment(comment_str));
    gptr->heading()->appendChild(comment.get());

    SSepNode* ssep = new SSepNode;
    NodePtr br(ssep);
    if (config().root()->getSafeProperty(App::APP_CRLF_LINE_BREAKS)->getBool())
        ssep->setData(String(1, '\r'));
    ssep->setData(String(1, '\n'));
    gptr->heading()->appendChild(ssep);
}

static void fixup_xinclude_paths(Grove* grove, const String& newPath)
{
    EntityReferenceTable::ErtTable::iterator it =
        grove->document()->ert()->begin();
    Url rel_path(Url(grove->topSysid()).relativePath(newPath));
    if (!rel_path.isRelative() || rel_path[Url::DIRPATH].isEmpty())
        return;
    String rp_str = rel_path[Url::DIRPATH] + '/';
    for (; it != grove->document()->ert()->end(); ++it) {
        if ((*it)->decl()->declType() != EntityDecl::xinclude)
            continue;
        XincludeDecl* xd = static_cast<XincludeDecl*>((*it)->decl());
        if (!Url(xd->url()).isRelative())
            continue;
        DDBG << "** FIXUP XD PATH, RELPATH=" << rp_str << std::endl;
        if (starts_with(xd->url(), rp_str)) // safe because of appended '/'
            xd->setUrl(xd->url().right(xd->url().length() - rp_str.length()));
        DDBG << "** FIXUP XD PATH, RESULT=" << xd->url() << std::endl;
    }
}

static void notify_save(bool* ok, SernaDoc* sd)
{
    if (*ok) {
        DocumentStateEventData saveEv(DocumentStateEventData::DOCUMENT_SAVED,
                                      sd);
        sd->stateChangeFactory().dispatchEvent(&saveEv);
    }
}

bool StructEditor::saveDocument(PropertyNode* dsi)
{
    DocumentStateEventData save_ev(DocumentStateEventData::ABOUT_TO_SAVE);
    sernaDoc()->stateChangeFactory().dispatchEvent(&save_ev);

    String need_reload = check_document_reload(getDsi());
    if (!need_reload.isEmpty() && sernaDoc()->showMessageBox(
        SernaDoc::MB_WARNING, "",
        tr("Another application has modified the following file(s):\n%1"
        "\n Do you wish to continue? ").arg(need_reload),
        tr("C&ontinue"), tr("&Cancel Saving")) == 1)
            return false;
    bool ok = false;

    ON_BLOCK_EXIT(&notify_save, &ok, sernaDoc());

    String new_path = (dsi ? dsi : getDsi())->
        getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    PropertyNode* is_new = getDsi()->getProperty(DocSrcInfo::IS_NEW_DOCUMENT);
    if (is_new)
        fixup_xinclude_paths(grove(), new_path);

    int gs_flags = Grove::GS_DEF_FILEFLAGS |
                   Grove::GS_INDENT | Grove::GS_RESET_MODFLAGS;
    if (config().getProperty(App::APP_CRLF_LINE_BREAKS)->getBool())
        gs_flags |= Grove::GS_CRLF_LINE_BREAKS;
    else
        gs_flags = gs_flags & ~Grove::GS_CRLF_LINE_BREAKS;
    if (getDsi()->getSafeProperty(DocSrcInfo::GENERATE_UNICODE_BOM)->getBool())
        gs_flags |= Grove::GS_GENERATE_UNICODE_BOM;
    if (0 == dsi) {
        add_comment(grove_);
        ok = grove_->saveAsXmlFile(gs_flags, xsltEngine_->stripInfo());
        if (ok) {
            setNotModified();
            if (is_new)
                is_new->remove();
        }
        // experimental: read/write xsl:document
        Xslt::CachedDocument* cdoc =
            xsltEngine()->documentCache()->docList().firstChild();
        for (; cdoc; cdoc = cdoc->nextSibling()) {
            if (!cdoc->grove()->registerVisitors())  // not read/write
                continue;
            cdoc->grove()->saveAsXmlFile(gs_flags, xsltEngine_->stripInfo());
        }
        doTimeStamps(grove(),
                     getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
        return ok;
    }
    bool save_all = Url(new_path)[Url::DIRPATH] != Url(getDsi()->
        getSafeProperty(DocSrcInfo::DOC_PATH)->getString())[Url::DIRPATH];
    String origSysid(grove_->topSysid());
    grove_->setTopSysid(new_path);
    if (save_all) {
        EntityReferenceTable* ert = grove_->document()->ert();
        EntityReferenceTable::iterator ei = ert->begin();
        for (; ei != ert->end(); ++ei) {
            if ((*ei)->decl()->declType() != EntityDecl::externalGeneralEntity)
                continue;
            const ExternalEntityDecl* ed =
                (*ei)->decl()->asConstExternalEntityDecl();
            if (!Url(ed->sysid()).isRelative()) {
                if (sernaDoc()->showMessageBox(
                    SernaDoc::MB_WARNING,
                    tr("Attempt to save-as external entity"
                       " with absolute pathname"),
                    tr("<qt>Entity <b>%1</b> has absolute pathname %2"
                       "<br><nobr><b>and will not be \"saved as\"</b>, but "
                       "will be saved in its current location.</nobr></qt>").
                    arg(ed->name()).arg(ed->sysid()),
                    tr("C&ontinue"), tr("&Cancel Saving")) == 1) {
                    grove_->setTopSysid(origSysid);
                    return false;
                }
                continue;
            }
            Url cpn((*ei)->node(0)->xmlBase());
            if (cpn.exists()) {
                if (sernaDoc()->showMessageBox(
                    SernaDoc::MB_WARNING, tr("File Exists"),
                    tr("File %1 already exists.").arg(String(cpn)),
                    tr("C&ontinue"), tr("&Cancel Saving")) == 1) {
                    grove_->setTopSysid(origSysid);
                    return false;
                }
            }
        }
    }
    add_comment(grove_);
    const PropertyNode* encoding_prop = dsi->getProperty(DocSrcInfo::ENCODING);
    if (encoding_prop) {
        grove_->groveBuilder()->setEncoding(Encodings::encodingByName
            (dsi->getSafeProperty(DocSrcInfo::ENCODING)->getString()));
        getDsi()->makeDescendant(DocSrcInfo::ENCODING)->
            setString(encoding_prop->getString());
    }
    ok = grove_->saveAsXmlFile(gs_flags|(save_all ? Grove::GS_FORCE_SAVE : 0),
                               xsltEngine_->stripInfo());
    if (ok) {
        if (is_new)
            is_new->remove();
        // re-acquire all locks
        release_locks(grove_.pointer());
        set_lock_status(grove_.pointer());
        setNotModified();
        getDsi()->makeDescendant(DocSrcInfo::DOC_PATH, new_path, true);
        sernaDoc()->updateTooltip();
    } else
        grove_->setTopSysid(origSysid);
    doTimeStamps(grove(), getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
    return ok;
}

namespace {

class LineNumberTest {
public:
    LineNumberTest(const Node* n)
        : node_(n) {}
    bool operator()(const Node* n) const
    {
        return n->nodeExt() && n->nodeExt()->asConstLineLocExt();
    }
private:
    const Node* node_;
};

} // namespace

void StructEditor::saveContextToDsi()
{
    GroveEditor::GrovePos pos = editViewSrcPos();
    if (pos.isNull())
        return;
    getDsi()->makeDescendant(DocSrcInfo::CURSOR_TREELOC, pos.asString(), true);
    const Node* n = 0;
    if (pos.type() == GroveEditor::GrovePos::TEXT_POS)
        n = pos.node();
    else
        n = pos.before() ? pos.before()->nextSibling() : pos.node();
    n = find_diffuse<Node, LineNumberTest>(n, LineNumberTest(n));
    if (0 == n)
        return;
    EntityReferenceStart* ers = get_ers(n);
    if (ers && ers->getSectParent()) {
        const EntityDecl* ed =
            static_cast<EntityReferenceStart*>(ers)->entityDecl();
        if (ed->declType() == EntityDecl::externalGeneralEntity) {
            String path = ers->xmlBase();
            if (!path.isEmpty())
                getDsi()->makeDescendant(DocSrcInfo::CURSOR_FILENAME,
                                         path, true);
        }
    }
    getDsi()->makeDescendant(DocSrcInfo::LINE_NUMBER,
        String::number(n->nodeExt()->asConstLineLocExt()->line()), true);
}

static String get_ro_msg(const PropertyNode* dsi)
{
    const PropertyNode* pn =
        dsi->getProperty("#app-messages/#document-is-readonly-message");
    if (0 == pn)
        return qApp->translate("structEditor", "Cannot modify content: "
                               "source document is read-only or locked");
    return pn->getString();
}

StructEditor::
PositionStatus StructEditor::isEditableEntity(const GroveEditor::GrovePos& gp,
                                              int opClass) const
{
    if (gp.isNull())
        return POS_FAIL;
    if (gp.node()->isGroveSectionNode() && !(opClass & CFE_OP))
        return POS_FAIL;
    const Node* const contextNode = gp.contextNode();
    EntityReferenceStart* ers = gp.getErs();
    if (0 == ers)
        ers = gp.node()->grove()->document()->ers();
    const EntityDecl* ed =
        static_cast<EntityReferenceStart*>(ers)->entityDecl();
    if (opClass & ANY_OP) {
        if ((opClass & NOROOT_OP) &&
            (!contextNode->parent() ||
            (gp.type() == GroveEditor::GrovePos::ELEMENT_POS &&
             gp.before() && !contextNode->parent()->parent()))) {
                if (opClass & SILENT_OP)
                    return POS_FAIL;
                String err = tr("Invalid operation on root element");
                sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                                     err, err, tr("Close"));
                return POS_FAIL;
        }
        const LineLocExt* le = gp.node()->nodeExt() ?
            gp.node()->nodeExt()->asLineLocExt() : 0;
        if (le && le->isReadOnly()) {
            if (opClass & ALLOW_RDONLY)
                return POS_RDONLY;
            if (opClass & SILENT_OP)
                return POS_FAIL;
            String msg = get_ro_msg(getDsi());
            if (!msg.isEmpty())
                sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                    tr("Cannot modify read-only content"),
                    msg, tr("&Ok"));
            return POS_FAIL;
        }
        if (ed->declType() == EntityDecl::internalGeneralEntity &&
            ed->declOrigin() != EntityDecl::prolog) {
            if (opClass & ALLOW_RDONLY)
                return POS_RDONLY;
            if (opClass & SILENT_OP)
                return POS_FAIL;
            sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                tr("Cannot change contents of external subset entity"),
                tr("<nobr>Attempt to edit contents of internal entity "
                   "which is declared in <b>external subset</b>.</nobr>\n"
                   "Operation not performed."), tr("&Ok"));
            return POS_FAIL;
        }
        if (ed->isReadOnly()) {
            if (opClass & ALLOW_RDONLY)
                return POS_RDONLY;
            if (opClass & SILENT_OP)
                return POS_FAIL;
            String msg = get_ro_msg(getDsi());
            if (!msg.isEmpty())
                sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                    tr("Cannot modify read-only content"),
                    msg, tr("&Ok"));
            return POS_FAIL;
        }
        if (ed->declType() == EntityDecl::xinclude) {
            const XincludeDecl* const xd =
                static_cast<const XincludeDecl*>(ed);
            if (xd->isFallback()) {
                if (opClass & ALLOW_RDONLY)
                    return POS_RDONLY;
                if (opClass & SILENT_OP)
                    return POS_FAIL;
                sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                    tr("Cannot modify XInclude fallback"),
                    tr("<nobr>XInclude fallback content is not editable."),
                    tr("&Ok"));
                return POS_FAIL;
            }
            if (xd->dataType() == EntityDecl::cdata &&
                (opClass & ~(TEXT_OP|SILENT_OP))) {
                if (opClass & SILENT_OP)
                    return POS_FAIL;
                sernaDoc()->showMessageBox(SernaDoc::MB_WARNING,
                    tr("Wrong operation for text XInclude"),
                    tr("Only text copy/insertion/deletions are "
                       "allowed in text XInclude sections"), tr("&Ok"));
                return POS_FAIL;
            }
            if (contextNode->nodeType() != Node::TEXT_NODE &&
                parentNode(contextNode) == ers->parent()) {
                if ((!gp.before() && !(opClass & PARENT_OP)) ||
                    opClass & CFE_OP)
                        return POS_OK;
                if (opClass & ALLOW_RDONLY)
                    return POS_RDONLY;
                if (opClass & SILENT_OP)
                    return POS_FAIL;
                sernaDoc()->showMessageBox(
                    SernaDoc::MB_WARNING,
                    tr("Cannot modify top of XInclude reference"),
                    tr("<nobr>Cannot edit top-level element content "
                       "of XInclude reference (to prevent breaking "
                       "the reference)</nobr><br/>"
                       "<b>Operation not done.</b>"), tr("&Ok"));
                return POS_FAIL;
            }
        }
    }
    return POS_OK;
}
