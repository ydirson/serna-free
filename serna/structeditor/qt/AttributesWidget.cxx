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

#include "common/Url.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTreeEventData.h"

#include "structeditor/StructEditor.h"
#include "structeditor/AttributesItem.h"
#include "structeditor/AttrPropertyModel.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/impl/entity_utils.h"

#include "structeditor/AttributesToolBase.hpp"
#include "structeditor/AttributesDialogBase.hpp"
#include "structeditor/AttributesModalDialogBase.hpp"

#include "xs/Schema.h"
#include "grove/Grove.h"
#include "groveeditor/GrovePos.h"

#include "docview/SernaDoc.h"
#include "docview/qt/BrowseButtonMenu.h"
#include "docutils/doctags.h"

#include "utils/HelpAssistant.h"
#include "utils/ElementHelp.h"
#include "utils/DocSrcInfo.h"

#include "ui/IconProvider.h"

#include <QDialog>
#include <QPointer>
#include <QHeaderView>
#include <iostream>

using namespace Common;
using namespace GroveLib;
using namespace AttributesSpace;

QWidget* QtTableView::editorWidget()
{
    QWidget* obj = findChild<QWidget*>(NOTR("AttributeItemEditor"));
    return obj;
}

////////////////////////////////////////////////////////////////////////////

class AttributeEditor {
public:
    AttributeEditor(StructEditor* se, AttrPropertyModel* attrModel, 
                    PropertyNode* props)
        : se_(se),
          attrModel_(attrModel),
          props_(props) {}
    virtual ~AttributeEditor() {}

protected:
    void                setup(QToolButton* browseButton,
                              QTableView* attributeTableView);

    void                setupCaption();
    virtual void        setCaption(const String& text) = 0;

    void                showElementHelp(const QModelIndex& index);

    void                setBrowseBaseUrl();
    void                selectUrl(const QModelIndex& currentIndex,
                                  const String& url);

    QAbstractItemModel* qModel() const { 
        return dynamic_cast<QAbstractItemModel*>(attrModel_);
    }

protected:  
    StructEditor*               se_;
    AttrPropertyModel*          attrModel_;
    PropertyNodePtr             props_;
    QPointer<BrowseButtonMenu>  attrBrowseMenu_;
};

void AttributeEditor::setup(QToolButton* browseButton,
                            QTableView* attributeTableView)
{
    browseButton->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("browse")));

    attrModel_->setView(attributeTableView);
    attributeTableView->setModel(qModel());
    attributeTableView->verticalHeader()->setShown(false);    
    QHeaderView* header = attributeTableView->horizontalHeader();
    header->setResizeMode(0, QHeaderView::Interactive);    
    header->setResizeMode(1, QHeaderView::Stretch);    
    header->setSortIndicatorShown(true);    
    header->setSortIndicator(0, Qt::AscendingOrder);    
    
    attrModel_->sort(header->sortIndicatorSection(), 
                     header->sortIndicatorOrder());

    QObject* obj = dynamic_cast<QObject*>(this);
    obj->connect(attributeTableView->selectionModel(), 
                 SIGNAL(currentChanged(const QModelIndex&, 
                                       const QModelIndex&)),
                 obj, 
                 SLOT(currentChanged(const QModelIndex&,const QModelIndex&)));

    obj->connect(header, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
                 qModel(), SLOT(sort(int, Qt::SortOrder)));
    obj->connect(qModel(), 
                 SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                 obj, 
                 SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
    obj->connect(qModel(), SIGNAL(contextChanged()),
                 obj, SLOT(contextChanged()));
    if (browseButton) {   
        browseButton->setPopupMode(QToolButton::InstantPopup);
        attrBrowseMenu_ = new BrowseButtonMenu(se_->sernaDoc(), browseButton,
                                               DocSrcInfo::myDocumentsPath());
        attrBrowseMenu_->setCaption(
            tr("Choose a file to set attribute value"));
        attrBrowseMenu_->setFilters(tr("All files (*)"));

        obj->connect(attrBrowseMenu_, SIGNAL(browseStarted()),
                     obj, SLOT(browseStarted()));
        obj->connect(attrBrowseMenu_, 
                     SIGNAL(urlSelected(const Common::String&)),
                     obj, SLOT(selectUrl(const Common::String&)));
    }

    attributeTableView->setCurrentIndex(qModel()->index(0, 0));
    setupCaption();
}

void AttributeEditor::setupCaption()
{
    String name = attrModel_->attributeContext()->elementName();
    String caption = (!name.isEmpty())
        ? tr("Attributes of <%1>").arg(name) : tr("Element Attributes");
    setCaption(caption);
}

