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
#include "groveeditor/GroveCommands.h"
#include "groveeditor/EntityCommands.h"
#include "groveeditor/Exception.h"
#include "groveeditor/groveeditor_debug.h"

#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/XNodePtr.h"
#include "grove/GroveBuilder.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/SectionSyncher.h"
#include "grove/xinclude.h"
#include "grove/XmlPredefNs.h"
#include "common/Url.h"
#include <iostream>

USING_GROVE_NAMESPACE
USING_COMMON_NS

#ifndef QT_TRANSLATE_NOOP
# define QT_TRANSLATE_NOOP(context, text) text
#endif // QT_TRANSLATE_NOOP

namespace GroveEditor {

/////////////////////////////////////////////////////////////////////

InsertEntity::InsertEntity(const GrovePos& elementPos,
                           const String& name,
                           const String& path,
                           const GroveLib::StripInfo* stripInfo)
    : pos_(elementPos), 
      stripInfo_(stripInfo), 
      declCreated_(false)
{
    if (pos_.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
        pos_ = GrovePos(pos_.node()->parent(), pos_.node()->nextSibling());
    EntityDeclSet* decl_set = entity_decls(pos_);
    decl_ = decl_set->lookupDecl(name);
    if (decl_.isNull() && !path.isEmpty()) {
        ExternalEntityDecl* eed = new ExternalEntityDecl;
        declCreated_ = true;
        decl_ = eed;
        eed->setDeclType(EntityDecl::externalGeneralEntity);

        ExternalId ext_id;
        ext_id.setSysid(path);
        eed->setExternalId(ext_id);

        decl_->setDataType(EntityDecl::sgml);
        decl_->setDeclOrigin(EntityDecl::prolog);
        decl_->setEntityName(name);
        decl_->setDeclModified(true);
        decl_->setContentModified(false);
    }
}

String InsertEntity::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Insert Reference to Entity '%1'");
        case 1:  return decl_->name(); 
        default: return String::null();
    }
}

GroveLib::Node* InsertEntity::validationContext() const
{
    return pos_.node();
}

Node* InsertEntity::parseExternalEntity() const
{
    bool isReadonly = false;
    DocumentFragment* df = decl_->parse(get_ers(pos_.node()), &isReadonly);
    if (0 == df)
        throw GroveEditor::Exception(GroveEditorMessages::entityNotParsed);
    decl_->setReadOnly(isReadonly);
    return df;
}

Node* InsertEntity::parseInternalEntity() const
{
    DocumentFragment* df = decl_->parse(get_ers(pos_.node()));
    return df ? df : new DocumentFragment;
}

void InsertEntity::doExecute()
{
    if (decl_.isNull())
        throw GroveEditor::Exception(GroveEditorMessages::entityDeclNotFound);
    if (EntityDecl::internalGeneralEntity != decl_->declType() &&
        EntityDecl::externalGeneralEntity != decl_->declType())
            throw GroveEditor::Exception(
                GroveEditorMessages::entityGeneralRef);
    if (declCreated_)
        entity_decls(pos_)->insertDecl(decl_.pointer(), true);
    suggestedPos_ = pos_;
    if (!removedRef_.isNull()) {
        pos_.insert(removedRef_.pointer());
        setEntityModified(pos_);
        return;
    }
    const EntityReferenceTable::ErtEntry* ref_entry = 0;
    if (pos_.node()->getGSR()->ers())
        ref_entry = pos_.node()->getGSR()->ert()->lookup(decl_.pointer());
    EntityReferenceStart* ers = 0;
    //! If at least one reference already exists
    if (ref_entry) {
        ers = pos_.node()->getGSR()->insertEntityRef(
            ref_entry->node(0), pos_.before(), pos_.node());
        ers_ = GrovePos(ers->parent(), ers);
        setEntityModified(pos_);
        return;
    }
    NodePtr frag = (EntityDecl::externalGeneralEntity == decl_->declType()) ?
        parseExternalEntity() : parseInternalEntity();
    if (stripInfo_)
        stripInfo_->strip(frag.pointer());
    GroveSectionRoot* gsr  = pos_.node()->getGSR();
    Node* first_child = frag->firstChild();
    if (first_child) {
        Node* last_child = frag->lastChild();
        pos_.insert(frag.pointer());
        ers = new EntityReferenceStart;
        ers->setDecl(decl_.pointer());
        GroveSectionStart::convertToSection(Node::ENTITY_SECTION,
                                            first_child, last_child, ers);
    } 
    else 
        ers = gsr->insertEntityRef(decl_.pointer(), 0,
            pos_.before(), pos_.node());
    ers_ = GrovePos(ers->parent(), ers);
    setEntityModified(pos_);
}

