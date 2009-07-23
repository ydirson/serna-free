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
# pragma warning ( disable: 4786 4251)
#endif

#include "structeditor/se_defs.h"
#include "structeditor/impl/CommandLink.h"
#include "structeditor/impl/debug_se.h"
#include "common/CommonMessages.h"
#include "common/safecast.h"

using namespace Common;

CommandLink::CommandLink(const CommandPtr& command)
    : command_(command)
{}

CommandLink::~CommandLink()
{
    command_->setAutoUnexecute(false, true);
    if (scmd_)
        scmd_->setAutoUnexecute(false, true);
}

void CommandLink::doExecute()
{
    if (!command_)
        throw Command::Exception(CommonMessages::cmdLinkNoLink);
    if (!command_->isExecuted())
        command_->execute();
    if (scmd_ && !scmd_->isExecuted())
        scmd_->execute();
}

void CommandLink::doUnexecute()
{
    if (!command_)
        throw Command::Exception(CommonMessages::cmdLinkNoLink);
    if (scmd_ && scmd_->isExecuted())
        scmd_->unexecute();
    if (command_->isExecuted())
        command_->unexecute();
}

void CommandLink::setSecondaryCommand(const CommandPtr& p)
{
    scmd_ = p;
}

bool CommandLink::mergeWith(Command* cmd)
{
    const CommandLink* clink = SAFE_CAST(CommandLink*, cmd);
    if (!isMergeable() || !clink->isMergeable())
        return false;
    return command_->mergeWith(clink->command_.pointer());
}
