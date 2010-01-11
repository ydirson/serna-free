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
#include "docview/Clipboard.h"

#include "plaineditor/PlainDocument.h"
#include "plaineditor/PlainEditor.h"
#include "plaineditor/qt/QtPlainEditor.h"
#include "plaineditor/impl/debug_pe.h"
#include "ui/UiItemSearch.h"

#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "utils/file_utils.h"

#include "common/Encodings.h"
#include "common/Url.h"
#include "common/OwnerPtr.h"
#include "common/Debug.h"

#include "dav/DavManager.h"
#include "dav/IoStream.h"
#include "dav/DavQIODevice.h"
#include "dav/Session.h"

#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/Origin.h"
#include "spgrovebuilder/SpGroveBuilder.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include <QContextMenuEvent>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerxml.h>

#include <map>
#include <iostream>

#include "genui/PlainDocumentActions.hpp"

using namespace Common;
using namespace GroveLib;
using namespace XmlCatalogs;

static const bool PE_DEBUG =
#ifndef NDEBUG
false;
#else
false;
#endif

class EntityInfo {
public:
    EntityInfo(const Url& epath, const String& id, bool isRel = false)
     :  id_(id), path_(epath), isRoot_(false),
        isRelative_(isRel), isModified_(false), line_(0), col_(0) {}

    EntityInfo()
     :  isRoot_(false), isRelative_(false),
        isModified_(false), line_(0), col_(0) {}

    String              id_;
    String              name_;
    Url                 path_;
    QString             buffer_;
    bool                isRoot_;
    bool                isRelative_;
    bool                isModified_;
    int                 line_;
    int                 col_;
};

typedef std::list<EntityInfo> EntityBuffers;

static void update_combo_text(Sui::Action* top_action, const String& text)
{
    Sui::Action* action = top_action->getSubAction(text);
    if (action)
        top_action->setInt(Sui::CURRENT_ACTION, action->siblingIndex());
}

static EntityBuffers::value_type* get_root_einfo(EntityBuffers& eb)
{
    for (EntityBuffers::iterator it = eb.begin(); eb.end() != it; ++it) {
        if (it->isRoot_)
            return &*it;
    }
    return 0;
}

static EntityBuffers::value_type* get_einfo_by_url(EntityBuffers& eb,
                                                   const String& url)
{
    DBG_TAG(PE, TEST, PE_DEBUG);
    DBG(PE_TEST) << "url = " << url << std::endl;
    for (EntityBuffers::iterator it = eb.begin(); eb.end() != it; ++it) {
        DBG(PE_TEST) << "check url = " << it->path_ << std::endl;
        if (url == it->path_)
            return &*it;
    }
    return 0;
}

static EntityBuffers::value_type* get_einfo_by_id(EntityBuffers& eb,
                                                  const String& id)
{
    DBG_TAG(PE, TEST, PE_DEBUG);
    DBG(PE_TEST) << "id = " << id << std::endl;
    for (EntityBuffers::iterator it = eb.begin(); eb.end() != it; ++it) {
        DBG(PE_TEST) << "check id = " << it->id_ << std::endl;
        if (id == it->id_)
            return &*it;
    }
    return 0;
}

class QtPlainEditor;

class SernaScintilla : public QsciScintilla {
public:
    SernaScintilla(QtPlainEditor* pe, QObject* evf)
        : QsciScintilla(0/*, NOTR("PlainEditor_QsciScintilla")*/),
          pe_(pe), eventFilter_(evf)
    {
        installEventFilter(evf);
    }
    ~SernaScintilla()
    {
        if (eventFilter_)
            removeEventFilter(eventFilter_);
    }
    virtual void contextMenuEvent(QContextMenuEvent* e);

private:
    QtPlainEditor* pe_;
    QPointer<QObject> eventFilter_;
};

class QtPlainEditor : public QObject, public PlainEditor {
    Q_OBJECT
public:
    QtPlainEditor(PlainDocument* doc, Common::PropertyNode* dsi);
    ~QtPlainEditor();

    virtual bool        find(const Common::String& text, bool matchCase,
                             bool reverse, bool start);
    virtual void        cut();
    virtual void        copy();
    virtual void        insert(const String& text);
    virtual void        paste(const String& text);
    virtual String      selectedText() const;
    virtual void        setCursor(int line, int column);

