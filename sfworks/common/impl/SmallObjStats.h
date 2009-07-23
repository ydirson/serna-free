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
    SmallObjectAllocator statistics handling
 */
#ifndef SMALL_OBJ_STATS_H_
#define SMALL_OBJ_STATS_H_


#include <stddef.h>
#include "common/AssocVector.h"

#if !defined(SMALLOBJ_STATS)
# define SMALLOBJ_STATS 0
#endif

COMMON_NS_BEGIN

namespace SmallObjPrivate {

class AllocatorStat {
        friend class SOA_Stat;
    public:
        size_t registerAllocation()     { return ++allocs_; }
        size_t registerDeallocation()   { return ++deallocs_; }
    private:
        AllocatorStat() : allocs_(0), deallocs_(0), regs_(0) {}
        size_t allocs_;
        size_t deallocs_;
        size_t regs_;       // paranoia
};

class SOA_Stat {
    public:
        AllocatorStat* registerAllocator(FixedAllocator* allocp);
        void unregisterAllocator(FixedAllocator* allocp);
        void printStats();
        static SOA_Stat& instance();
    private:
        friend class dummy; // make g++ happy
        SOA_Stat() : alloc_regs_(0), alloc_unregs_(0), stats_(0) {}
        ~SOA_Stat();

        size_t alloc_regs_;
        size_t alloc_unregs_;

        typedef COMMON_NS::AssocVector<size_t, AllocatorStat> Stats;
        Stats* stats_;
};

} // end of namespace SmallObjPrivate

COMMON_NS_END

#if (0 != SMALLOBJ_STATS)
# define REGISTER_ALLOCATOR(stat, allocp) \
    stat = COMMON_NS::SmallObjPrivate::SOA_Stat::instance().registerAllocator(allocp);
# define UNREGISTER_ALLOCATOR(allocp) \
    COMMON_NS::SmallObjPrivate::SOA_Stat::instance().unregisterAllocator(allocp);
# define REGISTER_ALLOCATION(alloc_stat) alloc_stat->registerAllocation();
# define REGISTER_DEALLOCATION(alloc_stat) alloc_stat->registerDeallocation();
#else
# define REGISTER_ALLOCATOR(stat, allocp)
# define UNREGISTER_ALLOCATOR(allocp)
# define REGISTER_ALLOCATION(alloc_stat)
# define REGISTER_DEALLOCATION(alloc_stat)
#endif // SMALLOBJ_STATS

#endif

