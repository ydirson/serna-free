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
    SmallObjectAllocator statistics handling implementation
 */
#include "common/common_defs.h"
#include "common/SmallObject.h"
#include "SmallObjStats.h"
#include <stddef.h>
#include <algorithm>
#include <iostream>
#include <utility>


#if (0 != SMALLOBJ_STATS)

COMMON_NS_BEGIN

namespace SmallObjPrivate {

using namespace std;

inline bool
operator<(pair<size_t, AllocatorStat>& p, size_t s)
{
    return p.first < s;
}

inline bool
operator==(pair<size_t, AllocatorStat>& p, size_t s)
{
    return p.first == s;
}

SOA_Stat::~SOA_Stat()
{
    printStats();
    // not deleting stats_ as there might be undestroyed FixedAllocators
    // this is tolerable memleak
}

void
SOA_Stat::printStats()
{
    if (0 != stats_ && 0 != stats_->size()) {
        cerr << "Allocator statistics: " << endl;
        for (Stats::iterator it = stats_->begin(); it != stats_->end(); ++it) {
            AllocatorStat& s = it->second;
            cerr << " Allocator for size " << it->first << endl;
            cerr << "   Number of allocations: " << s.allocs_ << endl;
            cerr << "   Number of deallocations: " << s.deallocs_ << endl;
        }
    }
}

AllocatorStat*
SOA_Stat::registerAllocator(FixedAllocator* allocp)
{
    ++alloc_regs_;
    if (0 == stats_)
        stats_ = new Stats;

    const size_t block_size = allocp->BlockSize();
    Stats::iterator it = lower_bound(stats_->begin(), stats_->end(), block_size);
    if (stats_->end() != it && it->first == block_size) {
        ++(it->second).regs_;
        return &(it->second);
    }
    return &(stats_->insert(it, make_pair(block_size, AllocatorStat())))->second;
}

void
SOA_Stat::unregisterAllocator(FixedAllocator* allocp)
{
    const size_t chunk_size = allocp->BlockSize();
    Stats::iterator it = std::find(stats_->begin(), stats_->end(), chunk_size);
    if (stats_->end() != it) {
        --(it->second).regs_;
        ++alloc_unregs_;
    }
    else {
        cerr << "attempt to unregister unregistered allocator " << allocp;
        cerr << " for size " << chunk_size << endl;
    }
}

SOA_Stat&
SOA_Stat::instance()
{
    static SOA_Stat stat;
    return stat;
}

} // end of namespace SmallObjPrivete

COMMON_NS_END

#endif
