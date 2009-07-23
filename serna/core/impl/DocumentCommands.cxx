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
#include "core/core_defs.h"
#include "core/DocBuilders.h"
#include "core/debug_core.h"

#include "structeditor/StructDocument.h"
#include "structeditor/StructEditor.h"
#include "plaineditor/PlainDocument.h"

#include "ui/UiAction.h"

#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "utils/Properties.h"
#include "common/PropertyTreeEventData.h"
#include "utils/DocTemplate.h"
#include "utils/struct_autosave_utils.h"

#include "docview/EventTranslator.h"
#include "docview/DocumentStateEventData.h"

#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/Url.h"
#include "common/PathName.h"
#include "common/Exception.h"
#include "common/Message.h"
#include "common/StringCvt.h"

#include "grove/Grove.h"

#include <QFileDialog>
#include <QApplication>
#include <QDir>

USING_COMMON_NS;

class ChooseDocTemplate;
class OpenStructDocument;
class NewStructDocument;

class ProgressGuard {
public:
    ProgressGuard(SernaDoc* sernaDoc)
        : sernaDoc_(sernaDoc) {}
    ~ProgressGuard()
    {
        sernaDoc_->showStageInfo();
    }
private:
    SernaDoc* sernaDoc_;
};

static void set_browse_dir(PropertyNode* dsi)
{
    dsi->makeDescendant(DocSrcInfo::BROWSE_DIR,
                        DocSrcInfo::get_browse_dir(dsi), false);
}

static bool save_park_info(PropertyNode* dsi)
{
    //NOTE: Save path for parking dialogs
    String path = dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    PathName path_doc(path);
    if (path.isEmpty() || !path_doc.exists())
        return false;
    config().root()->makeDescendant(App::DOC_BROWSE)->
             setString(path_doc.dirname().name());
    return false;
}

static bool append_document(SernaDoc* prevSernaDoc, SernaDoc* doc)
{
    prevSernaDoc->parent()->appendChild(doc);
    DocumentStateEventData state_ev(DocumentStateEventData::POST_INIT);
    doc->stateChangeFactory().dispatchEvent(&state_ev);
    return true;
}

class DoOpenPlainDocument;

static bool make_document(SernaDoc* prevSernaDoc,
                          const PropertyTreeEventData& result,
                          bool isNew)
{
    ProgressGuard progressGuard(prevSernaDoc);

    PropertyNodePtr dsi = result.root();

    if (!prevSernaDoc->checkDsi(dsi.pointer()))
        return false;
    DocSrcInfo::resolve_dsi(dsi.pointer());

    save_park_info(dsi.pointer());

    DBG_IF(CORE.TEST) dsi->dump();

    CommandEventPtr event = (isNew)
        ? makeCommand<NewStructDocument>(&result)
        : makeCommand<OpenStructDocument>(&result);
    RefCntPtr<StructDocument> struct_doc;
    try {
        struct_doc = new StructDocument(event, dsi.pointer(),
                                        prevSernaDoc,
                                        new StructDocumentBuilder());
    }
    catch (int) {
        prevSernaDoc->showStageInfo();
        return false;
    }
    catch (Exception& exc) {
        prevSernaDoc->showStageInfo();
        prevSernaDoc->showMessageBox(
            SernaDoc::MB_CRITICAL, (isNew)
            ? qApp->translate("DocumentCommands", "Cannot Create Document")
            : qApp->translate("DocumentCommands", "Cannot Open Document"),
            exc.what(), tr("OK"));
        return false;
    }
    PropertyNode* hasXmlErrors =
        struct_doc->getDsi()->getProperty("#has-xml-errors");
    if (hasXmlErrors) {
        hasXmlErrors->remove();
        struct_doc->showStageInfo();
        if (prevSernaDoc->showMessageBox(
            SernaDoc::MB_CRITICAL,
            qApp->translate("DocumentCommands", "XML Parser Error"),
            qApp->translate(
                "DocumentCommands",
                "<qt><nobr>Non-recoverable syntax errors occurred when "
                "parsing XML document.</nobr><br><b>Do you want to view "
                "and fix them manually in Plain Text Editor mode?</b></qt>"),
            tr("&Ok"), tr("&Cancel")))
            return false;
        RefCntPtr<PlainDocument> plain_doc =
            new PlainDocument(dsi.pointer(), struct_doc->messageTree(),
                              new PlainDocumentBuilder);
        PropertyTreeEventData ed(dsi.pointer());
        if (makeCommand<DoOpenPlainDocument>(&ed)->execute(
                plain_doc.pointer())) {
            append_document(prevSernaDoc, plain_doc.pointer());
            if (!isNew)
                DocSrcInfo::add_recent_document(dsi.pointer());
            return true;
        }
        return false;
    }
    append_document(prevSernaDoc, struct_doc.pointer());

    if (!isNew)
        DocSrcInfo::add_recent_document(dsi.pointer());

    return true;
}

