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
#ifndef SAPI_COMMON_COMMAND_H_
#define SAPI_COMMON_COMMAND_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/SString.h"
#include "sapi/common/xtn_wrap.h"

namespace SernaApi {

class GroveCommand;

/// The base class for the actions performed on the document
class SAPI_EXPIMP Command : public RefCountedWrappedObject {
public:

    /// If constructed w/o args, then custom Command is created
    /// (user must redefine doExecute()/doUnexecute())
    Command();
    Command(SernaApiBase*);

    virtual ~Command();

    /// Execute the command
    void            execute();

    /// Un-execute (undo) the command
    void            unexecute();

    /// Returns TRUE if command is already executed
    bool            isExecuted() const;

    /// Additional command info, as seen in undo/redo list. For null value
    /// of argIndex the info string itself is returned; non-zero values
    /// are used to access optional arguments. When there is no argument
    /// for any given index, null string is returned.
    SString         info(unsigned int argIndex) const;

    /// Get custom user data on the command
    unsigned short  getUdata() const;

    /// Set custom user data on the command
    void            setUdata(unsigned short);

    /// If the current command was created by the GroveEditor,
    /// it may be of GroveCommand type. GroveCommands contain some 
    /// additional information such as current position, etc.
    GroveCommand    asGroveCommand() const;

    XTREENODE_WRAP_DECL(Command)

    /// Allows to redefine behaviour of original command. Used only
    /// when Command was constructed w/o arguments.
    /// THESE FUNCTIONS SHOULD NEVER BE CALLED DIRECTLY. USE execute()
    /// AND unexecute() INSTEAD.
    virtual void    doExecute();

    /// Allows to redefine behaviour of original command. Used only
    /// when Command was constructed w/o arguments.
    /// THESE FUNCTIONS SHOULD NEVER BE CALLED DIRECTLY. USE execute()
    /// AND unexecute() INSTEAD.
    virtual void    doUnexecute();
};

/// The base class for the document Command exector
class SAPI_EXPIMP CommandExecutor : public SimpleWrappedObject {
public:
    CommandExecutor(SernaApiBase* = 0);
    virtual ~CommandExecutor();

    /// Executes command and adds it to undo/redo stack
    Command     executeAndAdd(const Command& command);

    /// Undo (unexecute) the command
    Command     undo();

    /// Redo (re-execute) the command
    Command     redo();

    /// Checks if there are any executed/added commands
    bool        isModified() const;

    /// Returns last executed command
    Command     lastDone() const;

    XLIST_WRAP_DECL(Command)
};

} // namespace SernaApi

#endif // SAPI_COMMON_COMMAND_H
