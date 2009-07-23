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
 #include "common/asserts.h"

#include "grove/Grove.h"
#include "grove/GroveBuilder.h"
#include "grove/IdManager.h"
#include "grove/EntityDeclSet.h"
#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"
#include "grove/SectionSyncher.h"
#include "grove/udata.h"
#include "grove/xinclude.h"
#include "grove/XmlValidator.h"

#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommands.h"
#include "groveeditor/EntityCommands.h"
#include "groveeditor/Exception.h"
#include "groveeditor/RedlineCommands.h"
#include "groveeditor/groveeditor_debug.h"
#include "common/Vector.h"
#include "dav/DavManager.h"
#include "dav/IoStream.h"

#include <map>
#include <set>
#include <iostream>

#define GEDIT_TRY   try {
#define GEDIT_CATCH } catch (Common::MessageExceptionBase& e) { \
    errorMessage_ = (GroveEditorMessages::Messages)e.messageId(); return 0; }

#define GEDIT_CHECK_TEXT_POS  if (pos.type() != GrovePos::TEXT_POS) { \
        errorMessage_ = GroveEditorMessages::textPos; \
        return 0; \
    } \

using namespace Common;
using namespace GroveLib;

namespace GroveEditor
{

class ForcedCommand {
public:
    ForcedCommand(const GrovePos& grovePos);

