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
#include "structeditor/SernaDragData.h"
#include "structeditor/StructEditor.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/ElementList.h"
#include "docview/SernaDoc.h"

#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "grove/Nodes.h"
#include "ui/QtObjectTracker.h"
#include "ui/MimeHandler.h"
#include "xs/XsNodeExt.h"
#include "utils/DocSrcInfo.h"
#include "structeditor/impl/debug_se.h"

#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QDragMoveEvent>
#include <QStringList>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace Sui;

static const int DND_OP_CLASS =
    StructEditor::SILENT_OP|StructEditor::ALLOW_RDONLY|StructEditor::STRUCT_OP; 

static const char* DOCFRAG_MIMEPREFIX = NOTR("df");

String SernaDocFragment::fragMimeType(const Common::PropertyNode* pn)
{
    using namespace DocSrcInfo;
    String mt(DOCFRAG_MIMEPREFIX);
    if (0 == pn)
        return mt;
    String category(pn->getString(TEMPLATE_CATEGORY));
    if (category.isEmpty())
        return mt;
    mt.reserve(128);
    mt.append("-").append(category).append(NOTR("--")).
        append(pn->getString(TEMPLATE_NAME));
    return mt;
}

//////////////////////////////////////////////////////////////////

SernaDocFragment::SernaDocFragment()
    : firstElem_(0), hasText_(false)
{
}

DocumentFragment* SernaDocFragment::clipboard() const
{
    return clipboard_.pointer(); 
}

void SernaDocFragment::setFragment(DocumentFragment* df)
{
    firstElem_ = 0;
    hasText_   = false;
    if (0 == df)
        return;
    clipboard_ = df;
    const Node* n = clipboard_->firstChild();
    for (; n && !(hasText_ && firstElem_); n = n->nextSibling()) {
        switch (n->nodeType()) {
            case Node::TEXT_NODE:
                if (CONST_TEXT_CAST(n)->data().length())
                    hasText_ = true;
                continue;
            case Node::ELEMENT_NODE:
                if (!firstElem_)
                    firstElem_ = CONST_ELEMENT_CAST(n);
                continue;
            default:
                continue;
        }
    }
    DBG(SE.DRAG) << "SernaDocFragment: setFrag, hasText_=" << hasText_
        << std::endl;
}

bool SernaDocFragment::is_within_range() const
{
    return false;
}

GrovePos SernaDocFragment::fromPos() const
{
    return GrovePos();
}

GrovePos SernaDocFragment::toPos() const
{
    return GrovePos();
}

bool SernaDocFragment::validate(QDropEvent* event)
{
    if (clipboard_.isNull() || !clipboard_->firstChild()) {
        DBG(SE.DRAGV) << "DND: validate_pos: fail because of empty clipboard\n";
        return false;
    }
    if (is_within_range()) {
        DBG(SE.DRAGV) << "DND: validate_pos: fail (within range)\n";
        return false;
    } else {
        DBG(SE.DRAGV) << "DND: validate_pos OK\n";
        return valid_at(event);
    }
}

bool SernaDocFragment::valid_at(QDropEvent*) const
{
    StructEditor* const se = dropStructEditor_;
    const int opClass = (clipboard_->firstChild() && 
        clipboard_->firstChild() == clipboard_->lastChild() &&
        clipboard_->firstChild()->nodeType() == Node::TEXT_NODE) 
            ? StructEditor::TEXT_OP|StructEditor::SILENT_OP
            : StructEditor::STRUCT_OP|StructEditor::SILENT_OP;
    // validate entity references for recursion
    const EntityReferenceStart* doc_ers = dropPos_.getErs();
    if (!doc_ers)
        return false;
    const EntityReferenceStart* ers = clipboard_->ers() ?
        CONST_ERS_CAST(clipboard_->ers()->getFirstSect()) : 0;
    for (; ers; ers = CONST_ERS_CAST(ers->GroveSectionStart::getNextSect())) {
        const EntityReferenceStart* ce = doc_ers;
        for (; ce; ce = CONST_ERS_CAST(ce->getSectParent())) 
            if (ce->entityDecl() == ers->entityDecl()) {
                DBG(SE.DRAGV) << "DND: validAt: FAILED, recursive entity\n";
                return false;
            }
    }
    //if (!from_pos.isNull() && event->action() == QDragMoveEvent::Move && 
    //    se->isEditableEntity(from_pos, opClass) != StructEditor::POS_OK) {
    //        DBG(SE.DRAG) << "DND: validAt: FAILED, not editable from_\n";
    //        return false;
    //}
    if (se->isEditableEntity(dropPos_, opClass) != StructEditor::POS_OK) {
        DBG(SE.DRAGV) << "DND: validAt: FAILED, drop_pos not editable\n";
        return false; 
    }
    if (!se->isValidationOn()) {
        DBG(SE.DRAGV) << "DND: validAt: OK (no validation)\n";
        return true;
    }
    const Element* elem = traverse_to_element(dropPos_);
    if (0 == elem) {
        DBG(SE.DRAGV) << "DND: validAt: FAILED (traverse_to_element)\n";
        return false;
    }
    const Xs::XsElement* xselem = XsNodeExt::xsElement(elem);
    if (0 == xselem) {
        DBG(SE.DRAGV) << "DND: validAt: OK (element not validated)\n";
        return true;    // validation off
    }
    if (hasText_ && !xselem->cdataAllowed()) {
        DBG(SE.DRAGV) << "DND: validAt: FAILED (no text allowed)\n";
        return false;
    }
    if (!firstElem_) {
        DBG(SE.DRAGV) << "DND: validAt: OK (no firstElem)\n";
        return true;
    }
    String ns = firstElem_->xmlNsUri();
    if (ns.isNull() && !fromPos().isNull()) {
        // resolve namespace in context of fromPos
        elem = traverse_to_element(fromPos());
        ns = elem ? elem->getXmlNsByPrefix(firstElem_->xmlNsPrefix()) :
            XmlNs::defaultNs();
    }
    if (se->elementList().canInsert(firstElem_->nodeName(), ns, &dropPos_)) {
        DBG(SE.DRAGV) << "DND: validAt: OK (canInsert)\n";
        return true;
    } else {
        DBG(SE.DRAGV) << "DND: validAt: FAIL (canInsert), {" << ns
            << "}" << firstElem_->nodeName() << std::endl;
        return false;
    }
}                           
    
