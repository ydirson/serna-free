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
#ifndef INSERT_SYMBOL_DIALOG_H
#define INSERT_SYMBOL_DIALOG_H

#include "docview/InsertSymbolItem.h"
#include "docview/InsertSymbolDialogBase.hpp"

#include "common/PropertyTree.h"
#include "common/String.h"

#include <vector>

class QComboBox;
class QTimer;
class QFontMetrics;

class InsertSymbolDialog : public QWidget,
                           public Ui::InsertSymbolDialogBase {
    Q_OBJECT

public slots:
    virtual void    grabFocus();
    virtual void    on_insertButton__clicked() { insertSymbol(); }
    virtual void    on_fontBox__activated(const QString& name);
    virtual void    on_symbolGrid__pageChanged();
    virtual void    on_favoritesGrid__pageChanged();
    virtual void    on_rangeEdit__returnPressed();
    virtual void    on_closeButton__clicked();
    virtual void    on_nextButton__clicked() {}
    virtual void    on_rangeBox__activated(const QString& name) 
        { scriptChanged(name); }

    virtual void    insertSymbol();
    virtual void    postponedFontCalc();
    virtual void    addToFavorites();
    virtual void    removeFromFavorites();
    virtual void    symbolActivated();
    virtual void    help() const;

public:
    class CharacterRange {
    public:
        CharacterRange(const QString& script, const QString& diapason)
            : script_(script), diapason_(diapason){}
        const QString& script() { return script_; }

        ushort start();
        ushort end();
    private:
        QString script_;
        QString diapason_;
    };
    typedef std::vector<ushort> UnicodeTable;
    typedef std::vector<CharacterRange> RangeTable;

    InsertSymbolDialog(QWidget* parent, InsertSymbolItem* isymbol,
                       Common::PropertyNode* ptn);
private:
    void            fillOneRange(UnicodeTable& table);
    void            scriptChanged(const QString&);

    InsertSymbolItem*       isymbol_;
    Common::PropertyNode*   property_;
    RangeTable           ranges_;
    RangeTable::iterator currentRange_;
    UnicodeTable         symbols_;
    UnicodeTable         cachedSymbols_;
    UnicodeTable         favorites_;
    QTimer*              timer_;
    QFontMetrics*        fontMetrix_;
    int                  lastSymbol_;
};

#endif  // INSERT_SYMBOL_DIALOG_H
