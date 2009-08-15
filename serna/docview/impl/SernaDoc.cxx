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
#include "docview/dv_utils.h"
#include "docview/SernaDoc.h"
#include "docview/EventTranslator.h"
#include "docview/DocumentStateEventData.h"
#include "docview/DocumentStateFactory.h"
#include "docview/PluginLoader.h"
#include "docview/impl/debug_dv.h"

#include "common/PropertyTree.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/PathName.h"
#include "common/Url.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/PropertyTreeEventData.h"

#include "utils/Properties.h"
#include "utils/SernaMessages.h"
#include "utils/SernaUiItems.h"
#include "utils/MsgBoxStream.h"
#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "utils/IdleHandler.h"
#include "utils/Version.h"
#include "utils/reg_utils.h"

#include "ui/UiStackItem.h"
#include "ui/MainWindow.h"
#include "ui/ActionSet.h"
#include "ui/MimeHandler.h"

#include <QApplication>
#include <iostream>
using namespace Common;

class ProgressMessenger : public Messenger {
public:
    ProgressMessenger(SernaDoc* doc)
        : doc_(doc) {}
    virtual void dispatch(RefCntPtr<Message>& msg)
    {
        doc_->showStageInfo(msg->format(BuiltinMessageFetcher::instance()));
    }
private:
    SernaDoc* doc_;
};

//////////////////////////////////////////////////////////////////////////

SernaDoc::SernaDoc(const DocBuilder* builder, Sui::Item* prevItem)
    : Sui::Document(new EventTranslator),
      contextAction_(0),
      docBuilder_(builder),
      progressMessenger_(new ProgressMessenger(this)),
      progressStream_(new MessageStream(SernaMessages::getFacility(),
                                        progressMessenger_.pointer()))
{
    if (prevItem)
        setBool("no-ax", prevItem->getBool("no-ax"));
    PropertyNode* file_prop =
        config().root()->makeDescendant("dav/#protocols/file");
    file_prop->makeDescendant("commandEvent")->setString(
        NOTR("ShowFileDialog"));
    file_prop->makeDescendant("browse-inscription")->setString(
        tr("&File Browse..."));

    static_cast<EventTranslator*>(dispatcher())->setDocument(this);
    Sui::Document* prevDoc = dynamic_cast<Sui::Document*>(prevItem);
    if (prevDoc)
        contextAction_ = prevDoc->actionSet()->findAction(NOTR("docContext"));
}

static void remove_temp_properties(PropertyNode* pn)
{
    if (0 == pn)
        return;
    if (pn->name().left(1) == "#" && pn->name() != NOTR("#app-messages")) {
        pn->remove();
        return;
    }
    pn = pn->firstChild();
    while (pn) {
        PropertyNode* next = pn->nextSibling();
        remove_temp_properties(pn);
        pn = next;
    }
}

SernaDoc::~SernaDoc()
{
    DDBG << "SernaDoc::~SernaDoc: " << this << std::endl;
    qt_clean_sst();
    pluginLoader().unloadPlugins(this);
    remove_temp_properties(dsi_.pointer());
}

DynamicEventFactory& SernaDoc::stateChangeFactory()
{
    return stateChangeFactory_;
}

void SernaDoc::postInit()
{
    contextAction_ = actionSet()->findAction(NOTR("docContext"));

    enable_serna_registration(actionSet(), !is_serna_registered());
}

bool update_interface(PropertyNode* iface, PropertyNode* builtin);

void SernaDoc::buildInterface(PropertyNode* iface, bool& isUpdated)
{
    PropertyNodePtr builtin = new PropertyNode(itemClass());
    if (!iface || itemClass() != iface->name() || !iface->firstChild()) {
        iface = builtin.pointer();
        docBuilder_->buildInterface(iface);
        DBG(DV.TEST) << "  Builtin Interface description used." << std::endl;
    }
    else {
        //! Updating user interface
        PropertyNodePtr builtin = new PropertyNode(itemClass());
        docBuilder_->buildInterface(builtin.pointer());
        isUpdated = update_interface(iface, builtin.pointer());
    }
    removeAllChildren();
    restore_ui_item(this, iface, this);
}

