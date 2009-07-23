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
#include "docview/impl/debug_dv.h"
#include "docview/qt/InsertSymbolWidget.h"
#include "docview/qt/SymbolGrid.h"
#include "docview/qt/unicode_ranges.h"

#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include "ui/UiItem.h"
#include "ui/IconProvider.h"

#include <QApplication>
#include <QMessageBox>
#include <QFontDatabase>
#include <QFileInfo>
#include <QMenu>
#include <QToolTip>
#include <QTimer>
#include <QStringList>
#include <QShortcut>
#include <QAbstractItemView>

#include <iostream>

using namespace Common;
using namespace InsertSymbolSpace;

static const char APP_INSERT_SYMBOL_FONT[] = 
    NOTR("app/insert-symbol-dialog/font");
static const char APP_INSERT_SYMBOL_FAVOURITES[] =
    NOTR("app/insert-symbol-dialog/favourites");

///////////////////////////////////////////////////////////////////////////

QWidget* InsertSymbolItem::makeWidget(QWidget* parent, Type)
{
    return new InsertSymbolDialog(parent, this, itemProps());
}

///////////////////////////////////////////////////////////////////////////

void InsertSymbolDialog::grabFocus()
{
    setActiveWindow();
}

InsertSymbolDialog::InsertSymbolDialog(QWidget* parent,
                                       InsertSymbolItem* isymbol,
                                       PropertyNode* property)
    : QWidget(parent),
      isymbol_(isymbol),
      property_(property),
      lastSymbol_(-1)
{
    setupUi(this);

    prevButton_->setIcon(Sui::icon_provider().
        getIconSet(NOTR("symbol_arrow_left")));
    nextButton_->setIcon(Sui::icon_provider().
        getIconSet(NOTR("symbol_arrow_right")));
    prevFavoritesButton_->setIcon(Sui::icon_provider().
        getIconSet(NOTR("symbol_arrow_left")));
    nextFavoritesButton_->setIcon(Sui::icon_provider().
        getIconSet(NOTR("symbol_arrow_right")));


    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), SLOT(postponedFontCalc()));
    connect(nextButton_, SIGNAL(clicked()), symbolGrid_, SLOT(nextPage()));
    connect(prevButton_, SIGNAL(clicked()), symbolGrid_, SLOT(prevPage()));
    connect(nextFavoritesButton_, SIGNAL(clicked()), favoritesGrid_,
        SLOT(nextPage()));
    connect(prevFavoritesButton_, SIGNAL(clicked()), favoritesGrid_,
        SLOT(prevPage()));

    QLineEdit* combo_lineedit = new QLineEdit(this);
    combo_lineedit->setReadOnly(true);
    rangeBox_->setLineEdit(combo_lineedit);
    rangeBox_->setInsertionPolicy(QComboBox::NoInsert);
    rangeBox_->view()->setTextElideMode(Qt::ElideNone);

    QFontDatabase fdb;
    QStringList families = fdb.families();
    int i = 0;
    for(; i < 102; ++i) {
        QString text(unicode_ranges[i]);
        if (text[0] == '#')
            continue;
        int pos = text.find("; ");
        if (pos < 9)
            continue;
        QString diapason = text.left(pos);
        QString script = text.mid(pos + 2);
        ranges_.push_back(CharacterRange(script, diapason));
    }
    String symbols = config().getProperty(
        APP_INSERT_SYMBOL_FAVOURITES)->getString();
    if (symbols.isEmpty()) {
        for(i = 0; i < 23; ++i) {
            bool ok;
            ushort sym = QString(favorites_default[i]).toUShort(&ok, 16);
            if (ok)
                favorites_.push_back(sym);
        }
    }
    else {
        QStringList f = QStringList::split(" ", symbols);
        for(i = 0; i < (int)f.size(); ++i) {
            bool ok;
            ushort sym = f[i].toUShort(&ok, 16);
            if (ok)
                favorites_.push_back(sym);
        }
    }
    currentRange_ = ranges_.begin();
    symbolGrid_->setUnicodeTable(&symbols_);
    favoritesGrid_->setUnicodeTable(&favorites_);
    favoritesGrid_->setAutoSizes(true);
    fontBox_->insertStringList(families);

    connect(symbolGrid_, SIGNAL(activated()), SLOT(symbolActivated()));
    connect(symbolGrid_, SIGNAL(doubleClicked()), SLOT(insertSymbol()));
    connect(symbolGrid_, SIGNAL(activated()),
        favoritesGrid_, SLOT(inactivate()));
    
    connect(favoritesGrid_, SIGNAL(activated()), SLOT(symbolActivated()));
    connect(favoritesGrid_, SIGNAL(doubleClicked()), SLOT(insertSymbol()));
    connect(favoritesGrid_, SIGNAL(activated()), 
        symbolGrid_, SLOT(inactivate()));
    
    QMenu* contextMenu = new QMenu(this);
    QAction* action = new QAction(tr("&Add to Favorites"), this); 
    connect(action, SIGNAL(triggered(bool)), this, SLOT(addToFavorites()));
    QKeySequence add_kseq(
#ifdef __APPLE__
        Qt::Key_I
#else
        Qt::Key_Insert
#endif
        );
    connect(new QShortcut(add_kseq, this), SIGNAL(activated()),
        this, SLOT(addToFavorites()));
    action->setShortcut(add_kseq);
    contextMenu->addAction(action); 
    symbolGrid_->setContextMenu(contextMenu);

    QKeySequence del_kseq(Qt::Key_Delete);
    QMenu* favoritesMenu = new QMenu(this);
    action = new QAction(tr("&Remove from Favorites"), this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(removeFromFavorites()));
    connect(new QShortcut(del_kseq, this), SIGNAL(activated()),
        this, SLOT(removeFromFavorites()));
    action->setShortcut(del_kseq);
    favoritesMenu->addAction(action);
    favoritesGrid_->setContextMenu(favoritesMenu);
    
    String font_name = config().
        getProperty(APP_INSERT_SYMBOL_FONT)->getString();
    if (font_name.isEmpty())
        font_name = NOTR("Serif");
    fontBox_->setCurrentItem(0);
    for (i = 0; i < fontBox_->count(); ++i) {
        if (font_name == fontBox_->text(i)) {
            fontBox_->setCurrentItem(i);
            break;
        }
    } 
    on_fontBox__activated(fontBox_->currentText());
    favoritesGrid_->gridUpdate();
    on_symbolGrid__pageChanged();
}

