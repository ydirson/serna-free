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
#include "customcontent.h"
#include "xpathutils.h"
#include "CustomContentDialog.h"
#include "structeditor/StructEditor.h"
#include "docview/SernaDoc.h"
#include "common/PropertyTree.h"
#include "ui/IconProvider.h"
#include "ui/ActionSet.h"

#include "utils/HelpAssistant.h"
#include "utils/Config.h"
#include "docutils/doctags.h"

#include <QTextEdit>
#include <QFileDialog>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPixmap>
#include <QShortcut>

using namespace Common;
using namespace ContentProps;

// START_IGNORE_LITERALS
const char* CONTENT_TEXT_EDIT       = "contentTextEdit";
const char* PATTERN_LINE_EDIT       = "patternLineEdit";
const char* INVALID_PATTERN_LABEL   = "invalidPatternLabel";

const char* LOCATION_COMBO          = "locationComboBox";
const char* LOCATION_LINE_EDIT      = "locationLineEdit";
const char* INVALID_LOCATION_LABEL  = "invalidLocationLabel";
// STOP_IGNORE_LITERALS

CustomContentDialog::CustomContentDialog(CustomNameChecker* nameChecker,
                                         PropertyNode* contentProp,
                                         QWidget* parent, StructEditor* se)
    : QDialog(parent),
      nameChecker_(nameChecker),
      contentProp_(contentProp),
      initialName_(contentProp_->getSafeProperty(NAME)->getString()),
      se_(se)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
        SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    ambiguousHotkeyIcon_->setPixmap(
        Sui::icon_provider().getPixmap(NOTR("modified_attribute")));
    nameLineEdit_->setText(contentProp_->getSafeProperty(NAME)->getString());
    inscriptionLineEdit_->setText(
        contentProp_->getSafeProperty(INSCRIPTION)->getString());

    iconComboBox_->setEditable(true);
    const String& curr_icon = contentProp_->getSafeProperty(ICON)->getString();
    Vector<String> icons;
    Sui::icon_provider().getIconNames(icons);
    bool is_current_in = false;
    for (uint c = 0; c < icons.size(); c++) {
        QSize size(Sui::icon_provider().getPixmap(icons[c]).size());
        if (size.width() > 20 || size.height() > 20)
            continue;
        iconComboBox_->insertItem(
            Sui::icon_provider().getPixmap(icons[c]), icons[c]);
        if (!is_current_in && curr_icon == icons[c])
            is_current_in = true;
    }
    if (!is_current_in)
        iconComboBox_->insertItem(
            Sui::icon_provider().getPixmap(curr_icon), curr_icon);

    for (int c = 0; c < iconComboBox_->count(); c++)
        if (curr_icon == iconComboBox_->text(c))
            iconComboBox_->setCurrentItem(c);

    createButtonCheckBox_->setChecked(
        contentProp_->getSafeProperty(MAKE_TOOL_BUTTON)->getBool()); 
    hotKeyLineEdit_->setText(QKeySequence(contentProp_->getSafeProperty(
                                              HOT_KEY)->getString()));
    on_hotKeyLineEdit__textChanged();
    
    contentTabWidget_->removePage(contentTabWidget_->page(0));
    const PropertyNode* c_specs = contentProp_->getSafeProperty(CONTEXT_SPECS);
    for (PropertyNode* c = c_specs->firstChild(); c ; c = c->nextSibling())
        addContent(c);
    updateContextButtons();
    connect(contentTabWidget_, SIGNAL(currentChanged(QWidget*)),
            SLOT(updateContextButtons()));
    
    qApp->installEventFilter(this);
    topLevelWidget()->installEventFilter(this);
}

void CustomContentDialog::updateContextButtons()
{
    removeContentButton_->setEnabled(1 < contentTabWidget_->count());
    moveLeftButton_->setEnabled(contentTabWidget_->currentPageIndex() > 0);
    moveRightButton_->setEnabled(contentTabWidget_->currentPageIndex() < 
                                 contentTabWidget_->count() - 1);
}

