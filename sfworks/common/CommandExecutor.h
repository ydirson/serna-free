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
#ifndef COMMON_COMMAND_EXECUTOR_H
#define COMMON_COMMAND_EXECUTOR_H

#include "common/common_defs.h"
#include "common/RefCntPtr.h"
#include "common/Command.h"
#include "common/XList.h"
#include "common/XTreePolicies.h"

namespace Common {

class COMMON_EXPIMP CommandExecutor : public SernaApiBase,
    public XList<Command, XTreeNodeRefCounted<Command> > {
public:
    CommandExecutor();
    virtual ~CommandExecutor();

    Command*    executeAndAdd(Command* command);

    Command*    undo();
    Command*    redo();
    void        removeLastCommand();
    bool        mergeLastCommand();

    bool        isUndoPossible() const;
    bool        isRedoPossible() const;

    bool        isModified() const;

    void        getUndoCommandList(Command*& from, Command*& to) const;
    void        getRedoCommandList(Command*& from, Command*& to) const;
    void        setNotModified();

    void        clearHistory();
    Command*    lastDone() const { return lastDone_; }
    
    int         childCount() const { return childCount_; }
    void        setDepthLimit(int limit) { depthLimit_ = limit; }
    int         depthLimit() const { return depthLimit_; }

protected:
    void        add(Command* command);

private:
    Command*    notModifiedAt_;
    Command*    lastDone_;
    int         childCount_;
    int         depthLimit_;
    bool        wasCleared_;
};

} // namespace Common

#endif // COMMON_COMMAND_EXECUTOR_H
