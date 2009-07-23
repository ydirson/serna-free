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
#ifndef SAPI_GROVE_COMMAND_H_
#define SAPI_GROVE_COMMAND_H_

#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/common/Command.h"

namespace SernaApi {

class SAPI_EXPIMP GroveCommand : public Command {
public:
    GroveCommand(SernaApiBase*);
    virtual ~GroveCommand();

    enum CommandFlags {
        CF_NONE     = 0,
        CF_NOVISUAL = 01,   // command may not have visual effect
        CF_NOSKEL   = 02,   // when validating, don't add elems and choices
        CF_RMATT    = 04,   // remove extra attrs; don't complain about them
        CF_BATCH    = 010   // batch command - validate all subcommands
    };
    /// Returns command flags as above
    virtual CommandFlags flags() const;

    /// Returns validation context for this command (root of the subtree
    /// which should be re-validated after command execution)
    virtual GroveNode validationContext() const;

    /// Returns suggested cursor position after command execution
    virtual GrovePos        pos() const;
};

/// The class for binding several commands into a single command
/*! Batch command allows to combine multiple commands returned by the
 *  GroveEditor so they will look as a single command in the Undo/Redo list.
 *  Use appendChild() function to add commands.
 *  Note that BatchCommands CANNOT be nested.
 */
class SAPI_EXPIMP GroveBatchCommand : public GroveCommand {
public:
    GroveBatchCommand();
    virtual ~GroveBatchCommand();

    /// Returns suggested cursor position after command execution
    virtual GrovePos        pos() const;

    /// Sets command flags as above
    void            setFlags(CommandFlags f);

    /// Returns command flags 
    virtual CommandFlags flags() const;

    /// Executes command and adds to BatchCommand. If command execution
    /// fails, then FALSE is returned.
    bool            executeAndAdd(const Command& command,
                                  bool refuseNull = true);

    /// Explicitly sets new validation context. By default it uses
    /// context from the last child command.
    void            setValidationContext(const GroveNode& node);

    /// Returns validation context for this command (root of the subtree
    /// which should be re-validated after command execution)
    virtual GroveNode validationContext() const;
    
    /// Explicitly sets cursor position after command execution.
    void            setSuggestedPos(const GrovePos& pos);

    /// Set batch command info string
    void            setInfo(const SString& s);
};

} // namespace SernaApi

#endif // SAPI_GROVE_COMMAND_H_

