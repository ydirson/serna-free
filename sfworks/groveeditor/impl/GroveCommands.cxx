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
#ifdef _MSC_VER
# pragma warning ( disable: 4786 4251)
#endif

#include "groveeditor/GroveCommands.h"
#include "groveeditor/Exception.h"
#include "groveeditor/groveeditor_debug.h"

#include "common/asserts.h"
#include "common/safecast.h"
#include "common/TreelocRep.h"
#include "common/PropertyTree.h"

#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/XNodePtr.h"
#include "grove/XmlNs.h"
#include "grove/SectionSyncher.h"
#include "grove/EntityReferenceTable.h"
#include "grove/ChoiceNode.h"

using namespace Common;
using namespace GroveLib;

#ifndef QT_TRANSLATE_NOOP
# define QT_TRANSLATE_NOOP(context, text) text
#endif // QT_TRANSLATE_NOOP

namespace GroveEditor
{

static bool check_xinclude_removal(const DocumentFragment* df,
                                   const Node* context_node)
{
    if (0 == df || 0 == df->ers() || !df->ers()->getFirstSect())
        return true;
    const EntityReferenceStart* ers = get_ers(context_node);
    const EntityReferenceStart* source_ers = ers;
    for (; ers && ers->getSectParent();
        ers = CONST_ERS_CAST(ers->getSectParent()))
            if (ers->entityDecl()->declType() == EntityDecl::xinclude)
                break;
    if (0 == ers || 0 == ers->getSectParent())
        return true;
    const Grove* refGrove = static_cast<const XincludeDecl*>
        (ers->entityDecl())->referenceErs()->grove();
    ers = CONST_ERS_CAST(df->ers()->getFirstSect());
    bool nested_xinclude = false;
    for (; ers; ers = CONST_ERS_CAST(ers->getNextSect())) {
        if (ers->entityDecl()->declType() != EntityDecl::xinclude)
            continue;
        const XincludeDecl* xd = 
            static_cast<const XincludeDecl*>(ers->entityDecl());
        if (xd->referenceErs() && xd->referenceErs()->grove() == refGrove) {
                nested_xinclude = true;
                break;
        }
    }
    if (nested_xinclude) {
        const EntityReferenceTable::ErtEntry* ee =
            context_node->getGSR()->ert()->lookup(source_ers->entityDecl());
        if (ee->numOfRefs() > 1)
            return false;
    }
    return true;
}

static bool is_text_join_possible(const Node* node1, const Node* node2)
{
    return (0 != node1 && 0 != node2 && node1 != node2 &&
            (node1->nextSibling() == node2) &&
            Node::TEXT_NODE == node1->nodeType() &&
            Node::TEXT_NODE == node2->nodeType());
}

bool GroveCommand::isTextJoinPossible(const Node* node1, const Node* node2,
                                      bool dontCheckAdjacency)
{
    return (0 != node1 && 0 != node2 && node1 != node2 &&
            (dontCheckAdjacency || node1->nextSibling() == node2) &&
            Node::TEXT_NODE == node1->nodeType() &&
            Node::TEXT_NODE == node2->nodeType());
}

static bool is_id_attr(const PropertyNode* ai)
{
    return ai->getSafeProperty("type")->getString() == NOTR("ID");
}

void CommandContext::setEntityModified(const GrovePos& gp)
{
    setEntityModified(gp.contextNode());
}

void CommandContext::unsetEntityModified(const GrovePos& gp)
{
    unsetEntityModified(gp.contextNode());
}

static EntityDecl* context_decl(Node* n)
{
    EntityReferenceStart* ers = 0;
    if (n->nodeType() == Node::ENTITY_REF_START_NODE) {
        ers = static_cast<EntityReferenceStart*>(n);
        ers = static_cast<EntityReferenceStart*>(ers->getSectParent());
    } 
    else
        ers = get_ers(n);
    if (ers)
        return ers->entityDecl();
    return 0;
}

void CommandContext::setEntityModified(Node* n)
{
    EntityDecl* decl = context_decl(n);
    if (0 == decl)
        return;
    if (decl->isContentModified())
        asCommand()->udata1() |= CD_IS_ENTITY_MODIFIED;
    else
        asCommand()->udata1() &= CD_IS_ENTITY_MODIFIED;
    decl->setContentModified(true);
}

void CommandContext::unsetEntityModified(Node* n)
{
    EntityDecl* decl = context_decl(n);
    if (0 == decl)
        return;
    decl->setContentModified(asCommand()->udata1() & CD_IS_ENTITY_MODIFIED);
}
    
void CommandContext::setTextModified(GroveLib::Text* t)
{
    if (is_text_choice(t))
        asCommand()->udata1() |= CD_IS_TEXT_CHOICE;
    else
        asCommand()->udata1() &= ~CD_IS_TEXT_CHOICE;
    set_text_choice(t, false);
}

void CommandContext::unsetTextModified(GroveLib::Text* t)
{
    set_text_choice(t, asCommand()->udata1() & CD_IS_TEXT_CHOICE);
}

////////////////////////////////////////////////////////////////////

const GrovePos& cmd_pos(const Command* cmd)
{
    RT_ASSERT(cmd);
    return dynamic_cast<const CommandContext*>(cmd)->pos();
}

const GrovePos& GroveMacroCommand::pos() const
{
    if (isExecuted()) {
        Command* cmd = lastChild();
        while (cmd_pos(cmd).isNull() && cmd != firstChild())
            cmd = cmd->prevSibling();
        return cmd_pos(cmd);
    } 
    else {
        Command* cmd = firstChild();
        while (cmd_pos(cmd).isNull() && cmd != lastChild())
            cmd = cmd->nextSibling();
        return cmd_pos(cmd);
    }
}

GroveLib::Node* GroveMacroCommand::validationContext() const
{
    return dynamic_cast<const CommandContext*>
        (lastChild())->validationContext();
}

const GrovePos& GroveBatchCommand::pos() const
{
    if (isExecuted()) {
        if (!suggestedPos_.isNull())
            return suggestedPos_;
    }
    else {
        if (!undoPos_.isNull())
            return undoPos_;
    }
    return GroveMacroCommand::pos();
}

String GroveBatchCommand::info(uint idx) const
{
    if (info_.isNull()) 
        return GroveMacroCommand::info(idx);
    return idx ? String::null() : info_;
}

////////////////////////////////////////////////////////////////////

const GrovePos& GroveReplicatedCommand::pos() const
{
    for (Command* cmd = firstChild(); cmd; cmd = cmd->nextSibling()) {
        const GrovePos& gp = cmd_pos(cmd);
        if (!gp.isNull())
            return gp;
    }
    return cmd_pos(firstChild());
}

CommandContext::CommandFlags GroveReplicatedCommand::flags() const
{
    return dynamic_cast<const CommandContext*>(firstChild())->flags();
}

GroveLib::Node* GroveReplicatedCommand::validationContext() const
{
    for (Command* cmd = firstChild(); cmd; cmd = cmd->nextSibling()) {
        GroveLib::Node* vc = dynamic_cast<const CommandContext*>
            (cmd)->validationContext();
        if (vc)
            return vc;
    }
    return 0;
}

bool GroveReplicatedCommand::mergeWith(Common::Command* cmd)
{
    GroveReplicatedCommand* rc = cmd->asGroveReplicatedCommand();
    if (!rc)
        return false;
    Command* c1 = firstChild();
    Command* c2 = cmd->firstChild();
    for (; c1 && c2; c1 = c1->nextSibling(), c2 = c2->nextSibling())
        if (!c1->mergeWith(c2))
            return false;
    return true;
}

///////////////////////////////////////////////////////////////////////

void GroveBatchCommand::executeAndAdd(Command* cmd, bool refuseNull)
{
    if (0 == cmd && !refuseNull)
        return;
    try {
        cmd->execute();
    } catch (Command::Exception& e) {
        if (refuseNull) {
            doUnexecute();
            setValid(false);
            throw;
        }
        return;
    }
    appendChild(cmd);
}

///////////////////////////////////////////////////////////////////////

InsertText::InsertText(const GrovePos& pos, const String& text)
    : pos_(pos),
      text_(text)
{
}

String InsertText::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Insert Text: '%1'");
        case 1:  return text_;
        default: return String::null();
    }
}

