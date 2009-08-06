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
#include "core/debug_core.h"
#include "core/Serna.h"
#include "core/AxSerna.h"
#include "core/impl/WindowCommands.h"

#include "common/String.h"
#include "common/StringCvt.h"
#include "common/ScopeGuard.h"
#include "common/PropertyTree.h"
#include "common/PathName.h"

#include "ui/IconProvider.h"
#include "ui/UiAction.h"
#include "ui/ActionSet.h"

#include "docview/dv_defs.h"
#include "docview/SernaDoc.h"
#include "docview/PluginLoader.h"
#include "docview/Clipboard.h"

#include "utils/env_utils.h"
#include "utils/i18n_utils.h"
#include "utils/Properties.h"
#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "utils/config_defaults.h"
#include "utils/struct_autosave_utils.h"
#include "utils/SernaUiItems.h"
#include "utils/Version.h"
#include "dav/DavManager.h"

#include <QtSingleApplication>
#include <QFileInfo>
#include <QSplashScreen>
#include <QTimer>
#include <QStringList>
#include <QMessageBox>
#include <QPixmap>
#include <QFocusEvent>
#include <QEvent>
#include <QPointer>
#include <QStringList>
#include <QEventLoop>
#include <QtCore/QDate>

///////////////////////////////////////////////////////////////////////////

static void null_qt_message_handler(QtMsgType, const char *) {}

#ifdef WIN32
# include "utils/impl/win32/config_win32.h"
# include <windows.h>
#endif

using namespace Common;
class ExitSerna;

class QtSplashScreen : public QSplashScreen {
public:
    QtSplashScreen(QPixmap px, Qt::WindowType type) : QSplashScreen(px, type) {}
    ~QtSplashScreen()
    {
        std::cerr << "~QtSplashScreen(" << this << ')' << std::endl;
    }
};

static void register_serna(SernaDoc*);
const unsigned char REGISTRATION_REMIND_LATER_DAYS  5

class QtSerna : public QtSingleApplication,
                public Serna {

    //protected Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    QtSerna(int& argc, char** argv);
    ~QtSerna()
    {
        if (firstChild() && firstChild()->firstChild()) {
            SernaDoc* sd = dynamic_cast<SernaDoc*>(firstChild()->firstChild());
            makeCommand<ExitSerna>(0)->execute(sd);
        }
    }

    //! Prevents GUI freeze in long operation
    virtual bool        notify(QObject* receiver, QEvent* e);

    virtual void        propertyChanged(Common::PropertyNode* property);

    virtual void        exit() { quit(); }

    void                openFirstMainWindow(QWidget* w = 0);

public slots:
    //! Destroys everything in the right order
    void                handleParams(const QString&);
    void                hideSplash();
    void                sernaAboutToQuit();
    void                checkPending();
    void                startSerna();
    void                lastWindowClosed();
    void                openPendingFiles(const QString& files);

private:
    virtual const char* selfTypeId() const { return typeid(Serna).name(); }
    void                openDocument(const Common::String& filename,
                                     SernaDoc* doc = 0);

    void                restore_autosave();
    void                register_file_handlers();

    QPointer<QtSplashScreen>      splash_;
    QStringList         pendingFiles_;
    bool                exiting_;
    bool                isOpeningFiles_;
    Sui::MainWindow*    openDocWin_;
};

#include "apple_dochandler.i"

static const int OPEN_DOC_EVENT = QEvent::MaxUser - 256;

void QtSerna::checkPending()
{
    if (!pendingFiles_.empty())
        postEvent(this, new QEvent((QEvent::Type)OPEN_DOC_EVENT));
}

static void raise_doc(Sui::Item* item)
{
    if (item->firstChild()) {
        QWidget* w = item->firstChild()->widget(0);
        w->setActiveWindow();
        if (!w->isMaximized())
            w->showNormal();
        w->raise();
    }
}

////////////////////////////////////////////////////////////////////////////

