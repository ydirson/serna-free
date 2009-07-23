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
#include "structeditor/impl/debug_se.h"

#include "common/CommandEvent.h"
#include "common/Url.h"
#include "common/PropertyTreeEventData.h"
#include "utils/GrovePosEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/file_utils.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "grove/SectionSyncher.h"
#include "grove/EntityDeclSet.h"
#include "grove/xinclude.h"

#include "editableview/EditableView.h"

using namespace Common;
using namespace FileUtils;

SIMPLE_COMMAND_EVENT_IMPL(ConvertToInternalEntity, StructEditor)

class ConvertToIntEntityDialog;

bool is_root_writable(StructEditor* se, const GroveEditor::GrovePos& gp)
{
    GroveLib::Grove* grove = GroveEditor::get_current_grove(gp.node());
    GroveEditor::GrovePos root_pos(grove->document()->documentElement());
    return StructEditor::POS_OK == se->isEditableEntity(root_pos,
        StructEditor::STRUCT_OP);
}

bool ConvertToInternalEntity::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos from, to;
    if (StructEditor::POS_OK != se->getSelection(
            from, to, StructEditor::STRUCT_OP|StructEditor::NOROOT_OP) ||
        !is_root_writable(se, from))
            return false;
    PropertyTreeEventData ed;
    if (!makeCommand<ConvertToIntEntityDialog>()->execute(se, &ed))
        return false;
    return se->executeAndUpdate(se->groveEditor()->convertToEntity(from, to,
        ed.root()->getSafeProperty("entityName")->getString()));
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ConvertToExternalEntity, StructEditor)

class ConvertToExtEntityDialog;

bool ConvertToExternalEntity::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos from, to;
    if (StructEditor::POS_OK != se->getSelection(
            from, to, StructEditor::STRUCT_OP|StructEditor::NOROOT_OP) ||
        !is_root_writable(se, from))
            return false;
    PropertyTreeEventData ed;
    if (!makeCommand<ConvertToExtEntityDialog>()->execute(se, &ed))
        return false;
    GroveLib::ExternalId extid;
    String entityName = ed.root()->getSafeProperty("entityName")->getString();
    extid.setSysid(ed.root()->getSafeProperty("entityPath")->getString());
    bool ok = se->executeAndUpdate(se->groveEditor()->convertToEntity(
        from, to, entityName, &extid));
    PropertyNode* mtime =
        se->getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME);
    doTimeStamps(se->grove(), mtime);
    return ok;
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ConvertFromEntity, StructEditor)

bool ConvertFromEntity::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos pos = se->editViewSrcPos();
    if (pos.isNull())
        return false;
    if (pos.node()->isGroveSectionNode() && 
        (pos.node()->nodeType() & GroveLib::Node::SECT_TYPE_MASK) != 
            GroveLib::Node::ENTITY_SECTION)
                return false;
    GroveLib::EntityReferenceStart* ers = pos.getErs();
    GroveLib::EntityReferenceStart* edit_ctx = ers;
    if (edit_ctx->getSectParent())
        edit_ctx = static_cast<GroveLib::EntityReferenceStart*>
            (edit_ctx->getSectParent());
    if (se->isEditableEntity(edit_ctx, StructEditor::STRUCT_OP|
        StructEditor::CFE_OP) != StructEditor::POS_OK)
            return false;
    return se->executeAndUpdate(se->groveEditor()->convertFromEntity(pos));
}

/////////////////////////////////////////////////////////////////////

class EntityBatchCommand : public GroveEditor::GroveBatchCommand {
public:
     virtual String info(uint i) const { return lastChild()->info(i); }
};

static bool insert_entity(StructEditor* se, 
                          const GroveEditor::GrovePos& pos,
                          const PropertyNode* props)
{
    using namespace GroveEditor;
    String entityName = props->getSafeProperty("entityName")->getString();
    String entityPath = props->getSafeProperty("entityPath")->getString();
    Editor& ge = *se->groveEditor();
    const GroveLib::StripInfo* si = se->stripInfo();
    GrovePos from, to;
    if (se->editableView().getSelection().src_.isEmpty() ||
        StructEditor::POS_OK != se->getSelection(from, to,
            StructEditor::STRUCT_OP|StructEditor::SILENT_OP))
                return se->executeAndUpdate(ge.insertEntity(pos,
                    entityName, entityPath, si));
    RefCntPtr<GroveBatchCommand> batch_cmd = new EntityBatchCommand;
    CommandPtr cut_cmd = ge.cut(from, to);
    if (!cut_cmd) {
        se->removeSelection();
        return se->executeAndUpdate(ge.insertEntity(pos, 
            entityName, entityPath, si));
    }
    batch_cmd->executeAndAdd(cut_cmd.pointer());
    se->removeSelection();
    GrovePos src_pos = batch_cmd->firstChild()->asCommandContext()->pos();
    CommandPtr ins_cmd = ge.insertEntity(src_pos, entityName, entityPath, si);
    batch_cmd->executeAndAdd(ins_cmd.pointer());
    return se->executeAndUpdate(batch_cmd.pointer());
}

SIMPLE_COMMAND_EVENT_IMPL(InsertInternalEntity, StructEditor)

class InsertIntEntityDialog;

bool InsertInternalEntity::doExecute(StructEditor* se, EventData*)
{
    GrovePosEventData ed;
    if (!se->getCheckedPos(ed.pos_))
        return false;
    if (!makeCommand<InsertIntEntityDialog>(&ed)->execute(se, &ed))
        return false;
    return insert_entity(se, ed.pos_, ed.root());
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(InsertExternalEntity, StructEditor)

class InsertExtEntityDialog;

bool InsertExternalEntity::doExecute(StructEditor* se, EventData*)
{
    GrovePosEventData ed;
    if (!se->getCheckedPos(ed.pos_))
        return false;
    const GroveLib::EntityReferenceStart* ers = ed.pos_.getErs(); 
    ed.root()->makeDescendant("browseDir", ers->xmlBase());
    if (!makeCommand<InsertExtEntityDialog>(&ed)->execute(se, &ed))
        return false;
    String entityName = ed.root()->getSafeProperty("entityName")->getString();
    // if entity doesn't exist and root document is read-only, disallow
    // insertion (because entity can't be declared in read-only file)
    GroveLib::Grove* grove = GroveEditor::get_current_grove(ed.pos_.node());
    if (!grove->entityDecls()->lookupDecl(entityName) &&
        !is_root_writable(se, ed.pos_))
            return false;
    bool ok = insert_entity(se, ed.pos_, ed.root());
    PropertyNode* mtime =
        se->getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME);
    doTimeStamps(se->grove(), mtime);
    return ok;
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ChangeEntities, StructEditor)

class EntitiesDialog;

bool ChangeEntities::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos pos;
    if (!se->getCheckedPos(pos))
        return false;
    PropertyTreeEventData ed;
    if (!makeCommand<EntitiesDialog>()->execute(se, &ed))
        return false;
    bool ok = se->executeAndUpdate(se->groveEditor()->changeEntityDecl(
        pos,
        ed.root()->getSafeProperty("entityName")->getString(),
        ed.root()->getSafeProperty("newEntityName")->getString(),
        ed.root()->getSafeProperty("entityPath")->getString()));
    PropertyNode* mtime =
        se->getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME);
    doTimeStamps(se->grove(), mtime);
    return ok;
}

