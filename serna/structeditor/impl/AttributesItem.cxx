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
#include "structeditor/AttributesItem.h"

#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "structeditor/GroveCommandEventData.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/XsUtils.h"
#include "editableview/EditableView.h"

#include "docview/dv_utils.h"
#include "docview/SernaDoc.h"

#include "utils/Properties.h"
#include "utils/GrovePosEventData.h"
#include "utils/ElementHelp.h"
#include "utils/IdleHandler.h"
#include "utils/DocSrcInfo.h"

#include "ui/IconProvider.h"
#include "ui/UiItemSearch.h"
#include "ui/TreeModel.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "grove/Nodes.h"
#include "grove/XmlNs.h"
#include "grove/XmlName.h"
#include "grove/NodeVisitor.h"
#include "grove/ElementMatcher.h"

#include "xs/XsNodeExt.h"
#include "xs/Schema.h"

#include "common/ScopeGuard.h"

#include <set>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace AttributesSpace;

//////////////////////////////////////////////////////////////////////////

static inline String ns_name(const String& qname)
{
    if (qname.left(6) == NOTR("xmlns:"))
        return qname.mid(6);
    if (qname == NOTR("xmlns"))
        return XmlNs::defaultNs();
    return String();
}

/* TODO: check if necessary when operating with element 
static inline bool is_attr_exists(const String& qname, Element* elem,
                                  Attr*& attr)
{
    if (is_ns_map(qname)) {
        attr = 0;
        return !elem->lookupPrefixMap(qname.mid(6)).isNull();
    }
    attr = elem->attrs().getAttribute(qname);
    return 0 != attr;
}
*/

//////////////////////////////////////////////////////////////////////////

class ElementContext : public AttributeContext,
                       public CommandEventContext,
                       public NodeVisitor,
                       public IdleHandler {
public:
    ElementContext();
    virtual ~ElementContext() {}
    
    AttrPropertyModel*  attrModel() const { return model_.pointer(); }

    StructEditor*   structEditor() const { return structEditor_; }
    void            setStructEditor(StructEditor* se);
    void            notifyPosChanged(const GrovePos&);
    void            selectAttribute(const String& attrName);

public:
    virtual bool    addAttribute(const String& name, const String& value);
    virtual bool    renameAttribute(const String& attrName,
                                    const String& newName,
                                    const String& value);
    virtual bool    changeAttribute(const String& attrName,
                                    const String& newValue);
    virtual bool    removeAttribute(const String& attrName);

protected:
    virtual const Element*  element() const { return element_; }

    //! Processes context change events from editor
    virtual bool    processQuanta();
    void            updateModel();
    void            notifyAttrsChanged();
    
    //! NodeVisitor notifications
    virtual void    attributeChanged(const Attr*);
    virtual void    attributeRemoved(const Element*, const Attr*);
    virtual void    attributeAdded(const Attr*);
    virtual void    nsMappingChanged(const NodeWithNamespace*, const String&);

    //! don't care about other notifications
    virtual void    nodeDestroyed(const Node*) {};
    virtual void    childInserted(const Node*) {};
    virtual void    childRemoved (const Node*, const Node*) {};
    virtual void    nameChanged(const Node*) {};
    virtual void    textChanged(const Text*) {};

protected:
    OwnerPtr<AttrPropertyModel> model_;
    StructEditor*               structEditor_;
    GrovePos                    pendingPos_;
    GroveLib::Element*          element_;
    bool                        lockNotifications_;
};

//////////////////////////////////////////////////////////////////////////

GROVEPOS_EVENT_IMPL(AttributesContext, ElementContext);

bool AttributesContext::doExecute(ElementContext* elementContext, EventData*)
{
    elementContext->notifyPosChanged(pos_);
    return true;
}

//////////////////////////////////////////////////////////////////////////
ElementContext::ElementContext()
    : model_(AttrPropertyModel::makeInstance(this)),
      structEditor_(0),
      element_(0),
      lockNotifications_(false)
{
}

void ElementContext::setStructEditor(StructEditor* se)
{    
    if (se == structEditor_)
        return;
    if (structEditor_) {
        unsubscribeFromAll();
        //! TODO: unsubscribe AttributesContext
    }

    structEditor_ = se;
    GrovePos pos;

    if (structEditor_) {
        if (patternMatcher_.isNull())
            patternMatcher_ = new ElementMatcher(
                structEditor_->getDsi()->
                getSafeProperty(DocSrcInfo::HIDE_ATTRIBUTES)->getString());
        helpHandle_ = structEditor_->helpHandle();
        
        structEditor_->getCheckedPos(
            pos, StructEditor::SILENT_OP | StructEditor::STRUCT_OP);
        structEditor_->elementContextChange().subscribe(
            makeCommand<AttributesContext>, this);
    }

    //! NOTE: model update will be made in idle time
    notifyPosChanged(pos);
}

