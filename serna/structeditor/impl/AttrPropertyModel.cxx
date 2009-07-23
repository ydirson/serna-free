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
#include "structeditor/qt/AttributesWidget.h"
#include "structeditor/qt/AttributeItemDelegate.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/XsUtils.h"

#include "common/ScopeGuard.h"

#include "grove/Nodes.h"
#include "grove/XmlName.h"
#include "grove/ElementMatcher.h"

#include "ui/IconProvider.h"
#include "xs/Schema.h"

#include "utils/tr.h"
#include "utils/ElementHelp.h"
#include "utils/NameChecker.h"

#include <QMessageBox>
#include <QTimer>
#include <QTextDocument>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QApplication>

#include <iostream>
using namespace GroveLib;
using namespace Common;

const int ATTR_NAME_COLUMN  = 0;
const int ATTR_VALUE_COLUMN = 1;
const int ATTR_TYPE_COLUMN  = 2;

static const char* IS_MODIFIED  = NOTR("is-modified");
static const char* OLD_NAME     = NOTR("old-name");

bool is_ns_map(const String& qname)
{
    return qname == NOTR("xmlns") || qname.left(6) == NOTR("xmlns:");
}

/////////////////////////////////////////////////////////////////////////////

AttributeContext::AttributeContext()
    : nsContext_(0),
      patternMatcher_(0),
      helpHandle_(0)
{}

AttributeContext::AttributeContext(const String& elementName, 
                                   const String& topSysid, 
                                   const Node* nsContext)
    : elementName_(elementName),
      topSysid_(topSysid),
      nsContext_(nsContext),
      patternMatcher_(0),
      helpHandle_(0)
{}

AttributeContext::~AttributeContext()
{}

const Element* AttributeContext::element() const
{
    return 0;
}

const Node* AttributeContext::nsContext() const
{
    return (element()) ? element() : nsContext_;
}

String AttributeContext::elementName() const
{
    return (element()) ? element()->name() : elementName_;
}

String AttributeContext::topSysid() const
{
    return (element()) ? element()->grove()->topSysid() : topSysid_;
}

bool AttributeContext::isHiddenAttr(const String& attrName) const
{
    if (!patternMatcher_ || !element())
        return false;
    return patternMatcher_->matchAttr(element(), attrName);
}

PropertyNodePtr AttributeContext::getAttrHelp(const String& attrName) const
{
    String element_name = elementName();
    if (element_name.isEmpty() || !helpHandle_)
        return 0;
    return helpHandle_->attrHelp(element_name, attrName, nsContext());
}

void AttributeContext::init(const ElementMatcher* patternMatcher,
                            const HelpHandle* helpHandle)
{
    patternMatcher_ = patternMatcher;
    helpHandle_ = helpHandle;
}

bool AttributeContext::addAttribute(const String& /*name*/, 
                                    const String& /*value*/)
{
    return true;
}

bool AttributeContext::renameAttribute(const String& /*attrName*/,
                                       const String& /*newName*/,
                                       const String& /*value*/)
{
    return true;
}

bool AttributeContext::changeAttribute(const String& /*attrName*/,
                                       const String& /*newValue*/)
{
    return true;
}

bool AttributeContext::removeAttribute(const String& /*attrName*/)
{
    return true;
}

/////////////////////////////////////////////////////////////////////////////

AttrPropertyModel::AttrPropertyModel(AttributeContext* attrContext)
    : attrContext_(attrContext),
      state_(IDLE),
      view_(0)
{}

void AttrPropertyModel::update(const PropertyNodePtr& attrs,
                               const PropertyNodePtr& specs,
                               const PropertyNodePtr& nsMap)
{
    attrs_ = attrs;
    specs_ = specs;
    nsMap_ = nsMap;
}