void InsertEntity::doUnexecute()
{
    unsetEntityModified(pos_);
    EntityReferenceStart* ers =
        (static_cast<EntityReferenceStart*>(ers_.before()));
    removedRef_ = ers->takeAsFragment(ers->getSectEnd());
    if (declCreated_)
        entity_decls(pos_)->removeDecl(decl_->name());
}

/////////////////////////////////////////////////////////////////////

ConvertToEntitySelection::ConvertToEntitySelection(
    const GrovePos& from, const GrovePos& to, 
    const ConvertToEntityData& data, bool replicated)
    : SplitSelectionCommand(from, to), 
      data_(data),
      posCmd_(0),
      replicated_(replicated)
{
}

const GrovePos& ConvertToEntitySelection::pos() const
{
    if (posCmd_)
        return posCmd_->pos();
    else
        return GroveMacroCommand::pos();
}

void ConvertToEntitySelection::makeSubcommand(GroveLib::Node* first,
                                              GroveLib::Node* last)
{
    DBG_EXEC(GROVEEDITOR.POS,
             SplitSelectionCommand::makeSubcommand(first, last));
    posCmd_ = add(new ConvertToEntity(first, last, data_,   
                                      first_, replicated_));
}

String ConvertToEntitySelection::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Convert Selection to Entity '%1'");
        case 1:  return posCmd_->info(1); 
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////////

ConvertToEntity::ConvertToEntity(GroveLib::Node* from,
                                 GroveLib::Node* to,
                                 const ConvertToEntityData& data,
                                 const GrovePos& origPos,
                                 bool replicated)
    : firstPos_(from), 
      lastPos_(to),
      data_(data), 
      origPos_(origPos), 
      replicated_(replicated)
{
    suggestedPos_ = GrovePos(from->parent(), from);
}

void ConvertToEntity::doExecute()
{
    EntityDeclSet& decl_set = *entity_decls(firstPos_.pointer());
    const EntityDecl* decl = decl_set.lookupDecl(data_.name);
    if (!replicated_ && decl && firstPos_->getGSR()->ert()->lookup(decl))
        throw GroveEditor::Exception(GroveEditorMessages::entityRedefine);
    InternalEntityDecl* i_decl = 0;
    if (!replicated_) {
        DBG(GROVEEDITOR.TEST) << "ConvertToEntity Execute: from/to:\n";
        DBG_EXEC(GROVEEDITOR.TEST, (firstPos_->dump(), lastPos_->dump()));
        setEntityModified(origPos_);
        oed_ = decl_set.lookupDecl(data_.name);
        ExternalEntityDecl* e_decl = 0;
        if (!data_.isExternalEntity) {
            i_decl = new InternalEntityDecl;
            i_decl->setDeclType(EntityDecl::internalGeneralEntity);
            decl_ = i_decl;
        } 
        else {
            e_decl = new ExternalEntityDecl;
            e_decl->setDeclType(EntityDecl::externalGeneralEntity);
            e_decl->setExternalId(data_.externalId);
            decl_ = e_decl;
        }
        decl_->setDataType(EntityDecl::sgml);
        decl_->setDeclOrigin(EntityDecl::prolog);
        decl_->setEntityName(data_.name);
        decl_->setDeclModified(true);
        decl_->setContentModified(true);
        decl_set.insertDecl(decl_.pointer(), true);
        if (!oed_.isNull() && oed_->entityDeclNode()) {
            decl_->setEntityDeclNode(oed_->entityDeclNode());
            decl_->entityDeclNode()->setDecl(decl_.pointer());
        }
    } 
    else {
        const EntityReferenceTable::ErtEntry* ert =
            firstPos_->getGSR()->ert()->lookup(decl);
        decl_ = ert->node(0)->entityDecl();
    }
    InternalEntityDecl* udi = get_ers(firstPos_.pointer())->
        entityDecl()->asInternalEntityDecl();
    if (udi) {
        udi->rebuildContent(get_current_grove(firstPos_.pointer()));
        udi->setContentModified(true);
    }
    if (0 == savedErs_) 
        savedErs_ = new EntityReferenceStart;
    savedErs_->setDecl(decl_.pointer());
    if (!GroveSectionStart::convertToSection(
            Node::ENTITY_SECTION, firstPos_.pointer(), lastPos_.pointer(),
            savedErs_.pointer(), savedEre_.pointer()))
        throw GroveEditor::Exception(GroveEditorMessages::entityCannotConvert);

    savedEre_ = static_cast<EntityReferenceEnd*>(savedErs_->getSectEnd());
    suggestedPos_ = GrovePos(firstPos_->parent(), firstPos_.pointer());
    if (i_decl && !replicated_) 
        i_decl->rebuildContent(get_current_grove(firstPos_.pointer()));
}

