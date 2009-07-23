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
#include "docview/InsertSymbolItem.h"
#include "docview/impl/debug_dv.h"
#include "common/PropertyTreeEventData.h"
#include "ui/UiItemSearch.h"
#include <qwidget.h>

using namespace Common;

namespace InsertSymbolSpace {

typedef DOCVIEW_EXPIMP const char* const exported_literal;

// START_IGNORE_LITERALS
exported_literal FAVOURITE_SYMBOLS = "favourite_symbols";
exported_literal FAVOURITES        = "favourites";
exported_literal FONT_NAME         = "font";
exported_literal SYMBOL            = "symbol";
// STOP_IGNORE_LITERALS

}

InsertSymbolItem::InsertSymbolItem(PropertyNode* props)
    : LiquidItem(props)
{
    itemProps()->makeDescendant("caption")->setString(tr("Insert Symbol"));
}

InsertSymbolItem::~InsertSymbolItem()
{
}

bool InsertSymbolItem::insert()
{
    if (!doInsert()) {
        return false;
    }
    return true;
}


InsertSymbolItem* InsertSymbolItem::findInstance(const Sui::Item* from)
{
    return dynamic_cast<InsertSymbolItem*>(
        from->findItem(Sui::ItemClass(Sui::INSERT_SYMBOL)));
}

