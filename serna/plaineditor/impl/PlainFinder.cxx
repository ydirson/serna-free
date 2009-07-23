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
#include "ui/UiItemSearch.h"

#include "docview/dv_utils.h"
#include "docview/Finder.h"

#include "common/PropertyTreeEventData.h"

#include "plaineditor/PlainEditor.h"
#include "plaineditor/PlainDocument.h"

using namespace Common;

class PlainFinder : public Finder {
public:
    PlainFinder(Sui::Action*, PropertyNode* properties,
                bool isReplace = false)
        : Finder(properties, false),
          plainEditor_(0) {}
private:
    virtual bool doFind(bool start)
    {
        if (0 == plainEditor_)
            return false;
        bool ok = plainEditor_->find(
            findProps()->getString(Find::FIND_TEXT),
            findProps()->getSafeProperty(Find::MATCH_CASE)->getBool(),
            !findProps()->getSafeProperty(Find::SEARCH_BACKWARDS)->getBool(),
            start);
        return ok;
    }
    virtual bool replaceSelection()
    {
        if (0 == plainEditor_)
            return false;
        plainEditor_->cut();
        plainEditor_->insert(
            findProps()->getSafeProperty(Find::REPLACE_TEXT)->getString());
        return true;
    }
    virtual bool hasSelection() const
    {
        if (0 == plainEditor_)
            return false;
        return !plainEditor_->selectedText().isEmpty();
    }
    //! Tells that this item is inserted/removed from parent
    virtual void inserted()
    {
        Sui::Item* doc = documentItem();
        if (0 == doc)
            return;
        plainEditor_ = dynamic_cast<PlainEditor*>(
            doc->findItem(Sui::ItemClass(Sui::PLAIN_EDITOR)));
        if (plainEditor_)
            findProps_ = plainEditor_->plainDoc()->
                getDsi()->makeDescendant("recentFind");
    };
    virtual void removed() { plainEditor_ = 0; }

private:
    PlainEditor* plainEditor_;
};

namespace Sui {
    CUSTOM_ITEM_MAKER(PlainFinder, PlainFinder)
}

/////////////////////////////////////////////////////////////////////////

static void make_plain_finder(PlainEditor* plainEditor, bool isReplace)
{
    PropertyNode* properties = new PropertyNode(Sui::ITEM_PROPS);
    properties->makeDescendant(Sui::NAME, Sui::FINDER);
    Finder* finder = new PlainFinder(0, properties, isReplace);
    plainEditor->plainDoc()->appendChild(finder);
    finder->buildChildren(&restore_ui_item);
    finder->attach();
    finder->setReplaceTab(isReplace);
}

SIMPLE_COMMAND_EVENT_IMPL(CallPlainFind, PlainEditor)

bool CallPlainFind::doExecute(PlainEditor* plainEditor, EventData*)
{
    Finder* finder = Finder::findInstance(plainEditor->plainDoc());
    if (finder) {
        finder->setReplaceTab(false);
        finder->grabFocus();
        return true;
    }
    make_plain_finder(plainEditor, false);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(CallPlainFindReplace, PlainEditor)

bool CallPlainFindReplace::doExecute(PlainEditor* plainEditor, EventData*)
{
    Finder* finder = Finder::findInstance(plainEditor->plainDoc());
    if (finder) {
        finder->setReplaceTab(true);
        finder->grabFocus();
        return true;
    }
    make_plain_finder(plainEditor, true);
    return true;
}

