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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "core/XmlCatalogsWidgetBase.hpp"
#include "core/Preferences.h"

#include "common/PathName.h"
#include "common/String.h"
#include "ui/UiPropertySyncher.h"

#include "utils/Config.h"
#include "utils/Properties.h"

#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>

using namespace Common;

class XmlCatalogsWidget : public QWidget,
                          public Ui::XmlCatalogsWidgetBase,
                          public Sui::PropertySyncherHolder {
    Q_OBJECT
public:
    XmlCatalogsWidget(Common::PropertyNode* props);

public slots:
    virtual void on_addCatalogsButton__clicked();
    virtual void on_addDtdPathButton__clicked();

private:
    Common::PropertyNode* props_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(XmlCatalogs, 6, true)

XmlCatalogsWidget::XmlCatalogsWidget(PropertyNode* props)
    : props_(props->makeDescendant(XmlCatalogs::XML_CATALOGS))
{
    setupUi(this);
#ifdef _WIN32
    QString repl(tr("semicolons", "separated by"));
#else
    QString repl(tr("colons", "separated by"));
#endif
    dtdPathLabel_->setText(dtdPathLabel_->text().arg(repl));
    catalogsLabel_->setText(catalogsLabel_->text().arg(repl));
    addSyncher(new Sui::LineEditSyncher(
        props_->makeDescendant(XmlCatalogs::CAT_PATH), catalogsEdit_)); 
    addSyncher(new Sui::LineEditSyncher(
        props_->makeDescendant(XmlCatalogs::DTD_PATH), dtdPathEdit_));
    addSyncher(new Sui::ButtonSyncher(
        props_->makeDescendant(XmlCatalogs::USE_CATALOGS), useRadioButton_));
    Sui::SelectorSyncher* sel = new Sui::SelectorSyncher(
        props_->makeDescendant(XmlCatalogs::USE_CATALOGS));
    sel->addSelector(NOTR("true"),   useRadioButton_);
    sel->addSelector(NOTR("false"),  dontUseRadioButton_);
    addSyncher(sel);
    sel = new Sui::SelectorSyncher(props_->makeDescendant(XmlCatalogs::PREFER));
    sel->addSelector(NOTR("public"), preferPublicButton_);
    sel->addSelector(NOTR("system"), preferSystemButton_);
    addSyncher(sel);
}

static void add_path(QString textToAdd, PropertyNode* ptn)
{
    if (textToAdd.isEmpty())
        return;
    QString text = ptn->getString().stripWhiteSpace();
    if (!text.isEmpty() && !text.endsWith(QChar(PathName::PATH_SEP)))
        text += PathName::PATH_SEP;
    text += textToAdd;
    ptn->setString(text);
}

void XmlCatalogsWidget::on_addCatalogsButton__clicked()
{
    QStringList tmp(QFileDialog::getOpenFileNames(
                        tr("All files (*)"), QString::null, this,
                        tr("add catalog dialog"), tr("Choose catalog(s)")));
    QString catalogs(tmp.join(QChar(PathName::PATH_SEP)));
    add_path(catalogs, props_->makeDescendant(XmlCatalogs::CAT_PATH));
}

void XmlCatalogsWidget::on_addDtdPathButton__clicked()
{
    QString dp(QFileDialog::getExistingDirectory(
                   QString::null, this, tr("add dtd path dialog"),
                   tr("Choose DTD path to add")));
    add_path(dp, props_->makeDescendant(XmlCatalogs::DTD_PATH));
}

#include "moc/XmlCatalogsWidget.moc"
