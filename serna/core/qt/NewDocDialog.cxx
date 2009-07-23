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
#include "common/String.h"
#include "common/PathName.h"
#include "common/Encodings.h"
#include "common/Url.h"

#include "grove/Grove.h"

#include "ui/IconProvider.h"

#include "docview/SernaDoc.h"
#include "docview/qt/BrowseButtonMenu.h"
#include "core/core_defs.h"

#include "utils/DocSrcInfo.h"
#include "utils/DocTemplate.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include <QMessageBox>
#include <QShortcut>
#include <QPixmap>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QApplication>

#include "core/NewDocDialogBase.hpp"
#include "core/TemplateDetailsBase.hpp"

using namespace Common;
using namespace DocSrcInfo;
using namespace DocTemplate;

///////////////////////////////////////////////////////////////////////////

class NewDocDialog;

namespace {

class TemplateItem;
class TemplateDetails;

class QtNewDocDialog : public QDialog,
                       public Ui::NewDocDialogBase,
                       public Common::PropertyTree {
    Q_OBJECT
public:
    QtNewDocDialog(SernaDoc* doc, const COMMON_NS::PropertyNode* params,
                   bool isChooseTemplate);
    ~QtNewDocDialog() {}

public slots:
    void    on_detailsButton__toggled(bool);
    void    on_filenameEdit__textChanged(const QString&);

    void    on_categoryView__currentItemChanged(QListWidgetItem*,
                                                QListWidgetItem* = 0);
    void    on_categoryView__itemDoubleClicked(QListWidgetItem*);

    void    on_templateListBox__currentItemChanged(QListWidgetItem*,
                                                   QListWidgetItem* = 0);
    void    on_templateListBox__itemDoubleClicked(QListWidgetItem*);

public slots:
    void    selectDocumentUrl(const Common::String& url);
    void    accept();
    void    help();

private:
    void    loadDocTemplates();
    void    selectFile();
    void    updateResult(bool);

private:
//    friend class NewDocDialog;

    Common::PropertyNodePtr         params_;
    Common::PropertyTree            docparam_;
    TemplateItem*                   currentTemplate_;
    TemplateDetails*                details_;
    Common::String                  docpath_;
    const bool                      isChooseTemplate_;
    QPointer<BrowseButtonMenu>      browseMenu_;
};

/////////////////////////////////////////////////////////////////

class TemplateDetails : public QWidget,
                        private Ui::TemplateDetailsBase,
                        public Common::PropertyTree {
    Q_OBJECT
public:
    TemplateDetails(SernaDoc* doc,
                    const Common::PropertyNode* tprops,
                    bool isChooseTemplate);
    ~TemplateDetails();

    void    update(const Common::PropertyNode* props);
    Common::PropertyNode* getProps();

public slots:
    virtual void    selectSchemaUrl(const Common::String& url);
    virtual void    selectStyleUrl(const Common::String& url);
    virtual void    on_dtdSysid__textChanged();
    virtual void    on_helpButton__clicked();

private:
    bool                         isChooseTemplate_;
    Common::String               browsePath_;
    QPointer<BrowseButtonMenu>   schemaBrowseMenu_;
    QPointer<BrowseButtonMenu>   styleBrowseMenu_;
};

/////////////////////////////////////////////////////////////////

class DocTemplateCategory : public QListWidgetItem {
public:
    DocTemplateCategory(const Common::PropertyNode* prop)
        : templates_(prop)
    {
        QPixmap pixmap(getPixmap(prop));
        setIcon(pixmap);
        setText(prop->name());
        setSizeHint(QSize(font().pointSize() * 15 + pixmap.width(),
            (font().pointSize() + 1) / 2 + pixmap.height() + 6));
    }
    static QPixmap getPixmap(const PropertyNode* prop)
    {
        const PropertyNode* pn =
            prop->firstChild()->getProperty(TEMPLATE_CATEGORY_PIXMAP);
        if (pn) {
            QPixmap pixmap = Sui::icon_provider().getPixmap(pn->getString());
            if (!pixmap.isNull())
                return pixmap;
        }
        return Sui::icon_provider().getPixmap(NOTR("other_template"));
    }
    const Common::PropertyNode* templates_;
};

class TemplateItem : public QListWidgetItem {
public:
    TemplateItem(const PropertyNode* tmpl, const PropertyNode* skel)
        : QListWidgetItem(skel ?
            skel->getSafeProperty("name")->getString() : tmpl->name()),
          template_(tmpl), skeleton_(skel) {}
    const PropertyNode* template_;
    const PropertyNode* skeleton_;
};

} // namespace