void InsertText::doExecute()
{
    String str(pos_.text()->data());
    str.insert(pos_.idx(), text_);
    pos_.text()->setData(str);
    suggestedPos_ = GrovePos(pos_.text(), pos_.idx() + text_.length());
    setEntityModified(pos_);
    setTextModified(pos_.text());
}

void InsertText::doUnexecute()
{
    String str(pos_.text()->data());
    str.remove(pos_.idx(), text_.length());
    pos_.text()->setData(str);
    suggestedPos_ = pos_;
    unsetEntityModified(pos_);
    unsetTextModified(pos_.text());
}

bool InsertText::mergeWith(Command* cmd)
{
    InsertText* other = dynamic_cast<InsertText*>(cmd);
    if (!other)
        return false;
    text_.append(other->text_);
    return true;
}

GroveLib::Node* InsertText::validationContext() const
{
    return pos_.text()->parent();
}

///////////////////////////////////////////////////////////////////////

InsertTextNode::InsertTextNode(const GrovePos& pos,
                               const String& text)
    : pos_(pos),
      text_(new Text(text)),
      str_(text)
{
}

String InsertTextNode::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Insert Text: '%1'");
        case 1:  return str_;
        default: return String::null();
    }
}

void InsertTextNode::doExecute()
{
    setEntityModified(pos_);
    pos_.insert(text_.pointer());
    suggestedPos_ = GrovePos(text_.pointer(), text_->data().length());
}

void InsertTextNode::doUnexecute()
{
    suggestedPos_ = pos_;
    text_->remove();
    unsetEntityModified(pos_);
}

bool InsertTextNode::mergeWith(Command* cmd)
{
    InsertText* other = dynamic_cast<InsertText*>(cmd);
    if (!other || other->pos().node() != text_ || !other->isExecuted())
        return false;
    // nothing to do - text is already inserted by InsertText command.
    str_ = text_->data();
    return true;    
}

GroveLib::Node* InsertTextNode::validationContext() const
{
    return pos_.node();
}


///////////////////////////////////////////////////////////////////////

RemoveText::RemoveText(const GrovePos& pos, uint length, GrovePos* adjustPos)
    : pos_(pos)
{
    uint total_len = pos.text()->data().length();
    if ((pos.idx() + length) > total_len)
        length = total_len - pos.idx();
    removed_ = pos_.text()->data().mid(pos_.idx(), length);
    if (adjustPos && adjustPos->node() == pos.node() && 
        uint(adjustPos->idx()) >= (pos.idx() + length)) 
        *adjustPos = GrovePos(adjustPos->node(), adjustPos->idx() - length);
}

String RemoveText::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Remove Text: '%1'");
        case 1:  return removed_;
        default: return String::null();
    }
}

void RemoveText::doExecute()
{
    if (0 == removed_.length()) 
        throw GroveEditor::Exception(GroveEditorMessages::removeText);
    String str(pos_.text()->data());
    str.remove(pos_.idx(), removed_.length());
    pos_.text()->setData(str);
    suggestedPos_ = pos_;
    setEntityModified(pos_);
    setTextModified(pos_.text());
}

void RemoveText::doUnexecute()
{
    String str(pos_.text()->data());
    str.insert(pos_.idx(), removed_);
    pos_.text()->setData(str);
    suggestedPos_ = GrovePos(pos_.text(), pos_.idx() + removed_.length());
    unsetEntityModified(pos_);
    unsetTextModified(pos_.text());
}

GroveLib::Node* RemoveText::validationContext() const
{
    return parentNode(pos_.node());
}

//////////////////////////////////////////////////////////

ReplaceText::ReplaceText(const GrovePos& pos, uint length,
                         const COMMON_NS::String& newText)
    : pos_(pos), newText_(newText)
{
    RT_ASSERT(pos.type() == GrovePos::TEXT_POS);
    uint total_len = pos.text()->data().length();
    if ((pos.idx() + length) > total_len)
        length = total_len - pos.idx();
    removedText_ = pos_.text()->data().mid(pos_.idx(), length);
}

void ReplaceText::replace(const String& oldText, const String& newText)
{
    String str(pos_.text()->data());
    str.replace(pos_.idx(), oldText.length(), newText);
    pos_.text()->setData(str);
    suggestedPos_ = GrovePos(pos_.text(), pos_.idx() + newText.length());
}

void ReplaceText::doExecute()
{
    replace(removedText_, newText_);
    setEntityModified(pos_);
    setTextModified(pos_.text());
}

void ReplaceText::doUnexecute()
{
    replace(newText_, removedText_);
    suggestedPos_ = pos_;
    unsetEntityModified(pos_);
    unsetTextModified(pos_.text());
}

GroveLib::Node* ReplaceText::validationContext() const
{
    return parentNode(pos_.node());
}

String ReplaceText::info(uint idx) const
{
    switch (idx) {
        case 0:  
            if (newText_.length())
                return QT_TRANSLATE_NOOP("GroveEditor",
                    "Replace: '%1' -> '%2'");
            else
                return QT_TRANSLATE_NOOP("GroveEditor", "Remove Text: '%1'");

        case 1:  return removedText_;
        case 2:  return newText_;
        default: return String::null();
    }
}

///////////////////////////////////////////////////////////////////////

static void make_attribute(Element* elem, Grove*,
                           const String& name, const String& value,
                           bool isIdAttr)
{
    if (name.left(6) == NOTR("xmlns:") || name == NOTR("xmlns")) {
        String prefix;
        GroveLib::XmlNs::parseXmlNsAtt(name, prefix);
        elem->addToPrefixMap(prefix, value);
        return;
    }
    Attr* attr = new Attr(name);
    if (isIdAttr)
        attr->setIdClass(Attr::IS_ID);
    attr->setValue(value);
    elem->attrs().appendChild(attr);
}

GROVEEDITOR_EXPIMP void make_attributes(Element* elem, Grove* grove,
                                        const PropertyNode* attrs)
{
    if (0 == attrs)
        return;
    const PropertyNode* attr = attrs->firstChild();
    for (; attr; attr = attr->nextSibling())
        make_attribute(elem, grove, attr->name(),
                       attr->getString(), is_id_attr(attr));
}

//////////////////////////////////////////////////////////////////////////

InsertElement::InsertElement(const GrovePos& elementPos, const String& name,
                             const PropertyNode* attrMap)
    : pos_(elementPos),
      elem_(new Element(name))
{
    make_attributes(elem_.pointer(),
                    (pos_.node()) ? pos_.node()->grove() : 0,
                    attrMap);
}

String InsertElement::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Insert Element: <%1>");
        case 1:  return elem_->nodeName();
        default: return String::null();
    }
}

void InsertElement::doExecute()
{
    setEntityModified(pos_);
    pos_.insert(elem_.pointer());
    suggestedPos_ = GrovePos(elem_.pointer());
}

void InsertElement::doUnexecute()
{
    suggestedPos_ = GrovePos(elem_->parent(), elem_->nextSibling());
    unsetEntityModified(suggestedPos_);
    elem_->remove();
}

GroveLib::Node* InsertElement::validationContext() const
{
    return pos_.node();
}

///////////////////////////////////////////////////////////////////////

