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
#ifndef COMMON_COMMAND_H
#define COMMON_COMMAND_H

#include "common/common_defs.h"
#include "common/RefCntPtr.h"
#include "common/SernaApiRefCounted.h"
#include "common/String.h"
#include "common/MessageException.h"
#include "common/XTreeNode.h"
#include "common/XTreePolicies.h"

namespace GroveEditor {
    // this is not nice, but it helps to avoid lots of dynamic_casts
    class CommandContext;
    class GroveCommand;
    class GroveMacroCommand;
    class GroveReplicatedCommand;
};

namespace Common {

class Message;

class COMMON_EXPIMP Command : public SernaApiRefCounted,
    public XTreeNode<Command, XTreeNodeRefCounted<Command> > {
public:
    Command()
        : isExecuted_(false), autoUnexecute_(true), valid_(true),
          udata1_(0), udata2_(0) {}
    
    virtual ~Command() {}

    void    execute();
    void    unexecute();
    bool    isExecuted() const { return isExecuted_; }

    virtual String info(uint index) const;

    virtual GroveEditor::CommandContext* asCommandContext() { return 0; }
    virtual GroveEditor::GroveCommand* asGroveCommand() { return 0; }
    virtual GroveEditor::GroveMacroCommand*
        asGroveMacroCommand() { return 0; }
    virtual GroveEditor::GroveReplicatedCommand*
        asGroveReplicatedCommand() { return 0; }

    // placeholders for custom user data
    uchar&  udata1() { return udata1_; }
    ushort& udata2() { return udata2_; }
    
    // Those are flags used by MacroCommand and CommandExecutor
    void    setAutoUnexecute(bool v, bool recursive = false);
    bool    autoUnexecuteEnabled() const { return autoUnexecute_; }

    bool    isValid() const { return valid_; }
    void    setValid(bool v) { valid_ = v; }

    // called to merge current command with next one. Should return FALSE if 
    // commands are not mergeable, and leave commands state intact (!).
    virtual bool mergeWith(Command*) { return false; }

    typedef MessageException<Command> Exception;
    static  uint getFacility();

protected:
    virtual void    doExecute() = 0;
    virtual void    doUnexecute() = 0;

private:
    char            isExecuted_ : 1;
    char            autoUnexecute_ : 1;
    char            valid_ : 1;
    uchar           udata1_;
    ushort          udata2_;
};

typedef RefCntPtr<Command> CommandPtr;

} // namespace Common

#endif // COMMON_COMMAND_H
