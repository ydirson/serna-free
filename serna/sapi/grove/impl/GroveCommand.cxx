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
#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveCommand.h"
#include "groveeditor/GroveCommand.h"
#include "grove/Node.h"
#include "common/safecast.h"

#define NODE_IMPL(n) SAFE_CAST(GroveLib::Node*, n.getRep())
#define SELF         SAFE_CAST(BatchCommandWrap*, getRep())
#define SELF_CTX SAFE_CAST(Common::Command*, getRep())->asCommandContext()

namespace {

class BatchCommandWrap : public GroveEditor::GroveBatchCommand {
public:
    BatchCommandWrap()
        : flags_(CF_BATCH) {}

    virtual CommandFlags    flags() const
    {
        return (CommandFlags) (CF_BATCH | flags_);
    }
    virtual GroveLib::Node* validationContext() const
    {
        return validationContext_;
    }
private:
    friend class SernaApi::GroveBatchCommand;

    CommandFlags        flags_;
    GroveLib::Node*     validationContext_;
};

} // namespace

namespace SernaApi {

GroveBatchCommand::GroveBatchCommand()
    : GroveCommand(new BatchCommandWrap)
{
}

GroveBatchCommand::~GroveBatchCommand()
{
}

bool GroveBatchCommand::executeAndAdd(const Command& command, bool refuseNull)
{
    if (!getRep())
        return false;
    if (command.isNull()) {
        if (refuseNull) {
            SELF->setValid(false);
            SELF->setInfo("Attempt to add null command to BatchCommand");
            return false;
        }
        return true;
    }
    if (!SELF->isValid())
        return false;
    try {
        SELF->executeAndAdd(static_cast<Common::Command*>
            (command.getRep()), true);
    } 
    catch (Common::Command::Exception& exc) {
        SELF->setInfo(exc.whatString());
        SELF->setValid(false);
        return false;
    }
    return true;
}

GroveBatchCommand::CommandFlags GroveBatchCommand::flags() const
{
    if (getRep())
        return (CommandFlags) SELF->flags();
    return (CommandFlags) 0;
}

void GroveBatchCommand::setFlags(CommandFlags f)
{
    if (getRep())
        SELF->flags_ = (GroveEditor::GroveCommand::CommandFlags) f;
}

GroveNode GroveBatchCommand::validationContext() const
{
    return getRep() ? SELF->validationContext() : 0;
}

void GroveBatchCommand::setValidationContext(const GroveNode& node)
{
    if (getRep())
        SELF->validationContext_ = NODE_IMPL(node);
}

GrovePos GroveBatchCommand::pos() const
{
    return SELF->pos();
}

void GroveBatchCommand::setSuggestedPos(const GrovePos& pos)
{
    SELF->setSuggestedPos(pos);
}

void GroveBatchCommand::setInfo(const SString& s)
{
    SELF->setInfo(s);
}

/////////////////////////////////////////////////////////////////

GroveCommand::GroveCommand(SernaApiBase* rep)
    : Command(rep)
{
}

GroveCommand::CommandFlags GroveCommand::flags() const
{
    return getRep() ? (CommandFlags)SELF_CTX->flags() : CF_NONE;
}

GroveNode GroveCommand::validationContext() const
{
    return getRep() ? SELF_CTX->validationContext() : GroveNode();
}

GrovePos GroveCommand::pos() const
{
    return getRep() ? GrovePos(SELF_CTX->pos()) : GrovePos();
}

GroveCommand Command::asGroveCommand() const
{
    if (!getRep())
        return 0;
    ::GroveEditor::CommandContext* ctx = SELF_CTX;
    return ctx ? GroveCommand(getRep()) : GroveCommand(0);
}

GroveCommand::~GroveCommand()
{
}

} // namespace SernaApi