RenameElement::RenameElement(const GrovePos& pos,
                             const String& name)
    : pos_(pos), name_(name)
{
    Node* n = pos.node();
    while (n && n->nodeType() != Node::ELEMENT_NODE)
        n = parentNode(n);
    elem_ = ELEMENT_CAST(n);
    oldName_ = elem_->name();
    parent_  = parentNode(elem_.pointer());
    suggestedPos_ = pos_;
    newElem_ = static_cast<Element*>(elem_->cloneNode(0, elem_->parent()));
    GroveLib::Attr* attr = newElem_->attrs().firstChild();
    GroveLib::Attr* nextAttr = 0;
    while (attr) {
        nextAttr = attr->nextSibling();
        if (attr->defaulted() == Attr::DEFAULTED)
            attr->remove();
        attr = nextAttr;
    }
}

String RenameElement::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Rename Element: <%1> -> <%2>");
        case 1:  return oldName_;
        case 2:  return name_;
        default: return String::null();
    }
}

void RenameElement::doExecute()
{
    GrovePos new_pos(elem_->parent(), elem_->nextSibling());
    DocumentFragmentPtr fragment;
    if (elem_->firstChild())
        fragment = elem_->firstChild()->takeAsFragment(elem_->lastChild());
    newElem_->setName(name_);
    elem_->remove();
    elem_->setGSR(0);
    new_pos.insert(newElem_.pointer());
    if (!fragment.isNull())
        newElem_->appendChild(fragment.pointer());
    setEntityModified(newElem_.pointer());
    if (pos_.type() == GrovePos::TEXT_POS)
        suggestedPos_ = pos_;
    else
        suggestedPos_ = GrovePos(newElem_.pointer(), pos_.before());
}

void RenameElement::doUnexecute()
{
    GrovePos new_pos(newElem_->parent(), newElem_->nextSibling());
    DocumentFragmentPtr fragment;
    if (newElem_->firstChild())
        fragment = newElem_->firstChild()->takeAsFragment(
            newElem_->lastChild());
    newElem_->remove();
    newElem_->setGSR(0);
    new_pos.insert(elem_.pointer());
    if (!fragment.isNull())
        elem_->appendChild(fragment.pointer());
    suggestedPos_ = pos_;
    unsetEntityModified(elem_.pointer());
}

GroveLib::Node* RenameElement::validationContext() const
{
    return parent_.pointer();
}

///////////////////////////////////////////////////////////////////////

RemoveNode::RemoveNode(const NodePtr& node)
    : node_(node),
      parent_(node_->parent()),
      before_(node_->nextSibling())
{
    suggestedPos_ = GrovePos(parent_.pointer(), node_.pointer());

    if (isTextJoinPossible(node_->prevSibling(), node_->nextSibling(), true))
        joinText_ = new Join(node_->prevSibling(), node_->nextSibling());
}

String RemoveNode::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Remove Node: <%1>");
        case 1:  return node_->nodeName();
        default: return String::null();
    }
}

void RemoveNode::doExecute()
{
    setEntityModified(node_.pointer());
    Node* ns = node_->nextSibling();
    const Node* context_node = node_->prevSibling();
    if (0 == context_node)
        context_node = parentNode(node_.pointer());
    //! Keep pointer to fragment to take references to entities
    frag_ = node_->takeAsFragment();
    if (!joinText_.isNull()) {
        joinText_->execute();
        suggestedPos_ = joinText_->pos();
    }
    else
        suggestedPos_ = GrovePos(parent_.pointer(), ns);
    if (!check_xinclude_removal(frag_.pointer(), context_node)) {
        doUnexecute();
        throw GroveEditor::Exception
            (GroveEditorMessages::cantRemoveXincludeSect);
    }
}

void RemoveNode::doUnexecute()
{
    if (!joinText_.isNull())
        joinText_->unexecute();
    if (!before_.isNull() && before_->parent()) {
        before_->insertBefore(frag_.pointer());
        unsetEntityModified(before_.pointer());
    }
    else {
        parent_->appendChild(frag_.pointer());
        unsetEntityModified(parent_.pointer());
    }
}

const GrovePos& RemoveNode::pos() const
{
    if (isExecuted() && !joinText_.isNull())
        return joinText_->pos();
    return GroveCommand::pos();
}

GroveLib::Node* RemoveNode::validationContext() const
{
    return parent_.pointer();
}

///////////////////////////////////////////////////////////////////////

Untag::Untag(const GrovePos& pos)
{
    origPos_ = suggestedPos_ = pos;
    if (pos.type() == GrovePos::TEXT_POS)
        elem_ = pos.node()->parent();
    else
        elem_ = pos.node();
    untagTo_ = GrovePos(elem_->parent(), elem_->nextSibling());
}

void Untag::doExecute()
{
    FragmentPtr fragment;
    setEntityModified(elem_.pointer());
    from_ = elem_->firstChild();
    to_   = elem_->lastChild();
    if (!from_.isNull())
        fragment = from_->takeAsFragment(to_.pointer());
    Node* joinLeft = elem_->prevSibling();
    elem_->remove();
    if (!fragment.isNull()) 
        untagTo_.insert(fragment.pointer());
    suggestedPos_ = untagTo_;
    if (!from_.isNull()) {
        if (isTextJoinPossible(joinLeft, from_.pointer(), false)) {
            joinLeft_ = new Join(joinLeft, from_.pointer());
            joinLeft_->execute();
            suggestedPos_ = joinLeft_->pos();
        }
        if (joinLeft == to_->prevSibling()) {
            if (isTextJoinPossible(joinLeft, untagTo_.before(), false)) {
                joinRight_ = new Join(joinLeft, untagTo_.before());
                joinRight_->execute();
                suggestedPos_ = joinRight_->pos();
            }
        } 
        else {
            if (isTextJoinPossible(to_.pointer(), untagTo_.before(), false)) {
                joinRight_ = new Join(to_.pointer(), untagTo_.before());
                joinRight_->execute();
                suggestedPos_ = joinRight_->pos();
            }
        }
    }
}

void Untag::doUnexecute()
{
    FragmentPtr fragment;
    if (!joinRight_.isNull())
        joinRight_->unexecute();
    if (!joinLeft_.isNull())
        joinLeft_->unexecute();
    if (!from_.isNull())
        fragment = from_->takeAsFragment(to_.pointer());
    untagTo_.insert(elem_.pointer());
    if (!fragment.isNull())
        elem_->appendChild(fragment.pointer());
    unsetEntityModified(elem_.pointer());
    suggestedPos_ = origPos_;
}

String Untag::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Untag Element: <%1>");
        case 1:  return elem_->nodeName();
        default: return String::null();
    }
}

GroveLib::Node* Untag::validationContext() const
{
    return untagTo_.node();
}

///////////////////////////////////////////////////////////////////////

RemoveGroup::RemoveGroup(const NodePtr& from, const NodePtr& to, 
                         GrovePos* adjustPos)
    : from_(from),
      to_(to),
      parent_(from_->parent()),
      before_(to_->nextSibling())
{
    suggestedPos_ = GrovePos(to_->parent(), to_.pointer()->nextSibling());

    if (isTextJoinPossible(from_->prevSibling(), to_->nextSibling(), true)) {
        if (adjustPos && to_->nextSibling() == adjustPos->node())
            *adjustPos = GrovePos(from_->prevSibling(), 
                                  adjustPos->idx() + static_cast<Text*>(
                                      from_->prevSibling())->data().length());
        joinText_ = new Join(from_->prevSibling(), to_->nextSibling());
    }
}

String RemoveGroup::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Remove Node Group");
        default: return String::null();
    }
}

