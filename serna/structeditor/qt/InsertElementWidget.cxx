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
#include "structeditor/StructEditor.h"
#include "structeditor/InsertElementItem.h"
#include "docview/SernaDoc.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Properties.h"
#include "utils/HelpAssistant.h"
#include "utils/ElementHelp.h"
#include "utils/NameChecker.h"
#include "docutils/doctags.h"
#include "editableview/EditableView.h"
#include "ui/IconProvider.h"
#include "ui/UiPropertySyncher.h"

#include "structeditor/InsertElementToolBase.hpp"
#include "structeditor/InsertElementMiniToolBase.hpp"
#include "structeditor/InsertElementDialogBase.hpp"
#include "structeditor/RenameElementDialogBase.hpp"

#include <QAbstractItemModel>
#include <QApplication>
#include <QFont>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDialog>
#include <QShortcut>
#include <QDockWidget>
#include <QTimer>

using namespace Common;
using namespace InsertElementProps;

static const char ELEM_HELP_ICON[] = NOTR("element_help");

class ElementListModel : public QAbstractItemModel {
public:
    class ElementItem {
    public:
        ElementItem(const QString& str, int recent_idx)
            : str_(str), recentIndex_(recent_idx) {}
        QString str_;
        int     recentIndex_;
    };
    ElementListModel(QObject* parent, StructEditor* se)
        : QAbstractItemModel(parent),
          structEditor_(se), elemList_(0), hasCdata_(false)
    {
        items_.reserve(512);
        fonts_[0] = fonts_[1] = qApp->font();
        fonts_[0].setBold(true);
        fonts_[0].setUnderline(true);
    }
    virtual int columnCount(const QModelIndex&) const { return 2; }
    virtual QModelIndex index(int row, int col,
                              const QModelIndex& = QModelIndex()) const
    {
        return (row < int(items_.size())) 
            ? createIndex(row, col, (void*) &items_[row]) : QModelIndex();
    }
    virtual QModelIndex parent(const QModelIndex&) const
    {
        return QModelIndex();
    }
    virtual int rowCount(const QModelIndex& = QModelIndex()) const
    {
        return items_.size();
    }
    QModelIndex         find(const QString& s)
    {
        for (uint i = 0; i < items_.size(); ++i)
            if (items_[i].str_.startsWith(s))
                return index(i, 1);
        return QModelIndex();
    }
    virtual QVariant    data(const QModelIndex& index, int role) const;

    void                fillElemList();
    void                setElementList(PropertyNode* pn) { elemList_ = pn; }
    void                setCdataState(bool v);

private:
    StructEditor*       structEditor_;
    Vector<ElementItem> items_;
    QFont               fonts_[2];
    PropertyNodePtr     elemList_;
    bool                hasCdata_;
};

//////////////////////////////////////////////////////////////////

class InsertElementToolCommon : public QWidget,
                                public InsertElementNotifications {
    Q_OBJECT
public:
    InsertElementToolCommon(QWidget* parent, InsertElementItem* item);
    void    setupCommon(StructEditor*, QTreeView*, QLineEdit*, QToolButton*);
    
    virtual void updateSelectionState(bool sel, bool hasCdata);
    virtual void setElementList(Common::PropertyNode* pn) 
    {
        nameListModel_->setElementList(pn);
    }
    virtual void    updateElementList();
    void            setText(const QString&);

public slots:
    virtual void    grabFocus();
    virtual void    help() const;
    virtual void    insertClicked();
    virtual void    currentChanged(const QModelIndex& i, const QModelIndex&);
    void            nameChanged(const QString&);

public:
    virtual bool    eventFilter(QObject*, QEvent*);

    ElementListModel*   nameListModel_;
    QTreeView*          nameListWidget_;
    QLineEdit*          nameEditWidget_;
    QToolButton*        insButton_;

    QString             lastEditText_;
    bool                textChangedLock_;
};

InsertElementToolCommon::InsertElementToolCommon(QWidget* parent, 
                                                 InsertElementItem* item)
    : QWidget(parent),
      InsertElementNotifications(item),
      textChangedLock_(false)
{
}

