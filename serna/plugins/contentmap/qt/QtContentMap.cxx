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

#include "ui/UiItems.h"
#include "ui/UiAction.h"
#include "ui/IconProvider.h"
#include "common/safecast.h"

#include "contentmap/cm_debug.h"
#include "contentmap/ContentMap.h"
#include "contentmap/qt/QtContentMap.h"
#include "contentmap/csl/Template.h"
#include "contentmap/csl/Exception.h"
#include "contentmap/csl/RootInstance.h"
#include "contentmap/csl/Stylesheet.h"

#include "xslt/ResultOrigin.h"

#include "utils/SernaMessages.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"
#include "structeditor/StructEditor.h"
#include "structeditor/SernaDragData.h"
#include "common/safecast.h"

#include <qlayout.h>
#include <qtabbar.h>
#include <q3listview.h>
#include <q3header.h>
#include <qstring.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qtoolbutton.h>
#include <q3stylesheet.h>
#include <qbitmap.h>
//Added by qt3to4:
#include <QPixmap>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <Q3HBoxLayout>
#include <QEvent>
#include <Q3VBoxLayout>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <Q3PointArray>
#include <QWindowsStyle>

#include <iostream>
#include <map>

using namespace Csl;
using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

class ContentMapListItem : public Q3ListViewItem {
public:
    enum State { HIGHLIGHTED        = 0x01,
                 SELECTED           = 0x02,
                 SELECTED_PARENT    = 0x04,
                 NONE               = 0x0    
    };
    enum Type {
        PLACEHOLDER,
        ELEMENT_ITEM,
        ENTITY_ITEM,
        XINCLUDE_ITEM,
        PI_ITEM,
        COMMENT_ITEM
    };
    
    ContentMapListItem(Csl::Instance* instance, Q3ListView* listView);
    ContentMapListItem(Csl::Instance* instance, Q3ListViewItem* parent);
    ContentMapListItem(Csl::Instance* instance, Q3ListViewItem* parent, 
                       Q3ListViewItem* after);
    virtual ~ContentMapListItem() {};

    virtual Type    type() const = 0;
    Csl::Instance*  instance() const { return instance_; }

    virtual GroveEditor::GrovePos   grovePos() const;
    virtual void                    selection(GroveEditor::GrovePos& from,
                                              GroveEditor::GrovePos& to) const;
    virtual QString                 tooltip() const = 0;
    virtual Common::String          contextMenuName() const = 0;
    ContentMapListItem*             prevSibling() const;

    void            setState(State state, bool isOn);    
    bool            isState(State state) const { return state_ & state; }
    virtual void    dump() const {};

    //! reimplemented from QListViewItem
    virtual void    paintCell(QPainter* p, const QColorGroup& cg,
                              int column, int width, int align);
    virtual void    paintCell(QPainter* p, const QColorGroup& cg, int width);
    bool            isDragging() const { return isDragging_; }

protected:
    Csl::Instance*  instance_;
    QColor          textColor_;
    
private:
    int             state_;
    bool            isDragging_;
};

///////////////////////////////////////////////////////////////////////////

namespace {

class CursorPlaceholder : public ContentMapListItem {
public:
    CursorPlaceholder(Csl::Instance* afterInst, Q3ListViewItem* parent);
    CursorPlaceholder(Csl::Instance* afterInst, Q3ListViewItem* parent, 
                      Q3ListViewItem* after);
    virtual ~CursorPlaceholder() {};
    
    virtual Type    type() const { return PLACEHOLDER; }
    
    virtual GroveEditor::GrovePos   grovePos() const;
    virtual void                    selection(GroveEditor::GrovePos& from, 
                                              GroveEditor::GrovePos& to) const;
    virtual Common::String          contextMenuName() const;
    virtual QString                 tooltip() const;

    virtual void    setup();
    virtual void    dump() const;
};

///////////////////////////////////////////////////////////////////////////

class InstanceListItem : public ContentMapListItem {
public:
    InstanceListItem(Csl::Instance* instance, Q3ListView* listView);
    InstanceListItem(Csl::Instance* instance, Q3ListViewItem* parent);
    InstanceListItem(Csl::Instance* instance, Q3ListViewItem* parent,
                     Q3ListViewItem* after);
    virtual ~InstanceListItem();

    virtual Type    type() const { return ELEMENT_ITEM; }

    CursorPlaceholder*      placeholder() const { return placeholder_; }
    virtual Common::String  contextMenuName() const;
    virtual QString         tooltip() const;
    
    //! reimplemented to process instance
    virtual void        setOpen(bool isToOpen);
    //! reimplemented from QListViewItem
    virtual int         width(const QFontMetrics& fm,
                              const Q3ListView* list, int c) const;
    virtual void        dump() const;

protected:
    Common::String      text() const;
    //! reimplemented from QListViewItem
    virtual void        paintCell(QPainter* p, const QColorGroup& cg,
                                  int column, int width, int align);
private:
    CursorPlaceholder*  placeholder_;
};

///////////////////////////////////////////////////////////////////////////

class SpecialElement : public InstanceListItem {
public:
    SpecialElement(Csl::Instance* instance, Q3ListViewItem* parent);
    SpecialElement(Csl::Instance* instance, Q3ListViewItem* parent, 
                  Q3ListViewItem* after);
    virtual ~SpecialElement() {};
    
    virtual Type    type() const { return type_; }

    virtual Common::String          contextMenuName() const;
    virtual QString                 tooltip() const;

    virtual void        setup();
    virtual int         width(const QFontMetrics& fm,
                              const Q3ListView* list, int c) const;
    virtual Common::String text() const;

protected:
    //! reimplemented from QListViewItem
    virtual void        paintCell(QPainter* p, const QColorGroup& cg,
                                  int column, int width, int align);
protected:
    Type                type_;
    QPixmap             icon_;
};

///////////////////////////////////////////////////////////////////////////

class CmSectionCorner : public SpecialElement {
public:
    CmSectionCorner(Csl::Instance* instance, Q3ListViewItem* parent);
    CmSectionCorner(Csl::Instance* instance, Q3ListViewItem* parent, 
                  Q3ListViewItem* after);
    virtual ~CmSectionCorner() {};
    
    virtual GroveEditor::GrovePos   grovePos() const;
    virtual Common::String          contextMenuName() const;
    virtual QString                 tooltip() const;

    virtual void        selection(GroveEditor::GrovePos& from,
                                  GroveEditor::GrovePos& to) const;
    virtual void        setup();
    virtual Common::String text() const;
};

///////////////////////////////////////////////////////////////////////////

class ListViewInstance : public Csl::Instance {
public:
    ListViewInstance(const Csl::InstanceInit& data)
        : Csl::Instance(data) {}
    virtual ~ListViewInstance() {}

    void            dump() const;
    
protected:
    // called when text or children had changed
    virtual void    resultChanged() { 
        if (!listItem_.isNull()) {
            listItem_->widthChanged();
            listItem_->repaint();
        }
    }
public:
    Common::OwnerPtr<InstanceListItem>    listItem_;
};

inline const ContentMapListItem* item_cast(const Q3ListViewItem* item)
{
    return SAFE_CAST(const ContentMapListItem*, item);
}

} // namespace