void RemoveGroup::doExecute()
{
    if (from_->parent() != to_->parent())
        throw GroveEditor::Exception(GroveEditorMessages::removeGroup);
    const Node* context_node = from_->prevSibling();
    if (0 == context_node)
        context_node = parentNode(from_.pointer());
    setEntityModified(from_.pointer());

    //! Keep pointer to fragment to take references to entities
    frag_ = from_->takeAsFragment(to_.pointer());
    if (!joinText_.isNull()) {
        joinText_->execute();
        suggestedPos_ = joinText_->pos();
    }
    if (!check_xinclude_removal(frag_.pointer(), context_node)) {
        doUnexecute();
        throw GroveEditor::Exception
            (GroveEditorMessages::cantRemoveXincludeSect);
    }
}

void RemoveGroup::doUnexecute()
{
    if (!joinText_.isNull())
        joinText_->unexecute();
    if (frag_.isNull())
        return;
    if (!before_.isNull()) {
        before_->insertBefore(frag_.pointer());
        unsetEntityModified(before_.pointer());
    }
    else {
        parent_->appendChild(frag_.pointer());
        unsetEntityModified(parent_.pointer());
    }
}

const GrovePos& RemoveGroup::pos() const
{
    if (isExecuted() && !joinText_.isNull())
        return joinText_->pos();
    return GroveCommand::pos();
}

GroveLib::Node* RemoveGroup::validationContext() const
{
    return parent_.pointer();
}

/////////////////////////////////////////////////////////////////////

static void dump_plist(const char* s, const GrovePosList& pl)
{
    DBG(GROVEEDITOR.POS) << s << std::endl;
    for (uint i = 0; i < pl.size(); ++i)
        pl[i].dump();
    DBG(GROVEEDITOR.POS) << "-------------\n";
}

static void make_extra_pos(GrovePosList& gpl, Node* cp, bool right)
{
    GrovePos& back = gpl.back();
    if (back.node() != cp && back.type() != GrovePos::TEXT_POS)
        gpl.push_back(GrovePos(back.node()->parent(),
            right ? back.node() : back.node()->nextSibling()));
}

SelectionCommand::SelectionCommand(const GrovePos& first,
                                   const GrovePos& last)
    : first_(first.adjustBoundaryPos()),
      last_(last.adjustBoundaryPos())
{
    DBG(GROVEEDITOR.POS) << "** SELECTION POSITIONS: \n";
    DBG_EXEC(GROVEEDITOR.POS, first_.dump());
    DBG_EXEC(GROVEEDITOR.POS, last_.dump());
    DBG(GROVEEDITOR.POS) << "** --- (SELECTION)\n";

    Node* n  = first_.node();
    cp_ = first_.node()->commonAos(last_.node());

    DBG(GROVEEDITOR.POS) << "Common parent: " << cp_->nodeName() << std::endl;

    left_.push_back(first_);
    right_.push_back(last_);

    for (; n != cp_ && n->parent() != cp_; n = n->parent())
        left_.push_back(GrovePos(n->parent(), n->nextSibling()));
    for (n = last_.node(); n != cp_ && n->parent() != cp_; n = n->parent())
        right_.push_back(GrovePos(n->parent(), n));

    if (left_.back().node() != right_.back().node()) {
        make_extra_pos(left_,  cp_, false);
        make_extra_pos(right_, cp_, true);
    }
    DBG_EXEC(GROVEEDITOR.POS, dump_plist("Left:", left_));
    DBG_EXEC(GROVEEDITOR.POS, dump_plist("Right:", right_));
}

GroveCommand* SelectionCommand::add(GroveCommand* c)
{
    executeAndAdd(c);
    return c;
}

const GrovePos& SelectionCommand::pos() const
{
    return GroveMacroCommand::pos();
}

//////////////////////////////////////////////////////////////////

SplitSelectionCommand::SplitSelectionCommand(const GrovePos& first,
                                             const GrovePos& last)
    : SelectionCommand(first, last)
{
}

void SplitSelectionCommand::buildSubCommands()
{
    uint leftIdx = 0, rightIdx = 0;
    bool hadLeftSplit = false, hadRightSplit = false;
    SplitText* cp1, *cp2;
    if (first_.node() == last_.node() &&
        first_.type() == GrovePos::TEXT_POS &&
        last_.type()  == GrovePos::TEXT_POS) {
            add(cp1 = new SplitText(first_));
            add(cp2 = new SplitText(
                    GrovePos(static_cast<Text*>(cp1->elementPos().before()),
                             last_.idx() - first_.idx())));
            left_[leftIdx++]   = cp1->elementPos();
            right_[rightIdx++] = cp2->elementPos();
    } 
    else {
        if (first_.type() == GrovePos::TEXT_POS) {
            add(cp1 = new SplitText(first_));
            left_[leftIdx++] = cp1->elementPos();
        }
        if (last_.type() == GrovePos::TEXT_POS) {
            add(cp1 = new SplitText(last_));
            right_[rightIdx++] = cp1->elementPos();
        }
    }
    for (; leftIdx < (left_.size() - 1); ++leftIdx) {
        GrovePos& p = left_[leftIdx];
        if (leftIdx) {
            if (p.before())
                p = GrovePos(p.node(), p.before()->prevSibling());
            else
                p = GrovePos(p.node(), p.node()->lastChild());
        }
        DBG(GROVEEDITOR.POS) << "Left split: ";
        DBG_EXEC(GROVEEDITOR.POS, p.dump());
        hadLeftSplit = true;
        add(new SplitElement(p));
    }
    for (; rightIdx < (right_.size() - 1); ++rightIdx) {
        GrovePos& p = right_[rightIdx];
        if (rightIdx) {
            if (p.before())
                p = GrovePos(p.node(), p.before()->nextSibling());
            else
                p = GrovePos(p.node(), p.node()->firstChild());
        }
        DBG(GROVEEDITOR.POS) << "Right split: ";
        DBG_EXEC(GROVEEDITOR.POS, p.dump());
        hadRightSplit = true;
        add(new SplitElement(p));
    }
    DBG(GROVEEDITOR.POS) << "Result position lists:\n";
    DBG_EXEC(GROVEEDITOR.POS, dump_plist("Left-res: ", left_));
    DBG_EXEC(GROVEEDITOR.POS, dump_plist("Right-res: ", right_));
    Node* ln = left_.back().before();
    Node* rn = right_.back().before();
    if (0 == ln && 0 == rn) {
        ln = left_.back().node()->lastChild();
        makeSubcommand(ln, ln);
        return;
    }
    if (0 == ln)
        ln = left_.back().node()->lastChild();
    if (hadLeftSplit)
        ln = ln->prevSibling();
    if (ln == rn || !ln)
        throw GroveEditor::Exception(GroveEditorMessages::splitEmpty);
    if (rn)
        rn = hadRightSplit ? rn : rn->prevSibling();
    else
        rn = right_.back().node()->lastChild();
    if (ln->parent() != rn->parent())
        throw GroveEditor::Exception(GroveEditorMessages::splitBadParents);
    makeSubcommand(ln, rn);
}

void SplitSelectionCommand::makeSubcommand(GroveLib::Node* first,
                                           GroveLib::Node* last)
{
    DBG_IF(GROVEEDITOR.POS) {
        DBG(GROVEEDITOR.POS) << "Selection: makeSubcommand: ***************\n";
        const Node* n = first;
        do {
            DBG_IF(GROVEEDITOR.POS) Node::dumpSubtree(n);
            if (n == last)
                break;
            n = n->nextSibling();
        } while (n);
        DBG(GROVEEDITOR.POS) << "makeSubcommand: END ****************\n";
    }
}

//////////////////////////////////////////////////////////////////

RemoveSelection::RemoveSelection(const GrovePos& first, const GrovePos& last, 
                                 GrovePos* adjustPos)
    : SelectionCommand(first, last),
      adjustPos_(adjustPos)
{
}

