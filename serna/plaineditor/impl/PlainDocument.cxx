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
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.
//

#include "plaineditor/PlainDocument.h"
#include "plaineditor/PlainEditor.h"
#include "plaineditor/qt/QtPlainEditor.h"
#include "plaineditor/impl/debug_pe.h"

#include "docview/MessageView.h"
#include "docview/MessageTree.h"
#include "docview/EventTranslator.h"
#include "docview/Finder.h"
#include "docview/DocumentStateEventData.h"
#include "docview/PluginLoader.h"

#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/Config.h"

#include "common/PathName.h"
#include "common/PropertyTree.h"
#include "common/XTreeIterator.h"
#include "common/CommandEvent.h"

#include "ui/UiItems.h"
#include "ui/UiStackItem.h"
#include "ui/UiItemSearch.h"

#include "genui/PlainDocumentActions.hpp"

using namespace Common;

class DoOpenPlainDocument;
class PlainGoToMessageOrigin;
class PlainFinder;

PlainDocument::PlainDocument(PropertyNode* dsi,
                             MessageTreeNode* mtn,
                             const DocBuilder* builder)
    : SernaDoc(builder),
      MessageTreeHolder(mtn),
      MessageViewHolder(this)
{
    DBG(PE.TEST) << "PlainDocument cons: " << this << std::endl;

    builder->buildActions(dispatcher(), actionSet());
    dsi->makeDescendant("#do-not-save-view");

    addItemMaker(Sui::FINDER, 
                 new Sui::CustomItemMaker(Sui::makeUiItem<PlainFinder>),
                 Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);

    addItemMaker(Sui::MESSAGE_VIEW, 
                 new Sui::CustomItemMaker(Sui::makeUiItem<MessageView>),
                 Sui::MAIN_WINDOW_WIDGET + String(' ') + Sui::SIMPLE_WIDGET);

    PlainDocumentActions* actions =
        static_cast<PlainDocumentActions*>(&uiActions());
    contextAction_ = actions->docContext();

    plainEditor_ = make_plain_editor(this, dsi);//new QtPlainEditor(this, dsi);

    setNextContext(plainEditor_.pointer());

    pluginLoader().loadFor(NOTR("text-mode"), this);
    set_trace_tags();

    buildInterface();
    setMessageTreeHolder(this);

    if (messageView()) {
        plainEditor_->setNextContext(messageView());
        messageView()->gotoMessage().subscribe(
            makeCommand<PlainGoToMessageOrigin>, plainEditor_.pointer());
    }

    itemProps()->makeDescendant(Sui::ICON)->setString(NOTR("document"));

    PropertyNode* prop = dsi->getProperty(Sui::INSCRIPTION);
    if (prop)
        itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
            prop->getString());
    else
        itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
            DocSrcInfo::docPathFilename(dsi));

    itemProps()->makeDescendant(Sui::TOOLTIP)->setString
        (DocSrcInfo::makeDsiTooltip(dsi));

    DocumentStateEventData newDocEd(DocumentStateEventData::BUILD_INTERFACE);
    stateChangeFactory().dispatchEvent(&newDocEd);
    if (messageView())
        messageView()->update();
}

PlainDocument::~PlainDocument()
{
    DBG(PE.TEST) << "~PlainDocument: " << this << std::endl;
}

Sui::Item* PlainDocument::makeItem(const String& itemClass,
                                  PropertyNode* properties) const
{
    if (Sui::PLAIN_EDITOR == itemClass) {
        if (0 == plainEditor_->parent())
            return plainEditor_.pointer();
        return 0;
    }
    return SernaDoc::makeItem(itemClass, properties);
}

void PlainDocument::grabFocus() const
{
    plainEditor_->grabFocus();
}

void PlainDocument::updateTooltip()
{
    itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
        DocSrcInfo::docPathFilename(getDsi()));
    itemProps()->makeDescendant(Sui::TOOLTIP)->
        setString(DocSrcInfo::makeDsiTooltip(getDsi()));
}


bool PlainDocument::canCloseDocument() const
{
    return plainEditor_ ? plainEditor_->canCloseDocument() : true;
}

class DoSavePlainDocWithDsi;

Common::CommandEventPtr PlainDocument::makeSaveEvent(PropertyNode* dsi) const
{
    PropertyTreeEventData ed(dsi);
    return makeCommand<DoSavePlainDocWithDsi>(&ed);
}

String PlainDocument::getLevelFile(Level level, String* comment) const
{
    switch (level) {
        case TEMPLATE_LEVEL: {
            PathName path(config().getConfigDir());
            path.append(NOTR("plaintext_view"));
            if (comment)
                *comment = tr("Plain-text View", "SaveView for:");
            return path.name();
        }
        case ORIGINAL_TEMPLATE_LEVEL: {
            PathName path(config().getDataDir());
            path.append(NOTR("plaintext_view"));
            if (comment)
                *comment = tr("Plain-text View", "SaveView for:");
            return path.name();
        }
        case BUILTIN_LEVEL: {
            PathName path(config().getDataDir());
            path.append(NOTR("ui")).append(NOTR("PlainDocument"));
            if (comment)
                *comment = tr("Builtin Plain-text View", "SaveView for:");
            return path.name();
        }
        default:
            break;
    }
    if (comment)
        *comment = String();
    return String();
}

class ExecutePlainUiAction : public CommandEvent<PlainEditor> {
public:
    ExecutePlainUiAction(ActionExecutor* ue)
        : ue_(ue) {}
    virtual bool doExecute(PlainEditor*, EventData*)
    {
        ue_->execute();
        return true;
    }
private:
    ActionExecutor* ue_;
};

CommandEventPtr PlainDocument::makeUiEventExecutor(ActionExecutor* ue)
{
    return new ExecutePlainUiAction(ue);
}