extern STRUCTEDITOR_EXPIMP QString special_node_tooltip(const Node* node);

///////////////////////////////////////////////////////////////////////////

class ProfileMap : public std::map<int, const Profile*> {};

ProfileSelector::ProfileSelector(QWidget* parent,
                                 ContentMapWidget* contentMapWidget, 
                                 const Csl::Stylesheet* stylesheet)
    : QWidget(parent),
     stylesheet_(stylesheet),
     profileMap_(new ProfileMap()),
     contentMap_(contentMapWidget->contentMap()),
     policyAction_(contentMap_->focusPolicyAction())
{
    Q3VBoxLayout* vert_layout  = new Q3VBoxLayout(this);
    Q3HBoxLayout* horiz_layout = new Q3HBoxLayout(vert_layout);
    tabBar_ = new QTabBar(this);
    horiz_layout->addWidget(tabBar_);
    horiz_layout->addStretch();
    contentMapWidget->reparent(this, QPoint(0, 0));
    if (policyAction_) {
        focusPolicyButton_ = new QToolButton(this);
        focusPolicyButton_->setToggleButton(true);
        focusPolicyButton_->setAutoRaise(true);
        focusPolicyButton_->setSizePolicy(QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
        connect(focusPolicyButton_, SIGNAL(toggled(bool)), this,
            SLOT(setStrongFocus(bool)));
        focusPolicyButton_->setOn(policyAction_->getBool(Sui::IS_TOGGLED));
        focusPolicyButton_->setIconSet(
            Sui::icon_provider().getIconSet(policyAction_->get(Sui::ICON)));
        setStrongFocus(focusPolicyButton_->isOn());
        horiz_layout->addWidget(focusPolicyButton_);
    }
    vert_layout->addWidget(contentMapWidget);
    
    for (Csl::Profile* profile = stylesheet_->profiles().firstChild();
         profile; profile = profile->nextSibling()) {
         int tab_index = 
            tabBar_->addTab(Sui::icon_provider().getIconSet(profile->icon()),
                             (!profile->inscription().isEmpty()) 
                             ? Sui::get_translated(
                                 "csl", profile->inscription()) 
                             : Sui::get_translated("csl", "ContentMap"));
         (*profileMap_)[tab_index] = profile;
    }
    connect(tabBar_, SIGNAL(currentChanged(int)),
            this, SLOT(selectProfile(int)));
}

ProfileSelector::~ProfileSelector()
{
}

String ProfileSelector::currentProfile() const
{
    return (*profileMap_)[tabBar_->currentIndex()]->name();
}

void ProfileSelector::selectProfile(int id)
{
    emit profileSelected((*profileMap_)[id]->name());
}

void ProfileSelector::setStrongFocus(bool v)
{
    if (policyAction_) {
        policyAction_->setToggled(v);
        focusPolicyButton_->setToolTip(NOTR("<nobr>") + 
            policyAction_->getTranslated(Sui::TOOLTIP) + 
                NOTR("</nobr>"));
    }
}

void ProfileSelector::languageChanged()
{
    ProfileMap::iterator it = profileMap_->begin();
    for (; it != profileMap_->end(); ++it) {
        tabBar_->setTabText(it->first, 
                    (!it->second->inscription().isEmpty()) 
                     ? Sui::get_translated("csl", it->second->inscription()) 
                     : Sui::get_translated("csl", "ContentMap"));
    }
    if (policyAction_) 
        setStrongFocus(policyAction_->getBool(Sui::IS_TOGGLED));
}

void ContentMap::languageChanged()
{
    if (widget_)
        widget_->languageChanged();
}

///////////////////////////////////////////////////////////////////////////

ContentMap::~ContentMap()
{
    delete contentMapWidget_;
}

#include <qwindowsstyle.h>

QWidget* ContentMap::makeWidget(QWidget* parent, Type)
{
    try {
        init();
    }
    catch (CslException& e) {
        //std::cerr << e.what() << std::endl;
        //msgStream_ << SernaMessages::tocStylesheetEmptyResult
        //           << Message::L_ERROR << tocUri_;
    }
    contentMapWidget_ = new ContentMapWidget(widget_, this);

    ProfileSelector* selector = new ProfileSelector(parent, contentMapWidget_, 
                                                    engine_->stylesheet());
    widget_ = selector;
    contentMapWidget_->selectProfile(selector->currentProfile());
    
    contentMapWidget_->connect(
        widget_, SIGNAL(profileSelected(const Common::String&)), 
        SLOT(selectProfile(const Common::String&)));
    return widget_;
}

///////////////////////////////////////////////////////////////////////////

Instance* make_qt_csl_instance(const InstanceInit& data)
{
    return new ListViewInstance(data);
}

static inline Q3ListViewItem* list_view_item(Instance* instance)
{
    if (!instance)
        return 0;
    return SAFE_CAST(ListViewInstance*, instance)->listItem_.pointer();
}

static InstanceListItem* make_item(Instance* instance, Q3ListViewItem* parent, 
                                   Q3ListViewItem* after)
{
    switch (instance->origin()->nodeType()) {
        case Node::ELEMENT_NODE : 
        case Node::CHOICE_NODE:
            return (after) 
                ? new InstanceListItem(instance, parent, after)
                : new InstanceListItem(instance, parent);
        case Node::PI_NODE:
        case Node::COMMENT_NODE:
            return (after) 
                ? new SpecialElement(instance, parent, after)
                : new SpecialElement(instance, parent);
        case Node::ENTITY_REF_START_NODE:
            return (after) 
                ? new CmSectionCorner(instance, parent, after)
                : new CmSectionCorner(instance, parent);
        default:
            break;
    }
    return (after) 
        ? new InstanceListItem(instance, parent, after)
        : new InstanceListItem(instance, parent);
}


void make_child_items(Instance* instance, Q3ListView* list,
                      Q3ListViewItem* listItem);

static Q3ListViewItem* make_item(Instance* instance, Q3ListView* list, 
                                Q3ListViewItem* parent, Q3ListViewItem* after)
{
    InstanceListItem* item = (parent)
        ? make_item(instance, parent, after)
        : new InstanceListItem(instance, list);
    SAFE_CAST(ListViewInstance*, instance)->listItem_ = item;

    if (instance->isOpen()) {
        make_child_items(instance, list, item);
        item->setOpen(true);
    }
    else {
        item->setExpandable(true);
        item->setOpen(false);
    }
    return item;
}

inline void make_child_items(Instance* instance, Q3ListView* list,
                             Q3ListViewItem* listItem)
{
    if (!instance->firstChild())
        return;
    Q3ListViewItem* after = 0;
    for (Instance* child = instance->firstChild(); child; 
         child = child->nextSibling()) {
        after = make_item(child, list, listItem, after);
        after = after->nextSibling();
    }
}

static Instance* ensure_open(Instance* instance, Instance* child, 
                             Q3ListView* list)
{
    if (!instance)
        return child;
    const Node* child_origin = child->origin();
    if (instance->isStub() || (instance->parent() && 
                               !instance->parent()->isOpen()))
        instance = ensure_open(instance->parent(), instance, list);
    child = csl_instance_origin(child_origin);
    if (instance->isOpen() && !child->isStub())
        return child;
    instance->setOpen(true);
    make_child_items(instance, list, 
        SAFE_CAST(ListViewInstance*, instance)->listItem_.pointer()); 
    return csl_instance_origin(child_origin);
}

static Instance* make_instance(Node* origin, Q3ListView* list)
{
    if (!origin)
        return 0;
    Instance* instance = csl_instance_origin(origin);
    if (!instance) {
        Instance* parent_inst = make_instance(origin->parent(), list);
        instance = csl_instance_origin(origin);
        if (!instance)
            return parent_inst;
    }
    return ensure_open(instance->parent(), instance, list);
}

///////////////////////////////////////////////////////////////////////////

ContentMapWidget::ContentMapWidget(QWidget* parent, ContentMap* contentMap)
    : Q3ListView(parent),
      contentMap_(contentMap),
      highlighted_(0),
      selectedParent_(0),
      buttonState_(Qt::NoButton),
      prevItem_(0),
      dropSerialNumber_(0),
      focusState_(false),
      isStartDrag_(false),
      pendingContextMenu_(false),
      mousePressPos_(QPoint()),
      autoOpenTimer_(this)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
#ifdef __APPLE__
    setStyle(new QWindowsStyle);
#endif // __APPLE__

    addColumn("");
    header()->hide();
    setColumnAlignment(0, Qt::AlignLeft);
    setSorting(-1);
    setFont(QApplication::font());
    
    // setFocusPolicy((FocusPolicy) (StrongFocus & ~ClickFocus));
    
    setMouseTracking(true);
    qApp->installEventFilter(this);
    
    connect(this, SIGNAL(returnPressed(Q3ListViewItem*)),
            this, SLOT(sendSrcPos(Q3ListViewItem*)));
    connect(this, SIGNAL(clicked(Q3ListViewItem*)),
            this, SLOT(itemClicked(Q3ListViewItem*)));
    connect(this, SIGNAL(spacePressed(Q3ListViewItem*)),
            this, SLOT(selectItem(Q3ListViewItem*)));
    connect(this, SIGNAL(selectionChanged(Q3ListViewItem*)),
            this, SLOT(itemSelected(Q3ListViewItem*)));

    connect(&autoOpenTimer_, SIGNAL(timeout()), this, SLOT(openItem()));
    //QToolTip::remove(viewport());
    //toolTip_ = new ContentMapToolTip(this);
    //tipManager_ = qApp->child(NOTR("toolTipManager")); 
}

