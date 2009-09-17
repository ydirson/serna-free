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

#include "SpellCheckPlainDialogBase.hpp"
#include "SpellCheckDialog.h"
#include "SpellerReactor.h"

#include "common/String.h"
#include "common/StringUtils.h"
#include "common/StringCvt.h"

#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"
#include "speller_debug.h"

#include <Qt>
#include <QString>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QApplication>

USING_COMMON_NS

static const char SD_NAME[] = NOTR("spell_check_dialog");

class SpellCheckPlainDialog : public QWidget,
                              private Ui::SpellCheckPlainDialogBase,
                              public SpellCheckDialog {
    Q_OBJECT
public:
    SpellCheckPlainDialog(QWidget* parent, SpellerReactor* reactor);
    ~SpellCheckPlainDialog();

protected slots:
    virtual void on_misspellEdit__textChanged(const QString& newText);
    // ignore()
    virtual void on_ignoreButton__clicked();
    // ignoreAll()
    virtual void on_ignoreAllButton__clicked();
    // add()
    virtual void on_addButton__clicked();
    // change()
    virtual void on_variantsList__itemSelectionChanged();
    //
    virtual void on_variantsList__currentItemChanged(QListWidgetItem* current,
                                                     QListWidgetItem* previous);
    //
    virtual void on_variantsList__itemActivated(QListWidgetItem* item);
    // change()
    virtual void on_changeButton__clicked();
    // changeAll()
    virtual void on_changeAllButton__clicked();
    // skipElement()
    virtual void on_skipElementButton__clicked();
    // setDict()
    virtual void on_languageCombo__activated(const QString& dict);
    virtual void help() const;
    virtual void grabFocus();
    void         focusStateChanged(bool);
    void         updateSuggestions(bool async = false);

protected:
    virtual void doSetMisspell(const Word& word);
    virtual void doClearSuggestions();
    virtual void doAddToSuggestions(const Word& word);
    virtual void doAddDict(const Word& dict);
    virtual void doSetDict(const Word& dict);
    virtual bool doExec() { show(); return true; }
    virtual QWidget* doGetWidget() { return this; }
    //!
private:
    bool                updateScheduled_;
    QStringList         suggestionsList_;
    QString             wordLabel_;
};

SpellCheckDialog* makeSpellCheckDialog(QWidget* parent,
                                       SpellerReactor* reactor,
                                       Sui::LiquidItem::Type)
{
    return new SpellCheckPlainDialog(parent, reactor);
}

void SpellCheckPlainDialog::grabFocus()
{
    activateWindow();
    misspellEdit_->setFocus();
}

SpellCheckPlainDialog::SpellCheckPlainDialog(QWidget* parent,
                                             SpellerReactor* reactor)
 :  QWidget(parent),
    SpellCheckDialog(reactor),
    updateScheduled_(false)
{
    DDBG << rbr(this) << "->SpellCheckDialog()" << std::endl;
    setupUi(this);
    setObjectName(SD_NAME);
    wordLabel_ = misspellLabel_->text();
    variantsList_->clear();
    languageCombo_->setDuplicatesEnabled(false);
    //ignoreButton_->setDefault(true);
}

SpellCheckPlainDialog::~SpellCheckPlainDialog()
{
    DDBG << rbr(this) << "->~SpellCheckDialog()" << std::endl;
}

inline const QString qcs(const Char* w, unsigned l) { return QString(w, l); }

inline const QString qcs(const ustring& s)
{
    return QString(s.data(), s.size());
}

void SpellCheckPlainDialog::help() const
{
    helpAssistant().show(DOCTAG(SPELLCHECKING));
}

void SpellCheckPlainDialog::doSetMisspell(const Word& word)
{
    QString qs(word.data(), word.size());
    misspellLabel_->setText(wordLabel_ + NOTR(" <b>") + qs + NOTR("<\b>"));
    if (!suggestionsList_.empty())
        qs = suggestionsList_.front();
    misspellEdit_->setText(qs);
}

void SpellCheckPlainDialog::doClearSuggestions()
{
    suggestionsList_.clear();
    updateSuggestions();
}
//!
void SpellCheckPlainDialog::doAddToSuggestions(const Word& word)
{
    suggestionsList_.push_back(qcs(word));
    updateSuggestions();
}
//!
void SpellCheckPlainDialog::doAddDict(const Word& lang_id)
{
    languageCombo_->addItem(qcs(lang_id));
}
//!
void SpellCheckPlainDialog::doSetDict(const Word& lang_id)
{
    int i = languageCombo_->findText(qcs(lang_id), Qt::MatchExactly);
    if (i >= 0)
        languageCombo_->setCurrentIndex(i);
}

void SpellCheckPlainDialog::updateSuggestions(bool async)
{
    if (!updateScheduled_) {
        updateScheduled_ = true;
        QMetaObject::invokeMethod(this, NOTR("updateSuggestions"),
                                  Qt::QueuedConnection, Q_ARG(bool, true));
    }
    else if (async) {
        bool saveBlockedState = variantsList_->blockSignals(true);
        variantsList_->clear();
        if (!suggestionsList_.empty())
            variantsList_->addItems(suggestionsList_);
        variantsList_->blockSignals(saveBlockedState);
        updateScheduled_ = false;
    }
}

////////////////// SpellCheckDialogBase slots ///////////////////////

void SpellCheckPlainDialog::on_misspellEdit__textChanged(const QString& newText)
{
    bool isChangeEnabled = newText != getMisspell();
    changeButton_->setEnabled(isChangeEnabled);
    changeAllButton_->setEnabled(isChangeEnabled);
}

void SpellCheckPlainDialog::on_ignoreButton__clicked()
{
    getReactor().ignore(getMisspell());
}

void SpellCheckPlainDialog::on_ignoreAllButton__clicked()
{
    getReactor().ignoreAll(getMisspell());
}

void SpellCheckPlainDialog::on_addButton__clicked()
{
    getReactor().add(getMisspell());
}

void SpellCheckPlainDialog::on_variantsList__itemSelectionChanged()
{
}

void
SpellCheckPlainDialog::on_variantsList__currentItemChanged(QListWidgetItem* cur,
                                                     QListWidgetItem*)
{
    misspellEdit_->setText(cur->text());
}

static void replace(SpellerReactor& reactor,
                    const SpellCheckDialog::Word& what, const QString& repl)
{
    reactor.change(what, repl);
}

void
SpellCheckPlainDialog::on_variantsList__itemActivated(QListWidgetItem* item)
{
    misspellEdit_->setText(item->text());
    on_changeButton__clicked();
}

void SpellCheckPlainDialog::on_changeButton__clicked()
{
    replace(getReactor(), getMisspell(), misspellEdit_->text());
}

void SpellCheckPlainDialog::on_changeAllButton__clicked()
{
    QString repl(misspellEdit_->text());
    getReactor().changeAll(getMisspell(), repl);
}

void SpellCheckPlainDialog::on_languageCombo__activated(const QString& lang_id)
{
    getReactor().setDict(lang_id);
}

void SpellCheckPlainDialog::on_skipElementButton__clicked()
{
    getReactor().skipElement();
}

void SpellCheckPlainDialog::focusStateChanged(bool isFocusIn)
{
    if (isFocusIn) 
        misspellEdit_->setFocus();
}

#include "moc/SpellCheckPlainDialog.moc"