void SernaDocFragment::setDropData(const GrovePos& dpos,
                                   StructEditor* dse)
{                             
    dropPos_ = dpos;
    dropStructEditor_ = dse;
}

SernaDocFragment* SernaDocFragment::fragment(const MimeHandler& mh,
                                             const String& mt,
                                             bool useDefault)
{
    SernaApiBase* obj = mh.getObject(mt);
    if (0 == obj && useDefault)
        obj = mh.getObject(DOCFRAG_MIMEPREFIX, true);  // get any frag
    return dynamic_cast<SernaDocFragment*>(obj);
}

SernaDocFragment* SernaDocFragment::fragment(const SernaDoc* sd)
{
    return fragment(sd->mimeHandler(), fragMimeType(sd->getDsi()));
}

SernaDocFragment* SernaDocFragment::newFragment(MimeHandler& mh, 
                                                const Common::String& mt)
{
    SernaDocFragment* new_sdf = new SernaDocFragment; 
    mh.addObject(new_sdf, mt);
    return new_sdf;
}

bool SernaDocFragment::dropContentCopy()
{
    if (clipboard_.isNull() || !dropStructEditor_)
        return false;
    DBG(SE.DRAG) << "DND: SernaDragData:: dropContentCopy\n";
    dropStructEditor_->executeAndUpdate(dropStructEditor_->
        groveEditor()->paste(clipboard_, dropPos_).pointer());
    dropStructEditor_ = 0;
    return true;
}

bool SernaDocFragment::dropContentMove()
{
    return dropContentCopy();
}

bool SernaDocFragment::dragEnter(const StructEditor* se,
                                 QDragEnterEvent* event)
{
    MimeHandler& mh = se->sernaDoc()->mimeHandler();
    mh.dragEnter(event);
    String target_mt = SernaDocFragment::fragMimeType(se->getDsi());
    DBG_IF(SE.DRAG) {
        QStringList qsl = mh.mimeData()->formats();
        DBG(SE.DRAG) << "SernaDocFrag: dragEnter: formats before=" 
            << String(qsl.join(",")) << std::endl;
    }
    if (!mh.getObject(fragMimeType(0), true)) {
        DBG(SE.DRAG) << "SernaDocFrag: dragEnter: creating frag\n";
        mh.callback(MimeCallback::CREATE_FRAGMENT, event);
    }
    if (!mh.getObject(target_mt)) {
        DBG(SE.DRAG) << "SernaDocFrag: dragEnter: no matching dt frag, conv\n";
        mh.callback(MimeCallback::TRANSFORM, event);
    }
    DBG_IF(SE.DRAG) {
        QStringList qsl = mh.mimeData()->formats();
        DBG(SE.DRAG) << "SernaDocFrag: dragEnter: formats after=" 
            << String(qsl.join(",")) << std::endl;
    }
    event->acceptProposedAction();
    return true;
}

SernaDocFragment::~SernaDocFragment()
{
    DBG(SE.DRAG) << "~SernaDocFragment: " << this << std::endl;
}

///////////////////////////////////////////////////////////////////