void ElementContext::selectAttribute(const String& attrName)
{
    if (!pendingPos_.isNull())
        processQuanta();
    model_->selectAttribute(attrName);
}

void ElementContext::notifyPosChanged(const GrovePos& pos)
{
    //! We need no more notifications untill processQuanta
    deregisterFromAllNodes();

    pendingPos_ = pos;
    IdleHandler::registerHandler(this);
}

bool ElementContext::processQuanta()
{
    IdleHandler::deregisterHandler(this);
    if (!model_->view())
        return false;
    element_ = traverse_to_element(pendingPos_);
    updateModel();
    pendingPos_ = GrovePos();

    if (element_)
        element_->registerNodeVisitor(this);
    return false;
}

void ElementContext::updateModel() 
{ 
    PropertyNodePtr attrs = new PropertyNode(NOTR("attrs"));
    PropertyNodePtr specs = new PropertyNode(NOTR("specs"));
    PropertyNodePtr ns_map = new PropertyNode(NOTR("ns-map"));
    
    if (!element_) 
        return model_->update(attrs, specs, ns_map);
    
    //! Collect existing element attributes
    for (Attr* a = element_->attrs().firstChild(); a; a = a->nextSibling()) {
        if (!isHiddenAttr(a->nodeName())) {
            PropertyNode* attr_prop =
                new PropertyNode(a->nodeName(), a->value());
            attrs->appendChild(attr_prop);
        }
    }

    //! Get attribute specifications from schema
    get_schema_attributes(element_, specs.pointer());
    PropertyNode* attr_spec = specs->firstChild();
    while (attr_spec) {
        if (isHiddenAttr(attr_spec->name())) {
            PropertyNode* next_spec = attr_spec->nextSibling();
            attr_spec->remove();
            attr_spec = next_spec;
        }
        else 
            attr_spec = attr_spec->nextSibling();
    }

    //! Collect xmlns for showing them like existing attributes
    for (const XmlNsMapItem* xmi = element_->nsMapList().firstChild(); 
         xmi; xmi = xmi->nextSibling()) {

        if (xmi->prefix() == NOTR("xml") || xmi->isDefaulted())
            continue;
        String name = (xmi->prefix().isEmpty())
            ? NOTR("xmlns") : NOTR("xmlns:") + xmi->prefix();
        if (name == NOTR("xmlns") && xmi->uri().isEmpty()) {
            if (element_->parent()->nodeType() != GroveLib::Node::ELEMENT_NODE)
                continue;
            const Node* node = element_->prevSibling();
            if (node && node->nodeType() == Node::ENTITY_REF_START_NODE) {
                const EntityReferenceStart* ers = CONST_ERS_CAST(node);
                if (ers->entityDecl() &&
                    ers->entityDecl()->declType() == EntityDecl::xinclude) 
                    continue;
            }
        }
        if (!isHiddenAttr(name)) 
            attrs->appendChild(new PropertyNode(name, xmi->uri()));
    }

    //! Collect NS mappings for mapping purposes
    ns_map->removeAllChildren();
    std::set<String> prefix_set;
    for (const GroveLib::Node* cn = element_; cn && cn->parent(); 
         cn = cn->parent()) {
        if (cn->nodeType() != GroveLib::Node::ELEMENT_NODE)
            break;
        const Element* e = static_cast<const Element*>(cn);
        for (const XmlNsMapItem* xmi = e->nsMapList().firstChild(); 
             xmi; xmi = xmi->nextSibling()) {

            if (prefix_set.find(xmi->prefix()) != prefix_set.end())
                continue;
            prefix_set.insert(xmi->prefix());
            ns_map->makeDescendant(xmi->prefix(), xmi->uri());
        }
    }

    //! Update model
    model_->update(attrs, specs, ns_map);
}
    
/////////////////////////////////////////////////////////////////////////////

class ExecuteAndUpdate;

bool ElementContext::addAttribute(const String& name, const String& value)
{
    ValScopeGuard<bool, bool> lock_guard(lockNotifications_, true);

    PropertyNode attr_prop(name, value);
    DBG(SE.ATTR) <<"Attribute added to model: [" 
                 << name << "]=[" << value << "]" << std::endl;
    DBG_IF(SE.ATTR) attr_prop.dump();

    Editor* editor = structEditor_->groveEditor();
    GroveCommandEventData gcmd(
        (is_ns_map(name)) 
        ? editor->mapXmlNs(element_, ns_name(name), value)
        : editor->addAttribute(element_, &attr_prop));
    bool ok = makeCommand<ExecuteAndUpdate>(&gcmd)->execute(structEditor_);
    return ok;
}