void AttrPropertyModel::sort(int column, bool isAscending)
{
    if (attrs_.isNull())
        return;
    
    typedef std::multimap<String, PropertyNodePtr> PropMap;
    PropMap attr_map;

    while (attrs_->firstChild()) {
        PropertyNodePtr attr = attrs_->firstChild();
        String key = (ATTR_NAME_COLUMN == column) 
            ? attr->name() : attr->getString();
        attr_map.insert(PropMap::value_type(key, attr));
        attr->remove();        
    }
    if (isAscending)
        for (PropMap::iterator c = attr_map.begin(); c != attr_map.end(); c++) 
            attrs_->appendChild(c->second.pointer());
    else
        for (PropMap::reverse_iterator c = attr_map.rbegin(); 
             c != attr_map.rend(); c++) 
            attrs_->appendChild(c->second.pointer());
}

bool AttrPropertyModel::isEditing()
{
    if (!view_)
        return false;
    QtTableView* table_view = dynamic_cast<QtTableView*>(&*view_);
    return table_view->isEditing();
}

void AttrPropertyModel::setView(QAbstractItemView* view)
{
    view_ = view;   
    view_->setItemDelegate(new AttributeItemDelegate);
}

QAbstractItemView* AttrPropertyModel::view() const
{
    return view_;
}

bool AttrPropertyModel::isValidName(const Common::String& attrName) const
{
    if (attrName.isEmpty())
        return false;

    QWidget* parent_widget = qApp->activeWindow();
    //! Check existence
    PropertyNode* attr = findByQname(attrs_, attrName);
    if (0 != attr) {
        QMessageBox::critical(
            parent_widget, tr("Attribute already exists"),
            tr("<nobr>Attribute <b>%1</b> "
               "already exists</nobr>").arg(attrName), QMessageBox::Ok);
        return false;
    }

    //! Check validity
    bool is_valid_name = (QString(attrName).startsWith(NOTR("xmlns:")))
        ? NameChecker::isValidLocalName(attrName.mid(6))
        : NameChecker::isValidQname(attrName);
    if (!is_valid_name) {
        QMessageBox::critical(
            parent_widget, tr("Attribute name is not valid"),
            tr("<nobr>Attribute name <b>%1</b> "
               "is not valid</nobr>").arg(attrName), QMessageBox::Ok);
        return false;
    }

    //! Check if hidden
    if (attrContext_->isHiddenAttr(attrName)) {
        QMessageBox::critical(
            parent_widget, tr("Attribute is hidden!"),
            tr("<nobr>Attribute <b>%1</b> is hidden</nobr>").arg(attrName),
            QMessageBox::Ok);
        return false;
    }

    return true;
}

