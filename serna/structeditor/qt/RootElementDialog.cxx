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
#include "docview/SernaDoc.h"
#include "utils/NameChecker.h"
#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/HelpAssistant.h"
#include "common/StringTokenizer.h"
#include "docutils/doctags.h"

#include "structeditor/RootElementDialogBase.hpp"
#include <QShortcut>

using namespace Common;
using namespace GroveLib;

class QtRootElementDialog : public QDialog,
                            public Ui::RootElementDialogBase {
    Q_OBJECT
public:
    QtRootElementDialog(Common::PropertyNode* param,
                        QWidget* parentWidget);
protected slots:
    void        on_nameComboBox__textChanged();
    void        on_showAll__toggled(bool);
    void        on_buttonHelp__clicked();

public:
    void        fillNameCombobox(bool);
    void        turnExtension(bool);

    Common::PropertyNode*  p_;
    Common::String  uri_;
    Common::String  rootElems_;
};

static const PropertyNode* get_first_elem(const PropertyNode* root)
{
    const PropertyNode* pn = root->getProperty("all-elements");
    return pn ? pn->firstChild() : 0;
}

QtRootElementDialog::QtRootElementDialog(PropertyNode* param,
                                         QWidget* parentWidget)
    : QDialog(parentWidget), p_(param)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
        SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));
    rootElems_ = param->getSafeProperty(DocSrcInfo::ROOT_ELEMENTS)->getString();
    if (rootElems_.isEmpty())
        showAll_->hide();
    fillNameCombobox(false);
    turnExtension(false);
    nameComboBox_->setFocus();
    nameComboBox_->lineEdit()->selectAll();
    on_nameComboBox__textChanged();
    show();
    setFixedHeight(minimumHeight());
}

void QtRootElementDialog::fillNameCombobox(bool showAll)
{
    nameComboBox_->clear();
    const PropertyNode* node = get_first_elem(p_);
    for(; node; node = node->nextSibling()) {
        if (showAll || rootElems_.isEmpty()) {
            nameComboBox_->insertItem(node->name());
            continue;
        }
        COMMON_NS::StringTokenizer st(rootElems_);
        while (st) {
            if (st.next() == node->name()) {
                nameComboBox_->insertItem(node->name());
                break;
            }
        }
    }
}

void QtRootElementDialog::turnExtension(bool state)
{
    prefixLabel_->setVisible(state);
    prefixNsLabel_->setVisible(state);
    prefixName_->setVisible(state);
    prefixLine_->setVisible(state);
}

void QtRootElementDialog::on_showAll__toggled(bool v)
{
    fillNameCombobox(v);
    nameComboBox_->lineEdit()->selectAll();
}

void QtRootElementDialog::on_nameComboBox__textChanged()
{
    String elem_name = nameComboBox_->lineEdit()->text();
    bool v = !elem_name.isEmpty();
    v = NameChecker::isValidQname(elem_name) & v;
    buttonOk_->setEnabled(v);
    const PropertyNode* pn = get_first_elem(p_);
    if (pn)
        pn = pn->parent()->getProperty(elem_name);
    if (0 == pn)
        return;
    uri_ = pn->getString();
    if (uri_.isEmpty()) {
        turnExtension(false);
        prefixNsLabel_->setText("");
    } else {
        prefixNsLabel_->setText(NOTR("\"") + uri_ + NOTR("\""));
        turnExtension(true);
    }
}

void QtRootElementDialog::on_buttonHelp__clicked()
{
    helpAssistant().show(DOCTAG(UG_ROOT_D));
}

//////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(RootElementDialog, SernaDoc)

bool RootElementDialog::doExecute(SernaDoc* sd, EventData* result)
{
    QtRootElementDialog d(ed_, sd->widget(0));
    if (QDialog::Accepted != d.exec())
        return false;
    PropertyNode* prop = static_cast<PropertyTreeEventData*>(result)->root();
    prop->makeDescendant("root-element")->setString(
        d.nameComboBox_->currentText());
    if (!d.uri_.isEmpty()) {
        prop->makeDescendant("prefix")->setString(d.prefixLine_->text());
        prop->makeDescendant("uri")->setString(d.uri_);
    }
    return true;
}

#include "moc/RootElementDialog.moc"
