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
#include "docview/impl/debug_dv.h"
#include "docview/qt/FindAndReplace.h"

#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include "ui/UiItem.h"
#include "ui/IconProvider.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QAbstractButton>
#include <QKeyEvent>
#include <QEvent>
#include <QMainWindow>
#include <QKeySequence>

using namespace Common;
using namespace Find;

///////////////////////////////////////////////////////////////////////////

QWidget* Finder::makeWidget(QWidget* parent, Type type)
{
    if (HORIZONTAL_TOOL == type)
        return new FindAndReplaceMiniTool(parent, this, isStruct_);
    else
        if (VERTICAL_TOOL == type)
            return new FindAndReplaceTool(parent, this, isStruct_);
    return new FindAndReplaceDialog(parent, this, isStruct_);
}

static QString mode_annotation(const PropertyNode* props)
{
    String s = props->getSafeProperty(SEARCH_TYPE)->getString();
    if (ATTRVAL_TYPE == s)
        return QObject::tr("attribute value");
    if (ATTRNAME_TYPE == s)
        return QObject::tr("attribute name");
    if (COMMENT_TYPE == s)
        return QObject::tr("comment");
    if (PATTERN_TYPE == s)
        return QObject::tr("XSLT pattern");
    return QObject::tr("text");
}

bool Finder::showNotFound(bool start) const
{
    if (0 == findProps())
        return false;
    if (!start) {
        QMessageBox::warning(
            widget(0)->parentWidget(),
            tr("Search pattern not found"),
            tr("Could not find %1 \"%2\"").
                arg(mode_annotation(findProps())).
                arg(findProps()->getString(FIND_TEXT)));
        return false;
    }
    return (QMessageBox::Ok == QMessageBox::warning(widget(0)->parentWidget(),
            tr("Search pattern not found"),
            tr("Could not find %1 \"%2\". Start from the beginning?").
                arg(mode_annotation(findProps())).
                arg(findProps()->getString(FIND_TEXT)),
        QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel));
}

bool Finder::showSearchFinished(bool start) const
{
    if (!start) {
        QMessageBox::warning(
            widget(0)->parentWidget(), tr("Search Finished"),
                tr("Search finished."));
        return false;
    }
    return (QMessageBox::Ok == QMessageBox::warning(widget(0)->parentWidget(),
        tr("Search Finished"),
        tr("Search finished. Start from the beginning?"),
        QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel));
}

void Finder::showChanged(int n)
{
    QMessageBox::information(widget(0)->parentWidget(),
                             tr("Replace completed"),
                             tr("Replace completed: %1"
                                " occurences changed").arg(QString::number(n)));
}

//////////////////////////////////////////////////////////////////////

namespace Find {

const char* find_mode_names[] = {
    TEXT_TYPE,
    ATTRVAL_TYPE,
    ATTRNAME_TYPE,
    COMMENT_TYPE,
    PATTERN_TYPE,
    0
};

const char* mode_annotations[] = {
    QT_TRANSLATE_NOOP("FindDialog", "Text"),
    QT_TRANSLATE_NOOP("FindDialog", "Attribute Values"),
    QT_TRANSLATE_NOOP("FindDialog", "Attribute Names"),
    QT_TRANSLATE_NOOP("FindDialog", "Comments"),
    QT_TRANSLATE_NOOP("FindDialog", "XSLT Pattern"),
    0
};

// START_IGNORE_LITERALS
const char* mode_icons[] = {
    "find_text_16",
    "find_attribute_value_16",
    "find_attribute_name_16",
    "comment",
    "find_pattern_16",
    0
};
// STOP_IGNORE_LITERALS

const char* replace_disabled_modes[] = {
    PATTERN_TYPE,
    0
};

bool is_replace_disabled(const String& mode)
{
    for (int i = 0; replace_disabled_modes[i]; ++i)
        if (mode == replace_disabled_modes[i])
            return true;
    return false;
}

bool is_matchcase_disabled(const String& mode)
{
    return is_replace_disabled(mode);
}

} // namespace Find

