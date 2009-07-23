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
#include "common/Encodings.h"
#include "ui/IconProvider.h"
#include "dav/DavManager.h"

#include "docview/dv_utils.h"
#include "docview/SernaDoc.h"

#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"
#include "utils/SernaUiItems.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "docutils/doctags.h"

#include <QHeaderView>
#include <QTreeWidget>
#include <QShortcut>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

#include "docview/UrlBrowseDialogBase.hpp"

// START_IGNORE_LITERALS
static const char* CAPTION         = "caption";
static const char* FILTER          = "filter";
static const char* SAVE_URL        = "save-url";
static const char* IS_COLLECTION   = "is_collection";
static const char* LAST_URL        = "last-url";
static const char* URL_LIST        = "url-list";
static const char* RECENT_URL_LIST = "recent-url-list";
static const char* URL             = "url";
// STOP_IGNORE_LITERALS

using namespace Common;

typedef Common::Encodings ENC;

class UrlBrowseDialog : public QDialog,
                        protected Ui::UrlBrowseDialogBase {
    Q_OBJECT

public:
    UrlBrowseDialog(PropertyNode* prop, QWidget* parent);

    void            listFiles(bool silent = false);
    PropertyNodePtr urlProps(const String& filename) const;
    void            selectRelativeUrl(const String& localUrl);

protected slots:
    virtual void    currentUrlChanged(QTreeWidgetItem* curr, QTreeWidgetItem*);
    virtual void    on_upToolButton__clicked();
    virtual void    on_okButton__clicked() { chooseFile(); }
    virtual void    on_fileLineEdit__returnPressed() { chooseFile(); }

    virtual void    on_urlComboBox__activated(const QString& url) {
        DBG(DV.DAV) << "urlSelected: " << String(url) << std::endl;
        listFiles();
    }

    virtual void    urlSelected(QTreeWidgetItem* item, int) {
        DBG(DV.DAV) << "urlSelected: " << item << std::endl;
        if (!item)
            return;
        selectRelativeUrl(item->text(0));
    }

    virtual void    on_helpButton__clicked()
    {
        helpAssistant().show(DOCTAG(WEBDAV_OPEN));
    }
protected:
    void            chooseFile();
    void            acceptUrl(const String& url);

private:
    PropertyNode*   prop_;
    PropertyNode    files_;
    const bool      isSaveUrl_;
};

