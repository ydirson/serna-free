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
//#include "docview/Serna.h"

#include "ui/MainWindow.h"
#include "ui/impl/qt/QtMainWindow.h"
#include "core/core_defs.h"
#include "core/debug_core.h"
#include "core/EmptyDocument.h"
#include "core/Serna.h"
#include "core/DocBuilders.h"
#include "core/impl/LicenseChecker.h"

#include "utils/Config.h"
#include "utils/Version.h"
#include "utils/SernaCatMgr.h"
#include "utils/DocTemplate.h"
#include "utils/struct_autosave_utils.h"
#include "utils/DocSrcInfo.h"

#include "common/PropertyTree.h"
#include "common/StringCvt.h"
#include "common/asserts.h"

#include "docview/dv_utils.h"
#include "docview/EventTranslator.h"
#include "docview/DocumentStateEventData.h"
#include "grove/Grove.h"

#include <QApplication>
#include <list>

using namespace Common;
using namespace Sui;

static bool has_ax_windows(Sui::Item* rootItem)
{
    using namespace Sui;
    if (rootItem)
        for (Item* i = rootItem->firstChild(); i; i = i->nextSibling())
            if (i->getBool("ax"))
                return true;
    return false;
}

void make_main_window(Sui::Item* rootItem, QWidget* ctl = 0)
{
    QPoint pos;
    QSize  size;
    Item* firstChild = rootItem->firstChild();
    const bool hasAxWindows = has_ax_windows(rootItem);
    if (!firstChild || hasAxWindows) {
        pos = QPoint(config().getProperty("geometry/x")->getInt(),
                     config().getProperty("geometry/y")->getInt());
        size = QSize(config().getProperty("geometry/width")->getInt(),
                     config().getProperty("geometry/height")->getInt());
    }
    else {
        QWidget* prev_win = rootItem->lastChild()->widget(0);
        pos = QPoint(prev_win->pos().x(), prev_win->geometry().y());
        size = prev_win->size();
    }
    //! Making new window
    Sui::MainWindow* main_win =
        Sui::MainWindow::make(pos.x(), pos.y(), size.width(), size.height(),
                              String::null(), true, ctl);

    PropertyNode* cfgRoot = config().root();
    const bool isActiveX = cfgRoot->getSafeProperty("#is-ax")->getBool();
    if (!firstChild && isActiveX && 0 == ctl) {
        main_win->setBool("hidden", isActiveX);
        main_win->widget()->setVisible(!isActiveX);
    }

    SernaDoc* empty_doc = new EmptyDocument(new EmptyDocumentBuilder);
    main_win->appendChild(empty_doc);
    rootItem->appendChild(main_win);

#if !defined(__APPLE__) && !defined(_WIN32)
    //! Polishing look and feel
    main_win->setIcon(NOTR("serna_app"));
#endif

    QString qcaption = QString(SERNA_VERSION_STR)
        .arg(Version::version()).arg(Version::build_date());

    main_win->setCaption(qcaption);
    main_win->set(NOTR("#caption"), qcaption);

    if (!main_win->prevSibling()) {
        DocumentStateEventData state_ev(DocumentStateEventData::POST_INIT);
        empty_doc->stateChangeFactory().dispatchEvent(&state_ev);
    }
}

/*
 */
SIMPLE_COMMAND_EVENT_IMPL(FirstSernaWindow, Serna)

class SetTraceTags;

bool FirstSernaWindow::doExecute(Serna* serna, EventData*)
{
    DBG(CORE.TEST) << "Event: FirstSernaWindow\n";

    if (Sui::Item* mainWin = serna->firstChild()) {
        if (dynamic_cast<SernaDoc*>(mainWin->firstChild()) &&
            !mainWin->getBool("ax"))
            return true;
    }
    (void) SernaCatMgr::instance();
    (void) DocTemplate::DocTemplateHolder::instance();
    make_main_window(serna);
    return true;
}

/*
 */
SIMPLE_COMMAND_EVENT_IMPL(NewSernaWindow, SernaDoc)

bool NewSernaWindow::doExecute(SernaDoc* sernaDoc, EventData*)
{
    DBG(CORE.TEST) << "Event: NewSernaWindow\n";
    make_main_window(sernaDoc->parent()->parent());
    return true;
}

typedef std::list<RefCntPtr<SernaDoc> > SernaDocList;

