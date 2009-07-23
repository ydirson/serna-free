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
 /****************************************************************************
** $Id: qt/qurl.h   3.3.6   edited Aug 31 2005 $
**
** Definition of QOldUrl class
**
** Created : 950429
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QURL_H
#define QURL_H

#ifndef QT_H
#include "qstring.h"
#endif // QT_H

#ifndef QT_NO_URL

class QOldUrlPrivate;

class QOldUrl
{
public:
    QOldUrl();
    QOldUrl( const QString& url );
    QOldUrl( const QOldUrl& url );
    QOldUrl( const QOldUrl& url, const QString& relUrl, bool checkSlash = FALSE );
    virtual ~QOldUrl();

    QString protocol() const;
    virtual void setProtocol( const QString& protocol );

    QString user() const;
    virtual void setUser( const QString& user );
    bool hasUser() const;

    QString password() const;
    virtual void setPassword( const QString& pass );
    bool hasPassword() const;

    QString host() const;
    virtual void setHost( const QString& user );
    bool hasHost() const;

    int port() const;
    virtual void setPort( int port );
    bool hasPort() const;

    QString path( bool correct = TRUE ) const;
    virtual void setPath( const QString& path );
    bool hasPath() const;

    virtual void setEncodedPathAndQuery( const QString& enc );
    QString encodedPathAndQuery();

    virtual void setQuery( const QString& txt );
    QString query() const;

    QString ref() const;
    virtual void setRef( const QString& txt );
    bool hasRef() const;

    bool isValid() const;
    bool isLocalFile() const;

    virtual void addPath( const QString& path );
    virtual void setFileName( const QString& txt );

    QString fileName() const;
    QString dirPath() const;

    QOldUrl& operator=( const QOldUrl& url );
    QOldUrl& operator=( const QString& url );

    bool operator==( const QOldUrl& url ) const;
    bool operator==( const QString& url ) const;

    static void decode( QString& url );
    static void encode( QString& url );

    operator QString() const;
    virtual QString toString( bool encodedPath = FALSE, bool forcePrependProtocol = TRUE ) const;

    virtual bool cdUp();

    static bool isRelativeUrl( const QString &url );

protected:
    virtual void reset();
    virtual bool parse( const QString& url );

private:
    QOldUrlPrivate *d;

};

#endif //QT_NO_URL

#endif
