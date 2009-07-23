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
#ifndef QT_EXT_GRIDVIEW_H
#define QT_EXT_GRIDVIEW_H

#include "ui/ui_defs.h"
#include <QScrollArea>

///! This is a (mostly rewritten) native port of QQtGridView
class UI_EXPIMP QtGridView : public QScrollArea {
    Q_OBJECT
public:
    QtGridView(QWidget *parent = 0);
    ~QtGridView() {}

    int             numRows() const { return nrows; }
    virtual void    setNumRows(int);
    int             numCols() const { return ncols; }
    virtual void    setNumCols(int);

    int             cellWidth() const { return cellw; }
    virtual void    setCellWidth(int);
    int             cellHeight() const { return cellh; }
    virtual void    setCellHeight(int);

    QRect   cellRect() const { return QRect(0, 0, cellw, cellh); }
    QSize   gridSize() const { return QSize(ncols * cellw, nrows * cellh); }

    QRect   cellGeometry(int row, int column);

    int     rowAt(int y) const    { return y / cellh; }
    int     columnAt(int x) const { return x / cellw; }

    void repaintCell(int row, int column, bool erase = true);
    void updateCell(int row, int column);
    void ensureCellVisible(int row, int column);

protected:
    virtual void paintCell(QPainter*, int row, int col) = 0;

private:
    QtGridView(const QtGridView&);
    QtGridView& operator=(const QtGridView&);
    friend class GridWidget;

    void        updateGrid();
    
    int         nrows;
    int         ncols;
    int         cellw;
    int         cellh;
    QWidget*    widget_;
};

#endif // Q3GRIDVIEW_H