static bool close_documents(Sui::Item* mainWin, SernaDocList& removedDocs)
{
    while (mainWin->firstChild() != mainWin->lastChild()) {
        SernaDoc* serna_doc = dynamic_cast<SernaDoc*>(mainWin->lastChild());
        if (0 == serna_doc)
            return false;
        if (serna_doc->closeDocument()) {
            PropertyNode* dsi = serna_doc->getDsi();
            String topSysid(dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->
                            getString());
            AutoSaveUtils::clean_auto_save(topSysid);
            removedDocs.push_back(serna_doc);
            serna_doc->removeItem();
        }
        else
            return false;
    }
    if (mainWin->firstChild()) {
        SernaDoc* no_doc = dynamic_cast<SernaDoc*>(mainWin->firstChild());
        if (no_doc)
            save_view_on_close(no_doc);
    }
    return true;
}

static void exit_serna(Sui::Item* root)
{
    using namespace Sui;
    SernaDocList removed_docs;

    bool hasAxWindows = false;
    std::list<Item*> nonAxWindows;
    for (Item* win = root->firstChild(); win; win = win->nextSibling()) {
        if (win->getBool("ax")) {
            DBG(CORE.AX) << "Serna has ActiveX windows at exit" << std::endl;
            hasAxWindows = true;
        }
        else {
            if (!close_documents(win, removed_docs))
                return;
            nonAxWindows.push_back(win);
        }
    }

    QWidget* mainwin = 0;
    if (!nonAxWindows.empty())
        mainwin = nonAxWindows.front()->widget(0);

    if (mainwin) {
        QPoint pos = mainwin->pos();
        QSize size = mainwin->size();
        PropertyNode* root_prop = config().root();
        root_prop->makeDescendant("geometry/x")->setInt(pos.x());
        root_prop->makeDescendant("geometry/y")->setInt(pos.y());
        root_prop->makeDescendant("geometry/width")->setInt(size.width());
        root_prop->makeDescendant("geometry/height")->setInt(size.height());
    }
    if (!hasAxWindows) {
        DBG(CORE.TEST) << "FAST EXIT!\n";
        dynamic_cast<Serna*>(root)->exit();
    }
    else {
        std::list<Item*>::const_iterator it = nonAxWindows.begin();
        for (; nonAxWindows.end() != it; ++it)
            (*it)->removeItem();
    }
}

bool has_visible_windows(Sui::Item* rootItem, Sui::Item* exceptItem = 0)
{
    using namespace Sui;
    bool hasVisibleWindows = false;
    if (rootItem) {
        for (Item* w = rootItem->firstChild(); w; w = w->nextSibling())
            if (!exceptItem || w != exceptItem)
                hasVisibleWindows |= !w->getBool("hidden");
    }
    return hasVisibleWindows;
}

static bool remove_window(Sui::Item* main_win)
{
    if (0 == main_win)
        return false;
    Sui::RootItem* root = dynamic_cast<Sui::RootItem*>(main_win->parent());
    if (0 == root)
        return false;
    //! Save geometry of the last window
    if (!has_visible_windows(main_win->parent(), main_win)) {
        exit_serna(root);
        return false;
    }
    //! Remove all documents and close window
    SernaDocList removed_docs;
    close_documents(main_win, removed_docs);
    removed_docs.clear();
    if (main_win->firstChild() != main_win->lastChild())
        return false;
    main_win->removeItem();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(CloseSernaWindow, SernaDoc)

bool CloseSernaWindow::doExecute(SernaDoc* sernaDoc, EventData*)
{
    //! NOTE: sernaDoc is EmptyDocument
    DBG(CORE.TEST) << "Event: CloseSernaWindow\n";
    return remove_window(sernaDoc->parent());
}

SIMPLE_COMMAND_EVENT_IMPL(ExitSerna, SernaDoc)

bool ExitSerna::doExecute(SernaDoc* sernaDoc, EventData*)
{
    DBG(CORE.TEST) << "Event: ExitSerna\n";
    exit_serna(sernaDoc->root());
    return false;
}

static void save_document(Sui::Item* win)
{
    for (win = win->firstChild(); 0 != win; win = win->nextSibling()) {
        SernaDoc* sernaDoc = dynamic_cast<SernaDoc*>(win);
        if (0 != sernaDoc) {
            DBG(CORE.TEST) << "Save all: " << sernaDoc->getDsi()->
                getSafeProperty("doc-src")->getString() << std::endl;
            sernaDoc->saveDocument();
        }
    }
}

SIMPLE_COMMAND_EVENT_IMPL(SaveAllDocuments, SernaDoc)

bool SaveAllDocuments::doExecute(SernaDoc* sernaDoc, EventData*)
{
    if (Sui::Item* win = sernaDoc->root()) {
        for (win = win->firstChild(); 0 != win; win = win->nextSibling())
            save_document(win);
    }
    return false;
}