bool ElementContext::renameAttribute(const String& oldName,
                                     const String& newName, 
                                     const String& value)
{
    ValScopeGuard<bool, bool> lock_guard(lockNotifications_, true);

    DBG(SE.ATTR) << "Attribute name changed in model: [" 
                 << oldName << "] -> [" << newName << "]=[" 
                 << value << "]" << std::endl;
    
    Editor* editor = structEditor_->groveEditor();
    CommandPtr cmd;

    if (is_ns_map(oldName) != is_ns_map(newName)) {
        if (!removeAttribute(oldName))
            return false;
        if (!addAttribute(newName, value))
            return false;
        return true;
    }
    if (is_ns_map(oldName))
        cmd = editor->mapXmlNs(element_, ns_name(newName), value);
    else {
        Attr* attr = element_->attrs().getAttribute(oldName);
        PropertyNode attr_prop(newName, value);
        cmd = editor->renameAttribute(attr, &attr_prop);
    }
    GroveCommandEventData gcmd(cmd);
    return makeCommand<ExecuteAndUpdate>(&gcmd)->execute(structEditor_);
}

bool ElementContext::changeAttribute(const String& name,
                                     const String& newValue)
{
    ValScopeGuard<bool, bool> lock_guard(lockNotifications_, true);

    DBG(SE.ATTR) << "Attribute value or ns mapping changed in model: ["
                 << name << "]=[" << newValue << "]" << std::endl;
    
    Editor* editor = structEditor_->groveEditor();
    CommandPtr cmd;
    if (is_ns_map(name))
        cmd = editor->mapXmlNs(element_, ns_name(name), newValue);
    else {
        Attr* attr = element_->attrs().getAttribute(name);
        cmd = editor->setAttribute(attr, newValue);
    }
    GroveCommandEventData gcmd(cmd);
    return makeCommand<ExecuteAndUpdate>(&gcmd)->execute(structEditor_);
}

bool ElementContext::removeAttribute(const String& attrName)
{
    ValScopeGuard<bool, bool> lock_guard(lockNotifications_, true);

    DBG(SE.ATTR) << "attribute removed: " << attrName << std::endl;
    Editor* editor = structEditor_->groveEditor();
    
    CommandPtr cmd;
    if (is_ns_map(attrName)) 
        cmd = editor->mapXmlNs(element_, ns_name(attrName), String());
    else {
        Attr* attr = element_->attrs().getAttribute(attrName);
        cmd = editor->removeAttribute(attr);
    }
    GroveCommandEventData gcmd(cmd);
    bool ok =  makeCommand<ExecuteAndUpdate>(&gcmd)->execute(structEditor_);
    return ok;
}

//! Element dynamics notifications /////////////////////////////////////////

void ElementContext::notifyAttrsChanged()
{
    if (lockNotifications_)
        return;
    
    //!NOTE: now we do simplest thing for all changes - recreate model data
    if (!structEditor_)
        return;
    GrovePos pos;
    structEditor_->getCheckedPos(
        pos, StructEditor::SILENT_OP | StructEditor::STRUCT_OP);
    notifyPosChanged(pos);
}

void ElementContext::attributeChanged(const Attr* attr)
{
    DBG(SE.ATTR) << "Attribute value changed in element: " 
                 << attr->name() << std::endl;
    if (isHiddenAttr(attr->name()))
        return;
    notifyAttrsChanged();

    /* TODO: use following code when necessary to update certain index
    PropertyNode* attr_prop = attrs_->makeDescendant(attr->name());
    attr_prop->setString(attr->value());

    QModelIndex index = createIndex(
        attr_prop->siblingIndex(), ATTR_VALUE_COLUMN, attr_prop);
    emit dataChanged(index, index);
    */
}

void ElementContext::attributeRemoved(const Element* element,
                                        const Attr* attr)
{
    if (element != element_)
        return;
    DBG(SE.ATTR) << "Attribute removed from element: " 
                 << attr->name() << std::endl;
    notifyAttrsChanged();

    /* TODO: use following code when necessary to update certain index
    PropertyNode* attr_prop = attrs_->getProperty(attr->name());
    if (!attr_prop) 
        return;

    QModelIndex index;
    int c = attr_prop->siblingIndex();
    beginRemoveRows(index, c, c);
    attr_prop->remove();
    endRemoveRows();
    */
}

