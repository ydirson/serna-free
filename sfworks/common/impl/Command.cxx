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

#include "common/Command.h"
#include "common/String.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/StrdupNew.h"
#include "common/CommonMessages.h"

namespace Common {

uint Command::getFacility() 
{
    return CommonMessages::getFacility();
}

void Command::setAutoUnexecute(bool v, bool recursive)
{
    autoUnexecute_ = v;
    if (recursive)
        for (Command* cmd = firstChild(); cmd; cmd = cmd->nextSibling())
            cmd->setAutoUnexecute(v, true);
}

void Command::execute()
{
    if (isExecuted_)
        throw Command::Exception(CommonMessages::commandExec);
    try {
        doExecute();
    }
    catch (Command::Exception& e) {
        throw;
    }
    catch (std::exception& e) {
        throw Command::Exception(CommonMessages::commandFailed, e.what());
    }
    isExecuted_ = true;
}

void Command::unexecute()
{
    if (!isExecuted_)
        throw Command::Exception(CommonMessages::commandUnExec);
    try {
        doUnexecute();
    }
    catch (Command::Exception& e) {
        throw;
    }
    catch (std::exception& e) {
        throw Command::Exception(CommonMessages::commandFailed, e.what());
    }
    isExecuted_ = false;
}

String Command::info(uint i) const
{
    return i ? String::null() : String("<Unknown command>");
}

} // namespace Common