bool QtSerna::notify(QObject* receiver, QEvent* e)
{
    DBG(CORE.APPEVENTS) << "QtSerna: got event(" << e << "), type="
        << e->type() << ", receiver=" << receiver
        << "(" << receiver->className() << ")" << std::endl;
    if (exiting_)
        return false;
    if (QEvent::Show == e->type() && receiver->isWidgetType()
        && ((QWidget*)receiver)->isTopLevel()) {
        DBG(CORE.APPEVENTS) << "Shown: " << receiver->name() << " "
                            << receiver->className() << std::endl;
        return QApplication::notify(receiver, e);
    }
    switch (e->type()) {
        case OPEN_DOC_EVENT: {
            if (qApp->activeModalWidget()) {
                pendingFiles_.clear();
                return true;
            }
            if (isOpeningFiles_)
                return true;
            raise_doc(this);
            ON_BLOCK_EXIT_VAL(isOpeningFiles_, true);
            while (!pendingFiles_.isEmpty()) {
                QString fileName(pendingFiles_.front());
                pendingFiles_.pop_front();
                SernaDoc* oDoc = 0;
                if (openDocWin_)
                    oDoc = dynamic_cast<SernaDoc*>(openDocWin_->firstChild());
                openDocument(fileName, oDoc);
                pendingFiles_.remove(fileName);
            }
            return true;
        }
        case QEvent::FocusOut: {
            // update clipboard on app focus-out event
            if (static_cast<const QFocusEvent*>(e)->reason() ==
                    Qt::ActiveWindowFocusReason) {
                bool ret = QApplication::notify(receiver, e);
                serna_clipboard().appFocusEvent(false, 0);
                return ret;
            }
            break;
        }
        case QEvent::Quit: {
            break;
        }
        default:
            break;
    }
    return QApplication::notify(receiver, e);
}

static const unsigned char serna_icons[] = {
#include "core_resource_data.cpp"
};

int Serna::exec(int& argc, char** argv)
{
    config().init(argc, argv);

    PropertyNode* cfgRoot = config().root();

#if defined(_WIN32) || defined(linux) && (!defined(_DEBUG) || defined(NDEBUG))
    String javaHome(cfgRoot->getString(NOTR("vars/java_home")));
    if (javaHome.empty()) {
        javaHome.append(config().getDataDir());
        javaHome.append(1, PathName::DIR_SEP).append(NOTR("jre"));
    }
    set_env(from_latin1(NOTR("JAVA_HOME")), javaHome);
#endif

    const bool isActiveX = ax_serna().isStartedByCOM();
    if (isActiveX)
        config().root()->makeDescendant(NOTR("#is-ax"))->setBool(true);

    set_trace_tags();

    QtSerna serna_app(argc, argv);

#ifdef __APPLE__
    serna_app.setStyle(new SernaMacStyle);
#endif // __APPLE__
    if (PropertyNode* pn = cfgRoot->getProperty(NOTR("vars/qt_plugins")))
        QApplication::addLibraryPath(pn->getString());

    QString message;
    if (!isActiveX) {
        using namespace CmdLineParams;
        const PropertyNode* fileArg = cfgRoot->getProperty(CMD_LINE_PARAMS,
                                                           CMD_FILE_ARGS);
        if (fileArg) {
            fileArg = fileArg->firstChild();
            for (; fileArg; fileArg = fileArg->nextSibling())
                message += fileArg->getString() + '\n';
        }
    }
    if (serna_app.sendMessage(message)) {
        DDBG << "Sent message!\n";
        return 0;
    }

    serna_app.initialize(false);
#ifdef __linux__
    setlocale(LC_NUMERIC, "C");
#endif // __linux__
    QTimer::singleShot(0, &serna_app, SLOT(startSerna()));

#if defined(_WIN32)
    // This prevents Qt to process WM_SETTINGCHANGE event when screensaver exits
    // or system wakes up from hibernation resulting in a slowdown or crash
    QApplication::setDesktopSettingsAware(false);
#endif
    return serna_app.QApplication::exec();
}

void QtSerna::propertyChanged(PropertyNode* property)
{
    DDBG << "\nchanged " << property->name() << std::endl;
    DDBG << " to value '" << property->getString() << '\'' << std::endl;
    if (App::SYS_FONT == property->name()) {
        QFont font;
        if (font.fromString(property->getString())) {
            setFont(font, true);
            postEvent(this, new QEvent(QEvent::ApplicationFontChange));
        }
    }
    else if (App::UI_LANG == property->name()) {
        DDBG << "loading translation " << property->getString() << std::endl;
        load_translation(property->getString());
        Item::languageChanged();
    }
    Serna::propertyChanged(property);
}