void RemoveSelection::buildSubCommands()
{
    if (first_.node() == last_.node() &&
        first_.type() == GrovePos::TEXT_POS &&
        last_.type()  == GrovePos::TEXT_POS) {
            add(new RemoveText(GrovePos(first_.text(), first_.idx()),
                               last_.idx() - first_.idx(), adjustPos_));
            return;
    }
    uint leftIdx = 0, rightIdx = 0;
    if (first_.type() == GrovePos::TEXT_POS) {
        add(new RemoveText(GrovePos(first_.text(), first_.idx()),
                           first_.text()->data().length() - first_.idx(), 
                           adjustPos_));
        ++leftIdx;
    }
    if (last_.type() == GrovePos::TEXT_POS) {
        add(new RemoveText(GrovePos(last_.text(), 0), 
                           last_.idx(), adjustPos_));
        ++rightIdx;
    }
    Node* n = 0;
    for (; leftIdx < (left_.size() - 1); ++leftIdx) {
        n = left_[leftIdx].before();
        if (n)
            add(new RemoveGroup(n, n->parent()->lastChild(), adjustPos_));
    }
    for (; rightIdx < (right_.size()- 1); ++rightIdx) {
        n = right_[rightIdx].before();
        Node* pn = right_[rightIdx].node();
        if (n) {
            if (n->prevSibling())
                add(new RemoveGroup(
                        n->parent()->firstChild(), 
                        n->prevSibling(), adjustPos_));
        } 
        else 
            if (pn->firstChild())
                add(new RemoveGroup(pn->firstChild(), pn->lastChild(), 
                                    adjustPos_));
    }
    if (left_.back().type() == GrovePos::TEXT_POS)
        n = left_.back().node()->nextSibling();
    else
        n = left_.back().before();
    if (0 == n)
        return;
    Node* lastn = 0;
    const GrovePos& bp = right_.back();
    if (bp.type() == GrovePos::ELEMENT_POS) {
        if (bp.before()) {
            if (n == bp.before())
                return;
            lastn = bp.before()->prevSibling();
        } 
        else
            lastn = bp.node()->lastChild();
    } 
    else {
        if (n == bp.node())
            return;
        lastn = bp.node()->prevSibling();
    }
    if (0 == lastn)
        return;
    DBG(GROVEEDITOR.POS) << "LAST-PAIR:\n";
    DBG_EXEC(GROVEEDITOR.POS, n->dump());
    DBG_EXEC(GROVEEDITOR.POS, lastn->dump());
    DBG(GROVEEDITOR.POS) << "****************************\n";
    if (n->parent() != lastn->parent())
        throw GroveEditor::Exception(GroveEditorMessages::removeSelection);
    if (!n->parent() || !n->parent()->parent())
        throw GroveEditor::Exception
            (GroveEditorMessages::cantRemoveRootElement);
    add(new RemoveGroup(n, lastn, adjustPos_));
}

String RemoveSelection::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Remove Selection");
        default: return String::null();
    }
}

GroveLib::Node* RemoveSelection::validationContext() const
{
    return cp_;
}

///////////////////////////////////////////////////////////////////////

TagRegion::TagRegion(const GrovePos& first,
                     const GrovePos& last,
                     const COMMON_NS::String& elemName,
                     const PropertyNode* attrMap)
    : SplitSelectionCommand(first, last),
      elemName_(elemName), attrMap_(attrMap)
{
}

const GrovePos& TagRegion::pos() const
{
    if (posCmd_)
        return posCmd_->pos();
    else
        return GroveMacroCommand::pos();
}

void TagRegion::makeSubcommand(GroveLib::Node* first, GroveLib::Node* last)
{
    DBG_EXEC(GROVEEDITOR.POS,
        SplitSelectionCommand::makeSubcommand(first, last));
    posCmd_ = add(new ConvertToTag(first, last, elemName_, first_, attrMap_));
}

String TagRegion::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Wrap to Element: <%1>");
        case 1:  return elemName_;
        default: return String::null();
    }
}

GroveLib::Node* TagRegion::validationContext() const
{
    return cp_;
}

///
ConvertToTag::ConvertToTag(const GroveLib::NodePtr& first,
                           const GroveLib::NodePtr& last,
                           const COMMON_NS::String& elemName,
                           const GrovePos& origPos,
                           const PropertyNode* attrMap)
    : first_(first), last_(last), origPos_(origPos)
{
    tag_ = new GroveLib::Element(elemName);
    make_attributes(tag_.pointer(),
        (!first.isNull()) ? first->grove() : 0, attrMap);
}

void ConvertToTag::doExecute()
{
    setEntityModified(origPos_);
    GrovePos new_pos(last_->parent(), last_->nextSibling());
    DocumentFragmentPtr fragment = first_->takeAsFragment(last_.pointer());
    new_pos.insert(tag_.pointer());
    tag_->appendChild(fragment.pointer());
    suggestedPos_ = GrovePos(tag_->parent(), tag_.pointer());
}

void ConvertToTag::doUnexecute()
{
    GrovePos new_pos(tag_->parent(), tag_->nextSibling());
    DocumentFragmentPtr fragment =
        tag_->firstChild()->takeAsFragment(tag_->lastChild());
    tag_->remove();
    new_pos.insert(fragment.pointer());
    suggestedPos_ = origPos_;
    unsetEntityModified(origPos_);
}

String ConvertToTag::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Wrap to Element: <%1>");
        case 1:  return tag_->nodeName();
        default: return String::null();
    }
}

//////////////////////////////////////////////////////////////////

SetAttribute::SetAttribute(Attr* attr, const String& newValue)
      : attr_(attr),
        newValue_(newValue),
        oldValue_(attr->value())
{
    suggestedPos_ = GrovePos();
}

void SetAttribute::doExecute()
{
    setEntityModified(parentNode(attr_.pointer()));
    attr_->setValue(newValue_);
}

void SetAttribute::doUnexecute()
{
    attr_->setValue(oldValue_);
    unsetEntityModified(parentNode(attr_.pointer()));
}

String SetAttribute::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Set Attribute: %1='%2'");
        case 1:  return attr_->name();
        case 2:  return newValue_;
        default: return String::null();
    }
}

GroveLib::Node* SetAttribute::validationContext() const
{
    return attr_->element();
}

/////////////////////////////////////////////////////////////////


AddAttribute::AddAttribute(Element* element,
                           const PropertyNode* attrSpec)
    : element_(element)
{
    attr_ = new Attr(attrSpec->name());
    attr_->setValue(attrSpec->getString());
    if (attrSpec->getSafeProperty("type")->getString() == "ID")
        attr_->setIdClass(Attr::IS_ID);
    suggestedPos_ = GrovePos();
}

void AddAttribute::doExecute()
{
    if (element_->attrs().getAttribute(attr_->name()))
        throw GroveEditor::Exception(GroveEditorMessages::addAttribute);
    setEntityModified(element_.pointer());
    element_->attrs().appendChild(attr_.pointer());
}

void AddAttribute::doUnexecute()
{
    attr_->remove();
    unsetEntityModified(element_.pointer());
}

String AddAttribute::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Add Attribute: %1='%2'");
        case 1:  return attr_->name();
        case 2:  return attr_->value();
        default: return String::null();
    }
}

GroveLib::Node* AddAttribute::validationContext() const
{
    return element_.pointer();
}

///////////////////////////////////////////////////////////////////////

RemoveAttribute::RemoveAttribute(Attr* attr)
    : element_(attr->element()),
      attr_(attr)
{
    suggestedPos_ = GrovePos();
}

String RemoveAttribute::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Remove Attribute: '%1'");
        case 1:  return attr_->name();
        default: return String::null();
    }
}

void RemoveAttribute::doExecute()
{
    setEntityModified(element_.pointer());
    attr_->remove();
}

void RemoveAttribute::doUnexecute()
{
    element_->attrs().appendChild(attr_.pointer());
    unsetEntityModified(element_.pointer());
}

GroveLib::Node* RemoveAttribute::validationContext() const
{
    return element_.pointer();
}

