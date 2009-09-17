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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef ONLINE_SPELLER_H_
#define ONLINE_SPELLER_H_

#include "common/RangeString.h"

class StructEditor;
class SpellCheckerSet;
class DocumentPlugin;

namespace Sui {
    class Action;
}

class OnlineSpeller {
public:
    static OnlineSpeller* make(StructEditor* se, DocumentPlugin* dp, 
                               bool isOn, SpellCheckerSet&);

    virtual void toggleState(bool) = 0;
    virtual void recheck() = 0;
    virtual void updateMenu() = 0;
    virtual void selectSuggestion() = 0;
    virtual void replaceSuggestion() = 0;
    virtual void add(bool ignored) = 0;
    virtual void unmarkWord(const Common::RangeString&) = 0;

    OnlineSpeller() {}
    virtual ~OnlineSpeller() {}

private:
    OnlineSpeller(const OnlineSpeller&);
    OnlineSpeller& operator=(const OnlineSpeller&);
};

#endif // ONLINE_SPELLER_H_