ContentMapWidget::~ContentMapWidget()
{
    qApp->removeEventFilter(this);
    const Csl::Instance* root = contentMap_->engine()->rootInstance();
    if (root && root->firstChild())
        removeListItems(root->firstChild());
}

void ContentMapWidget::selectProfile(const String& profileName)
{
    const Csl::Instance* root = contentMap_->engine()->rootInstance();
    if (root && root->firstChild())
        removeListItems(root->firstChild());
    try {
        contentMap_->engine()->transform(
            structEditor()->grove()->document(), profileName);
        root = contentMap_->engine()->rootInstance();
        if (root && root->firstChild()) {
            make_item(root->firstChild(), this, 0, 0);
            setCurrentItem(structEditor()->editViewSrcPos());
        }
        contentMap_->engine()->setInstanceWatcher(this);
        GrovePos from, to;
        contentMap_->getSelection(from, to);
        if (!from.isNull() && !to.isNull() && from != to)
            showSelection(from, to);
    }
    catch (CslException& e) {
        structEditor()->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING, tr("ContentMap: Stylesheet Error"), 
            e.what(), tr("&Close"));
    }
}

static const int autoopenTime = 750;

void ContentMapWidget::openItem()
{
    autoOpenTimer_.stop();
    if (currentItem() && !currentItem()->isOpen() ) {
        currentItem()->setOpen( TRUE );
        currentItem()->repaint();
    }
}

void ContentMapWidget::setCurrentItem(const GrovePos& srcPos)
{
    Q3ListViewItem* list_item = listViewItem(srcPos);
    if (list_item) {
        DDBG << "Set current item: ";
        DBG_IF(CM.TEST) 
            dynamic_cast<ContentMapListItem*>(list_item)->instance()->dump();
        setSelected(list_item, true);    
        ensureItemVisible2(list_item);
    }
}

static inline void reassign_state(ContentMapListItem** old_item, 
                                  ContentMapListItem* new_item,
                                  ContentMapListItem::State state)
{
    if (*old_item == new_item) 
        return;
    if (*old_item) 
        (*old_item)->setState(state, false);
    *old_item = new_item;
    if (*old_item) 
        (*old_item)->setState(state, true);
}

void ContentMapWidget::itemSelected(Q3ListViewItem* item)
{
    ContentMapListItem* parent = 
        dynamic_cast<ContentMapListItem*>(item->parent());
    reassign_state(&selectedParent_, parent,
                   ContentMapListItem::SELECTED_PARENT);
    if (!(buttonState_ & Qt::MouseButtonMask))
        ensureSelectionVisible(parent, item, item);
}

void ContentMapWidget::extendSelectionTo(const GrovePos& pos)
{
    GrovePos from, to;
    contentMap_->getSelection(from, to);
    if (!from.isNull() && !to.isNull())
        contentMap_->extendSelection(pos);
    else {
        ContentMapListItem* prev = 
            dynamic_cast<ContentMapListItem*>(prevItem_);
        if (prev)
            contentMap_->setSelection(prev->grovePos(), pos);
    }
}

void ContentMapWidget::selectItem(Q3ListViewItem* item)
{
    if (!item)
        return;
    GrovePos from, to;
    dynamic_cast<ContentMapListItem*>(item)->selection(from, to);
    contentMap_->setSelection(from, to);
}

void ContentMapWidget::sendSrcPos(Q3ListViewItem* item)
{
    if (!item)
        return;
    ContentMapListItem* cm_item = dynamic_cast<ContentMapListItem*>(item);
    DDBG << "CM: sendSrcPos: ";
    DBG_IF(CM.TEST) cm_item->instance()->dump();
    
    GrovePos pos = cm_item->grovePos();
    if (buttonState_ & Qt::ShiftModifier)
        extendSelectionTo(pos);
    else
        contentMap_->sendSrcPos(pos);
}

void ContentMapWidget::itemClicked(Q3ListViewItem* item)
{
    if (!item || pendingContextMenu_) 
        return;
    if (buttonState_ & Qt::ControlModifier)
        selectItem(item);
    else
        sendSrcPos(item);
}

void ContentMapWidget::removeListItems(Instance* instance)
{
    for (Instance* c = instance->firstChild(); c; c = c->nextSibling())
        removeListItems(c);
    if (instance->isStub())
        return;
    ListViewInstance* inst = SAFE_CAST(ListViewInstance*, instance);
    if (!inst->listItem_.isNull() && (
            highlighted_ == inst->listItem_.pointer() ||
            highlighted_ == inst->listItem_->placeholder()))
        highlighted_ = 0;
    if (!inst->listItem_.isNull() && (
            selectedParent_ == inst->listItem_.pointer() ||
            selectedParent_ == inst->listItem_->placeholder()))
        selectedParent_ = 0;
    inst->listItem_.clear();

    DBG(CM.TEST) << "instance item removed\n";
    DBG_IF(CM.TEST) instance->dump();
}

