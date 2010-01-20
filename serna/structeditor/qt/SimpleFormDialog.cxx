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
#include "structeditor/qt/SimpleFormDialog.h"

#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/impl/XsUtils.h"

#include "common/PropertyTreeEventData.h"
#include "groveeditor/GroveEditor.h"

#include "formatter/types.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/Font.h"
#include "editableview/EditableView.h"

#include <QApplication>
#include <QEventLoop>
#include <QDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QShortcut>
#include <QPointer>
#include <QAbstractItemView>
#include <iostream>

using namespace Common;
using namespace GroveLib;
using namespace Formatter;
using namespace GroveEditor;
using namespace ElementSpace;

/////////////////////////////////////////////////////////////////////////

static void get_schema_value_list(const Area* area, const StructEditor* se, 
                                  StringVector& valueList)
{   
    if (!area || !area->chain())
        return;
    //! For attribute node obtain possible values
    Node* origin = 
        const_cast<Node*>(XslFoExt::origin(area->chain()->headFoNode()));
    if (origin && Node::ATTRIBUTE_NODE == origin->nodeType()) {
        Attr* attr = static_cast<Attr*>(origin);
        PropertyNodePtr attr_spec_list(new PropertyNode(NOTR("attr-specs")));
        get_schema_attributes(attr->element(), attr_spec_list.pointer());
        const PropertyNode* attr_spec = 
            find_attr_spec(attr_spec_list.pointer(), attr);
        if (attr_spec)
            attr_spec = attr_spec->getProperty("enum");
        if (attr_spec) {
            for (PropertyNode* value = attr_spec->firstChild(); value; 
                value = value->nextSibling()) 
                    valueList.push_back(value->name());
        }
        return;
    }
    GrovePos pos;
    if (!se->getCheckedPos(pos))
        return;
    //! For text and element nodes: Check if it is simple type content
    Element* elem = traverse_to_element(pos);
    if (!elem)
        return;
    PropertyTreeEventData data;
    if (get_simpletype_content(elem, data.root())) {
        PropertyNode* value = data.root()->getProperty(ELEM_VALUE_ENUM);
        if (value)
            value = value->firstChild();
        for (; value; value = value->nextSibling()) 
            valueList.push_back(value->getString());
    }
}

static QRect area_geometry(const Area* area, const EditableView& ev)
{
    const CPoint allcPoint(area->absAllcPoint());
    CRange range(area->contRange());
    const double scale = ev.getZoom();
    range.h_ *= scale;
    range.w_ *= scale;
    return QRect(ev.mapToGlobal(area->absAllcPoint()),
        QSize(int(range.w_), int(range.h_)));
}

static bool is_equal(const String& a, const String& b)
{
    return (a == b || (a.isEmpty() && b.isEmpty()));
}

static bool edit_combo_box(StructEditor* structEditor, const Area* a, 
                           const QRect& geometry,
                           const QFont& font, bool isMouse, 
                           String& user_input)
{
    const ComboBoxArea* area = SAFE_CAST(const ComboBoxArea*, a);
    EditableView& ev = structEditor->editableView();
    if (!area || !area->isEnabled()) 
        return false;

    //! If value list in se:combo-box is empty try to obtain it from schema
    StringVector value_list = area->valueList();
    if (0 == value_list.size())
        get_schema_value_list(area, structEditor, value_list);

    //! Create and decorate QComboBox
    QPointer<SeComboBox> combo(new SeComboBox(ev.widget(),
        geometry, value_list, area->text()));
    combo->setEditable(area->isEditable());
    QFont myfont(font);
    if (ev.getZoom() != 1.)
        myfont.setPointSizeF(font.pointSizeF() * ev.getZoom());
    combo->setFont(myfont);

    //! If combo-box area was mouse-clicked, send the same event again 
    //  to SeComboBox widget.
    if (isMouse) {
        combo->view()->setAttribute(Qt::WA_NoMouseReplay, true);
        combo->view()->setAttribute(Qt::WA_NoMousePropagation, true);
        qApp->postEvent(combo, new QMouseEvent(
                            QEvent::MouseButtonPress, QPoint(5, 5), 
                           Qt::LeftButton, Qt::LeftButton));
    }
    bool ok = false;
    //! Execute combo box in separate event loop
    if (QDialog::Accepted == combo->exec() && 
        !is_equal(area->text(), combo->currentText())) {
        user_input = combo->currentText();
        ok = true;
    }
    delete combo;
    return ok;
}