void ConvertToEntity::doUnexecute()
{
    savedErs_->convertFromSection();
    InternalEntityDecl* udi = get_ers(firstPos_.pointer())->
        entityDecl()->asInternalEntityDecl();
    if (udi) {
        udi->rebuildContent(get_current_grove(firstPos_.pointer()));
        udi->setContentModified(false);
    }
    if (!replicated_) {
        EntityDeclSet& decl_set = *entity_decls(firstPos_.pointer());
        if (!oed_.isNull()) {
            if (oed_->entityDeclNode()) {
                oed_->setEntityDeclNode(decl_->entityDeclNode());
                oed_->entityDeclNode()->setDecl(oed_.pointer());
            }
            decl_set.insertDecl(oed_.pointer(), true);
            oed_->setContentModified(true);
        } 
        else 
            decl_set.removeDecl(decl_->name());
    }
    suggestedPos_ = origPos_;
    unsetEntityModified(origPos_);
}

String ConvertToEntity::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Convert to Entity: '%1'");
        case 1:  return data_.name; 
        default: return String::null();
    }
}

//////////////////////////////////////////////////////////////////////

ConvertFromEntity::ConvertFromEntity(const GrovePos& pos)
    : pos_(pos)
{
    EntityReferenceStart* ers = get_ers(pos.contextNode());
    convFromSect_ = new ConvertFromSection(ers);
    suggestedPos_ = pos_;
    entityDecl_   = ers->entityDecl();
}

void ConvertFromEntity::doExecute()
{
    convFromSect_->execute();
    setEntityModified(pos_);
    suggestedPos_ = convFromSect_->pos();
}

void ConvertFromEntity::doUnexecute()
{
    convFromSect_->unexecute();
    EntityDeclSet& decl_set = *entity_decls(pos_);
    if (!decl_set.lookupDecl(entityDecl_->name()))
        decl_set.insertDecl(entityDecl_.pointer());
    suggestedPos_ = pos_;
    unsetEntityModified(pos_);
}

