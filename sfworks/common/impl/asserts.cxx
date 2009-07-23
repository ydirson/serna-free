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
 #include "common/common_defs.h"
#include "common/asserts.h"
#include <iostream>
#include <stdlib.h>


#ifdef _WIN32
extern "C" __declspec(dllimport) void __stdcall DebugBreak(void);
#else  // _WIN32
# include <unistd.h>
# include <signal.h>
#endif // _WIN32

COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

void abort_with_message(const char* file, int line, const char* msg)
{
    std::cerr << "ABORT: file " << file << ", line " << std::dec << line;
    if (msg)
        std::cerr << " (" << msg << ')';
    std::cerr << std::endl;
    abort();
}

void abort_now(const char* msg)
{
    if (msg)
        std::cerr << msg << std::endl;
    abort();
}

void debug_break()
{
#ifdef _WIN32
    DebugBreak();
#else
    kill(getpid(), SIGSTOP);
#endif // _WIN32
}

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END