PropertyNode* AttrPropertyModel::findByQname(const PropertyNodePtr& propParent,
                                             const String& qName) const
{
    //! TODO; check default namespace 
    if (propParent.isNull() || qName.isEmpty())
        return 0;

    QualifiedName qname;
    qname.parse(qName);

    const PropertyNode* ns_decl = (!nsMap_.isNull()) 
        ? nsMap_->getProperty(qname.prefix()) : 0;
    if (0 == ns_decl)
        return propParent->getProperty(qName);

    for (PropertyNode* prop = propParent->firstChild(); prop;
         prop = prop->nextSibling()) {
        QualifiedName prop_qname;
        prop_qname.parse(prop->name());
        const PropertyNode* prop_ns_decl =
            nsMap_->getProperty(prop_qname.prefix());
        if (prop_ns_decl && 
            prop_qname.localName() == qname.localName() &&
            prop_ns_decl->getString() == ns_decl->getString()) {
            return prop;
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////

class QtAttrPropertyModel : public QAbstractTableModel, 
                            public AttrPropertyModel {
    Q_OBJECT
public:
    QtAttrPropertyModel(AttributeContext* attrContext)
        : AttrPropertyModel(attrContext),
          isLocked_(false)
        {
        }
    
    virtual void        addAttribute();
    virtual void        removeAttribute();
    virtual void        update(const PropertyNodePtr& attrs, 
                               const PropertyNodePtr& specs, 
                               const PropertyNodePtr& nsMap);
    virtual void        reset();
    virtual void        selectAttribute(const String& attrName);
    

    //! Basic model methods
    QModelIndex         index(int row, int column,
                              const QModelIndex& parent) const;
    virtual int         rowCount(const QModelIndex& parent) const;
    virtual int         columnCount(const QModelIndex& parent) const;
    virtual QVariant    data(const QModelIndex& index, int role) const;
    virtual QVariant    headerData(int section, Qt::Orientation orientation,
                                   int role) const;
    
    //! Editing
    Qt::ItemFlags       flags(const QModelIndex& index) const;
    virtual bool        setData(const QModelIndex& index,
                                const QVariant& value, int role);
    
protected:
    String              attributeHelp(const String& attrName) const;

    bool                isModified(const PropertyNode* attrProp) const;
    bool                isNew(const PropertyNode* attrProp) const;

    String              toolTip(const PropertyNode* attrProp, 
                                const PropertyNode* attrSpec) const;
    bool                addAttribute(const String& name, const String& value);

    bool                setAttrName(const QModelIndex& index, 
                                    PropertyNode* attrProp, 
                                    const String& name);
    bool                setAttrValue(const QModelIndex& index, 
                                     PropertyNode* attrProp,
                                     const String& value);   
signals: 
    void                contextChanged();
    
protected slots: 
    void                editCurrentCell();
    virtual void        sort(int column, Qt::SortOrder order);

protected:
    bool                isLocked_;    
};
    
/////////////////////////////////////////////////////////////////////////////

AttrPropertyModel*
AttrPropertyModel::makeInstance(AttributeContext* attrContext)
{
    return new QtAttrPropertyModel(attrContext);
}

void QtAttrPropertyModel::reset()
{
    ValScopeGuard<bool, bool> lock_guard(isLocked_, true);

    view_->reset();
    state_ = IDLE;
    if (attrs_.isNull())
        return;
    PropertyNodePtr attr = attrs_->firstChild();
    while (attr) {
        PropertyNode* next_attr = attr->nextSibling();
        const int row = attr->siblingIndex();
        if (isNew(attr.pointer())) {
            beginRemoveRows(QModelIndex(), row, row);
            attr->remove();
            endRemoveRows();
        }
        else if (isModified(attr.pointer())) {
            if (attr->getProperty(OLD_NAME))
                attr->setName(attr->getProperty(OLD_NAME)->getString());
            attr->removeAllChildren();
            emit dataChanged(createIndex(row, ATTR_NAME_COLUMN, attr),
                             createIndex(row, ATTR_VALUE_COLUMN, attr));
        }
        attr = next_attr;
    }
}

void QtAttrPropertyModel::sort(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();
    AttrPropertyModel::sort(column, Qt::AscendingOrder == order);
    emit layoutChanged();
}

void QtAttrPropertyModel::selectAttribute(const String& attrName)
{
    PropertyNode* attr_prop = findByQname(attrs_, attrName);
    if (!attr_prop)
        return;
    QModelIndex value_index = 
        createIndex(attr_prop->siblingIndex(), ATTR_NAME_COLUMN, attr_prop);
    view_->setCurrentIndex(value_index);
    QModelIndex sibling_index = 
        createIndex(attr_prop->siblingIndex(), ATTR_VALUE_COLUMN, attr_prop);
    QItemSelection selection = QItemSelection(value_index, sibling_index);
    view_->selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
}

void QtAttrPropertyModel::addAttribute()
{
    if (IDLE != state_ || attrs_.isNull())
        return;
    PropertyNodePtr attr;
    for (attr = attrs_->firstChild(); attr; attr = attr->nextSibling()) {    
        if (isNew(attr.pointer()))
            return;
    }
    //! TODO: do something with modified attribute if any

    //! Notify insertion started
    QModelIndex index = view_->currentIndex();    
    int row = (index.isValid()) 
        ? index.row() + 1 
        : attrs_->countChildren();
    beginInsertRows(QModelIndex(), row, row);

    //! Create attribute property
    attr = new PropertyNode();
    attr->appendChild(new PropertyNode(IS_MODIFIED));

    //! Insert attribute property
    PropertyNode* attr_prop = (index.isValid())
        ? static_cast<PropertyNode*>(index.internalPointer())
        : 0;
    if (attr_prop)  
        attr_prop->insertAfter(attr.pointer());
    else
        attrs_->appendChild(attr.pointer());

    //! Notify insertion ended
    endInsertRows();

    if (view_) {
        QModelIndex name_index = createIndex(
            row, ATTR_NAME_COLUMN, attrs_->getChild(row));
        view_->setCurrentIndex(name_index);
        view_->edit(name_index);
        state_ = EDIT_NAME;
    }
}

void QtAttrPropertyModel::removeAttribute()
{
    QModelIndex index = view_->currentIndex();    
    if (!index.isValid())
        return;

    PropertyNodePtr attr = static_cast<PropertyNode*>(index.internalPointer());
    
    //! Notify removal process
    beginRemoveRows(index.parent(), index.row(), index.row());
    attr->remove();
    endRemoveRows();

    if (!isNew(attr.pointer())) {
        if (isModified(attr.pointer())) {
            //! TODO: use old name
        }
        attrContext_->removeAttribute(attr->name());
    }
    state_ = IDLE;
}

void QtAttrPropertyModel::update(const PropertyNodePtr& attrs, 
                                 const PropertyNodePtr& specs, 
                                 const PropertyNodePtr& nsMap) 
{ 
    DBG_IF(SE.ATTR) {
        attrs->dump();
        specs->dump();
        nsMap->dump();
    }
    
    if (!attrs_.isNull()) {
        //! Notify views about removing attributes
        int child_count = attrs_->countChildren();
        if (child_count) {
            beginRemoveRows(QModelIndex(), 0, child_count - 1);
            attrs_->removeAllChildren();
            endRemoveRows();
        }
    }
    if (!specs_.isNull())
        specs_->removeAllChildren();
    if (!nsMap_.isNull())
        nsMap_->removeAllChildren();

    const int attr_count = (attrs.isNull()) ? 0 : attrs->countChildren();
    //! Notify views about adding attributes
    if (attr_count) 
        beginInsertRows(QModelIndex(), 0, attr_count - 1);
    //! Update the model data
    AttrPropertyModel::update(attrs, specs, nsMap);
    if (attr_count) 
        endInsertRows();

    if (view_) {
        QTableView* table_view = dynamic_cast<QTableView*>(&*view_);
        QHeaderView* header = table_view->horizontalHeader();
        sort(header->sortIndicatorSection(), 
             header->sortIndicatorOrder());
    }
    emit contextChanged();
}

/////////////////////////////////////////////////////////////////////////////

QModelIndex QtAttrPropertyModel::index(int row, int column,  
                                       const QModelIndex& parent) const
{
    return (!attrs_.isNull() && hasIndex(row, column, parent))
        ? createIndex(row, column, attrs_->getChild(row)) 
        : QModelIndex();
}
  
int QtAttrPropertyModel::rowCount(const QModelIndex& parent) const
{
    return (!parent.isValid() && !attrs_.isNull()) 
        ? attrs_->countChildren() 
        : 0;
}

int QtAttrPropertyModel::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant QtAttrPropertyModel::headerData(int column, 
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (orientation == Qt::Horizontal && Qt::DisplayRole == role) {
        if (ATTR_NAME_COLUMN == column)
            return QVariant(QString(tr("Name")));   
        else
            if (ATTR_VALUE_COLUMN == column)
                return QVariant(QString(tr("Value")));   
    }
    return QVariant();
}

static QVariant pixmap_variant(const String& name)
{
    if (name.isEmpty()) 
        return QVariant();
    return qVariantFromValue<QPixmap>(Sui::icon_provider().getPixmap(name));
}

QVariant QtAttrPropertyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const PropertyNode* attr_prop = 
        static_cast<PropertyNode*>(index.internalPointer());
    const PropertyNode* attr_spec = findByQname(specs_, attr_prop->name());

    if (Qt::ToolTipRole == role)
        return QVariant(toolTip(attr_prop, attr_spec));

    switch (index.column()) {
        case ATTR_NAME_COLUMN : {
            switch (role) {
                case AttributeItemDelegate::EDITOR_TYPE_ROLE : {
                    if (specs_ && specs_->firstChild()) 
                        return AttributeItemDelegate::EDIT_COMBO_ROLE;
                    else
                        return AttributeItemDelegate::EDIT_DEFAULT_ROLE;
                    break;
                }
                case AttributeItemDelegate::EDIT_COMBO_ROLE: {
                    if (specs_ && specs_->firstChild()) {
                        QStringList str_list;
                        for (PropertyNode* name = specs_->firstChild();
                             name; name = name->nextSibling()) {
    
                            if (name->name().isEmpty())
                                continue;   
                            if (attrContext_->isHiddenAttr(name->name()))
                                continue;
                            if (findByQname(attrs_, name->name()))
                                continue;
                            str_list << name->name();
                        }
                        return QVariant(str_list);
                    }
                    break;
                }
                case Qt::DisplayRole :
                case Qt::EditRole : {
                    return QVariant(QString(attr_prop->name()));
                    break;
                }
                case Qt::DecorationRole : {
                    if (isModified(attr_prop)) 
                        return pixmap_variant(NOTR("modified_attribute"));
                    if (is_ns_map(attr_prop->name())) 
                        return pixmap_variant(NOTR("XMLNS_attribute_type"));
                    if (attr_spec) {
                        String icon_name;
                        if (attr_spec->getProperty(Xs::DEFAULT_ATTR_VALUE))
                            return pixmap_variant(NOTR("default_attribute"));
                        if (attr_spec->getProperty(Xs::FIXED_ATTR_VALUE))
                            return pixmap_variant(NOTR("fixed_attribute"));
                        if (attr_spec->getProperty(Xs::ATTR_REQUIRED))
                            return pixmap_variant(NOTR("required_attribute"));
                    }
                    break;
                }
            }
            break;
        }
        case ATTR_VALUE_COLUMN : {
            switch (role) {
                case AttributeItemDelegate::EDITOR_TYPE_ROLE : {
                    PropertyNode* value_enum = (attr_spec)
                        ? attr_spec->getProperty(Xs::ATTR_VALUE_ENUM) : 0;
                    if (value_enum && value_enum->firstChild()) 
                        return AttributeItemDelegate::EDIT_COMBO_ROLE;
                    else
                        return AttributeItemDelegate::EDIT_DEFAULT_ROLE;
                    break;
                }
                case AttributeItemDelegate::EDIT_COMBO_ROLE: {
                    PropertyNode* value_enum = (attr_spec)
                        ? attr_spec->getProperty(Xs::ATTR_VALUE_ENUM) : 0;
                    if (value_enum) {
                        QStringList str_list;
                        for (PropertyNode* val = value_enum->firstChild();
                             val; val = val->nextSibling())
                            str_list << val->name();
                        return QVariant(str_list);
                    }
                    break;
                }
                case Qt::DisplayRole :
                case Qt::EditRole : {
                    return QVariant(QString(attr_prop->getString()));
                    break;
                }
                case DEFAULT_VALUE_ROLE : {
                    if (attr_spec) {
                        PropertyNode* default_prop = 
                            attr_spec->getProperty(Xs::DEFAULT_ATTR_VALUE);
                        if (default_prop)
                            return QVariant(default_prop->getString());
                    }
                    break;
                }    
                case Qt::DecorationRole : {
                    if (attr_spec) {
                        PropertyNode* type_prop =
                            attr_spec->getProperty(Xs::ATTR_TYPE);
                        String type = (type_prop) 
                            ? type_prop->getString() : String();
                        QPixmap pixmap = Sui::icon_provider().getPixmap(
                            type + NOTR("_attribute_type"));
                        if (pixmap.isNull() && NOTR("string") != type)
                            pixmap = Sui::icon_provider().getPixmap(
                                NOTR("custom_attribute_type"));
                        return qVariantFromValue<QPixmap>(pixmap);
                    }
                    break;
                }                    
            }
            break;
        }
        default:
            return QVariant();
    };
    return QVariant();
}

Qt::ItemFlags QtAttrPropertyModel::flags(const QModelIndex& index) const 
{ 
    Qt::ItemFlags flags;
    if (!index.isValid())
        return flags;

    const PropertyNode* attr_prop = 
        static_cast<PropertyNode*>(index.internalPointer());
    const PropertyNode* attr_spec = findByQname(specs_, attr_prop->name());

    const bool is_required = (attr_spec)
        ? (attr_spec->getProperty(Xs::DEFAULT_ATTR_VALUE) ||
           attr_spec->getProperty(Xs::FIXED_ATTR_VALUE) ||
           attr_spec->getProperty(Xs::ATTR_REQUIRED))
        : false;

    const bool is_fixed = (attr_spec)
        ? attr_spec->getProperty(Xs::FIXED_ATTR_VALUE) : false;

    switch (index.column()) {
        case ATTR_NAME_COLUMN : {   
            if (!is_required && !is_fixed)
                flags |= Qt::ItemIsEnabled | Qt::ItemIsEditable;
            break;
        }
        case ATTR_VALUE_COLUMN: {
            if (!is_fixed)
                flags |= Qt::ItemIsEnabled | Qt::ItemIsEditable;
            break;
        } 
        default: {
            flags = Qt::ItemIsEnabled;  
            break;
        }
    };
    return flags;
}

bool QtAttrPropertyModel::setAttrName(const QModelIndex& index, 
                                      PropertyNode* attrProp,
                                      const String& name) 
{
    DBG(SE.ATTR) << "AttrModel setAttrName: " << name << std::endl;

    if (isLocked_ || attrProp->name() == name)
        return false;

    if (!isValidName(name)) {
        if (isNew(attrProp)) {
            beginRemoveRows(index.parent(), index.row(), index.row());
            attrProp->remove();
            endRemoveRows();
        }
        state_ = IDLE;
        return false;
    
        //! TODO: uncomment if it is desired to continue editing
        /*
          view_->setCurrentIndex(index);
          view_->edit(index);
          state_ = EDIT_NAME;
        */
    }
    //! New name is valid and differs with current name

    if (!isNew(attrProp))
        attrProp->makeDescendant(OLD_NAME, attrProp->name());
    attrProp->makeDescendant(IS_MODIFIED);
    attrProp->setName(name);
    emit dataChanged(index, index);
        
    //! Start editing value
    QModelIndex value_index = 
        createIndex(index.row(), ATTR_VALUE_COLUMN, attrProp);
    view_->setCurrentIndex(value_index);
    QTimer::singleShot(0, this, SLOT(editCurrentCell()));
    
    state_ = EDIT_VALUE;
    return true;
}

void QtAttrPropertyModel::editCurrentCell()
{
    view_->edit(view_->currentIndex());
}

bool QtAttrPropertyModel::setAttrValue(const QModelIndex& /*index*/, 
                                       PropertyNode* attrProp,
                                       const String& value) 
{
    DBG(SE.ATTR) << "AttrModel setAttrValue: " << std::endl;
    const int row = attrProp->siblingIndex();
    bool ok = false;
    if (isNew(attrProp)) {
        ok = (isLocked_) 
            ? false :  attrContext_->addAttribute(attrProp->name(), value);
        if (!ok) { //! Revert to previous state
            beginRemoveRows(QModelIndex(), row, row);
            attrProp->remove();
            endRemoveRows();
            state_ = IDLE;
            return false;
        }
    }
    else {
        PropertyNode* old_name = attrProp->getProperty(OLD_NAME);
        if (old_name) {
            ok = (isLocked_) 
                ? false 
                : attrContext_->renameAttribute(old_name->getString(),
                                                attrProp->name(), value);
            if (!ok) //! Revert to previous state
                attrProp->setName(old_name->getString());
        }
        else {
            if (attrProp->getString() == value) {
                attrProp->removeAllChildren();
                state_ = IDLE;
                return false;
            }
            ok = (isLocked_) 
                ? false 
                : attrContext_->changeAttribute(attrProp->name(), value);
        }
    }
    
    if (ok)
        attrProp->setString(value);
    attrProp->removeAllChildren();
    state_ = IDLE;
    
    //Note: removed to avoid crash 
    //Q:I don`t remember what crash.
    emit dataChanged(createIndex(row, ATTR_NAME_COLUMN, attrProp),
                     createIndex(row, ATTR_VALUE_COLUMN, attrProp));
    return ok;
}

bool QtAttrPropertyModel::setData(const QModelIndex& index,
                                  const QVariant& value, int role)
{
    if (!index.isValid() || Qt::EditRole != role)
        return false;

    DBG(SE.ATTR) << "AttrModel setData: [" << String(value.toString()) 
                 << "]" << std::endl;
    
    PropertyNode* attr_prop =
        static_cast<PropertyNode*>(index.internalPointer());
    String str_value = value.toString();


    switch (index.column()) {
        case ATTR_NAME_COLUMN : {
            if (IDLE == state_ || EDIT_NAME == state_) 
                return setAttrName(index, attr_prop, str_value);
            else 
                //! TODO: this should not happen
                return false;

            break;
        }
        case ATTR_VALUE_COLUMN : {
            if (IDLE == state_ || EDIT_VALUE == state_) 
                return setAttrValue(index, attr_prop, str_value);
            else
                //! TODO: this should not happen
                return false;
        }
        default:
            break;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

String QtAttrPropertyModel::attributeHelp(const String& attrName) const
{
    PropertyNodePtr help_prop = attrContext_->getAttrHelp(attrName);
    if (help_prop.isNull())
        return String();    
    return help_prop->getSafeProperty(HelpHandle::SHORT_HELP)->getString();
}

static inline String attr_type(const String& attrName,
                               const PropertyNode* attrSpec)
{
    if (QString(attrName).startsWith(NOTR("xmlns:")))
        return tr("XML namespace mapping");

    PropertyNode* type_prop = (attrSpec) 
        ? attrSpec->getProperty(Xs::ATTR_TYPE) : 0;
    return (type_prop) ? type_prop->getString() : String(tr("string"));
}

bool QtAttrPropertyModel::isModified(const PropertyNode* attrProp) const 
{
    return attrProp->getProperty(IS_MODIFIED);
}

bool QtAttrPropertyModel::isNew(const PropertyNode* attrProp) const 
{
    return (isModified(attrProp) && !attrProp->getProperty(OLD_NAME));
}

String QtAttrPropertyModel::toolTip(const PropertyNode* attrProp, 
                                    const PropertyNode* attrSpec) const
{
    String note;
    if (attrSpec) {
        if (attrSpec->getProperty(Xs::DEFAULT_ATTR_VALUE))
            note = tr("has default value");
        if (attrSpec->getProperty(Xs::FIXED_ATTR_VALUE))
            note = tr("has fixed value");
        if (attrSpec->getProperty(Xs::ATTR_REQUIRED))
            note = tr("required attribute");
    }
    if (!note.isEmpty())
        note = tr("<br/><nobr>Note: %1</nobr>").arg(note);

    String attr_help = attributeHelp(attrProp->name());
    if (!attr_help.isEmpty())
        attr_help.prepend(NOTR("<br/><hr/>"));
        
    return tr("<qt><nobr>Name: <b>%1"
              "</b></nobr><br/><nobr>Value: <b>%2"
              "</b></nobr><br/><font color='#990000'>"
              "<nobr>Type: %3</nobr>%4 %5</font>%6</qt>").
        arg(attrProp->name()).
        arg(Qt::escape(attrProp->getString())).
        arg(attr_type(attrProp->name(), attrSpec)).
        arg(note).
        arg((isModified(attrProp))
            ? tr("<br/><nobr>Attribute is NOT set!</nobr>") : QString()).
        arg(attr_help);
}

#include "moc/AttrPropertyModel.moc"
