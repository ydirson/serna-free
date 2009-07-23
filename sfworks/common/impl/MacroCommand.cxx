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
# pragma warning ( disable: 4251)
#endif

#include "common/MacroCommand.h"
#include "common/asserts.h"
#include "common/CommonMessages.h"

#include <iostream>

namespace Common {

void MacroCommand::executeAndAdd(Command* command)
{
    if (!command) {
        doUnexecute();
        throw Exception(CommonMessages::cmdAddNull);
    }
    try {
        command->execute();
        RT_ASSERT(!command->parent());
        appendChild(command);
    }
    catch (Command::Exception& e) {
        doUnexecute();
        throw;
    }
    catch (std::exception& e) {
        doUnexecute();
        throw Exception(CommonMessages::commandFailed, e.what());
    }
}

void MacroCommand::doExecute()
{
    if (!firstChild()) {
        try {
            buildSubCommands();
        }
        catch (Command::Exception& e) {
            doUnexecute();
            throw;
        }
        catch (std::exception& e) {
            doUnexecute();
            throw Exception(CommonMessages::commandFailed, e.what());
        }
        if (!firstChild())
            throw Exception(CommonMessages::macroCmdSub);
    }
    else {
        for (Command* cmd = firstChild(); cmd; cmd = cmd->nextSibling())
            if (!cmd->isExecuted())
                cmd->execute();
    }
}

MacroCommand::~MacroCommand()
{
    if (autoUnexecuteEnabled() && firstChild())
        doUnexecute();
}

void MacroCommand::doUnexecute()
{
    for (Command* cmd = lastChild(); cmd; cmd = cmd->prevSibling()) {
        if (!cmd->isExecuted())
            continue;
        try {
            cmd->unexecute();
        } catch (std::exception& e) { // if cannot unexecute, don't do it again
            setAutoUnexecute(false);
            throw;
        }
    }
}

String MacroCommand::info(uint idx) const
{
    return contextCommand()->info(idx);
}

const Command* MacroCommand::contextCommand() const
{
    if (contextCommand_)
        return contextCommand_;
    return firstChild();
}

void MacroCommand::setContextCommand(const Command* cmd)
{
    contextCommand_ = cmd;
}

} // namespace Common
