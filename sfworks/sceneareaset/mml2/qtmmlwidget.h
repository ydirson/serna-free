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
**
** Copyright (C) 2003-2005 Trolltech AS. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding valid Qt Solutions licenses may use this file in
** accordance with the Qt Solutions License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/products/solutions/index.html
** or email sales@trolltech.com for information about Qt Solutions
** License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef QTMMLWIDGET_H
#define QTMMLWIDGET_H

#include <QFrame>

class MmlDocument;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTMMLWIDGET_EXPORT) && !defined(QT_QTMMLWIDGET_IMPORT)
#    define QT_QTMMLWIDGET_EXPORT
#  elif defined(QT_QTMMLWIDGET_IMPORT)
#    if defined(QT_QTMMLWIDGET_EXPORT)
#      undef QT_QTMMLWIDGET_EXPORT
#    endif
#    define QT_QTMMLWIDGET_EXPORT __declspec(dllimport)
#  elif defined(QT_QTMMLWIDGET_EXPORT)
#    undef QT_QTMMLWIDGET_EXPORT
#    define QT_QTMMLWIDGET_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTMMLWIDGET_EXPORT
#endif

class QT_QTMMLWIDGET_EXPORT MmlRenderer {
public:
    enum MmlFont { NormalFont, FrakturFont, SansSerifFont, ScriptFont,
                                MonospaceFont, DoublestruckFont };

    MmlRenderer();
    virtual ~MmlRenderer();

    QString fontName(MmlFont type) const;
    void setFontName(MmlFont type, const QString &name);
    int baseFontPointSize() const;
    void setBaseFontPointSize(int size);

    bool setContent(const QString &text, QString *errorMsg = 0,
                        int *errorLine = 0, int *errorColumn = 0);
    bool setContent(const GroveLib::Node* node, QString* errorMsg = 0);
    void dump() const;
    QSize mmlSizeHint() const;

    void setDrawFrames(bool b);
    bool drawFrames() const;

    void paintMml(QPainter* p, const QPoint& = QPoint());
    void setPaintDevice(QPaintDevice*);
    void layoutMml();

    void stretchTo(const QRect&);
    void clear();

    virtual void update_view() = 0;

protected:
    MmlDocument *m_doc;

private:
    MmlRenderer(const MmlRenderer&);
    MmlRenderer& operator=(const MmlRenderer&);
};

class QT_QTMMLWIDGET_EXPORT QtMmlWidget : public QFrame
{
public:
    enum MmlFont { NormalFont, FrakturFont, SansSerifFont, ScriptFont,
                                MonospaceFont, DoublestruckFont };

    QtMmlWidget(QWidget *parent = 0);
    ~QtMmlWidget();

    QString fontName(MmlFont type) const;
    void setFontName(MmlFont type, const QString &name);
    int baseFontPointSize() const;
    void setBaseFontPointSize(int size);

    bool setContent(const QString &text, QString *errorMsg = 0,
                        int *errorLine = 0, int *errorColumn = 0);
    void dump() const;
    virtual QSize sizeHint() const;

    void setDrawFrames(bool b);
    bool drawFrames() const;

    void clear();

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    MmlDocument *m_doc;
};

namespace GroveLib {
class Node;
}

class QT_QTMMLWIDGET_EXPORT QtMmlDocument
{
public:
    QtMmlDocument();
    virtual ~QtMmlDocument();
    void clear();

    bool setContent(QString text, QString *errorMsg = 0,
                    int *errorLine = 0, int *errorColumn = 0);
    bool setContent(const GroveLib::Node*, QString* errorMsg = 0);
    virtual void paint(QPainter *p, const QPoint &pos) const;
    QSize size() const;

    QString fontName(QtMmlWidget::MmlFont type) const;
    void setFontName(QtMmlWidget::MmlFont type, const QString &name);

    int baseFontPointSize() const;
    void setBaseFontPointSize(int size);
    void setDrawFrames(bool b);
    bool drawFrames() const;

    void dump() const;
private:
    MmlDocument *m_doc;
};

#endif