String ConvertFromEntity::info(uint idx) const
{
    switch (idx) {
        case 0:  if (entityDecl_->declType() == EntityDecl::xinclude)
                    return QT_TRANSLATE_NOOP("GroveEditor", 
                        "Convert From XInclude");
                 else
                    return QT_TRANSLATE_NOOP("GroveEditor", 
                        "Convert From Entity");
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////////

ChangeEntityDecl::ChangeEntityDecl(const GrovePos& pos,
                                   const COMMON_NS::String& name,
                                   const COMMON_NS::String& newName,
                                   const COMMON_NS::String& newpath)
    : pos_(pos), oldName_(name),
      newName_(newName), newPath_(newpath),
      was_modified_(false), removed_(false)
{
    decl_ = entity_decls(pos_)->lookupDecl(name);
    if (!decl_.isNull()) {
        const ExternalEntityDecl* e_decl = decl_->asConstExternalEntityDecl();
        if (e_decl)
            oldPath_ = e_decl->sysid();
    }
}

void ChangeEntityDecl::doExecute()
{
    if (decl_.isNull())
        throw GroveEditor::Exception(GroveEditorMessages::entityDeclNotExist);
    Document* document = get_current_grove(pos_.node())->document();
    EntityDeclSet* decl_set = entity_decls(pos_.contextNode());
    setEntityModified(pos_.node());
    const EntityDecl* old_ed = decl_set->lookupDecl(oldName_);
    if (newName_.isEmpty()) { // remove
        if (document->ert()->lookup(old_ed))
            throw GroveEditor::Exception(
                GroveEditorMessages::entityDeclRemove);
        removed_ = true;
        decl_set->removeDecl(oldName_);
        return;
    }
    ExternalEntityDecl* eed = decl_->asExternalEntityDecl();
    was_modified_ = decl_->isDeclModified();
    if (oldName_ != newName_) {
        const EntityReferenceTable::ErtEntry* er =
            document->ert()->lookup(old_ed);
        decl_->setName(newName_);
        decl_->setDeclModified(true);
        if (er) {
            for (uint i = 0; i < er->numOfRefs(); ++i) {
                EntityDecl* pd = static_cast<EntityReferenceStart*>
                    (er->node(i)->getSectParent())->entityDecl();
                er->node(i)->notifyForceModified();
                if (pd->declType() != EntityDecl::internalGeneralEntity)
                    continue;
                affectedDecls_.push_back(new AffectedDecl
                    (pd->name(), pd->isContentModified()));
                pd->setContentModified(true);
                pd->asInternalEntityDecl()->rebuildContent(document->grove());
            }
        }
    }
    if (eed && eed->sysid() != newPath_) {
        eed->setSysid(newPath_);
        decl_->setDeclModified(true);
        decl_->setContentModified(true);
    }
}

void ChangeEntityDecl::doUnexecute()
{
    Document* document = get_current_grove(pos_.node())->document();
    EntityDeclSet* decl_set = entity_decls(pos_.contextNode());
    if (removed_) {
        decl_set->insertDecl(decl_.pointer());
        return;
    }
    if (oldName_ != newName_)
        decl_->setName(oldName_);
    ExternalEntityDecl* eed = decl_->asExternalEntityDecl();
    if (eed && eed->sysid() != oldPath_)
        eed->setSysid(oldPath_);
    decl_->setDeclModified(was_modified_);
    AffectedDecl* d = 0;
    while ((d = affectedDecls_.pop_front())) {
        EntityDecl* ad = decl_set->lookupDecl(d->name);
        if (0 == ad)
            continue;
        ad->setContentModified(d->modified);
        ad->asInternalEntityDecl()->rebuildContent(document->grove());
        delete d;
    }
}

String ChangeEntityDecl::info(uint idx) const
{
    switch (idx) {
        case 0:  if (oldName_ == newName_) 
                    return QT_TRANSLATE_NOOP("GroveEditor", 
                        "Entity Declaration Changed: '%1'");
                 else
                    return QT_TRANSLATE_NOOP("GroveEditor", 
                        "Entity Renamed: '%1' -> '%2'");
        case 1:  return oldName_; 
        case 2:  return (oldName_ != newName_) ? newName_ : String::null();
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////

DeclareEntity::DeclareEntity(GroveLib::Grove* grove,
                             const EntityDecl* ed)
    : grove_(grove), decl_(ed->copy())
{
    decl_->setDeclModified(true);
    decl_->setContentModified(true);
}

DeclareEntity::DeclareEntity(GroveLib::Grove* grove, const String& name)
    : grove_(grove), 
      decl_(new InternalEntityDecl)
{
    decl_->setDeclType(EntityDecl::internalGeneralEntity);
    decl_->setDataType(EntityDecl::sgml);
    decl_->setDeclOrigin(EntityDecl::prolog);
    decl_->setEntityName(name);
    decl_->setDeclModified(true);
    decl_->setContentModified(true);
}

void DeclareEntity::doExecute()
{
    grove_->entityDecls()->insertDecl(decl_.pointer(), true);
}

void DeclareEntity::doUnexecute()
{
    grove_->entityDecls()->removeDecl(decl_->name());
}

String DeclareEntity::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", 
            "Declare Entity: '%1'");
        case 1:  return decl_->name();
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////

EntityDeclSet* entity_decls(const GrovePos& pos)
{
    return get_current_grove(pos.getErs())->entityDecls();
}

Grove* get_current_grove(const Node* n)
{
    const EntityReferenceStart* cur_ers = get_ers(n);
    return cur_ers ? cur_ers->currentGrove() : n->grove();
}

////////////////////////////////////////////////////////////

DeclareXinclude::DeclareXinclude(Node* node, Grove* mainGrove,
                                 const String& url, const String& xpointer, 
                                 const String& encoding, bool isConverted)
    : node_(node),
      grove_(node_->grove()),
      mainGrove_(mainGrove),
      isConverted_(isConverted)
{
    decl_ = new XincludeDecl;
    decl_->setDataType(grove_->topDecl()->dataType());
    decl_->setUrl(url);

    if (!xpointer.isEmpty()) 
        decl_->setOriginalDecl(xpointer);
    if (!encoding.isEmpty())
        decl_->setEncodingString(encoding);

    ers_ = new EntityReferenceStart;
    ere_ = new EntityReferenceEnd;
    ers_->setDecl(decl_.pointer());

    decl_->setReferenceErs(ers_.pointer());
    decl_->setParentGrove(mainGrove_);
}

void DeclareXinclude::doExecute()
{
    //! Append grove to main grove if not already appended
    if (0 == grove_->parent()) {
        mainGrove_->appendChild(grove_.pointer());
        groveInserted_ = true;
    }
    GroveSectionStart::convertToSection(    
        Node::ENTITY_SECTION, node_.pointer(), node_.pointer(),
        ers_.pointer(), ere_.pointer());

    if (isConverted_)
        decl_->setContentModified(true);
}

void DeclareXinclude::doUnexecute()
{
    ers_->convertFromSection();
    //! Remove grove
    if (groveInserted_ && grove_->parent()) {
        grove_->remove();
        groveInserted_ = false;
    }
}

String DeclareXinclude::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Declare XInclude");
        default: return String::null();
    }
}

////////////////////////////////////////////////////////////

static void add_attribute(ElementPtr& elem, const String& name,
                          const String& value)
{
    if (value.isEmpty())
        return;
    AttrPtr a = new Attr(name);
    a->setValue(value);
    elem->attrs().appendChild(a.pointer());
}

InsertXinclude::InsertXinclude(const GrovePos& pos, XincludeDecl* decl,
                               const String& url, const String& xpointer,
                               const String& encoding, 
                               const PropertyNode* attrs)
    : pos_(pos), 
      decl_(decl)
{
    //! Adjust insertion position
    if (pos_.before() && pos_.before()->prevSibling() &&
        pos_.before()->prevSibling()->nodeType() == Node::CHOICE_NODE)
        pos_ = GrovePos(pos_.node(), pos_.before()->prevSibling());
    else 
        if (pos_.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
            pos_ = GrovePos(pos_.node()->parent(), pos_.node()->nextSibling());
    suggestedPos_ = pos_;

    xinclude_elem_ = new Element(NOTR("xi:include"));
    xinclude_elem_->addToPrefixMap(NOTR("xi"), W3C_XINCLUDE_NAMESPACE);

    add_attribute(xinclude_elem_, NOTR("href"), url);
    add_attribute(xinclude_elem_, NOTR("xpointer"), xpointer);
    add_attribute(xinclude_elem_, NOTR("encoding"), encoding);
    if (decl_->dataType() == EntityDecl::cdata)
        add_attribute(xinclude_elem_, NOTR("parse"), NOTR("text"));

    //! Set all other attributes
    if (attrs) {
        for (const PropertyNode* attr = attrs->firstChild(); attr; 
             attr = attr->nextSibling()) 
            add_attribute(xinclude_elem_, attr->name(), attr->getString());
    }
    sourceErs_ = new EntityReferenceStart;
    sourceErs_->setDecl(decl_.pointer());
    sourceEre_ = new EntityReferenceEnd;
}

void InsertXinclude::doExecute()
{
    if (oldSect_) {
        pos_.insert(oldSect_.pointer());
        return;
    }
    // 0. Check if we already have references
    //    to the same decl in the grove.
    const EntityReferenceTable::ErtEntry* ref_entry = pos_.node()->grove()->
        root()->document()->ert()->lookup(decl_.pointer());
    if (ref_entry) {
        sourceErs_ = pos_.node()->getGSR()->insertEntityRef(
            ref_entry->node(0), pos_.before(), pos_.node());
        sourceErs_->appendChild(xinclude_elem_.pointer());
        sourceEre_ = static_cast<EntityReferenceEnd*>
            (sourceErs_->getSectEnd());
        setEntityModified(pos_);
        return;
    }
    // 1. make empty section
    NodePtr node = pos_.insert(new Comment(NOTR("")));
    GroveSectionStart::convertToSection(Node::ENTITY_SECTION, node.pointer(),
                                        node.pointer(), sourceErs_.pointer(), 
                                        sourceEre_.pointer());
    node->remove();
    sourceErs_->appendChild(xinclude_elem_.pointer());

    // 2. get reference content. 
    EntityReferenceStart* reference_ers = decl_->referenceErs();
    DocumentFragmentPtr fragment;
    if (reference_ers && (reference_ers->nextSibling() != 
                          reference_ers->getSectEnd()))
        fragment = reference_ers->nextSibling()->copyAsFragment(
            reference_ers->getSectEnd()->prevSibling());
    else
        fragment = new DocumentFragment;

    // 3. sync with reference ERS
    sourceErs_->ert()->removeEntityRef(sourceErs_.pointer());
    sourceErs_->setDecl(decl_.pointer());
    sourceErs_->ert()->addEntityRef(sourceErs_.pointer());
    if (fragment->firstChild())
        sourceErs_->insertAfter(fragment.pointer());
    else
        sync_with_xinclude_sect(sourceErs_.pointer(), reference_ers);

    setEntityModified(pos_);
    suggestedPos_ = GrovePos(sourceErs_->parent(), sourceErs_.pointer());
}

void InsertXinclude::doUnexecute()
{
    oldSect_ = sourceErs_->takeAsFragment(sourceEre_.pointer());
    unsetEntityModified(pos_);
    suggestedPos_ = pos_;
}

String InsertXinclude::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Insert XInclude");
        default: return String::null();
    }
}

} // namespace GroveEditor