/////////////////////////////////////////////////////////////////////

RenameAttribute::RenameAttribute(Attr* attr,
                                 const PropertyNode* attrSpec)
{
    removeAttr_ = new RemoveAttribute(attr);
    addAttr_    = new AddAttribute(attr->element(), attrSpec);
    suggestedPos_ = GrovePos();
}

void RenameAttribute::doExecute()
{
    removeAttr_->execute();
    addAttr_->execute();
}


void RenameAttribute::doUnexecute()
{
    addAttr_->unexecute();
    removeAttr_->unexecute();
}

GroveLib::Node* RenameAttribute::validationContext() const
{
    return addAttr_->validationContext();
}

String RenameAttribute::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Rename Attribute: '%1' -> '%2'");
        case 1:  return removeAttr_->info(1);
        case 2:  return addAttr_->info(1);
        default: return String::null();
    }
}

//////////////////////////////////////////////////////////////////

MapXmlNs::MapXmlNs(Element* element,
                   const String& prefix,
                   const String& uri)
    : element_(element),
      prefix_(prefix),
      newUri_(uri)
{
    oldUri_ = element->getXmlNsByPrefix(prefix);
    suggestedPos_ = GrovePos();
}

void MapXmlNs::doExecute()
{
    setEntityModified(element_.pointer());
    element_->eraseFromPrefixMap(prefix_);
    if (!newUri_.isNull())
        element_->addToPrefixMap(prefix_, newUri_);
}

void MapXmlNs::doUnexecute()
{
    unsetEntityModified(element_.pointer());
    element_->eraseFromPrefixMap(prefix_);
    if (!oldUri_.isNull())
        element_->addToPrefixMap(prefix_, oldUri_);
}

String MapXmlNs::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Map Namespace Prefix: %1='%2'");
        case 1:  return prefix_;
        case 2:  return newUri_;
        default: return String::null();
    }
}

GroveLib::Node* MapXmlNs::validationContext() const
{
    return element_.pointer();
}

//////////////////////////////////////////////////////////////////

SplitText::SplitText(const GrovePos& pos)
    : pos_(pos)
{
}

void SplitText::doExecute()
{
    if (GrovePos::TEXT_POS != pos_.type())
        throw GroveEditor::Exception(GroveEditorMessages::splitTextPos);
    Text* text_node = pos_.text();
    if (0 == pos_.idx() || text_node->data().length() <= (uint)pos_.idx())
        throw GroveEditor::Exception(GroveEditorMessages::splitTextCorner);
    setEntityModified(pos_);
    if (split_.isNull())
        split_ = new Text(text_node->data().mid(pos_.idx()));
    GrovePos(text_node->parent(), text_node->nextSibling()).
        insert(split_.pointer());
    text_node->setData(text_node->data().left(pos_.idx()));

    suggestedPos_ = GrovePos(static_cast<Text*>(split_.pointer()), 0);
}

void SplitText::doUnexecute()
{
    Text* first_text = pos_.text();
    suggestedPos_ = GrovePos(first_text, first_text->data().length());
    first_text->appendData(split_->data());
    split_->remove();
    unsetEntityModified(pos_);
}

GrovePos SplitText::elementPos() const
{
    return GrovePos(split_->parent(), split_.pointer());
}

String SplitText::info(uint idx) const
{
    return idx ? String::null() :
        QT_TRANSLATE_NOOP("GroveEditor", "Split Text");
}

GroveLib::Node* SplitText::validationContext() const
{
    return pos_.text()->parent();
}

/////////////////////////////////////////////////////////////////////

SplitElement::SplitElement(const GrovePos& pos)
    : pos_(pos)
{
}

void SplitElement::doExecute()
{
    if (GrovePos::ELEMENT_POS != pos_.type())
        throw GroveEditor::Exception(GroveEditorMessages::splitElemPos);
    if (pos_.node()->nodeType() != Node::ELEMENT_NODE)
        throw GroveEditor::Exception(GroveEditorMessages::splitElemNode);
    setEntityModified(pos_);
    GroveLib::Node* n = pos_.node();
    if (split_.isNull()) {
        split_ = new Element(n->nodeName());
        const Element* original = static_cast<const Element*>(n);
        Element* splitted = static_cast<Element*>(split_.pointer());
        Attr* attr = original->attrs().firstChild();
        for (; attr; attr = attr->nextSibling()) {
            if (!attr->specified())
                continue;
            splitted->attrs().appendChild
                (static_cast<Attr*>(attr->cloneNode(true, splitted)));
            set_unique_ids(splitted, pos_.contextNode());
        }
    }
    GrovePos(n->parent(), n->nextSibling()).insert(split_.pointer());
    Node* move_from = pos_.before();
    if (move_from)
        split_->appendChild(move_from->takeAsFragment(n->lastChild()));
    //! Move selected children to splitted node
    suggestedPos_ = GrovePos(split_.pointer(), split_->firstChild());
}

void SplitElement::doUnexecute()
{
    suggestedPos_ = pos_;
    split_->remove();
    if (split_->firstChild())
        pos_.node()->appendChild
            (split_->firstChild()->takeAsFragment(split_->lastChild()));
    unsetEntityModified(pos_);
}

String SplitElement::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Split Element: <%1>");
        case 1:  return pos_.node()->nodeName();
        default: return String::null();
    }
}

GroveLib::Node* SplitElement::validationContext() const
{
    return pos_.node()->parent();
}

//////////////////////////////////////////////////////////////////

Join::Join(const NodePtr& first, const NodePtr& second, bool placeCursorAtEnd)
    : first_(first),
      second_(second),
      placeCursorAtEnd_(placeCursorAtEnd)
{
    if (Node::TEXT_NODE == first_->nodeType())
        pos_ = TEXT_CAST(first_.pointer())->data().length();
    else
        pos_ = first_->countChildren();
}

String Join::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Join Two Elements: <%1>");
        case 1:  return first_->nodeName();
        default: return String::null();
    }
}

void Join::doExecute()
{
    if (first_.isNull() || second_.isNull())
        throw GroveEditor::Exception(GroveEditorMessages::joinEmpty);

    if (first_->nodeType() != second_->nodeType() ||
        (Node::TEXT_NODE != first_->nodeType() && 
         (first_->nodeName() != second_->nodeName())))
        throw GroveEditor::Exception(GroveEditorMessages::joinDifferent);

    if (first_->nextSibling() != second_)
        throw GroveEditor::Exception(GroveEditorMessages::joinNeighbours);

    setEntityModified(first_.pointer());
    if (Node::TEXT_NODE == first_->nodeType()) {
        Text* first_text = TEXT_CAST(first_.pointer());
        Text* second_text = TEXT_CAST(second_.pointer());

        int pos = first_text->data().length();
        if (placeCursorAtEnd_) 
            pos += second_text->data().length();
        suggestedPos_ = GrovePos(first_text, pos);

        first_text->appendData(second_text->data());
    }
    else {
        if (Node::ELEMENT_NODE != first_->nodeType())
        throw GroveEditor::Exception(GroveEditorMessages::joinElem);
        suggestedPos_ = GrovePos(first_.pointer());

        if (second_->firstChild()) {
            FragmentPtr frag = second_->firstChild()->takeAsFragment(
                second_->lastChild());
            first_->appendChild(frag.pointer());
        }
    }
    second_->remove();
}

void Join::doUnexecute()
{
    GrovePos(first_->parent(), 
             first_->nextSibling()).insert(second_.pointer());
    if (Node::TEXT_NODE == first_->nodeType()) {
        Text* text = TEXT_CAST(first_.pointer());
        text->setData(text->data().left(pos_));
    }
    else {
        if (Node::ELEMENT_NODE != first_->nodeType())
            throw GroveEditor::Exception(GroveEditorMessages::splitJoin);

        Node* move_from = first_->getChild(pos_);
        if (move_from) {
            FragmentPtr frag =
                move_from->takeAsFragment(first_->lastChild());
            second_->appendChild(frag.pointer());
        }
    }
    suggestedPos_ = GrovePos(first_->parent());
    unsetEntityModified(first_.pointer());
}