    virtual void        undo();
    virtual void        redo();

    virtual bool        openDocument(const Common::String& file);
    virtual bool        openSingleFile(const Common::String&)
    {
        return false;
    }
    virtual bool        switchToEntity(const String&);
    virtual bool        saveDocument(const Common::String&);

    ///////////////////////////////////////////////////////////////////////

    //! Reimplemented from UiItem
    void                grabFocus() const;
    virtual bool        doAttach();
    virtual QWidget*    widget(const Sui::Item* child) const;

    virtual bool        canCloseDocument();

    void                resetFind();
    void                resetSelection();

public slots:
    void                copyAvailable(bool);
    void                undoAvailable(bool);
    void                redoAvailable(bool);
    void                cursorPositionChanged(int, int);
    void                modificationChanged(bool m);
    void                textChanged();

private:
    void                init();
    QsciScintilla&      textEdit()
    {
        if (!textEdit_)
            init();
        return *textEdit_;
    }
    void                setText(const QString& text)
    {
        textEdit().blockSignals(true);
        textEdit().setText(text);
        textEdit().blockSignals(false);
    }

    bool                readEntity(EntityInfo* einfo);
    void                initEntityBuffers();
    void                updateActionsState();
    bool                isModified() const
    {
        bool rv = false;
        EntityBuffers::const_iterator it = entities_.begin();
        for (; !rv && entities_.end() != it; ++it)
            rv = rv || it->isModified_;
        return rv;
    }
    bool save_url(const Url& file, const String& text,
                  Encodings::Encoding encoding) const;
    bool read_url(const Url& url, QString& dst, Encodings::Encoding encoding);
    bool copy_url(const Url& src, const Url& dst) const;
    void report_read_error(const QString& err, const String& url) const;
    void report_write_error(const QString& err, const String& url) const;

    struct FindStruct {
        FindStruct() : isReverse_(false), isCaseSensitive_(false) {}
        String pattern_;
        bool isReverse_;
        bool isCaseSensitive_;
    };

    Common::Encodings::Encoding get_encoding() const;
    void                        updateComboList();

    FindStruct              findStruct_;
    QPointer<QsciScintilla> textEdit_;
    QsciLexer*              lexer_;
    EntityBuffers           entities_;

    EntityInfo*             currentEntity_;
};

void SernaScintilla::contextMenuEvent(QContextMenuEvent* e)
{
    Sui::Item* item = 
        pe_->findItem(Sui::ItemName(NOTR("plainEditorContextMenu")));
    if (item)
        item->showContextMenu(e->globalPos());
    e->accept();
}

void QtPlainEditor::grabFocus() const
{
    if (!textEdit_->hasFocus())
        textEdit_->setFocus();
}

PlainEditor* make_plain_editor(PlainDocument* doc, Common::PropertyNode* dsi)
{
    return new QtPlainEditor(doc, dsi);
}

static const char XML_PI_RE[] =
    NOTR("\\s*\\<\\?xml(?=\\s).*\\s+encoding\\s*=\\s*[\"']([^\"']+)[\"'].*\\?\\>.*");

Encodings::Encoding QtPlainEditor::get_encoding() const
{
    QRegExp qre(XML_PI_RE, false);
    String enc_str;
    if (textEdit_ && qre.exactMatch(textEdit_->text()))
        enc_str = qre.cap(1);
    else
        enc_str = plainDoc()->getDsi()->getSafeProperty(DocSrcInfo::ENCODING)
                  ->getString();

    if (enc_str.empty())
        return Encodings::UTF_8;

    Encodings::Encoding enc = Encodings::encodingByName(enc_str);
    if (enc == Encodings::XML)
        enc = Encodings::UTF_8;
    return enc;
}

void QtPlainEditor::resetFind()
{
    findStruct_.pattern_.clear();
}

////////////////////////////////////////////////////////////////////////

// Event filter is required to override bindings of QTextEdit

class PlainCopy;
class PlainPaste;
class PlainCut;

