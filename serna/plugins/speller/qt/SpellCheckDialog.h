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
#ifndef SPELL_CHECK_DIALOG_H
#define SPELL_CHECK_DIALOG_H

#include "common/String.h"
#include "ui/LiquidItem.h"

class SpellerReactor;
class QWidget;

class SpellCheckDialog {
public:
    typedef Common::String Word;
    //!
    void setMisspell(const Word& word);
    //!
    const Word& getMisspell() const;
    //!
    void clearSuggestions();
    //!
    void addToSuggestions(const Word& word);
    //!
    void addDict(const Word& dict);
    //!
    void setDict(const Word& dict);
    //!
    bool exec();
    //!
    QWidget* getWidget();
protected:
    // this interface must be implemented by all kinds of concrete spell dialogs
    virtual void doSetMisspell(const Word& word) = 0;
    virtual void doClearSuggestions() = 0;
    virtual void doAddToSuggestions(const Common::String& word) = 0;
    virtual void doAddDict(const Common::String& dict) = 0;
    virtual void doSetDict(const Common::String& dict) = 0;
    virtual bool doExec() = 0;
    virtual QWidget* doGetWidget() = 0;
    //!
    SpellerReactor&     getReactor() const;
    //!
    SpellCheckDialog(SpellerReactor* r);
    virtual ~SpellCheckDialog();
private:
    SpellerReactor*     reactor_;
    Word                misspell_;
};

SpellCheckDialog* makeSpellCheckDialog(QWidget* parent,
                                       SpellerReactor* reactor,
                                       Sui::LiquidItem::Type);

#endif // SPELL_CHECK_DIALOG_H