GroveLib::Node* Join::validationContext() const
{
    return first_->parent();
}

//////////////////////////////////////////////////////////////////////

Copy::Copy(const GrovePos& first,
           const GrovePos& last,
           const NodePtr& clipboard)
    : first_(first),
      last_(last),
      grove_(first_.node()->grove()),
      clipboard_(clipboard)
{
}

String Copy::info(uint) const
{
    return String::null();
}

void Copy::copyChildren(const NodePtr& parent, ulong from, ulong to,
                        const NodePtr& copyParent)
{
    Node* from_child = parent->getChild(from);
    DocumentFragment* frag = 0;
    if (from_child && from < to) {
        if (to - 1 > from) {
            Node* to_child = parent->getChild(to - 1);
            if (!to_child)
                to_child = parent->lastChild();
            frag = from_child->copyAsFragment(to_child);
            if (0 == frag)
                throw GroveEditor::Exception
                    (GroveEditorMessages::copyAsFragment);
            copyParent->appendChild(frag);
        }
        else {
            frag = from_child->copyAsFragment();
            if (0 == frag)
                throw GroveEditor::Exception
                    (GroveEditorMessages::copyAsFragment);
            copyParent->appendChild(from_child->copyAsFragment());
        }
    }
}

NodePtr Copy::copyAfter(const NodePtr& node, ulong from,
                        const NodePtr& commonParent, ulong commonTo)
{
    if (node.pointer() == commonParent.pointer()) {
        if (Node::TEXT_NODE == node->nodeType()) {
            Node* text = new Text(static_cast<const Text*>(node.pointer())->
                                  data().mid(from, commonTo - from));
            clipboard_->appendChild(text);
            return text;
        }
        else {
            copyChildren(node, from, commonTo, clipboard_);
            return clipboard_;
        }
    }
    else {
        ulong idx = node->siblingIndex();
        NodePtr copy_parent =
            copyAfter(node->parent(), idx + 1, commonParent, commonTo);
        if (Node::TEXT_NODE == node->nodeType()) {
            Node* text = new Text(static_cast<const Text*>(node.pointer())->
                                  data().mid(from));
            GrovePos(copy_parent.pointer(),
                copy_parent->firstChild()).insert(text);
            return text;
        }
        else {
            Node* copy_node = node->copy(copy_parent.pointer());
            GrovePos(copy_parent.pointer(),
                copy_parent->firstChild()).insert(copy_node);
            copyChildren(node, from, node->countChildren(), copy_node);
            return copy_node;
        }
    }
}

NodePtr Copy::copyBefore(const NodePtr& node, ulong to,
                         const NodePtr& commonParent,
                         const NodePtr& commonParentCopy)
{
    if (node.pointer() == commonParent.pointer())
        return commonParentCopy;
    else {
        ulong idx = node->siblingIndex();
        NodePtr copy_parent =
            copyBefore(node->parent(), idx, commonParent, commonParentCopy);

        if (Node::TEXT_NODE == node->nodeType()) {
            Node* text = new Text(
                static_cast<const Text*>(node.pointer())->data().left(to));
            copy_parent->appendChild(text);
            return text;
        }
        else {
            Node* copy_node = node->copy(copy_parent.pointer());
            copy_parent->appendChild(copy_node);
            copyChildren(node, 0, to, copy_node);
            return copy_node;
        }
    }
}

static Node* copy_commonParent(const TreelocRep& first_rep,
                               const TreelocRep& last_rep,
                               Node* root)
{
    Node* common_parent = root;
    const ulong min_len = (first_rep.length() < last_rep.length())
        ? first_rep.length()
        : last_rep.length();
    for (ulong i = 0; i < min_len; ++i) {
        ulong from_idx = first_rep[i];
        ulong to_idx = last_rep[i];
        if (from_idx == to_idx)
            common_parent = common_parent->getChild(from_idx);
        else
            break;
    }
    return common_parent;
}

static inline void clear_choices(Node* n)
{
    n = n->firstChild();
    while (n) {
        clear_choices(n);
        Node* next = n->nextSibling();
        if (n->nodeType() == Node::CHOICE_NODE)
            n->remove();
        n = next;
    }
}

void Copy::doExecute()
{
    //! Take care of cross-entity selections
    uint firstPos = first_.idx();
    uint lastPos  = last_.idx();

    //! Find common parent for corner nodes
    TreelocRep first_rep;
    first_.node()->treeLoc(first_rep);

    TreelocRep last_rep;
    last_.node()->treeLoc(last_rep);
    
    TreelocRep common_rep;
    NodePtr common_parent =
        copy_commonParent(first_rep, last_rep, first_.node()->root());
    if (common_parent.isNull())
        throw GroveEditor::Exception(GroveEditorMessages::copySelection);
    
    common_parent->treeLoc(common_rep);

    //! Clean clipboard
    clipboard_->removeAllChildren();
    //! Copy
    ulong common_to = lastPos;
    if (last_.node() != common_parent.pointer())
        common_to = last_rep[common_rep.length()];
    copyAfter(first_.node(), firstPos, common_parent, common_to);
    if (last_.node() != common_parent)
        copyBefore(last_.node(), lastPos, common_parent, clipboard_);
    // clean up Choices
    clear_choices(clipboard_.pointer());
    // Node::dumpSubtree(clipboard_.pointer());
}

//////////////////////////////////////////////////////////////////

class PasteFragment : public GroveCommand {
public:
    PasteFragment(const NodePtr& fragment, const GrovePos& elementPos)
        : pos_(elementPos),
          frag_(fragment) {
        //Node::dumpSubtree(fragment.pointer());
    }

protected:
    void doExecute() 
    {
        Node* node = 0;
        setEntityModified(pos_);
        suggestedPos_ = pos_;
        if (pos_.before()) {
            node = pos_.before()->prevSibling();
            pos_.before()->insertBefore(frag_.pointer());
        }
        else {
            node = pos_.node()->lastChild();
            pos_.node()->appendChild(frag_.pointer());
        }
        if (0 == node)
            frag_ = pos_.node()->firstChild();
        else
            frag_ = node->nextSibling();
    }
    
    void doUnexecute()
    {
        unsetEntityModified(pos_);
        if (pos_.before())
            frag_ = frag_->takeAsFragment(pos_.before()->prevSibling());
        else
            frag_ = frag_->takeAsFragment(pos_.node()->lastChild());
    }

private:
    GrovePos            pos_;
    GroveLib::NodePtr   frag_;
};

//////////////////////////////////////////////////////////////////

Paste::Paste(const NodePtr& clipboard, const GrovePos& elementPos)
    : pos_(elementPos),
      clipboard_(clipboard)
{
}

void Paste::buildSubCommands()
{
    Node* after = pos_.before();
    Node* before = (after) ? after->prevSibling() : pos_.node()->lastChild();
    if (clipboard_->firstChild())
        executeAndAdd(new PasteFragment(
                          clipboard_->firstChild()->copyAsFragment(
                              clipboard_->lastChild()), pos_));

    if (before && is_text_join_possible(before, before->nextSibling())) {
        if (before->nextSibling() == after)
            after = 0;
        executeAndAdd(new Join(before, before->nextSibling(), true));
    }
    if (after && is_text_join_possible(after->prevSibling(), after)) {
        executeAndAdd(new Join(after->prevSibling(), after));
    }
}
    
String Paste::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Paste");
        default: return String::null();
    }
}