////////////////////////////////////////////////////////////////////////

static String make_unnamed_filename(const SernaDoc* sernaDoc)
{
    static int unnamedDocIndex = 1;
    String nonameFn;
    for (;;) {
        nonameFn = NOTR("Noname") + String::number(unnamedDocIndex++) +
            NOTR(".xml");
        Sui::Item* ci = sernaDoc->parent()->firstChild();
        for (; ci; ci = ci->nextSibling()) {
            SernaDoc* cd = dynamic_cast<SernaDoc*>(ci);
            if (0 == cd)
                continue;
            const PropertyNode* ptn = cd->getDsi();
            if (0 == ptn)
                continue;
            if (PathName::isSameFile(PathName(nonameFn),
                PathName(ptn->getSafeProperty
                    (DocSrcInfo::DOC_PATH)->getString())))
                        break;
        }
        if (0 == ci)
            return nonameFn;
    }
}

static void clean_auto_save(PropertyNode* dsi)
{
    // avoid forced encoding on re-open
    dsi->makeDescendant(DocSrcInfo::ENCODING)->remove();
    String topSysid(dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString());
    if (!topSysid.empty()) {
        AutoSaveUtils::clean_auto_save(topSysid);
        AutoSaveUtils::Sysids().remove(topSysid);
    }
}

class NewDocDialog;

SIMPLE_COMMAND_EVENT_IMPL(NewDocument, SernaDoc)

bool NewDocument::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTreeEventData iparam;
    iparam.root()->merge(config().getProperty("doc-defaults"));
    set_browse_dir(iparam.root());
    iparam.root()->makeDescendant
        (DocSrcInfo::DOC_PATH, make_unnamed_filename(sernaDoc));

    DBG(CORE.TEST) << "NewDoc: Input params:\n";
    DBG_IF(CORE.TEST) iparam.root()->dump();

    PropertyTreeEventData result;
    //! Making new Document
    PropertyNode* root = result.root();
    root->makeDescendant(DocSrcInfo::IS_NEW_DOCUMENT)->setBool(true);
    root->makeDescendant(NOTR("#not-saved"));
    if (makeCommand<NewDocDialog>(&iparam)->execute(sernaDoc, &result)) {
        DBG(CORE.TEST) << "NewDoc: Out params:\n";
        DBG_IF(CORE.TEST) root->dump();
        //! Make flag to call saveas on save
        QFileInfo doc(root->getSafeProperty(DocSrcInfo::DOC_PATH)->getString());
        if (doc.baseName().startsWith(NOTR("Noname"))) {
            bool do_saveas = true;
            String res = doc.baseName().mid(6);
            for (int i = 0; i < (int)res.length(); i++) {
                if (!res[i].isDigit())
                    do_saveas = false;
            }
            if (do_saveas)
                root->makeDescendant("#do-save-as")->setBool(true);
        }
        return make_document(sernaDoc, result, true);
    }
    return false;
}

///////////////////////////////////////////////////////////////////

static bool if_default_style(const String& style)
{
    PathName p1(config().
        getProperty("doc-defaults/xml-stylesheet")->getString());
    PathName p2(style);
    bool is_same = PathName::isSameFile(p1, p2);
    DBG(CORE.TEST) << "isSameStyle: same=" << is_same << ", p1=" << p1.name() 
        << ", p2=" << p2.name() << std::endl;
    return is_same;
}