static bool edit_line_edit(StructEditor* structEditor, const Area* a, 
                           const QRect& geometry, const QFont& font, 
                           bool isMouse, String& user_input)
{
    const LineEditArea* area = SAFE_CAST(const LineEditArea*, a);
    EditableView& ev = structEditor->editableView();
    if (!area || !area->isEnabled()) 
        return false;

    //! Create and decorate QLineEdit
    String text = area->text() + user_input;
    QPointer<SeLineEdit> line_edit(new SeLineEdit(ev.widget(),
        geometry, text, text.length()));
    QFont myfont(font);
    if (ev.getZoom() != 1.)
        myfont.setPointSizeF(font.pointSizeF() * ev.getZoom());
    line_edit->setFont(myfont);

    //! If line_edit area was mouse-clicked, send the same event again 
    //  to SeLineEdit widget.
    if (isMouse) 
        qApp->postEvent(line_edit, new QMouseEvent(
                            QEvent::MouseButtonPress, QPoint(0, 0), 
                            Qt::LeftButton, Qt::LeftButton));
    bool ok = false;
    //! Execute line_edit box in separate event loop
    if (QDialog::Accepted == line_edit->exec() && 
        !is_equal(area->text(), line_edit->text())) {
        user_input = line_edit->text();
        ok = true;
    }
    delete line_edit;
    return ok;
}

void edit_simple_form_area(StructEditor* structEditor, QEvent* event = 0)
{
    QMouseEvent* mouse_event = (event && 
                                event->type() == QEvent::MouseButtonPress) 
        ? static_cast<QMouseEvent*>(event) : 0;
    QKeyEvent* key_event = (event && event->type() == QEvent::KeyPress) 
        ? static_cast<QKeyEvent*>(event) : 0;
    
    const Area* area = 
        structEditor->editableView().context().areaPos().area();
    if (!area)
        return;
    Node* src_node = const_cast<Node*>(
        XslFoExt::origin(area->chain()->headFoNode()));
    if (!src_node)
        return;

    //! Check if current section is editable
    GrovePos src_pos = (Node::ATTRIBUTE_NODE == src_node->nodeType()) 
        ? GrovePos(static_cast<Attr*>(src_node)->element(), (Node*)(0))
        : structEditor->editViewSrcPos();
    if (StructEditor::POS_OK != structEditor->isEditableEntity(src_pos, 
        StructEditor::TEXT_OP))
            return;
    //! TODO: find appropriate handler for this areaType
    String user_input = (key_event) 
        ? String(key_event->text()) : String::null();
    const QFont& qt_font = area->chain()->font()->qfont();
    QRect geometry = area_geometry(area, structEditor->editableView());
    switch (area->type()) {
        case COMBO_BOX_AREA :
            if (!edit_combo_box(structEditor, area, geometry, 
                                qt_font, mouse_event, user_input))
                return;
            break;
        case LINE_EDIT_AREA :
            if (!edit_line_edit(structEditor, area, geometry, 
                                qt_font, mouse_event, user_input))
                return;
            break;
        default:
            return;
    }
    
    GroveEditor::Editor* grove_editor = structEditor->groveEditor();
    //! Set value returned by user
    if (Node::ATTRIBUTE_NODE == src_node->nodeType()) {
        structEditor->executeAndUpdate(
            grove_editor->setAttribute(static_cast<Attr*>(src_node), 
                                       user_input)); 
        return;
    }
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;

    if (Node::TEXT_NODE != src_node->nodeType() && src_node->firstChild())
        src_node = src_node->firstChild();
    if (Node::TEXT_NODE == src_node->nodeType()) {
        Text* text = TEXT_CAST(src_node);
        Node* parent = text->parent();
        CommandPtr cmd = (text->data().isEmpty()) 
            ? grove_editor->insertText(GrovePos(text), user_input)
            : grove_editor->replaceText(
                GrovePos(text), text->data().length(), user_input);
        batch_cmd->executeAndAdd(cmd.pointer());
        batch_cmd->setSuggestedPos((parent == text->parent()) 
                                   ? GrovePos(parent, text) 
                                   : GrovePos(parent));
    }
    else {
        batch_cmd->executeAndAdd(
            structEditor->groveEditor()->insertText(
                GrovePos(src_node), user_input).pointer());
        batch_cmd->setSuggestedPos(GrovePos(src_node, (Node*)(0)));
    }
    structEditor->executeAndUpdate(batch_cmd.pointer());
}

