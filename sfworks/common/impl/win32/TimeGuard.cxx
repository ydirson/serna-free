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

#include "common/TimeGuard.h"
#include "common/common_types.h"
#include "common/winlean.h"
#include "common_debug.h"
#include <windows.h>

#ifdef DBG_DEFAULT_TAG
# undef DBG_DEFAULT_TAG
#endif
#define DBG_DEFAULT_TAG COMMON.TMGUARD

COMMON_NS_BEGIN

#ifdef _MSC_VER
// conversion from 'type1' to 'type2', possible loss of data
# pragma warning (disable : 4244)
#endif // _MSC_VER

static inline int64
operator-(const LARGE_INTEGER& i1, const LARGE_INTEGER& i2)
{
    return i1.QuadPart - i2.QuadPart;
}

static inline int64 operator/(const int64 i1, const LARGE_INTEGER& i2)
{
    return i1 / i2.QuadPart;
}

static LARGE_INTEGER freq;

static inline int64
get_seconds(const LARGE_INTEGER& start, const LARGE_INTEGER& stop)
{
    return (stop - start) / freq;
}

static inline int64
get_useconds(const LARGE_INTEGER& start, const LARGE_INTEGER& stop)
{
    return 1000000UL * get_seconds(start, stop);
}

TimeGuard::TimeGuard(const char* id) : id_(id)
{
    QueryPerformanceFrequency(&freq);
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    tv_.sec_ = counter.LowPart;
    tv_.usec_ = counter.HighPart;
}

TimeGuard::~TimeGuard()
{
    if (0 != id_)
        DDBG << id_;
    LARGE_INTEGER start = { tv_.sec_, tv_.usec_ };
    LARGE_INTEGER stop;
    QueryPerformanceCounter(&stop);
    uint64 usecs(get_useconds(start, stop));
    DDBG << " T: high" << rbr(unsigned(usecs >> 32)) << ", low"
         << rbr(unsigned(usecs & 0xffffffff)) << std::endl;
}

COMMON_NS_END