class NativeOpenDocDialog;

PROPTREE_EVENT_IMPL(OpenDocumentWithDsi, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(OpenDocumentWithDsi, "PropertyTree", "-")

bool OpenDocumentWithDsi::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTreeEventData result;
    result.root()->merge(config().getProperty("doc-defaults"));
    set_browse_dir(result.root());
    if (ed_)
        result.root()->merge(ed_, true);
    else {
        if (!makeCommand<NativeOpenDocDialog>(&result)->execute(sernaDoc,
            &result))
            return save_park_info(result.root());
    }
    String doc_path =
        result.root()->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    Url docpath_url(doc_path);
    if (docpath_url[Url::PROTOCOL] == NOTR("file") &&
        docpath_url.isRelative()) {
            doc_path = docpath_url.absolute();
            result.root()->makeDescendant(DocSrcInfo::DOC_PATH, doc_path, true);
    }
    if (DocTemplate::DocTemplateHolder::getDocProperties(doc_path,
        result.root()))
            return make_document(sernaDoc, result, false);
    if (result.root()->getProperty("#url-open-failed")) {
        sernaDoc->showMessageBox(SernaDoc::MB_CRITICAL,
            tr("Cannot open file or URL"),
            tr("Cannot open file or URL '%0' for reading").arg(doc_path),
            tr("&Ok"));
        return false;
    }
    PropertyNodePtr dsi_copy = result.root()->copy(true);
    DocSrcInfo::resolve_dsi(dsi_copy.pointer());
    if (!if_default_style(dsi_copy->getSafeProperty
        (DocSrcInfo::RESOLVED_STYLE_PATH)->getString()))
            return make_document(sernaDoc, result, false);
    int action = sernaDoc->showMessageBox(SernaDoc::MB_WARNING,
        tr("No matched document template found"),
        tr("<qt><nobr>No document template matched for <pre>%1</pre>"
        "</nobr>You can open document with default stylesheet or select "
        "appropriate document template manually.</qt>").arg(doc_path),
        tr("&Use Default"), tr("Choose &Template"), tr("&Cancel"));
    switch (action) {
        case 0: {
            PropertyNode* def_tmpl =
                DocTemplate::DocTemplateHolder::instance().root()->
                firstChild();
            if (def_tmpl)
                def_tmpl = def_tmpl->firstChild();
            if (0 == def_tmpl) {
                sernaDoc->showMessageBox(SernaDoc::MB_CRITICAL,
                    tr("No default template found"),
                    tr("Sorry, no default template found"), tr("&Ok"));
                return save_park_info(result.root());
            }
            result.root()->merge(def_tmpl, true);
            return make_document(sernaDoc, result, false);
        }
        case 1:
            if (!makeCommand<ChooseDocTemplate>(&result)->
                execute(sernaDoc, &result))
                return save_park_info(result.root());
            return make_document(sernaDoc, result, false);
        case 2:
        default:
            return save_park_info(result.root());
    }
}

PROPTREE_EVENT_IMPL(OpenStructDocumentWithDsi, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(OpenStructDocumentWithDsi, "PropertyTree", "-")

bool OpenStructDocumentWithDsi::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTreeEventData evd(ed_);
    return make_document(sernaDoc, evd, false);
}

SIMPLE_COMMAND_EVENT_IMPL(OpenDocument, SernaDoc)

bool OpenDocument::doExecute(SernaDoc* sernaDoc, EventData*)
{
    return makeCommand<OpenDocumentWithDsi>()->execute(sernaDoc);
}

PROPTREE_EVENT_IMPL(DoOpenDocumentWithTemplate, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(DoOpenDocumentWithTemplate, "PropertyTree", "-")

bool DoOpenDocumentWithTemplate::doExecute(SernaDoc* doc, EventData*)
{
    PropertyTreeEventData data(ed_);
    if (!makeCommand<ChooseDocTemplate>(&data)->execute(doc, &data))
        return save_park_info(data.root());
    data.root()->merge(config().getProperty("doc-defaults"));
    return make_document(doc, data, false);
}

SIMPLE_COMMAND_EVENT_IMPL(OpenDocumentWithTemplate, SernaDoc)

bool OpenDocumentWithTemplate::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyTreeEventData data;
    PropertyTreeEventData result;
    set_browse_dir(data.root());
    data.root()->makeDescendant("#open-with-template");
    if (!makeCommand<NativeOpenDocDialog>(&data)->execute(sernaDoc, &result))
        return save_park_info(result.root());
    return makeCommand<DoOpenDocumentWithTemplate>(&result)->execute(sernaDoc);
}

