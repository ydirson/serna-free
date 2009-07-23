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
#include "docview/dv_utils.h"
#include "docview/InsertSymbolItem.h"

#include "ui/UiItemSearch.h"

#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"

#include "plaineditor/PlainEditor.h"
#include "plaineditor/PlainDocument.h"

#include <qstring.h>

using namespace Common;
using namespace InsertSymbolSpace;

class PlainInsertSymbol : public InsertSymbolItem {
public:
    PlainInsertSymbol(PropertyNode* props)
        : InsertSymbolItem(props),
          plainEditor_(0) {}
private:
    virtual bool doInsert()
    {
        if (0 == plainEditor_)
            return false;
        QChar symbol(getInt(SYMBOL));
        plainEditor_->insert(symbol);
//        plainEditor_->insert(QString().sprintf("&#x%.4X;", symbol));
        return true;
    }
    //! Tells that this item is inserted/removed from parent
    virtual void inserted()
    {
        Sui::Item* doc = documentItem();
        if (0 == doc)
            return;
        plainEditor_ = dynamic_cast<PlainEditor*>(
            doc->findItem(Sui::ItemClass(Sui::PLAIN_EDITOR)));
    };
    virtual void removed() { plainEditor_ = 0; }

private:
    PlainEditor* plainEditor_;
};

namespace Sui {
    COMMANDLESS_ITEM_MAKER(PlainInsertSymbol, PlainInsertSymbol)
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CallPlainInsertSymbol, PlainEditor)

bool CallPlainInsertSymbol::doExecute(PlainEditor* plainEditor, EventData*)
{
    InsertSymbolItem* isymbol = 
        InsertSymbolItem::findInstance(plainEditor->plainDoc());
    if (isymbol) {
        isymbol->grabFocus();
        return true;
    }
    PropertyNode* properties = new PropertyNode(Sui::ITEM_PROPS);
    properties->makeDescendant(Sui::NAME, Sui::INSERT_SYMBOL);
    isymbol = new PlainInsertSymbol(properties);
    plainEditor->plainDoc()->appendChild(isymbol);
    isymbol->buildChildren(&restore_ui_item);
    isymbol->attach();
    isymbol->grabFocus();
    return true;
}
