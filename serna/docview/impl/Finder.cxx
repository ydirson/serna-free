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
#include "docview/Finder.h"
#include "docview/impl/debug_dv.h"
#include "common/PropertyTreeEventData.h"

#include "ui/UiItemSearch.h"

#include <qwidget.h>

using namespace Common;

namespace Find
{

typedef DOCVIEW_EXPIMP const char* const exported_literal;

// START_IGNORE_LITERALS
exported_literal FIND_TEXT            = "findText";
exported_literal REPLACE_TEXT         = "replaceText";
exported_literal MATCH_CASE           = "matchCase";
exported_literal SEARCH_BACKWARDS     = "searchBackwards";
exported_literal SEARCH_TYPE          = "searchType";
exported_literal REPLACE_MODE         = "replaceMode";

exported_literal TEXT_TYPE            = "text";
exported_literal ATTRVAL_TYPE         = "attrval";
exported_literal ATTRNAME_TYPE        = "attrname";
exported_literal COMMENT_TYPE         = "comment";
exported_literal PATTERN_TYPE         = "pattern";
// STOP_IGNORE_LITERALS

}

Finder::Finder(PropertyNode* props, bool isStruct)
    : LiquidItem(props), 
      isStruct_(isStruct)
{
    itemProps()->makeDescendant("caption")->setString(tr("Find and Replace"));
}

void Finder::setReplaceTab(bool isReplace)
{
    if (findProps())
        findProps()->makeDescendant(Find::REPLACE_MODE)->setBool(isReplace);
    grabFocus();
}

Finder::~Finder()
{
}

bool Finder::find()
{
    if (!doFind(false)) {
        if (showNotFound(true)) {
            if (doFind(true))
                return true;
            showNotFound(false);
        } 
        return false;
    }
    return true;
}

bool Finder::replace()
{
    if (hasSelection()) {
        replaceSelection();
        return replace();
    }
    if (!doFind(false)) {
        if (showSearchFinished(true)) {
            if (doFind(true))
                return true;
            showSearchFinished(false);
        }
        return false;
    }
    return true;
}

bool Finder::replaceAll()
{
    if (hasSelection())
        replaceSelection();
    int found = 0;
    while (doFind()) {
        ++found;
        replaceSelection();
    }
    if (0 == found) {
        showNotFound(false);
        return false;
    } else {
        showChanged(found);
        return true;
    }
}

Finder* Finder::findInstance(const Sui::Item* from)
{
    return dynamic_cast<Finder*>(from->findItem(Sui::ItemClass(Sui::FINDER)));
}

