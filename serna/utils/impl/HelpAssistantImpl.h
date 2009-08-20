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

/** \file
 */

#ifndef HELP_ASSISTANT_IMPL_H_
#define HELP_ASSISTANT_IMPL_H_

#include "utils/HelpAssistant.h"
#include "common/StringDecl.h"

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <QMap>

class QString;

class Assistant : public QObject {
    Q_OBJECT
public:
    Assistant(const QString& exe, const QString& helpCollectionFile);
    ~Assistant();
    void show(const QString& ref, const QString& baseUrl);

private slots:
    void assistantFinished(int, QProcess::ExitStatus);
    void assistantStarted();
    void assistantError(QProcess::ProcessError error);

private:
    void startAssistant();
    void showHelpUrl();

    QString             helpUrl_;
    QString             assistantExe_;
    QString             helpCollectionFile_;
    QPointer<QProcess>  assistantProc_;
};

typedef QMap<QString, Assistant*> AssistantMap;

class HelpAssistantImpl : public QObject, public HelpAssistant {
    Q_OBJECT
public:
    HelpAssistantImpl();
    ~HelpAssistantImpl();

    virtual void        show(const Common::String& ref,
                             const Common::String& baseUrl,
                             const Common::String& adp) const;
    virtual void        showLongHelp(const Common::PropertyNode*,
                                     const Common::String&) const;

protected slots:
    void                showError(const QString& msg) const;

private:
    void                showHelpUrl() const;
    void                startAssistant() const;
    bool                registerHelpFile(const QString& helpFile) const;

    QString             helpCollectionFile_;
    QString             assistantExe_;

    mutable AssistantMap        assistants_;
};

#endif // HELP_ASSISTANT_IMPL_H_