void AttributeEditor::showElementHelp(const QModelIndex& currentIndex)
{
    const int row = currentIndex.row();
    QModelIndex index = currentIndex.sibling(row, ATTR_NAME_COLUMN);
    const AttributeContext* attr_context = attrModel_->attributeContext();

    String attr_name = index.data().toString();
    String element_name = attr_context->elementName();

    PropertyNodePtr attr_prop = attr_context->getAttrHelp(attr_name);

    helpAssistant().showLongHelp(
        attr_prop.pointer(), element_name + "/@" + attr_name);
}

void AttributeEditor::setBrowseBaseUrl()
{
    String base_url = props_->getSafeProperty("last-url")->getString();
    if (base_url.isEmpty() && se_) {
        base_url = String(get_current_entity_url(se_));
    }
    if (base_url.isEmpty())
        base_url = DocSrcInfo::myDocumentsPath();
    attrBrowseMenu_->setBaseUrl(base_url);
}

void AttributeEditor::selectUrl(const QModelIndex& currentIndex,
                                const String& url)
{
    PropertyNode* base_url = props_->getProperty("last-url");
    if (base_url)
        base_url->setString(url);    
    String rel_path = 
        Url(attrModel_->attributeContext()->topSysid()).relativePath(url);

    //! Ensure that current index is not being edited
    QtTableView* table_view = dynamic_cast<QtTableView*>(attrModel_->view());
    if (table_view->editorWidget())
        table_view->closeEditor();
    qModel()->setData(currentIndex, QVariant(QString(rel_path)));
}
    
////////////////////////////////////////////////////////////////////////////

class AttributesTool : public QWidget,
                       public AttributeEditor,
                       public Ui::AttributesToolBase {
    Q_OBJECT
public:
    AttributesTool(QWidget* parent, StructEditor* se, 
                   AttrPropertyModel* attrModel,
                   PropertyNode* props,
                   Sui::LiquidItem::Type type);

    virtual QSize sizeHint () const { return sizeHint_; }
    
protected:
    void    setCaption(const String& text);
    void    updateButtons(const QModelIndex& index);

public slots:
    void    on_addAttributeButton__clicked() { 
        attrModel_->addAttribute(); 
    }
    void    on_removeAttributeButton__clicked() { 
        attrModel_->removeAttribute();
    }
    void    on_setDefaultValueButton__clicked();
    void    on_helpButton__clicked() { 
        helpAssistant().show(DOCTAG(UG_ATTR_D)); 
    }
    void    on_elementHelpButton__clicked() {
        showElementHelp(attributeTableView_->currentIndex());
    }
    
protected slots:
    void    selectUrl(const Common::String& url) {
        AttributeEditor::selectUrl(attributeTableView_->currentIndex(), url);
    }
    void    browseStarted() {
        AttributeEditor::setBrowseBaseUrl();        
    }
    void    contextChanged() { setupCaption(); }
    void    currentChanged(const QModelIndex& current,  
                           const QModelIndex& /*previous*/) {
        updateButtons(current);
    }
    void    dataChanged(const QModelIndex& topLeft, 
                        const QModelIndex& bottomRight) {
        QModelIndex current_index = attributeTableView_->currentIndex();
        if (current_index == topLeft || current_index == bottomRight)
            updateButtons(current_index);
    }
private:
    QSize sizeHint_;
};

QWidget* AttributeItem::makeWidget(QWidget* parent, Type type)
{
    return new AttributesTool(
        parent, structEditor(), attrModel(), properties(), type);
}

AttributesTool::AttributesTool(QWidget* parent, StructEditor* se, 
                               AttrPropertyModel* attrModel, 
                               PropertyNode* props,
                               Sui::LiquidItem::Type type)
    : QWidget(parent),
      AttributeEditor(se, attrModel, props)
{
    switch (type) {
        case Sui::LiquidItem::VERTICAL_TOOL:
            sizeHint_ = QSize(100, 270); break;
        case Sui::LiquidItem::HORIZONTAL_TOOL: 
            sizeHint_ = QSize(50, 300); break;
        default:
            sizeHint_ = QSize(350, 270); break;
    }
    setupUi(this);

    addAttributeButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("add_attribute")));
    setDefaultValueButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("set_default_attribute_value")));
    removeAttributeButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("delete_attribute")));
    helpButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("help")));
    elementHelpButton_->setIconSet(
        Sui::icon_provider().getIconSet(NOTR("element_help")));

    setup(browseButton_, attributeTableView_);
    updateButtons(attributeTableView_->currentIndex());
    setFocusProxy(attributeTableView_);
}

