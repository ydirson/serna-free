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

#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"

#include "ui/UiItemSearch.h"

#include "structeditor/StructEditor.h"
#include "structeditor/StructDocument.h"
#include "structeditor/InsertTextEventData.h"
#include "editableview/EditableView.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

using namespace Common;
using namespace InsertSymbolSpace;
using namespace Formatter;

class InsertText;

class StructInsertSymbol : public InsertSymbolItem {
public:
    StructInsertSymbol(PropertyNode* properties)
        : InsertSymbolItem(properties),
          structEditor_(0) {}
private:
    virtual bool doInsert()
    {
        if (0 == structEditor_)
            return false;
        Char sym = getInt(SYMBOL);
        InsertTextEventData text(String(&sym, 1));
        return makeCommand<InsertText>(&text)->execute(structEditor_);
    }
    //! Tells that this item is inserted/removed from parent
    virtual void inserted()
    {
        Sui::Item* doc = documentItem();
        if (0 == doc)
            return;
        structEditor_ = dynamic_cast<StructEditor*>(
            doc->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    };
    virtual void removed() { structEditor_ = 0; }


private:
    StructEditor* structEditor_;
};

namespace Sui {
    COMMANDLESS_ITEM_MAKER(StructInsertSymbol, StructInsertSymbol)
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CallStructInsertSymbol, StructEditor)

bool CallStructInsertSymbol::doExecute(StructEditor* se, EventData*)
{
    InsertSymbolItem* item = InsertSymbolItem::findInstance(se->sernaDoc());
    if (item) {
        item->grabFocus();
        return true;
    }
    PropertyNode* properties = new PropertyNode(Sui::ITEM_PROPS);
    properties->makeDescendant(Sui::NAME, Sui::INSERT_SYMBOL);
#if 0
    //TODO: get font from explicit formatter api (todo)
    if (!se->editableView().context().areaPos().isNull()) {
        ChainPos chain_pos = se->editableView().context().areaPos();
        if (!chain_pos.isNull())
            if (!chain_pos.chain()->font().isNull())
                properties->makeDescendant(
                FONT_NAME, chain_pos.chain()->font()->fontFamily());
    }
#endif // 0
    Sui::LiquidItem* isymbol = new StructInsertSymbol(properties);
    se->sernaDoc()->appendChild(isymbol);
    isymbol->buildChildren(&restore_ui_item);
    isymbol->attach();
    isymbol->grabFocus();
    return true;
}