void ElementContext::attributeAdded(const Attr* attr)
{
    if (isHiddenAttr(attr->name()))
        return;
    DBG(SE.ATTR) << "Attribute added to element: "  
                 << attr->name() << " = <" 
                 << attr->value() << ">" << std::endl;
    notifyAttrsChanged();

    /* TODO: use following code when necessary to update certain index
    const bool is_new = !attrs_->getProperty(attr->name());
    PropertyNode* attr_prop = attrs_->makeDescendant(attr->name());
    attr_prop->setString(attr->value());
    if (is_new) {
        const int row = attr_prop->siblingIndex();
        beginInsertRows(QModelIndex(), row, row);
        endInsertRows();
    }
    else {
        PropertyNode* is_modified = attr_prop->getProperty(IS_MODIFIED);
        if (is_modified)
            is_modified->remove();
        //! TODO: notify about changes in all columns
        QModelIndex index = createIndex(
            attr_prop->siblingIndex(), ATTR_NAME_COLUMN, attr_prop);
        emit dataChanged(index, index);
    }
    */
}

void ElementContext::nsMappingChanged(const NodeWithNamespace*,
                                      const String& prefix)
{
    //! TODO: test this method
    String name = (prefix.isEmpty()) ? NOTR("xmlns") : NOTR("xmlns:") + prefix;
    if (isHiddenAttr(name) || !element_)
        return;

    notifyAttrsChanged();

    /* TODO: use following code when necessary to update certain index
    String uri = element_->lookupPrefixMap(prefix);
    PropertyNode* attr_prop = attrs_->getProperty(name);
    //! Ns mapping removed
    if (uri.isNull()) {
        if (attr_prop) {
            const int row = attr_prop->siblingIndex();
            beginRemoveRows(QModelIndex(), row, row);
            attr_prop->remove();
            endRemoveRows();
        }
        return;
    }
    //! Ns mapping changed
    if (attr_prop) {
        attr_prop->setString(uri);
        QModelIndex index = createIndex(
            attr_prop->siblingIndex(), ATTR_VALUE_COLUMN, attr_prop);
        emit dataChanged(index, index);
    }
    //! Ns mapping added
    else {
        attr_prop = attrs_->makeDescendant(name);
        attr_prop->setString(uri);

        const int row = attr_prop->siblingIndex();
        beginInsertRows(QModelIndex(), row, row);
        endInsertRows();
    }
    */
}

//////////////////////////////////////////////////////////////////////////

namespace Sui {
    COMMANDLESS_ITEM_MAKER(AttributeItem, AttributeItem)
}

AttributeItem::AttributeItem(PropertyNode* props)
    : LiquidItem(props),
      elementContext_(new ElementContext)
{
    itemProps()->makeDescendant("caption")->
        setString(tr("Element Attributes"));
}

AttributeItem::~AttributeItem()
{
}

StructEditor* AttributeItem::structEditor() const 
{ 
    return elementContext_->structEditor(); 
}

AttrPropertyModel* AttributeItem::attrModel() const
{
    return elementContext_->attrModel();
}

AttributeItem* AttributeItem::findInstance(const Sui::Item* from)
{
    return dynamic_cast<AttributeItem*>(
        from->findItem(Sui::ItemClass(Sui::CHANGE_ATTRIBUTES)));
}

void AttributeItem::inserted()
{
    Sui::Item* doc = documentItem();
    if (0 == doc)
        return;
    elementContext_->setStructEditor(
        dynamic_cast<StructEditor*>(
            doc->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR))));
}

void AttributeItem::removed()
{
    elementContext_->setStructEditor(0);
}

void AttributeItem::escapePressed()
{
    if (attrModel()->isEditing()) 
        attrModel()->reset();
    LiquidItem::escapePressed();
}

void AttributeItem::visibilityChanged(bool isVisible)
{
    if (!isVisible)
        attrModel()->reset();
}

void AttributeItem::showAttribute(const String& attrName)
{
    elementContext_->selectAttribute(attrName);
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(Attributes, StructEditor)

bool Attributes::doExecute(StructEditor* se, EventData*)
{
    Sui::Item* doc_item = se->sernaDoc();
    AttributeItem* item = AttributeItem::findInstance(doc_item);
    if (item) {
        item->grabFocus();
        return true;
    }
    PropertyNode* item_props = new PropertyNode(Sui::ITEM_PROPS);
    item_props->makeDescendant(Sui::NAME, Sui::CHANGE_ATTRIBUTES);
    Sui::LiquidItem* attr_item = new AttributeItem(item_props);
    doc_item->appendChild(attr_item);
    attr_item->buildChildren(&restore_ui_item);
    attr_item->attach();
    attr_item->grabFocus();
    return true;
}

PROPTREE_EVENT_IMPL(ShowElementAttribute, StructEditor)
REGISTER_COMMAND_EVENT_MAKER(ShowElementAttribute, "PropertyTree", "-")

bool ShowElementAttribute::doExecute(StructEditor* se, EventData*)
{
    makeCommand<Attributes>()->execute(se);
    AttributeItem* attr_editor =
        AttributeItem::findInstance(se->documentItem());
    if (attr_editor) {
        if (ed_->firstChild())
            attr_editor->showAttribute(ed_->firstChild()->name());
        return true;
    }
    return false;
}
