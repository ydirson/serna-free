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
#include "common/timing_profiler.h"
#include "common/Singleton.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>
#include <string>

#ifdef _WIN32
# include <windows.h>
#else // _WIN32
# include <sys/time.h>
#endif // _WIN32

namespace {

typedef unsigned long long LONG_TIME;

struct TimingProfilerStatsImpl : public Common::TimingProfilerStats {
    LONG_TIME call_start_time;
    
    TimingProfilerStatsImpl()
        : call_start_time(0) {}
};
    
inline LONG_TIME get_time()
{
#ifdef _WIN32
    static LARGE_INTEGER freq = { 0 };
     if (0 == freq.QuadPart) {
        QueryPerformanceFrequency(&freq);
        freq.QuadPart /= 1000000;
        if (0 == freq.QuadPart)
            freq.QuadPart = 1;
    }
    LARGE_INTEGER tv;
    QueryPerformanceCounter(&tv);
    return tv.QuadPart / freq.QuadPart;
#else // _WIN32
    struct timeval tv;
    (void) gettimeofday(&tv, 0);
    return LONG_TIME(tv.tv_sec) * 1000000 + tv.tv_usec;
#endif // _WIN32
}

class TimingProfilerImpl : public Common::TimingProfiler {
    typedef std::map<std::string, TimingProfilerStatsImpl*> StatsMap;
public:
    virtual Common::TimingProfilerStats* startCall(const char* name)
    {
        TimingProfilerStatsImpl* s = get_entry(name);
        s->last_call_time  = 0;
        s->call_start_time = get_time();
        ++s->n_calls;
        return s;
    }
    virtual void endCall(Common::TimingProfilerStats* stats)
    {
        TimingProfilerStatsImpl* s = 
            static_cast<TimingProfilerStatsImpl*>(stats);
        s->last_call_time = get_time() - s->call_start_time;
        s->total_time += s->last_call_time;
    }
    virtual const Common::TimingProfilerStats* getStats(const char* name) const
    {
        return get_entry(name);
    }
    virtual void print(const char* prefix, bool last) const
    {
        int pref_len = prefix ? strlen(prefix) : 0;
        StatsMap::const_iterator it = stats_.begin();
        if (last)
            std::cerr << "TPROF: ";
        for (; it != stats_.end(); ++it) {
            if (pref_len && strncmp(it->first.c_str(), prefix, pref_len))
                continue;
            TimingProfilerStatsImpl& s = *it->second;
            if (last)
                std::cerr << it->first << ":" << s.last_call_time << " ";
            else
                std::cerr << "TPROF<" << it->first << ">:\tlastcall=" 
                    << s.last_call_time << "u, ncalls=" << s.n_calls
                    << ", total=" << s.total_time << "u" << std::endl;
            s.last_call_time = 0;
        }
        if (last)
            std::cerr << std::endl;
    }
    virtual void clearStats()
    {
        StatsMap::iterator it = stats_.begin();
        for (; it != stats_.end(); ++it)
            delete it->second;
        stats_.clear();
    }
    virtual ~TimingProfilerImpl()
    {
        clearStats();
    }
private:
    TimingProfilerStatsImpl* get_entry(const char* name) const
    {
        StatsMap::iterator it = stats_.find(name);
        if (it != stats_.end())
            return it->second;
        TimingProfilerStatsImpl* s = new TimingProfilerStatsImpl;    
        it = stats_.insert(StatsMap::value_type(name, s)).first;
        s->name = it->first.c_str();
        return s;
    }
    mutable StatsMap stats_;
};

} // namespace

COMMON_EXPIMP Common::TimingProfiler& Common::timing_profiler()
{
    return Common::SingletonHolder<TimingProfilerImpl>::instance();
}