template <class TAB> void init_tab(TAB* tab, bool isStruct, bool isReplace)
{
    tab->setupUi(tab);
    tab->addSyncher(FIND_TEXT,         tab->findLineEdit_);
    tab->addSyncher(REPLACE_TEXT,      tab->replaceLineEdit_);
    tab->addSyncher(MATCH_CASE,        tab->matchCase_);
    tab->addSyncher(SEARCH_BACKWARDS,  tab->searchBackwards_);
    if (isStruct) {
        for (int i = 0; mode_annotations[i]; ++i) {
            if (isReplace && is_replace_disabled(find_mode_names[i]))
                continue;
            tab->searchIn_->insertItem(
                Sui::icon_provider().getPixmap(mode_icons[i]),
                qApp->translate("FindDialog", mode_annotations[i]));
        }
    }
    else {
        tab->searchIn_->hide();
        tab->searchInLabel_->hide();
    }
    if (!isReplace) {
        tab->replaceLabel_->setPaletteForegroundColor(
            tab->replaceLabel_->paletteBackgroundColor());
        tab->replaceLineEdit_->hide();
        tab->replaceButton_->hide();
        tab->replaceAllButton_->hide();
    }
    tab->setFocusProxy(tab->findLineEdit_);
    tab->findButton_->setAutoDefault(false);
    tab->findButton_->setDefault(true);
}

FindDialogTab::FindDialogTab(Finder* finder,
                             bool isStruct,
                             bool isReplace)
    : QWidget(0, NOTR("FindDialogTabWidget")),
      PropertySyncherHolder(finder->findProps()),
      finder_(finder)
{
    init_tab<FindDialogTab>(this, isStruct, isReplace);
}

FindToolTab::FindToolTab(Finder* finder,
                         bool isStruct,
                         bool isReplace)
    : QWidget(0, NOTR("FindToolTabWidget")),
      PropertySyncherHolder(finder->findProps()),
      finder_(finder)
{
    init_tab<FindToolTab>(this, isStruct, isReplace);
}

/////////////////////////////////////////////////////////////////

template <class DIALOG, class TAB>
void init_dialog(DIALOG* dlg, Finder* finder, bool isStruct)
{
    dlg->finder_ = finder;
    dlg->tabWidget_  = new QTabWidget(dlg);
    QGridLayout* layout = new QGridLayout(dlg);
    layout->setMargin(2);
    layout->addWidget(dlg->tabWidget_, 1, 1);
    dlg->findTab_    = new TAB(finder, isStruct, false);
    dlg->replaceTab_ = new TAB(finder, isStruct, true);
    dlg->tabWidget_->addTab(dlg->findTab_,
                            qApp->translate("Finder", "Find"));
    dlg->tabWidget_->addTab(dlg->replaceTab_,
                            qApp->translate("Finder", "Find And Replace"));
    dlg->connect(dlg->findTab_->searchIn_, SIGNAL(activated(int)),
        dlg, SLOT(modeChanged(int)));
    dlg->connect(dlg->replaceTab_->searchIn_, SIGNAL(activated(int)),
        dlg, SLOT(modeChanged(int)));
    dlg->grabFocus();
    if (finder->findProps()) {
        String mode = finder->findProps()->
            getSafeProperty(SEARCH_TYPE)->getString();
        uint i = 0;
        for (; find_mode_names[i]; ++i) {
            if (mode == find_mode_names[i]) {
                dlg->modeChanged(i);
                break;
            }
        }
        if (!find_mode_names[i])
            dlg->modeChanged(0);
    }
    QObject::connect(dlg->tabWidget_, SIGNAL(currentChanged(QWidget*)),
        dlg, SLOT(tabChanged()));
}

/////////////////////////////////////////////////////////////////

FindAndReplaceDialog::FindAndReplaceDialog(QWidget* parent, Finder* finder,
                                           bool isStruct)
    : FindDialogTabData(parent)
{
    init_dialog<FindAndReplaceDialog, FindDialogTab>(this, finder, isStruct);
    if (0 == finder->findProps())
        Sui::disable_widget(this);
}