Q3ListViewItem* ContentMapWidget::listViewItem(const GrovePos& srcPos)
{
    if (srcPos.isNull())
        return 0;
    
    GrovePos pos((GrovePos::TEXT_POS == srcPos.type()) 
                 ? GrovePos(srcPos.node()->parent(), srcPos.node()) : srcPos);
    while (pos.before() && Node::TEXT_NODE == pos.before()->nodeType())
        pos = GrovePos(pos.node(), pos.before()->nextSibling());

    Instance* instance = make_instance(pos.node(), this);
    if (!instance) 
        return 0; //! Abnormal situation, might be at least ancestor 

    //! Only ancestor processed, node is filtered
    if (instance->origin() != pos.node())
        return list_view_item(instance);

    Node* after_node = (pos.before())
        ? pos.before()->prevSibling()
        : pos.node()->lastChild();    
    Instance* after = 0;
    while (after_node) {
        if (Node::TEXT_NODE != after_node->nodeType()) {
            if (Node::ENTITY_REF_END_NODE == after_node->nodeType())
                after = csl_instance_origin(
                    static_cast<EntityReferenceEnd*>(
                        after_node)->getSectStart());
            else
                after = csl_instance_origin(after_node);
            if (after) {
                if (!instance->isOpen()) {
                    instance->setOpen(true);    
                    make_child_items(instance, this, 
                                     SAFE_CAST(ListViewInstance*, instance)
                                     ->listItem_.pointer()); 
                    after = csl_instance_origin(after_node);
                }
                break;
            }
        }
        after_node = after_node->prevSibling();
    }        
    if (after) {
        Q3ListViewItem* list_item = list_view_item(after);
        //! Return exact item
        if (list_item) {
            if (Node::ENTITY_REF_START_NODE == after_node->nodeType())
                return list_item;
            return list_item->nextSibling();
        }
    }
    return list_view_item(instance);
}

static void update_selection(Q3ListViewItem* item, Q3ListViewItem const* from, 
                             Q3ListViewItem const* to, bool& isSelection)
{
    if (to == item) 
        isSelection = false;

    dynamic_cast<ContentMapListItem*>(item)->setState(
        ContentMapListItem::SELECTED, isSelection);

    if (from == item && from != to) 
        isSelection = true;

    for (Q3ListViewItem* c = item->firstChild(); c; c = c->nextSibling())
        update_selection(c, from, to, isSelection);
    
}

void ContentMapWidget::ensureItemVisible2(const Q3ListViewItem* item)
{
    if (!item || !item->isVisible())
        return;
    ensureItemVisible(item);
    int y = itemPos(item);
    int h = QMAX((item->parent()) ? item->parent()->height() : 0, 
                 item->height());
    if (isVisible() && y + 3*h > contentsY() + visibleHeight())
        setContentsPos(contentsX(), y - visibleHeight() + 3*h);
    else 
        if (!isVisible() || y - 2*h < contentsY())
            setContentsPos(contentsX(), y - 2*h);
}

void ContentMapWidget::ensureSelectionVisible(const Q3ListViewItem* from,
                                              const Q3ListViewItem* to,
                                              const Q3ListViewItem* cursor)
{
    if (!from || !to || !cursor)
        return;
    
    int y = itemPos(cursor);
    int min_y = QMIN(itemPos(from), y);
    int max_y = QMAX(itemPos(to), y);
    int h = QMAX(QMAX(from->height(), to->height()), cursor->height());
    max_y += h;
    
    if (max_y - min_y >= visibleHeight()) {
        ensureItemVisible(cursor);
        return;
    }
    if (max_y > contentsY() + visibleHeight())
        setContentsPos(contentsX(), max_y - visibleHeight());
    else 
        if (min_y < contentsY())
            setContentsPos(contentsX(), min_y);
}

void ContentMapWidget::showSelection(const GrovePos& from, const GrovePos& to)
{
    if (!firstChild())
        return;
    const Csl::Instance* root = contentMap_->engine()->rootInstance();
    if (root && from.node() == root->origin()) {
        bool is_selection = true;
        update_selection(firstChild(), 0, 0, is_selection);
        return;
    }    
    Q3ListViewItem* from_item = listViewItem(from);
    Q3ListViewItem* to_item = listViewItem(to);

    bool is_selection = false;
    update_selection(firstChild(), from_item, to_item, is_selection);

    ensureSelectionVisible(from_item, to_item, currentItem());
}

void ContentMapWidget::notifyInstanceInserted(Instance* instance)
{
    DBG(CM.DYN) << "inserted: " << instance << std::endl;
    DBG_IF(CM.DYN) instance->dump();
    if (instance->isStub()) {
        if (instance->parent()->isStub()) 
            return;
        Q3ListViewItem* item = 
            SAFE_CAST(ListViewInstance*, 
                      instance->parent())->listItem_.pointer(); 
        if (item) {
            item->setExpandable(true);
            item->setOpen(false);
        }
        return;
    }
    Q3ListViewItem* after = list_view_item(instance->prevSibling());
    make_item(instance, this, list_view_item(instance->parent()), 
              (after) ? after->nextSibling() : 0);
}

void ContentMapWidget::notifyInstanceRemoved(Instance* inst, Instance* child)
{
    DBG(CM.DYN) << "removed: " << child << std::endl;
    DBG_IF(CM.DYN) child->dump();
    removeListItems(child);
    if (inst->isStub())
        return;
    ListViewInstance* instance = SAFE_CAST(ListViewInstance*, inst);
    if (!instance->listItem_.isNull() && !instance->listItem_->firstChild())
        instance->listItem_->setExpandable(false);
}

//// Drag and Drop actions ///////////////////////////////////////////////////

StructEditor* ContentMapWidget::structEditor() const
{
    return contentMap_->structEditor();
}

Sui::MimeHandler& ContentMapWidget::mimeHandler() const
{
    return structEditor()->sernaDoc()->mimeHandler();
}

void ContentMapWidget::startDrag(ContentMapListItem* item)
{
    if (!item || !item->dragEnabled())
        return;
    QDrag* drag = mimeHandler().makeDrag(viewport());
    SernaDragData* d = new SernaDragData(structEditor());
    GrovePos drag_pos = item->grovePos();
    GrovePos from_pos = d->fromPos();
    GrovePos to_pos   = d->toPos();
    DBG(CM.DRAG) << "ContentMap: start drag\n";
    if (from_pos.isNull() || to_pos.isNull()) {
        item->selection(from_pos, to_pos);
        d->setSelection(from_pos, to_pos);
        DBG(CM.DRAG) << "ContentMap: resetting selection\n";
    }
    if (from_pos.isNull() || to_pos.isNull())
        return;
    int width =  QMIN(300, item->width(fontMetrics(), this, 0) + 4);
    
    // paint ancestor-or-self for selection...
    if (d->fromPos().node() && d->toPos().node() &&
        d->fromPos().node() != d->toPos().node()) {
        const Node* node = d->fromPos().node()->commonAos(d->toPos().node());
        if (node) {
            ContentMapListItem* ci = item;
            for (; ci; ci = dynamic_cast<ContentMapListItem*>(ci->parent()))
                if (ci->instance()->origin() == node) {
                    item = ci;
                    break;
                }
        }
    }
    QSize size(width, item->height());
    QPixmap pixmap(size);
    QPainter painter(&pixmap);

    painter.eraseRect(0, 0, size.width(), size.height());
    item->paintCell(&painter, colorGroup(), size.width());
    //painter.drawWinFocusRect(0, 0, size.width(), size.height());

    drag->setPixmap(pixmap);
    d->startDrag(drag, mimeHandler());
}