void CustomContentDialog::on_addContentButton__clicked()
{
    addContent(0);
    contentTabWidget_->setCurrentPage(contentTabWidget_->count() - 1);
    updateContextButtons();
}

void CustomContentDialog::on_removeContentButton__clicked()
{
    if (1 > contentTabWidget_->count())
        return;
    if (0 != se_->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING, tr("Remove Context Tab"),
            tr("Are you sure you want to remove current context tab?"),
            tr("&Remove"), tr("&Cancel")))
        return;
    const int idx = contentTabWidget_->currentPageIndex();
    contentTabWidget_->removePage(contentTabWidget_->currentPage());
    for (int i = idx; i < contentTabWidget_->count(); i++) 
        contentTabWidget_->setTabLabel(contentTabWidget_->page(i), 
                                       String::number(i + 1));
    updateContextButtons();
}

void CustomContentDialog::on_moveLeftButton__clicked()
{
    const int idx = contentTabWidget_->currentPageIndex();
    if (idx < 1)
        return;
    QWidget* widget = contentTabWidget_->currentPage();
    contentTabWidget_->removePage(widget);
    contentTabWidget_->insertTab(widget, String::number(idx), idx - 1);
    contentTabWidget_->setTabLabel(contentTabWidget_->page(idx), 
                                   String::number(idx + 1));
    contentTabWidget_->setCurrentPage(idx - 1);
    updateContextButtons();
}

void CustomContentDialog::on_moveRightButton__clicked()
{
    const int idx = contentTabWidget_->currentPageIndex();
    if (idx >= contentTabWidget_->count())
        return;
    QWidget* widget = contentTabWidget_->currentPage();
    contentTabWidget_->removePage(widget);
    contentTabWidget_->insertTab(widget, String::number(idx + 2), idx + 1);
    contentTabWidget_->setTabLabel(contentTabWidget_->page(idx), 
                                   String::number(idx + 1));
    contentTabWidget_->setCurrentPage(idx + 1);
    updateContextButtons();
}

bool CustomContentDialog::isValidPattern(const String& pattern) const
{
    if (pattern.isEmpty())
        return true;
    return PatternMatcher(pattern, GroveEditor::GrovePos(
        se_->grove()->document()->documentElement())).isValid();
}

bool CustomContentDialog::isValidLocation(const String& xpathLoc) const
{
    if (xpathLoc.isEmpty())
        return true;
    return XpathLocator(xpathLoc, GroveEditor::GrovePos(
        se_->grove()->document()->documentElement())).isValid();
}