///////////////////////////////////////////////////////////////////////////

static void remove_property(PropertyNode* root, const String& name)
{
    PropertyNode* pn = root->getProperty(name);
    if (pn)
        pn->remove();
}

static QString opendoc_filters(const PropertyNode* root)
{
    if (const PropertyNode* ptn = root->getProperty(SernaFileType::TYPE_LIST)) {
        QStringList filters;
        for (ptn = ptn->firstChild(); ptn; ptn = ptn->nextSibling()) {
            if (SernaFileType::TYPE != ptn->name())
                continue;
            QString description =
                ptn->getSafeProperty(SernaFileType::DESCRIPTION)->getString();
            if (description.isEmpty())
                continue;
            QStringList patterns;
            const PropertyNode* tmpNode = ptn->firstChild();
            for (; tmpNode; tmpNode = tmpNode->nextSibling())
                if (SernaFileType::PATTERN == tmpNode->name())
                    patterns.push_back(tmpNode->getString());
            if (patterns.empty())
                continue;
            filters << (description + " (" + patterns.join(QChar(' ')) + ')');
        }
        return filters.join(";;");
    }
    return QString::null;
}

///////////////////////////////////////////////////////////////////////////

QtNewDocDialog::QtNewDocDialog(SernaDoc* doc, const PropertyNode* ip,
                               bool isChooseTemplate)
    : QDialog(doc->widget(0)),
      params_(ip->copy(true)),
      currentTemplate_(0),
      isChooseTemplate_(isChooseTemplate)
{
    setupUi(this);
    hboxLayout->setStretchFactor(vboxLayout,  3);
    hboxLayout->setStretchFactor(vboxLayout1, 1);

    connect(new QShortcut(QKeySequence(Qt::Key_F1), this),
            SIGNAL(activated()), this, SLOT(help()));

    docpath_ = params_->getSafeProperty(DOC_PATH)->getString();
    if (Url(docpath_).isRelative()) {
        Url browse_dir(params_->getSafeProperty(BROWSE_DIR)->getString());
        docpath_ = String(browse_dir.combineDir2Path(Url(docpath_)));
    }
    filenameEdit_->setText(Url(docpath_)[Url::FILENAME]);
    on_filenameEdit__textChanged(filenameEdit_->text());
    params_->makeDescendant("base-url")->setString(docpath_);
    details_ = new TemplateDetails(doc, params_.pointer(), isChooseTemplate_);
    setExtension(details_);
    setOrientation(Qt::Vertical);
    setSizeGripEnabled(true);
    okButton_->setFocus();
    loadDocTemplates();
    if (isChooseTemplate_) {
        setCaption(tr("Choose Document Template"));
        browseButton_->setEnabled(false);

        filenameEdit_->setReadOnly(true);
        filenameEdit_->setPaletteBackgroundColor
            (filenameEdit_->palette().inactive().background());
        selectFile();
    }
    else {
        browseMenu_ = new BrowseButtonMenu(doc, browseButton_, docpath_, true);
        browseMenu_->setCaption(tr("Choose a File"));
        browseMenu_->setFilters(tr("XML Documents (*.xml);;All files (*)"));
        connect(browseMenu_, SIGNAL(urlSelected(const Common::String&)),
                this, SLOT(selectDocumentUrl(const Common::String&)));
    }
}