void ContentMapWidget::contentsDragEnterEvent(QDragEnterEvent* event)
{
    DBG(CM.DRAG) << "ContentMap: dragEnter, event=" << event << std::endl;
    if (SernaDragData::dragEnter(structEditor(), event))
        autoOpenTimer_.start(autoopenTime);
}

void ContentMapWidget::contentsDragLeaveEvent(QDragLeaveEvent*)
{
    autoOpenTimer_.stop();
}

void ContentMapWidget::contentsDragMoveEvent(QDragMoveEvent* event)
{
    autoOpenTimer_.stop();
    SernaDocFragment* sdo = 
        SernaDocFragment::fragment(structEditor()->sernaDoc());
    Q3ListViewItem* i = 
        itemAt(contentsToViewport(((QDragMoveEvent*)event)->pos()));
    if (!sdo || !i) {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
        return;
    }
    setSelected(i, true);
    autoOpenTimer_.start(autoopenTime);
    ContentMapListItem* item = dynamic_cast<ContentMapListItem*>(i);
    sdo->setDropData(item->grovePos(), structEditor());
    mimeHandler().callback(Sui::MimeCallback::VALIDATE_CONTENT, event);
}

void ContentMapWidget::contentsDropEvent(QDropEvent* event)
{
    autoOpenTimer_.stop();
    if (event->isAccepted())
        return;
    DBG(CM.DYN) << "contentsDropEvent " << event << std::endl;
    
    ContentMapListItem* item = dynamic_cast<ContentMapListItem*>(
        itemAt(contentsToViewport(((QDragMoveEvent*)event)->pos())));
    SernaDocFragment* sdo = 
        SernaDocFragment::fragment(structEditor()->sernaDoc());
    if (0 == sdo || 0 == item)
        return event->ignore();
    event->acceptProposedAction();
    sdo->setDropData(item->grovePos(), structEditor());
    mimeHandler().callback(Sui::MimeCallback::DROP_CONTENT, event);
    event->accept();
}

/// Event Handling ///////////////////////////////////////////////////////

void ContentMapWidget::setHighlighted(ContentMapListItem* item)
{
    reassign_state(&highlighted_, item,
                   ContentMapListItem::HIGHLIGHTED);
}

bool ContentMapWidget::event(QEvent* e)
{
    //if (QEvent::ApplicationFontChange == e->type()) {
    //    setFont(QApplication::font());
    //    repaint();
    //    std::cerr << "Font changed\n";
    //}
    return Q3ListView::event(e);
}

void ContentMapWidget::focusStateChanged(bool v)
{
    if (focusState_ == v)
        return;
    focusState_ = v;
    QObject* obj = QWidget::parent();
    if (obj)
        obj = obj->child(NOTR("cm_tab"));
    if (!obj)
        return;
    QWidget* tab_bar = static_cast<QWidget*>(obj);
    Sui::set_widget_focus_color(tab_bar, v);
}

bool ContentMapWidget::eventFilter(QObject* receiver, QEvent* e)
{
    if (receiver == viewport() && e->type() == QEvent::ToolTip) {
        const QHelpEvent* hev = static_cast<const QHelpEvent*>(e);
        Q3ListViewItem* item = itemAt(hev->pos());
        if (0 == item)
            return false;
        QRect r = itemRect(item);
        if (!r.isValid())
            return false;
        QString tooltip = item_cast(item)->tooltip();
        if (!tooltip.isEmpty())
            QToolTip::showText(hev->globalPos(), tooltip, viewport(), r);
        return true;
    }
    switch (e->type()) {
        case QEvent::MouseMove: 
            if (receiver != viewport())
                setHighlighted(0);
            break;
        case QEvent::FocusIn:
            focusStateChanged(this == receiver);
            break;
        case QEvent::FocusOut:
            if (this == receiver)
                focusStateChanged(false);
            break;
        case QEvent::ApplicationFontChange:
            if (font() != QApplication::font()) 
                setFont(QApplication::font());
            break;
        case QEvent::MouseButtonPress: {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            buttonState_ = Qt::ButtonState(ev->state() | ev->button());
            pendingContextMenu_ = false;
            break;
        }
        case QEvent::MouseButtonRelease: {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            buttonState_ = Qt::ButtonState(ev->state() & ~ev->button());
            if (Qt::LeftButton == ev->button()) 
                isStartDrag_ = false;
            if (Qt::RightButton == ev->button())
                pendingContextMenu_ = true;
            break;
        }
        default:
            break;
    }
    if (this == receiver || viewport() == receiver)
        return Q3ListView::eventFilter(receiver, e);
    return false;
}

void ContentMapWidget::keyPressEvent(QKeyEvent* e)
{
    GrovePos from, to;
    if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) 
        contentMap_->getSelection(from, to);

    switch(e->key()) {
        case Qt::Key_Escape:
            contentMap_->removeSelection();
            break;
            
        case Qt::Key_Backspace:
            {
                if (from.isNull() || to.isNull()) {
                    ContentMapListItem* item = 
                        dynamic_cast<ContentMapListItem*>(currentItem());
                    if (!item || 
                        ContentMapListItem::PLACEHOLDER != item->type())
                        return;
                    item = dynamic_cast<ContentMapListItem*>(
                        item->prevSibling());
                    if (!item) 
                        return;
                    dynamic_cast<ContentMapListItem*>(item)->selection(
                        from, to);
                }
                if (!from.isNull() && !to.isNull())
                    contentMap_->cutSelection(from, to);                
            }
            break;
        case Qt::Key_Delete: 
            {
                if (from.isNull() || to.isNull()) {
                    ContentMapListItem* item = 
                        dynamic_cast<ContentMapListItem*>(currentItem());
                    if (item && 
                        ContentMapListItem::PLACEHOLDER == item->type())
                        item = dynamic_cast<ContentMapListItem*>(
                            item->nextSibling());
                    if (!item) 
                        return;
                    dynamic_cast<ContentMapListItem*>(item)->selection(
                        from, to);
                }
                if (!from.isNull() && !to.isNull())
                    contentMap_->cutSelection(from, to);
                break;
            }
        default:
            break;
    };
    prevItem_ = currentItem();
    Q3ListView::keyPressEvent(e);
    Q3ListViewItem* item = currentItem();
    if (item && item != prevItem_ && (e->state() & Qt::ShiftModifier))
        extendSelectionTo(dynamic_cast<ContentMapListItem*>(item)->grovePos());
}