SernaDragData::SernaDragData(StructEditor* se)
    : structEditor_(se),
      allowedActions_(Qt::IgnoreAction),
      completed_(false)
{
    GroveEditor::GrovePos from, to;
    structEditor_->getSelection(from, to, DND_OP_CLASS);
    setSelection(from, to);
    se->sernaDoc()->mimeHandler().addObject(this, fragMimeType(se->getDsi()));
    DBG(SE.DRAG) << "SernaDragData cons: " << this << std::endl;
}

GrovePos SernaDragData::fromPos() const
{
    return from_;
}

GrovePos SernaDragData::toPos() const
{
    return from_;
}

bool SernaDragData::is_within_range() const
{
    return GroveEditor::is_within_range(dropPos_, from_, to_, true);
}

void SernaDragData::setSelection(const GroveEditor::GrovePos& from, 
                                 const GroveEditor::GrovePos& to)
{
    from_ = from;
    to_   = to;

    // TODO: use setFragment, cannot access fields from bae cons
    RefCntPtr<DocumentFragment> df = new DocumentFragment;
    try {
        structEditor_->groveEditor()->copy(from_, to_, df.pointer());
    } catch (...) {
        DBG(SE.DRAG) << "SernaDragData: exception\n" << std::endl;
        return;
    }
    setFragment(df.pointer());

    allowedActions_ = Qt::IgnoreAction;
    if (from_.isNull() || to_.isNull()) {
        DBG(SE.DRAG) << "DND: checkSelection: start/end pos null\n";
        return;
    }
    StructEditor::PositionStatus ps1 = 
        structEditor_->isEditableEntity(from_, DND_OP_CLASS);
    StructEditor::PositionStatus ps2 = 
        structEditor_->isEditableEntity(to_, DND_OP_CLASS);
    if (ps1 == StructEditor::POS_FAIL || ps2 == StructEditor::POS_FAIL) {
        DBG(SE.DRAG) << "DND: checkSelection: posFail\n";
        return;
    }
    if (ps1 == StructEditor::POS_OK && ps2 == StructEditor::POS_OK) 
        allowedActions_ = Qt::CopyAction|Qt::MoveAction|Qt::LinkAction;
    else
        allowedActions_ = Qt::CopyAction;
    DBG(SE.DRAG) << "DND: checkSelection: allowedActions="
        << allowedActions_ << std::endl;
} 

bool SernaDragData::validate(QDropEvent* event)
{
    if (!clipboard() || !clipboard()->firstChild()) {
        DBG(SE.DRAGV) << "DND: validate_pos: fail because of empty clipboard\n";
        return false;
    }
    if (event->proposedAction() != Qt::MoveAction || 
        dropStructEditor_ != structEditor_)
            return valid_at(event);
    return SernaDocFragment::validate(event);
}

bool SernaDragData::dropContentMove()
{
    if (!clipboard() || !dropStructEditor_ || dropPos_.isNull())
        return false;
    if (completed_)
        return true;
    DBG(SE.DRAG) << "DND: SernaDragData:: dropContentMove\n";
    GrovePos initial_pos = dropStructEditor_->editViewSrcPos();
    Editor* editor = dropStructEditor_->groveEditor();
    dropPos_ = dropPos_.adjustChoice();
    DBG(SE.DRAG) << "DND: dropPos_ = "; DBG_IF(SE.DRAG) dropPos_.dump();
    if (structEditor_ == dropStructEditor_) {
        RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
        batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Drag`n`Drop"));
        batch_cmd->executeAndAdd(
            editor->cut(from_, to_, &dropPos_).pointer());
        batch_cmd->executeAndAdd(
            editor->paste(clipboard(), dropPos_).pointer());
        batch_cmd->setUndoPos(initial_pos);
        dropStructEditor_->executeAndUpdate(batch_cmd.pointer());
    } else {
        structEditor_->executeAndUpdate(structEditor_->groveEditor()->cut(
            from_, to_).pointer());
        dropStructEditor_->executeAndUpdate(editor->paste(clipboard(),
            dropPos_).pointer());
    }
    completed_ = true;
    return true;
}

void SernaDragData::startDrag(QDrag* drag, MimeHandler& mh)
{
    DBG(SE.DRAG) << "SernaDragData: Start Drag!\n";
    mh.callback(MimeCallback::CREATE_CONTENT, 0);  
    drag->exec(allowedActions(), 
        (allowedActions() & Qt::MoveAction) ? Qt::MoveAction : Qt::CopyAction);
}

SernaDragData::~SernaDragData()
{
    DBG(SE.DRAG) << "~SernaDragData: " << this << std::endl;
}

/////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(CreateFragmentFromText, CREATE_FRAGMENT, 
    virtual Common::String mimeType() const;)

Common::String CreateFragmentFromText::mimeType() const
{
    return NOTR("text/plain");
}

