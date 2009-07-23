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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "common/common_defs.h"
#include "common/common_opt.h"
#include "common/oalloc.h"
#include "common/Debug.h"
#include <string.h>
#include <algorithm>
#include <functional>


COMMON_NS_BEGIN

//! returns pointer to arg (if any) of option 'what'.
//! @param param indicates whether such an arg should be looked for
//! @param arg must not begin with '-'
//! effects: decreases argc accordingly, moves 'what' and its arg (if any)
//! so that argv[argc] points to 'what' argv[argc+1] points to arg
static const char* find_arg(int& argc, char** begin,
                            const char* what, bool param = false)
{
    using namespace std;
    char** end = begin + argc;
    char** arg = find_if(begin, end, not1(bind1st(ptr_fun(&strcmp), what)));
    if (arg == end)
        return 0;
    char* rv = *arg;
    if (param) {
        rv = *(arg + 1);
        if (rv && rv[0] != '-') {
            argc = remove(arg + 1, end, rv) - begin;
            end = begin + argc;
        }
        else
            rv = 0;
    }
    argc = remove(arg, end, *arg) - begin;
    return rv;
}

int process_oalloc_options(int argc, char* argv[])
{
    int oflags = 0;
    oflags = OallocStats<>::getFlags();
    if (0 != find_arg(argc, argv, "-oa"))
        oflags |= OALLOC_FLAGS::ABORT_ON_ERROR;
    if (0 != find_arg(argc, argv, "-od"))
        oflags &= ~OALLOC_FLAGS::ENABLED;
    if (0 != find_arg(argc, argv, "-op")) {
        OallocStats<>::printStats();
        atexit(&OallocStats<>::printStats);
    }
    OallocStats<>::setFlags(oflags);
    return argc;
}

int process_debug_options(int argc, char* argv[])
{
    const char* dbg_flags = find_arg(argc, argv, "-de", true);
    if (dbg_flags)
        DBG_SET_TAGS(dbg_flags, true);
    dbg_flags = find_arg(argc, argv, "-dd", true);
    if (dbg_flags)
        DBG_SET_TAGS(dbg_flags, false);
    return argc;
}

int process_common_options(int argc, char* argv[])
{
    argc = process_oalloc_options(argc, argv);
    return process_debug_options(argc, argv);
}

COMMON_NS_END