const GrovePos& Paste::pos() const
{
    if (!pos_.before()) // if we are at the very end, remain there
        return pos_;
    return GroveMacroCommand::pos();
}

GroveLib::Node* Paste::validationContext() const
{
    return pos_.node();
}

///////////////////////////////////////////////////////

InsertPi::InsertPi(const GrovePos& pos,
                   const COMMON_NS::String& target,
                   const COMMON_NS::String& text)
    : pos_(pos)
{
    pi_ = new GroveLib::ProcessingInstruction;
    pi_->setTarget(target);
    pi_->setData(text);
    suggestedPos_ = pos;
}

void InsertPi::doExecute()
{
    pos_.insert(pi_.pointer());
    setEntityModified(pos_);
    suggestedPos_ = GrovePos(pi_.pointer());
}

void InsertPi::doUnexecute()
{
    pi_->remove();
    unsetEntityModified(pos_);
    suggestedPos_ = pos_;
}

String InsertPi::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Insert PI: <?%1 ...>");
        case 1:  return pi_->target();
        default: return String::null();
    }
}

///////////////////////////////////////////////////////

InsertComment::InsertComment(const GrovePos& pos,
                             const COMMON_NS::String& text)
    : pos_(pos)
{
    comment_ = new GroveLib::Comment;
    comment_->setComment(text);
    suggestedPos_ = pos;
}

void InsertComment::doExecute()
{
    pos_.insert(comment_.pointer());
    setEntityModified(pos_);
    suggestedPos_ = GrovePos(comment_.pointer());
}

void InsertComment::doUnexecute()
{
    comment_->remove();
    unsetEntityModified(pos_);
    suggestedPos_ = pos_;
}

String InsertComment::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Insert XML Comment");
        default: return String::null();
    }
}

///////////////////////////////////////////////////////

ChangePi::ChangePi(const GrovePos& pos,
                   const COMMON_NS::String& target,
                   const COMMON_NS::String& text)
    : pos_(pos)
{
    const GroveLib::ProcessingInstruction* pi =
        static_cast<const GroveLib::ProcessingInstruction*>(pos.node());
    oldtarget_ = pi->target();
    oldtext_   = pi->data();
    newtext_   = text;
    newtarget_ = target;
    suggestedPos_ = pos;
}

void ChangePi::doExecute()
{
    if (pos_.node()->nodeType() != GroveLib::Node::PI_NODE)
        throw GroveEditor::Exception(GroveEditorMessages::changePI);
    GroveLib::ProcessingInstruction* pi =
        static_cast<GroveLib::ProcessingInstruction*>(pos_.node());
    pi->setTarget(newtarget_);
    pi->setData(newtext_);
    pi->notifyForceModified();
    setEntityModified(pos_);
}

void ChangePi::doUnexecute()
{
    GroveLib::ProcessingInstruction* pi =
        static_cast<GroveLib::ProcessingInstruction*>(pos_.node());
    pi->setTarget(oldtarget_);
    pi->setData(oldtext_);
    pi->notifyForceModified();
    unsetEntityModified(pos_);
}

String ChangePi::info(uint idx) const
{
    switch (idx) {
        case 0:  
            if (newtarget_ != oldtarget_)
                return QT_TRANSLATE_NOOP("GroveEditor",
                    "Rename PI: '%1' -> '%2'");
            else
                return QT_TRANSLATE_NOOP("GroveEditor", "Change PI: '%1'");
        case 1: return oldtarget_;
        case 2: return (oldtarget_ != newtarget_) ? 
            newtarget_ : String::null();
        default: return String::null();
    }
}

///////////////////////////////////////////////////////

ChangeComment::ChangeComment(const GrovePos& pos,
                   const COMMON_NS::String& text)
    : pos_(pos)
{
    const GroveLib::Comment* c =
        static_cast<const GroveLib::Comment*>(pos.node());
    oldtext_   = c->comment();
    newtext_   = text;
    suggestedPos_ = pos;
}

void ChangeComment::doExecute()
{
    if (pos_.node()->nodeType() != GroveLib::Node::COMMENT_NODE)
        throw GroveEditor::Exception(GroveEditorMessages::changeComment);
    GroveLib::Comment* c = static_cast<GroveLib::Comment*>(pos_.node());
    c->setComment(newtext_);
    c->notifyForceModified();
    setEntityModified(pos_);
}

void ChangeComment::doUnexecute()
{
    GroveLib::Comment* c = static_cast<GroveLib::Comment*>(pos_.node());
    c->setComment(oldtext_);
    c->notifyForceModified();
    unsetEntityModified(pos_);
}

String ChangeComment::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Change XML Comment");
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////////////

ConvertFromSection::ConvertFromSection(GroveSectionStart* gss)
    : parent_(gss->parent()), gss_(gss)
{
    gse_  = gss->getSectEnd();
    from_ = gss_->nextSibling();
    to_   = gse_->prevSibling();
    after_ = gse_->nextSibling();
}

void ConvertFromSection::doExecute()
{
    if (gss_.isNull() || !gss_->getSectParent())
        throw GroveEditor::Exception(GroveEditorMessages::entityPos);
    Node* ljn = (gss_->nextSibling() != gse_.pointer())
        ? 0 : gss_->prevSibling();
    gss_->convertFromSection();
    suggestedPos_ = GrovePos();
    if (0 == ljn) {
        if (isTextJoinPossible(from_->prevSibling(), from_.pointer(), false)) {
            ljn = from_->prevSibling();
            leftJoin_ = new Join(ljn, from_.pointer());
            leftJoin_->execute();
            suggestedPos_ = leftJoin_->pos();
        }
        if (0 == ljn)
            ljn = to_.pointer();
        if (isTextJoinPossible(ljn, ljn->nextSibling(), false)) {
            rightJoin_ = new Join(ljn, ljn->nextSibling());
            rightJoin_->execute();
            if (suggestedPos_.isNull())
                suggestedPos_ = rightJoin_->pos();
        }
    } 
    else {
        if (isTextJoinPossible(ljn, ljn->nextSibling(), false)) {
            leftJoin_ = new Join(ljn, ljn->nextSibling());
            leftJoin_->execute();
            suggestedPos_ = leftJoin_->pos();
        }
    }
    if (suggestedPos_.isNull())
        suggestedPos_ = GrovePos(parent_.pointer(), after_.pointer());
}

void ConvertFromSection::doUnexecute()
{
    if (!rightJoin_.isNull())
        rightJoin_->unexecute();
    if (!leftJoin_.isNull())
        leftJoin_->unexecute();
    Node::GroveSectionType sectType = 
        (Node::GroveSectionType)(gss_->nodeType() & Node::SECT_TYPE_MASK);
    if (from_.pointer() == gse_.pointer()) {
        Comment* comment = new Comment("");
        GrovePos(parent_.pointer(), after_.pointer()).insert(comment);
        GroveSectionStart::convertToSection(sectType,
            comment, comment, gss_.pointer(), gse_.pointer());
        comment->remove();
    } 
    else
        GroveSectionStart::convertToSection(
            sectType, from_.pointer(), 
            to_.pointer(), gss_.pointer(), gse_.pointer());
    suggestedPos_ = GrovePos(parent_.pointer(), after_.pointer());
}
    
GroveLib::Node* ConvertFromSection::first() const
{
    return (from_.pointer() == gse_.pointer()) ? 0 : from_.pointer();
}

GroveLib::Node* ConvertFromSection::last() const
{
    return (from_.pointer() == gse_.pointer()) ? 0 : to_.pointer();
}

String ConvertFromSection::info(uint) const 
{
    return String::null();
}
 
PRTTI_BASE_STUB_NS(CommandContext, Command, Common)
PRTTI_IMPL2_NS(GroveCommand, Command, Common)
PRTTI_IMPL2_NS(GroveMacroCommand, Command, Common)
    
} // namespace GroveEditor
