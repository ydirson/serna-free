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
#include "docview/SernaDoc.h"
#include "docview/dv_utils.h"
#include "BrowseButtonMenu.h"

#include "common/Url.h"
#include "ui/IconProvider.h"

#include "utils/Config.h"
#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include <QFileDialog>
#include <QPushButton>
#include <QToolButton>
#include <QAction>

using namespace Common;

BrowseButtonMenu::BrowseButtonMenu(SernaDoc* doc,
                                   QAbstractButton* button,
                                   const String& baseUrl, bool isSaveUrl)
    : isSaveUrl_(isSaveUrl),
      doc_(doc),
      button_(button),
      baseUrl_(baseUrl),
      caption_(tr("Choose a file")),
      filters_(tr("XML files (*.xml);;All files (*)"))
{
    button_->disconnect(SIGNAL(clicked()));
    const PropertyNode* enabled_protocols =
        config().getProperty("dav/#protocols");
    if (enabled_protocols && enabled_protocols->countChildren() > 1) {
        for (PropertyNode* protocol = enabled_protocols->firstChild();
             protocol; protocol = protocol->nextSibling()) {
            //! Instantiate menu item
            if (browseMenu_.isNull())
                browseMenu_ = new QMenu(button);
            String icon = protocol->getSafeProperty(Sui::ICON)->getString();
            QAction* action = new QAction(this);
            action->setText(
                protocol->getSafeProperty("browse-inscription")->getString());
            action->setIcon(Sui::icon_provider().getIconSet(icon));
            action->setData(QVariant::fromValue((void*)protocol));
            browseMenu_->addAction(action);
            if (!browseMenu_->defaultAction())
                browseMenu_->setDefaultAction(action);
        }
        connect(browseMenu_, SIGNAL(triggered(QAction*)),
            this, SLOT(browse(QAction*)));
        if (button->inherits(NOTR("QPushButton"))) {
            static_cast<QPushButton*>(button)->setPopup(browseMenu_);
        }
        else if (button->inherits(NOTR("QToolButton"))) {
            static_cast<QToolButton*>(button)->setPopup(browseMenu_);
            static_cast<QToolButton*>(button)->setPopupDelay(0);
        }
    } else
        connect(button_, SIGNAL(clicked()), this, SLOT(browse()));
}

BrowseButtonMenu::~BrowseButtonMenu()
{
}

void BrowseButtonMenu::selectUrl(const String& url)
{
    if (url.isEmpty())
        return;
    emit urlSelected(url);
}

void BrowseButtonMenu::browse()
{
    if (!browseMenu_.isNull() && 0 != browseMenu_->count()) {
        QAction* action = browseMenu_->defaultAction();
        if (action) 
            return browse(action);
    }
    emit browseStarted();

    String url;
#if defined(__APPLE__) && 0   
    QFileDialog file_dialog(baseUrl_, filters_, button_->parentWidget(),
                            NOTR("open file dialog"), true);
    file_dialog.setCaption(caption_);
    file_dialog.setMode(QFileDialog::ExistingFile);
    file_dialog.setSelection(baseUrl_);
    if (file_dialog.exec() == QDialog::Accepted)
        url = file_dialog.selectedFile();
#else
    url = QFileDialog::getOpenFileName(
        button_->parentWidget(), caption_, baseUrl_, filters_);
#endif

    if (!url.isEmpty())
        return selectUrl(url);
    emit browseRejected();
}

typedef Common::CommandEventMakerRegistry MR;

void BrowseButtonMenu::browse(QAction* action)
{
    PropertyNodePtr protocol = 
        ((PropertyNode*) action->data().value<void*>())->copy(true);
    String command_event = 
        protocol->getSafeProperty("commandEvent")->getString();
    if (command_event.isEmpty())
        return;

    static const char ptn_class[] = NOTR("PropertyTree");
    const MR::MakerInfo* mi = MR::getMakerInfo(command_event.utf8().c_str());
    if (0 == mi)
        return;
    if (strcmp(mi->inClass_, ptn_class))
        return;
    if (strcmp(mi->outClass_, ptn_class))
        return;

    PropertyTreeEventData data;
    data.root()->appendChild(new PropertyNode(NOTR("caption"), caption_));
    data.root()->appendChild(new PropertyNode(NOTR("filter"), filters_));
    if (isSaveUrl_)
        data.root()->appendChild(new PropertyNode(NOTR("save-url"), "true"));

    emit browseStarted();
    make_browse_url_list(baseUrl_, protocol.pointer(), protocol.pointer(),
        config().getProperty("dav/protocols")->getProperty(protocol->name()));
    
    PropertyNode* url_list = protocol->getProperty("url-list");
    if (url_list) {
        url_list = url_list->copy(true);
        data.root()->appendChild(url_list);
    }
    if ((mi->maker_)(&data)->execute(doc_, &data)) {
        String url = data.root()->getSafeProperty("url")->getString();
        if (!url.isEmpty()) {
            save_recent_urls(data.root(), protocol->name());
            selectUrl(url);
            return;
        }
    }
    emit browseRejected();
}

void BrowseButtonMenu::setBaseUrl(const String& url)
{
    baseUrl_ = url;
}

void BrowseButtonMenu::setCaption(const String& caption)
{
    caption_ = caption;
}

void BrowseButtonMenu::setFilters(const String& filters)
{
    filters_ = filters;
}