void CustomContentDialog::addContent(PropertyNode* content)
{
    QWidget* widget = new QWidget(contentTabWidget_);
    QLayout* v_layout = new QVBoxLayout(widget, 11, 6);
    QTextEdit* text_edit = new QTextEdit(widget, CONTENT_TEXT_EDIT);
    text_edit->setTextFormat(Qt::PlainText);
    v_layout->add(text_edit);

    QLayout* pattern_layout = new QHBoxLayout(v_layout, 6);
    pattern_layout->add(new QLabel(tr("If matches:"), widget));
    QLineEdit* pattern_edit = new QLineEdit(widget, PATTERN_LINE_EDIT);
    pattern_layout->add(pattern_edit);
    QLabel* pattern_label = new QLabel(widget, INVALID_PATTERN_LABEL);
    pattern_label->setPixmap(
        Sui::icon_provider().getPixmap(NOTR("modified_attribute")));
    pattern_label->setToolTip( 
        tr("Context must either be a valid XSLT pattern or empty"));
    pattern_layout->add(pattern_label);

    QLayout* xpath_layout = new QHBoxLayout(v_layout, 6);
    xpath_layout->add(new QLabel(tr("Position:"), widget));
    QComboBox* loc_combo = new QComboBox(widget, LOCATION_COMBO);
    loc_combo->insertItem(location_inscription(INSERT_AFTER_), INSERT_AFTER_);
    loc_combo->insertItem(location_inscription(INSERT_BEFORE_), 
                          INSERT_BEFORE_);
    loc_combo->insertItem(location_inscription(APPEND_TO_), APPEND_TO_);
    loc_combo->setCurrentItem(APPEND_TO_);
    xpath_layout->add(loc_combo);
    xpath_layout->add(new QLabel(tr("Node:"), widget));
    QLineEdit* loc_edit = new QLineEdit(widget, LOCATION_LINE_EDIT);
    xpath_layout->add(loc_edit);
    QLabel* loc_label = new QLabel(widget, INVALID_LOCATION_LABEL);
    loc_label->setPixmap(Sui::icon_provider().getPixmap(NOTR("modified_attribute")));
    loc_label->setToolTip(
        tr("Must either be a valid XPATH expression or empty"));
    xpath_layout->add(loc_label);

    if (content) {
        text_edit->setPlainText(
            content->getSafeProperty(TEXT)->getString());
        pattern_edit->setText(
            content->getSafeProperty(PATTERN_STR)->getString());
        loc_edit->setText(
            content->getSafeProperty(XPATH_LOCATION)->getString());
        RelativeLocation loc = 
            location(content->getSafeProperty(REL_LOCATION)->getString());
        if (CURRENT_POS_ != loc)
            loc_combo->setCurrentItem(loc);
    }
    pattern_label->setVisible(!isValidPattern(pattern_edit->text()));
    loc_label->setVisible(!isValidLocation(loc_edit->text()));
    loc_combo->setEnabled(!loc_edit->text().isEmpty());

    contentTabWidget_->addTab(
        widget, String::number(contentTabWidget_->count() + 1));
    connect(pattern_edit, SIGNAL(textChanged(const QString&)), 
            SLOT(patternChanged()));
    connect(loc_edit, SIGNAL(textChanged(const QString&)), 
            SLOT(locationChanged()));
}

void CustomContentDialog::patternChanged()
{
    QWidget* page = contentTabWidget_->currentPage();
    if (!page)
        return;
    QObject* pattern_edit = page->child(PATTERN_LINE_EDIT, NOTR("QLineEdit"));
    String pattern = (pattern_edit) 
        ? String(static_cast<QLineEdit*>(pattern_edit)->text()) : String();
    QObject* label = page->child(INVALID_PATTERN_LABEL, NOTR("QLabel"));
    static_cast<QWidget*>(label)->setVisible(!isValidPattern(pattern));
}

void CustomContentDialog::locationChanged()
{
    QWidget* page = contentTabWidget_->currentPage();
    if (!page)
        return;
    QObject* loc_edit = page->child(LOCATION_LINE_EDIT, NOTR("QLineEdit"));
    String expr = (loc_edit) 
        ? String(static_cast<QLineEdit*>(loc_edit)->text()) : String();
    QObject* label = page->child(INVALID_LOCATION_LABEL, NOTR("QLabel"));
    static_cast<QWidget*>(label)->setVisible(!isValidLocation(expr));
    QObject* loc_combo = page->child(LOCATION_COMBO, NOTR("QComboBox"));
    if (loc_combo) 
        static_cast<QWidget*>(loc_combo)->setEnabled(!expr.isEmpty());
}

void CustomContentDialog::on_helpButton__clicked()
{
    helpAssistant().show(DOCTAG(CUSTOM_CONTENT_D));
}

void CustomContentDialog::on_browseIconButton__clicked()
{
    String icpath(config().getProperty("app/icon-path")->getString());
    String path = QFileDialog::getOpenFileName(this,
        tr("Choose an icon for Custom Content"),
        icpath,
        tr("Images (*.png *.xpm *.jpg);;All Files (*)"));
    if (!path.isEmpty()) {
        iconComboBox_->insertItem(Sui::icon_provider().getPixmap(path), path);
        iconComboBox_->setCurrentItem(iconComboBox_->count() - 1);
    }
}