String ContentMapWidget::contextMenuAt(const QPoint& globalPos)
{
    if (!viewport()->geometry().contains(viewport()->mapFromGlobal(globalPos)))
        return String();    
    pendingContextMenu_ = false;
    ContentMapListItem* item = 
        dynamic_cast<ContentMapListItem*>(currentItem());
    if (item) {
        GrovePos from, to;
        contentMap_->getSelection(from, to);
        if (!from.isNull() && !to.isNull())
            return NOTR("selectionContextMenu");
        if (prevItem_ != currentItem())
            contentMap_->sendSrcPos(item->grovePos());
        return item->contextMenuName();
    }
    return String();
}

void ContentMapWidget::contentsMousePressEvent(QMouseEvent* e)
{
    prevItem_ = currentItem();
    Q3ListView::contentsMousePressEvent(e);
    QPoint p(contentsToViewport(e->pos()));
    if (Qt::LeftButton == e->button() && itemAt(p)) {
        mousePressPos_ = e->pos();
        isStartDrag_ = true;
    }
}

void ContentMapWidget::contentsMouseMoveEvent(QMouseEvent* e)
{
    Q3ListView::contentsMouseMoveEvent(e);
    if (Qt::LeftButton & buttonState_ && isStartDrag_ &&
        (mousePressPos_ - e->pos()).manhattanLength() > 
        QApplication::startDragDistance()) {

        isStartDrag_ = false;
        startDrag(dynamic_cast<ContentMapListItem*>(
            itemAt(contentsToViewport(e->pos()))));
        return;
    }
    if (Qt::NoButton != e->state()) {
        setHighlighted(0);
        return;
    }
    ContentMapListItem* item = dynamic_cast<ContentMapListItem*>(
        itemAt(contentsToViewport(e->pos())));
    setHighlighted(item);
}

//////////////////////////////////////////////////////////////////////////

void ListViewInstance::dump() const 
{ 
    DBG_IF(CM.TEST) {
        DDBG << "ListViewInstance: " << this 
                 << " listItem: " << listItem_.pointer() << std::endl;
        if (!listItem_.isNull()) 
            listItem_->dump();
    }
    Instance::dump();
}
    
//////////////////////////////////////////////////////////////////////////

ContentMapListItem::ContentMapListItem(Csl::Instance* instance, 
                                       Q3ListView* listView)
    : Q3ListViewItem(listView),
      instance_(instance),
      state_(NONE),
      isDragging_(false)
{
}

ContentMapListItem::ContentMapListItem(Csl::Instance* instance, 
                                       Q3ListViewItem* parent)
    : Q3ListViewItem(parent),
      instance_(instance),
      state_(NONE),
      isDragging_(false)
{
}

ContentMapListItem::ContentMapListItem(Csl::Instance* instance, 
                                       Q3ListViewItem* parent, 
                                       Q3ListViewItem* after)
    : Q3ListViewItem(parent, after),
      instance_(instance),
      state_(NONE),
      isDragging_(false)
{
}

ContentMapListItem* ContentMapListItem::prevSibling() const
{
    Q3ListViewItem* prev = parent()->firstChild();
    if (prev == this)
        return 0;
    for (Q3ListViewItem* c = prev->nextSibling(); c; c = c->nextSibling()) {
        if (c == this)
            return dynamic_cast<ContentMapListItem*>(prev);
        prev = c;
    }
    return 0;
}

GrovePos ContentMapListItem::grovePos() const
{
    return GrovePos(instance_->origin(), instance_->origin()->firstChild());
}

void ContentMapListItem::selection(GrovePos& from, GrovePos& to) const
{
    from = GrovePos(instance_->origin()->parent(), instance_->origin());
    to = GrovePos(instance_->origin()->parent(), 
                  instance_->origin()->nextSibling());
}

void ContentMapListItem::setState(State state, bool isOn) 
{ 
    int old_state = state_;
    if (isOn)
        state_ |= state;
    else
        state_ &= ~state;
    if (old_state != state_)
        repaint();
}
    
void ContentMapListItem::paintCell(QPainter* p, const QColorGroup& cg,
                                   int column, int width, int)
{
    if (!p || !listView())
        return;

    QBrush brush;
    if (state_ & HIGHLIGHTED) {
        if (state_ & SELECTED) 
            brush = QBrush(QColor("#d6eeff"));
        else
            brush = QBrush(QColor("#d0d0d0"));
    }
    else
        if (state_ & SELECTED) 
            brush = QBrush(QColor("#96cbf2"));
        else
            brush = cg.brush(QColorGroup::Base);

    p->fillRect(0, 0, width, height(), brush);

    if (!isDragging() && isSelected() && (column == 0 || 
                                          listView()->allColumnsShowFocus())) {
        if (state_ & SELECTED)
            brush = QBrush(QColor("#0060a0"));
        else
            brush = cg.brush(QColorGroup::Highlight);
        p->fillRect(0, 0, width, height(), brush);
        if (isEnabled())
            p->setPen(cg.highlightedText());
        else
            p->setPen(listView()->palette().disabled().highlightedText());
    }
    else {
        if (isEnabled())
            p->setPen((textColor_.isValid()) ? textColor_ : cg.text());
        else
            p->setPen(listView()->palette().disabled().text());
    }
}

void ContentMapListItem::paintCell(QPainter* p, const QColorGroup& cg,
                                   int width)
{
    const int old_state = state_;
    state_ = 0;
    isDragging_ = true;
    paintCell(p, cg, 0, width, 0);
    state_ = old_state;
    isDragging_ = false;
}

//////////////////////////////////////////////////////////////////////////

CursorPlaceholder::CursorPlaceholder(Instance* afterInst, 
                                     Q3ListViewItem* parent, 
                                     Q3ListViewItem* after)
    : ContentMapListItem(afterInst, parent, after)
{}
  
String CursorPlaceholder::contextMenuName() const
{
    return NOTR("betweenElementsContextMenu");
}

void CursorPlaceholder::dump() const
{
    DBG_IF(CM.TEST) {
        DDBG << "CursorPlaceholder: after\n";
        dynamic_cast<ListViewInstance*>(instance_)->listItem_->dump();
    }
}

GrovePos CursorPlaceholder::grovePos() const
{
    if (instance_) {
        if (Node::ENTITY_REF_START_NODE == instance_->origin()->nodeType()) {
            Node* ere = static_cast<const EntityReferenceStart*>(
                instance_->origin())->getSectEnd();
            return GrovePos(ere->parent(), ere->nextSibling());
        }
        return GrovePos(instance_->origin()->parent(), 
                        instance_->origin()->nextSibling());
    }
    return GrovePos();
}

void CursorPlaceholder::selection(GrovePos& from, GrovePos& to) const
{
    from = to = GrovePos();
}