SIMPLE_COMMAND_EVENT_IMPL(ReloadStructDocument, StructEditor)

bool ReloadStructDocument::doExecute(StructEditor* se, EventData*)
{
    PropertyTreeEventData ed(se->getDsi());
    SernaDoc* psd = static_cast<SernaDoc*>(se->sernaDoc()->prevSibling());
    if (se->sernaDoc()->closeDocument()) {
        clean_auto_save(ed.root());
        se->sernaDoc()->removeItem();
        return makeCommand<OpenStructDocumentWithDsi>(&ed)->execute(psd);
    }
    return false;
}

///////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(OpenPlainDocumentWithDsi, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(OpenPlainDocumentWithDsi, "PropertyTree", "-")

bool OpenPlainDocumentWithDsi::doExecute(SernaDoc* doc, EventData*)
{
    PropertyTreeEventData data(ed_);
    PropertyNodePtr dsi = data.root();
    if (!doc->checkDsi(dsi.pointer()))
        return false;
    save_park_info(data.root());
    RefCntPtr<PlainDocument> plain_doc =
        new PlainDocument(data.root(), 0, new PlainDocumentBuilder);
    if (!makeCommand<DoOpenPlainDocument>(&data)->execute(plain_doc.pointer()))
        return false;
    append_document(doc, plain_doc.pointer());
    DocSrcInfo::add_recent_document(data.root());
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(OpenPlainDocument, SernaDoc)

bool OpenPlainDocument::doExecute(SernaDoc* doc, EventData*)
{
    String file = QFileDialog::getOpenFileName(
        doc->widget(0), tr("Open File as Plain Text"));
    if (file.isEmpty())
        return false;
    PropertyTreeEventData data;
    data.root()->makeDescendant(DocSrcInfo::DOC_PATH, file);
    return makeCommand<OpenPlainDocumentWithDsi>(&data)->execute(doc);
}

///////////////////////////////////////////////////////////////////

class OpenPlainDocumentWithDsi;

SIMPLE_COMMAND_EVENT_IMPL(SwitchStructToPlain, SernaDoc)

bool SwitchStructToPlain::doExecute(SernaDoc* sernaDoc, EventData*)
{
    PropertyNode* prop = config().root()->makeDescendant(
                         App::DONT_SHOW_SWITCH_EXPERT_MODE_FLAG);
    if (!prop->getBool()) {
        if (1 == sernaDoc->showMessageBox(SernaDoc::CHECKED_WARNING, "",
            tr("You are switching to plain text mode.\n"
            "All the advanced XML functionality\n"
            "(inserting elements, etc.) will be disabled."),
            tr("&Close")))
            prop->setBool(true);
    }
    PropertyTreeEventData prop_data(sernaDoc->getDsi());
    PropertyNodePtr mtimes(prop_data.root()->
        makeDescendant(DocSrcInfo::MODIFIED_TIME));
    SernaDoc* prev_doc = static_cast<SernaDoc*>(sernaDoc->prevSibling());

    if (!sernaDoc->closeDocument())
        return false;
    clean_auto_save(prop_data.root());
    sernaDoc->removeItem();
    if (!mtimes->parent())
        prop_data.root()->appendChild(&*mtimes);
    return makeCommand<OpenPlainDocumentWithDsi>(&prop_data)->execute(prev_doc);
}

SIMPLE_COMMAND_EVENT_IMPL(SwitchPlainToStruct, SernaDoc)

bool SwitchPlainToStruct::doExecute(SernaDoc* plainDoc, EventData*)
{
    PropertyTreeEventData prop_data(plainDoc->getDsi());
    SernaDoc* prev_doc = static_cast<SernaDoc*>(plainDoc->prevSibling());
    if (!plainDoc->closeDocument())
        return false;
    plainDoc->removeItem();
    prop_data.root()->makeDescendant(DocSrcInfo::IS_TEXT_DOCUMENT)->remove();
    if (!prop_data.root()->getSafeProperty(
        DocSrcInfo::RESOLVED_STYLE_PATH)->getString().isEmpty())
        return makeCommand<OpenStructDocumentWithDsi>(&prop_data)->
            execute(prev_doc);
    PropertyTreeEventData data;
    data.root()->merge(config().getProperty("doc-defaults"));
    data.root()->makeDescendant(DocSrcInfo::DOC_PATH,
                                prop_data.root()->getSafeProperty(
                                    DocSrcInfo::DOC_PATH)->getString(),
                                true);
    if (!makeCommand<ChooseDocTemplate>(&data)->execute(prev_doc, &data))
        return false;
    return make_document(prev_doc, data, false);
}

///////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CloseDocument, SernaDoc)

bool CloseDocument::doExecute(SernaDoc* sernaDoc, EventData*)
{
    if (!sernaDoc->closeDocument())
        return false;
    clean_auto_save(sernaDoc->getDsi());
    sernaDoc->showStageInfo();
    sernaDoc->removeItem();
    return true;
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(OpenExamples, SernaDoc)

class ExamplesDialog;

bool OpenExamples::doExecute(SernaDoc* se, EventData*)
{
    PropertyNode* examples = config().root()->getProperty(Examples::EXAMPLES);
    PropertyTreeEventData result;
    if (!makeCommand<ExamplesDialog>()->execute(se, &result))
        return false;
    String example_to_open =
        result.root()->makeDescendant(Examples::EXAMPLE_FILE)->getString();
    String path = example_to_open;
    if (PathName(path).isRelative())
        path = examples->getSafeProperty("base-path")->getString()
            + PathName::DIR_SEP + path;
    if (!example_to_open.isEmpty()) {
        PropertyTreeEventData data;
        data.root()->makeDescendant(DocSrcInfo::DOC_PATH, path);
        if (!makeCommand<OpenDocumentWithDsi>(&data)->execute(se, &data))
             return false;
        //! Mark example as already read
        PropertyNode* example = examples->firstChild();
        for (; example; example = example->nextSibling()) {
            if (Examples::EXAMPLE == example->name() && example_to_open ==
                example->getSafeProperty(Examples::EXAMPLE_FILE)->getString())
                    example->makeDescendant(Examples::READ)->setBool(true);
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ResetConfig, SernaDoc)

bool ResetConfig::doExecute(SernaDoc* doc, EventData*)
{
    Sui::Item* window = doc->root()->firstChild();
    for (; window; window = window->nextSibling())
        if (window->countChildren() > 1)
            break;
    if (window) {
        doc->showMessageBox(SernaDoc::MB_CRITICAL,
            tr("Must close all documents first"),
            tr("<qt><nobr>You must close all opened documents prior to "
            "performing this operation.</nobr></qt>"), tr("&Ok"));
        return false;
    }
    if (doc->showMessageBox(SernaDoc::MB_WARNING,
            tr("Reset to Factory Settings"),
            tr("<qt><nobr><b>Warning:</b> you will lose all your Preferences "
            "settings, they will be reset to defaults.</nobr><br/>"
            "<nobr>When you press OK, Serna will reset settings "
            "and shut down.</nobr></qt>"), tr("&Cancel"), tr("&Proceed"))) {
                config().root()->makeDescendant("erase-config-on-startup");
                config().save();
                String basename = config().getConfigDir();
                basename += PathName::DIR_SEP;
                basename += NOTR("tmp");
                QDir tmp_dir(basename, "*");
                uint i = 0;
                for (i = 0; i < tmp_dir.count(); i++)
                    tmp_dir.remove(tmp_dir[i]);
                QDir dir(config().getConfigDir(), NOTR("*.sui"));
                for (i = 0; i < dir.count(); i++)
                    dir.remove(dir[i]);
                exit(0);
    }
    return false;
}

