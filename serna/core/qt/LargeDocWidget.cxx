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

#include "core/LargeDocWidgetBase.hpp"
#include "core/Preferences.h"
#include "ui/UiPropertySyncher.h"
#include "common/PropertyTree.h"
#include "utils/Properties.h"

using namespace Common;

class LargeDocWidget : public QWidget,
                       public Ui::LargeDocWidgetBase {
    Q_OBJECT
public:
    LargeDocWidget(Common::PropertyNode* prop);

private:
    Sui::PropertySyncherPtr  largeLimitSyncher_;
    Sui::PropertySyncherPtr  foldingModeSyncher_;
    Sui::PropertySyncherPtr  undoDepthSyncher_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(LargeDoc, 5, true)

LargeDocWidget::LargeDocWidget(PropertyNode* root)
{
    setupUi(this);
    PropertyNode* folding =
        root->makeDescendant("doc-defaults/folding-rules");
    largeLimitSyncher_ = new Sui::SpinnerSyncher(
        folding->makeDescendant("large-doc-limit"), largeLimitSpinner_);
    Sui::SelectorSyncher* selector_syncher = new Sui::SelectorSyncher(
        folding->makeDescendant("folding-mode", "always-fold", false));
    foldingModeSyncher_ = selector_syncher;

    selector_syncher->addSelector(NOTR("always-fold"), alwaysFoldButton_);
    selector_syncher->addSelector(NOTR("ask-user"), askUserButton_);
    selector_syncher->addSelector(NOTR("never-fold"), neverFoldButton_);

    undoDepthSyncher_ = new Sui::SpinnerSyncher(root->makeDescendant(
        App::APP)->makeDescendant(App::MAX_UNDO_DEPTH), undoDepth_);
}

#include "moc/LargeDocWidget.moc"