QString CursorPlaceholder::tooltip() const
{
    QString tip;
    if (!parent() || nextSibling())
        return tip;
    const Node* parent_node = item_cast(parent())->instance()->origin();
    switch (parent_node->nodeType()) {
        case Node::ELEMENT_NODE:
            tip = tr("end of element:</i> <b>%0</b>").
                arg(parent_node->nodeName());
            break;
        case Node::ENTITY_REF_START_NODE: 
            return special_node_tooltip(CONST_ERS_CAST(parent_node));
        default:
            return QString();
    }
    if (!tip.isEmpty())
        tip = QString(NOTR("<nobr><i>%0</i></nobr>")).arg(tip);
    return tip;
}

void CursorPlaceholder::setup()
{
    Q3ListViewItem::setup();
    setHeight(5);
}

//////////////////////////////////////////////////////////////////////////

InstanceListItem::InstanceListItem(Instance* instance, Q3ListView* listView)
    : ContentMapListItem(instance, listView),
      placeholder_(0)
{
    setDragEnabled(true);
}

InstanceListItem::InstanceListItem(Instance* instance, Q3ListViewItem* parent)
    : ContentMapListItem(instance, parent),
      placeholder_(new CursorPlaceholder(instance, parent, this))
{
    setDragEnabled(true);
}

InstanceListItem::InstanceListItem(Instance* instance, Q3ListViewItem* parent,
                                   Q3ListViewItem* after)
    : ContentMapListItem(instance, parent, after),
      placeholder_(new CursorPlaceholder(instance, parent, this))
{
    setDragEnabled(true);
}

InstanceListItem::~InstanceListItem()
{
    delete placeholder_;
}

String InstanceListItem::contextMenuName() const
{
    return NOTR("elementContextMenu");
}

QString InstanceListItem::tooltip() const
{
    QString tip;
    if (Node::ELEMENT_NODE != instance()->origin()->nodeType())
        return tip;

    const Element* elem = CONST_ELEMENT_CAST(instance()->origin());
    const Attr* attr = elem->attrs().firstChild();
    if (!instance()->text().isEmpty()) {
        tip += NOTR("<nobr>") + Q3StyleSheet::escape(instance()->text()) + NOTR("</nobr>");
        if (attr)
            tip += NOTR("<hr/>");
    }
    for (; attr; attr = attr->nextSibling()) {
        tip += NOTR("<nobr><b>@") + attr->nodeName() + NOTR("</b>='")
            + String(Q3StyleSheet::escape(attr->value())) + "'";
        if (attr->defaulted())
            tip += QString(NOTR(" <i><font color='#009900'>%0</font></i>")).
                arg(tr("(default)"));
        tip += NOTR("</nobr>");
        if (attr->nextSibling())
            tip += ", ";
    }
    if (!tip.isEmpty())
        tip = NOTR("<qt>") + tip + NOTR("</qt>");
    return tip;
}

void InstanceListItem::setOpen(bool isToOpen)
{
    if ((instance_->isOpen() && instance_->firstChild() && !isToOpen) ||
        (!instance_->isOpen() && isToOpen)) {
        if (!isToOpen) {
            for (Instance* c = instance_->firstChild(); c; c= c->nextSibling())
                static_cast<ContentMapWidget*>(
                    listView())->removeListItems(c);
        }
        instance_->setOpen(isToOpen);
        if (isToOpen)
            make_child_items(instance_, listView(), this);
        else
            setExpandable(true);
    }
    Q3ListViewItem::setOpen(isToOpen);
}

int InstanceListItem::width(const QFontMetrics& fm,
                            const Q3ListView* view, int) const
{
    QString txt = 
        instance_->origin()->nodeName() + text().simplifyWhiteSpace();
    int w = int(view->itemMargin()*2 + fm.width(txt)*1.2 + 
                4 + ((height() - 3) / 2));
    return QMAX(w, QApplication::globalStrut().width());
}

void InstanceListItem::dump() const
{
    DBG(CM.TEST) << "InstanceListItem: " << instance_->text() << std::endl;
}

static void collect_text(const Node* node, String& text)
{
    if (text.length() >= 50)
        return;
    if (Node::TEXT_NODE == node->nodeType()) {
        text  += static_cast<const GroveLib::Text*>(node)->data().left(
            50 - text.length()) + ' ';
    }
    for (const Node* c = node->firstChild(); c; c = c->nextSibling())
        collect_text(c, text);
}

String InstanceListItem::text() const
{
    if (isOpen() || !instance_->text().isEmpty())
        return instance_->text();
    String str;
    collect_text(instance_->origin(), str);
    return str;
}

void InstanceListItem::paintCell(QPainter* p, const QColorGroup& cg,
                                 int column, int width, int align)
{
    if (!p || !listView())
        return;

    ContentMapListItem::paintCell(p, cg, column, width, align);

    QString name = instance_->origin()->nodeName();
    const char* CHOICE_NODE = QT_TR_NOOP("#choice");
    if (CHOICE_NODE == name)
        name = tr(name);
    
    int margin = listView()->itemMargin();
    int r = margin;

    //! Setup tag shape
    int hh = (height() - 3) / 2;
    int w = p->fontMetrics().width(name) + 2;
    Q3PointArray shape;
    shape.setPoints(7,
                    r + 0, 1,
                    r + w, 1,
                    r + w + hh, 1 + hh,
                    r + w + hh, height() - 2 - hh,
                    r + w, height() - 2,
                    r + 0, height() - 2,
                    r + 0, 1);

    //! Draw element tag
    QBrush old_brush(p->brush());
    QPen old_pen(p->pen());

    bool is_selected = (isDragging()) ? false : isSelected();

    QColor color((is_selected) ? Qt::darkGray : QColor("#c6f7c0"));
    if (isState(SELECTED_PARENT))
        color = QColor("#3d9b23");
    p->setBrush(QBrush(color));
    p->setPen(QPen((is_selected) ? Qt::white : color.dark(150)));

    p->drawPolygon(shape);
    p->setBrush(old_brush);
    p->setPen(old_pen);

    //! Draw element name
    r += 2;
    if (!name.isEmpty()) {
        if (isState(SELECTED_PARENT))
            p->setPen(QPen(Qt::white));
        if (!(align & Qt::AlignTop || align & Qt::AlignBottom))
            align |= Qt::AlignVCenter;
        p->drawText(r, 0, width - margin - r, height(), align, name);
        p->setPen(old_pen);
    }

    //! Draw collected text
    
    QFont font = p->font();
    const Csl::Template* templ = instance_->selectedTemplate();
    if (NOTR("italic") == templ->fontStyle())
        font.setItalic(true);
    if (NOTR("bold") == templ->fontWeight())
        font.setBold(true);
    if (NOTR("underline") == templ->fontDecoration())
        font.setUnderline(true);
    else
        if (NOTR("overline") == templ->fontDecoration())
            font.setOverline(true);
        else
            if (NOTR("line-through") == templ->fontDecoration())
                font.setStrikeOut(true);
    p->setFont(font);
    if (!instance_->isOpen())
        p->setPen(Qt::gray);

    QString txt = instance_->text().simplifyWhiteSpace();
    r += w + hh;
    if (!txt.isEmpty()) {
        if (!(align & Qt::AlignTop || align & Qt::AlignBottom))
            align |= Qt::AlignVCenter;
        p->drawText(r, 0, width - margin - r, height(), align, txt);
    }
}

//////////////////////////////////////////////////////////////////////////

