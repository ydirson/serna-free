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
#include "structeditor/AttrPropertyModel.h"
#include "structeditor/qt/AttributeItemDelegate.h"

#include <QComboBox>

#include <iostream>

AttributeItemDelegate::AttributeItemDelegate(QObject* parent)
    : QItemDelegate(parent)     
{}
    
QWidget* AttributeItemDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const 
{
    QVariant editor_type = index.data(EDITOR_TYPE_ROLE);
    if (editor_type.isNull())
        return QItemDelegate::createEditor(parent, option, index);
    QVariant data = index.data(editor_type.toInt());
    
    switch (editor_type.toInt()) {
        case EDIT_COMBO_ROLE: {
            QComboBox* combo = new QComboBox(parent);
            combo->setObjectName(NOTR("AttributeItemEditor"));
            if (data.type() == QVariant::StringList) {
                QStringList str_list = data.toStringList();
                str_list.sort();
                QListIterator<QString> i(str_list);
                while (i.hasNext())
                    combo->addItem(i.next());
            }
            if (ATTR_NAME_COLUMN == index.column())
                combo->setEditable(true);
            return combo;
            break;
        }
        default:
            QWidget* editor = 
                QItemDelegate::createEditor(parent, option, index);
            editor->setObjectName(NOTR("AttributeItemEditor"));
            return editor;
            break;
    }
}

void AttributeItemDelegate::setEditorData(QWidget* editor, 
                                          const QModelIndex& index) const 
{
    QVariant editor_type = index.data(EDITOR_TYPE_ROLE);
    if (editor_type.isNull())
        return QItemDelegate::setEditorData(editor, index);

    QVariant data = index.data(Qt::DisplayRole);    
    switch (editor_type.toInt()) {
        case EDIT_COMBO_ROLE: {
            QComboBox* combo = dynamic_cast<QComboBox*>(editor);
            if (!combo)
                return;
            int index = combo->findText(data.toString());
            if (-1 == index) 
                combo->setEditText(data.toString());
            else
                combo->setCurrentIndex(index);
            break;
        }
        default:
            return QItemDelegate::setEditorData(editor, index);
            break;
    }
}

void AttributeItemDelegate::setModelData(QWidget* editor, 
                                         QAbstractItemModel* model,
                                         const QModelIndex& index) const 
{
    QVariant editor_type = index.data(EDITOR_TYPE_ROLE);
    if (editor_type.isNull())
        return QItemDelegate::setModelData(editor, model, index);

    switch (editor_type.toInt()) {
        case EDIT_COMBO_ROLE: {
            QComboBox* combo = static_cast<QComboBox*>(editor);
            model->setData(index, QVariant(combo->currentText()), 
                           Qt::EditRole);
            break;
        }
        default:
            return QItemDelegate::setModelData(editor, model, index);
            break;
    }
}

void AttributeItemDelegate::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option, 
    const QModelIndex& index) const 
{
    editor->setGeometry(option.rect);
}
