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

#ifndef SUBALLOCATOR_H_
#define SUBALLOCATOR_H_

#include "common/common_defs.h"
#include "common/common_types.h"
#include "common/Singleton.h"
#include "common/ThreadingPolicies.h"
#include "common/SmallStlAllocator.h"
#include <list>

#if defined(_MSC_VER) && !defined(__ICL)
# pragma warning (push)
# pragma warning (disable: 4146) // unary minus applied to unsigned
#endif

#ifndef MAX_SUBALLOC_OBJ_SIZE
# define MAX_SUBALLOC_OBJ_SIZE 256
#endif

#ifndef MAX_CHUNK_SIZE
# define MAX_CHUNK_SIZE 16384U
#endif

COMMON_NS_BEGIN

typedef unsigned int size_type;

COMMON_EXPIMP void suballoc_abort(const char* fmt, ...);

const size_type MAX_ALIGN = sizeof(void*);

static inline size_type align2(size_type sz, size_type pow2)
{
    return (sz + pow2 - 1) & -pow2;
}
//!
static inline size_type align(size_type sz)
{
    if (sz >= MAX_ALIGN)
        return align2(sz, MAX_ALIGN);
    else
        return align2(sz, sizeof(void*));
}

template<class Allocator> struct NoAllocStats {
    void registerAllocation(size_type) {}
    void registerDeallocation(void*, size_type) {}
};

template<class Allocator> struct CheckingAllocStats {
    void registerAllocation(size_type sz) {}
    void registerDeallocation(void* p, size_type sz) {}
};

template<class Allocator> struct AllocStats {
    unsigned allocs_;
    unsigned deallocs_;
    unsigned allocs_peak_;

    void registerAllocation(size_type)
    {
        if (++allocs_ > allocs_peak_)
            allocs_peak_ = allocs_;
    }
    void registerDeallocation(void*, size_type) { deallocs_++; }
    AllocStats() : allocs_(0), deallocs_(0), allocs_peak_(0) {}
};

#if defined(USE_SUBALLOC_STATS)
# if defined(SUBALLOC_STATS)
#  define DEFAULT_SUBALLOC_STATS SUBALLOC_STATS
# else
#  define DEFAULT_SUBALLOC_STATS AllocStats
# endif
#else
# define DEFAULT_SUBALLOC_STATS NoAllocStats
#endif

class Chunk : public DEFAULT_SUBALLOC_STATS<Chunk> {
    typedef DEFAULT_SUBALLOC_STATS<Chunk> Stats;
    using Stats::registerAllocation;
public:
    //!
    Chunk(size_type block_sz, size_type nbytes)
    {
        block_sz = align(block_sz);
        buf_ = static_cast<uchar*>(::operator new(nbytes));
        end_ = buf_ + (nbytes / block_sz) * block_sz;
        high_wm_ = buf_;
    }
    //!
    ~Chunk()
    {
        ::operator delete(buf_);
    }
public:
    void* allocate(size_type block_sz)
    {
        registerAllocation(block_sz);
        void* retval = high_wm_;
        high_wm_ += block_sz;
        return retval;
    }
    bool    isFull() const { return high_wm_ == end_; }
    const uchar*  begin() const { return buf_; }
    const uchar*  end() const { return end_; }
private:
    uchar*  buf_;
    uchar*  end_;
    uchar*  high_wm_;
};

template
<
    class T = void*,
    class Thr = DEFAULT_THREADING<>,
    size_type maxChunkSize = MAX_CHUNK_SIZE
