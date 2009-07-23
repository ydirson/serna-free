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
#include "sapi/app/Config.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/common/doctags.h"
#include "common/XList.h"

#include <QKeyEvent>
#include <QtGui/QTextBrowser>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QTextCursor>

#include "QtPyDialog.h"
#include "QtPyDialogBase.hpp"

using namespace SernaApi;

QtPyDialog* QtPyDialog::py_dialog_  = 0;
bool QtPyDialog::pyConsoleAutoShow_ = true;

class QtPyDialogImpl : public QDialog,
                       public QtPyDialog,
                       public Ui::QtPyDialogBase {
    Q_OBJECT
public:
    class HistoryItem : public Common::XListItem<HistoryItem> {
    public:
        HistoryItem(const SernaApi::SString& cmd)
            : cmd_(cmd) {}
        SernaApi::SString cmd_;
    };
    typedef Common::XList<HistoryItem> HistoryList;

    typedef void (*pyrss_func)(const char*);

    QtPyDialogImpl(QWidget* w,
               const SernaApi::PropertyNode&,
               const SernaApi::SernaDoc& sd);
    virtual ~QtPyDialogImpl();

    virtual void propertyChanged(SernaApi::PropertyNodeWatcher*);
    virtual void show() { QDialog::show(); }
    void         refill();
    void         goNext(bool backwards);

    void         executeCommand();
    void         clearMessages();

protected slots:
    void        on_closeButton__clicked() { reject(); }
    void        on_helpButton__clicked() { help(); }
    void        on_commandLineEdit__returnPressed() { executeCommand(); }
    void        on_clearButton__clicked() { clearMessages(); }

public slots:
    void        reject();
    void        help();

private:
    static const int HISTORY_SIZE = 100;

    SernaApi::PropertyNode pyMessages_;
    SernaApi::PropertyNode first_;
    SernaApi::PropertyNode last_;
    pyrss_func             pfunc_;
    HistoryList            historyList_;
    int                    historySize_;
    HistoryItem*           currentItem_;
    SernaApi::SernaDoc     sernaDoc_;
};

static const char* const PYTHON_RSS_PROP  =
    NOTR("#PythonInterp-PyRunSimpleString");

inline void scroll_to_bottom(const QTextBrowser* browser)
{
    browser->verticalScrollBar()->setValue(
                        browser->verticalScrollBar()->maximum());
}

inline void remove_block(const QTextBrowser* browser, const int pos)
{
    QTextCursor cursor(browser->document()->findBlockByNumber(pos));
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();
}

class TextEditEventFilter : public QObject {
public:
    TextEditEventFilter(QtPyDialogImpl* dialog)
        : dialog_(dialog) {}

protected:
    bool eventFilter(QObject*, QEvent *e)
    {
        if (e->type() != QEvent::KeyPress)
            return false;
        QKeyEvent* kev = static_cast<QKeyEvent*>(e);
        if (kev->state() & (Qt::ControlButton|Qt::AltButton|
            Qt::MetaButton|Qt::ShiftButton))
                return false;
        switch (kev->key()) {
            case Qt::Key_Up:
            case Qt::Key_Prior:
                dialog_->goNext(true);
                return true;
            case Qt::Key_Down:
            case Qt::Key_Next:
                dialog_->goNext(false);
                return true;
            default:
                return false;
        }
    }
    QtPyDialogImpl* dialog_;
};

QtPyDialogImpl::QtPyDialogImpl(QWidget* w,
                               const SernaApi::PropertyNode& pyMessages,
                               const SernaApi::SernaDoc& sd)
    : QDialog(w), pyMessages_(pyMessages), historySize_(0), currentItem_(0),
      sernaDoc_(sd)
{
    setModal(false);
    setupUi(this);

    pfunc_ = (pyrss_func) SernaConfig::root().
        getSafeProperty(PYTHON_RSS_PROP).getInt();
    if (0 == pfunc_)
        commandLineEdit_->setEnabled(false);
    textEdit_->clear();
    first_ = pyMessages_.firstChild();
    last_  = pyMessages_.lastChild();
    for (PropertyNode n = first_; n; n = n.nextSibling())
        textEdit_->append(n.getString());
    if (first_)
        scroll_to_bottom(textEdit_);

    activateWindow();
    show();
    commandLineEdit_->installEventFilter(new TextEditEventFilter(this));
    commandLineEdit_->setFocus();
}

