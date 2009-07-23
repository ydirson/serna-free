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
#include "structeditor/StructEditor.h"
#include "structeditor/impl/XsUtils.h"

#include "docview/SernaDoc.h"
#include "docview/qt/BrowseButtonMenu.h"

#include "grove/Decls.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/IdManager.h"
#include "grove/GroveBuilder.h"
#include "grove/SectionSyncher.h"
#include "grove/xinclude.h"
#include "grove/XmlPredefNs.h"
#include "grove/XmlValidator.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "docutils/doctags.h"

#include "common/MessageUtils.h"
#include "common/String.h"
#include "common/MessageUtils.h"
#include "common/common_defs.h"
#include "common/Url.h"

#include "utils/Properties.h"
#include "utils/HelpAssistant.h"
#include "utils/GroveProperty.h"
#include "utils/MessageListProperty.h"
#include "common/PropertyTreeEventData.h"

#include "dav/DavManager.h"
#include "dav/IoStream.h"
#include "xs/Schema.h"

#include <QShortcut>
#include <iostream>

#include "structeditor/InsertXincludeDialogBase.hpp"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace AttributesSpace;

class SetElementAttributes;

class StructEditor;

class InsertXincludeDialogImpl : public QDialog, 
                                 protected Ui::InsertXincludeDialogBase {
    Q_OBJECT

public:
    InsertXincludeDialogImpl(StructEditor* se, bool isText_, bool isNew);
    ~InsertXincludeDialogImpl();

    Grove*          grove() const { return grove_.pointer(); }
    const String&   selectedId() const { return selectedId_; }
    MessageList&    messageList() { return messageList_; }
    PropertyNode*   attrs() const { return attrs_.root(); }
    const String&   encoding() const { return encoding_; }
    String          url() const;

public slots:
    void    on_okButton__clicked();
    void    on_filenameLineEdit__returnPressed();
    void    on_attributesButton__clicked();
    void    on_helpButton__clicked() {
        helpAssistant().show(DOCTAG(WORKING_W_XINCLUDE));
    }

    void            setFilename(const Common::String& filename);

private:
    bool            isValidFilename();
    void            setEncodingLabel();

    StructEditor*           se_;
    GrovePtr                grove_;
    String                  validFilename_;
    MessageList             messageList_;
    String                  selectedId_;
    QPointer<BrowseButtonMenu>  browseMenu_;
    String                  encoding_;
    PropertyTreeEventData   attrs_;
    bool                    isText_;
    bool                    isNew_;
};

InsertXincludeDialogImpl::InsertXincludeDialogImpl(StructEditor* se,
                                                   bool isText, bool isNew)
    : QDialog(se->widget()),
      se_(se), 
      isText_(isText), 
      isNew_(isNew)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    //! Setup brose menu button
    browseMenu_ = new BrowseButtonMenu(
        se_->sernaDoc(), browseButton_, 
        se->editViewSrcPos().getErs()->xmlBase(), isNew);
    if (isText_)
        browseMenu_->setFilters(tr("All files (*)"));
    connect(browseMenu_, SIGNAL(urlSelected(const Common::String&)),
            this, SLOT(setFilename(const Common::String&)));

    //! Hide unnecessary fields
    if (isText_ || isNew) {
        idLabel_->hide();
        idComboBox_->hide();
    }
    setEncodingLabel();
    setFixedHeight(minimumHeight());

    //! Setup properties for editing xinclude properties
    attrs_.root()->setName(NOTR("attributes"));
    PropertyNodePtr attr_specs = new PropertyNode(ATTR_SPEC_LIST);
    PropertyNode* parse_prop = new PropertyNode(NOTR("xi:parse"));
    parse_prop->setString(W3C_XINCLUDE_NAMESPACE);
    parse_prop->makeDescendant(Xs::FIXED_ATTR_VALUE)->setString(
        (isText_) ? NOTR("text") : NOTR("xml"));
    attr_specs->appendChild(parse_prop);
    PropertyNode* existing_attrs =
        attrs_.root()->makeDescendant(EXISTING_ATTRS);
    existing_attrs->makeDescendant(NOTR("xi:parse"))->
        setString(isText_ ? NOTR("text") : NOTR("xml"));
    PropertyNode* encoding_prop = new PropertyNode(NOTR("xi:encoding"));
    encoding_prop->setString(W3C_XINCLUDE_NAMESPACE);
    attr_specs->appendChild(encoding_prop);
    PropertyNode* enc_enum = encoding_prop->makeDescendant(ATTR_VALUE_ENUM);
    for (int i = Encodings::FIRST_ENCODING; i < Encodings::XML; ++i)
        enc_enum->appendChild(new PropertyNode
            (Encodings::encodingName((Encodings::Encoding)i)));
    attrs_.root()->appendChild(attr_specs.pointer());
    attrs_.root()->makeDescendant(
        ElementSpace::ELEMENT_NAME)->setString(NOTR("xi:include"));
}

