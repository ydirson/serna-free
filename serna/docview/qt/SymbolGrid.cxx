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
#include "common/String.h"
#include "docview/qt/SymbolGrid.h"

#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <QFontDatabase>
#include <QCursor>
#include <QContextMenuEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QKeyEvent>

using namespace Common;

SymbolGrid::SymbolGrid( QWidget* parent)
    : QtGridView(parent), utable_(0), curIdx_(0),
      activeCell_(QPoint(-1,-1)), inActiveCell_(QPoint(-1,-1)),
      contextMenu_(0), autoSizes_(false)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCellWidth(30);
    setCellHeight(30);
    setNumCols(20);
    setNumRows(4);
}

void SymbolGrid::setUnicodeTable(UnicodeTable* utable)
{
    utable_ = utable;
}

void SymbolGrid::setAutoSizes(bool yes)
{
    autoSizes_ = yes;
}

QString SymbolGrid::firstSymbol()
{
    QString s;
    if (utable_ && !utable_->empty())
        s = s.sprintf( NOTR("%#.4X"), utable_->operator[](curIdx_));
    return s.mid(2);
}

void SymbolGrid::paintCell(QPainter* painter, int row, int col)
{
    if (0 == utable_ || utable_->empty())
        return;
    painter->save();
    painter->setPen(Qt::gray);
    QRect bounded = QRect(-1, -1, cellWidth()+1, cellHeight()+1);
    if (0 == col)
        bounded.rLeft() += 1;
    if (0 == row)
        bounded.rTop() += 1;
    if ((numCols()-1) == col)
        bounded.rRight() -= 1;
    if ((numRows()-1) == row)
        bounded.rBottom() -= 1;

    painter->drawRect(bounded);
    painter->setPen(Qt::black);
    uint pos = curIdx_ + numCols()*row + col;
    if (pos >= utable_->size())
        return painter->restore();

    QChar ch(utable_->operator[](pos));
    QString s(ch);
    if (activeCell_.x() == col && activeCell_.y() == row) {
        painter->fillRect(cellRect(), QBrush(QColor(0,0,127)));
        painter->setPen(QPen(QColor(127,127,0), 2, Qt::DotLine));
        painter->drawRect(cellRect());
        painter->setPen(QPen(Qt::white));
    }
    else if ((0 > activeCell_.x() || 0 > activeCell_.y()) &&
             inActiveCell_.x() == col && inActiveCell_.y() == row) {
        painter->fillRect(cellRect(), QBrush(QColor(200,200,200)));
    }
    QFontMetrics fm(font_);
    uint hoffcet = (cellWidth() - fm.width(ch)) / 2;
    uint voffcet = cellHeight() * 5/6;
    painter->drawText(hoffcet, voffcet,s);
    painter->restore();
}

void SymbolGrid::gridUpdate()
{
    int col = activeCell_.x();
    int row = activeCell_.y();
    if (0 <= row && 0 <= col &&
       (curIdx_ + numCols()*row + col) >= (int)utable_->size()) {
        if (0 == col) {
            col = numCols();
            row--;
        }
        activeCell_ = QPoint(col - 1, row);
    }
    int numcols = (width() / cellWidth());
    int numrows = (height() / cellHeight());

    if (numCols() != numcols)
        setNumCols(numcols);
    if (numRows() != numrows)
        setNumRows(numrows);

    if (autoSizes_ && utable_->size()) {
        int minh = cellHeight()*((int)((utable_->size()-1)/numCols()) + 1) + 3;
        setMinimumHeight(QMIN(minh, maximumHeight()));
    }

    for (int j = 0; j<=numrows; j++)
        for (int i = 0; i<=numcols; i++)
            updateCell(j,i);
    emit pageChanged();
}

void SymbolGrid::resizeEvent(QResizeEvent*)
{
    int numcols = (width() / cellWidth());
    int numrows = (height() / cellHeight());
    if (numcols != numCols() || numrows != numRows())
        gridUpdate();
    if (width() != cellWidth() * numcols || height() != cellHeight() * numrows) {
        setGeometry(QRect(x(), y(),
            cellWidth() * numcols + 3, cellHeight() * numrows + 3));
    }
}

void SymbolGrid::setContextMenu(QMenu* contextMenu)
{
    contextMenu_ = contextMenu;
}

void SymbolGrid::contextMenuEvent(QContextMenuEvent* e)
{
    if (contextMenu_) {
        int col = activeCell_.x();
        int row = activeCell_.y();
        if (0 > row || 0 > col) {
            e->ignore();
            return;
        }
        if (QContextMenuEvent::Mouse == e->reason() &&
            (col != columnAt(e->pos().x()) ||
             row != rowAt(e->pos().y()))) {
                e->ignore();
                return;
        }
        e->accept();
        contextMenu_->exec(QCursor::pos());
    }
}

void SymbolGrid::mousePressEvent(QMouseEvent* e)
{
    QtGridView::mousePressEvent(e);
    if (int(utable_->size()) <= curIdx_ + numCols() *
                           rowAt(e->pos().y()) + columnAt(e->pos().x()))
        return;
    int col = activeCell_.x();
    int row = activeCell_.y();
    activeCell_ = QPoint(columnAt(e->pos().x()), rowAt(e->pos().y()));
    updateCell(row, col);
    emit activated();
    updateCell(activeCell_.y(), activeCell_.x());
}