UrlBrowseDialog::UrlBrowseDialog(PropertyNode* prop, QWidget* parent)
    : QDialog(parent),
      prop_(prop),
      isSaveUrl_(prop_->getProperty(SAVE_URL))
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));
    upToolButton_->setIcon(Sui::icon_provider().
        getIconSet(NOTR("assistant_up")));
    setCaption(prop->getSafeProperty(CAPTION)->getString());

    if (isSaveUrl_)
        okButton_->setText(tr("&Save"));
    else
        okButton_->setText(tr("&Open"));

    if (prop_->getProperty("choose-encoding")) {
        for (int enc = ENC::FIRST_ENCODING; enc < ENC::XML; ++enc)
            encodingComboBox_->insertItem
                (ENC::encodingName(ENC::Encoding(enc)).qstring());
        encodingComboBox_->setCurrentText
            (prop->getSafeProperty(DocSrcInfo::ENCODING)->getString());
    }
    else {
        encodingLabel_->setVisible(false);
        encodingComboBox_->setVisible(false);
    }

    PropertyNode* url_list = prop_->getProperty(URL_LIST);
    if (url_list) {
        for (PropertyNode* url = url_list->firstChild(); url;
             url = url->nextSibling()) {
            if (url->name() == URL)
                urlComboBox_->insertItem(url->getString());
        }
    }
    urlComboBox_->setCurrentItem(0);
    //fileListView_->setColumnAlignment(1, Qt::AlignRight);
    fileListView_->setSortingEnabled(false);
    listFiles(true);
    fileLineEdit_->setText(prop_->getSafeProperty("filename")->getString());

    connect(fileListView_, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(urlSelected(QTreeWidgetItem*, int)));
    connect(fileListView_,
            SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(currentUrlChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

static QTreeWidgetItem* make_file_item(QTreeWidget* view,
                                       QTreeWidgetItem* after,
                                       const PropertyNode* prop, bool isDir)
{
    QTreeWidgetItem* item = (after)
        ? new QTreeWidgetItem(view, after)
        : new QTreeWidgetItem(view);
    item->setText(0, prop->name());
    if (isDir) {
        item->setText(2, qApp->translate("UrlBrowseDialog", "Dir"));
        item->setIcon(0, Sui::icon_provider().getPixmap(NOTR("browse")));
    }
    else {
        item->setText(1, prop->getSafeProperty("size")->getString());
        item->setText(2, qApp->translate("UrlBrowseDialog", "File"));
        item->setIcon(0, Sui::icon_provider().getPixmap(NOTR("document")));
    }
    QDateTime time;
    time.setTime_t(prop->getSafeProperty("modtime")->getInt());
    const String format = NOTR("dd/MM/yyyy hh:mm:ss");
    item->setText(3, time.toString(format));

    if (NOTR("write") == prop->getSafeProperty("lock/locktype")->getString()) {
        item->setIcon(4, Sui::icon_provider().getPixmap(
                          NOTR("fixed_attribute")));
        item->setText(4, qApp->translate("UrlBrowseDialog", "Read only"));
    }
    else
        item->setText(4, qApp->translate("UrlBrowseDialog", "Read-Write"));
    return item;
}

void UrlBrowseDialog::listFiles(bool silent)
{
    DBG(DV.DAV) << "listFiles: " << std::endl;
    Dav::DavManager& dav = Dav::DavManager::instance();
    Url url(urlComboBox_->currentText());
    files_.removeAllChildren();
    Dav::OpStatus status = dav.listCollection(url, &files_);
    DBG_IF(DV.DAV) files_.dump();

    fileListView_->clear();
    if (Dav::DAV_RESULT_OK != status) {
        if (!silent)
            QMessageBox::critical(
                qApp->activeWindow(),
                tr("Cannot access resource"),
                tr("Cannot get resource list from URL '%0'").arg(String(url)));
        return;
    }
    else
        prop_->makeDescendant(LAST_URL)->setString(
            urlComboBox_->currentText());

    QTreeWidgetItem* last_dir = 0;
    QTreeWidgetItem* last_file = 0;
    for (PropertyNode* c = files_.firstChild(); c; c = c->nextSibling()) {
        if (c->getProperty(IS_COLLECTION))
            last_dir = make_file_item(fileListView_, last_dir, c, true);
        else
            last_file = make_file_item(
                fileListView_, (last_file) ? last_file : last_dir, c, false);
    }
    int columns_width = 0;
    fileListView_->resizeColumnToContents(0);
    for (int col = 1; col < fileListView_->columnCount(); col++) {
        fileListView_->resizeColumnToContents(col);
        columns_width += fileListView_->columnWidth(col);
    }
    /*
    int w = fileListView_->width();
    if (w < fileListView_->contentsWidth())
        w = fileListView_->contentsWidth();
    w -= columns_width;
    if (w > fileListView_->columnWidth(0))
        fileListView_->setColumnWidth(0, w);
    */
}

void UrlBrowseDialog::currentUrlChanged(QTreeWidgetItem* item,
                                        QTreeWidgetItem*)
{
    DBG(DV.DAV) << "currentUrlChanged: " << item << std::endl;
    if (!item)
        return;
    PropertyNodePtr url_prop = urlProps(item->text(0));
    if (url_prop.isNull()) {
        fileLineEdit_->setText("");
        return;
    }
    if (!url_prop->getProperty(IS_COLLECTION))
        fileLineEdit_->setText(item->text(0));
}

void UrlBrowseDialog::on_upToolButton__clicked()
{
    Url upper_url(Url(urlComboBox_->currentText()).upperUrl());
    if (String(upper_url).isEmpty())
        return;

    PropertyNodePtr prop = new PropertyNode();
    Dav::OpStatus status = Dav::DavManager::instance().
        getResourceInfo(upper_url, prop.pointer());
    if (Dav::DAV_RESULT_OK == status) {
        urlComboBox_->setCurrentText(upper_url.operator String());
        listFiles();
    }
}

void UrlBrowseDialog::acceptUrl(const String& url)
{
    prop_->makeDescendant(URL)->setString(url);
    if (isSaveUrl_)
        prop_->makeDescendant(DocSrcInfo::ENCODING)->setString(
            encodingComboBox_->currentText());
    PropertyNode* recent_urls = prop_->makeDescendant(RECENT_URL_LIST);
    recent_urls->removeAllChildren();
    PropertyNode* last_url = prop_->getProperty(LAST_URL);
    if (last_url)
        recent_urls->appendChild(new PropertyNode(URL, last_url->getString()));
    const int max_url = urlComboBox_->count();
    for (int c = 0; c < max_url; c++) {
        String url = urlComboBox_->text(c);
        if (!url.isEmpty())
            recent_urls->appendChild(new PropertyNode(URL, url));
    }
    accept();
}

void UrlBrowseDialog::chooseFile()
{
    DBG(DV.DAV) << "chooseFile" << std::endl;
    if (fileLineEdit_->text().isEmpty()) {
        QTreeWidgetItem* item = fileListView_->currentItem();
        if (item)
            urlSelected(item, 0);
    }
    else {
        Url url(fileLineEdit_->text());
        if (url.isRelative())
            selectRelativeUrl(url);
        else
            acceptUrl(url);
    }
}

PropertyNodePtr UrlBrowseDialog::urlProps(const String& filename) const
{
    Url url = Url(urlComboBox_->currentText()).combineDir2Path(Url(filename));
    DBG(DV.DAV) << "urlProps, url=" << url << std::endl;
    PropertyNodePtr prop = new PropertyNode();
    Dav::OpStatus status =
        Dav::DavManager::instance().getResourceInfo(url, prop.pointer());
    if (Dav::DAV_RESULT_OK != status) {
        if (!isSaveUrl_)
            QMessageBox::critical(
                qApp->activeWindow(),
                tr("Cannot access resource"),
                tr("Cannot get resource from URL '%0'").arg(String(url)));
        DBG(DV.DAV) << "status: " << status << std::endl;
        return 0;
    }
    prop->makeDescendant("absolute-url")->setString(url);
    prop->makeDescendant("relative-url")->setString(filename);
    return prop;
}

void UrlBrowseDialog::selectRelativeUrl(const String& relativeUrl)
{
    DBG(DV.DAV) << "selectRelativeUrl: " << relativeUrl << std::endl;
    PropertyNodePtr url_prop = urlProps(relativeUrl);
    if (url_prop.isNull()) {
        if (isSaveUrl_) {
            Url url = Url(urlComboBox_->currentText()).combineDir2Path(
                Url(relativeUrl));
            acceptUrl(String(url));
        }
        return;
    }
    if (url_prop->getProperty(IS_COLLECTION)) {
        urlComboBox_->setCurrentText(
            url_prop->getSafeProperty("absolute-url")->getString());
        listFiles();
    }
    else
        acceptUrl(url_prop->name());
}

////////////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(ShowUrlDialog, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(ShowUrlDialog, "PropertyTree", "PropertyTree")

bool ShowUrlDialog::doExecute(SernaDoc* sernaDoc, EventData*)
{
    UrlBrowseDialog dialog(ed_, sernaDoc->widget(0));
    if (dialog.exec() != QDialog::Accepted)
        return false;
    DBG_IF(DV.DAV) ed_->dump();
    return true;
}

/////////////////////////////////////////////////////////////////////
void copy_or_set(PropertyNode* src, PropertyNode* dst,
                 const String& name, const String& value)
{

    PropertyNode* src_prop = src->getProperty(name);
    if (src_prop)
        dst->appendChild(src_prop->copy(true));
    else
        dst->makeDescendant(name)->setString(value);
}



bool get_url(SernaDoc* sernaDoc, PropertyNode* dsi,
             const String& caption, bool saveUrl)
{
    PropertyTreeEventData data;
    copy_or_set(dsi, data.root(), NOTR("caption"), caption);

    if (saveUrl)
        copy_or_set(dsi, data.root(), NOTR("url"),
                    sernaDoc->getDsi()->getSafeProperty(
                        DocSrcInfo::DOC_PATH)->getString());

    Url url(data.root()->getSafeProperty(NOTR("url"))->getString());
    if (saveUrl) {
        data.root()->appendChild(new PropertyNode(NOTR("save-url"), true));
        data.root()->appendChild(new PropertyNode(NOTR("choose-encoding"), true));
        const PropertyNode* encoding = sernaDoc->getDsi()->
            getSafeProperty(DocSrcInfo::ENCODING);
        data.root()->makeDescendant(DocSrcInfo::ENCODING)->setString(
            encoding->getString());

        copy_or_set(dsi, data.root(), NOTR("filename"), url[Url::FILENAME]);
    }
    //////////
    PropertyNode* protocol =
        config().root()->getProperty("dav/#protocols/http");
    if (protocol) {
        PropertyNodePtr prop = protocol->copy(true);
        PropertyNode* saved_protocol =
            config().root()->getProperty("dav/protocols/http");
        String browse_dir = (saveUrl)
            ? String(url) : DocSrcInfo::get_browse_dir(sernaDoc->getDsi());
        make_browse_url_list(browse_dir, prop.pointer(),
                             data.root(), saved_protocol);
    }
    //////////
    if (!makeCommand<ShowUrlDialog>(&data)->execute(sernaDoc, &data))
        return false;

    save_recent_urls(data.root(), NOTR("http"));
    dsi->makeDescendant(DocSrcInfo::DOC_PATH)->setString(
        data.root()->getSafeProperty("url")->getString());
    dsi->makeDescendant(DocSrcInfo::ENCODING)->setString(
        data.root()->getSafeProperty(DocSrcInfo::ENCODING)->getString());
    return true;
}

PROPTREE_EVENT_IMPL(OpenUrlDialog, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(OpenUrlDialog, "PropertyTree", "PropertyTree")

bool OpenUrlDialog::doExecute(SernaDoc* sernaDoc, EventData* result)
{
    return get_url(sernaDoc,
                   static_cast<PropertyTreeEventData*>(result)->root(),
                   tr("WebDAV: Open Document"), false);
}

PROPTREE_EVENT_IMPL(SaveAsUrlDialog, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(SaveAsUrlDialog, "PropertyTree", "PropertyTree")

bool SaveAsUrlDialog::doExecute(SernaDoc* sernaDoc, EventData* result)
{
    return get_url(sernaDoc,
                   static_cast<PropertyTreeEventData*>(result)->root(),
                   tr("WebDAV: Save As"), true);
}

///////////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(ShowFileDialog, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(ShowFileDialog, "PropertyTree", "PropertyTree")

bool ShowFileDialog::doExecute(SernaDoc* doc, EventData* result)
{
    QString filter = ed_->getSafeProperty(FILTER)->getString();
    if (filter.isEmpty())
        filter = tr("All Files (*)");
    QString caption = ed_->getSafeProperty(CAPTION)->getString();
    if (caption.isEmpty())
        caption = tr("Serna");

    QString base_url = ed_->getSafeProperty(LAST_URL)->getString();
    if (base_url.isEmpty())
        base_url = ed_->getSafeProperty
            (URL_LIST + String("/") + URL)->getString();
    QWidget* parent = qApp->activeWindow() ?
                      qApp->activeWindow() : doc->widget(0);

    QString url;
    if (ed_->getProperty(SAVE_URL))
        url = QFileDialog::getSaveFileName(base_url, filter, parent,
                                           caption, caption);
    else
        url = QFileDialog::getOpenFileName(base_url, filter, parent,
                                           caption, caption);

    static_cast<PropertyTreeEventData*>(result)->root()->
        makeDescendant(URL, url, true);
    if (!url.isEmpty())
        static_cast<PropertyTreeEventData*>(result)->root()->
            makeDescendant(LAST_URL, Url(url)[Url::COLLPATH], true);

    if (!url.isEmpty()) {
        PropertyNode* recent_urls = ed_->makeDescendant(RECENT_URL_LIST);
        PropertyNodePtr base_url =
            new PropertyNode(URL, String(Url(url)[Url::COLLPATH]));
        if (recent_urls->firstChild())
            recent_urls->firstChild()->insertBefore(base_url.pointer());
        else
            recent_urls->appendChild(base_url.pointer());
        return true;
    }
    return false;
}

#include "moc/UrlBrowseDialog.moc"