    Command*         setCommand(Command* newCommand);
    const GrovePos&  pos() const { return elementPos_; }

private:
    GrovePos                 elementPos_;
    RefCntPtr<MacroCommand>  macro_;
};

ForcedCommand::ForcedCommand(const GrovePos& gp)
{
    GrovePos grovePos = gp.adjustBoundaryPos();
    if (GrovePos::TEXT_POS == grovePos.type()) {
        RefCntPtr<SplitText> split = new SplitText(grovePos);
        macro_ = new GroveMacroCommand;
        macro_->executeAndAdd(split.pointer());
        elementPos_ = split->elementPos();
    }
    else
        if (GrovePos::ELEMENT_POS == grovePos.type())
            elementPos_ = grovePos;
}

Command* ForcedCommand::setCommand(Command* newCommand)
{ 
    if (!macro_.isNull()) {
        macro_->executeAndAdd(newCommand);
        macro_->setContextCommand(newCommand);
        return macro_.pointer();
    }
    return newCommand;
}

#define DECLARE_REPLICATOR(pos) \
    GrovePosList pl; \
    get_pos_list(pos, pl); \
    COMMON_NS::RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;

#define DECLARE_NREPLICATOR(node) \
    NodePosList npl; \
    get_pos_list(node, npl); \
    COMMON_NS::RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;

/////////////////////////////////////////////////////////////////////////////

Editor::Editor()
    : errorMessage_((GroveEditorMessages::Messages)0)
{
}

Editor::~Editor()
{
}

bool Editor::checkSelection(const GrovePos& first, const GrovePos& last)
{
    GroveSelection selection(first, last);
    GroveSelection::Status status = selection.status();
    if (GroveSelection::IS_VALID_SELECTION == status)
        return true;
    switch (status) {
        case GroveSelection::IS_EMPTY_SELECTION:
        case GroveSelection::IS_NULL_SELECTION:
            errorMessage_ = GroveEditorMessages::errorOpSelection;
            break;
        case GroveSelection::IS_ATTRIBUTE_SELECTION :
            errorMessage_ = GroveEditorMessages::errorAttrSelection;
            break;
        case GroveSelection::IS_CROSS_SECTION :
            errorMessage_ = GroveEditorMessages::errorCrossEntity;
            break;
        default:
            break;
    };
    return false;
}

String Editor::errorMessage() const
{
    if (!errorMessage_ || GroveEditorMessages::errorEmpty == errorMessage_)
        return String();
    UintIdMessage message(errorMessage_, GroveEditorMessages::getFacility());
    return message.format(BuiltinMessageFetcher::instance());
}

CommandPtr Editor::insertText(const GrovePos& pos, const String& text)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        if (pos.type() == GrovePos::TEXT_POS)
            mc->executeAndAdd(new InsertText(pl[i], text));
        else
            mc->executeAndAdd(new InsertTextNode(pl[i], text));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::removeText(const GrovePos& pos, uint count)
{
    GEDIT_CHECK_TEXT_POS
    GEDIT_TRY
    DECLARE_REPLICATOR(pos);
    for (uint i = 0; i < pl.size(); ++i) {
        if (0 == pl[i].idx() && count == pl[i].text()->data().length())
            mc->executeAndAdd(new RemoveNode(pl[i].text()));
        else
            mc->executeAndAdd(new RemoveText(pl[i], count));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::replaceText(const GrovePos& pos, uint count,
                               const COMMON_NS::String& newText)
{
    GEDIT_CHECK_TEXT_POS
    GEDIT_TRY
    DECLARE_REPLICATOR(pos);
    for (uint i = 0; i < pl.size(); ++i)
        if (0 == pl[i].idx() && count == pl[i].text()->data().length() &&
            newText.isEmpty())
            mc->executeAndAdd(new RemoveNode(pl[i].text()));
        else
            mc->executeAndAdd(new ReplaceText(pl[i], count, newText));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::del(const GrovePos& srcpos)
{
    GEDIT_TRY
    GroveLib::Node* n = 0;
    if (srcpos.type() == GrovePos::TEXT_POS)
        n = srcpos.text()->nextSibling();
    else
        n = srcpos.before();
    if (!n || n->nodeType() == GroveLib::Node::ENTITY_REF_END_NODE) {
        errorMessage_ = GroveEditorMessages::errorEmpty;
        return 0;
    }
    if (!can_remove_xinclude_section(n)) {
        errorMessage_ = GroveEditorMessages::cantRemoveXincludeSect;
        return 0;
    }
    GrovePosList pl;
    get_pos_list(srcpos, pl, true);
    COMMON_NS::RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    for (uint i = 0; i < pl.size(); ++i) {
        const GrovePos& pos = pl[i];
        if (pos.type() == GrovePos::TEXT_POS) {
            RT_ASSERT((int)pos.text()->data().length() <= pos.idx());
            GroveLib::Node* ns = pos.text()->nextSibling();
            if (0 == ns) {
                errorMessage_ = GroveEditorMessages::errorDel;
                return 0;
            }
            mc->executeAndAdd(getDeleteCommand(ns));
        }
        else {
            if (!pos.before()) {
                errorMessage_ = GroveEditorMessages::errorDel;
                return 0;
            }
            mc->executeAndAdd(getDeleteCommand(pos.before()));
        }
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::backspace(const GrovePos& srcpos)
{
    GEDIT_TRY
    GroveLib::Node* n = 0;
    bool rflag = false;
    if (srcpos.type() == GrovePos::TEXT_POS)
        n = srcpos.text()->prevSibling();
    else {
        n = srcpos.before();
        if (n && n->nodeType() == GroveLib::Node::ENTITY_REF_START_NODE)
            rflag = true;
        n = srcpos.before() ?
            srcpos.before()->prevSibling() : srcpos.node()->lastChild();
    }
    if (!n || n->nodeType() == GroveLib::Node::ENTITY_REF_START_NODE) {
        errorMessage_ = GroveEditorMessages::errorEmpty;
        return 0;
    }
    if (!can_remove_xinclude_section(n)) {
        errorMessage_ = GroveEditorMessages::cantRemoveXincludeSect;
        return 0;
    }
    GrovePosList pl;
    get_pos_list(srcpos, pl, rflag);
    COMMON_NS::RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    for (uint i = 0; i < pl.size(); ++i) {
        const GrovePos& pos = pl[i];
        if (pos.type() == GrovePos::TEXT_POS) {
            RT_ASSERT(pos.idx() == 0);
            GroveLib::Node* ps = pos.text()->prevSibling();
            if (0 == ps) {
                errorMessage_ = GroveEditorMessages::errorBackspace;
                return 0;
            }
            mc->executeAndAdd(getDeleteCommand(ps));
        }
        else {
            n = pos.before() ? pos.before()->prevSibling()
                : pos.node()->lastChild();
            if (!n) {
                errorMessage_ = GroveEditorMessages::errorBackspace;
                return 0;
            }
            mc->executeAndAdd(getDeleteCommand(n));
        }
    }
    return mc.pointer();
    GEDIT_CATCH
}

////////////////////////////////////////////////////////////////////////////

CommandPtr Editor::insertElement(const GrovePos& pos,
                                 const String& elementName,
                                 const PropertyNode* attrMap)
{
    GEDIT_TRY
    if (elementName.isEmpty()) {
        errorMessage_ = GroveEditorMessages::errorNoName;
        return 0;
    }
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(
            new InsertElement(c.pos(), elementName, attrMap)));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::removeNode(const NodePtr& node)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(node);
    for (uint i = 0; i < npl.size(); ++i)
        mc->executeAndAdd(getDeleteCommand(npl[i].pointer()));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::splitElement(const GrovePos& pos)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        if (c.pos().type() == GrovePos::TEXT_POS)
            mc->executeAndAdd(c.setCommand(new SplitText(c.pos())));
        else
            mc->executeAndAdd(c.setCommand(new SplitElement(c.pos())));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::joinElements(const NodePtr& firstNode)
{
    if (!firstNode->nextSibling()) {
        errorMessage_ = GroveEditorMessages::joinEmpty;
        return 0;
    }
    GEDIT_TRY
    DECLARE_NREPLICATOR(firstNode);
    for (uint i = 0; i < npl.size(); ++i) {
        GroveLib::Node* n = npl[i].pointer();
        GroveLib::Node* fn = n->lastChild();
        GroveLib::Node* ln = n->nextSibling()->firstChild();
        mc->executeAndAdd(new Join(n, n->nextSibling()));
        if (fn && ln && fn->nodeType() == GroveLib::Node::TEXT_NODE &&
            ln->nodeType() == GroveLib::Node::TEXT_NODE)
                mc->executeAndAdd(new Join(fn, ln));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::renameElement(const GroveEditor::GrovePos& pos,
                                 const String& elementName)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new RenameElement(pl[i], elementName));
    return mc.pointer();
    GEDIT_CATCH
}

////////////////////////////////////////////////////////////////////////////

CommandPtr Editor::setAttribute(GroveLib::Attr* attr, const String& value)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(attr->element());
    for (uint i = 0; i < npl.size(); ++i) {
        Attr* repl_attr = static_cast<GroveLib::Element*>
            (npl[i].pointer())->attrs().getAttribute(attr->name());
        if (0 == repl_attr) {
            errorMessage_ = GroveEditorMessages::errorAttrReplace;
            return 0;
        }
        mc->executeAndAdd(new SetAttribute(repl_attr, value));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::removeAttribute(GroveLib::Attr* attr)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(attr->element());
    for (uint i = 0; i < npl.size(); ++i) {
        Attr* repl_attr = static_cast<GroveLib::Element*>
            (npl[i].pointer())->attrs().getAttribute(attr->name());
        if (0 == repl_attr) {
            errorMessage_ = GroveEditorMessages::errorAttrReplace;
            return 0;
        }
        mc->executeAndAdd(new RemoveAttribute(repl_attr));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::renameAttribute(GroveLib::Attr* attr,
                                   const PropertyNode* spec)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(attr->element());
    for (uint i = 0; i < npl.size(); ++i) {
        Attr* repl_attr = static_cast<GroveLib::Element*>
            (npl[i].pointer())->attrs().getAttribute(attr->name());
        if (0 == repl_attr) {
            errorMessage_ = GroveEditorMessages::errorAttrReplace;
            return 0;
        }
        mc->executeAndAdd(new RenameAttribute(repl_attr, spec));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::addAttribute(GroveLib::Element* element,
                                const PropertyNode* spec)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(element);
    for (uint i = 0; i < npl.size(); ++i)
        mc->executeAndAdd(new AddAttribute
            (static_cast<GroveLib::Element*>(npl[i].pointer()), spec));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::mapXmlNs(GroveLib::Element* element,
                            const Common::String& prefix,
                            const Common::String& uri)
{
    GEDIT_TRY
    DECLARE_NREPLICATOR(element);
    for (uint i = 0; i < npl.size(); ++i)
        mc->executeAndAdd(new MapXmlNs
            (static_cast<GroveLib::Element*>(npl[i].pointer()), prefix, uri));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::cut(const GrovePos& from, const GrovePos& to, 
                       GrovePos* adjustPos)
{
    GEDIT_TRY
    GrovePos adj_from(from.adjustChoice());
    GrovePos adj_to(to.adjustChoice());
    if (!checkSelection(adj_from, adj_to))
        return 0;
    GrovePosList pl_from, pl_to;
    get_pos_list(adj_from, pl_from, true);
    get_pos_list(adj_to,   pl_to);
    if (pl_from.size() != pl_to.size()) {
        errorMessage_ = GroveEditorMessages::errorSelReplace;
        return 0;
    }
    RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    for (uint i = 0; i < pl_from.size(); ++i)
        mc->executeAndAdd(new RemoveSelection(
                              pl_from[i], pl_to[i], adjustPos));
    return mc.pointer();
    GEDIT_CATCH
}

bool Editor::copy(const GrovePos& from, const GrovePos& to,
                  const FragmentPtr& clipboard)
{
    try {
        GrovePos adj_from(from.adjustChoice());
        GrovePos adj_to(to.adjustChoice());
        if (!checkSelection(adj_from, adj_to))
            return false;
        CommandPtr cmd = new Copy(adj_from, adj_to, clipboard.pointer());
        cmd->execute();
        clipboard->setGrove(from.node()->grove());
    } 
    catch (Exception& e) {
        errorMessage_ = (GroveEditorMessages::Messages)e.messageId();
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////

namespace {

struct EntitySubstData {
    EntitySubstData(const EntityDecl* decl, EntityReferenceStart* masterErs,
                    FragmentPtr& masterFragment, NodePtr anchor)
        : decl_(decl), masterErs_(masterErs), 
          masterFragment_(masterFragment.pointer()), anchor_(anchor) {}
    
    const EntityDecl*       decl_;
    EntityReferenceStart*   masterErs_;
    FragmentPtr             masterFragment_;
    NodePtr                 anchor_;
};

} // namespace

/////////////////////////////////////////////////////////////////////////////

static void substitute_references(FragmentPtr& frag, Grove* targetGrove,
                                  EntityReferenceStart* targetErs,
                                  RefCntPtr<MacroCommand>& mc, bool insertRef)
{
    if (!frag->ers())
        return;
    //! Collect all top-level entity references (which are not nested) for
    //  substitution
    typedef EntityReferenceStart ERS;
    std::list<ERS*> ers_list;

    for (ERS* ers = static_cast<ERS*>(frag->ers()->getFirstSect()); ers;
         ers = static_cast<ERS*>(ers->getNextSect())) {
        if (ers->entityDecl()->declType() == EntityDecl::xinclude)
            continue;
        ers_list.push_back(ers);
    }

    EntityDeclSet& target_decl_set = *targetErs->entityDecls();
    typedef std::list<EntitySubstData> SubstitutionList;
    SubstitutionList subst_list;
    typedef std::map<const EntityDecl*, FragmentPtr> DeclFragmentMap;
    DeclFragmentMap fragment_map;

    //! For all collected entity references: insert anchor node (TextNode)
    //  for further substitution, and remove entity reference itself
    for (std::list<ERS*>::iterator i = ers_list.begin(); 
         i != ers_list.end(); ++i) {
        ERS* ers = *i;
        const EntityDecl* target_decl =
            target_decl_set.lookupDecl(ers->entityDecl()->name());

        //! If no such (with same name) entity declared in target grove
        //! declare it using content of source entity declaration
        if (0 == target_decl) {
            if (insertRef) 
                throw GroveEditor::Exception(GroveEditorMessages::errorPaste2);

            const EntityDecl* master_decl = ers->entityDecl();
            if (EntityDecl::undefined == master_decl->declType()) {
                if (frag->grove() && frag->grove()->entityDecls())
                    master_decl = frag->grove()->entityDecls()->lookupDecl(
                        master_decl->name());
                //! For unknown entity create empty internal entity
                if (master_decl && 
                    EntityDecl::undefined == master_decl->declType()) 
                    master_decl = 0;
            }
            //! Declare missing entity
            if (master_decl)
                mc->executeAndAdd(new DeclareEntity(targetGrove, master_decl));
            else
                mc->executeAndAdd(new DeclareEntity(
                                      targetGrove, ers->entityDecl()->name()));
            target_decl = 
                target_decl_set.lookupDecl(ers->entityDecl()->name());
        }

        //! Find first entity reference
        const EntityReferenceTable::ErtEntry* master_ert_entry =
            targetGrove->document()->ert()->lookup(target_decl);
        //if (0 == master_ert_entry) 
        //    master_ert_entry = referencesTable->findErtEntry(target_decl);
        EntityReferenceStart* master_ers = 
            (master_ert_entry) ? master_ert_entry->node(0) : 0;

        //! If declared entity has no references in target grove, parse it`s
        //  content and keep it as DocumentFragment
        FragmentPtr master_fragment;
        if (0 == master_ers) {
            DeclFragmentMap::iterator i = fragment_map.find(target_decl);
            if (i != fragment_map.end())
                master_fragment = (*i).second;
            else {
                master_fragment = target_decl->parse(targetErs);
                if (master_fragment.isNull())
                    master_fragment = new DocumentFragment;
                master_fragment->setGrove(frag->grove());
                //! Recursively substitute entity references in parsed fragment
                substitute_references(master_fragment, targetGrove, 
                                     targetErs, mc, insertRef);
                fragment_map[target_decl] = master_fragment;
            }
        }
        EntitySubstData data(target_decl, master_ers,
                             master_fragment, new Text());
        subst_list.push_back(data);
        ers->insertBefore(data.anchor_.pointer());
        ers->removeGroup(ers->getSectEnd());
    }
    //! Substitute entity references
    for (SubstitutionList::iterator i = subst_list.begin(); 
         i != subst_list.end(); ++i) {
        EntitySubstData subst = *i;
        if (subst.masterErs_) {
            frag->insertEntityRef(subst.masterErs_, subst.anchor_.pointer(), 
                                  subst.anchor_->parent());
        }
        else {
            FragmentPtr master_ref;
            if (!subst.masterFragment_.isNull() && 
                subst.masterFragment_->firstChild())
                master_ref = subst.masterFragment_->firstChild()->
                    copyAsFragment(subst.masterFragment_->lastChild());
            else
                master_ref = new DocumentFragment;
            frag->insertEntityRef(
                const_cast<EntityDecl*>(subst.decl_), master_ref.pointer(),
                subst.anchor_.pointer(), subst.anchor_->parent());
        }
        subst.anchor_->remove();
    }
}

typedef std::set<String> UsedIdSet;

static void set_unique_ids(const IdManager* idm, Node* node, 
                           UsedIdSet& idset, const Node* context)
{
    node->setUdata(node->udata() & ~REDLINE_MASK);
    if (node->nodeType() == GroveLib::Node::ELEMENT_NODE) {
        Attr* attr = CONST_ELEMENT_CAST(node)->attrs().firstChild();
        for (; attr; attr = attr->nextSibling()) {
            if (attr->idClass() != Attr::IS_ID && 
                attr->idSubClass() != Attr::SCOPED_ID)
                    continue;
            if (!idm->lookupScopedElement(attr->value(), context))
                continue;
            String value = attr->value();
            uint index = 1;
            if (value.length() >= 2) {
                const Char* cs = value.unicode();
                const Char* ce = cs + value.length();
                const Char* cp = ce - 1;
                while (cp > cs && cp->isDigit())
                    --cp;
                if (cp >= cs && (ce - cp) > 0 && cp->unicode() == '_') {
                    ++cp;
                    bool ok = false;
                    index = String(cp, ce - cp).toUInt(&ok);        
                    if (ok && index > 0) 
                        value = String(cs, cp - cs - 1);
                    else
                        index = 1;
                }
            }
            String new_id;
            for (;;) {
                new_id = value + "_" + String::number(index);
                if (!idm->lookupScopedElement(new_id, context) && 
                    idset.find(new_id) == idset.end())
                        break;
                ++index;
            }
            attr->setValue(new_id);
            idset.insert(new_id);
        }
    }
    for (node = node->firstChild(); node; node = node->nextSibling())
        set_unique_ids(idm, node, idset, context);
}

GROVEEDITOR_EXPIMP void set_unique_ids(Node* node, const Node* context)
{
    UsedIdSet idset;
    set_unique_ids(context->grove()->idManager(), node, idset, context);
}

CommandPtr Editor::paste(const FragmentPtr& clipboard, const GrovePos& pos,
                         int toDel, bool insertRef)
{
    GEDIT_TRY
    GrovePos adj_pos(pos.adjustChoice());
    DECLARE_REPLICATOR(adj_pos);
    EntityReferenceTable::iterator ert_it;
    EntityReferenceStart* my_ers = adj_pos.getErs();

    //! Check for entity reference loops
    if (clipboard->ert()) {
        for (ert_it = clipboard->ert()->begin();
             ert_it != clipboard->ert()->end(); ++ert_it) {

            if (my_ers->hasLoop((*ert_it)->decl()->name())) {
                errorMessage_ = GroveEditorMessages::entityRecursive;
                return 0;
            }
        }
    }
    //! Make deep copy of clipboard to operate on it
    FragmentPtr frag = (clipboard->firstChild())
        ? clipboard->firstChild()->copyAsFragment(clipboard->lastChild())
        : new DocumentFragment;
    frag->setGrove(clipboard->grove());
    set_unique_ids(frag.pointer(), adj_pos.contextNode());

    //! Substitute fragment`s entity references with content
    //! of references declared in target grove
    Grove* target_grove = get_current_grove(adj_pos.node());
    substitute_references(frag, target_grove, adj_pos.getErs(), mc, insertRef);
    
    for (uint i = 0; i < pl.size(); ++i) {
        if (toDel)
            mc->executeAndAdd(new RemoveText(pl[i], toDel));
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(new Paste(frag.pointer(), c.pos())));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::tagRegion(const GrovePos& from,
                             const GrovePos& to,
                             const COMMON_NS::String& elemName,
                             const PropertyNode* attrMap)
{
    GEDIT_TRY
    GrovePos adj_from(from.adjustChoice());
    GrovePos adj_to(to.adjustChoice());
    if (!checkSelection(adj_from, adj_to))
        return 0;
    GrovePosList pl_from, pl_to;
    get_pos_list(adj_from, pl_from, true);
    get_pos_list(adj_to,   pl_to);
    if (pl_from.size() != pl_to.size()) {
        errorMessage_ = GroveEditorMessages::errorSelReplace;
        return 0;
    }
    RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    for (uint i = 0; i < pl_from.size(); ++i)
        mc->executeAndAdd(new TagRegion(pl_from[i], pl_to[i],
            elemName, attrMap));
    return mc.pointer();
    GEDIT_CATCH
}

///////////////////////////////////////////////////////////////////////

CommandPtr Editor::convertToEntity(const GrovePos& first,
                                   const GrovePos& last,
                                   const COMMON_NS::String& name,
                                   const GroveLib::ExternalId* externalId)
{
    GEDIT_TRY
    if (!checkSelection(first, last))
        return 0;
    GrovePosList pl_first, pl_last;
    get_pos_list(first, pl_first, true);
    get_pos_list(last,  pl_last);
    if (pl_first.size() != pl_last.size()) {
        errorMessage_ = GroveEditorMessages::errorSelReplace;
        return 0;
    }
    RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    ConvertToEntityData data;
    data.name = name;
    if (externalId) {
        data.isExternalEntity = true;
        data.externalId = *externalId;
    } 
    else
        data.isExternalEntity = false;
    for (uint i = 0; i < pl_first.size(); ++i)
        mc->executeAndAdd(
            new ConvertToEntitySelection(pl_first[i], pl_last[i], data, !!i));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::convertFromEntity(const GrovePos& pos, bool convertAll)
{
    GEDIT_TRY
    EntityReferenceStart* ers = pos.getErs();
    if (!can_remove_xinclude_section(ers)) {
        errorMessage_ = GroveEditorMessages::cantRemoveXincludeSect;
        return 0;
    }
    if (convertAll) {
        DECLARE_REPLICATOR(pos);
        for (uint i = 0; i < pl.size(); ++i)
            mc->executeAndAdd(new ConvertFromEntity(pl[i]));
        return mc.pointer();
    } 
    else {
        Common::RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
        if (!ers->getSectParent()->getSectParent())
            return new ConvertFromEntity(GrovePos(ers));
        EntityReferenceStart* pers = ERS_CAST(ers->getSectParent());
        const EntityReferenceTable::ErtEntry* ee =
            pers->getGSR()->ert()->lookup(pers->entityDecl());
        if (ee->numOfRefs() < 2)
            return new ConvertFromEntity(GrovePos(ers));
        // calculate ers offset in parent section node
        uint ers_index = 0;
        GroveSectionStart* gss = pers->getFirstSect();
        for (; gss; gss = gss->getNextSect()) {
            if (gss == ers)
                break;
            ers_index++;
        }
        // for each parent reference, find corresponding sub-ers
        Vector<EntityReferenceStart*> ers_list;
        ers_list.push_back(ers);
        for (uint i = 0; i < ee->numOfRefs(); ++i) {
            gss = ee->node(i)->getFirstSect();
            for (uint j = 0; j < ers_index; ++j)
                gss = gss->getNextSect();
            if (gss != ers)
                ers_list.push_back(ERS_CAST(gss));
        }
        for (uint i = 0; i < ers_list.size(); ++i)
            mc->executeAndAdd(new ConvertFromEntity(GrovePos(ers_list[i])));
        return mc.pointer();
    }
    GEDIT_CATCH
}

CommandPtr Editor::insertEntity(const GrovePos& pos,
                                const String& entityName,
                                const String& path,
                                const GroveLib::StripInfo* si)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(new InsertEntity(c.pos(),
            entityName, path, si)));
    }
    return mc.pointer();
    GEDIT_CATCH
}


CommandPtr Editor::untag(const GrovePos& pos)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new Untag(pl[i]));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::changeEntityDecl(const GroveEditor::GrovePos& pos,
                                    const String& name,
                                    const String& newName,
                                    const String& newPath)
{
    GEDIT_TRY
    return new ChangeEntityDecl(pos, name, newName, newPath);
    GEDIT_CATCH
}

CommandPtr Editor::declareEntity(const GroveEditor::GrovePos& pos,
                                 const PropertyNode* params)
{
    String s = params->getSafeProperty("data-type")->getString();
    EntityDecl::DataType data_type = EntityDecl::cdata;
    if (NOTR("sgml") == s)
        data_type = EntityDecl::sgml;
    else if (NOTR("ndata") == s)
        data_type = EntityDecl::ndata;
    String notation = params->getSafeProperty("notation")->getString();
    if (!s.isEmpty())
        data_type = EntityDecl::ndata;
    String content = params->getSafeProperty("content")->getString();
    EntityDeclPtr entity_decl;
    s = params->getSafeProperty("decl-type")->getString();
    if (NOTR("notation") == s) {
        Notation* notation = new Notation;
        notation->setDeclType(EntityDecl::notation);
        notation->setSysid(content);
        entity_decl = notation;
    } else if (NOTR("external") == s) {
        ExternalEntityDecl* ext_decl = new ExternalEntityDecl;
        ext_decl->setDeclType(EntityDecl::externalGeneralEntity);
        ext_decl->setSysid(content);
        ext_decl->setNotationName(notation);
        entity_decl = ext_decl;
    } else {
        InternalEntityDecl* int_decl = new InternalEntityDecl;
        int_decl->setDeclType(EntityDecl::internalGeneralEntity);
        int_decl->setContent(content);
        entity_decl = int_decl;
    }
    entity_decl->setEntityName(params->getSafeProperty("name")->getString());
    entity_decl->setDataType(data_type);
    entity_decl->setDeclOrigin(EntityDecl::prolog);
    GEDIT_TRY
    return new DeclareEntity(pos.getErs()->currentGrove(),
                             entity_decl.pointer());
    GEDIT_CATCH
}
/////////////////////////////////////////////////////////////////////////////

static XincludeDecl* get_xinclude_decl(Node* node, Grove* mainGrove, 
                                       RefCntPtr<MacroCommand>& mc, 
                                       const String& url, 
                                       const String& xpointer, 
                                       const String& encoding, 
                                       bool isConverted)
{
    //! Look for declaration
    Node* prev = node->prevSibling();
    if (prev && prev->nodeType() == Node::ENTITY_REF_START_NODE) {
        EntityReferenceStart* ers = ERS_CAST(prev);
        if (ers->entityDecl()->declType() == EntityDecl::xinclude)
            return static_cast<XincludeDecl*>(ers->entityDecl());
    }
    //! Create declaration
    RefCntPtr<DeclareXinclude> declare_cmd = new DeclareXinclude(
        node, mainGrove, url, xpointer, encoding, isConverted);
    mc->executeAndAdd(declare_cmd.pointer());
    XincludeDecl* const decl = declare_cmd->decl();
    if (decl->referenceErs()->grove()->topDecl()->isReadOnly())
        decl->setReadOnly(true);
    return decl;
}

static String xinclude_url(const String& href, const GrovePos& pos)
{
    Url url(href);
    return (url.isRelative()) 
        ? url : Url(get_ers(pos.contextNode())->xmlBase()).relativePath(url);
}

CommandPtr Editor::insertXinclude(const GrovePos& pos, Grove* targetGrove,
                                  bool insertAsText, const PropertyNode* props)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());

    const String& xpointer = 
        props->getSafeProperty(NOTR("xpointer"))->getString();
    const String& encoding = 
        props->getSafeProperty(NOTR("encoding"))->getString();
    const String url = xinclude_url(
        props->getSafeProperty(NOTR("href"))->getString(), pos);

    GroveLib::Node* include_node = findXincludeNode(pos, targetGrove, 
                                                    xpointer, insertAsText);
    if (0 == include_node)
        return 0;
    
    //! Declare xinclude node if necessary
    XincludeDecl* decl = get_xinclude_decl(
        include_node, pos.node()->grove(), mc, url, xpointer, encoding, false);

    //! Include node
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(
            c.setCommand(new InsertXinclude(
                             c.pos(), decl, url, xpointer, encoding, 
                             props->getSafeProperty(NOTR("attrs")))));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::insertXinclude(const GrovePos& pos,
                                  const PropertyNode* props,
                                  const StripInfo* stripInfo, bool isText)
{
    if (0 == props) {
        errorMessage_ = GroveEditorMessages::xincludeMustSpecifyProps;
        return 0;
    }
    //! Get uri for xinclusion 
    String href = props->getSafeProperty(NOTR("href"))->getString();
    if (href.isEmpty()) {
        errorMessage_ = GroveEditorMessages::xincludeHrefMissing;
        return 0;
    } 
    //! Build grove for inclusion
    GrovePtr grove = buildXincludeGrove(pos, href,
        props->getSafeProperty(NOTR("encoding"))->getString(), isText);
    if (grove.isNull()) 
        return 0;
    if (!isText && stripInfo)
        stripInfo->strip(grove->document()->documentElement());

    return insertXinclude(pos, grove.pointer(), isText, props);
}

CommandPtr Editor::convertToXinclude(const GrovePos& first,
                                     const GrovePos& last,
                                     const GrovePtr& grove,
                                     const PropertyNode* props)
{
    GEDIT_TRY
    if (!checkSelection(first, last))
        return 0;
    if (first.node() != last.node())
        return 0;

    // Check that xinclude does not already included or used as entity
    const EntityReferenceStart* base_ers = first.getErs();
    String adjusted_path = base_ers->xmlBase(
        props->getSafeProperty(NOTR("href"))->getString());
    
    if (check_has_entity(first.node()->grove(), adjusted_path)) {
        errorMessage_ = GroveEditorMessages::xincludedAsExternalEntity;
        return 0;
    }
    if (find_grove(first.node()->grove(), adjusted_path)) {
        errorMessage_ = GroveEditorMessages::xincludeAlreadyExists;
        return 0;
    }
    // Replicate selections
    GrovePosList pl_first, pl_last;
    get_pos_list(first, pl_first, true);
    get_pos_list(last,  pl_last);
    if (pl_first.size() != pl_last.size()) {
        errorMessage_ = GroveEditorMessages::errorSelReplace;
        return 0;
    }
    // collect active namespace mappings
    typedef std::map<String, String> NsMap;
    NsMap ns_map;
    NsMap::iterator nmi;
    const Node* nsNode = first.node();
    for (; nsNode; nsNode = parentNode(nsNode)) {
        if (nsNode->nodeType() != Node::ELEMENT_NODE)
            continue;
        const XmlNsMapItem* xmi =
            CONST_ELEMENT_CAST(nsNode)->nsMapList().firstChild();
        for (; xmi; xmi = xmi->nextSibling()) {
            if (NOTR("xml") == xmi->prefix())
                continue;
            nmi = ns_map.find(xmi->prefix());
            if (nmi == ns_map.end())
                ns_map[xmi->prefix()] = xmi->uri();
        }
    }
    grove->setTopSysid(adjusted_path);
    const String& xpointer =
        props->getSafeProperty(NOTR("xpointer"))->getString();
    const String& encoding =
        props->getSafeProperty(NOTR("encoding"))->getString();
    const String url = xinclude_url(
        props->getSafeProperty(NOTR("href"))->getString(), first);

    RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    Element* docElem = grove->document()->documentElement();
    XincludeDecl* decl = 0;
    if (docElem) {
        //! Declare xinclude node if necessary
        decl = get_xinclude_decl(docElem, first.node()->grove(),
            mc, url, xpointer, encoding, true);

        // re-declare namespaces on top element
        for (nmi = ns_map.begin(); nmi != ns_map.end(); ++nmi)
            if (!nmi->second.isEmpty())
                mc->executeAndAdd(new MapXmlNs(docElem,
                    nmi->first, nmi->second));
    } else {
        decl = get_xinclude_decl(grove->document()->firstChild(),
            first.node()->grove(), mc, url, xpointer, encoding, true);
    }    
    //! Replace selections with xincludes
    for (uint i = 0; i < pl_first.size(); ++i) {
        CommandPtr cut = new RemoveSelection(pl_first[i], pl_last[i]);
        if (cut.isNull()) {
            errorMessage_ = GroveEditorMessages::errorSelReplace;
            return 0;
        }
        mc->executeAndAdd(cut.pointer());
        ForcedCommand c(cmd_pos(cut.pointer()));
        mc->executeAndAdd(c.setCommand(new InsertXinclude(
                              c.pos(), decl, url, xpointer, encoding,
                              props->getSafeProperty(NOTR("attrs")))));
    }
    return mc.pointer();
    GEDIT_CATCH
}

static bool has_parent(const GroveLib::Node* n1, const GroveLib::Node* n2)
{
    for (; n1 && n2 && n1 != n2; n1 = parentNode(n1))
        ;
    return n1 && n2 && n1 == n2;
}

Node* Editor::findXincludeNode(const GrovePos& pos, GroveLib::Grove* grove,
                               const String& xpointer, bool isText) 
{
    //! Find node by xpointer
    GroveLib::Node* target_node = 0;
    if (isText) {
        target_node = GroveLib::get_grove_text(grove);
        if (0 == target_node) {
            errorMessage_ = GroveEditorMessages::xincludeBadTextGrove;
            return 0;
        }
    } 
    else {
        target_node  = xptr_element(grove, xpointer);
        if (0 == target_node) {
            errorMessage_ = GroveEditorMessages::xincludeTargetNotFound;
            return 0;
        }
    }
    //! Check for recursion
    if (has_parent(pos.node(), target_node) ||
        xinclude_check_recursion(get_ers(pos.node()), target_node)) {
        errorMessage_ = GroveEditorMessages::xincludeRecursion;
        return 0;
    }
    
    return target_node;
}

GrovePtr Editor::buildXincludeGrove(const GrovePos& pos, const String& url, 
                                    const String& encodingStr, bool isText)
{
    const EntityReferenceStart* base_ers = pos.getErs();
    String adjusted_path = base_ers->xmlBase(url);
    
    if (check_has_entity(pos.node()->grove(), adjusted_path)) {
        errorMessage_ = GroveEditorMessages::xincludedAsExternalEntity;
        return 0;
    }
    GrovePtr grove = find_grove(pos.node()->grove(), adjusted_path);
    if (!grove.isNull()) {
        const EntityDecl::DataType type = (isText) 
            ? EntityDecl::cdata : EntityDecl::sgml;
        if (grove->topDecl()->dataType() != type) {
            errorMessage_ = (isText) 
                ? GroveEditorMessages::xincludedAsXml
                : GroveEditorMessages::xincludedAsText;
            return 0;
        }
        return grove;
    }
    
    //! Build grove if not already built 
    OwnerPtr<GroveBuilder> builder(get_current_grove(base_ers)->
                                   groveBuilder()->copy());
    builder->setFlags(builder->flags() | 
                      GroveBuilder::disableXincludeBackrefs);
    Encodings::Encoding encoding = builder->getEncoding();
    if (!encodingStr.isEmpty()) {
        encoding = Encodings::encodingByName(encodingStr);
        if (encoding == Encodings::XML)
            encoding = Encodings::UTF_8;
        builder->setEncoding(encoding);
    } 
    else 
        if (!isText)
            builder->setEncoding(Encodings::XML);
    MessageList message_list;
    builder->setMessenger(new MessageListMessenger(message_list));

    //! Make special grove to keep unparsed text for inclusion
    if (isText) {
        Dav::IoStream stream;
        if (Dav::DAV_RESULT_OK != Dav::DavManager::instance().open(
                adjusted_path, Dav::DAV_OPEN_READ, stream)) {
            errorMessage_ = GroveEditorMessages::xincludeCannotReadUrl;
            return 0;
        }
        stream.setEncoding(encoding);
        String str;
        stream >> str;
        grove = new Grove;
        grove->setGroveBuilder(builder.release());
        grove->topDecl()->setEncoding(encoding);
        grove->document()->appendChild(new Text(str));
        grove->topDecl()->setDataType(EntityDecl::cdata);
        grove->setTopSysid(adjusted_path);
    }
    else { //! Parse document for structural inclusion
        DDBG << "Building XINCLUDE grove " << adjusted_path
            << ", flags=" << builder->flagsAsString() << std::endl;
        grove = builder->buildGroveFromFile(adjusted_path, false);
        if (grove.isNull()) {
            errorMessage_ = GroveEditorMessages::xincludeCannotParseDocument;
            return 0;
        }
        if (!grove->document()->documentElement()) {
            builder.release(); 
            grove.clear();
            errorMessage_ = GroveEditorMessages::xincludeNoDocElement;
            return 0;
        }
        XmlValidatorPtr validator = (builder->getValidatorProvider())
            ? builder->getValidatorProvider()->getValidator(grove.pointer())
            : XmlValidatorPtr();
        if (validator)
            validator->validate(grove->document()->documentElement(),
                                XmlValidator::OPEN_MODE | 
                                XmlValidator::HIDE_PROGRESS);
        builder.release();
    }
    if (grove.isNull())
        errorMessage_ = GroveEditorMessages::xincludeCannotParseDocument;
    return grove;
}

GrovePtr Editor::buildXincludeGrove(const GrovePos& first,
                                    const GrovePos& last)
{
    if (!checkSelection(first, last))
        return 0;
    if (first.node() != last.node())
        return 0;

    //! Copy selected content 
    FragmentPtr fragment = new DocumentFragment;
    if (!copy(first, last, fragment) || 1 != fragment->countChildren()) 
        return 0;
    
    //! Create grove for inclusion
    GrovePtr grove = new Grove;
    grove->setIdManager(new IdManager);
    grove->idManager()->enable(grove->document());
    grove->setGroveBuilder(
        get_current_grove(first.node())->groveBuilder()->copy());
    grove->topDecl()->setEncoding(grove->groveBuilder()->getEncoding());
    grove->document()->appendChild(fragment.pointer());

    const bool insert_as_text = 
        (Node::TEXT_NODE == grove->document()->firstChild()->nodeType());
    grove->topDecl()->setDataType(
        (insert_as_text) ? EntityDecl::cdata : EntityDecl::sgml);
    
    return grove;
}

///////////////////////////////////////////////////////////////////////

CommandPtr Editor::insertPi(const GrovePos& pos,
                            const COMMON_NS::String& target,
                            const COMMON_NS::String& text)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(new InsertPi(c.pos(), target, text)));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::insertComment(const GrovePos& pos,
                                 const COMMON_NS::String& text)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(new InsertComment(c.pos(), text)));
    }
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::changePi(const GrovePos& pos,
                            const COMMON_NS::String& target,
                            const COMMON_NS::String& text)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new ChangePi(pl[i], target, text));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::changeComment(const GrovePos& pos,
                                 const COMMON_NS::String& text)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new ChangeComment(pl[i], text));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::convertToRedline(const GrovePos& from,
                                    const GrovePos& to,
                                    uint redlineMask,
                                    const Common::String& annotation)
{
    GEDIT_TRY
    GrovePos adj_from(from.adjustChoice());
    GrovePos adj_to(to.adjustChoice());
    if (!checkSelection(adj_from, adj_to))
        return 0;
    GrovePosList pl_from, pl_to;
    get_pos_list(adj_from, pl_from, true);
    get_pos_list(adj_to,   pl_to);
    if (pl_from.size() != pl_to.size()) {
        errorMessage_ = GroveEditorMessages::errorSelReplace;
        return 0;
    }
    RefCntPtr<RedlineData> rd = new RedlineData(redlineMask, annotation);
    RefCntPtr<MacroCommand> mc = new GroveReplicatedCommand;
    for (uint i = 0; i < pl_from.size(); ++i)
        mc->executeAndAdd(new NewRedlineSelection(pl_from[i], pl_to[i],
            rd.pointer()));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::convertFromRedline(const GrovePos& pos)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new ConvertFromRedline(pl[i]));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::changeRedline(const GrovePos& pos,
                                 uint redlineMask,
                                 const COMMON_NS::String& annotation)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    for (uint i = 0; i < pl.size(); ++i)
        mc->executeAndAdd(new ChangeRedline(pl[i], redlineMask, annotation));
    return mc.pointer();
    GEDIT_CATCH
}

CommandPtr Editor::insertRedline(const GrovePos& pos,
                                 uint redlineMask,
                                 const COMMON_NS::String& annotation)
{
    GEDIT_TRY
    DECLARE_REPLICATOR(pos.adjustChoice());
    RefCntPtr<RedlineData> rd = new RedlineData(redlineMask, annotation);
    for (uint i = 0; i < pl.size(); ++i) {
        ForcedCommand c(pl[i]);
        mc->executeAndAdd(c.setCommand(new InsertRedline(c.pos(),
            rd.pointer())));
    }
    return mc.pointer();
    GEDIT_CATCH
}

///////////////////////////////////////////////////////////////////////

Command* Editor::getDeleteCommand(const NodePtr& node) const
{
    if (!(node->nodeType() & Node::SECT_NODE_MASK))
        return new RemoveNode(node);
    if (node->nodeType() & Node::SECT_END_BIT)
        return new RemoveGroup(
            static_cast<GroveSectionEnd*>(node.pointer())->getSectStart(), 
            node.pointer());
    else
        return new RemoveGroup(
            node.pointer(), 
            static_cast<GroveSectionStart*>(node.pointer())->getSectEnd());
}

} // namespace GroveEditor