void InsertSymbolDialog::on_closeButton__clicked()
{
    String favorites;
    for (uint i = 0; i < favorites_.size(); ++i)
       favorites += QString().sprintf( NOTR("%.4X "), favorites_[i]);
    config().root()->makeDescendant(APP_INSERT_SYMBOL_FAVOURITES, 
        favorites, true);
    isymbol_->setBool(Sui::IS_VISIBLE, false);
}

void InsertSymbolDialog::symbolActivated()
{
    ushort symbol = 0;
    int idx = symbolGrid_->activeSymbolIndex();
    if (0 <= idx)
        symbol = symbolGrid_->symbol(idx);
    else {
        idx = favoritesGrid_->activeSymbolIndex();
        if (0 <= idx)
            symbol = favorites_[idx];
    }
    insertButton_->setEnabled((0 <= idx));
    if (0 > idx)
        return;
    rangeEdit_->setText(QString().sprintf( NOTR("%.4X"), symbol));
    bool ok;
    for (int i = 1; i < rangeBox_->count(); ++i) {
        ushort rsymbol = rangeBox_->text(i).left(4).toUShort(&ok, 16);
        if (ok && symbol < rsymbol) {
            rangeBox_->setCurrentItem(i-1);
            rangeBox_->lineEdit()->setText(rangeBox_->currentText().mid(5));
            return;
        }
    }
}

void InsertSymbolDialog::insertSymbol()
{
    ushort symbol = 0;
    int idx = symbolGrid_->activeSymbolIndex();
    if (0 <= idx)
        symbol = symbolGrid_->symbol(idx);
    else {
        idx = favoritesGrid_->activeSymbolIndex();
        if (0 <= idx)
            symbol = favorites_[idx];
    }
    if (0 > idx)
        return;
    if (!((9 == symbol || 10 == symbol || 13 == symbol) ||
          (32 <= symbol && 55295 >= symbol) ||
          (57344 <= symbol && 65533 >= symbol))) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid character\n"
                                "According to XML W3C Recommendation,\n"
                                "\"x0009 | #x000A | #x000D | [#x0020-#xD7FF] "
                                "| [#xE000-#xFFFD]\"\n"
                                "characters are allowed."));
        return;
    }
    property_->makeDescendant(SYMBOL)->setInt(symbol);
    isymbol_->insert();
}

void InsertSymbolDialog::postponedFontCalc()
{
    if (currentRange_ == ranges_.end())  {
        if (0 <= lastSymbol_) {
            symbolGrid_->setUnicodeTable(&symbols_);
            rangeBox_->removeItem(0);
            bool ok;
            for (int i = 1; i < rangeBox_->count(); ++i) {
                ushort rsymbol = rangeBox_->text(i).left(4).toUShort(&ok, 16);
                if (ok && lastSymbol_ < rsymbol) {
                    rangeBox_->setCurrentItem(i-1);
                    scriptChanged(rangeBox_->currentText());
                    lastSymbol_ = -1;
                    return;
                }
            }
            QString s;
            rangeEdit_->setText(s.sprintf( NOTR("%.4X"), lastSymbol_));
            lastSymbol_ = -1;
            on_rangeEdit__returnPressed();
        }
        return;
    }
    fillOneRange(symbols_);
    timer_->start(0, true);
}