SpecialElement::SpecialElement(Instance* instance, Q3ListViewItem* parent)
    : InstanceListItem(instance, parent)
{
}

SpecialElement::SpecialElement(Instance* instance, Q3ListViewItem* parent, 
                             Q3ListViewItem* after)
    : InstanceListItem(instance, parent, after)
{
}

QString SpecialElement::tooltip() const
{
    return special_node_tooltip(instance_->origin());
}

String SpecialElement::contextMenuName() const
{
// START_IGNORE_LITERALS
    switch (instance_->origin()->nodeType()) {
        case Node::PI_NODE:
            return "piContextMenu";
            break;
        case Node::COMMENT_NODE:
            return "commentContextMenu";
            break;
        default:
            break;
    }
    return String();
// STOP_IGNORE_LITERALS
}

void SpecialElement::setup()
{
    Q3ListViewItem::setup();
    textColor_ = QColor();
    QString icon_name;
    switch (instance_->origin()->nodeType()) {
        case Node::PI_NODE:
            icon_name = NOTR("pi");
            type_     = PI_ITEM;
            break;
        case Node::COMMENT_NODE:
            icon_name = NOTR("comment");
            type_     = COMMENT_ITEM;
            break;
        default:
            break;
    }
    if (!icon_name.isEmpty())
        icon_ = Sui::icon_provider().getPixmap(icon_name);
    if (!icon_.isNull()) 
        setHeight(icon_.height());
}

String SpecialElement::text() const
{
    switch (instance_->origin()->nodeType()) {
        case Node::PI_NODE: {
            const ProcessingInstruction* pi = 
                CONST_PI_CAST(instance_->origin());
            return pi->target().simplifyWhiteSpace() + ": " + 
                pi->data().simplifyWhiteSpace();
        }
        case Node::COMMENT_NODE:
            return CONST_COMMENT_CAST(
                instance_->origin())->comment().simplifyWhiteSpace();
        default:
            break;
    }
    return String::null();
}

int SpecialElement::width(const QFontMetrics& fm,
                          const Q3ListView* view, int) const
{
    int w = fm.width(text()) + view->itemMargin()*2;
    if (!icon_.isNull()) 
        w += icon_.width() + listView()->itemMargin();

    return QMAX(w, QApplication::globalStrut().width());
}

void SpecialElement::paintCell(QPainter* p, const QColorGroup& cg,
                               int column, int width, int align)
{
    if (!p || !listView())
        return;
    String item_text = text();
    ContentMapListItem::paintCell(p, cg, column, width, align);

    int margin = listView()->itemMargin();
    int r = margin;

    //! Draw icon
    int icon_width = 0;
    if (!icon_.isNull()) {
        icon_width = icon_.width() + listView()->itemMargin();
        if (isState(SELECTED_PARENT)) {
#if defined(_WIN32)
			QImage image = icon_.convertToImage();
			image.invertPixels(false);
			QPixmap icon(image);
            icon.setMask(icon_.mask());
			p->drawPixmap(r, (height() - icon_.height())/2, icon);
#else
            QPixmap icon(icon_);
            QPainter p_(&icon);
            p_.fillRect(0, 0, icon.width(), icon.height(), 
                        QBrush(QColor("#27700f"), Qt::Dense4Pattern));
			p->drawPixmap(r, (height() - icon_.height())/2, icon);
#endif
        }
        else
            p->drawPixmap(r, (height() - icon_.height())/2, icon_);
    }
    r += icon_width;

    //! Draw collected text
    if (!item_text.isEmpty()) {
        QFont font = p->font();
        font.setItalic(true);
        p->setFont(font);
        if (!(align & Qt::AlignTop || align & Qt::AlignBottom))
            align |= Qt::AlignVCenter;
        p->drawText(icon_width, 0, width - margin, height(), align, item_text);
    }
}

//////////////////////////////////////////////////////////////////////////

CmSectionCorner::CmSectionCorner(Instance* instance, Q3ListViewItem* parent)
    : SpecialElement(instance, parent)
{
}

CmSectionCorner::CmSectionCorner(Instance* instance, Q3ListViewItem* parent, 
                             Q3ListViewItem* after)
    : SpecialElement(instance, parent, after)
{
}

QString CmSectionCorner::tooltip() const
{
    return special_node_tooltip(instance_->origin());
}

String CmSectionCorner::contextMenuName() const
{
// START_IGNORE_LITERALS
    EntityDecl* decl = SAFE_CAST(const EntityReferenceStart*, 
                                 instance_->origin())->entityDecl();
    switch (decl->declType()) {
        case EntityDecl::internalGeneralEntity:
        case EntityDecl::externalGeneralEntity:
            return "entityContextMenu";
            break;
        case EntityDecl::xinclude:
            return "xincludeContextMenu";
            break;
        default:
            break;
    }
    return String();
// STOP_IGNORE_LITERALS
}

GrovePos CmSectionCorner::grovePos() const
{
    return GrovePos(instance_->origin()->parent(), 
                    instance_->origin()->nextSibling());
}

void CmSectionCorner::selection(GrovePos& from, GrovePos& to) const
{ 
    const EntityReferenceStart* ers = 
        SAFE_CAST(const EntityReferenceStart*, instance_->origin());
    Node* ere = ers->getSectEnd();
    from = GrovePos(ers->parent(), ers);
    to = GrovePos(ere->parent(), ere->nextSibling());
}

String CmSectionCorner::text() const
{
    const EntityDecl* decl = SAFE_CAST(const EntityReferenceStart*, 
                                 instance_->origin())->entityDecl();
    switch (decl->declType()) {
        case EntityDecl::internalGeneralEntity:
        case EntityDecl::externalGeneralEntity:
            return decl->name();
            
        case EntityDecl::xinclude: {
            const XincludeDecl* xinclude = 
                static_cast<const XincludeDecl*>(decl);
            return xinclude->expr().isEmpty() 
                ? xinclude->url()
                : xinclude->url() + "#" + xinclude->expr();
        }
        default:
            return String::null();
    }
}

void CmSectionCorner::setup()
{
    Q3ListViewItem::setup();

    textColor_ = QColor();
    const EntityDecl* decl = SAFE_CAST(const EntityReferenceStart*, 
                                 instance_->origin())->entityDecl();
    String icon_name;
// START_IGNORE_LITERALS
    switch (decl->declType()) {
        case EntityDecl::internalGeneralEntity:
            icon_name = "internal_entity";
            textColor_ = QColor("#cc8400");
            type_ = ENTITY_ITEM;
            break;
        case EntityDecl::externalGeneralEntity:
            icon_name = "external_entity";
            textColor_ = QColor("#ff5132");
            type_ = ENTITY_ITEM;
            break;
        case EntityDecl::xinclude: 
            icon_name = "xinclude";
            textColor_ = QColor("#007dcc");
            type_ = XINCLUDE_ITEM;
            break;
        default:
            break;
    }
// STOP_IGNORE_LITERALS
    if (!icon_name.isEmpty())
        icon_ = Sui::icon_provider().getPixmap(icon_name);
    if (!icon_.isNull()) 
        setHeight(icon_.height());
}

