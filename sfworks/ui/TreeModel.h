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
#ifndef UI_TREE_MODEL_H_
#define UI_TREE_MODEL_H_

#include "ui/ui_defs.h"
#include "common/SernaApiBase.h"

#include <QAbstractItemModel>

namespace Sui {


class UI_EXPIMP QTreeModel : public QAbstractItemModel {
public:
    typedef SernaApiBase    TreeItem;

    QTreeModel(QObject* parent = 0);
    virtual ~QTreeModel();

    virtual TreeItem*   rootItem() const = 0;
    virtual TreeItem*   parentItem(TreeItem* item) const = 0;
    virtual TreeItem*   firstChild(TreeItem* item) const = 0;
    
    virtual TreeItem*   childItem(TreeItem* item, int index) const = 0;
    virtual int         itemIndex(TreeItem* item) const = 0;
    virtual int         childCount(TreeItem* item) const = 0;
    virtual int         columnCount(TreeItem* item) const = 0;

    virtual QVariant    data(TreeItem* item, int column, int role) const = 0;
    virtual bool        setData(TreeItem* /*item*/, int /*column*/, 
                                const QVariant& /*value*/,
                                int /*role*/) { return false; }
    
    virtual Qt::ItemFlags flags(TreeItem* item, int column) const = 0;

//! Model items navigation

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = 
                              QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;

//! Read-Only / Editable access to tree data

    virtual int         rowCount(const QModelIndex& parent = 
                                 QModelIndex()) const;
    virtual int         columnCount(const QModelIndex& parent = 
                                    QModelIndex()) const;
    virtual bool        hasChildren(const QModelIndex& parent = 
                                    QModelIndex()) const;

    virtual QVariant    data(const QModelIndex& index, 
                             int role = Qt::DisplayRole) const;
    virtual bool        setData(const QModelIndex& index, 
                                const QVariant& value, 
                                int role = Qt::EditRole);
    virtual QVariant    headerData(int section, Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const;
    //virtual bool setHeaderData(int section, Qt::Orientation orientation, 
    //                           const QVariant &value, int role = Qt::EditRole);

    //virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
    //virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

//! Drag and Drop support

    /*
    virtual QStringList     mimeTypes() const {};
    virtual QMimeData*      mimeData(const QModelIndexList& indexes) const {};
    virtual Qt::DropActions supportedDropActions() const {};
    virtual bool            dropMimeData(const QMimeData* data, 
                                         Qt::DropAction action,
                                         int row, int column, 
                                         const QModelIndex& parent) {};
    */

    //Qt::DropActions supportedDragActions() const;
    //void setSupportedDragActions(Qt::DropActions);


    //virtual void fetchMore(const QModelIndex &parent);
    //virtual bool canFetchMore(const QModelIndex &parent) const;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    //virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    //virtual QModelIndex buddy(const QModelIndex &index) const;
    //virtual QModelIndexList match(const QModelIndex &start, int role,
    //                              const QVariant &value, int hits = 1,
    //                              Qt::MatchFlags flags =
    //                              Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
    //virtual QSize span(const QModelIndex &index) const;


//public Q_SLOTS:
    //virtual bool submit();
    //virtual void revert();

protected:
    TreeItem*    treeItem(const QModelIndex& index) const;
};

}

#endif // UI_TREE_MODEL_H_

