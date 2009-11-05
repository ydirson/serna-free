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

#include "utils/Properties.h"
#include "utils/MsgBoxStream.h"
#include "utils/SernaMessages.h"
#include "utils/Config.h"
#include "utils/config_defaults.h"
#include "utils/config_package.h"
#include "utils/DocSrcInfo.h"
#include "utils/i18n_utils.h"

#include "docview/SernaDoc.h"
#include "common/PathName.h"
#include "common/StringCvt.h"
#include "common/StringTokenizer.h"
#include "ui/UiPropertySyncher.h"
#include "core/Preferences.h"
#include "core/Serna.h"

#include "GeneralPropertiesWidgetBase.hpp"

#include <QFontDialog>
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QRegExp>

using namespace Common;

static const char CFG_PACKAGE_DIR_PROP[] = NOTR("vars/config_package_dir");

static void fill_ui_lang_combo(const PropertyNode& appProps,
                               QComboBox& uiLangCombo);

class GeneralPropertiesWidget : public QWidget,
                                public Ui::GeneralPropertiesWidgetBase,
                                public Sui::PropertySyncherHolder {
    Q_OBJECT
public:
    GeneralPropertiesWidget(PropertyNode* props);

protected:
    virtual void            hintsToggled();

protected slots:
    virtual void            on_chooseSysFontButton__clicked();
    virtual void            on_spellDictCombo__activated(const QString& dict);
    virtual void            on_browseDocLocationButton__clicked();
    virtual void            on_hintsPersistent__clicked();
    virtual void            sysFontChanged();
    virtual void            on_resetToFactorySettings__clicked();
    virtual void            on_autoSaveCheckBox__toggled(bool isEnabled);
    virtual void            on_uiLangCombo__activated(const QString& lang);
    virtual void            on_browseCfgDirButton__clicked();
    virtual void            on_applyCfgPkgButton__clicked()
        { on_cfgPkgDirEdit__returnPressed(); }
    virtual void            on_cfgPkgDirEdit__returnPressed();

private:
    Common::PropertyNode*   hints_;
    Common::PropertyNode*   appProps_;
    Common::PropertyNode*   spellerProps_;
    QString                 cfgPkgDir_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(GeneralProperties, 0, true)

GeneralPropertiesWidget::GeneralPropertiesWidget(PropertyNode* props)
    : hints_(props->makeDescendant(DocLook::DOC_LOOK)->
        makeDescendant(DocLook::CONTEXT_HINTS)),
      appProps_(props->makeDescendant(App::APP)),
      spellerProps_(props->makeDescendant(Speller::SPELLER))
{
    using namespace Speller;

    setupUi(this);

    // speller combo init
    StringTokenizer st(
        spellerProps_->getSafeProperty(SPELLER_LANGUAGES)->getString());
    while (st)
        spellDictCombo_->insertItem(st.next());
    String default_dict =
        spellerProps_->getSafeProperty(SPELLER_DEFAULT_DICT)->getString();
    for (int i = 0; i < spellDictCombo_->count(); i++) {
        if (spellDictCombo_->itemText(i) == default_dict) {
            spellDictCombo_->setCurrentIndex(i);
            break;
        }
    }
    PropertyNode* auto_spc = spellerProps_->makeDescendant(SPELLER_AUTO, 
        "true", false);
    addSyncher(new Sui::ButtonSyncher(auto_spc, autoSpellCheckBox_));
    // end of speller combo init
    fill_ui_lang_combo(*appProps_, *uiLangCombo_);

    addSyncher(new Sui::LineEditSyncher(
        appProps_->makeDescendant(App::SYS_FONT), sysFontLineEdit_));
    addSyncher(new Sui::LineEditSyncher(
        appProps_->makeDescendant(App::DOC_PATH), docPathLineEdit_));
    PropertyNode* pn = appProps_->getProperty(App::AUTOSAVE_DELAY);
    if (0 == pn) {
        pn = appProps_->makeDescendant(App::AUTOSAVE_DELAY);
        pn->setInt(10);
    }
    addSyncher(new Sui::SpinnerSyncher(pn, autoSaveSpinBox_));
    pn = appProps_->getProperty(App::AUTOSAVE_ENABLED);
    if (0 == pn) {
        pn = appProps_->makeDescendant(App::AUTOSAVE_ENABLED);
        pn->setBool(false);
    }
    autoSaveCheckBox_->setChecked(pn->getBool());
    if (docPathLineEdit_->text().isEmpty())
        docPathLineEdit_->setText(DocSrcInfo::myDocumentsPath());
    addSyncher(new Sui::ButtonSyncher(
        hints_->makeDescendant(DocLook::HINTS_ENABLED), hintsEnabled_));
    addSyncher(new Sui::ButtonSyncher(
        hints_->makeDescendant(DocLook::HINTS_PERSISTENT), hintsPersistent_));
    addSyncher(new Sui::ButtonSyncher(
        hints_->makeDescendant(DocLook::HINTS_SHOW_PARENT), hintsParent_));
    addSyncher(new Sui::SpinnerSyncher(
        hints_->makeDescendant(DocLook::HINTS_DELAY), delaySpinner_));
    addSyncher(new Sui::ButtonSyncher(appProps_->makeDescendant(
        App::CLOSE_DOCKWINDOW_ON_ESCAPE), closeWhenEscape_));

    Sui::SelectorSyncher* sel = new Sui::SelectorSyncher(
        appProps_->makeDescendant(App::CRLF_LINE_BREAKS));
    sel->addSelector(NOTR("false"),  unixStyleRadioButton_);
    sel->addSelector(NOTR("true"),   win32StyleRadioButton_);
    addSyncher(sel);

#if !defined (WIN32) && !defined(__APPLE__)
    docPathLabel_->hide();
    docPathLineEdit_->hide();
    browseDocLocationButton_->hide();
#endif
    if (!FileUtils::has_cfg_package())
        cfgGroupBox_->hide();

    cfgPkgDirEdit_->setText(config().
        getProperty(CFG_PACKAGE_DIR_PROP)->getString());

    hintsToggled();
}

void GeneralPropertiesWidget::on_spellDictCombo__activated(const QString& dict)
{
    if (!dict.isEmpty())
        spellerProps_->makeDescendant(
            Speller::SPELLER_DEFAULT_DICT)->setString(dict);
}

void GeneralPropertiesWidget::on_autoSaveCheckBox__toggled(bool isEnabled)
{
    appProps_->makeDescendant(App::AUTOSAVE_ENABLED)->setBool(isEnabled);
}

void GeneralPropertiesWidget::on_chooseSysFontButton__clicked()
{
    PropertyNode* sys_font = appProps_->makeDescendant(App::SYS_FONT);
    QFont font;
    bool ok = true;
    QString font_str(sys_font->getString());
    if (font_str.isEmpty())
        font = QFontDialog::getFont(&ok, this);
    else {
        font.fromString(font_str);
        font = QFontDialog::getFont(&ok, font, this);
    }
    if (ok)
        sys_font->setString(font.toString());
}

void GeneralPropertiesWidget::on_browseDocLocationButton__clicked()
{
    String doc_path = appProps_->makeDescendant(App::DOC_PATH)->getString();
    doc_path =
        QFileDialog::getExistingDirectory(doc_path, this,
                                          NOTR("open file dialog"));
    if (!doc_path.isEmpty())
        appProps_->makeDescendant(App::DOC_PATH)->setString(doc_path);
}

void GeneralPropertiesWidget::hintsToggled()
{
    hintsPersistent_->setEnabled(hintsEnabled_->isChecked());
    bool delay_enabled =
        hintsPersistent_->isEnabled() && !hintsPersistent_->isChecked();
    delayLabel_->setEnabled(delay_enabled);
    delaySpinner_->setEnabled(delay_enabled);
    delaySuffix_->setEnabled(delay_enabled);
}

void GeneralPropertiesWidget::on_hintsPersistent__clicked()
{
    hintsToggled();
}

void GeneralPropertiesWidget::sysFontChanged()
{
}

void GeneralPropertiesWidget::on_browseCfgDirButton__clicked()
{
    QString tmp(QFileDialog::getExistingDirectory(cfgPkgDirEdit_->text(), this,
                                                  NOTR("open file dialog")));
    if (!tmp.isEmpty())
        cfgPkgDirEdit_->setText(tmp);
}

void GeneralPropertiesWidget::on_cfgPkgDirEdit__returnPressed()
{
    FileUtils::apply_pkgdir(cfgPkgDirEdit_->text(), this);
}

class ResetConfig;

void GeneralPropertiesWidget::on_resetToFactorySettings__clicked()
{
    SernaDoc* sd = dynamic_cast<SernaDoc*>(
        dynamic_cast<Serna*>(qApp)->firstChild()->firstChild());
    makeCommand<ResetConfig>()->execute(sd);
}

void GeneralPropertiesWidget::on_uiLangCombo__activated(const QString& lang)
{
    appProps_->makeDescendant(App::UI_LANG)->setString(lang);
}

static void fill_ui_lang_combo(const PropertyNode& appProps,
                               QComboBox& uiLangCombo)
{
    PathName transPath(get_translations_dir());
    QDir transDir(to_string<QString>(transPath.name()));
    if (!transDir.exists())
        return;
    transDir.setNameFilter(NOTR("serna_*.qm"));
    QStringList trList(transDir.entryList(QDir::Files));
    if (trList.isEmpty())
        return;
    QRegExp trRegexp(NOTR("serna_([^\\.]+)\\.qm"));
    QStringList::const_iterator it = trList.begin();
    uiLangCombo.insertItem(NOTR("default"));
    for (; it != trList.end(); ++it) {
        if (!trRegexp.exactMatch(*it))
            continue;
        uiLangCombo.insertItem(trRegexp.cap(1));
    }
    String curLang(appProps.getSafeProperty(App::UI_LANG)->getString());
    if (curLang.empty())
        curLang = NOTR("default");
    if (uiLangCombo.count())
        uiLangCombo.setCurrentText(curLang);
}

#include "moc/GeneralPropertiesWidget.moc"