bool CreateFragmentFromText::execute(MimeHandler& mh, QDropEvent*) 
{
    RT_ASSERT(mh.mimeData()->hasText());
    String text = mh.mimeData()->text();
    DBG(SE.DRAG) << "CreateFragFromText: execute, text=" << text << std::endl;
    DocumentFragment* df = new DocumentFragment;
    Text* text_node = new Text(text);
    df->appendChild(text_node);
    SernaDocFragment* sdf = new SernaDocFragment; 
    sdf->setFragment(df);
    mh.addObject(sdf, CustomMimetypeCallback::mimeType());
    return true;
}

////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(ValidateDocFragment,    VALIDATE_CONTENT,  /**/)

bool ValidateDocFragment::execute(MimeHandler& mh, QDropEvent* event)
{
    SernaDocFragment* sdf = SernaDocFragment::fragment(mh, mimeType());
    if (0 == sdf)
        return false;
    bool result = sdf->validate(event);
    DBG(SE.DRAGV) << "ValidateDocFragment: execute, result=" << result 
        << std::endl;
    return result;
}

////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(DropDocFragmentCopy,    DROP_COPY_CONTENT, /**/)

bool DropDocFragmentCopy::execute(MimeHandler& mh, QDropEvent*) 
{
    SernaDocFragment* sdf = SernaDocFragment::fragment(mh, mimeType());
    if (0 == sdf)
        return false;
    bool result = sdf->dropContentCopy();
    DBG(SE.DRAG) << "DropContentCopy: execute, result=" << result 
        << std::endl;
    return result;
}

////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(DropDocFragmentMove,    DROP_MOVE_CONTENT, /**/)

bool DropDocFragmentMove::execute(MimeHandler& mh, QDropEvent*) 
{
    SernaDocFragment* sdf = SernaDocFragment::fragment(mh, mimeType());
    if (0 == sdf)
        return false;
    bool result = sdf->dropContentMove();
    DBG(SE.DRAG) << "DropContentMove: execute, result=" << result 
        << std::endl;
    return result;
}

////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(DefaultFragTransform,   TRANSFORM, /**/)

bool DefaultFragTransform::execute(MimeHandler& mh, QDropEvent*) 
{
    SernaDocFragment* sdf = SernaDocFragment::fragment(mh, mimeType(), false);
    DBG(SE.DRAG) << "DefaultFragTransform: have=" << !!sdf << std::endl;
    if (sdf)
        return true;    // already have proper target
    sdf = SernaDocFragment::fragment(mh, mimeType(), true);
    if (0 == sdf)
        return false;   // no fragments at all
    SernaDocFragment::newFragment(mh, mimeType())->
        setFragment(sdf->clipboard());
    DBG(SE.DRAG) << "DefautFragTransform: creating " << mimeType()
        << std::endl;
    return true;
}

////////////////////////////////////////////////////////////////

CUSTOM_MIMETYPE_CALLBACK_DECL(CreateTextFromFragment, CREATE_CONTENT, 
    virtual Common::String mimeType() const;)

Common::String CreateTextFromFragment::mimeType() const
{
    return NOTR("text/plain");
}

bool CreateTextFromFragment::execute(MimeHandler& mh, QDropEvent*) 
{
    if (mh.mimeData()->hasFormat(mimeType()))
        return true;
    SernaDocFragment* sdf = SernaDocFragment::fragment(mh, 
        CustomMimetypeCallback::mimeType(), true);
    if (!sdf || !sdf->clipboard())
        return false;   // no fragment
    DBG(SE.DRAG) << "DND: Creating text from fragment\n";
    const StripInfo* si = sdf->structEditor() ?
        sdf->structEditor()->stripInfo() : 0;
    String result;
    sdf->clipboard()->saveAsXmlString(result, Grove::GS_SAVE_CONTENT|
        (si ? Grove::GS_INDENT : 0), si);
    mh.mimeData()->setText(result);
    return true;
}

////////////////////////////////////////////////////////////////

STRUCTEDITOR_EXPIMP void register_struct_dnd_callbacks(SernaDoc* sd)
{
    String dmt(SernaDocFragment::fragMimeType(sd->getDsi()));
    sd->mimeHandler().registerCallback(new CreateFragmentFromText(dmt), 100);
    sd->mimeHandler().registerCallback(new ValidateDocFragment(dmt),    100);
    sd->mimeHandler().registerCallback(new DropDocFragmentCopy(dmt),    100);
    sd->mimeHandler().registerCallback(new DropDocFragmentMove(dmt),    100);
    sd->mimeHandler().registerCallback(new DefaultFragTransform(dmt),   100);
    sd->mimeHandler().registerCallback(new CreateTextFromFragment(dmt), 100);
}

