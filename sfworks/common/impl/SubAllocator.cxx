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

#if defined(_MSC_VER)
# define SUBALLOC_INST
#endif

#include "common/common_defs.h"
#include "common/SubAllocator.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

COMMON_NS_BEGIN

void suballoc_abort(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    abort();
}

template<> class SubAllocator<DEFAULT_THREADING<>, MAX_CHUNK_SIZE, 
                              MAX_SUBALLOC_OBJ_SIZE>;

template<>
class SubAllocator<DEFAULT_THREADING<>, MAX_CHUNK_SIZE, MAX_SUBALLOC_OBJ_SIZE> 
 : public DEFAULT_SUBALLOC_STATS<SubAllocator<DEFAULT_THREADING<>, MAX_CHUNK_SIZE, MAX_SUBALLOC_OBJ_SIZE> > {

    typedef SubAllocator<> allocator;
    friend class DEFAULT_SUBALLOC_STATS<allocator>;
    typedef DEFAULT_SUBALLOC_STATS<allocator> Stats;

    typedef AllocatorSingleton<allocator> singleton;

    struct SALock : public DEFAULT_THREADING_CL<> {};
public:
    COMMON_EXPIMP static unsigned getMaxObjSize()
    {
        return MAX_SUBALLOC_OBJ_SIZE;
    }
    //!
    COMMON_EXPIMP static unsigned getChunkSize()
    {
        return MAX_CHUNK_SIZE;
    }
    //!
    COMMON_EXPIMP static void* allocate(unsigned int sz);
//    {
//        return singleton::instance().do_allocate(sz);
//    }
    //!
    COMMON_EXPIMP static void  deallocate(void* p, unsigned int sz);
//    {
//        return singleton::instance().do_deallocate(p, sz);
//    }
private:
    void* do_allocate(unsigned int sz)
    {
        if (sz < sizeof(void*) || sz > MAX_SUBALLOC_OBJ_SIZE)
            return ::operator new(sz);

        const unsigned int fa_index = sz / MAX_ALIGN - 1;
        if (0 == fa_array[fa_index]) {
            SALock::Lock lk;
            if (0 == fa_array[fa_index]) // double check lock
                fa_array[fa_index] = new uniq_allocator(sz);
        }
        return fa_array[fa_index]->allocate();
    }
    //!

    struct unique {};
    // use unique allocator
    typedef FixedAllocator<unique, DEFAULT_THREADING<>, MAX_CHUNK_SIZE> 
            uniq_allocator;

    void check_dealloc(void* p, unsigned int sz, uniq_allocator* ua)
    {
        if (!ua->is_within(p)) {
            for (unsigned int i = 0; i < FA_ARRAY_SZ; ++i) {
                ua = fa_array[i];
                if (ua && ua->is_within(p))
                    suballoc_abort("Deallocation of '%x' (%d bytes) which "
                                   "belongs to allocator for %d bytes\n",
                                   p, sz, ua->size());
            }
            suballoc_abort("Deallocation of unallocated '%x' (%d bytes)\n",
                           p, sz);
        }
        if (!ua->is_valid(p))
            suballoc_abort("Deallocation of unaligned '%x' (%d bytes)\n",
                           p, sz);
    }
    void  do_deallocate(void* p, unsigned int sz)
    {
        if (0 != p) {
            if (sz < sizeof(void*) || sz > MAX_SUBALLOC_OBJ_SIZE)
                ::operator delete(p);
            else {
                const unsigned int fa_index = sz / MAX_ALIGN - 1;
                if (uniq_allocator* ua = fa_array[fa_index]) {
#if !defined(NO_CHECK_SUBALLOC_DEALLOC)
                    check_dealloc(p, sz, ua);
#endif
                    fa_array[fa_index]->deallocate(p);
                }
                else
                    ::operator delete(p);
            }
        }
    }

    //!
    static const unsigned int FA_ARRAY_SZ = 1 + MAX_SUBALLOC_OBJ_SIZE/MAX_ALIGN;
    static uniq_allocator* fa_array[FA_ARRAY_SZ];
};

SubAllocator<>::uniq_allocator*
SubAllocator<>::fa_array[1 + MAX_SUBALLOC_OBJ_SIZE / MAX_ALIGN];

void* SubAllocator<>::allocate(unsigned int sz)
{
    return singleton::instance().do_allocate(sz);
}
    //!
void SubAllocator<>::deallocate(void* p, unsigned int sz)
{
    return singleton::instance().do_deallocate(p, sz);
}

COMMON_NS_END