void SernaDoc::buildInterface()
{
    PropertyNodePtr iface = new PropertyNode(itemClass());
    PropertyNodePtr specific_path =
        iface->makeDescendant("#specific-file-path");
    Level level = BUILTIN_LEVEL;
    load_most_specific_file(this, iface.pointer(), NOTR(".sui"), level);
    if (specific_path->parent())
        specific_path->remove();
    else
        specific_path->setString(getLevelFile(BUILTIN_LEVEL) + NOTR(".sui"));
    specific_path->setName(tr("UI File Path"));
    itemProps()->makeDescendant("#specific-file-path")->
        appendChild(specific_path.pointer());
    bool is_updated = false;
    buildInterface(iface.pointer(), is_updated);
}

bool SernaDoc::isToMakeItem(const PropertyNode* itemProps) const
{
    if (0 == itemProps)
        return true;

    using namespace Sui;
    if (const PropertyNode* actionProps = itemProps->getProperty(ACTION)) {
        const String& actionName = actionProps->getString();
        if (!actionName.empty()) {
            if (Action* action = findAction(actionName)) {
                if (action->getBool("no-ax") && getBool("no-ax")) {
                    return false;
                }
            }
        }
    }

    const PropertyNode* origin = itemProps->getProperty(PLUGIN_ORIGIN);
    if (0 == origin)
        return true;
    return (pluginLoader().isLoaded(origin->getString(), this));
    return true;
}

BuiltinUiActions& SernaDoc::uiActions() const
{
    return static_cast<EventTranslator*>(dispatcher())->uiActions();
}

BuiltinActionGroups& SernaDoc::actionGroups() const
{
    return static_cast<EventTranslator*>(dispatcher())->actionGroups();
}

void SernaDoc::registerActionExecutor(ActionExecutor* executor)
{
    static_cast<EventTranslator*>(dispatcher())->
        registerActionExecutor(executor);
}

void SernaDoc::deregisterActionExecutor(const ActionExecutor* executor)
{
    static_cast<EventTranslator*>(dispatcher())->
        deregisterActionExecutor(executor);
}

void SernaDoc::showPageInfo(const String&) const
{
}

MessageStream* SernaDoc::progressStream() const
{
    return progressStream_.pointer();
}

static bool is_same_doc(const String& p1, const String& p2)
{
    return Url(p1) == Url(p2)
        || PathName::isSameFile(PathName(p1), PathName(p2));
}

bool SernaDoc::checkDsi(const PropertyNode* dsi) const
{
    String path = dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    for (Sui::Item* win = root()->firstChild();
         win; win = win->nextSibling()) {
        for (Sui::Item* doc = win->firstChild();
             doc; doc= doc->nextSibling()) {
            SernaDoc* serna_doc = dynamic_cast<SernaDoc*>(doc);
            if (serna_doc && serna_doc->getDsi()) {
                String doc_path = serna_doc->getDsi()->
                    getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
                if (is_same_doc(path, doc_path))
                    return (1 == showMessageBox(
                                MB_WARNING, tr("File Exists"),
                                tr("<qt><nobr>File %1</nobr> is aready opened."
                                   "<br/><b>Do you want to proceed anyway?</b>"
                                   "</qt>").arg(doc_path),
                                tr("Cancel"), tr("Ok")));
            }
        }
    }
    return true;
}

void SernaDoc::setDsi(PropertyNode* pn)
{
    dsi_ = pn;
}

//////////////////////////////////////////////////////////////////////

class CannotSaveWarningDialog;
class SaveAsDialog;

SIMPLE_COMMAND_EVENT_IMPL(SaveDocument, SernaDoc)

static bool save_with_autosave_check(SernaDoc* sd, PropertyNode* ptn = 0)
{
    bool savedOk = sd->saveDocument(ptn);
    if (savedOk)
        if (PropertyNode* nsp = sd->getDsi()->getProperty("#not-saved"))
            nsp->remove();
    return savedOk;
}