void AttributesTool::setCaption(const String& text)
{
    QWidget::setCaption(text);
    for (QWidget* w = this; w; w = w->parentWidget()) 
        if (w && w->inherits(NOTR("QDockWidget"))) 
            w->setWindowTitle(text);
}

void AttributesTool::on_setDefaultValueButton__clicked()
{
    QModelIndex curr = attributeTableView_->currentIndex();
    QVariant default_value = curr.data(AttrPropertyModel::DEFAULT_VALUE_ROLE);
    if (!default_value.isValid() || default_value== curr.data(Qt::DisplayRole))
        return;
    //! Ensure that current index is not being edited
    QtTableView* table_view = dynamic_cast<QtTableView*>(attrModel_->view());
    if (table_view->editorWidget())
        table_view->closeEditor();
    qModel()->setData(curr, default_value);
}

void AttributesTool::updateButtons(const QModelIndex& index)
{
    QModelIndex name_index = index.sibling(index.row(), ATTR_NAME_COLUMN);
    Qt::ItemFlags name_flags = qModel()->flags(name_index);
    removeAttributeButton_->setEnabled(name_flags.testFlag(Qt::ItemIsEnabled));

    Qt::ItemFlags curr_flags = qModel()->flags(index);
    const bool is_editable = curr_flags.testFlag(Qt::ItemIsEditable);
    
    browseButton_->setEnabled(
        ATTR_VALUE_COLUMN == index.column() && is_editable);
    elementHelpButton_->setEnabled(index.isValid());

    QString default_value = index.data(
        AttrPropertyModel::DEFAULT_VALUE_ROLE).toString();
    setDefaultValueButton_->setEnabled(
        !default_value.isNull() &&
        default_value != index.data(Qt::DisplayRole).toString());
}

////////////////////////////////////////////////////////////////////////////

class AttributesModalDialog : public QDialog,   
                              public Ui::AttributesDialogBase {
    Q_OBJECT
public:
    AttributesModalDialog(StructEditor* se, AttrPropertyModel* attrModel, 
                          PropertyNode* props) 
        : QDialog(se->sernaDoc()->widget(0))
    {    
        setupUi(this);
        
        QVBoxLayout* layout = new QVBoxLayout(toolWidget_);
        layout->setObjectName(QString::fromUtf8(NOTR("layout")));
        attributesTool_ = new AttributesTool(
            toolWidget_, se, attrModel, props, Sui::LiquidItem::UNDOCKED_TOOL);
        layout->addWidget(attributesTool_);
        layout->setContentsMargins(0, 0, 0, 0);
        attributesTool_->gridLayout->setContentsMargins(0, 0, 0, 0);
       
    }

public slots:
    void    on_okButton__clicked() { accept(); }
    void    on_cancelButton__clicked() { reject(); }

private:
    AttributesTool* attributesTool_;
};

////////////////////////////////////////////////////////////////////////////

class RequiredAttributesDialog : public QDialog,   
                                 public AttributeEditor,
                                 public Ui::AttributesModalDialogBase {
    Q_OBJECT
public:
    RequiredAttributesDialog(StructEditor* se, AttrPropertyModel* attrModel, 
                             PropertyNode* props) 
        : QDialog(se->sernaDoc()->widget(0)),
          AttributeEditor(se, attrModel, props)
    {    
        setupUi(this);

        helpButton_->setIconSet(
            Sui::icon_provider().getIconSet(NOTR("help")));
        elementHelpButton_->setIconSet(
            Sui::icon_provider().getIconSet(NOTR("element_help")));
        iconLabel_->setPixmap(
            Sui::icon_provider().getPixmap(NOTR("modified_attribute")));
        textLabel_->setText(
            tr("<qt>Set required attributes for element &lt;%1&gt;</qt>").
            arg(attrModel->attributeContext()->elementName()));

        setup(browseButton_, attributeTableView_);
        updateButtons(attributeTableView_->currentIndex());
    }

protected:
    void    setCaption(const String& text);
    void    updateButtons(const QModelIndex& index);

public slots:
    void    on_helpButton__clicked() { 
        helpAssistant().show(DOCTAG(UG_ATTR_D)); 
    }
    void    on_elementHelpButton__clicked() {
        showElementHelp(attributeTableView_->currentIndex());
    }
    void    on_okButton__clicked() { accept(); }
    void    on_cancelButton__clicked() { reject(); }

protected slots:
    void    selectUrl(const Common::String& url) {
        AttributeEditor::selectUrl(attributeTableView_->currentIndex(), url);
    }
    void    browseStarted() {
        AttributeEditor::setBrowseBaseUrl();        
    }
    void    currentChanged(const QModelIndex& current,  
                           const QModelIndex& /*previous*/) {
        updateButtons(current);
    }
};

