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
#ifndef _COMMON_TIMING_PROFILE_H_
#define _COMMON_TIMING_PROFILE_H_

#include "common/common_defs.h"
#include "common/common_types.h"

namespace Common {

typedef ulong TTIME; // time in microseconds; 70 mins max (do not need more)

struct TimingProfilerStats {
    const char* name;           // entry name
    ulong       n_calls;        // number of calls
    TTIME       last_call_time; // time spent in last call
    TTIME       total_time;     // total time spent

    TimingProfilerStats()
        : name(0), n_calls(0), last_call_time(0), total_time(0) {}
};

class TimingProfiler {
public:
    virtual const TimingProfilerStats* getStats(const char*) const = 0;
    virtual void print(const char*, bool lastOnly = false) const = 0;
    virtual void clearStats() = 0;
    virtual ~TimingProfiler() {}
    
private:
    friend class TimingProfilerGuard;
    virtual TimingProfilerStats* startCall(const char*) = 0;
    virtual void endCall(TimingProfilerStats*) = 0;
};

COMMON_EXPIMP TimingProfiler& timing_profiler();

class TimingProfilerGuard {
public:
    TimingProfilerGuard(const char* name)
    {
        stat_ = timing_profiler().startCall(name);
    }
    ~TimingProfilerGuard()
    {
        timing_profiler().endCall(stat_);
    }
private:
    TimingProfilerStats* stat_;
};

} // namespace

// Usage:
//  { TPROF_GUARD(xx); my_calls(); } - counts time spent in scope
//  TPROF_CALL(xx, my_func())        - counts function call time
//  TPROF_PRINT(xx)                  - prints result for given prefix or name.
//
// Note that name (xx in this example) must not contain characters which are 
// not allowed in C++ varitable names.

#if defined(_NDEBUG) && !defined(FORCE_TPROF)
# define TPROF_GUARD(name)
# define TPROF_CALL(name, x) x;
# define TPROF_PRINT(prefix) 
# define TPROF_PRINTALL     
# define TPROF_PRINTLAST
# define TPROF_CLEAR
# define TPROF_DBG_CALL(tag, name, x) 
# define TPROF_DBG_PRINT(tag, prefix)
# define TPROF_DBG_PRINTALL(tag)
# else // _NDEBUG
# define TPROF_GUARD(name)   TimingProfilerGuard tpguard_##name(#name)
# define TPROF_CALL(name, x) { TPROF_GUARD(name); x; }
# define TPROF_PRINT(prefix) Common::timing_profiler().print(#prefix)
# define TPROF_PRINTALL      Common::timing_profiler().print(0)
# define TPROF_PRINTLAST     Common::timing_profiler().print(0, true)
# define TPROF_CLEAR        Common::timing_profiler().clearStats()
# define TPROF_DBG_PRINT(tag, prefix) DBG_IF(tag) TPROF_PRINT(prefix);
# define TPROF_DBG_PRINTALL(tag) DBG_IF(tag) TPROF_PRINTALL
#endif // _NDEBUG

#endif // _COMMON_TIMING_PROFILE_H_