bool CustomContentDialog::eventFilter(QObject*, QEvent *e)
{
    switch (e->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::Accel:
        case QEvent::AccelOverride: {
            QKeyEvent* ke = static_cast<QKeyEvent*>(e);
            if (focusWidget() == hotKeyLineEdit_ && 
                ((ke->state() & Qt::ControlButton) || 
                 (ke->state() & Qt::AltButton) ||
                 (ke->key() == Qt::Key_Control) ||
                 (ke->key() == Qt::Key_Alt) ) ) {
                bool is_real = true;
                int k = ke->key();
                int modifier = 0;
                switch (k) {
                    case Qt::Key_Shift:   modifier = Qt::SHIFT; break;
                    case Qt::Key_Control: modifier = Qt::CTRL; break;
                    case Qt::Key_Alt:     modifier = Qt::ALT; break;

                    case Qt::Key_Escape:
                    case Qt::Key_Backspace:  
                    case Qt::Key_Meta: is_real = false; break;
                }
                if (modifier) {
                    is_real = false;
                    k = Qt::Key_A;
                    k |= modifier;
                }
                if (!ke->state() && !is_real && !modifier) {
                    return false;
                }
                if (e->type() != QEvent::KeyRelease) {
                    if (ke->state() & Qt::ControlButton)
                        k |= Qt::CTRL;
                    if (ke->state() & Qt::AltButton)
                        k |= Qt::ALT;
                    if (ke->state() & Qt::ShiftButton) 
                        k |= Qt::SHIFT;
                    if (ke->state() & Qt::MetaButton)
                        k |= Qt::META;
                        
                    QString seq = QKeySequence(k);
                    hotKeyLineEdit_->setText(modifier 
                        ? seq.left(seq.length() - 1) : seq);
                }
                else if (!is_real && hotKeyLineEdit_->text().right(1) == "+") {
                    QString seq;
                    if (modifier) {
                        seq = QString(QKeySequence(k));
                        seq = seq.left(seq.length() - 1);
                        QString text = hotKeyLineEdit_->text();
                        if (0 <= text.find(seq))
                            seq = text.replace(seq, "");
                        if (NOTR("Shift+") == seq) 
                            seq = NOTR("");
                    }
                    hotKeyLineEdit_->setText(seq);
                }
                ke->accept();
                return true;
            }
            break;
        }
        //case QEvent::IMStart:
        //case QEvent::IMCompose:
        //case QEvent::IMEnd: 
        //    if (focusWidget() == hotKeyLineEdit_) 
        //        return true; 
        //    break;
        default:
            break;
    }
    return false;
}

void CustomContentDialog::on_iconComboBox__textChanged(const QString& path)
{
    QPixmap pixmap = Sui::icon_provider().getPixmap(path);
    if (pixmap.isNull())
        //pixmap = Sui::icon_provider().getPixmap("null");
        pixmap = Sui::icon_provider().getPixmap(NOTR("broken_image"));
    iconLabel_->setPixmap(pixmap);
}

void CustomContentDialog::dataChanged()
{
    String name = nameLineEdit_->text();
    bool name_is_valid = !name.isEmpty() &&
        (initialName_ == name || nameChecker_->isValidName(name));
    invalidNameLabel_->setVisible(!name_is_valid);
    okButton_->setEnabled(name_is_valid &&
                          !inscriptionLineEdit_->text().isEmpty());
}

class ActionHotkey : public Sui::ActionPred {
public:
    ActionHotkey(const String& hotkey, const String& exceptName)
        : hotkey_(hotkey),
          exceptName_(exceptName) {};
    virtual ~ActionHotkey() {}

