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
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "docview/dv_defs.h"
#include <QMenu>
#include <QPointer>
#include "ui/QtGridView.h"

#include <vector>

class QMenu;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;
class QContextMenuEvent;

class SymbolGrid : public QtGridView {
    Q_OBJECT
public:
    typedef std::vector<ushort> UnicodeTable;

    SymbolGrid(QWidget* parent);

    //! set table to show
    void setUnicodeTable(UnicodeTable* utable);

    //! set context menu (right click)
    void setContextMenu(QMenu* contextMenu);

    //! returns -1 if there is no active symbol
    int  activeSymbolIndex();
    // inactive - grayed symbol when focus is out
    int  inActiveSymbolIndex();

    //! activate symbol. Does not scroll to range, where symbol is.
    //  Used to keep synchronized symbol while changing font.
    void activateSymbol(ushort symbol);
    //! return symbol by index 'idx' from internal unicode table.
    ushort symbol(int idx);

    //! used to enable/disable scrolling
    bool isLastPage();
    bool isFirstPage();

    //! return left top corner symbol
    QString firstSymbol();

    //! update grid
    void gridUpdate();

    //! auto sizes - calculate and set minimumHeight()
    //  needed by favorites table.
    //  auto sizes is off by default.
    void setAutoSizes(bool yes);
signals:
    void activated();
    void doubleClicked();
    void pageChanged();

public slots:
    void chooseFont(const QFont& font);
    void scriptChanged(const QString& name);
    void inactivate();
    void nextPage();
    void prevPage();

protected:
    void paintCell(QPainter* painter, int row, int col);
    void resizeEvent(QResizeEvent*);
    void contextMenuEvent(QContextMenuEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);

private:
    UnicodeTable* utable_;
    ushort        curIdx_;
    QFont         font_;
    QPoint        activeCell_;
    QPoint        inActiveCell_;
    QPointer<QMenu> contextMenu_;
    bool            autoSizes_;
};


#endif //SYMBOL_TABLE_H