void InsertSymbolDialog::fillOneRange(UnicodeTable& table)
{
    uint old_size = table.size();
    for (ushort i = currentRange_->start(); i != currentRange_->end(); ++i) {
        if (0 <= lastSymbol_ && i == lastSymbol_) {
            table.push_back(i);
            continue;
        }
        if (fontMetrix_->inFont(QChar(i)))
            table.push_back(i);
    }
    if (table.size() > old_size) {
        QString prep;
        prep = prep.sprintf( NOTR("%.4X "), table[old_size]);
        prep += currentRange_->script();
        rangeBox_->insertItem(prep);
    }
    ++currentRange_;
}


void InsertSymbolDialog::on_fontBox__activated(const QString& name)
{
    config().root()->makeDescendant(APP_INSERT_SYMBOL_FONT, name, true);
    timer_->stop();
    int idx = symbolGrid_->activeSymbolIndex();
    if (0 > idx)
        idx = symbolGrid_->inActiveSymbolIndex();
    currentRange_ = ranges_.begin();
    if (0 <= idx) {
        lastSymbol_ = symbolGrid_->symbol(idx);
        while (currentRange_ != ranges_.end()) {
            if (lastSymbol_ < currentRange_->end())
                break;
            ++currentRange_;
        }
        cachedSymbols_.clear();
        symbolGrid_->setUnicodeTable(&cachedSymbols_);
    }

    QFontDatabase fdb;
    QFont fnt = fdb.font(name, NOTR("Normal"), 16);
    fontMetrix_ = new QFontMetrics(fnt);
    uint count = symbolGrid_->numCols() * symbolGrid_->numRows();
    rangeBox_->clear();
    symbols_.clear();
    if (0 <= lastSymbol_) {
        while (cachedSymbols_.size() <= count &&
                           currentRange_ != ranges_.end())
            fillOneRange(cachedSymbols_);

    }
    else
        while (symbols_.size() <= count && currentRange_ != ranges_.end())
            fillOneRange(symbols_);
    symbolGrid_->chooseFont(fnt);
    favoritesGrid_->chooseFont(fnt);
    if (0 <= lastSymbol_) {
        symbolGrid_->activateSymbol(lastSymbol_);
        QString rng = rangeBox_->currentText();
        rangeBox_->clear();
        rangeBox_->insertItem(rng);
        rangeBox_->lineEdit()->setText(rng);
        currentRange_ = ranges_.begin();
    }
    else {
        rangeBox_->setCurrentItem(0);
        scriptChanged(rangeBox_->currentText());
    }
    postponedFontCalc();
}

void InsertSymbolDialog::on_rangeEdit__returnPressed()
{
    bool ok;
    ushort symbol = rangeEdit_->text().toUShort(&ok, 16);
    rangeBox_->lineEdit()->setText("");
    symbolGrid_->scriptChanged(rangeEdit_->text());
    if (ok)
        symbolGrid_->activateSymbol(symbol);
}

void InsertSymbolDialog::scriptChanged(const QString& name)
{
    rangeBox_->lineEdit()->setText(name.mid(5));
    symbolGrid_->scriptChanged(name);
}

void InsertSymbolDialog::addToFavorites()
{
    int idx = symbolGrid_->activeSymbolIndex();
    if (0 > idx)
        return;
    int idx_fav = favoritesGrid_->inActiveSymbolIndex();
    if (0 > idx_fav)
        idx_fav = 0;

    favorites_.insert(favorites_.begin() + idx_fav,
                       symbolGrid_->symbol(idx));
    favoritesGrid_->gridUpdate();
}

void InsertSymbolDialog::removeFromFavorites()
{
    int idx = favoritesGrid_->activeSymbolIndex();
    if (0 <= idx)
        favorites_.erase(favorites_.begin() + idx);
    favoritesGrid_->gridUpdate();
}

ushort InsertSymbolDialog::CharacterRange::start()
{
    bool ok;
    return diapason_.left(4).toUShort(&ok, 16);
}

ushort InsertSymbolDialog::CharacterRange::end()
{
    bool ok;
    return diapason_.right(4).toUShort(&ok, 16);
}


void InsertSymbolDialog::on_symbolGrid__pageChanged()
{
    prevButton_->setDisabled(symbolGrid_->isFirstPage());
    nextButton_->setDisabled(symbolGrid_->isLastPage());
}

void InsertSymbolDialog::on_favoritesGrid__pageChanged()
{
    prevFavoritesButton_->setDisabled(favoritesGrid_->isFirstPage());
    nextFavoritesButton_->setDisabled(favoritesGrid_->isLastPage());
    if (prevFavoritesButton_->isEnabled() ||
        nextFavoritesButton_->isEnabled()) {
        prevFavoritesButton_->show();
        nextFavoritesButton_->show();
    }
    else {
        prevFavoritesButton_->hide();
        nextFavoritesButton_->hide();
    }
}

void InsertSymbolDialog::help() const
{
    helpAssistant().show(DOCTAG(UG_INSERT_SYMBOL_D));
}