    virtual bool        operator()(const Sui::Action* action) const
    {   
        if (hotkey_.isEmpty())
            return false;
        if (action->get(Sui::NAME) == exceptName_)
            return false;
        if (QKeySequence(action->get(Sui::ACCEL)) == hotkey_)
            return true;
        return false;
    }
private:
    QKeySequence        hotkey_;
    Common::String      exceptName_;
};

void CustomContentDialog::on_hotKeyLineEdit__textChanged()
{
    String hot_key = hotKeyLineEdit_->text();
    Sui::Action* action = se_->sernaDoc()->actionSet()->findAction(
        ActionHotkey(hot_key,
                      contentProp_->getSafeProperty(NAME)->getString()));
    if (action)
        ambiguousHotkeyLabel_->setText(
            tr("This hot key is already assigned to <b>%1</b> action.")
            .arg(action->get(Sui::NAME)));
    ambiguousHotkeyLabel_->setVisible(action);
    ambiguousHotkeyIcon_->setVisible(action);
}

void CustomContentDialog::on_okButton__clicked()
{
    String hot_key = hotKeyLineEdit_->text();
    Sui::Action* action = se_->sernaDoc()->actionSet()->findAction(
        ActionHotkey(hot_key,
                      contentProp_->getSafeProperty(NAME)->getString()));
    if (action) {
        if (0 != se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Assign Hot Key"), 
                tr("The hot-key [%0] will now work for"
                   "action [%1] instead of [%2]") 
                .arg(hot_key)
                .arg(nameLineEdit_->text())
                .arg(action->get(Sui::NAME)), 
                tr("&Ok"), tr("&Cancel")))
            return;
        else
            action->properties()->makeDescendant(
                Sui::ACCEL)->setString(String());
    }

    contentProp_->makeDescendant(NAME)->setString(nameLineEdit_->text());
    contentProp_->makeDescendant(INSCRIPTION)->setString(
        inscriptionLineEdit_->text());
    contentProp_->makeDescendant(Sui::TOOLTIP)->setString(
        inscriptionLineEdit_->text());
    contentProp_->makeDescendant(ICON)->setString(
        iconComboBox_->currentText());
    contentProp_->makeDescendant(MAKE_TOOL_BUTTON)->setBool(
        createButtonCheckBox_->isChecked());
    contentProp_->makeDescendant(HOT_KEY)->setString(hotKeyLineEdit_->text());

    PropertyNode* c_specs = contentProp_->makeDescendant(CONTEXT_SPECS);
    c_specs->removeAllChildren();
    for (int i = 0; i < contentTabWidget_->count(); i++) {
        PropertyNode* content = new PropertyNode(CONTENT);
        QWidget* page = contentTabWidget_->page(i);
        QObject* text_edit = page->child(CONTENT_TEXT_EDIT, NOTR("QTextEdit"));
        if (text_edit)
            content->makeDescendant(TEXT)->setString(
                static_cast<QTextEdit*>(text_edit)->text());
        QObject* pattern_edit = 
            page->child(PATTERN_LINE_EDIT, NOTR("QLineEdit"));
        if (pattern_edit)
            content->makeDescendant(PATTERN_STR)->setString(
                static_cast<QLineEdit*>(pattern_edit)->text());
        QObject* xpath_edit = page->child(LOCATION_LINE_EDIT, NOTR("QLineEdit"));
        if (xpath_edit)
            content->makeDescendant(XPATH_LOCATION)->setString(
                static_cast<QLineEdit*>(xpath_edit)->text());
        QObject* loc_combo = page->child(LOCATION_COMBO, NOTR("QComboBox"));
        if (loc_combo) {
            RelativeLocation loc =  (RelativeLocation)(
                static_cast<QComboBox*>(loc_combo)->currentItem());
            content->makeDescendant(REL_LOCATION)->setString(
                location_string(loc));
        }
        c_specs->appendChild(content);
    }
    QDialog::accept();
}

