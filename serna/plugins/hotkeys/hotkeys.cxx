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
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/UiAction.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/app/UiItem.h"
#include "sapi/app/UiAction.h"
#include "sapi/app/Config.h"
#include <iostream>

#include "sapi/app/UiItem.h"
#include "sapi/app/IconProvider.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/common/doctags.h"

#include <QKeySequence>
#include <QPixmap>
#include <QShortcut>
#include <QColor>
#include <QHeaderView>

#include "common/common_defs.h"

#include "HotkeysDialogBase.hpp"

#include <map>
#include <iostream>

class HotKeysPlugin;

using namespace SernaApi;

SAPI_PLUGIN_EXECUTOR_IMPL(ShowShortcuts, HotKeysPlugin)

/////////////////////////////////////////////////////////////////////////////

static QColor backColor1   = QColor(250, 248, 235);
static QColor backColor2   = QColor(255, 255, 255);
static QColor selectedBack = QColor(230, 230, 230);

class ListItem : public QTreeWidgetItem {
public:
    ListItem(QTreeWidget* list, const UiAction& action,
             QString accel, bool isDup)
        : QTreeWidgetItem(list)
    {
        QString name = action.get(NOTR("name"));
        QString info = action.getTranslated(NOTR("inscription"));
        info.replace('&', "");
        QString icon = action.get(NOTR("icon"));
        if (isDup)
            accel += NOTR(" [DUP!]");
        setText(3, name);   setToolTip(3, name);
        setText(0, info);   setToolTip(0, info);
        setText(1, accel);  setToolTip(1, accel);
        if (!icon.isEmpty()) 
            setIcon(2, IconProvider::getPixmap(icon));
        QFont accel_font(font(1));
        accel_font.setBold(true);
        setFont(1, accel_font);
        if (isDup)
            setForeground(1, QColor(NOTR("red")));
    }
    QColor       backgroundColor(int) const
    {
        int idx  = treeWidget()->indexOfTopLevelItem(
            const_cast<ListItem*>(this));
        return (idx % 2) ? backColor1 : backColor2;
    }

private:
    QColor       backColor_;
};

/////////////////////////////////////////////////////////////////////////////

typedef std::map<QString, QString> StringItemMap;

void make_accel_list(UiItem item, QTreeWidget* list, StringItemMap& accelMap) 
{
    QString accel_str = item.action().getTranslated(NOTR("accel"));
    QString accel = QKeySequence(accel_str);
    QString name = item.action().get(NOTR("name"));
    if (!accel.isEmpty()) {
        StringItemMap::const_iterator it = accelMap.find(accel);
        if (it == accelMap.end()) {
            (void) new ListItem(list, item.action(), accel, false); 
            accelMap[accel] = name;
        } else if (it->second != name)
            (void) new ListItem(list, item.action(), accel, true);
  }
  for (item = item.firstChild(); item; item = item.nextSibling())
      make_accel_list(item, list, accelMap);
}

class HotkeysDialog : public QDialog, 
                      protected Ui::HotkeysDialogBase {
    Q_OBJECT
public:
    HotkeysDialog(const SernaApi::SernaDoc& se) 
        : QDialog(se.widget()), 
          se_(se)
    {
        setupUi(this);
        connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
                SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));
        QHeaderView& h = *listView_->header();
        h.setResizeMode(QHeaderView::ResizeToContents);
        h.setMinimumSectionSize(24);
        h.setResizeMode(0, QHeaderView::ResizeToContents);
        h.setResizeMode(1, QHeaderView::ResizeToContents);
        h.setResizeMode(2, QHeaderView::ResizeToContents);
        h.setMovable(false);
        listView_->setAutoFillBackground(true);
        StringItemMap accel_map;
        make_accel_list(se, listView_, accel_map);
    }
    
private slots:
    void on_helpButton__clicked() { se_.showHelp(DOCTAG(SHORTCUT_KEYS_D)); }

private:
    const SernaApi::SernaDoc& se_;
};

///////////////////////////////////////////////////////////////

class HotKeysPlugin : public SernaApi::DocumentPlugin {
public:
    HotKeysPlugin(SernaApiBase* ssd, SernaApiBase* properties, char**)
        : DocumentPlugin(ssd, properties)
    {
        SAPI_REGISTER_UI_EXECUTOR(ShowShortcuts);
        buildPluginExecutors();
    }
    ~HotKeysPlugin() {}
};

void ShowShortcuts::execute()
{
    SernaDoc doc = plugin()->sernaDoc();    
    HotkeysDialog(doc).exec();
}

SAPI_DEFINE_PLUGIN_CLASS(HotKeysPlugin)

#include "moc/hotkeys.moc"