bool SaveDocument::doExecute(SernaDoc* sd, EventData*)
{
    return save_with_autosave_check(sd);
}

SIMPLE_COMMAND_EVENT_IMPL(SaveDocumentAs, SernaDoc)

bool SaveDocumentAs::doExecute(SernaDoc* sd, EventData*)
{
    PropertyTreeEventData dsi(sd->getDsi()->copy(true));
    if (!makeCommand<SaveAsDialog>(&dsi)->execute(sd, &dsi))
        return false;
    return save_with_autosave_check(sd, dsi.root());
}

PROPTREE_EVENT_IMPL(SaveDocumentWithDsi, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(SaveDocumentWithDsi, "PropertyTree", "-")

bool SaveDocumentWithDsi::doExecute(SernaDoc* sd, EventData*)
{
    return save_with_autosave_check(sd, ed_);
}

bool SernaDoc::saveDocument(PropertyNode* dsi)
{
    CommandEventPtr ev = makeSaveEvent(dsi);
    if (ev.isNull())
        return false;
    PropertyNodePtr old_dsi = getDsi()->copy(true);

    if (0 == dsi) {
        bool saveas = getDsi()->getSafeProperty("#do-save-as")->getBool();
        DDBG << "SernaDoc::saveDocument, saveas=" << saveas << std::endl;
        if (saveas)
            return makeCommand<SaveDocumentAs>()->execute(this);
        if (ev->execute(this)) {
            dsi = getDsi();
            if (0 == dsi->parent()) {
                DocSrcInfo::add_recent_document(dsi);
                saveView();
            }
            return true;
        }
        makeCommand<CannotSaveWarningDialog>()->execute(this);
        return false;
    }
    DDBG << "SernaDoc::saveDocument with DSI:\n";
    DBG_IF(DV.TEST) dsi->dump();
    if (ev->execute(this)) {
        getDsi()->remove();
        config().eventFactory().update();
        if (0 == old_dsi->getProperty(DocSrcInfo::IS_NEW_DOCUMENT))
            DocSrcInfo::add_recent_document(old_dsi.pointer());
        DocSrcInfo::add_recent_document(getDsi());
        saveView();
        if (0 != getDsi()->parent()) {
            config().eventFactory().update();
            config().save();
        }
        PropertyNode* node = getDsi()->getProperty("#do-save-as");
        if (node)
            node->remove();
        return true;
    }
    makeCommand<CannotSaveWarningDialog>()->execute(this);
    return false;
}

const char* DONT_SAVE_VIEW = NOTR("#dont-save-view");

void SernaDoc::saveView(String path)
{
    DBG(DV.TEST) << "SernaDoc::saveView: " << path << std::endl;
    //! Save document look & feel
    if (getBool(DONT_SAVE_VIEW)) {
        DBG(DV.VIEWS) << "Not saving view" << std::endl;
        return;
    }
    if (path.isEmpty())
        path = getLevelFile(RECENT_DOCUMENT_LEVEL);
    if (path.isEmpty()) {
        DBG(DV.TEST) << "saveView: no RECENT_DOCUMENT_LEVEL path\n";
        return;
    }
    PropertyNodePtr iface = new PropertyNode(NOTR("interface"));
    save_ui_item(this, iface.pointer(), true);
    //DBG_IF(DV.TEST) iface->dump();
    save_specific_file(iface->firstChild(), path, NOTR(".sui"));
    //! Save plugins properties
    DocumentStateEventData save_event(DocumentStateEventData::SAVE_PROPERTIES);
    save_event.prop_ = new PropertyNode(NOTR("path"), path);
    stateChangeFactory().dispatchEvent(&save_event);
}

DOCVIEW_EXPIMP void toggle_save_view(SernaDoc* doc, bool dontSave)
{
    doc->setBool(DONT_SAVE_VIEW, dontSave);
}

bool SernaDoc::restoreView(const String& restoreFrom)
{
    if (restoreFrom.isEmpty())
        return false;
    PropertyNode* restore_level = 0;
    PropertyNodePtr path_list =
        new PropertyNode(getLevelFile(RECENT_DOCUMENT_LEVEL));
    for (Level l = BUILTIN_LEVEL; l >= TEMPLATE_LEVEL; l = Level(l - 1)) {
        String path = getLevelFile(l);
        PropertyNode* level_prop = new PropertyNode(path);
        if (path_list->firstChild())
            path_list->firstChild()->insertBefore(level_prop);
        else
            path_list->appendChild(level_prop);
        level_prop->setBool(restore_level && !is_original_level(l));
        if (path == restoreFrom)
            restore_level = level_prop;
    }
    if (0 == restore_level)
        return false;
    DBG(DV.VIEWS) << "SernaDoc::restore_view:\n";
    DBG_IF(DV.VIEWS) path_list->dump();
    restore_specific_file(path_list.pointer(), NOTR(".sui"));
    DocumentStateEventData restore_event(
        DocumentStateEventData::RESTORE_PROPERTIES);
    restore_event.prop_ = path_list.pointer();
    stateChangeFactory().dispatchEvent(&restore_event);
    toggle_save_view(this, true);
    return true;
}

void save_view_on_close(SernaDoc* doc)
{
    if (doc->getDsi())
        DBG(DV.VIEWS) << "Saving view for: "
                     << DocSrcInfo::docPathFilename(doc->getDsi())
                     << std::endl;
    doc->saveView();
}

bool SernaDoc::closeDocument()
{
    DocumentStateEventData stateEd(DocumentStateEventData::PRE_CLOSE);
    bool okToClose = stateChangeFactory().dispatchEvent
        (&stateEd, DynamicEventFactory::And);
    //! Remove document if not modified
    if (!canCloseDocument() && okToClose) {
        save_view_on_close(this);
        return true;
    }
    const PropertyNode* pn = getDsi()->getProperty(
        "#app-messages/#save-changes-message");

    PropertyNode* inscription = getDsi()->getProperty(Sui::INSCRIPTION);
    String filename = (inscription && !inscription->getString().isEmpty())
        ? inscription->getString()
        : DocSrcInfo::docPathFilename(getDsi());

    String std_msg = (pn && !pn->getString().isEmpty())
        ? pn->getString()
        : String(tr("Do you want to save the changes to %1?").arg(filename));

    switch (showMessageBox(MB_WARNING, "", std_msg,
                           tr("&Yes"), tr("&No"), tr("&Cancel"))) {
        case 0:
            return saveDocument(0);
        case 1:
            save_view_on_close(this);
            return true;

        default:
            return false;
    }
    return false;
}

SernaDoc* SernaDoc::activeDocument()
{
    Sui::Item* root = dynamic_cast<Sui::Item*>(qApp);
    Sui::Item* win  = root->firstChild();
    for (; win; win = win->nextSibling()) {
        const QWidget* w = win->widget(0);
        if (w && (qApp->activeWindow() == w || w->isActiveWindow()))
            break;
    }
    if (0 == win)
        win = root->firstChild();
    Sui::Item* curr_item =
        static_cast<const Sui::MainWindow*>(win)->currentItem();
    return static_cast<SernaDoc*>(curr_item ? curr_item : win->firstChild());
}

///////////////////////////////////////////////////////////////

UICMD_EVENT_IMPL(SwitchDocTab, SernaDoc)

bool SwitchDocTab::doExecute(SernaDoc* sd, EventData*)
{
    const Sui::Action* subact = activeSubAction();
    Sui::Item* new_current = 0;
    if (subact) 
        new_current = sd->parent()->getChild(subact->siblingIndex() + 1);
    else if (!action()->firstChild()) {
        new_current = action()->getBool(NOTR("direction")) 
            ? sd->nextSibling() : sd->prevSibling();
        if (new_current && !new_current->prevSibling())
            return false;
    }        
    if (new_current && new_current != sd)
        sd->stackWidget()->currentChanged(new_current);
    return true;
}