////////////////////////////////////////////////////////////////////////////

SeComboBox::SeComboBox(QWidget* parent, const QRect& geometry,
                       const StringVector& valueList,
                       const String& currentValue) 
    : QComboBox(parent),
      geometry_(geometry)
{
    reparent(parent, Qt::WType_Popup , geometry.topLeft());
    setFixedSize(geometry_.size());

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this),
        SIGNAL(activated()), this, SLOT(done()));
    connect(this, SIGNAL(activated(const QString&)),
            SLOT(select(const QString&)));
    bool curr_value_exists = false;
    for (uint c = 0; c < valueList.size(); c++) {
        insertItem(valueList[c]);
        if (valueList[c] == currentValue)
            curr_value_exists = true;
    }
    if (!curr_value_exists)
        insertItem(currentValue);
    setCurrentText(currentValue);
}

int SeComboBox::exec()
{
    if (!isEnabled()) 
        return QDialog::Rejected;

    if (eventLoop_) {
        qWarning(NOTR("QDialog::exec: Recursive call detected"));
        return -1;
    }
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowFlags(Qt::Popup|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
#ifdef _WIN32
    QWidget* active_window = qApp->activeWindow();
#endif // _WIN32
    show();
    setFocus();
    QEventLoop eventLoop;
    eventLoop_ = &eventLoop;
    result_ = 0;
    (void) eventLoop.exec();
    eventLoop_ = 0;
#ifdef _WIN32
    active_window->activateWindow();
#endif // _WIN32
    return result_;
}

void SeComboBox::select(const QString&)
{
    done(QDialog::Accepted);
}

void SeComboBox::done(int result)
{
    result_ = result;
    done();
}

void SeComboBox::done()
{
    close();
    eventLoop_->exit();
}

void SeComboBox::mousePressEvent(QMouseEvent* e)
{
    QRect rect(QPoint(0, 0), geometry().size());
    if (!rect.contains(e->pos())) {
        done(QDialog::Accepted);
        return;
    }
    QComboBox::mousePressEvent(e);
}

////////////////////////////////////////////////////////////////////////////

SeLineEdit::SeLineEdit(QWidget* parent, const QRect& geometry,
                       const String& currentValue, int cursorPos) 
    : QLineEdit(currentValue, parent), accepted_(false)
{
    reparent(parent, Qt::WType_Popup , geometry.topLeft());
    setFixedSize(geometry.size());

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this),
        SIGNAL(activated()), this, SLOT(reject()));
    connect(this, SIGNAL(returnPressed()), SLOT(accept()));
    setCursorPosition(cursorPos);
}

int SeLineEdit::exec()
{
    if (!isEnabled())
        return QDialog::Rejected;
 
    if (!isEnabled()) 
        return QDialog::Rejected;

    if (eventLoop_) {
        qWarning(NOTR("QDialog::exec: Recursive call detected"));
        return -1;
    }
    setWindowFlags(Qt::Popup|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_NoMouseReplay, true);
    setAttribute(Qt::WA_NoMousePropagation, true);
    show();
    setFocus();
    QEventLoop eventLoop;
    eventLoop_ = &eventLoop;
    result_ = 0;
    (void) eventLoop.exec();
    eventLoop_ = 0;
    return result_;
}

void SeLineEdit::accept()
{
    if (accepted_)
        return;
    accepted_ = true;
    result_ = QDialog::Accepted;
    eventLoop_->exit();
    close();
}

void SeLineEdit::reject()
{
    if (accepted_)
        return;
    accepted_ = true;
    result_ = QDialog::Rejected;
    eventLoop_->exit();
    close();
}

void SeLineEdit::mousePressEvent(QMouseEvent* e)
{
    QRect rect(QPoint(0, 0), geometry().size());
    if (!rect.contains(e->pos())) {
        accept();
        return;
    }
    QLineEdit::mousePressEvent(e);
}

bool SeLineEdit::event(QEvent* e)
{
    if (e->type() == QEvent::HideToParent) 
        accept();    
    return QLineEdit::event(e);
}
