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
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef FIND_AND_REPLACE_DIALOG_H
#define FIND_AND_REPLACE_DIALOG_H

#include "docview/Finder.h"
#include "docview/FindDialogTabBase.hpp"
#include "docview/FindToolTabBase.hpp"
#include "docview/FindMiniToolBase.hpp"
#include "ui/UiPropertySyncher.h"

#include "utils/IdleHandler.h"

#include "common/PropertyTree.h"

template <class T> void find_text_changed(T* tab)
{
    tab->findButton_->setEnabled(!tab->findLineEdit_->text().isEmpty());
    tab->replaceButton_->setEnabled(tab->findButton_->isEnabled());
    tab->replaceAllButton_->setEnabled(tab->findButton_->isEnabled());
}

template <class T> void set_focus_state(T* tab, bool v)
{
    if (v)
        qt_single_shot_timer(tab->findLineEdit_, SLOT(selectAll()));
    else
        qt_single_shot_timer(tab, SLOT(deselect()));
}

class FindDialogTab : public QWidget,
                      public Ui::FindDialogTabBase,
                      public Sui::PropertySyncherHolder {
    Q_OBJECT

public slots:
    void   on_closeButton__clicked()      { finder_->removeItem(); }
    void   on_findButton__clicked()       { finder_->find(); }
    void   on_replaceButton__clicked()    { finder_->replace(); }
    void   on_replaceAllButton__clicked() { finder_->replaceAll(); }
    void   on_findLineEdit__returnPressed() { finder_->find(); }
    void   on_replaceLineEdit__returnPressed() { finder_->replace(); }
    void   setFocusState(bool v)          { set_focus_state(this, v); }
    void   on_findLineEdit__textChanged(const QString&)
        { find_text_changed(this); }
    void   deselect() { findLineEdit_->deselect(); }

public:
    FindDialogTab(Finder* finder, bool isStruct, bool repl);

private:
    Finder* finder_;
};

class FindToolTab : public QWidget,
                    public Ui::FindToolTabBase,
                    public Sui::PropertySyncherHolder {
    Q_OBJECT

public slots:
    void   on_findButton__clicked()       { finder_->find(); }
    void   on_replaceButton__clicked()    { finder_->replace(); }
    void   on_replaceAllButton__clicked() { finder_->replaceAll(); }
    void   on_findLineEdit__returnPressed() { finder_->find(); }
    void   on_replaceLineEdit__returnPressed() { finder_->replace(); }
    void   setFocusState(bool v)          { set_focus_state(this, v); }
    void   on_findLineEdit__textChanged(const QString&)
        { find_text_changed(this); }
public:
    FindToolTab(Finder* finder, bool isStruct, bool repl);

private:
    Finder* finder_;
};

namespace Find {

extern const char* find_mode_names[];
bool is_matchcase_disabled(const Common::String&);

} // namespace Find

class QMainWindow;

class FindDialogDataBase : public QWidget {
    Q_OBJECT
public:
    FindDialogDataBase(QWidget* parent);
    virtual ~FindDialogDataBase();

public slots:
    virtual void    focusStateChanged(bool) = 0;
    virtual void    modeChanged(int) = 0;
    virtual void    tabChanged() = 0;
    virtual void    grabFocus() = 0;
protected:
    virtual bool    eventFilter(QObject* obj, QEvent* event);
};

template <class TAB> class FindDialogData : public FindDialogDataBase {
public:
    FindDialogData(QWidget* parent)
        : FindDialogDataBase(parent),
          firstShow_(1) {}

    TAB* currentTab() const
    {
        return static_cast<TAB*>(tabWidget_->currentPage());
    }
    virtual void    grabFocus()
    {
        if (0 == finder_->findProps())
            return;
        showPage(finder_->findProps()->
                 getSafeProperty(Find::REPLACE_MODE)->getBool());
    }
    void showPage(int idx)
    {
        tabWidget_->showPage(tabWidget_->page(idx));
        currentTab()->findLineEdit_->setFocus();
    }
    virtual void    focusStateChanged(bool v)
    {
        static_cast<TAB*>(tabWidget_->currentPage())->setFocusState(v);
    }
    virtual void modeChanged(int i)
    {
        if (0 == finder_->findProps())
            return;
        finder_->findProps()->
            makeDescendant(Find::SEARCH_TYPE)->
                setString(Find::find_mode_names[i]);
        currentTab()->searchIn_->setCurrentItem(i);
        if (Find::is_matchcase_disabled(Find::find_mode_names[i])) {
            currentTab()->matchCase_->setEnabled(false);
            finder_->findProps()->makeDescendant(Find::MATCH_CASE)->
                setBool(false);
        } else
            currentTab()->matchCase_->setEnabled(true);
    }
    virtual void tabChanged()
    {
        focusStateChanged(true);
        bool isReplace = !!tabWidget_->currentPageIndex();
        int idx = isReplace ?
            findTab_->searchIn_->currentItem() :
            replaceTab_->searchIn_->currentItem();
        if (isReplace && idx >= replaceTab_->searchIn_->count())
            idx = 0;
        if (finder_->findProps())
            finder_->findProps()->makeDescendant
                (Find::REPLACE_MODE)->setBool(isReplace);
        if (!firstShow_ && currentTab()->searchIn_->currentItem() != idx)
            modeChanged(idx);
        firstShow_ = 0;
    }

    QTabWidget*     tabWidget_;
    TAB*            findTab_;
    TAB*            replaceTab_;
    Finder*         finder_;
    bool            firstShow_;
};

typedef FindDialogData<FindDialogTab> FindDialogTabData;
typedef FindDialogData<FindToolTab>   FindToolTabData;

class FindAndReplaceDialog : public FindDialogTabData {
    Q_OBJECT
public:
    FindAndReplaceDialog(QWidget* parent, Finder* finder, bool isStruct);

    //virtual void escapePressed() { finder_->removeItem(); }

public slots:
    virtual void    help() const;
};

class FindAndReplaceTool : public FindToolTabData {
    Q_OBJECT
public:
    FindAndReplaceTool(QWidget* parent, Finder* finder, bool isStruct);

    virtual void    focusStateChanged(bool);

public slots:
    virtual void    help() const;
};

class FindAndReplaceMiniTool : public QWidget,
                               public Ui::FindMiniToolBase,
                               public Sui::PropertySyncherHolder,
                               public Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    FindAndReplaceMiniTool(QWidget* parent, Finder* finder, bool isStruct);

    virtual void    grabFocus();
    virtual void    propertyChanged(Common::PropertyNode* property);

public slots:
    virtual void    focusStateChanged(bool);
    void            on_findButton__clicked()       { finder_->find(); }
    void            on_replaceButton__clicked()    { finder_->replace(); }
    void            on_replaceAllButton__clicked() { finder_->replaceAll(); }
    void   on_findLineEdit__returnPressed() { finder_->find(); }
    void   on_replaceLineEdit__returnPressed() { finder_->replace(); }
    void            setFocusState(bool v) { set_focus_state(this, v); }

    virtual void    help() const;
    void            on_findLineEdit__textChanged(const QString&)
        { findTextChanged(); }
    void            on_replaceLineEdit__textChanged(const QString&)
        { findTextChanged(); }

private:
    void            findTextChanged();
    Finder* finder_;
    Sui::PixmapSyncher* modeSyncher_;
};

#endif  // FIND_AND_REPLACE_DIALOG_H
