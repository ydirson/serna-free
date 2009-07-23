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

#include "common/CommandExecutor.h"
#include "common/Command.h"

namespace Common {

CommandExecutor::CommandExecutor()
    : notModifiedAt_(0), lastDone_(0), childCount_(0),
      depthLimit_(-1), wasCleared_(false)
{}

CommandExecutor::~CommandExecutor()
{
    clearHistory();
}

void CommandExecutor::clearHistory()
{
    Command* cmd;
    while ((cmd = firstChild())) {
        cmd->setAutoUnexecute(false, true);
        cmd->remove();
    }
    notModifiedAt_ = 0;
    lastDone_ = 0;
    childCount_ = 0;
}

void CommandExecutor::add(Command* command)
{
    RT_ASSERT(!command->parent());
    if (isRedoPossible()) {
        Command* redo_start =
            lastDone_ ? lastDone_->nextSibling() : firstChild();
        while (redo_start) {
            redo_start->setAutoUnexecute(false, true);
            Command* next = redo_start->nextSibling();
            redo_start->remove();
            --childCount_;
            redo_start = next;
        }
    }
    if (depthLimit_ > 0 && childCount_ > depthLimit_) {
        Command* cmd = firstChild();
        if (cmd) {
            cmd->setAutoUnexecute(false, true);
            cmd->remove();
            --childCount_;
            wasCleared_ = true;
        }
    }
    appendChild(command);
    lastDone_ = command;
    ++childCount_;
}

/*! In the function command is likely to throw an exception if
  execution failed. Therefore it will not be added to the command list.
*/
Command* CommandExecutor::executeAndAdd(Command* command)
{
    if (!command->isValid())  // do not attempt to execute invalid commands
        return command;
    command->execute();
    add(command);
    return command;
}

bool CommandExecutor::isUndoPossible() const
{
    return !!lastDone_;
}

Command* CommandExecutor::undo()
{
    Command* undone = 0;
    if (isUndoPossible()) {
        undone = lastDone_;
        lastDone_->unexecute();
        lastDone_ = lastDone_->prevSibling();
    }
    // do nothing if nothing left to undo
    return undone;
}

void CommandExecutor::removeLastCommand()
{
    if (firstChild()) {
        Command* cmd = lastChild();
        cmd->setAutoUnexecute(false, true);
        cmd->remove();
        if (cmd == lastDone_)
            lastDone_ = lastChild();
        if (notModifiedAt_ == lastChild())
            notModifiedAt_ = 0;
        --childCount_;
    }
}

bool CommandExecutor::mergeLastCommand()
{
    if (firstChild() && firstChild()->nextSibling()) {
        if (lastChild()->prevSibling()->mergeWith(lastChild())) {
            removeLastCommand();
            return true;
        }
    }
    return false;
}

bool CommandExecutor::isRedoPossible() const
{
    return lastDone_ != lastChild() && firstChild();
}

Command* CommandExecutor::redo()
{
    if (isRedoPossible()) {
        Command* redone = 0;
        if (isUndoPossible()) {
            redone = lastDone_;
            redone = redone->nextSibling();
        } else
            redone = firstChild();
        redone->execute();
        lastDone_ = redone;
        return redone;
    }
    return 0;
}

void CommandExecutor::getUndoCommandList(Command*& from,
                                         Command*& to) const
{
    if (isUndoPossible()) {
        from = firstChild();
        to   = lastDone_;
    } else
        from = to = 0;
}

void CommandExecutor::getRedoCommandList(Command*& from,
                                         Command*& to) const
{
    if (isUndoPossible()) {
        from = lastDone_->nextSibling();
        to   = lastChild();
    } else {
        from = firstChild();
        to   = lastChild();
    }
}

bool CommandExecutor::isModified() const
{
    // The unmodified state is only when unexecuted lastDone
    // equals to notModifiedAt_
    if (wasCleared_)
        return true;
    if (!notModifiedAt_)
        return !!lastDone_;
    return (notModifiedAt_ != lastDone_);
}

void CommandExecutor::setNotModified()
{
    wasCleared_ = false;
    if (0 == lastDone_)
        notModifiedAt_ = 0;
    else
        notModifiedAt_ = lastDone_;
}

} // namespace Common