void QtPyDialogImpl::goNext(bool backwards)
{
    if (0 == currentItem_) {
        currentItem_ = backwards
            ? historyList_.lastChild()
            : historyList_.firstChild();
        if (currentItem_)
            commandLineEdit_->setText(currentItem_->cmd_);
        return;
    }
    if (backwards) {
        if (currentItem_->prevSibling())
            currentItem_ = currentItem_->prevSibling();
    } else {
        if (currentItem_->nextSibling())
            currentItem_ = currentItem_->nextSibling();
    }
    commandLineEdit_->setText(currentItem_->cmd_);
}

void QtPyDialogImpl::clearMessages()
{
    pyMessages_.removeAllChildren();
    textEdit_->clear();
    first_ = last_ = 0;
}

void QtPyDialogImpl::executeCommand()
{
    if (0 == pfunc_ || commandLineEdit_->text().isEmpty())
        return;
    historyList_.appendChild(new HistoryItem(commandLineEdit_->text()));
    currentItem_ = 0;
    if (historySize_ >= HISTORY_SIZE && historyList_.firstChild())
        delete historyList_.firstChild();
    else
        ++historySize_;
    PropertyNode cmd(NOTR("line"));
    cmd.setString(NOTR(">>> ") + commandLineEdit_->text());
    pyMessages_.appendChild(cmd);
    (*pfunc_)(commandLineEdit_->text().utf8().data());
    commandLineEdit_->clear();
}

void QtPyDialogImpl::propertyChanged(SernaApi::PropertyNodeWatcher* watcher)
{
    int nItems = textEdit_->document()->blockCount();
    if (last_ && last_ == pyMessages_.lastChild() && nItems) {
        QString text = textEdit_->document()->findBlock(nItems - 1).text();
        if (text.startsWith(last_.getString()))
            return;
        remove_block(textEdit_, nItems - 1);
        textEdit_->append(pyMessages_.lastChild().getString());
    } else if (first_ && first_ != pyMessages_.firstChild()) {
        first_ = pyMessages_.firstChild();
        remove_block(textEdit_, 0);
    } else if (last_ && last_ != pyMessages_.lastChild()) {
        last_ = pyMessages_.lastChild();
        if (nItems == 1 && first_ != last_) {
            textEdit_->clear();
            PropertyNode pn = pyMessages_.firstChild();
            for (; pn; pn = pn.nextSibling())
                textEdit_->append(pn.getString());
        } else
            textEdit_->append(last_.getString());
        last_.addWatcher(watcher);
    } else {
        first_ = last_ = pyMessages_.firstChild();
        textEdit_->append(last_.getString());
        last_.addWatcher(watcher);
    }

    scroll_to_bottom(textEdit_);
}

void QtPyDialogImpl::reject()
{
    pyConsoleAutoShow_ = autoShow_->isChecked();
    py_dialog_ = 0;
    deleteLater();
}

void QtPyDialogImpl::help()
{
    if (0 != sernaDoc_)
        sernaDoc_.showHelp(DOCTAG(PYTHON_CONSOLE_DIALOG));
}

QtPyDialogImpl::~QtPyDialogImpl()
{
    while (historyList_.firstChild())
        delete historyList_.firstChild();
    py_dialog_ = 0;
}

///////////////////////////////////////////////////////////////////

void QtPyDialog::make(const SernaApi::PropertyNode& pyMessages,
                      const SernaApi::SernaDoc& sd)

{
    QWidget* parent = qApp->activeWindow();
    if (parent) {
        if (parent->inherits("QSplashScreen"))
            parent = 0;
        else
            while (parent && !parent->inherits("QMainWindow"))
                parent = parent->parentWidget();
    }
    py_dialog_ = new QtPyDialogImpl(parent, pyMessages, sd);
}

#include "moc/QtPyDialog.moc"
