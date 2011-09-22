// 
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
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

#include "utils/Config.h"
#include "utils/Properties.h"
#include "common/StringTokenizer.h"
#include "ui/UiPropertySyncher.h"
#include "SpellChecker.h"
#include "SpellerLibrary.h"

#include "SpellCheckerSettingsDialogBase.hpp"
#include "AddWordsDialogBase.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QTimer>
#include <iostream>

using namespace Common;

class SpellCheckerSettingsDialog : public QDialog,
                                   public Ui::SpellCheckerSettingsDialog,
                                   public Sui::PropertySyncherHolder {
    Q_OBJECT
public:
    SpellCheckerSettingsDialog();

protected slots:
    virtual void on_buttonBox__rejected();
    virtual void on_buttonBox__accepted();
    virtual void on_dictCombo1__activated(const QString&);
    virtual void on_dictCombo2__activated(const QString&);
    virtual void on_wordList__itemSelectionChanged();
    virtual void on_addButton__clicked();
    virtual void on_removeButton__clicked();
    virtual void on_selectAllButton__clicked() { select_items(true); }
    virtual void on_deselectAllButton__clicked() { select_items(false); }
    virtual void on_importButton__clicked();

private:
    void            fill_dict_combo(QComboBox*);
    void            init_settings_tab();
    void            init_pwl_tab();
    void            refill_pwl_list(const String& lang,
                                    const SpellChecker::WordSet* = 0);
    void            do_update();
    void            select_items(bool select);

    PropertyNode*   props_;
    PropertyNode*   spp_;
    bool            modified_;
    bool            needsRecheck_;
};

class AddWordsDialog : public QDialog, public Ui::AddWordsDialog {
    Q_OBJECT
public:
    AddWordsDialog(QWidget* parent, String& result)
        : QDialog(parent), result_(result) 
    {
        setupUi(this);
        QTimer::singleShot(0, textEdit_, SLOT(setFocus()));
    }
public slots:
    virtual void accept() 
    { 
        result_ = textEdit_->toPlainText(); 
        QDialog::accept();
    }
private:
    String& result_;
};

void SpellCheckerSettingsDialog::on_buttonBox__accepted()
{
    do_update(); 
    if (needsRecheck_) 
        accept();
    else
        reject();
}

void SpellCheckerSettingsDialog::on_buttonBox__rejected()
{
    reject();
}

void SpellCheckerSettingsDialog::fill_dict_combo(QComboBox* combo)
{
    StringTokenizer st(props_->getString(Speller::SPELLER_LANGUAGES));
    String default_dict(props_->getString(Speller::SPELLER_DEFAULT_DICT));
    while (st)
        combo->addItem(QString(st.next()));
    for (int i = 0; i < combo->count(); i++) {
        if (combo->itemText(i) == default_dict) {
            combo->setCurrentIndex(i);
            break;
        }
    }
}

static QTreeWidgetItem* 
add_item(QTreeWidgetItem* parent, QString s1, QString s2 = QString())
{
    QStringList sl;
    sl << s1 << s2;
    return new QTreeWidgetItem(parent, sl);
}

SpellCheckerSettingsDialog::SpellCheckerSettingsDialog()
    : QDialog(qApp->activeWindow()),
      props_(config().root()->getProperty(Speller::SPELLER)),
      modified_(false),
      needsRecheck_(false)
{
    setupUi(this);
    if (!props_)
        return;
    spp_ = props_->getProperty(props_->getString("use"));
    if (!spp_)
        return;
    init_settings_tab();
    init_pwl_tab();
}

typedef QList<QListWidgetItem*> ListItems;

void SpellCheckerSettingsDialog::select_items(bool select)
{
    for (int i = 0; i < wordList_->count(); i++) 
        wordList_->item(i)->setSelected(select);
}

void SpellCheckerSettingsDialog::on_dictCombo1__activated(const QString& dict)
{
    do_update();
    refill_pwl_list(dict);
}

void SpellCheckerSettingsDialog::on_wordList__itemSelectionChanged()
{
    bool has_items = !wordList_->selectedItems().empty();
    removeButton_->setEnabled(has_items);
    deselectAllButton_->setEnabled(has_items);
}

void SpellCheckerSettingsDialog::refill_pwl_list(const String& lang,
                                            const SpellChecker::WordSet* src)
{
    wordList_->clear();
    SpellChecker* chk = SpellerLibrary::instance()->getSpellChecker(lang);
    if (!chk)
        return;
    const SpellChecker::WordSet& si = src ? *src : chk->getPwl();    
    SpellChecker::WordSet::const_iterator it = si.begin();
    for (; it != si.end(); ++it)
        wordList_->addItem(*it);
    on_wordList__itemSelectionChanged();
}

