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

#include "core/PluginsTabWidgetBase.hpp"
#include "core/Preferences.h"

#include "utils/Config.h"
#include "utils/Properties.h"
#include "docview/PluginLoader.h"

#include "common/PropertyTree.h"
#include "common/String.h"
#include "common/Url.h"

#include <QHeaderView>

using namespace Common;

class PluginsTabWidget : public QWidget,
                         public Ui::PluginsTabWidgetBase {
    Q_OBJECT
public:
    PluginsTabWidget(PropertyNode*);

public slots:
    virtual void on_pluginsListView__itemChanged(QTreeWidgetItem*, int);

private:
    Common::PropertyNode*   pluginDesc_;
    Common::PropertyNode*   appPtn_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(PluginsTab, 7, true)

////////////////////////////////////////////////////////////////////////////

static QString get_name(const PropertyNode* ptn)
{
    String nm(ptn->getSafeProperty(PluginLoader::PLUGIN_DESC)->getString());
    if (nm.isEmpty())
        nm = ptn->getSafeProperty(PluginLoader::PLUGIN_NAME)->getString();
    return nm;
}

class PluginCheckListItem : public QTreeWidgetItem {
public:
    PluginCheckListItem(QTreeWidget* parent,
                        PropertyNode* ptn)
        : ptn_(ptn)
    {
        setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        setText(0, get_name(ptn));
        setCheckState(0, ptn->getSafeProperty(
            PluginLoader::IS_DISABLED)->getBool() ? 
            Qt::Unchecked : Qt::Checked);
        String path = ptn->getSafeProperty(PluginLoader::RESOLVED_PATH)->
            getString() + "/" +
            ptn->getSafeProperty(PluginLoader::SPD_FILE_NAME)->getString();
        path = Url(config().getDataDir()).relativePath(path);
        setText(1, path);
        parent->addTopLevelItem(this);
    }
    PropertyNode* ptn_;
};

void 
PluginsTabWidget::on_pluginsListView__itemChanged(QTreeWidgetItem* item, int)
{
    PluginCheckListItem* pi = dynamic_cast<PluginCheckListItem*>(item);
    if (0 == pi)
        return;
    String disabled_plugins;
    for (int i = 0; i < pluginsListView_->topLevelItemCount(); ++i) {
        PluginCheckListItem* item = static_cast<PluginCheckListItem*>
            (pluginsListView_->topLevelItem(i));
        PropertyNode* dp = item->ptn_->getProperty(PluginLoader::IS_DISABLED);
        if (item->checkState(0) != Qt::Checked) {
            item->ptn_->makeDescendant(PluginLoader::IS_DISABLED)->setBool(1);
            disabled_plugins += ' ' + item->ptn_->getSafeProperty(
                PluginLoader::PLUGIN_NAME)->getString();
        } else {
            if (dp && dp->getBool())
                dp->setBool(0);
        }
    }
    appPtn_->makeDescendant(NOTR("disabled-plugins"), disabled_plugins, true);
}

//////////////////////////////////////////////////////////////////////////////

PluginsTabWidget::PluginsTabWidget(Common::PropertyNode* props)
:   pluginDesc_(pluginLoader().getPluginProps()),
    appPtn_(props->makeDescendant(App::APP))
{
    setupUi(this);
    pluginsListView_->header()->setResizeMode(QHeaderView::Stretch);

    PropertyNode* ptn = pluginDesc_->firstChild();
    for (; ptn; ptn = ptn->nextSibling()) 
        new PluginCheckListItem(pluginsListView_, ptn);
}

#include "moc/PluginsTabWidget.moc"

