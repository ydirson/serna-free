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
#ifndef XSLT_PARAMS_DIALOG_H_
#define XSLT_PARAMS_DIALOG_H_

#include "common/StringDecl.h"
#include "common/PropertyTree.h"
#include "XsltParamsDialogBase.hpp"

class StructEditor;

class XsltParamsDialog : public QDialog, 
                         private Ui::XsltParamsDialogBase {
    Q_OBJECT
public:
    XsltParamsDialog(StructEditor* se,
                     Common::PropertyNode* params,
                     Common::PropertyNode* defaltParams);
public slots:
    void    on_newButton__clicked();
    void    on_cloneButton__clicked();
    void    on_deleteButton__clicked();
    void    on_editButton__clicked();
    void    on_setButton__clicked();
    void    on_helpButton__clicked();
    void    on_applyButton__clicked();
    
    void    on_description__textChanged();
    
    void on_checkBox__stateChanged(int) { paramSetChanged(); }
    void on_paramsComboBox__activated(int) { paramSetChanged(); }

    void on_paramsListView__currentItemChanged(QTreeWidgetItem* i,
                                               QTreeWidgetItem*)
        { paramSelected(i); }
    void on_paramsListView__itemActivated(QTreeWidgetItem*, int) 
        { on_editButton__clicked(); }

private:
    void    clone(bool isNew = false);
    void    paramSetChanged();
    void    paramSelected(QTreeWidgetItem*);
    bool    currentIsDefault() const;

    StructEditor* se_;
    Common::PropertyNode*   params_;
    Common::PropertyNodePtr paramsCopy_;
    Common::PropertyNode*   default_;
    Common::PropertyNode*   curSet_;
};

#endif // XSLT_PARAMS_DIALOG_H_
