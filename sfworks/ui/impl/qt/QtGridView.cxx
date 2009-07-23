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
#include "ui/QtGridView.h"
#include <QPainter>
#include <QPaintEvent>

using namespace Qt;

class GridWidget : public QWidget {
public:
    GridWidget(QtGridView* parent)
        : QWidget(parent),
          gridView_(parent) {}
    virtual void paintEvent(QPaintEvent*);
private:
    QtGridView* gridView_;
};

QtGridView::QtGridView(QWidget *parent)
    : QScrollArea(parent),
      nrows(5), ncols(5), cellw(12), cellh(12)
{
    widget_ = new GridWidget(this);
    widget_->setBackgroundMode(PaletteBase);
    setBackgroundMode(PaletteBackground, PaletteBase);
    widget_->setFocusProxy(this);
    setWidget(widget_);
    setWidgetResizable(false);
}

void QtGridView::updateGrid()
{
    widget_->resize(QSize(ncols * cellw, nrows * cellh));
}

void QtGridView::setNumRows(int numRows)
{
    nrows = numRows;
    updateGrid();
}

void QtGridView::setNumCols(int numCols)
{
    ncols = numCols;
    updateGrid();
}

void QtGridView::setCellWidth(int cellWidth)
{
    cellw = cellWidth;
    updateGrid();
}

void QtGridView::setCellHeight(int cellHeight)
{
    cellh = cellHeight;
    updateGrid();
}

QRect QtGridView::cellGeometry(int row, int column)
{
    QRect r;
    if (row >= 0 && row < nrows && column >= 0 && column < ncols)
	r.setRect(cellw * column, cellh * row, cellw, cellh);
    return r;
}

void QtGridView::updateCell(int row, int column)
{
    widget_->update(cellGeometry(row, column));
}

void QtGridView::ensureCellVisible(int row, int column)
{
    QRect r = cellGeometry(row, column);
    ensureVisible(r.x(), r.y(), r.width(), r.height());
}

void GridWidget::paintEvent(QPaintEvent* pev)
{
    QPainter p(this);
    const QRect& r = pev->rect();
    int colfirst = gridView_->columnAt(r.x());
    int collast  = gridView_->columnAt(r.x() + r.width());
    int rowfirst = gridView_->rowAt(r.y());
    int rowlast  = gridView_->rowAt(r.y() + r.height());

    if (rowfirst == -1 || colfirst == -1) 
	return;
    if (collast < 0 || collast >= gridView_->numCols())
	collast = gridView_->numCols() - 1;
    if (rowlast < 0 || rowlast >= gridView_->numRows())
	rowlast = gridView_->numRows() - 1;

    for (int row = rowfirst; row <= rowlast; ++row) {
	int rowp = row * gridView_->cellHeight();
	for (int col = colfirst; col <= collast; ++col) {
	    int colp = col * gridView_->cellWidth();
	    p.translate(colp, rowp);
	    gridView_->paintCell(&p, row, col);
	    p.translate(-colp, -rowp);
	}
    }
}
