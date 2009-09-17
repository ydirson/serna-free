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

/** \file
 */

#ifndef SPELLER_AGENT_H_
#define SPELLER_AGENT_H_

#include "SpellerReactor.h"
#include "ui/LiquidItem.h"

#include "common/common_defs.h"
#include "common/RangeString.h"

namespace Common {
    class PropertyNode;
}

class DocSpeller;
class SpellCheckDialog;

// dialog event call sequence:
// QtDialog(slot) -> SpellerAgent(Reactor) -> DocSpeller(Reactor)

class SpellerAgent : public Sui::LiquidItem, private SpellerReactor {
public:
    SpellerAgent(DocSpeller* dsp);
    virtual ~SpellerAgent();

    void            show();
    void            setSpeller(DocSpeller& dsp);

private:
    SpellerAgent(const SpellerAgent&);
    SpellerAgent& operator=(const SpellerAgent&);
    
    /// reimplemented from LiquidItem
    virtual int         toolDockMask() const { return UNDOCKED_TOOL; }
    virtual QWidget*    makeWidget(QWidget* parent, LiquidItem::Type dlgtype);
    virtual Common::String itemClass() const;
    
    /// reimplemented from SpellerReactor
    virtual bool ignore(const RangeString&);
    virtual bool ignoreAll(const RangeString&);
    virtual bool add(const RangeString&);
    virtual bool change(const RangeString& word, const RangeString& repl);
    virtual bool changeAll(const RangeString& word, const RangeString& repl);
    virtual bool skipElement();
    virtual bool setDict(const RangeString&);
    virtual bool start();
    virtual bool shutdown();
    virtual PropertyNode* getProps() const;

    Common::PropertyNode*   get_prop(const Common::String& pname);
    void                    update_dlg();
    bool                    checkResult(bool rv);

    DocSpeller*         speller_;
    SpellCheckDialog*   dlg_;
};

#endif // SPELLER_AGENT_H_
