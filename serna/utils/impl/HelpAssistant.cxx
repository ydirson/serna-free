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

#include "HelpAssistantImpl.h"

#include "utils/Config.h"
#include "utils/env_utils.h"
#include "utils/file_utils.h"
#include "docutils/doctags.h"

#include "common/Url.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/Singleton.h"
#include "common/PropertyTree.h"
#include "utils/utils_debug.h"
#include "utils/appver.hpp"
#include "utils/ElementHelp.h"

#include <QAssistantClient>
#include <QApplication>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QTextStream>

#include <stdlib.h>

using namespace Common;

#ifndef _WIN32
static const char ASSISTANT_EXE[] = NOTR("assistant");
#else
static const char ASSISTANT_EXE[] = NOTR("assistant.exe");
#endif
static const char HELP_PATH_PROP[] = NOTR("app/help-path");

HelpAssistantImpl::HelpAssistantImpl()
{
#if defined(_WIN32)
    String home_dir(config().getHomeDir());
    set_env(from_local_8bit(NOTR("HOME")), home_dir);
#endif
    set_env(from_latin1(NOTR("SERNA_VERSION_ID")),
            from_latin1(SERNA_VERSION_ID));

    Url exec_dir(FileUtils::argv0());
    DDBG << "HelpAssistant: exec_dir = '" << exec_dir << '\'' << std::endl;
    String assistant_exe(exec_dir.combinePath2Path(String(ASSISTANT_EXE)));
    DDBG << "HelpAssistant: exe = '" << assistant_exe << '\'' << std::endl;
    QFileInfo fi(assistant_exe);
    if (!fi.exists()) {
         assistant_exe = QLibraryInfo::location(QLibraryInfo::BinariesPath)
         + QLatin1Char('/') + QLatin1String(ASSISTANT_EXE);
         fi.setFile(assistant_exe);
         if (fi.exists())
             assistantExe_ = assistant_exe;
    }
    Url help_dir(config().getProperty(HELP_PATH_PROP)->getString());
    String qhc = help_dir.combineDir2Path(String(NOTR("serna.qhc")));
    helpCollectionFile_ = qhc;
}

void HelpAssistantImpl::showHelpUrl() const
{
    QTextStream cmd(assistantProc_);
    QString cmdStr = QLatin1String("setSource ") + helpUrl_;
    cmd << cmdStr << QChar('\0') << QChar('\n');
}

void HelpAssistantImpl::show(const String& ref, const String& helpFile) const
{
    DDBG << "HelpAssistant: showing ref=" << ref << std::endl;

    String helptag;
    QString baseUrl(NOTR("qthelp://com.syntext.docs.serna/doc/"));

    if (helpFile.empty()) {
        helptag = doctags::get_tag(ref.empty() ? String(DOCTAG(INDEX)) : ref);
        helpUrl_ = baseUrl + QString(helptag);
    }
    else {
        if (!registerHelpFile(helpFile))
            return;

        QFileInfo helpFi(helpFile);
        QString base = helpFi.baseName();
        helpUrl_ = baseUrl + base;
        helpUrl_ += QChar('/');

        int i = ref.find('#');
        if (i > 0)
            helpUrl_ += ref.left(i);
        else
            helpUrl_ += ref;
    }
    DDBG << "Showing help-tag (ref=" << helptag
         << "): " << helpUrl_ << std::endl;
    if (!assistantProc_) {
        startAssistant();
        return;
    }
    showHelpUrl();
}

void HelpAssistantImpl::showError(const QString& msg) const
{
    QMessageBox::critical(qApp->activeWindow(), tr("Serna Help error"), msg);
}

///////////////////////////////////////////////////////////////////////

void HelpAssistantImpl::showLongHelp(const Common::PropertyNode* ptn,
                                     const Common::String& desc) const
{
    const PropertyNode* short_help =
        ptn ? ptn->getProperty(HelpHandle::SHORT_HELP) : 0;
    const PropertyNode* qta =
        ptn ? ptn->getProperty(HelpHandle::QTA_HELP) : 0;
    const PropertyNode* href =
        qta ? qta->getProperty(HelpHandle::QTA_HREF) : 0;
    String desc_str;
    if (!desc.isEmpty())
        desc_str = " " + String(tr("for", "No full help is available for")) +
            NOTR(" <b>") + desc + NOTR("</b>");
    if (!ptn || !short_help) {
        QMessageBox::critical(qApp->activeWindow(),
            tr("Serna Help"),
            tr("<qt><nobr>Sorry, no help available%0</nobr></qt>").
                arg(desc_str));
        return;
    }
    if (href) {
        show(href->getString(),
            qta->getSafeProperty(HelpHandle::QTA_ADP_FILE)->getString());
        return;
    }
    QMessageBox::information(qApp->activeWindow(), tr("Short Help"),
        NOTR("<qt>") + QString(short_help->getString()) +
            NOTR("<br/><hr/><i>") +
            tr("Note: No full help is available%0").arg(desc_str) +
            NOTR("</i></qt>"));
}

bool HelpAssistantImpl::registerHelpFile(const QString& helpFile) const
{
    if (assistantExe_.isEmpty()) {
        showError(tr("Help viewer cannot be found"));
        return false;
    }

    QPointer<QProcess> proc = new QProcess(qApp);

    QStringList args(NOTR("-collectionFile"));
    args.push_back(helpCollectionFile_);
    args.push_back(NOTR("-register"));
    args.push_back(helpFile);

    proc->start(assistantExe_, args);
    if (!proc->waitForFinished()) {
        QString msg(tr("Error registering help file '%s'"));
        showError(msg.arg(helpFile));
        return false;
    }
    return true;
}

void HelpAssistantImpl::startAssistant() const
{
    if (assistantExe_.isEmpty()) {
        showError(tr("Help viewer cannot be found"));
        return;
    }
    assistantProc_ = new QProcess(qApp);

    QStringList args(NOTR("-collectionFile"));
    args.push_back(helpCollectionFile_);
    args.push_back(NOTR("-enableRemoteControl"));

    assistantProc_->start(assistantExe_, args);

    connect(&*assistantProc_, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(assistantError(QProcess::ProcessError)));
    connect(&*assistantProc_, SIGNAL(started()), this, SLOT(assistantStarted()));

    connect(&*assistantProc_,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(assistantFinished(int, QProcess::ExitStatus)));
}

void HelpAssistantImpl::assistantFinished(int, QProcess::ExitStatus)
{
    assistantProc_ = 0;
}

void HelpAssistantImpl::assistantStarted()
{
    showHelpUrl();
}

void HelpAssistantImpl::assistantError(QProcess::ProcessError error)
{
    showError(QString());
    assistantProc_ = 0;
}

///////////////////////////////////////////////////////////////////////

HelpAssistant& helpAssistant()
{
    return SingletonHolder<HelpAssistantImpl>::instance();
}

/////////////////