QtSerna::QtSerna(int& argc, char** argv)
  : QtSingleApplication(Version::currentVersion().getStrVersion(), argc, argv),
    splash_(0), exiting_(false), isOpeningFiles_(false)
{
    load_translation(config().root()->getSafeProperty("app/lang")->getString());
#ifdef NDEBUG
    qInstallMsgHandler(null_qt_message_handler);
#else // NDEBUG
    (void) null_qt_message_handler(QtDebugMsg, 0); // kill warning
#endif // NDEBUG
    install_apple_doc_handler();
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(sernaAboutToQuit()));
    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(lastWindowClosed()));
}

void QtSerna::lastWindowClosed()
{
    if (!has_visible_windows(this))
        QApplication::postEvent(qApp, new QEvent(QEvent::Quit));
}
// last-minute cleanup
void QtSerna::sernaAboutToQuit()
{
    DBG(CORE.TEST) << "QtSerna: About to quit core app\n";
    exiting_ = true;
    config().save();
}

class OpenDocumentWithDsi;
class FirstSernaWindow;
class OpenExamples;
class RegisterSerna;

void QtSerna::openDocument(const String& url, SernaDoc* serna_doc)
{
    DBG(CORE.TEST) << "OpenDocument: " << url << std::endl;
    if (!serna_doc) {
        if (!firstChild() || !firstChild()->firstChild()) {
            makeCommand<FirstSernaWindow>(0)->execute(this);
            if (splash_)
                splash_->hide();
        }
        serna_doc = dynamic_cast<SernaDoc*>(firstChild()->firstChild());
    }
    PropertyTreeEventData iparam;
    iparam.root()->makeDescendant(DocSrcInfo::DOC_PATH, url);
    makeCommand<OpenDocumentWithDsi>(&iparam)->execute(serna_doc, &iparam);
}

// This code is executed when another Serna instance is NOT detected

static void check_builtin_sui(const String& filename)
{
    PathName path(config().getDataDir());
    path.append(NOTR("ui")).append(filename);
    if (!path.exists()) {
        QMessageBox::critical(
            qApp->activeWindow(),
            qApp->translate("QtSerna", "Error Loading UI Description"),
            qApp->translate("QtSerna", "No such file: %1")
            .arg(String(path.name())),
            tr("&Exit"));
        exit(0);
    }
}

void QtSerna::hideSplash()
{
    if (splash_) {
//        splash_->hide();
//        splash_->deleteLater();
//        qApp->flush();
//        splash_ = 0;
    }
}

#ifdef __APPLE__
namespace Sui {
    UI_EXPIMP extern bool is_mac_app_menu;
} // namespace Sui
#endif // __APPLE__

