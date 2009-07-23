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
#include "docview/SernaDoc.h"
#include "docview/impl/debug_dv.h"

#include "common/PathName.h"
#include "common/StringCvt.h"

#include "utils/Config.h"
#include "utils/HelpAssistant.h"

#include "docutils/doctags.h"

#include "ui/UiAction.h"
#include "ui/UiDocument.h"

#include <QApplication>
#include <QAssistantClient>
#include <QMessageBox>
#include <QPixmap>

#include "docview/MessageBoxDialogBase.hpp"

USING_COMMON_NS
using namespace Sui;

///////////////////////////////////////////////////////////////////////////

class MessageBoxDialog : public QDialog, 
                         public Ui::MessageBoxDialogBase {
public:
    MessageBoxDialog(QWidget* parent, const String& msgStr) 
        : QDialog(parent, NOTR("msgbx"), TRUE, 
                  Qt::WStyle_Customize | Qt::WStyle_DialogBorder | 
                  Qt::WStyle_Title | Qt::WStyle_SysMenu)
    {
        setupUi(this);
        icon_->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning));
        text_->setText(msgStr);
    }
};

///////////////////////////////////////////////////////////////////////////

class StageEventFilter : public QObject {
public:
    StageEventFilter()
        : QObject(qApp) 
    {
        qApp->installEventFilter(this);
    }
    virtual bool eventFilter(QObject*, QEvent* ev)
    {
        if (ev->type() >= QEvent::User)
            return true;
        switch (ev->type()) {
            default:
                return (ev->type() >= QEvent::User);
            case QEvent::FocusIn:
            case QEvent::FocusOut:
            case QEvent::DragEnter:
            case QEvent::DragLeave:
            case QEvent::DragMove:
            case QEvent::Drop:
            case QEvent::Enter:
            case QEvent::EnterWhatsThisMode:
            case QEvent::HoverEnter:
            case QEvent::HoverLeave:
            case QEvent::HoverMove:
            case QEvent::InputMethod:
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
            case QEvent::Leave:
            case QEvent::NonClientAreaMouseButtonDblClick:
            case QEvent::NonClientAreaMouseButtonPress:
            case QEvent::NonClientAreaMouseButtonRelease:
            case QEvent::NonClientAreaMouseMove:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::MouseTrackingChange:
            case QEvent::QueryWhatsThis:
            case QEvent::Shortcut:
            case QEvent::ShortcutOverride:
            case QEvent::StatusTip:
            case QEvent::TabletMove:
            case QEvent::TabletPress:
            case QEvent::TabletRelease:
            case QEvent::WhatsThis:
            case QEvent::WhatsThisClicked:
            case QEvent::Wheel:
                return true;
        }
    }
};

///////////////////////////////////////////////////////////////////////////

void SernaDoc::showStageInfo(const String& stage) const
{
    if (!contextAction_)
        return;
    DDBG << "PROGRESS INFO:" << stage << std::endl;
    if (stage.isEmpty() && contextAction_->get(INSCRIPTION).isEmpty())
        return;
    if (QApplication::overrideCursor()) {
        if (stage.isEmpty()) {
            QApplication::restoreOverrideCursor();
            contextAction_->property(INSCRIPTION)->setString(stage);
            return;
        }
    } else {
        if (stage.isEmpty())
            return;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor), true);
    }
    contextAction_->property(INSCRIPTION)->setString(stage);
#ifdef Q_WS_X11
    qApp->syncX();
#endif // Q_WS_X11
    StageEventFilter ev_filter;
    for (uint i = 0; qApp->hasPendingEvents() && i < 100; ++i)
        qApp->processEvents(QEventLoop::AllEvents, 1);
}

void SernaDoc::showContextInfo(const String& context) const
{
    if (!contextAction_)
        return;
    DBG(DV.TEST) << "StageInfo: " << context << std::endl;
    if (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();
    contextAction_->property(INSCRIPTION)->setString(context);
}

int SernaDoc::showMessageBox(MessageBoxSeverity severity,
                             const String& origCaption,
                             const String& message,
                             const String& button0,
                             const String& button1,
                             const String& button2) const
{
    showStageInfo();
    String caption = tr("Serna");
    if (!origCaption.isEmpty())
        caption += ": " + origCaption;
    QWidget* parentWidget = widget(0) ? widget(0) : qApp->activeWindow();
    
    switch (severity) {
        case MB_INFO:
            return QMessageBox::information(parentWidget, caption, message,
                                            button0, button1, button2);

        case MB_WARNING:
            return QMessageBox::warning(parentWidget, caption, message,
                                        button0, button1, button2);

        case MB_CRITICAL: 
            {
                //QMessageBox mbox(QMessageBox::Critical, caption, message, QMessageBox::Ok, parentWidget, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
                //return mbox.exec();
            
                             
            return QMessageBox::critical(parentWidget, caption, message,
                                         button0, button1, button2);
            }
            
        case CHECKED_WARNING:
        {
            MessageBoxDialog mb(parentWidget, message);
            mb.adjustSize();
            if (mb.exec())
                return (mb.checkBox_->isChecked() ? 1 : -1);
        }
    }
    return -1;
}