void QtNewDocDialog::selectFile()
{
    PropertyTree matched_templates;
    PiReader pi(docpath_);
    docparam_.setRoot(pi.root());
    DocTemplateHolder::instance().findMatched(matched_templates.root(),
        pi.root(), docpath_);
    PropertyNode* best_template = matched_templates.root()->firstChild();
    if (best_template)
        best_template = best_template->firstChild();
    if (best_template) {
        String t_category =
            best_template->getSafeProperty(TEMPLATE_CATEGORY)->getString();
        String t_name     =
            best_template->getSafeProperty(TEMPLATE_NAME)->getString();
        const PropertyNode* tmpl = 0;
        DocTemplateCategory* dtc = 0;
        for (int i = 0; i < categoryView_->count(); ++i) {
            QListWidgetItem* item = categoryView_->item(i);
            dtc = static_cast<DocTemplateCategory*>(item);
            if (dtc->templates_->name() != t_category)
                continue;
            tmpl = dtc->templates_->firstChild();
            for (; tmpl; tmpl = tmpl->nextSibling()) {
                if (tmpl->name() == t_name)
                    break;
            }
            if (tmpl)
                break;
        }
        if (tmpl) {
            categoryView_->setCurrentItem(dtc);
            on_categoryView__currentItemChanged(dtc);
            templateListBox_->setCurrentItem
                (templateListBox_->item(tmpl->siblingIndex()));
        }
        else {
            categoryView_->setCurrentItem(categoryView_->item(0));
            on_categoryView__currentItemChanged(categoryView_->item(0));
            templateListBox_->setCurrentItem(templateListBox_->item(0));
        }
    }
    else {
        categoryView_->setCurrentItem(categoryView_->item(0));
        on_categoryView__currentItemChanged(categoryView_->item(0));
        templateListBox_->setCurrentItem(templateListBox_->item(0));
    }
    updateResult(!isChooseTemplate_);
}

void QtNewDocDialog::updateResult(bool selectFile)
{
    root()->removeAllChildren();
    PropertyNodePtr skel;
    if (currentTemplate_) {
        PropertyNode* pn = currentTemplate_->template_->firstChild();
        for (; pn; pn = pn->nextSibling())
            if (pn->name() != DOCUMENT_SKELETON &&
                pn->name() != TEMPLATE_DOCROOTGROVE)
                    root()->appendChild(pn->copy(true));
        if (currentTemplate_->skeleton_) {
            pn = currentTemplate_->skeleton_->
                getProperty(TEMPLATE_DOCROOTGROVE);
            if (pn)
                skel = pn->copy();
        }
    }
    if (isChooseTemplate_) {
        remove_property(root(), DOCINFO_DOCELEM);
        remove_property(root(), DOCINFO_PUBID);
        remove_property(root(), DOCINFO_SYSID);
    }
    root()->merge(docparam_.root(), selectFile);
    if (skel)
        root()->appendChild(skel.pointer());
    details_->update(root());
}

void QtNewDocDialog::loadDocTemplates()
{
    const PropertyNode* cat_prop =
        DocTemplateHolder::instance().root()->firstChild();
    for (; cat_prop; cat_prop = cat_prop->nextSibling()) {
        const PropertyNode* tmpl = cat_prop->firstChild();
        if (!tmpl)
            continue;
        if (!isChooseTemplate_) {
            int count = 0;
            for (; tmpl; tmpl = tmpl->nextSibling()) {
                if (!tmpl->getSafeProperty(HIDE_IN_NEWDOC_DIALOG)->getBool())
                    ++count;
            }
            if (!count)
                continue;
        }
        categoryView_->addItem(new DocTemplateCategory(cat_prop));
    }
    if (categoryView_->count()) {
        categoryView_->setCurrentRow(0);
        on_categoryView__currentItemChanged(categoryView_->item(0));
    }
}

