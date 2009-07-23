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
#include "ui/TreeModel.h"

namespace Sui {

////////////////////////////////////////////////////////////////////////////

QTreeModel::QTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{}
    
QTreeModel::~QTreeModel()
{}

////////////////////////////////////////////////////////////////////////////

QTreeModel::TreeItem* QTreeModel::treeItem(const QModelIndex& index) const
{
    return (index.isValid())
        ? static_cast<TreeItem*>(index.internalPointer())
        : rootItem();
}    

QModelIndex QTreeModel::index(int row, int column,  
                              const QModelIndex& parent) const
{
    TreeItem* child_item = childItem(treeItem(parent), row);
    if (child_item)
        return createIndex(row, column, child_item);
    else
        return QModelIndex();
}
  
QModelIndex QTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
         return QModelIndex();

    TreeItem* child_item = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parent_item = parentItem(child_item);

    if (parent_item == rootItem())
        return QModelIndex();

    return createIndex(itemIndex(parent_item), 0, parent_item);
}

int QTreeModel::rowCount(const QModelIndex& parent) const
{
    return childCount(treeItem(parent));
}

int QTreeModel::columnCount(const QModelIndex& parent) const
{
    return columnCount(treeItem(parent));
}

bool QTreeModel::hasChildren(const QModelIndex& parent) const
{
    return 0 != firstChild(treeItem(parent));
}    

QVariant QTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    return data(treeItem(index), index.column(), role);
}

bool QTreeModel::setData(const QModelIndex& index, const QVariant& value, 
                         int role) 
{
    if (!index.isValid())
        return false;
    return setData(treeItem(index), index.column(), value, role);
}

QVariant QTreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    return QVariant(QString("Blah"));
}
    
Qt::ItemFlags QTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return flags(treeItem(index), index.column());
}

} // namespace Sui