class PeEventFilter : public QObject {
public:
    PeEventFilter(QtPlainEditor* pe)
        : pe_(pe) {}
    ~PeEventFilter()
    {
        DBG(PE.TEST) << "~PeEventFilter: " << this << std::endl;
    }
protected:
    bool eventFilter(QObject*, QEvent *e);
    QtPlainEditor* pe_;
};

////////////////////////////////////////////////////////////////////////

void QtPlainEditor::init()
{
    textEdit_ = new SernaScintilla(this, new PeEventFilter(this));
    textEdit_->SendScintilla(textEdit_->SCI_SETMOUSEDOWNCAPTURES, 0, 0L);
    
    lexer_ = new QsciLexerXML(textEdit_/*, NOTR("PlainTextEditLexer")*/);

    textEdit_->setLexer(lexer_);

    textEdit_->setFocusPolicy(Qt::StrongFocus);
    textEdit_->setUtf8(true);
    setCursor(0, 0);

    QObject::connect(textEdit_, SIGNAL(copyAvailable(bool)),
                     this, SLOT(copyAvailable(bool)));
    QObject::connect(textEdit_, SIGNAL(modificationChanged(bool)),
                     this, SLOT(modificationChanged(bool)));
    QObject::connect(textEdit_, SIGNAL(cursorPositionChanged(int,int)),
                     this, SLOT(cursorPositionChanged(int, int)));
    QObject::connect(textEdit_, SIGNAL(textChanged()),
                     this, SLOT(textChanged()));
}

QtPlainEditor::QtPlainEditor(PlainDocument* doc, PropertyNode* dsi)
    : PlainEditor(doc, dsi), textEdit_(0), lexer_(0), currentEntity_(0)
{
}