void QtNewDocDialog::on_categoryView__currentItemChanged(
    QListWidgetItem* item, QListWidgetItem*)
{
    templateListBox_->clear();
    DocTemplateCategory* category = static_cast<DocTemplateCategory*>(item);
    const PropertyNode* tmpl = category->templates_->firstChild();
    for (; tmpl; tmpl = tmpl->nextSibling()) {
        if (isChooseTemplate_) {
            templateListBox_->addItem(new TemplateItem(tmpl, 0));
            continue;
        } else {
            if (tmpl->getSafeProperty(HIDE_IN_NEWDOC_DIALOG)->getBool())
                continue;
        }
        const PropertyNode* pn = tmpl->firstChild();
        bool foundSkel = false;
        for (; pn; pn = pn->nextSibling()) {
            if (pn->name() == DOCUMENT_SKELETON) {
                templateListBox_->addItem(new TemplateItem(tmpl, pn));
                foundSkel = true;
            }
        }
        if (!foundSkel)
            templateListBox_->addItem(new TemplateItem(tmpl, 0));
    }
    if (templateListBox_->count()) {
        templateListBox_->setItemSelected(templateListBox_->item(0), true);
        on_templateListBox__currentItemChanged(templateListBox_->item(0));
    }
}

void QtNewDocDialog::on_categoryView__itemDoubleClicked(QListWidgetItem* item)
{
    on_categoryView__currentItemChanged(item);
    on_templateListBox__currentItemChanged(templateListBox_->item(0));
    accept();
}

void QtNewDocDialog::on_templateListBox__currentItemChanged(
    QListWidgetItem* dt, QListWidgetItem*)
{
    currentTemplate_ = static_cast<TemplateItem*>(dt);
    updateResult(false);
}

void QtNewDocDialog::selectDocumentUrl(const String& url)
{
    if (isChooseTemplate_ || url.isEmpty())
        return;
    String path = (Url(url).isRelative())
        ? String(Url(docpath_).combinePath2Path(url)) : url;
    filenameEdit_->setText(path);
    on_filenameEdit__textChanged(path);
}

void QtNewDocDialog::on_detailsButton__toggled(bool isToShow)
{
    setSizeGripEnabled(!isToShow);
    showExtension(isToShow);
}

void QtNewDocDialog::on_filenameEdit__textChanged(const QString& filename)
{
    bool ok = !filename.isEmpty();
    okButton_->setEnabled(ok);
    if (ok)
        okButton_->setDefault(true);
    String path = filenameEdit_->text();
    if (Url(path).isRelative())
        docpath_ = Url(docpath_).combinePath2Path(path);
    else
        docpath_ = path;
    filenameEdit_->setToolTip(docpath_);
}

void QtNewDocDialog::on_templateListBox__itemDoubleClicked(
    QListWidgetItem* docTemplate)
{
    on_templateListBox__currentItemChanged(docTemplate);
    accept();
}