void QtSerna::openFirstMainWindow(QWidget* w)
{
    PropertyNode* cfgRoot = config().root();
    const bool isActiveX = cfgRoot->getSafeProperty(NOTR("#is-ax"))->getBool();

    PropertyNode* appProp = cfgRoot->makeDescendant(App::APP);
    PropertyNode* langProp = appProp->makeDescendant(App::UI_LANG);
    propertyChanged(langProp);

    langProp->addWatcher(this);

    Sui::icon_provider().registerBuiltinIcons(serna_icons, String());

    const PropertyNode* cmdParams =
        cfgRoot->getSafeProperty(CmdLineParams::CMD_LINE_PARAMS);

//    QTimer* timer = new QTimer(this);
    if (!isActiveX &&
        !cmdParams->getSafeProperty("#no-splash")->getBool() &&
        !appProp->getProperty("no-splash")) {

        QPixmap logo_pix = Sui::icon_provider().getPixmap(NOTR("SernaSplash"));
        splash_ = new QtSplashScreen(logo_pix, Qt::WindowStaysOnTopHint);
        splash_->show();
        splash_->raise();
//        qApp->flush();

//        connect(timer, SIGNAL(timeout()), SLOT(hideSplash()));
//        timer->start(3000, true);
    }

#ifdef __APPLE__
    if (cmdParams->getProperty("#guitest"))
	Sui::is_mac_app_menu = false;
#endif // __APPLE__

    PropertyNode* sys_font = appProp->makeDescendant(App::SYS_FONT,
                                                     font().toString(), false);
    if (sys_font)
        propertyChanged(sys_font);
    sys_font->addWatcher(this);

    check_builtin_sui(NOTR("EmptyDocument.sui"));
    check_builtin_sui(NOTR("StructDocument.sui"));
    check_builtin_sui(NOTR("PlainDocument.sui"));

    String icpath(appProp->getSafeProperty(App::ICON_PATH)->getString());
    Sui::icon_provider().registerIconsFromDir(icpath, String());

    register_file_handlers();

    makeCommand<FirstSernaWindow>((EventData*)w)->execute(this); //show main window
    if (splash_) {
        if (Sui::Item* mainWin = firstChild()) {
            splash_->finish(mainWin->widget());
        }
    }

    pluginLoader().loadFor(NOTR("start-up"), 0);
    SernaDoc* doc = dynamic_cast<SernaDoc*>(firstChild()->firstChild());

    if (!isActiveX) {
        PropertyNode* fileArgs =
            cmdParams->getProperty(CmdLineParams::CMD_FILE_ARGS);
        int opened_docs = 0;
        if (fileArgs) {
            PropertyNode* fileArg = fileArgs->firstChild();
            ON_BLOCK_EXIT_VAL(isOpeningFiles_, true);
            ON_BLOCK_EXIT_OBJ(*this, &QtSerna::checkPending);
            for (; 0 != fileArg; fileArg = fileArg->nextSibling()) {
                openDocument(fileArg->getString());
                ++opened_docs;
            }
        }
        if (opened_docs) {
//            timer->stop();
//            hideSplash();
            return;
        }
        restore_autosave();

	register_serna(doc);

        const PropertyNode* examples = cfgRoot->getProperty(Examples::EXAMPLES);
        if (examples && !examples->getSafeProperty
            (Examples::DONT_SHOW_ON_START)->getBool()) {
//                while (timer->isActive())
//                    processEvents(QEventLoop::ExcludeUserInputEvents);
                makeCommand<OpenExamples>()->execute(doc);
        }
//        while (timer->isActive())
//            processEvents();
        hideSplash();
    }
    setQuitOnLastWindowClosed(!isActiveX);
}

void QtSerna::startSerna()
{
    setQuitOnLastWindowClosed(false);

    connect(this, SIGNAL(messageReceived(const QString&)),
            this, SLOT(handleParams(const QString&)));

    if (firstChild() && dynamic_cast<SernaDoc*>(firstChild()->firstChild()))
        return;
    openFirstMainWindow();
}

void QtSerna::restore_autosave()
{
    using namespace AutoSaveUtils;
    Sysids sysids;
    if (PropertyNode* pn = sysids.first()) {
        ON_BLOCK_EXIT_VAL(isOpeningFiles_, true);
        ON_BLOCK_EXIT_OBJ(*this, &QtSerna::checkPending);
        for (String sysid; pn;) {
            sysid = pn->getString();
            PropertyTree pt;
            if (Dav::DavManager::instance().getResourceInfo(sysid,
                pt.root()) != Dav::DAV_RESULT_OK) {
                if (restore_auto_save(sysid)) {
                    openDocument(sysid);
                    hideSplash();
                }
                clean_auto_save(sysid);
            }
            pn = sysids.first();
        }
        sysids.clear();
    }
}

void QtSerna::register_file_handlers()
{
    PropertyNode* handlers = config().root()->
                             makeDescendant(FileHandler::HANDLER_LIST_TEMP);
#ifdef WIN32
    QStringList lst = QStringList::split(',', FileHandler::REGISTERED_VIEWERS);
    for (uint i = 0; i < lst.size(); i++) {
        String path = cfg::get_registered_viewer_path('.' + lst[i]).name();
        if (!path.isEmpty() && QFileInfo(path).exists()) {
            PropertyNode* app  = new PropertyNode(FileHandler::APP);
            app->appendChild(new PropertyNode(FileHandler::APP_EXT,
                                              String(lst[i])));
            app->appendChild(new PropertyNode(FileHandler::APP_PATH, path));
            handlers->appendChild(app);
        }
    }
#endif
#ifdef __linux__
    PropertyNode* app  = new PropertyNode(FileHandler::APP);
    app->appendChild(new PropertyNode(FileHandler::APP_EXT,
                                      NOTR("html")));
    app->appendChild(new PropertyNode(FileHandler::APP_PATH,
        NOTR("/usr/bin/firefox")));
    handlers->appendChild(app);
#endif
}