void FindAndReplaceDialog::help() const
{
    helpAssistant().show(DOCTAG(FIND_REPLACE_D));
}

/////////////////////////////////////////////////////////////////

FindAndReplaceTool::FindAndReplaceTool(QWidget* parent, Finder* finder,
                                       bool isStruct)
    : FindToolTabData(parent)
{
    init_dialog<FindAndReplaceTool, FindToolTab>(this, finder, isStruct);
    if (0 == finder->findProps())
        Sui::disable_widget(this);
}

void FindAndReplaceTool::focusStateChanged(bool v)
{
    Sui::set_widget_focus_color(this, v);
    Sui::set_widget_focus_color(tabWidget_, 0);
    FindToolTabData::focusStateChanged(v);
}

void FindAndReplaceTool::help() const
{
    helpAssistant().show(DOCTAG(FIND_REPLACE_D));
}

///////////////////////////////////////////////////////////////////////

FindAndReplaceMiniTool::FindAndReplaceMiniTool(QWidget* parent, Finder* finder,
                                               bool isStruct)
    : QWidget(parent),
      PropertySyncherHolder(finder->findProps()),
      finder_(finder)
{
    setupUi(this);
    PropertyNode* finderProps = finder->findProps();
    if (0 == finderProps) {
        disable_widget(this);
        return;
    }
    setFocusProxy(findLineEdit_);
    addSyncher(FIND_TEXT,    findLineEdit_);
    addSyncher(REPLACE_TEXT, replaceLineEdit_);
    modeSyncher_ = new Sui::PixmapSyncher
        (finderProps->makeDescendant(SEARCH_TYPE), textAttrsLabel_);
    addSyncher(modeSyncher_);
    modeSyncher_->addState(TEXT_TYPE, NOTR("find_text"),
        tr("<qt><center><nobr><b>Search mode: text.</b></nobr><br/>"
           "<nobr>Click to change search mode.</nobr></center></qt>"));
    if (isStruct) {
        modeSyncher_->addState(ATTRVAL_TYPE, NOTR("find_attributes"),
            tr("<qt><center><nobr><b>Search mode: attribute values.</b>"
               "</nobr><br/>Click to change search mode.</center></qt>"));
        modeSyncher_->addState(ATTRNAME_TYPE, NOTR("find_attribute_name"),
            tr("<qt><center><nobr><b>Search mode: attribute names.</b>"
               "</nobr><br/>Click to change search mode.</center></qt>"));
        modeSyncher_->addState(COMMENT_TYPE, NOTR("find_comments"),
            tr("<qt><center><nobr><b>Search mode: XML Comments.</b>"
               "</nobr><br/>Click to change search mode.</center></qt>"));
        modeSyncher_->addState(PATTERN_TYPE, NOTR("find_pattern"),
            tr("<qt><center><nobr><b>Search mode: XSLT Pattern.</b>"
               "</nobr><br/>Click to change search mode.</center></qt>"));
    }
    addSyncher(new Sui::PixmapSyncher(
        finderProps->makeDescendant(SEARCH_BACKWARDS, NOTR("false"), false),
        searchDirectionLabel_,
        NOTR("find_backwards"),
        tr("<qt><center><nobr><b>Search direction: backwards.</b>"
           "</nobr><br/><nobr>Click to search forward.</nobr></center></qt>"),
        NOTR("find_forward"),
        tr("<qt><center><nobr><b>Search direction: forward.</b>"
        "</nobr><br/><nobr>Click to search backwards.</nobr></center></qt>")));
    addSyncher(new Sui::PixmapSyncher(
        finderProps->makeDescendant(MATCH_CASE, NOTR("false"), false),
        matchCaseLabel_,
        NOTR("find_match_case"),
        tr("<qt><center><nobr><b>Case-sensitive search.</b>"
        "</nobr><br/><nobr>Click to ignore case.</nobr></center></qt>"),
        NOTR("find_no_match_case"),
        tr("<qt><center><nobr><b>Case-insensitive search.</b>"
        "</nobr><br/><nobr>Click to match case.</nobr></center></qt>")));
    addSyncher(new Sui::PixmapSyncher(
        finderProps->makeDescendant(REPLACE_MODE),
        replaceLabel_,
        NOTR("find_no_replace"), tr("Click to hide replace options"),
        NOTR("find_and_replace"), tr("Click to show replace options")));
    finderProps->getProperty(REPLACE_MODE)->addWatcher(this);
    finderProps->getProperty(SEARCH_TYPE)->addWatcher(this);
    modeSyncher_->update();
    propertyChanged(finderProps->getProperty(SEARCH_TYPE));
    propertyChanged(finderProps->getProperty(REPLACE_MODE));

    grabFocus();
}