void QtNewDocDialog::accept()
{
    root()->merge(details_->getProps(), true);
    root()->makeDescendant(DOC_PATH, docpath_, true);
    if (!isChooseTemplate_ && PathName(docpath_).exists()) {
        if (QMessageBox::warning(
                this, tr("New File"),
                tr("<qt><nobr>File %1 already exists.</nobr>\n<b>"
                   "Overwrite?</b></qt>").arg(docpath_),
                   QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
            return;
    }
    String doc_enc =
        docparam_.root()->getSafeProperty(DOC_ENCODING)->getString();
    String my_enc  = root()->getSafeProperty(ENCODING)->getString();
    if (isChooseTemplate_ && !doc_enc.isEmpty() &&
        doc_enc.lower() != my_enc.lower() && my_enc.lower() != NOTR("xml"))
        QMessageBox::warning(
            this, tr("Warning"),
            tr("Document will be read with encoding %0, which is not %1, "
               "declared in the document.").arg(my_enc).arg(doc_enc),
            tr("&OK"));
    root()->merge(params_.pointer(), false);
    QDialog::accept();
}

void QtNewDocDialog::help()
{
    helpAssistant().show(isChooseTemplate_ ? DOCTAG(OPEN_D_WITH_TMPL)
                                           : DOCTAG(UG_NEW_DOC_D));
}

/////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(NewDocDialog, SernaDoc)

bool NewDocDialog::doExecute(SernaDoc* doc, EventData* result)
{
    QtNewDocDialog d(doc, ed_, false);
    if (d.exec() != QDialog::Accepted)
        return false;
    PropertyNode* ptree = static_cast<PropertyTreeEventData*>(result)->root();
    ptree->merge(d.root(), true);
    return true;
}

///////////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(ChooseDocTemplate, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(ChooseDocTemplate, "PropertyTree", "PropertyTree")

bool ChooseDocTemplate::doExecute(SernaDoc* doc, EventData* result)
{
    QtNewDocDialog d(doc, ed_, true);
    if (d.exec() != QDialog::Accepted)
        return false;
    PropertyNode* ptree = static_cast<PropertyTreeEventData*>(result)->root();
    ptree->merge(d.root(), true);
    return true;
}

///////////////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(NativeOpenDocDialog, SernaDoc)

bool NativeOpenDocDialog::doExecute(SernaDoc* doc, EventData* result)
{
    String caption = (ed_->getProperty("#open-with-template"))
        ? tr("Open Document with Template")
        : tr("Open Document");
    String filename = QFileDialog::getOpenFileName(doc->widget(0), caption,
        ed_->getSafeProperty(BROWSE_DIR)->getString(),
        opendoc_filters(config().root()));
    static_cast<PropertyTreeEventData*>(result)->root()->
        makeDescendant(DOC_PATH, filename, true);
    return !filename.isEmpty();
}

#include "moc/NewDocDialog.moc"

////////////////////////////////////////////////////////////////////////

static void set_inactive(QLineEdit* w)
{
    w->setReadOnly(true);
    w->setPaletteBackgroundColor(w->palette().inactive().background());
}

TemplateDetails::TemplateDetails(SernaDoc* doc,
                                 const Common::PropertyNode* pn,
                                 bool isChooseTemplate)
    : isChooseTemplate_(isChooseTemplate)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
        SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    browsePath_ = pn->getSafeProperty(NOTR("base-url"))->getString();

    schemaBrowseMenu_ =
        new BrowseButtonMenu(doc, browseSchema_, browsePath_, false);
    schemaBrowseMenu_->setCaption(tr("Select XML Schema"));
    schemaBrowseMenu_->setFilters(
        tr("XML Schema files (*.xsd);;All files (*.*)"));
    connect(schemaBrowseMenu_, SIGNAL(urlSelected(const Common::String&)),
            this, SLOT(selectSchemaUrl(const Common::String&)));

    styleBrowseMenu_ =
        new BrowseButtonMenu(doc, browseStyle_, browsePath_, false);
    styleBrowseMenu_->setCaption(tr("Select XSL Stylesheet"));
    styleBrowseMenu_->setFilters(
        tr("XSL Stylesheets (*.xsl);;All files (*.*)"));
    connect(styleBrowseMenu_, SIGNAL(urlSelected(const Common::String&)),
            this, SLOT(selectStyleUrl(const Common::String&)));

    /////////
    root()->setName(pn->name());
    if (isChooseTemplate_) {
        set_inactive(dtdPubid_);
        set_inactive(dtdSysid_);
        set_inactive(rootElem_);
    }
    else {
        rootElemInscription_->hide();
        rootElem_->hide();
    }
    int lastEnc = (isChooseTemplate_)
        ? Encodings::LAST_ENCODING
        : Encodings::XML;
    for (int enc = Encodings::FIRST_ENCODING; enc < lastEnc; ++enc)
        encoding_->insertItem(Encodings::encodingName(
                                  Encodings::Encoding(enc)).qstring());
    update(pn);
    root()->makeDescendant(DocSrcInfo::BROWSE_DIR)->setString(browsePath_);

    if (isChooseTemplate_) {
        encoding_->setCurrentItem(Encodings::XML);
    }
    else {
        encoding_->setCurrentItem(Encodings::UTF_8);
        on_dtdSysid__textChanged();
    }
}

TemplateDetails::~TemplateDetails()
{
}

PropertyNode* TemplateDetails::getProps()
{
    root()->removeAllChildren();
    if (!isChooseTemplate_) {
        root()->makeDescendant(DOCINFO_PUBID, dtdPubid_->text(), true);
        root()->makeDescendant(DOCINFO_SYSID, dtdSysid_->text(), true);
    }
    root()->makeDescendant(SCHEMA_PATH, schema_->text(), true);
    root()->makeDescendant(STYLE_PATH, style_->text(), true);
    root()->makeDescendant(ENCODING, Encodings::encodingName
        (Encodings::Encoding(encoding_->currentItem())), true);
    return root();
}

static void set_lineedit(const PropertyNode* pn, QLineEdit* le,
                         const String& prop, bool isChooseTemplate)
{
    String s = pn->getSafeProperty(prop)->getString();
    if (isChooseTemplate && s.isEmpty())
        le->setText(qApp->translate("TemplateDetails", "Not Specified"));
    else
        le->setText(s);
}

void TemplateDetails::update(const PropertyNode* pn)
{
    set_lineedit(pn, rootElem_, DOCINFO_DOCELEM, isChooseTemplate_);
    set_lineedit(pn, dtdPubid_, DOCINFO_PUBID, isChooseTemplate_);
    set_lineedit(pn, dtdSysid_, DOCINFO_SYSID, isChooseTemplate_);

    schema_->setText(pn->getSafeProperty(SCHEMA_PATH)->getString());
    if (!schema_->text().isEmpty()) {
        String schema_url(config().resolveResource(
                              SCHEMA_PATH, schema_->text(),
                              browsePath_, pn));
        schemaBrowseMenu_->setBaseUrl(schema_url);
    }
    else
        schemaBrowseMenu_->setBaseUrl(browsePath_);

    style_->setText(pn->getSafeProperty(STYLE_PATH)->getString());
    if (!style_->text().isEmpty())
        styleBrowseMenu_->setBaseUrl(
            config().resolveResource(STYLE_PATH, style_->text(), browsePath_));
    else
        styleBrowseMenu_->setBaseUrl(browsePath_);

    if (isChooseTemplate_) {
        String enc =  pn->getSafeProperty(ENCODING)->getString();
        if (enc.isEmpty())
            enc = NOTR("xml");
        encoding_->setCurrentItem(Encodings::encodingByName(enc));
    }
}

void TemplateDetails::selectSchemaUrl(const String& url)
{
    schema_->setText(url);
}

void TemplateDetails::selectStyleUrl(const String& url)
{
    style_->setText(url);
}

void TemplateDetails::on_dtdSysid__textChanged()
{
    bool v = dtdSysid_->text().isEmpty();
    if (v == dtdPubid_->isReadOnly())
        return;
    if (v) {
        dtdPubid_->clear();
        dtdPubid_->setReadOnly(true);
        dtdPubid_->setEnabled(false);
    }
    else {
        dtdPubid_->setReadOnly(false);
        dtdPubid_->setEnabled(true);
    }
}

void TemplateDetails::on_helpButton__clicked()
{
    helpAssistant().show(isChooseTemplate_ ? DOCTAG(UG_OPEN_DOC_D)
                                           : DOCTAG(UG_NEW_DOC_D));
}
