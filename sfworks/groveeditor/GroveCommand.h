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
#ifndef GROVE_COMMAND_H_
#define GROVE_COMMAND_H_

#include "groveeditor/groveeditor_defs.h"
#include "groveeditor/GrovePos.h"
#include "common/RefCntPtr.h"
#include "common/common_types.h"
#include "common/Command.h"
#include "common/MacroCommand.h"
#include "common/prtti.h"
#include "grove/Decls.h"
#include "grove/Nodes.h"

namespace GroveEditor
{

const GROVEEDITOR_EXPIMP GrovePos& cmd_pos(const Common::Command* cmd);
GROVEEDITOR_EXPIMP void set_unique_ids(GroveLib::Node* node, 
                                        const GroveLib::Node* context);

class GroveCommand;
typedef COMMON_NS::RefCntPtr<GroveCommand>  GroveCommandPtr;
typedef COMMON_NS::String                   CommonString;

class GROVEEDITOR_EXPIMP CommandContext {
public:
    enum CommandFlags {
        CF_NONE     = 0,
        CF_NOVISUAL = 0001, // command may not have visual effect
        CF_NOSKEL   = 0002, // when validating, don't add elems and choices
        CF_RMATT    = 0004, // remove extra attrs; don't complain about them
        CF_BATCH    = 0010, // batch command - validate all subcommands
        CF_OLDPOS   = 0020  // use old position
    };
    enum CommandData {
        CD_IS_ENTITY_MODIFIED = 01,
        CD_IS_TEXT_CHOICE     = 02
    };
    virtual CommandFlags     flags() const { return CF_NONE; }
    virtual const GrovePos&  pos() const = 0;
    virtual GroveLib::Node*  validationContext() const { return 0; }

    void    setEntityModified(const GrovePos& gp);
    void    setEntityModified(GroveLib::Node* n);
    void    unsetEntityModified(const GrovePos& gp);
    void    unsetEntityModified(GroveLib::Node* n);

    void    setTextModified(GroveLib::Text*);
    void    unsetTextModified(GroveLib::Text*);

    virtual GroveEditor::CommandContext* asCommandContext() { return this; }
    PRTTI_DECL_NS(Command, Common);

    virtual ~CommandContext() {}
};

////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP GroveCommand : public Common::Command,
                                        public CommandContext {
public:
    PRTTI_DECL_NS(Command, Common);
    virtual const GrovePos& pos() const { return suggestedPos_; }
    virtual GroveEditor::GroveCommand* asGroveCommand() { return this; }
    virtual GroveEditor::CommandContext* asCommandContext() { return this; }

protected:
    static      bool isTextJoinPossible(const GroveLib::Node* node1,
                                        const GroveLib::Node* node2,
                                        bool  dontCheckAdjacency);
    GrovePos    suggestedPos_;
};

/////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP GroveMacroCommand : public Common::MacroCommand,
                                             public CommandContext {
public:
    PRTTI_DECL_NS(Command, Common);
    virtual GroveEditor::GroveMacroCommand*
        asGroveMacroCommand() { return this; }
    virtual GroveEditor::CommandContext* asCommandContext() { return this; }

    virtual GroveLib::Node*     validationContext() const;
    virtual const GrovePos&     pos() const;
};

class GROVEEDITOR_EXPIMP GroveReplicatedCommand : public GroveMacroCommand {
public:
    virtual const GrovePos&     pos() const;
    virtual CommandFlags        flags() const;
    virtual GroveLib::Node*     validationContext() const;
    virtual bool                mergeWith(Common::Command*);

    virtual GroveEditor::GroveReplicatedCommand*
        asGroveReplicatedCommand() { return this; }
};

class GROVEEDITOR_EXPIMP GroveBatchCommand : public GroveMacroCommand {
public:
    void    executeAndAdd(Command*, bool refuseNull = false); 

    virtual GroveEditor::CommandContext* asCommandContext() { return this; }
    virtual const GrovePos&     pos() const;
    virtual CommandFlags        flags() const { return CF_BATCH; }
    virtual GroveLib::Node*     validationContext() const { return 0; }

    void    setSuggestedPos(const GrovePos& pos) { suggestedPos_ = pos; }
    void    setUndoPos(const GrovePos& pos) { undoPos_ = pos; }

    virtual Common::String info(uint) const;

    void    setInfo(const Common::String& s) { info_ = s; }

private:
    GrovePos        suggestedPos_;
    GrovePos        undoPos_;
    Common::String  info_;
};

class SplitText;
typedef COMMON_NS::RefCntPtr<SplitText> SplitTextPtr;

} // namespace GroveEditor

#endif // GROVE_COMMAND_H_