///////////////////////////////////////////////////////////////////////////

void RequiredAttributesDialog::setCaption(const String& captionText)
{
    String text = captionText;
    String caption = props_->getSafeProperty("caption")->getString();
    if (!caption.isEmpty()) {
        //! Explicitely set caption takes precedence
        if (-1 != caption.find(NOTR("%1")))
            text = QString(caption).arg(text);
        else
            text = caption;
    }
    else
        text = tr("Required %1").arg(text);
    QDialog::setCaption(text);
}

void RequiredAttributesDialog::updateButtons(const QModelIndex& index)
{
    Qt::ItemFlags curr_flags = qModel()->flags(index);
    const bool is_editable = curr_flags.testFlag(Qt::ItemIsEditable);
    
    browseButton_->setEnabled(ATTR_VALUE_COLUMN == index.column() && 
                              is_editable);
    elementHelpButton_->setEnabled(index.isValid());
}

///////////////////////////////////////////////////////////////////////////

static bool exec_modal_attr_dialog(StructEditor* se, bool isRequired, 
                                   PropertyNode* props)
{
    AttributeContext attr_context(
        props->getSafeProperty("element-name")->getString(), 
        se->grove()->topSysid(),
        se->editViewSrcPos().node());
    OwnerPtr<AttrPropertyModel> attr_model( 
        AttrPropertyModel::makeInstance(&attr_context));

    PropertyNodePtr attrs = props->makeDescendant(EXISTING_ATTRS);    
    PropertyNodePtr specs = props->makeDescendant(ATTR_SPEC_LIST);
    PropertyNodePtr ns_map = props->makeDescendant(NS_MAP);
    attr_model->update(attrs, specs, ns_map);

    return (!isRequired) 
        ? (QDialog::Accepted == AttributesModalDialog(
               se, attr_model.pointer(), props).exec())
        : (QDialog::Accepted == RequiredAttributesDialog(
               se, attr_model.pointer(), props).exec());
}

///////////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(SetElementAttributes, StructEditor)
REGISTER_COMMAND_EVENT_MAKER(SetElementAttributes, 
                             "PropertyTree", "PropertyTree")
/* Allows to set required attributes and attributes passed in EXISTING_ATTRS.
   If no required attributes and no EXISTING_ATTRS, does nothing.  
*/
bool SetElementAttributes::doExecute(StructEditor* se, EventData*)
{
    PropertyNode* attrs = ed_->makeDescendant(EXISTING_ATTRS);
    PropertyNode* specs = ed_->makeDescendant(ATTR_SPEC_LIST);
    for (PropertyNode* attr_spec = specs->firstChild(); attr_spec;
         attr_spec = attr_spec->nextSibling()) {
        if (attr_spec->getProperty(Xs::ATTR_REQUIRED))
            attrs->makeDescendant(attr_spec->name(), String(), false);
    }
    if (!attrs->firstChild())
        return true;
    return exec_modal_attr_dialog(se, false, ed_);
}

PROPTREE_EVENT_IMPL(SetRequiredAttributes, SourceDocument)
/* Allows to set ONLY required attributes. If no required attributes
   passed in ATTR_SPEC_LIST, does nothing.  
*/
bool SetRequiredAttributes::doExecute(SourceDocument* srcDoc, EventData*)
{
    PropertyNode* attr_spec =
        ed_->makeDescendant(ATTR_SPEC_LIST)->firstChild();
    while (attr_spec) {
        if (!attr_spec->getProperty(Xs::ATTR_REQUIRED)) {
            PropertyNode* next_spec = attr_spec->nextSibling();
            attr_spec->remove();
            attr_spec = next_spec;
        }
        else
            attr_spec = attr_spec->nextSibling();
    }
    attr_spec = ed_->makeDescendant(ATTR_SPEC_LIST)->firstChild();
    if (0 == attr_spec)
        return true;

    PropertyNode* existing_attrs = ed_->makeDescendant(EXISTING_ATTRS);
    existing_attrs->removeAllChildren();
    for (; attr_spec; attr_spec = attr_spec->nextSibling())
        existing_attrs->appendChild(
            new PropertyNode(attr_spec->name(), String()));

    return exec_modal_attr_dialog(dynamic_cast<StructEditor*>(srcDoc), 
                                  true, ed_);
}

#include "moc/AttributesWidget.moc"