InsertXincludeDialogImpl::~InsertXincludeDialogImpl()
{
}

void InsertXincludeDialogImpl::setEncodingLabel()
{
    if (encoding_.isEmpty())
        encodingLabel_->setText(tr("<b>As in Parent Document</b>"));
    else
        encodingLabel_->setText(NOTR("<b>") + encoding_ + "</b>");
}

void InsertXincludeDialogImpl::on_attributesButton__clicked()
{
    if (!makeCommand<SetElementAttributes>(&attrs_)->execute(se_, &attrs_))
        return;
    encoding_ = attrs_.root()->getSafeProperty(EXISTING_ATTRS)->
        getSafeProperty("xi:encoding")->getString();
    setEncodingLabel();
}

void InsertXincludeDialogImpl::on_okButton__clicked()
{
    if (isNew_ && !isValidFilename())
        return;
    if (idComboBox_->currentItem() > 0)
        selectedId_ = idComboBox_->currentText();
    else
        selectedId_ = String();
    QDialog::accept();
}

String InsertXincludeDialogImpl::url() const
{
    Url base(se_->editViewSrcPos().getErs()->xmlBase());
    Url fn_url(String(filenameLineEdit_->text()));
    return fn_url.isRelative() ? base.combinePath2Path(fn_url) : fn_url;
}

void InsertXincludeDialogImpl::setFilename(const String& filename) 
{
    Url base(se_->editViewSrcPos().getErs()->xmlBase());
    Url fn_url(filename);
    if (!fn_url.isRelative())
        filenameLineEdit_->setText(String(base.relativePath(fn_url)));
    else
        filenameLineEdit_->setText(filename);
    on_filenameLineEdit__returnPressed();
}

void InsertXincludeDialogImpl::on_filenameLineEdit__returnPressed()
{
    const bool is_valid = isValidFilename();
    okButton_->setEnabled(isNew_ || is_valid);
}

bool InsertXincludeDialogImpl::isValidFilename()
{
    GrovePos pos = se_->editViewSrcPos();
    //! If new xinclude document should be created, ensure that it is not
    //  already included or used as external entity
    if (isNew_) {
        Grove* target_grove = pos.node()->grove();
        if (check_has_entity(target_grove, url())) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Cannot Xinclude XML document"),
                tr("Used as external entity"), tr("&Ok"));
            return false;
        }
        if (find_grove(target_grove, url())) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Cannot Xinclude XML document"),
                tr("Already included"), tr("&Ok"));
            return false;
        }    
    } 
    else {
        //! Build grove and collect id`s
        grove_ = se_->groveEditor()->buildXincludeGrove(
            pos, url(), encoding_, isText_);
        if (grove_.isNull()) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Cannot Xinclude XML document"),
                se_->groveEditor()->errorMessage(), tr("&Ok"));
            return false;
        }
        encoding_ = Encodings::encodingName(
            grove_->groveBuilder()->getEncoding());
        setEncodingLabel();

        if (!isText_) {
            idComboBox_->clear();
            idComboBox_->insertItem(tr("<Insert Whole Document>"));
            PropertyTree id_list;
            if (grove_->idManager())
                grove_->idManager()->getIdList(id_list.root());
            for (const PropertyNode* id = id_list.root()->firstChild(); id; 
                 id = id->nextSibling())
                idComboBox_->insertItem(id->name());
        }
    }
    validFilename_ = url();
    return true;
}

////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(InsertXincludeDialog, StructEditor)

bool InsertXincludeDialog::doExecute(StructEditor* se, EventData* ed)
{
    const PropertyNode* props =
        static_cast<const PropertyTreeEventData*>(ed)->root();
    InsertXincludeDialogImpl dialog(se, props->getProperty("is-text"),
                                    props->getProperty("is-new"));
    dialog.setCaption(props->getSafeProperty("caption")->getString());
    if (dialog.exec() != QDialog::Accepted)
        return false;

    PropertyNode* pn = static_cast<PropertyTreeEventData*>(ed)->root();
    pn->removeAllChildren();
    pn->appendChild(new PropertyNode(NOTR("href"), dialog.url()));
    pn->appendChild(new PropertyNode(NOTR("xpointer"), dialog.selectedId()));
    pn->appendChild(new GroveProperty(dialog.grove()));
    pn->appendChild(new MessageListProperty(dialog.messageList()));
    if (!dialog.encoding().isEmpty())
        pn->makeDescendant("encoding")->setString(dialog.encoding());
    PropertyNode* p = dialog.attrs()->getProperty(EXISTING_ATTRS);
    if (p)
        p = p->firstChild();
    for (; p; p = p->nextSibling()) {
        if (p->name() == NOTR("xi:encoding") || p->name() == NOTR("xi:parse"))
            continue;
        pn->makeDescendant("attrs")->appendChild(p->copy());
    }
    return true;
}

#include "moc/InsertXincludeDialog.moc"
