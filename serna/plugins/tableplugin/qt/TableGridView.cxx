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
#include "TableGridView.h"
#include "plugins/tableplugin/TableUtils.h"

#include "common/OwnerPtr.h"
#include "common/PropertyTreeEventData.h"

#include <QApplication>
#include <QPainter>
#include <QShowEvent>
#include <QMouseEvent>

#include <iostream>

using namespace Common;

TableGridView::TableGridView(QWidget* parent, PropertyNode* data)
    : QtGridView(parent), selectedCol_(-1),
      selectedRow_(-1), pressed_(true), data_(data)
{
        setNumRows(3);
        setNumCols(4);
        setCellWidth(30);
        setCellHeight(30);
    setMouseTracking(true);
    installEventFilter(this);
    setFocus();
}

void TableGridView::showEvent(QShowEvent *e)
{
    QtGridView::showEvent( e );
    setNumRows(3);
        setNumCols(4);
    resize(numCols() * cellWidth() + 5, numRows() * cellHeight()+ 5);
    if (parentWidget()) {
        parentWidget()->setMaximumSize(32767, 32767);
        parentWidget()->resize(numCols() * cellWidth() + 10, numRows() * cellHeight() + 10);
    }
    selectedCol_ = -1;
    selectedRow_ = -1;
}

void TableGridView::buttonReleased()
{
    pressed_ = false;
}

void TableGridView::contentsMousePressEvent(QMouseEvent*)
{
    pressed_ = true;
}

void TableGridView::contentsMouseReleaseEvent(QMouseEvent*)
{
    if (isVisible() && parentWidget() &&
        parentWidget()->inherits(NOTR("QPopupMenu")) ) {
        data_->makeDescendant(Sui::TABLE_SIZE)->setString(
                              String::number(selectedCol_+1) + "x" +
                              String::number(selectedRow_+1));
        parentWidget()->close();
        emit activated();
    }
}

void TableGridView::contentsMouseMoveEvent(QMouseEvent* e)
{
    QPoint pos = e->pos() - QPoint(geometry().topLeft() - frameGeometry().topLeft());
    if (pressed_) {
        QPoint end(geometry().bottomRight() - frameGeometry().bottomRight());
        bool to_update = false;
        if (e->pos().x() > width() - cellWidth() / 2) {
            setNumCols( numCols() + 1);
            to_update = true;
        }
        if (e->pos().y() > height() - cellHeight() / 2) {
            setNumRows(numRows() + 1);
            to_update = true;
        }
        if (to_update) {
            setFocus();
            resize(numCols() * cellWidth() + 5, numRows() * cellHeight()+ 5);
            if (parentWidget()) {
                parentWidget()->setMaximumSize(32767, 32767);
                parentWidget()->resize(numCols() * cellWidth() + 10, numRows() * cellHeight() + 10);
            }
            repaint();
        }
    }

    int col  = -1;
    int row  = -1;
    if (0 < pos.x())
        col = pos.x() / cellWidth();
    if (0 < pos.y())
        row = pos.y() / cellHeight();

    if (0 <= col && 0 <= row && (col != selectedCol_ || row != selectedRow_)) {
        int old_row = selectedRow_;
        int old_col = selectedCol_;
        selectedRow_ = row;
        selectedCol_ = col;

        for (int j = 0; j <= QMAX(old_row, row); j++)
            for (int i = QMIN(old_col, col); i <= QMAX(old_col, col); i++)
                updateCell(j,i);

        for (int j = 0; j <= QMAX(old_col, col); j++)
            for (int i = QMIN(old_row, row); i <= QMAX(old_row, row); i++)
                updateCell(i, j);
    }
    QtGridView::mouseMoveEvent(e);
}

void TableGridView::paintCell( QPainter *p, int row, int col )
{
    p->setPen(QPen(Qt::gray));
    QRect bounded = QRect(2, 2, cellWidth()-3, cellHeight()-3);
    if (0 == col)
        bounded.rLeft() += 1;
    if (0 == row)
        bounded.rTop() += 1;
    if ((numCols()-1) == col)
        bounded.rRight() -= 1;
    if ((numRows()-1) == row)
        bounded.rBottom() -= 1;

    p->drawRect(bounded);
    if ( col <= selectedCol_ && row <= selectedRow_) {
        QRect rect = QRect(bounded.x(), bounded.y(),
                     bounded.width()-1, bounded.height()-1);
        p->fillRect(rect, QBrush(QColor(0,0,127)));
    }
}
