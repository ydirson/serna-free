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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "qt/SpellCheckDialog.h"
#include "SpellerReactor.h"
#include "speller_debug.h"

#include "common/String.h"
#include "common/StringUtils.h"
#include "common/StringCvt.h"

SpellCheckDialog::SpellCheckDialog(SpellerReactor* r) : reactor_(r) {}
SpellCheckDialog::~SpellCheckDialog() {}

void SpellCheckDialog::setMisspell(const Word& word)
{
    misspell_.assign(word);
    doSetMisspell(word);
}

const SpellCheckDialog::Word& SpellCheckDialog::getMisspell() const
{
    return misspell_;
}

void SpellCheckDialog::clearSuggestions()
{
    doClearSuggestions();
}

void SpellCheckDialog::addToSuggestions(const Word& word)
{
    doAddToSuggestions(word);
}

void SpellCheckDialog::addDict(const Word& dict)
{
    doAddDict(dict);
}

void SpellCheckDialog::setDict(const Word& dict)
{
    doSetDict(dict);
}

bool SpellCheckDialog::exec()
{
    return doExec();
}

QWidget* SpellCheckDialog::getWidget()
{
    return doGetWidget();
}

SpellerReactor& SpellCheckDialog::getReactor() const
{
    return *reactor_;
}

