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
#ifndef CUSTOM_CONTENT_DIALOG_H_
#define CUSTOM_CONTENT_DIALOG_H_

#include "common/PropertyTree.h"
#include "CustomContentDialogBase.hpp"

class QEvent;

class CustomNameChecker {
public:
    virtual bool            isValidName(const Common::String& name) const = 0;
    virtual Common::String  getValidName(const Common::String& name) const = 0;
    virtual ~CustomNameChecker() {}
};

class StructEditor;

class CustomContentDialog : public QDialog,
                            public Ui::CustomContentDialogBase {
    Q_OBJECT
public:
    typedef Common::String String;
    
    CustomContentDialog(CustomNameChecker* nameChecker,
                        Common::PropertyNode* contentProp,
                        QWidget* parent, StructEditor* se);
protected slots:
    void    on_iconComboBox__textChanged(const QString& path);
    void    on_inscriptionLineEdit__textChanged() { dataChanged(); }
    void    on_nameLineEdit__textChanged() { dataChanged(); }
    void    on_hotKeyLineEdit__textChanged();
    void    on_browseIconButton__clicked();
    void    on_okButton__clicked();
    void    on_helpButton__clicked();
    void    on_addContentButton__clicked();
    void    on_removeContentButton__clicked();
    void    on_moveLeftButton__clicked();
    void    on_moveRightButton__clicked();

    void    updateContextButtons();
    void    patternChanged();
    void    locationChanged();

protected:
    bool                    eventFilter(QObject* receiver, QEvent *e);
    void                    addContent(Common::PropertyNode* context);
    bool                    isValidPattern(const String& pattern) const;
    bool                    isValidLocation(const String& xpathLoc) const;
    
private:
    void                    dataChanged();

    CustomNameChecker*      nameChecker_;
    Common::PropertyNode*   contentProp_;
    Common::String          initialName_;
    StructEditor*           se_;
};

#endif // CUSTOM_CONTENT_DIALOG_H_
