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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "utils/Properties.h"

#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/PathName.h"
#include "common/StringTokenizer.h"
#include "common/Url.h"

#include <QObject>
#include <QFile>
#include <QApplication>
#ifdef WIN32
# include "shlobj.h"
#endif
#include <set>
#include <iostream>

using namespace Common;

namespace DocSrcInfo {

typedef UTILS_EXPIMP const char* const exported_literal;


// START_IGNORE_LITERALS
exported_literal DOC_PATH                   = "doc-src";
exported_literal IS_TEXT_DOCUMENT           = "is-plain-text-document";
exported_literal NS_MAP                     = "#doctemplate-ns-map";
exported_literal STYLE_PATH                 = "xml-stylesheet";
exported_literal RESOLVED_STYLE_PATH        = "resolved-xml-stylesheet";
exported_literal SCHEMA_PATH                = "xml-schema";
exported_literal NONS_SCHEMA_PATH           = "nons-xml-schema";
exported_literal NONS_SCHEMA_REWRITE        = "nons-schema-rewrite";
exported_literal RESOLVED_SCHEMA_PATH       = "resolved-xml-schema";
exported_literal ENCODING                   = "encoding";
exported_literal DOC_ENCODING               = "#doc-encoding";
exported_literal GENERATE_UNICODE_BOM       = "#generate-unicode-bom";
exported_literal MODIFIED_TIME              = "#modified_time";
exported_literal WATCHED_FILE               = "watched-file";
exported_literal WATCHED_ENTITY             = "entity";
exported_literal WATCHED_ENTITY_PATH        = "path";
exported_literal WATCHED_ENTITY_TIME        = "time";

exported_literal BROWSE_DIR                 = "browse-dir";
exported_literal STYLESHEETS_DIR            = "stylesheets-dir";
exported_literal SCHEMAS_DIR                = "schemas-dir";

exported_literal ROOT_ELEMENTS              = "root-elements";
exported_literal DOCUMENT_SKELETON          = "document-skeleton";

exported_literal SCALE                      = "scale";
exported_literal ZOOM                       = "zoomComboBox";
exported_literal SELECTION_BALANCING        = "selection-balancing";
exported_literal SHOW_TAGS                  = "show-tags";
exported_literal SHOW_PAGINATED             = "show-paginated";

exported_literal DSI                        = "doc-src-info";
exported_literal IS_NEW_DOCUMENT            = "#is-new-document";
exported_literal RECENT_LIST                = "recently-opened-files";
exported_literal RECENT_MAXIMUM             = "maximum-records";
exported_literal HIDE_ELEMENTS              = "hide-elements";
exported_literal HIDE_ATTRIBUTES            = "hide-attributes";

exported_literal FOLDING_RULES              = "folding-rules";
exported_literal FOLD_IF_LARGE              = "fold-if-large";
exported_literal ELEMS_PER_PAGE             = "elems-per-page";
exported_literal LARGE_DOC_LIMIT            = "large-doc-limit";
exported_literal FOLDING_LIST               = "folding-list";
exported_literal FOLDING_MODE               = "folding-mode";
exported_literal HIDE_IN_NEWDOC_DIALOG      = "hide-in-new-document-dialog";

exported_literal CURSOR_TREELOC             = "cursor-treeloc";
exported_literal LINE_NUMBER                = "line-number";
exported_literal CURSOR_FILENAME            = "cursor-file";

exported_literal ZOOM_VALUES                = "zoom-values";

exported_literal VALIDATION_MODE            = "validation-mode";
exported_literal TOC_IS_ON                  = "toc-is-on";

exported_literal SPELLER_PROPS              = "spellcheck";
exported_literal SPELLER_SKIPELEMS          = "skip-elements";

exported_literal TEMPLATE_CATEGORY          = "category";
exported_literal TEMPLATE_NAME              = "name";
exported_literal TEMPLATE_DOCROOTGROVE      = "docroot-grove";
exported_literal TEMPLATE_WHENOPEN          = "open-document";
exported_literal TEMPLATE_DTD_GUESS         = "dtd-guess-string";
exported_literal TEMPLATE_FN_GUESS          = "filename-guess-string";
exported_literal TEMPLATE_ROOT_GUESS        = "rootelem-guess-string";
exported_literal TEMPLATE_NONS_SCHEMA_GUESS = "nons-schema-guess-string";
exported_literal TEMPLATE_PRIORITY          = "template-priority";
exported_literal TEMPLATE_SHOWUP_PRIORITY   = "template-showup-priority";
exported_literal TEMPLATE_PATH              = "template-path";
exported_literal TEMPLATE_DIR               = "template_dir";   // _ = variable
exported_literal TEMPLATE_CATEGORY_PIXMAP   = "category-pixmap";
exported_literal TEMPLATE_CATALOGS_LIST     = "xml-catalogs";
exported_literal RESOLVED_CATALOGS          = "resolved-catalogs-list";
exported_literal TEMPLATE_ELEM_HELP_FILE    = "element-help";
exported_literal RESOLVED_ELEM_HELP_FILE    = "resolved-element-help";

exported_literal DSI_ID                     = "dsi-id";
exported_literal DOCINFO_DOCTYPE            = "doctype-string";
exported_literal DOCINFO_SYSID              = "dtd-sysid";
exported_literal DOCINFO_PUBID              = "dtd-pubid";
exported_literal DOCINFO_DOCELEM            = "document-element";
exported_literal ELEMENT_ID_FORMAT          = "element-id-format";
exported_literal PROCESS_XINCLUDE           = "process-xinclude";

exported_literal DUMP_FO_PATH               = "dump-fo-path";
// STOP_IGNORE_LITERALS

UTILS_EXPIMP void resolve_dsi(PropertyNode* dsi, const PropertyNode* pdsi)
{
    using namespace DocSrcInfo;
    if (!pdsi)
        pdsi = dsi;
    String docpath = pdsi->getSafeProperty(DOC_PATH)->getString();
    String tmplpath =
        pdsi->getSafeProperty(TEMPLATE_PATH)->getString();

    PropertyTree localVars;
    if (!tmplpath.isEmpty())
        localVars.root()->makeDescendant(TEMPLATE_DIR,
            Url(tmplpath)[Url::DIRPATH], true);

    if (!dsi->getProperty(RESOLVED_STYLE_PATH))
        dsi->makeDescendant(RESOLVED_STYLE_PATH, config().resolveResource(
            NOTR("doc-defaults/xml-stylesheet"),
            dsi->getSafeProperty(STYLE_PATH)->getString(),
            docpath, localVars.root()));
    if (!dsi->getProperty(RESOLVED_SCHEMA_PATH)) {
        if (dsi->getProperty(SCHEMA_PATH))
            dsi->makeDescendant(RESOLVED_SCHEMA_PATH,
                config().resolveResource(String(), 
                    dsi->getString(SCHEMA_PATH), docpath, localVars.root()));
        else if (dsi->getProperty(NONS_SCHEMA_PATH)) {
            String nspath = dsi->getString(NONS_SCHEMA_PATH);
            if (!dsi->getSafeProperty(NONS_SCHEMA_REWRITE)->getBool())
                nspath = Url(docpath).combinePath2Path(nspath);
            dsi->makeDescendant(RESOLVED_SCHEMA_PATH, nspath, true);
        }
    }
    if (!dsi->getProperty(RESOLVED_CATALOGS)) {
        StringTokenizer st(dsi->getString(TEMPLATE_CATALOGS_LIST), NOTR(";"));
        String cat_list;
        for (int i = 0; st; ++i) {
            if (i)
                cat_list += ';';
            cat_list += config().resolveResource(
                String(), st.next(), docpath, localVars.root());
        }
        dsi->makeDescendant(RESOLVED_CATALOGS, cat_list);
    }        
    if (!dsi->getProperty(RESOLVED_ELEM_HELP_FILE))
        dsi->makeDescendant(RESOLVED_ELEM_HELP_FILE, config().resolveResource(
            String(),
            dsi->getSafeProperty(TEMPLATE_ELEM_HELP_FILE)->
                getString(),
            docpath, localVars.root()));
}

Common::String makeDsiTooltip(const Common::PropertyNode* dsi)
{
    if (dsi->getProperty(IS_TEXT_DOCUMENT))
        return qApp->translate(
            "PlainDocument", 
            "<qt><nobr><b>Plain Text File: </b>%1</nobr></qt>").
                arg(dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString());
    Common::String res = tr("<qt><nobr><b>Filename:</b> %1</nobr>").
                         arg(dsi->getSafeProperty(DOC_PATH)->getString());
    Common::String s = dsi->getSafeProperty(RESOLVED_STYLE_PATH)->getString();
    if (!s.isEmpty())
        res += tr("<br/><nobr><b>Stylesheet:</b> %1</nobr>").arg(s);
    s = dsi->getSafeProperty(RESOLVED_SCHEMA_PATH)->getString();
    if (!s.isEmpty())
        res += tr("<br/><nobr><b>Schema:</b> %1</nobr>").arg(s);
    res += NOTR("</qt>");
    return res;
}

Common::String docPathFilename(const Common::PropertyNode* dsi)
{
    return Common::PathName
        (dsi->getSafeProperty(DOC_PATH)->getString()).filename();
}

Common::String myDocumentsPath()
{
    using namespace Common;
#ifdef WIN32
    TCHAR szMyDocs[MAX_PATH];
    if (SHGetSpecialFolderPath(NULL, szMyDocs, CSIDL_PERSONAL, 0))
        return szMyDocs;
    else
        return config().getHomeDir();
#elif defined(__APPLE__)
    PathName mydocs(config().getHomeDir() + PathName::DIR_SEP + NOTR("Documents"));
    if (mydocs.exists())
        return mydocs.name();
    else
        return config().getHomeDir();
#else
    return PathName::getcwd().name();
#endif
}

static bool is_same_dsi(const PropertyNode* dsi1, const PropertyNode* dsi2)
{
    if (!dsi1 || !dsi2)
        return false;
    return Url(dsi1->getSafeProperty(DOC_PATH)->getString()) ==
        Url(dsi2->getSafeProperty(DOC_PATH)->getString());
}

static int count_props(const String& name, const PropertyNode* prop)
{
    int num = 0;
    for (const PropertyNode* child = prop->firstChild();
         child; child = child->nextSibling())
        if (name == child->name())
            num++;
    return num;
}

void add_recent_document(PropertyNode* dsi)
{
    PropertyNodePtr dsi_holder(dsi);
    if (dsi->parent())
        dsi->remove();
    dsi->setName(DSI);
    PropertyNode* dsi_list =
        config().root()->makeDescendant(RECENT_LIST);
    int max_len = dsi_list->getSafeProperty(RECENT_MAXIMUM)->getInt();
    if (max_len <= 0 || max_len > 20) {
        max_len = 10;
        dsi_list->makeDescendant(RECENT_MAXIMUM)->setInt(max_len);
    }
    PropertyNode* pn = dsi_list->getProperty(DSI);

    // 1. find document with same DSI, if any
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() != DSI)
            continue;
        if (is_same_dsi(dsi, pn))
            break;
    }
    // 2. found same dsi? remove it from the middle, put to front
    if (pn) {
        pn->remove();
        if (dsi_list->firstChild())
            dsi_list->firstChild()->insertBefore(dsi);
        else
            dsi_list->appendChild(dsi);
    }
    else {
        // 3. check length
        int recent_len = count_props(DSI, dsi_list);
        if (max_len > 0)
            max_len--;
        if (4 > max_len)
            max_len = 4;
        else
            if (20 < max_len)
                max_len = 20;
        // 4. if too much, truncate
        PropertyNode* p = dsi_list->lastChild();
        while (recent_len >= max_len && p) {
            PropertyNode* prev = p->prevSibling();
            if (RECENT_MAXIMUM != p->name()) {
                PropertyNode* dsi_id = p->getProperty(DSI_ID);
                if (dsi_id && !dsi_id->getString().isEmpty()) {
                    PathName path(config().getConfigDir());
                    path.append(NOTR("tmp"));
                    path.append(NOTR("recent_") + dsi_id->getString() + NOTR(".sui"));
                    QFile file(String(path.name()));
                    file.remove();
                    //String basename = config().getConfigDir();
                    //basename += PathName::DIR_SEP;
                    //basename += "tmp";
                    //basename += PathName::DIR_SEP;
                    //basename += "recent_" + dsi_id->getString() + ".sui";
                    //QFile file(basename);
                    //file.remove();
                }
                p->remove();
                recent_len--;
            }
            p = prev;
        }
        if (dsi_list->firstChild())
            dsi_list->firstChild()->insertBefore(dsi);
        else
            dsi_list->appendChild(dsi);
    }
    // 5. generate unique dsi-id
    if (!dsi->getProperty(DSI_ID)) {
        std::set<int> id_set;
        for (PropertyNode* pn = dsi_list->firstChild(); pn; 
             pn = pn->nextSibling()) {
            PropertyNode* id_prop = pn->getProperty(DSI_ID);
            if (id_prop) 
                id_set.insert(id_prop->getInt());
        }
        for (int id = 0; id <= max_len + 1; id++) {
            if (id_set.find(id) == id_set.end()) {
                dsi->makeDescendant(DSI_ID)->setInt(id);
                break;
            }
        }
    }
    config().eventFactory().update();
    config().save();
}

UTILS_EXPIMP PropertyNode* get_dsi(const int recentDepth)
{
    const PropertyNode* dsi_list = config().getProperty(RECENT_LIST);
    if (!dsi_list)
        return 0;
    int num = 1;
    for (PropertyNode* i = dsi_list->firstChild(); i; i = i->nextSibling()) {
        if (DSI != i->name())
            continue;
        if (recentDepth == num)
            return i;
        num++;
    }
    return 0;
}

String get_browse_dir(PropertyNode*)
{
    String browse_dir;
//NOTE: uncomment to make park to current document
//    if (sd->getDsi())
//        browse_dir = sd->getDsi()->
//                     getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    if (browse_dir.isEmpty())
        browse_dir = config().getProperty(App::DOC_BROWSE)->getString();
    if (browse_dir.isEmpty())
        browse_dir = config().getProperty(String("app/") +
                                          App::DOC_PATH)->getString();
    if (browse_dir.isEmpty())
        browse_dir = myDocumentsPath();
    return browse_dir;
}

} // namespace DocSrcInfo