static bool is_child(QWidget* parent, QWidget* child)
{
    for (; child; child = child->parentWidget())
        if (child == parent)
            return true;
    return false;
}

static Sui::MainWindow* find_opendoc_window(QtSerna* qtSerna)
{
    using namespace Sui;
    Item* firstHiddenWin = 0;
    std::list<Item*> visibleWindows;
    MainWindow* openDocWin = 0;
    for (Item* win = qtSerna->firstChild(); win; win = win->nextSibling()) {
        if (!win->getBool("ax")) {
            if (!win->getBool("hidden"))
                visibleWindows.push_back(win);
            else if (!firstHiddenWin)
                firstHiddenWin = win;
        }
    }
    if (visibleWindows.empty()) {
        if (firstHiddenWin)
            openDocWin = dynamic_cast<MainWindow*>(firstHiddenWin);
        else {
            qtSerna->openFirstMainWindow();
            openDocWin = dynamic_cast<MainWindow*>(qtSerna->lastChild());
        }
    }
    else {
        std::list<Item*>::const_iterator it = visibleWindows.begin();
        QWidget* activeWindow = qApp->activeWindow();
        if (activeWindow && activeWindow->isWindow()) {
            for (; visibleWindows.end() != it; ++it) {
                QWidget* w = (*it)->widget();
                if (is_child(activeWindow, w)) {
                    openDocWin = dynamic_cast<MainWindow*>(*it);
                    break;
                }
            }
        }
        if (!openDocWin)
            openDocWin = dynamic_cast<MainWindow*>(visibleWindows.front());
    }
    return openDocWin;
}

void QtSerna::openPendingFiles(const QString& message)
{
    using namespace Sui;
    if (MainWindow* mainWin = find_opendoc_window(this)) {
        if (QWidget* w = mainWin->widget(0)) {
            w->setActiveWindow();
            if (!w->isMaximized())
                w->showNormal();
            w->raise();
            mainWin->setBool("hidden", false);
        }
        openDocWin_ = mainWin;
        QStringList lst = QStringList::split('\n', message);
        if (lst.size()) {
            for (int i = 0; i < lst.size(); i++)
                if (!lst[i].isEmpty())
                    pendingFiles_.push_back(lst[i]);
            checkPending();
        }
    }
}

void QtSerna::handleParams(const QString& message)
{
    QMetaObject::invokeMethod(this, "openPendingFiles",
                              Qt::QueuedConnection, Q_ARG(QString, message));
}

void open_first_main_window(QWidget* w)
{
    if (QtSerna* qtSerna = dynamic_cast<QtSerna*>(qApp))
        qtSerna->openFirstMainWindow(w);
}

static void register_serna(SernaDoc* doc)
{
    PropertyNode* reg = config().root()->
	makeDescendant(Registration::REGISTRATION);

    const PropertyNode* already_registered = config().root()->
	makeDescendant(Registration::ALREADY_REGISTERED);

    const PropertyNode* dont_show =
	reg->getProperty(Registration::DONT_SHOW_ON_START);

    if (already_registered && already_registered->getBool()) {
	Sui::Action* action = doc->actionSet()->findAction(NOTR("registerSerna"));
	action->setEnabled(false);
	return;
    }

    if (dont_show && dont_show->getBool())
	return;

    bool ok;
    int later_day =
	reg->getSafeProperty(Registration::LATER_DAY)->getInt(&ok);
    int today = QDate::currentDate().toJulianDay();

    if (ok && later_day != today)
	return;

    PropertyTreeEventData result;
    if (!makeCommand<RegisterSerna>()->execute(doc, &result))
	reg->makeDescendant(Registration::LATER_DAY)->
	    setInt(today + REGISTRATION_REMIND_LATER_DAYS);

    return;
}


#include "moc/QtSerna.moc"