static void setup_listview(QTreeView* view, QWidget* bindTo)
{
    view->setRootIsDecorated(false);
    view->setUniformRowHeights(true);
    view->setItemsExpandable(false);
    view->setAllColumnsShowFocus(true);

    QHeaderView& header = *view->header();
    header.setResizeMode(QHeaderView::Fixed);
    QFontMetrics metrix(view->font());
    header.resizeSection(0, metrix.width("W") + 4);
    header.setStretchLastSection(true);
    header.hide();
    QObject::connect(view->selectionModel(), 
        SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
        bindTo, SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
    QObject::connect(view, SIGNAL(activated(const QModelIndex&)),
        bindTo, SLOT(insertClicked()));
}

void InsertElementToolCommon::setupCommon(StructEditor* se,
                                          QTreeView* nameList, 
                                          QLineEdit* nameEdit,
                                          QToolButton* insButton)
{
    nameListWidget_ = nameList;
    nameEditWidget_ = nameEdit;
    insButton_      = insButton;
    insButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("insert_element")));
    nameListModel_ = new ElementListModel(this, se);
    nameListWidget_->setModel(nameListModel_);  
    nameListWidget_->setFocusProxy(nameEditWidget_);
    setup_listview(nameListWidget_, this);
    connect(nameEditWidget_, SIGNAL(textChanged(const QString&)), 
        this, SLOT(nameChanged(const QString&)));
    connect(insButton_, SIGNAL(clicked()), this, SLOT(insertClicked()));
    nameEditWidget_->installEventFilter(this);
    //connect(new QShortcut(QKeySequence(Qt::Key_F1), this),
    //    SIGNAL(activated()), this, SLOT(help()));
}

void InsertElementToolCommon::updateElementList()
{
    nameListModel_->fillElemList();
    QModelIndex zero_index(nameListModel_->index(0, 1));
    nameListWidget_->setCurrentIndex(zero_index);
    setText(zero_index.data(Qt::DisplayRole).toString());
    if (nameEditWidget_->hasFocus())
        nameEditWidget_->selectAll();
}

void InsertElementToolCommon::updateSelectionState(bool sel, bool hasCdata)
{
    if (!item_)
        return;
    item_->set(Sui::INSCRIPTION, 
        sel ? QT_TRANSLATE_NOOP("InsertElementToolCommon",
                "Wrap Into Element") :
              QT_TRANSLATE_NOOP("InsertElementToolCommon",
                "Insert Element"));
    setCaption(item_->getTranslated(Sui::INSCRIPTION));
    nameListModel_->setCdataState(hasCdata);
}

void InsertElementToolCommon::nameChanged(const QString& text)
{
    insButton_->setEnabled(!nameEditWidget_->text().isEmpty() &&
        NameChecker::isValidQname(nameEditWidget_->text()));
    if (textChangedLock_)
        return;
    QModelIndex idx = nameListModel_->find(text);
    if (!idx.isValid() || int(lastEditText_.length()) >= text.length()) {
        lastEditText_ = text;
        return;
    }
    QString matched = idx.data(Qt::DisplayRole).asString();
    nameListWidget_->setCurrentIndex(idx);
    setText(matched);
    nameEditWidget_->setSelection(text.length(),
        matched.length() - text.length());
    lastEditText_ = text;
}

void InsertElementToolCommon::setText(const QString& qs)
{
    textChangedLock_ = true;
    nameEditWidget_->setText(qs);
    textChangedLock_ = false;
    nameEditWidget_->selectAll();
}

void InsertElementToolCommon::grabFocus()
{
    nameEditWidget_->setFocus();
}

void InsertElementToolCommon::insertClicked()
{
    item_->doInsert(nameEditWidget_->text());
    item_->releaseFocus();
}

void InsertElementToolCommon::currentChanged(const QModelIndex& i,
                                             const QModelIndex&)
{
    if (i.isValid()) {
        QModelIndex new_idx = nameListModel_->index(i.row(), 1);
        setText(new_idx.data(Qt::DisplayRole).asString());
        nameEditWidget_->selectAll();
    }
}