bool PeEventFilter::eventFilter(QObject*, QEvent *e)
{
    DBG(PE.TEST) << "PeEventFilter: got " << e->type() << std::endl;

    if (!pe_ || !pe_->documentItem())
        return false;
    switch (e->type()) {
        case QEvent::FocusIn:
            pe_->resetFind();
            serna_clipboard().appFocusEvent(true, 0);
            return false;
        case QEvent::FocusOut:
            serna_clipboard().setText(true, pe_->selectedText());
            return false;
        case QEvent::Accel: {
            const QKeyEvent* kev = static_cast<const QKeyEvent*>(e);
            DBG(PE.TEST) << "Accel, state=" << kev->state() << std::endl;
            if ((kev->state() & Qt::KeyButtonMask) == Qt::ShiftButton &&
                kev->key() == Qt::Key_Insert) {
                    makeCommand<PlainPaste>()->execute(pe_);
                    return true;
            }
            if (!(kev->state() & Qt::ControlButton))
                return false;
            switch (kev->key()) {
                case Qt::Key_C:
                case Qt::Key_Insert:
                    makeCommand<PlainCopy>()->execute(pe_);
                    return true;
                case Qt::Key_V:
                    makeCommand<PlainPaste>()->execute(pe_);
                    return true;
                case Qt::Key_X:
                    makeCommand<PlainCut>()->execute(pe_);
                    return true;
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}

void QtPlainEditor::cut()
{
    textEdit().cut();
}

void QtPlainEditor::resetSelection()
{
    int line, index;
    textEdit().getCursorPosition(&line, &index);
    textEdit().setSelection(0, 0, 0, 0);
    textEdit().setCursorPosition(line, index);
    textEdit().ensureCursorVisible();
}

void QtPlainEditor::copy()
{
    if (textEdit().hasSelectedText()) 
        textEdit().copy();
}

void QtPlainEditor::undo()
{
    textEdit().undo();
}

void QtPlainEditor::redo()
{
    textEdit().redo();
}

bool QtPlainEditor::doAttach()
{
    if (!textEdit_)
        init();
    if (!Sui::Item::doAttach())
        return false;
    textEdit().setFocus();
    return true;
}

QWidget* QtPlainEditor::widget(const Sui::Item*) const
{
    return textEdit_;
}

bool QtPlainEditor::find(const String& text, bool matchCase,
                         bool reverse, bool start)
{
    if (!start && !text.empty() && text == findStruct_.pattern_
        && matchCase == findStruct_.isCaseSensitive_
        && reverse == findStruct_.isReverse_) {

        return textEdit().findNext();
    }
    findStruct_.pattern_            = text;
    findStruct_.isCaseSensitive_    = matchCase;
    findStruct_.isReverse_          = reverse;

    return textEdit().findFirst(text,
                                false,       // not a regular expression
                                matchCase,
                                false,       // whole words?
                                false,       // wrap at the end of text?
                                reverse);
}

void QtPlainEditor::paste(const String& str)
{
    insert(str);
}

void QtPlainEditor::insert(const String& text)
{
    QString to_insert = text;
    if (1 == text.length() && 0 != lexer_) {
        QFontMetrics qfm(lexer_->defaultFont());
        QChar symbol(text[0]);
        if (!qfm.inFont(symbol)) {
            to_insert = QString().sprintf(NOTR("&#x%.4X;"),
                symbol.unicode());
        }
    }
    // note by apg: insert in scintilla does not work correctly, so we simulate
    // it with keyboard event...
    QKeyEvent kp_ev(QEvent::KeyPress, 0, 0, 0, to_insert);
    QApplication::sendEvent(&textEdit(), &kp_ev);
}

String QtPlainEditor::selectedText() const
{
    return textEdit_->selectedText();
}

void QtPlainEditor::copyAvailable(bool v)
{
    uiActions().copy()->setEnabled(v);
    uiActions().cut()->setEnabled(v);
}

void QtPlainEditor::undoAvailable(bool v)
{
    int cnt = 0;
    EntityBuffers::iterator it;
    for (it = entities_.begin(); it != entities_.end(); ++it)
        if (!it->buffer_.isNull())
            cnt++;
    if (1 == cnt)
        modificationChanged(v);
    redoAvailable(textEdit().isRedoAvailable());
}

void QtPlainEditor::redoAvailable(bool v)
{
    uiActions().redo()->setEnabled(v);
}

void QtPlainEditor::textChanged()
{
    modificationChanged(textEdit().isModified());
}

void QtPlainEditor::modificationChanged(bool)
{
    DBG_TAG(PE, TEST, PE_DEBUG);
    DBG(PE_TEST) << "modificationChanged: "
                 << "isModified() = " << textEdit().isModified()
                 << ", undoAvail() = " << textEdit().isUndoAvailable()
                 << ", redoAvail() = " << textEdit().isRedoAvailable()
                 << std::endl;
    if (currentEntity_)
        currentEntity_->isModified_ = textEdit().isModified();
    uiActions().saveDocument()->setEnabled(textEdit().isModified()
                                           || isModified());
    uiActions().undo()->setEnabled(textEdit().isUndoAvailable());
    redoAvailable(textEdit().isRedoAvailable());
}

static String make_cursor_ctx_str(int line, int col)
{
    QString ctx(qApp->translate("QtPlainEditor", "Line: %1 Pos: %2"));
    return ctx.arg(String::number(line + 1)).arg(String::number(col + 1));
}

void QtPlainEditor::cursorPositionChanged(int line, int pos)
{
    uiActions().docContext()->set(Sui::INSCRIPTION,
                                  make_cursor_ctx_str(line, pos));

    undoAvailable(textEdit().isUndoAvailable());
    redoAvailable(textEdit().isRedoAvailable());
}

void QtPlainEditor::updateActionsState()
{
    uiActions().saveDocument()->setEnabled(isModified());
    uiActions().undo()->setEnabled(textEdit().isUndoAvailable());

    uiActions().redo()->setEnabled(textEdit().isRedoAvailable());

    bool hasSelectedText = textEdit().hasSelectedText();
    uiActions().copy()->setEnabled(hasSelectedText);
    uiActions().cut()->setEnabled(hasSelectedText);

    int line(0), col(0);
    textEdit().getCursorPosition(&line, &col);

    String cursor_ctx(make_cursor_ctx_str(line, col));
    uiActions().docContext()->set(Sui::INSCRIPTION, cursor_ctx);

    if (0 != currentEntity_)
        uiActions().saveDocumentAs()->setEnabled(currentEntity_->isRoot_);
}

static void report_url_error(PlainDocument* pd,
                             const QString& caption,
                             const QString& err,
                             const String& url)
{
    pd->showMessageBox(SernaDoc::MB_CRITICAL, caption, err.arg(url), tr("&Ok"));
}

void
QtPlainEditor::report_write_error(const QString& err, const String& url) const
{
    report_url_error(plainDoc(), tr("URL write error"), err, url);
}

void
QtPlainEditor::report_read_error(const QString& err, const String& url) const
{
    report_url_error(plainDoc(), tr("URL read error"), err, url);
}

bool QtPlainEditor::save_url(const Url& url, const String& text,
                             Encodings::Encoding encoding) const
{
    Dav::IoStream ios;
    bool ok = true;
    if (Dav::DAV_RESULT_OK == 
      Dav::DavManager::instance().open(url, Dav::DAV_OPEN_WRITE, ios)) {
        ios.setAddCR(config().getProperty(
            App::APP_CRLF_LINE_BREAKS)->getBool());
        ios.setEncoding(encoding);
        ios << text;
        ok = !ios.close() && ok;
    } else
        ok = false;
    if (!ok) 
        report_write_error(tr("Cannot open file/URL '%1' for writing"), url);
    return ok;
}

static bool rewind(const Url& url, Dav::IoRequestHandle** pioh)
{
    using namespace Dav;
    IoRequestHandle* ioh = *pioh;
    if (DAV_RESULT_OK != ioh->setPosition(0)) {
        ioh->close();
        DavManager& dav_mgr = DavManager::instance();
        if (DAV_RESULT_OK != dav_mgr.open(url, DAV_OPEN_READ, &ioh))
            return false;
        *pioh = ioh;
    }
    return true;
}

UTILS_EXPIMP nstring detect_encoding(const String& url);

static const char TR_OPEN_READ_ERROR[] = QT_TRANSLATE_NOOP(
    "QtPlainEditor", "Cannot open file/URL '%1' for reading");

bool QtPlainEditor::read_url(const Url& url, QString& dst,
                             Encodings::Encoding encoding)
{
    if (url.isLocal()) {
        QFileInfo fi(String(url.absolute()));
        if (!fi.isFile()) {
            report_read_error(tr(TR_OPEN_READ_ERROR), url);
            return false;
        }
    }

    using namespace Dav;

    IoRequestHandle* ioh = 0;
    DavManager& dav_mgr = DavManager::instance();
    if (DAV_RESULT_OK == dav_mgr.open(url, DAV_OPEN_READ, &ioh)) {
        OwnerPtr<DavQIODevice> qdev(new DavQIODevice(ioh));

        nstring enc = detect_encoding(url);
        if (enc.empty()) {
            if (!rewind(url, &ioh)) {
                qdev.reset(0);
                if (DAV_RESULT_OK != dav_mgr.open(url, DAV_OPEN_READ, &ioh)) {
                    report_read_error(tr(TR_OPEN_READ_ERROR), url);
                    return false;
                }
                qdev.reset(new DavQIODevice(ioh));
            }
            enc = Encodings::qtEncodingName(encoding).latin1();
        }
        qdev->setTextModeEnabled(true);
        QTextStream is(&*qdev);
        if (QTextCodec* codec = QTextCodec::codecForName(enc.c_str())) {
            is.setCodec(codec);
        }
        else {
            is.setEncoding(QTextStream::UnicodeUTF8);
        }

        QString text(is.read());
        QTextStream::Status status = is.status();
        qdev->close();

        if (QTextStream::Ok == status) {
            dst = text;
            return true;
        }
    }
    report_read_error(tr(TR_OPEN_READ_ERROR), url);
    return false;
}

bool QtPlainEditor::copy_url(const Url& src, const Url& dst) const
{

    if (!FileUtils::copy_file(src, dst)) {
        report_write_error(tr("Cannot open file/URL '%1' for writing"), dst);
        return false;
    } 
    return true;
}

static String make_combo_text(const String& name, const String& path)
{
    unsigned ct_len = name.size() + path.size() + 3; // 3 extra chars = ' ()'
    String ct;
    ct.reserve(ct_len);
    ct.append(name).append(1, ' ').append(1, '(').append(path).append(1, ')');
    return ct;
}

static bool is_url_path_changed(const Url& lhs, const Url& rhs)
{
    if (lhs == rhs)
        return false;
    if (lhs.isLocal() && rhs.isLocal()) {
        if (lhs.absolute()[Url::DIRPATH] == rhs.absolute()[Url::DIRPATH])
            return false;
    }
    else {
        if (lhs[Url::DIRPATH]   == rhs[Url::DIRPATH]   &&
            lhs[Url::PORT]      == rhs[Url::PORT]      &&
            lhs[Url::HOST]      == rhs[Url::HOST]      &&
            lhs[Url::PASSWORD]  == rhs[Url::PASSWORD]  &&
            lhs[Url::USER]      == rhs[Url::USER]      &&
            lhs[Url::PROTOCOL]  == rhs[Url::PROTOCOL])
            return false;
    }
    return true;
}

bool QtPlainEditor::saveDocument(const Common::String& path)
{
    EntityBuffers::value_type* root_val = get_root_einfo(entities_);
    if (0 == root_val)
        return false;

    if (currentEntity_) {
        currentEntity_->buffer_ = textEdit().text();
        currentEntity_->isModified_ = currentEntity_->isModified_
                                      || textEdit().isModified();
    }

    DBG_TAG(PE, TEST, PE_DEBUG);

    bool ok = true;
    bool is_root_path_changed = false;

    Url new_root_path(path);

    EntityBuffers::value_type& root_einfo = *root_val;
    is_root_path_changed = is_url_path_changed(root_einfo.path_, new_root_path);
    Url orig_root_path(root_einfo.path_);
    EntityBuffers::iterator it = entities_.begin();
    for (; it != entities_.end(); ++it) {
        EntityBuffers::value_type& einfo = *it;

        Url path(einfo.path_);
        if (einfo.isRoot_)
            einfo.path_ = new_root_path;

        if (is_root_path_changed && einfo.isRelative_) {
            Url rel_path(orig_root_path.relativePath(einfo.path_));
            einfo.path_ = new_root_path.combinePath2Path(rel_path);
            DBG(PE_TEST) << "relative entity: orig_path = " << path << std::endl
                         << "\trel_path = " << rel_path << std::endl
                         << "\tnew_path = " << einfo.path_ << std::endl;
        }
        einfo.id_ = make_combo_text(einfo.name_,
                                    Url(einfo.path_)[Url::FILENAME]);

        DBG(PE_TEST) << "entity id = " << einfo.id_
                     << ", orig path = " << path
                     << ", save to = " << einfo.path_ << std::endl;

        if (einfo.isModified_) {
            einfo.isModified_ = 
                !save_url(einfo.path_, einfo.buffer_, get_encoding());
            ok = !einfo.isModified_ && ok;
        }
        else {
            if (path != einfo.path_) {
                ok = copy_url(path, einfo.path_) && ok;
            }
        }
    }
    updateComboList();
    if (currentEntity_)
        update_combo_text(uiActions().entities(), currentEntity_->id_);
    textEdit().setModified(!ok);
    updateActionsState();
    return ok;
}

bool QtPlainEditor::readEntity(EntityInfo* einfo)
{
    DBG_TAG(PE, TEST, PE_DEBUG);

    const String& url = einfo->path_;

    if (url.empty())
        return false;

    plainDoc()->showStageInfo();
    QApplication::restoreOverrideCursor();

    QString text;
    if (read_url(url, text, get_encoding())) {
        setText(text);
        return true;
    }
    return false;
}

bool QtPlainEditor::openDocument(const String& file)
{
    DBG_TAG(PE, TEST, PE_DEBUG);

    plainDoc()->showStageInfo();
    QApplication::restoreOverrideCursor();

    bool isRoot = false;
    PropertyNode* dsi = plainDoc()->getDsi();

    String fname;
    if (PropertyNode* fn_node = dsi->getProperty(DocSrcInfo::CURSOR_FILENAME)) {
        fname = fn_node->getString();
        fn_node->remove();
    }
    else {
        isRoot = true;
        fname = file;
    }

    if (fname.empty())
        return false;

    initEntityBuffers();
    updateComboList();

    EntityBuffers::value_type* einfo = 0;
    if (isRoot)
        einfo = get_root_einfo(entities_);
    else
        einfo = get_einfo_by_url(entities_, fname);

    if (0 == einfo)
        return false;

    DBG(PE_TEST) << "einfo->path = " << einfo->path_ << std::endl;

    if (!readEntity(einfo))
        return false;

    currentEntity_ = einfo;
    update_combo_text(uiActions().entities(), currentEntity_->id_);

    if (PropertyNode* pos_prop = dsi->getProperty(DocSrcInfo::LINE_NUMBER))
        setCursor(pos_prop->getInt() - 1, 0);

    if (PropertyNode* tloc_prop = dsi->getProperty(DocSrcInfo::CURSOR_TREELOC))
        tloc_prop->remove();

    updateActionsState();
    return true;
}

bool QtPlainEditor::switchToEntity(const String& eid)
{
    using namespace Sui;

    EntityBuffers::value_type* einfo = get_einfo_by_id(entities_, eid);
    if (0 == einfo || einfo == currentEntity_)
        return false;

    if (currentEntity_) {
        bool is_modified = textEdit().isModified();
        const bool is_null_buffer = currentEntity_->buffer_.isNull();
        if (!is_null_buffer && !is_modified)
            is_modified = currentEntity_->buffer_ != textEdit().text();
        currentEntity_->isModified_ = is_modified;
        if (is_modified || is_null_buffer)
            currentEntity_->buffer_ = textEdit().text();
        textEdit().getCursorPosition(&currentEntity_->line_,
                                     &currentEntity_->col_);
    }

    if (einfo->buffer_.isNull()) {
        if (!readEntity(einfo)) {
            if (currentEntity_)
                update_combo_text(uiActions().entities(), currentEntity_->id_);
            return false;
        }
    }
    else
        setText(einfo->buffer_);

    currentEntity_ = einfo;

    setCursor(currentEntity_->line_, currentEntity_->col_);
    updateActionsState();
    return true;
}

void QtPlainEditor::updateComboList()
{
    Sui::Action* ent_action = uiActions().entities();
    ent_action->removeAllChildren();
    EntityBuffers::const_iterator it = entities_.begin();
    for (; it != entities_.end(); ++it) {
        PropertyNodePtr props(new PropertyNode(Sui::ACTION));
        props->makeDescendant(Sui::NAME, it->id_, true);
        props->makeDescendant(Sui::INSCRIPTION, it->id_, true);
        ent_action->appendChild(Sui::Action::make(props.pointer()));
    }
}

void QtPlainEditor::initEntityBuffers()
{
    entities_.clear();

    PropertyNode* dsi = plainDoc()->getDsi();
    String doc(dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString());

    String root_eid(make_combo_text(NOTR("Root"), Url(doc)[Url::FILENAME]));
    entities_.push_back(EntityInfo(doc, root_eid));
    entities_.back().isRoot_ = true;
    entities_.back().name_ = NOTR("Root");

    if (PropertyNode* mtimes = dsi->getProperty(DocSrcInfo::MODIFIED_TIME)) {
//        mtimes->dump();
        for (PropertyNode* p = mtimes->firstChild(); p; p = p->nextSibling()) {
            const String& path = p->getSafeProperty("path")->getString();
            if (path.empty())
                continue;
            const String& name = p->name();
            String ename;
            if (DocSrcInfo::WATCHED_ENTITY == name) {
                ename = p->getSafeProperty("entity-name")->getString();
            }
            else if (DocSrcInfo::WATCHED_FILE == name && path != doc) {
                ename += NOTR("xincluded");
            }
            else
                continue;

            bool isRelative = p->getSafeProperty("is-relative")->getBool();
            String eid(make_combo_text(ename, Url(path)[Url::FILENAME]));
            entities_.push_back(EntityInfo(path, eid, isRelative));
            entities_.back().name_ = ename;
        }
    }
}

void QtPlainEditor::setCursor(int line, int col)
{
    textEdit().setCursorPosition(line, col);
    if (!textEdit().hasFocus())
        textEdit().setFocus();
}

bool QtPlainEditor::canCloseDocument()
{
    int line, pos;
    textEdit().getCursorPosition(&line, &pos);
    plainDoc()->getDsi()->makeDescendant(DocSrcInfo::LINE_NUMBER,
        String::number(line + 1), true);
    return uiActions().saveDocument()->getBool(Sui::IS_ENABLED);
}

QtPlainEditor::~QtPlainEditor()
{
    delete textEdit_;
}

#include "plaineditor/moc/QtPlainEditor.moc"