>
class FixedAllocator
 : public Thr,
   public DEFAULT_SUBALLOC_STATS<FixedAllocator<T, Thr, maxChunkSize> > {

    static uchar*& puchar_ref(void* p) { return *reinterpret_cast<uchar**>(p); }

public:
    typedef Chunk chunk_t;
    typedef std::list<chunk_t*> ChunkList;
    typedef typename Thr::Lock LockType;
    //!
    FixedAllocator(size_type block_sz = sizeof(T))
     :  block_size_(align(block_sz)),
        free_block_(0),
        chunks_(1, new chunk_t(block_size_, maxChunkSize))
    {
    }
    //!
    ~FixedAllocator()
    {
        typename ChunkList::iterator it = chunks_.begin();
        for (; it != chunks_.end(); ++it)
            delete *it;
    }
    //!
    void* allocate()
    {
        LockType lkg(*this);
        registerAllocation(size());
        if (0 != free_block_) {
            uchar* r = free_block_;
            free_block_ = puchar_ref(free_block_);
            return r;
        }
        chunk_t* pch = chunks_.back();
        if (pch->isFull()) {
            pch = new chunk_t(block_size_, maxChunkSize);
            chunks_.push_back(pch);
        }
        return pch->allocate(block_size_);
    }
    //!
    bool is_within(void* p)
    {
        return find_chunk(p) != 0;
    }
    bool is_valid(void* p)
    {
        if (Chunk* chunk = find_chunk(p)) {
            const int diff = static_cast<uchar*>(p) - chunk->begin();
            if (diff % block_size_)
                return false;
            return true;
        }
        return false;
    }
    void deallocate(void* p)
    {
        LockType lkg(*this);
        registerDeallocation(p, size());
        puchar_ref(p) = free_block_;
        free_block_ = reinterpret_cast<uchar*>(p);
    }
    //!
    unsigned size() const { return block_size_; }
    //!
    const ChunkList& getChunkList() const { return chunks_; }
private:
    Chunk* find_chunk(void* p)
    {
        const ChunkList& chunks = getChunkList();
        typename ChunkList::const_iterator it = chunks.begin();
        typename ChunkList::const_iterator end = chunks.end();
        for (; it != end; ++it) {
            const uchar* up = static_cast<uchar*>(p);
            if (up >= (*it)->begin() && up < (*it)->end())
                return *it;
        }
        return 0;
    }
    //!
    const size_type block_size_;
    uchar*          free_block_;
    ChunkList       chunks_;
};

template<class A> class AllocatorSingleton
 : public SingletonHolder<A, CreateUsingNew<A>, NoDestroy<A> > {};

template
<
    typename T,
    class Thr = DEFAULT_THREADING<>,
    size_type maxChunkSize = MAX_CHUNK_SIZE
>
class FixedAllocatorAdapter {
    typedef FixedAllocator<T, Thr, maxChunkSize> allocator;
    typedef AllocatorSingleton<allocator> singleton;
public:
    static void* allocate(size_type)
    {
        return singleton::instance().allocate();
    }
    static void deallocate(void* p, size_type)
    {
        return singleton::instance().deallocate(p);
    }
};

template
<
    typename T,
    class Thr = DEFAULT_THREADING_CL<>
>
class FixedStlAllocator
 :  public SmallStlAllocator<T, FixedAllocatorAdapter<T, Thr> >,
    public DEFAULT_SUBALLOC_STATS<FixedStlAllocator<T, Thr> > {

public:
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template<class Other> struct rebind {
        typedef FixedStlAllocator<Other, Thr> other;
    };
    FixedStlAllocator() {}
    template <class T1>
        FixedStlAllocator(const FixedStlAllocator<T1, Thr>&) {}
};

template
<
    class Thr = DEFAULT_THREADING<>,
    size_t chunkSize = MAX_CHUNK_SIZE,
    size_t maxSuballocObjSize = MAX_SUBALLOC_OBJ_SIZE
>
class SubAllocator {
public:
    static unsigned getMaxObjSize()
    {
        return maxSuballocObjSize;
    }
    //!
    static unsigned getChunkSize()
    {
        return chunkSize;
    }
    //!
    static void* allocate(unsigned int sz);
    //!
    static void  deallocate(void* p, unsigned int sz);
};

template<> struct CheckingAllocStats<SubAllocator<> >;

#if defined(_MSC_VER) && !defined(SUBALLOC_INST)
# pragma warning (disable: 4231)
extern template class COMMON_IMPORT SubAllocator<>;
# pragma warning (pop)
#endif

#ifndef USE_SUBALLOCATOR
# define USE_SUBALLOCATOR \
    static void* operator new(size_t size) { \
        return COMMON_NS::SubAllocator<>::allocate(size); \
    } \
    static void operator delete(void* p, size_t size) { \
        COMMON_NS::SubAllocator<>::deallocate(p, size); \
    }
#endif

COMMON_NS_END

#endif // SUBALLOCATOR_H_
