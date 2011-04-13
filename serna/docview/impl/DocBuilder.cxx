// 
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
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

#include "docview/DocBuilder.h"
#include "docview/EventTranslator.h"
#include "utils/SernaUiItems.h"
#include "utils/Config.h"
#include "proputils/PropertyTreeSaver.h"
#include "ui/ActionSet.h"
#include "common/PathName.h"

#include <QApplication>
#include <QMessageBox>

#include <iostream>

using namespace Common;

static PropertyNode* find_doceditor_props(PropertyNode* pn)
{
    if (!pn)
        return 0;
    if (Sui::STRUCT_EDITOR == pn->name()) 
        return pn->getProperty("properties");
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling()) {
        PropertyNode* res = find_doceditor_props(pn);
        if (res)
            return res;
    }
    return 0;
}

void DocBuilder::load_sui(const String& name)
{
    if (actionList_)
        return;     // already built
    PathName path(config().getSuiDir());
    path.append(name + ".sui");
    suiProps_ = new PropertyNode(name);
    PropUtils::PropertyTreeSaver loader(suiProps_.pointer(),
        suiProps_->name());
    if (!loader.readPropertyTree(path.name())) {
        QMessageBox::critical(qApp->activeWindow(), 
                              "Error Loading UI Description", 
                              loader.errmsg());
        exit(0);
    }
    actionList_ = suiProps_->getProperty("/properties/uiActions");
    if (!actionList_) {
        QMessageBox::critical(qApp->activeWindow(), 
                              "Invalid UI Description", 
                              "Builtin UI actions are not defined");
        exit(0);
    }
    docEditorProps_ = find_doceditor_props(suiProps_.pointer());
}

void DocBuilder::makeActions(Sui::ActionSet* actionSet) const
{
    for (PropertyNode* c = actionList_->firstChild(); 
         c; c = c->nextSibling()) {
        if ("uiAction" == c->name())
            actionSet->makeAction(c);
    }
}

DocBuilder::DocBuilder()
    : actionList_(0), docEditorProps_(0)
{
}

DocBuilder::~DocBuilder()
{
}