void SymbolGrid::keyPressEvent(QKeyEvent* e)
{
    QtGridView::keyPressEvent(e);
    int x = 0;
    int y = 0;
    switch (e->key()) {
        case Qt::Key_Left: x -= 1; break;
        case Qt::Key_Up: y -= 1; break;
        case Qt::Key_Right: x += 1; break;
        case Qt::Key_Down:y += 1; break;
        case Qt::Key_Return: emit doubleClicked();return;
        case Qt::Key_Space: emit doubleClicked();return;
        default:
            return;
    }
    int col = activeCell_.x();
    int row = activeCell_.y();
    if (int(utable_->size()) <= curIdx_ + numCols()*(row + y) + col + x)
        return;
    activeCell_ = QPoint(-1,-1);
    updateCell(row, col);
    col += x;
    row += y;
    if (col < 0)
        col = 0;
    else if (col >= numCols())
        col = numCols() - 1;
    if (row < 0)
        row = 0;
    else if (row >= numRows())
        row = numRows() - 1;
    activeCell_ = QPoint(col, row);
    emit activated();
    updateCell(activeCell_.y(), activeCell_.x());
}

void SymbolGrid::chooseFont(const QFont& font)
{
    font_ = font;
    setFont(font_);
    curIdx_ = 0;
    gridUpdate();
}

void SymbolGrid::scriptChanged(const QString& name)
{
    bool ok;
    ushort symbol = name.left(4).toUShort(&ok, 16);

    ok = ((9 == symbol || 10 == symbol || 13 == symbol) ||
          (32 <= symbol && 55295 >= symbol) ||
          (57344 <= symbol && 65533 >= symbol)) && ok;
    if (!ok) {
        QMessageBox::warning(this, tr("Invalid Character"),
        tr("Invalid character\n"
           "According to the XML W3C Recommendation, only characters\n"
           "\"x0009 | #x000A | #x000D | [#x0020-#xD7FF] | [#xE000-#xFFFD]\" "
           "are allowed."), QMessageBox::Ok, 0);
        return;
    }

    uint i = 0;
    for (; i < utable_->size(); i++) {
        if (symbol == utable_->operator[](i)) {
            curIdx_ = i;
            break;
        }
    }
    if (i == utable_->size()) {
        utable_->push_back(symbol);
        curIdx_ = utable_->size() - 1;
    }
    resizeEvent(0);
    gridUpdate();
    emit pageChanged();
    emit activated();
    if (!hasFocus())
        setFocus();
}

int SymbolGrid::activeSymbolIndex()
{
    int col = activeCell_.x();
    int row = activeCell_.y();
    if (0 > row || 0 > col)
        return -1;
    int idx = curIdx_ + numCols()*row + col;
    if (idx >= (int)utable_->size())
        return -1;
    return idx;
}

int SymbolGrid::inActiveSymbolIndex()
{
    int col = inActiveCell_.x();
    int row = inActiveCell_.y();
    if (0 > row || 0 > col)
        return -1;
    int idx = curIdx_ + numCols()*row + col;
    if (idx >= (int)utable_->size())
        return -1;
    return idx;
}

void SymbolGrid::mouseDoubleClickEvent(QMouseEvent*)
{
    emit doubleClicked();
}

void SymbolGrid::nextPage()
{
    uint idx = curIdx_ + numCols()*numRows();
    if (idx > utable_->size())
        return;
    curIdx_ = idx;
    gridUpdate();
    emit activated();
    if (!hasFocus())
        setFocus();
}

void SymbolGrid::prevPage()
{
    int idx = curIdx_ - numCols()*numRows();
    if (idx < 0)
        curIdx_ = 0;
    else
        curIdx_ = idx;
    gridUpdate();
    emit activated();
    if (!hasFocus())
        setFocus();
}

bool SymbolGrid::isFirstPage()
{
    return (curIdx_ == 0);
}

bool SymbolGrid::isLastPage()
{
    return ((curIdx_ + numCols()*numRows()) >= (int)utable_->size());
}

void SymbolGrid::focusInEvent(QFocusEvent*)
{
    if (activeCell_.x() >= 0 && activeCell_.y() >= 0)
        inactivate();
    int col = inActiveCell_.x();
    int row = inActiveCell_.y();
    inActiveCell_ = QPoint(-1,-1);
    if (0 <= row && 0 <= col)
        updateCell(row, col);
    else if (0 > row)
        row = 0;
    if (0 > col)
        col = 0;
    activeCell_ = QPoint(col, row);
    updateCell(row, col);
    emit activated();
}

void SymbolGrid::inactivate()
{
    int col = activeCell_.x();
    int row = activeCell_.y();
    if (col < 0 || row < 0)
        return;
    inActiveCell_ = activeCell_;
    activeCell_ = QPoint(-1, -1);
    if (0 <= row && 0 <= col)
        updateCell(row, col);
    col = inActiveCell_.x();
    row = inActiveCell_.y();
    if (0 <= row && 0 <= col)
        updateCell(row, col);
    emit activated();
}

void SymbolGrid::focusOutEvent(QFocusEvent* fev)
{
    if (fev->reason() == Qt::ActiveWindowFocusReason)
        inactivate();
}

void SymbolGrid::activateSymbol(ushort symbol)
{
    int pos = -1;
    for (uint i = 0; i < utable_->size(); i++) {
        if (symbol == utable_->operator[](i)) {
            pos = i - curIdx_;
            break;
        }
    }
    if (pos < 0)
        pos = curIdx_;
    int row = pos / numCols();
    int col = pos - row * numCols();
    int acol = activeCell_.x();
    int arow = activeCell_.y();
    activeCell_ = QPoint(col, row);
    updateCell(arow, acol);
    acol = inActiveCell_.x();
    arow = inActiveCell_.y();
    inActiveCell_ = QPoint(col, row);
    updateCell(arow, acol);
    emit activated();
    updateCell(activeCell_.y(), activeCell_.x());
}

ushort SymbolGrid::symbol(int idx)
{
    return utable_->operator[](idx);
}
