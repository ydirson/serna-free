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
#include "sapi/common/Command.h"
#include "sapi/common/impl/xtn_wrap_impl.h"
#include "common/Command.h"
#include "common/CommandExecutor.h"
#include "common/safecast.h"
#include "utils/tr.h"

#include <QApplication>
#include <QMessageBox>

#define CMD             SAFE_CAST(Common::Command*, getRep())
#define CMD_EXECUTOR    SAFE_CAST(Common::CommandExecutor*, getRep())

#define EXECUTOR_TRY(v) if (!getRep()) return v try {
#define EXECUTOR_CATCH(v) } catch (Common::Exception& exc) { \
    QMessageBox::critical(QApplication::activeWindow(), \
        tr("Command Execution Exception"), exc.what()); \
        return v } return v

namespace {

class CommandWrap : public Common::Command {
public:
    CommandWrap(SernaApi::Command* cmd)
        : cmd_(cmd) {}
    virtual void doExecute() { cmd_->doExecute(); }
    virtual void doUnexecute() { cmd_->doUnexecute(); }

private:
    SernaApi::Command* cmd_;
};

} // namespace

namespace SernaApi {

Command::Command(SernaApiBase* s)
    : RefCountedWrappedObject(s)
{
}

Command::Command()
    : RefCountedWrappedObject(new CommandWrap(this))
{
}

Command::~Command()
{
}

void Command::execute()
{
    EXECUTOR_TRY(;)
    CMD->execute();
    EXECUTOR_CATCH(;)
}

void Command::unexecute()
{
    EXECUTOR_TRY(;)
    CMD->unexecute();
    EXECUTOR_CATCH(;)
}

bool Command::isExecuted() const
{
    if (getRep())
        return CMD->isExecuted();
    return false;
}

SString Command::info(uint i) const
{
    return CMD->info(i);
}

unsigned short Command::getUdata() const
{
    return CMD->udata2();
}

void Command::setUdata(unsigned short v)
{
    CMD->udata2() = v;
}

void Command::doExecute()
{
}

void Command::doUnexecute()
{
}

XTREENODE_WRAP_IMPL(Command, Common::Command)

/////////////////////////////////////////////////////////////////////

CommandExecutor::CommandExecutor(SernaApiBase* s)
    : SimpleWrappedObject(s)
{
}

CommandExecutor::~CommandExecutor()
{
}

Command CommandExecutor::executeAndAdd(const Command& command)
{
    EXECUTOR_TRY(0;)
        return CMD_EXECUTOR->executeAndAdd
            (static_cast<Common::Command*>(command.getRep()));
    EXECUTOR_CATCH(0;)
}

Command CommandExecutor::undo()
{
    if (getRep())
        return CMD_EXECUTOR->undo();
    return 0;
}

Command CommandExecutor::redo()
{
    if (getRep())
        return CMD_EXECUTOR->redo();
    return 0;
}

bool CommandExecutor::isModified() const
{
    if (getRep())
        return CMD_EXECUTOR->isModified();
    return false;
}

Command CommandExecutor::lastDone() const
{
    if (getRep())
        return CMD_EXECUTOR->lastDone();
    return 0;
}

XLIST_WRAP_IMPL2(CommandExecutor, Command, Common::Command)

} // namespace SernaApi