void SpellCheckerSettingsDialog::do_update()
{
    if (!modified_)
        return;
    modified_ = false;
    needsRecheck_ = true;
    SpellChecker* chk = SpellerLibrary::instance()->
        getSpellChecker(dictCombo1_->currentText());
    if (!chk)
        return;
    OwnerPtr<SpellChecker::WordSet> new_pwl(new SpellChecker::WordSet);
    for (int i = 0; i < wordList_->count(); ++i)
        new_pwl->insert(wordList_->item(i)->text());
    chk->resetPwl(*new_pwl);
    chk->setPwl(new_pwl.release());
    chk->savePwl();
}

void SpellCheckerSettingsDialog::on_addButton__clicked()
{
    String words;
    AddWordsDialog awd(this, words);
    if (awd.exec() != QDialog::Accepted)
        return;
    SpellChecker::WordSet pwl;
    for (int i = 0; i < wordList_->count(); ++i)
        pwl.insert(wordList_->item(i)->text());
    StringTokenizer st(words, " \t.,=;:\n\"'");
    while (st) {
        String tok = st.next();
        if (tok.length() > 1)
            pwl.insert(tok);
    }
    refill_pwl_list(dictCombo1_->currentText(), &pwl);
    modified_ = true;
}

void SpellCheckerSettingsDialog::on_removeButton__clicked()
{
    ListItems items(wordList_->selectedItems());
    wordList_->clearSelection();
    ListItems::const_iterator it = items.begin();
    modified_ = true;
    for (; it != items.end(); ++it) 
        delete *it;
    selectAllButton_->setEnabled(wordList_->count());
}

void SpellCheckerSettingsDialog::on_importButton__clicked()
{   
    String lang(dictCombo1_->currentText());
    SpellChecker* chk = SpellerLibrary::instance()->getSpellChecker(lang);
    if (!chk)
        return;
    QString ifile(QFileDialog::getOpenFileName(qApp->activeWindow(),
        tr("Select word list file to import"),
        config().getConfigDir(), "Text Files (*.txt *.pws);;All Files (*)"));
    if (ifile.isEmpty())
        return;
    OwnerPtr<SpellChecker::WordSet> wset(new SpellChecker::WordSet);
    if (!chk->loadPwl(&*wset))
        return;
    for (int i = 0; i < wordList_->count(); ++i)
        wset->insert(wordList_->item(i)->text());
    refill_pwl_list(lang, &*wset);
    selectAllButton_->setEnabled(wordList_->count());
    modified_ = true;
}

void SpellCheckerSettingsDialog::init_pwl_tab()
{
    fill_dict_combo(dictCombo1_);
    refill_pwl_list(props_->getString(Speller::SPELLER_DEFAULT_DICT));
    selectAllButton_->setEnabled(wordList_->count());
}

void SpellCheckerSettingsDialog::init_settings_tab()
{
    fill_dict_combo(dictCombo2_);
    PropertyNode* auto_spc = props_->makeDescendant(Speller::SPELLER_AUTO, 
        "true", false);
    addSyncher(new Sui::ButtonSyncher(auto_spc, autoSpellCheck_));
    QStringList sl1;
    sl1 << "Spell Checker";
    sl1 << QString(props_->getString("use"));
    QTreeWidgetItem* root_item = new QTreeWidgetItem(sl1);
    propList_->addTopLevelItem(root_item);
    sl1.clear();
    sl1 << tr("Personal word list folder");
    sl1 << QString(props_->getString("pws-dir"));
    propList_->addTopLevelItem(new QTreeWidgetItem(sl1));
    add_item(root_item, tr("Speller DLL"), spp_->getString("lib"));
    root_item->setExpanded(true);
    const PropertyNode* pn = 0;
    QTreeWidgetItem* dict_folds = add_item(root_item, tr("Dictionary folders"));
    for (pn = spp_->firstChild(); pn; pn = pn->nextSibling()) {
        if (NOTR("dict-dir") != pn->name()) 
            continue;
        add_item(dict_folds, pn->getString())->setFirstColumnSpanned(true);
    }
    dict_folds->setExpanded(true);
    pn = spp_->getProperty("aliases");
    if (pn) {
        QTreeWidgetItem* alias_item = add_item(root_item, 
            tr("Language code aliases"));
        for (pn = pn->firstChild(); pn; pn = pn->nextSibling())
            add_item(alias_item, pn->name(), pn->getString());
        alias_item->setExpanded(true);
    }
    propList_->header()->setResizeMode(0, QHeaderView::ResizeToContents);
}

void SpellCheckerSettingsDialog::on_dictCombo2__activated(const QString& dict)
{
    if (!dict.isEmpty())
        props_->makeDescendant(Speller::SPELLER_DEFAULT_DICT)->setString(dict);
}

bool settings_dialog()
{
    return SpellCheckerSettingsDialog().exec() == QDialog::Accepted;
}

#include "moc/SpellCheckerSettingsDialog.moc"