void FindAndReplaceMiniTool::help() const
{
    helpAssistant().show(DOCTAG(FIND_REPLACE_D));
}

void FindAndReplaceMiniTool::grabFocus()
{
    findLineEdit_->setFocus();
}

void FindAndReplaceMiniTool::findTextChanged()
{
    findButton_->setEnabled(!findLineEdit_->text().isEmpty());
    replaceButton_->setEnabled(findButton_->isEnabled());
}

void FindAndReplaceMiniTool::propertyChanged(PropertyNode* pn)
{
    if (0 == finder_->findProps())
        return;
    PropertyNode* replaceProp =
        finder_->findProps()->getProperty(REPLACE_MODE);
    PropertyNode* modeProp =
        finder_->findProps()->getProperty(SEARCH_TYPE);
    if (pn == replaceProp) {
        String prevReplaceMode = modeProp->getString();
        modeSyncher_->enableState(PATTERN_TYPE, !pn->getBool());
        replaceLineEdit_->setVisible(pn->getBool());
        replaceButton_->setVisible(pn->getBool());
        if (is_replace_disabled(prevReplaceMode) && pn->getBool())
            modeProp->setString(TEXT_TYPE);
        return;
    }
    if (is_matchcase_disabled(modeProp->getString())) {
        finder_->findProps()->getProperty(MATCH_CASE)->setBool(false);
        matchCaseLabel_->setEnabled(false);
    }
    else
        matchCaseLabel_->setEnabled(true);
    if (is_replace_disabled(modeProp->getString())) {
        replaceLabel_->setEnabled(false);
        replaceProp->setBool(false);
    }
    else
        replaceLabel_->setEnabled(true);
}

void FindAndReplaceMiniTool::focusStateChanged(bool v)
{
    Sui::set_widget_focus_color(this, v);
    Sui::set_widget_focus_color(findButton_, v);
    Sui::set_widget_focus_color(replaceButton_, v);
    set_focus_state(this, v);
}

FindDialogDataBase::FindDialogDataBase(QWidget* parent)
 :  QWidget(parent)
{
    qApp->installEventFilter(this);
}

FindDialogDataBase::~FindDialogDataBase()
{
    qApp->removeEventFilter(this);
}

bool FindDialogDataBase::eventFilter(QObject* o, QEvent* e)
{
    if (!e || QEvent::Shortcut != e->type() || !isActiveWindow())
        return QObject::eventFilter(o, e);

    const QShortcutEvent* scEvent = static_cast<QShortcutEvent*>(e);
    if (!scEvent->isAmbiguous())
        return QObject::eventFilter(o, e);

    QList<QAbstractButton*> absButtons(findChildren<QAbstractButton*>());
    if (absButtons.isEmpty())
        return QObject::eventFilter(o, e);

    QList<QAbstractButton*>::const_iterator it = absButtons.begin();
    const QKeySequence& keySequenceFromEvent = scEvent->key();
    for (; absButtons.end() != it; ++it) {
        QAbstractButton* absBtn = *it;
        const QKeySequence ks(absBtn->shortcut());
        if (ks.isEmpty() || ks != keySequenceFromEvent)
            continue;
        e->accept();
        QMetaObject::invokeMethod(absBtn, NOTR("click"), Qt::QueuedConnection);
        return true;
    }

    return QObject::eventFilter(o, e);
}
