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

#include "editableview/EditableView.h"
#include "utils/Properties.h"
#include "core/qt/NsColorDialog.h"
#include "core/Preferences.h"

#include "ui/UiPropertySyncher.h"
#include "DocumentLookWidgetBase.hpp"

#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QValidator>

#include <time.h>

using namespace Common;

class NsColorItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
public:
    NsColorItem(QTreeWidget* list, Common::PropertyNode* property);

    Common::PropertyNode*   property() const { return property_; }
    bool                    isDefault() const { return isDefault_; }

    virtual void            setText(int col, const QString& text);

public slots:
    void                    setColor(QTreeWidgetItem* item, int column);

protected:
    QColor                  tagColor() const;
    QColor                  textColor() const;

private:
    Common::PropertyNode*   property_;
    const bool              isDefault_;
};

class DocumentLookWidget : public QWidget,
                           private Ui::DocumentLookWidgetBase {
    Q_OBJECT
public:
    DocumentLookWidget(Common::PropertyNode* prop);

protected slots:
    virtual void    on_chooseTagFontButton__clicked();
    virtual void    on_chooseColorEven__clicked();
    virtual void    on_chooseColorOdd__clicked();
    virtual void    on_addUriButton__clicked();
    virtual void    on_editUriButton__clicked();
    virtual void    on_deleteUriButton__clicked();
    virtual void    on_namespaceListView__itemSelectionChanged();

private:
    Common::PropertyNode*   prop_;
    QRegExpValidator        indentValidator_;
    Sui::PropertySyncherPtr indentSyncher_;
    Sui::PropertySyncherPtr tagFontSyncher_;
    Sui::PropertySyncherPtr oddColorSyncher_;
    Sui::PropertySyncherPtr evenColorSyncher_;
    Sui::PropertySyncherPtr showNbspSyncher_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(DocumentLook, 1, true)

static String color_str(const QColor& color)
{
    Formatter::Rgb rgb(color.red(), color.green(), color.blue());
    return String(DocLook::rgb_color(rgb));
}

static QColor get_color_prop(PropertyNode* prop)
{
    Formatter::Rgb color = DocLook::parse_rgb_color(prop->getString());
    return QColor(color.r_, color.g_, color.b_);
}

static void set_color_prop(PropertyNode* prop)
{
    QColor color =
        QColorDialog::getColor(get_color_prop(prop), 0, NOTR("color dialog"));
    if (color.isValid())
        prop->setString(color_str(color));
}

///////////////////////////////////////////////////////////////////////

NsColorItem::NsColorItem(QTreeWidget* list, PropertyNode* property)
    : property_(property),
      isDefault_(property_->getProperty(DocLook::DEFAULT_NS_COLOR))
{
    list->addTopLevelItem(this);
    if (isDefault_)
        setText(0, tr("No namespace URI"));
    else
        setText(0, property_->makeDescendant(DocLook::NS_URI)->getString());
    connect(list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(setColor(QTreeWidgetItem*, int)));
    setBackgroundColor(1, tagColor());
    setBackgroundColor(2, textColor());
}

QColor NsColorItem::tagColor() const
{
    return get_color_prop(property_->makeDescendant(DocLook::NS_TAG_COLOR));
}

QColor NsColorItem::textColor() const
{
    return get_color_prop(property_->makeDescendant(DocLook::NS_TEXT_COLOR));
}

void NsColorItem::setText(int col, const QString& text)
{
    if (0 == col && !text.isEmpty()) {
        if (!isDefault_)
            property_->makeDescendant(DocLook::NS_URI)->setString(text);
        QTreeWidgetItem::setText(col, text);
    }
}

void NsColorItem::setColor(QTreeWidgetItem* item, int column)
{
    if (item != this)
        return;
    if (0 < column) {
        if (1 == column)
            set_color_prop(property_->getProperty(DocLook::NS_TAG_COLOR));
        else
            set_color_prop(property_->getProperty(DocLook::NS_TEXT_COLOR));
    }
    else if (0 == column) {
        PropertyNodePtr property_copy = property_->copy(true);
        NsColorDialog dialog(treeWidget(), property_copy.pointer(), isDefault_);
        String uri;
        while (true) {
            if (QDialog::Accepted != dialog.exec())
                return;
            uri = property_copy->getProperty(DocLook::NS_URI)->getString();
            if (uri == property_->getProperty(DocLook::NS_URI)->getString())
                break;
            PropertyNode* p = property_->parent()->firstChild();
            bool duplicated = false;
            for (; p; p = p->nextSibling())
                if (uri == p->getProperty(DocLook::NS_URI)->getString())
                    duplicated = true;
            if (!duplicated)
                break;
            QMessageBox::warning(treeWidget(), tr("Warning"),
                tr("URI map with this name already exists."));
        }
        property_->getProperty(DocLook::NS_URI)->setString(uri);
        property_->getProperty(DocLook::NS_TAG_COLOR)->setString(
            property_copy->getProperty(DocLook::NS_TAG_COLOR)->getString());
        property_->getProperty(DocLook::NS_TEXT_COLOR)->setString(
            property_copy->getProperty(DocLook::NS_TEXT_COLOR)->getString());
        setBackgroundColor(1, tagColor());
        setBackgroundColor(2, textColor());
    }
}

//////////////////////////////////////////////////////////////////////////

void NsColorDialog::tagColor()
{
    set_color_prop(prop_->getProperty(DocLook::NS_TAG_COLOR));
}

void NsColorDialog::textColor()
{
    set_color_prop(prop_->getProperty(DocLook::NS_TEXT_COLOR));
}

//////////////////////////////////////////////////////////////////////////

PropertyNodePtr make_color_map_entry(bool isDefault)
{
    PropertyNodePtr p = new PropertyNode(DocLook::NS_COLOR_MAP_ENTRY);
    p->makeDescendant(DocLook::NS_URI);
    srand( (unsigned)time( NULL ) );
    int red = isDefault ? 161 : rand()%120 + 100;  //was 161
    int green = isDefault ? 221 : rand()%120 + 100;//was 221
    int blue = isDefault ? 221 : rand()%120 + 100; //was 221
    String color = String::number(red) + "," +
        String::number(green) + "," +String::number(blue);
    p->makeDescendant(DocLook::NS_TAG_COLOR)->setString(color);
    p->makeDescendant(DocLook::NS_TEXT_COLOR)->setString(NOTR("0,0,0"));
    if (isDefault)
        p->makeDescendant(DocLook::DEFAULT_NS_COLOR)->setBool(true);
    return p;
}

DocumentLookWidget::DocumentLookWidget(PropertyNode* prop)
    :  prop_(prop->makeDescendant(DocLook::DOC_LOOK)),
       indentValidator_(QRegExp(NOTR("^\\d{1,3}\\s?(cm|mm|in|pc|pt|px)$")), 0)
{
    setupUi(this);

    indentLineEdit_->setValidator(&indentValidator_);
    indentSyncher_ =
        new Sui::LineEditSyncher(prop_->makeDescendant(DocLook::INDENT),
            indentLineEdit_);
    String tag_font = (EditableView::fontManager())
        ? EditableView::fontManager()->getTagFont()->toString() : String();
    PropertyNode* tag_font_prop = prop_->makeDescendant(DocLook::TAG_FONT);
    if (tag_font_prop->getString().isEmpty())
        tag_font_prop->setString(tag_font);
    tagFontSyncher_ =
        new Sui::LineEditSyncher(prop_->makeDescendant(DocLook::TAG_FONT),
                            tagFontLineEdit_);

    oddColorSyncher_ = new Sui::ColorSyncher(
        prop_->makeDescendant(DocLook::ODD_COLOR),  colorOdd_);
    evenColorSyncher_ = new Sui::ColorSyncher(
        prop_->makeDescendant(DocLook::EVEN_COLOR), colorEven_);
    showNbspSyncher_ = new Sui::ButtonSyncher(
        prop_->makeDescendant(DocLook::SHOW_NBSP), nbspCheckBox_);

    PropertyNodePtr color_map = prop_->makeDescendant(DocLook::NS_COLOR_MAP);
    for (PropertyNodePtr p = color_map->firstChild();
         p; p = p->nextSibling()) {
        if (DocLook::NS_COLOR_MAP_ENTRY == p->name())
            new NsColorItem(namespaceListView_, p.pointer());
    }
    if (0 == namespaceListView_->topLevelItemCount()) {
        PropertyNodePtr default_prop = make_color_map_entry(true);
        color_map->appendChild(default_prop.pointer());
        new NsColorItem(namespaceListView_, default_prop.pointer());
    }
    namespaceListView_->header()->setResizeMode(QHeaderView::Stretch);
}

void  DocumentLookWidget::on_addUriButton__clicked()
{
    PropertyNodePtr color_map = prop_->makeDescendant(DocLook::NS_COLOR_MAP);
    PropertyNodePtr p = make_color_map_entry(false);

    NsColorDialog dialog(namespaceListView_, p.pointer(), false);
    while (true) {
        if (QDialog::Accepted != dialog.exec())
            return;
        String uri = p->getProperty(DocLook::NS_URI)->getString();
        PropertyNode* prop = color_map->firstChild();
        bool duplicated = false;
        for (; prop; prop = prop->nextSibling())
            if (uri == prop->getProperty(DocLook::NS_URI)->getString())
                duplicated = true;
        if (!duplicated)
            break;
        QMessageBox::warning(namespaceListView_, tr("Warning"),
                             tr("URI map with this name is already exists."));
    }

    color_map->appendChild(p.pointer());
    QTreeWidgetItem* new_item =
        new NsColorItem(namespaceListView_, p.pointer());
    QList<QTreeWidgetItem*> items(namespaceListView_->selectedItems());
    if (items.size())
        namespaceListView_->setItemSelected(items[0], false);
    namespaceListView_->setItemSelected(new_item, true);
}

void DocumentLookWidget::on_editUriButton__clicked()
{
    NsColorItem* ns_item =
        dynamic_cast<NsColorItem*>(namespaceListView_->currentItem());
    if (ns_item) 
        ns_item->setColor(ns_item, 0);
}

void DocumentLookWidget::on_deleteUriButton__clicked()
{
    NsColorItem* ns_item =
        dynamic_cast<NsColorItem*>(namespaceListView_->currentItem());
    if (ns_item && !ns_item->isDefault()) {
        ns_item->property()->remove();
        delete ns_item;
    }
}

void DocumentLookWidget::on_namespaceListView__itemSelectionChanged()
{
    QList<QTreeWidgetItem*> items(namespaceListView_->selectedItems());
    if (!items.size()) {
        deleteUriButton_->setEnabled(false);
        editUriButton_->setEnabled(false);
    } else {
        NsColorItem* ns_item = dynamic_cast<NsColorItem*>(items[0]);
        deleteUriButton_->setEnabled(ns_item && !ns_item->isDefault());
        editUriButton_->setEnabled(ns_item);
    }
}

void DocumentLookWidget::on_chooseColorEven__clicked()
{
    QColor color = QColorDialog::getColor(
        colorEven_->paletteBackgroundColor(),this, NOTR("color dialog"));
    if (color.isValid())
        prop_->makeDescendant(DocLook::EVEN_COLOR)->setString(
            color_str(color));
}

void DocumentLookWidget::on_chooseColorOdd__clicked()
{
    QColor color = QColorDialog::getColor
        (colorOdd_->paletteBackgroundColor(), this, NOTR("color dialog"));
    if (color.isValid())
        prop_->makeDescendant(DocLook::ODD_COLOR)->setString(color_str(color));
}

void DocumentLookWidget::on_chooseTagFontButton__clicked()
{
    if (tagFontLineEdit_->text().isEmpty()) {
        bool ok = true;
        QFont font(QFontDialog::getFont(&ok));
        if (ok)
            tagFontLineEdit_->setText(font.toString());
    }
    else {
        QFont font;
        bool ok = true;
        font.fromString(tagFontLineEdit_->text());
        font = QFontDialog::getFont(&ok, font);
        if (ok)
            tagFontLineEdit_->setText(font.toString());
    }
}

#include "moc/DocumentLookWidget.moc"

