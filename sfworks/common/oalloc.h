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
 *   Implementation of DefaultSmallObject - a SmallObject with
 *   default parameters
 */
#ifndef OALLOC_H_
#define OALLOC_H_

#include "common/common_defs.h"
#include <new>

#define STATS_BIT 1

#ifndef OALLOC_TYPE
# ifdef NDEBUG
#  define OALLOC_TYPE 4
# else // NDEBUG
#  define OALLOC_TYPE 5
#endif // NDEBUG
#endif // OALLOC_TYPE

//
// USE_OALLOC macro allows to define custom memory allocation policy
// with or without statistics. Both SmallObjectAllocator and regular
// new/delete can be used with or without statistics. This is determied
// by value of OALLOC_TYPE:
//
//  0 - use regular new/delete, no stats
//  1 - use regular new/delete, with stats
//  2 - use SmallObject, no stats
//  3 - use SmallObject, with stats
//  4 - use SubAllocator, no stats
//  5 - use SubAllocator, with stats
//
//  USE_OALLOC_NORMAL and USE_OALLOC_SMALLOBJ can be used to explicitly
//  specify kind of the allocator (only stats mode will be toggled by
//  OALLOC_TYPE).
//
//  It is supposed that module/class pair will be used as nameid for
//  USE_OALLOC. It is a good idea to define per-module macroses, for
//  example:
//     #define GROVE_OALLOC(className) USE_OALLOC(Grove##className)
//
//  Use of OALLOC in classes is simple:
//
//  class Node {
//      ...
//  public:
//      GROVE_OALLOC(Node);
//  ...
//
//  To print collected statistics, just call oalloc_printstats().
//

enum OallocType {
    OallocDefault   = 0,
    OallocSmall     = 1 << 1,
    OallocSub       = 1 << 2
};

const unsigned DefaultOallocType(OALLOC_TYPE & ~STATS_BIT);
const unsigned DefaultOallocStatsType(OALLOC_TYPE & STATS_BIT);

template <unsigned TYPE = DefaultOallocStatsType> struct OallocStats;
template <unsigned TYPE = DefaultOallocType> struct Oalloc;

template <> struct Oalloc<OallocDefault> {
    static void* operator new(size_t size)
    {
        return ::operator new(size);
    }
    static void operator delete(void* p, size_t)
    {
        ::operator delete(p);
    }
    static void* operator new(size_t, void* p) { return p; }
    static void operator delete(void*, void*) {}
};

#if ((OALLOC_TYPE & ~STATS_BIT) == 2)
# include "common/DefaultSmallObject.h"

template <> struct Oalloc<OallocSmall> {
    static void* operator new(size_t size)
    {
        return COMMON_NS::DefaultSmallObject::operator new(size);
    }
    static void operator delete(void* p, size_t size)
    {
        COMMON_NS::DefaultSmallObject::operator delete(p, size);
    }
    static void* operator new(size_t size, void* p) { return p; }
    static void operator delete(void*, void*) {}
};

#elif ((OALLOC_TYPE & ~STATS_BIT) == 4)
# include "common/SubAllocator.h"

template <> struct Oalloc<OallocSub> {
    static void* operator new(size_t size)
    {
        return COMMON_NS::SubAllocator<>::allocate(size);
    }
    static void operator delete(void* p, size_t size)
    {
        COMMON_NS::SubAllocator<>::deallocate(p, size);
    }
    static void* operator new(size_t, void* p) { return p; }
    static void operator delete(void*, void*) {}
};

#endif

#define USE_OALLOC_STATS(nameid, ALLOCTYPE) \
    static void* operator new(size_t size) { \
        void* p = Oalloc<ALLOCTYPE>::operator new(size); \
        OallocStats<>::alloc(#nameid, size, p); \
        return p; \
    } \
    static void operator delete(void* p, size_t size) { \
        OallocStats<>::dealloc(#nameid, size, p); \
        Oalloc<ALLOCTYPE>::operator delete(p, size); \
    } \
    static void* operator new(size_t size, void* p) { \
        OallocStats<>::alloc(#nameid, size, p, true); \
        return p; \
    } \
    static void operator delete(void*, void*) {}

#define USE_OALLOC_SMALL(nameid) USE_OALLOC_STATS(nameid, OallocSmall)
#define USE_OALLOC_SUB(nameid) USE_OALLOC_STATS(nameid, OallocSub)
// following macro is for compatibility with old oalloc
#define USE_OALLOC_SMALLOBJ(nameid) USE_OALLOC_SUB(nameid)

#if (OALLOC_TYPE & STATS_BIT)
# define USE_OALLOC_NORMAL(nameid) USE_OALLOC_STATS(nameid, OallocDefault)
#else
# define USE_OALLOC_NORMAL(nameid)
#endif

#if (OALLOC_TYPE == 0)   // use regular new/delete
#  define USE_OALLOC(nameid)
# else
#  define USE_OALLOC(nameid) USE_OALLOC_STATS(nameid, DefaultOallocType)
#endif

struct OALLOC_FLAGS {
    enum {
        ABORT_ON_ERROR = 1,
        ENABLED = 1 << 1
    };
};

template <> struct OallocStats<0> {
    static void printStats() {}
    static void alloc(const char*, size_t, const void*, bool = false) {}
    static void dealloc(const char*, size_t, const void*) {}
    static int getFlags() { return 0; }
    static int setFlags(int) { return 0; }
};

template <> struct COMMON_EXPIMP OallocStats<1> {
    static void printStats();
    static void alloc(const char* id, size_t size, const void* p,
                      bool placement = false);
    static void dealloc(const char* id, size_t size, const void* p);
    static int getFlags();
    static int setFlags(int flags);
};

#undef STATS_BIT

#endif // OALLOC_H_