bool InsertElementToolCommon::eventFilter(QObject*, QEvent* event)
{
    if (event->type() != QEvent::KeyPress)
        return false;
    QKeyEvent* kev = static_cast<QKeyEvent*>(event);
    if (kev->state() & Qt::AltButton) {
        if (kev->key() < '1' || kev->key() > '5')
            return false;
        currentChanged(nameListModel_->index(kev->key() - '1', 1),
            QModelIndex());
        insertClicked();
        return true;
    }
    if (kev->state() & (Qt::ControlButton|Qt::MetaButton|Qt::ShiftButton))
        return false;
    switch (kev->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Prior:
        case Qt::Key_Next:
        {
            QApplication::sendEvent(nameListWidget_, kev);
            nameEditWidget_->setFocus();
            return true;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter:
            insertClicked();
            return true;
        default:
            return false;
    }
}

void InsertElementToolCommon::help() const
{
    helpAssistant().show(DOCTAG(INS_EL_D));
}

////////////////////////////////////////////////////////////////

class InsertElementTool : public InsertElementToolCommon,
                          public Ui::InsertElementToolBase {
    Q_OBJECT
public:
    InsertElementTool(QWidget* parent,
                      InsertElementItem* item);

    void            setup(QWidget* setupWidget, StructEditor*);
    void            elemHelpImpl(StructEditor* se) const;

public slots:
    virtual void    elemHelp() const;
};

InsertElementTool::InsertElementTool(QWidget* parent,
                                     InsertElementItem* item)
    : InsertElementToolCommon(parent, item)
{
}

void InsertElementTool::elemHelp() const 
{
    if (0 == item_)
        return;
    elemHelpImpl(item_->structEditor());
}

void InsertElementTool::elemHelpImpl(StructEditor* se) const 
{
    if (!se->helpHandle())
            return;
    QString elem_name = nameEditWidget_->text();
    helpAssistant().showLongHelp(se->helpHandle()->elemHelp(elem_name,
        se->editViewSrcPos().node()).pointer(),
        NOTR("&lt;") + elem_name + NOTR("&gt;"));
}

void InsertElementTool::setup(QWidget* setupWidget, StructEditor* se)
{
    setupUi(setupWidget);
    setupCommon(se, nameList_, nameEdit_, insertToolButton_);
    elemHelpButton_->setIconSet(
        Sui::icon_provider().getIconSet(ELEM_HELP_ICON));
    connect(elemHelpButton_, SIGNAL(clicked()), this, SLOT(elemHelp()));
}

////////////////////////////////////////////////////////////////

class InsertElementMiniTool : public InsertElementToolCommon,
                              public Ui::InsertElementMiniToolBase {
    Q_OBJECT
public:
    InsertElementMiniTool(QWidget* parent,
                          InsertElementItem* item);
    void            setup(QWidget* setupWidget, StructEditor*);
public slots:
    void    on_listCombo__activated(int index) 
    {
        currentChanged(nameListModel_->index(index, 1), QModelIndex());
        insertClicked();
    }
};

InsertElementMiniTool::InsertElementMiniTool(QWidget* parent,
                                             InsertElementItem* item)
    : InsertElementToolCommon(parent, item)
{
}

void InsertElementMiniTool::setup(QWidget* setupWidget, StructEditor* se)
{
    setupUi(setupWidget);
    QTreeView* tree_view = new QTreeView(this);
    setupCommon(se, tree_view, listCombo_->lineEdit(), insertButton_);
    listCombo_->setModel(nameListModel_);
    listCombo_->setView(tree_view);
    listCombo_->setModelColumn(1);
    tree_view->setSelectionBehavior(QTreeView::SelectRows);
}

///////////////////////////////////////////////////////////////////

class RenameElementDialog : public QDialog,
                            public Ui::RenameElementDialogBase {
    Q_OBJECT
public:
    class RenameTool : public InsertElementTool {
    public:
        RenameTool(RenameElementDialog* dialog, StructEditor* se);
        virtual void insertClicked() { dialog_->accept(); }
        virtual void help() const;
        virtual void elemHelp() const { elemHelpImpl(structEditor_); }

    private:
        RenameElementDialog* dialog_;
        StructEditor*        structEditor_;
    };
    RenameElementDialog(QWidget* parent, const PropertyNode*, StructEditor*);
    QString result() const { return renameTool_->nameEditWidget_->text(); }

public slots:
    void    on_insertButton__clicked() { accept(); }
    void    on_closeButton__clicked()  { reject(); } 

private:
    RenameTool* renameTool_;
};

RenameElementDialog::RenameElementDialog(QWidget* parent,
                                         const PropertyNode* elemList,
                                         StructEditor* se)
    : QDialog(parent)
{
    setModal(true);
    RenameElementDialogBase::setupUi(this);
    renameTool_ = new RenameTool(this, se);
    setCaption(elemList->getSafeProperty(CAPTION)->getString());
    // hack - need to pass nonconst ptr
    renameTool_->setElementList(elemList->copy(true)); 
    renameTool_->updateElementList();
    renameTool_->grabFocus();
}

RenameElementDialog::RenameTool::RenameTool(RenameElementDialog* dialog, 
                                            StructEditor* se)
    : InsertElementTool(dialog, 0), dialog_(dialog), structEditor_(se)
{
    setup(dialog_->toolWidget_, se);
}

void RenameElementDialog::RenameTool::help() const
{
    helpAssistant().show(DOCTAG(REN_EL_D));
}
///////////////////////////////////////////////////////////////////

class InsertElementDialog : public InsertElementTool,
                            public Ui::InsertElementDialogBase {
    Q_OBJECT
public:
    InsertElementDialog(QWidget* parent, InsertElementItem* item);
    virtual QSize sizeHint() const 
    { 
        return QSize(normalGeometry().width(), normalGeometry().height());
    }

public slots:
    void    on_insertButton__clicked() { insertClicked(); }
    void    on_closeButton__clicked() { reject(); } 
    void    reject();
    void    delayedRemove();
    virtual void insertClicked();

private:
    Sui::PropertySyncherPtr closePropSyncher_;
};

InsertElementDialog::InsertElementDialog(QWidget* parent,
                                         InsertElementItem* item)
    : InsertElementTool(parent, item)
{
    InsertElementDialogBase::setupUi(this);
    setup(toolWidget_, item->structEditor());
    if (!item_)
        return;
    closePropSyncher_ = new Sui::ButtonSyncher(
        item->documentItem()->itemProps()->makeDescendant(
            "insert-element-item/close-on-insert", "true", false),
        closeCheckBox_);
}

void InsertElementDialog::insertClicked()
{
    InsertElementTool::insertClicked();
    if (closeCheckBox_->isChecked())
        reject();
}

void InsertElementDialog::delayedRemove()
{
    if (!item_ || item_->getBool(Sui::IS_VISIBLE))
        return;
    item_->removeItem();
}

void InsertElementDialog::reject()
{
    item_->setBool(Sui::IS_VISIBLE, false);
    QTimer::singleShot(100, this, SLOT(delayedRemove()));
}

///////////////////////////////////////////////////////////////////

QVariant ElementListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= int(items_.size())
        || index.column() > 1)
            return QVariant();
    const ElementItem& item = items_[index.row()];
    switch (role) {
        case Qt::DisplayRole: {
            if (index.column())
                return QString(item.str_);
            if (item.recentIndex_ > 0)
                return QString::number(item.recentIndex_);
            break;
        }
        case Qt::FontRole:
            return fonts_[index.column()];
        case Qt::TextAlignmentRole:
            return index.column() ? Qt::AlignLeft : Qt::AlignCenter;
        case Qt::ToolTipRole: {
            if (!structEditor_->helpHandle())
                return QVariant();
            PropertyNodePtr pn = structEditor_->helpHandle()->
                elemHelp(item.str_, structEditor_->editViewSrcPos().node());
            if (pn.isNull())
                return QVariant();
            QString text =
                pn->getSafeProperty(HelpHandle::SHORT_HELP)->getString();
            return QVariant(text);
        }
    }
    return QVariant();
}

