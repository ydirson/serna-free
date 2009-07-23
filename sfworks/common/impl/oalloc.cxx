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
/*! \file
 */

#if defined(_DEBUG) && !defined(NDEBUG)

#include "common/oalloc.h"

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

#define OALLOC_CHECKALLOC  // check individual pointers

// WARNING: code is currently non-reentrable

typedef long mstat_t;

struct AllocStats {
    struct Stats {
        mstat_t n_alloc;
        mstat_t n_dealloc;
        mstat_t n_peakalloc;
        Stats() : n_alloc(0), n_dealloc(0), n_peakalloc(0) {}
    };
    mstat_t tot_alloc;
    mstat_t tot_dealloc;
    mstat_t tot_nballoc;
    mstat_t tot_nbdealloc;
    mstat_t peak_nalloc;
    mstat_t peak_nballoc;

    typedef map<long, Stats> statsMap;
    typedef set<const void*> ptrSet;

    statsMap stats;

#ifdef OALLOC_CHECKALLOC
    ptrSet ptrs;
#endif

    AllocStats()
        : tot_alloc(0), tot_dealloc(0), tot_nballoc(0),
          tot_nbdealloc(0), peak_nalloc(0), peak_nballoc(0) {}
    AllocStats(size_t s)
        : tot_alloc(0), tot_dealloc(0), tot_nballoc(0),
          tot_nbdealloc(0), peak_nalloc(0), peak_nballoc(0)
    {
        (void) stats[s];
    }
};

typedef map<string, AllocStats> AllocSet;
static AllocSet* alloc_set = new AllocSet;

bool   oalloc_doabort = false;
static int oalloc_flags = OALLOC_FLAGS::ENABLED;

static int oalloc_setflags(int flags)
{
    int f = oalloc_flags;
    oalloc_flags = flags;
    return f;
}

static int oalloc_getflags()
{
    return oalloc_flags;
}

inline bool oalloc_enabled()
{
    return 0 != (oalloc_flags & OALLOC_FLAGS::ENABLED);
}

inline void check_abort()
{
    if (oalloc_doabort || (oalloc_flags & OALLOC_FLAGS::ABORT_ON_ERROR))
        abort();
}

static void oalloc_alloc__(const char* id, size_t size, const void* p,
                           bool placement)
{
    if (!(oalloc_enabled() && p))
        return;
    AllocStats* ap;
    AllocSet::iterator it = alloc_set->find(id);
    if (it == alloc_set->end())
        ap = &((*alloc_set)[id] = AllocStats(size));
    else
        ap = &it->second;
#ifdef OALLOC_CHECKALLOC
    ap->ptrs.insert(p);
#endif
    if (placement)
        return;
    ap->tot_alloc++;
    ap->tot_nballoc += size;
    mstat_t diffn = ap->tot_alloc - ap->tot_dealloc;
    if (diffn > ap->peak_nalloc)
        ap->peak_nalloc = diffn;
    mstat_t diffb = ap->tot_nballoc - ap->tot_nbdealloc;
    if (diffb > ap->peak_nballoc)
        ap->peak_nballoc = diffb;
    AllocStats::Stats* sp = &ap->stats[size];
    sp->n_alloc++;
    mstat_t diffp = sp->n_alloc - sp->n_dealloc;
    if (diffp > sp->n_peakalloc)
        sp->n_peakalloc = diffp;
}

static void oalloc_dealloc__(const char* id, size_t size, const void* p)
{
    if (!(oalloc_enabled() && p))
        return;
    AllocSet::iterator it = alloc_set->find(id);
    if (it == alloc_set->end()) {
        cerr << "OALLOC: deallocating unallocated objname "
            << id << ", p = " << p << endl;
        check_abort();
        return;
    }
    AllocStats* ap = &it->second;
    AllocStats::statsMap::iterator si = ap->stats.find(size);
    if (si == ap->stats.end()) {
        cerr << "OALLOC: deallocated instance of " << id
            << " with bad size " << size << ", p = " << p << endl;
        check_abort();
        return;
    }
    ap->tot_dealloc++;
    ap->tot_nbdealloc += size;
    si->second.n_dealloc++;
#ifdef OALLOC_CHECKALLOC
    AllocStats::ptrSet::iterator pi = ap->ptrs.find(p);
    if (pi != ap->ptrs.end() && *pi == p)
        ap->ptrs.erase(pi);
    else {
        cerr << "OALLOC: deallocating unallocated pointer to "
             << id << ", size = " << size << ", p = " << p << endl;
        check_abort();
    }
#endif // OALLOC_CHECKALLOC
}

static const char* fmt1 = "%-24s%-22s%-12s%s";

static void print_hdr()
{
    fprintf(stderr, "Allocation statistics is currently %s\n",
            oalloc_enabled() ? "enabled" : "disabled");
    fprintf(stderr, fmt1, "ClassID", "T-Alloc(N/pN/pKB)",
        "d(N/b)", "Sizes(pNA/dND)\n");
    fprintf(stderr, "-----------------------------------------------------"
        "------------------\n");
}

static void print_oentry(const char* id, const AllocStats* ap)
{
    char tmp1[128], tmp2[128];
    AllocStats::statsMap::const_iterator si;
    sprintf(tmp1, "%ld/%ld/%ld",
        ap->tot_alloc, ap->peak_nalloc, ap->peak_nballoc/1024);
    sprintf(tmp2, "%ld/%ld", ap->tot_alloc - ap->tot_dealloc,
        ap->tot_nballoc - ap->tot_nbdealloc);
    fprintf(stderr, fmt1, id, tmp1, tmp2, "");
    for (si = ap->stats.begin(); si != ap->stats.end(); ++si) {
        fprintf(stderr, "%ld(%ld/%ld) ", (long) si->first,
            si->second.n_peakalloc, si->second.n_alloc - si->second.n_dealloc);
    }
    fprintf(stderr, "\n");
}

static void oalloc_printstats()
{
    print_hdr();
    AllocSet::const_iterator it = alloc_set->begin();
    for (; it != alloc_set->end(); ++it)
        print_oentry(it->first.c_str(), &it->second);
}

void OallocStats<1>::printStats()
{
    oalloc_printstats();
}

void OallocStats<1>::alloc(const char* id, size_t size, const void* p,
                           bool placement)
{
    oalloc_alloc__(id, size, p, placement);
}

void OallocStats<1>::dealloc(const char* id, size_t size, const void* p)
{
    oalloc_dealloc__(id, size, p);
}

int OallocStats<1>::getFlags()
{
    return oalloc_getflags();
}

int OallocStats<1>::setFlags(int flags)
{
    return oalloc_setflags(flags);
}

#endif
