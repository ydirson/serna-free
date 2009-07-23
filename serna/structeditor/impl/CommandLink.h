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
#ifndef STRUCTEDITOR_COMMAND_LINK_H
#define STRUCTEDITOR_COMMAND_LINK_H

#include "common/Command.h"
#include "common/CommandExecutor.h"
#include "common/PackedPtrSet.h"
#include "docview/MessageTree.h"

class CommandLink : public Common::Command {
public:
    typedef COMMON_NS::String CommonString;
    typedef Common::PackedPtrSet<MessageTreeNode> MessageNodeList;

    CommandLink(const Common::CommandPtr& command);
    
    virtual CommonString    info(uint i) const { return command_->info(i); }
    
    Common::Command*        getLink() const { return command_.pointer(); }

    void                    setSecondaryCommand(const Common::CommandPtr&);
    Common::Command*        getSecondaryCommand() const
    {
        return scmd_.pointer();
    }
    MessageNodeList&        prevMessageNodes() { return prevList_; }
    MessageNodeList&        myMessageNodes() { return myList_; }

    virtual bool            mergeWith(Command*);
    bool                    isMergeable() const 
        { return !scmd_ && prevList_.isNull() && myList_.isNull(); }

    virtual ~CommandLink();

protected:
    void                doExecute();
    void                doUnexecute();

private:
    Common::CommandPtr          command_;
    Common::CommandPtr          scmd_;
    MessageNodeList             prevList_;
    MessageNodeList             myList_;
};

typedef COMMON_NS::RefCntPtr<CommandLink> CommandLinkPtr;

#endif // STRUCTEDITOR_COMMAND_LINK_H