void ElementListModel::fillElemList()
{
    items_.clear();
    if (!elemList_) 
        return reset();
    const PropertyNode* elem = elemList_->getProperty(RECENT_ELEMENTS);
    if (elem)
        elem = elem->firstChild();
    for (int i = 1; elem; ++i, elem = elem->nextSibling()) 
        if (!hasCdata_ || elem->getProperty("mixed"))
            items_.push_back(ElementItem(elem->name(), i));
    elem = elemList_->getProperty(OTHER_ELEMENTS);
    if (elem)
        elem = elem->firstChild();
    for (; elem; elem = elem->nextSibling()) 
        if (!hasCdata_ || elem->getProperty("mixed"))
            items_.push_back(ElementItem(elem->name(), -1));
    reset();
}

void ElementListModel::setCdataState(bool hasCdata)
{
    if (hasCdata != hasCdata_) {
        hasCdata_ = hasCdata;
        fillElemList();
    }
}

QWidget* InsertElementItem::makeWidget(QWidget* parent, Type type)
{
    switch (type) {
        case VERTICAL_TOOL: {
            InsertElementTool* w = new InsertElementTool(parent, this);
            w->setup(w, structEditor());
            init(w);
            return w;
        }
        case HORIZONTAL_TOOL: {
            InsertElementMiniTool* w = new InsertElementMiniTool(parent, this);
            w->setup(w, structEditor());
            init(w);
            return w;
        }
        default: {
            InsertElementDialog* w = new InsertElementDialog(parent, this);
            init(w);
            return w;
        }
    }
}

///////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(RenameElementWidget, StructEditor)

bool RenameElementWidget::doExecute(StructEditor* se, EventData* result)
{
    RenameElementDialog d(se->sernaDoc()->widget(0), ed_, se);
    if (d.exec() != QDialog::Accepted)
        return false;
    static_cast<PropertyTreeEventData*>(result)->root()->makeDescendant(
        ElementSpace::ELEMENT_NAME, d.result(), true);
    return true;
}

#include "moc/InsertElementWidget.moc"
