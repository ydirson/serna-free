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
    A simple small object allocator implementation. Code mostly derived
    from the Loki library.
 */
#ifndef SMALL_OBJ_H_
#define SMALL_OBJ_H_

#include "common/common_defs.h"
#include "common/ThreadingPolicies.h"
#include "common/Singleton.h"
#include "common/Vector.h"
#include <vector>
#include <stdexcept>


COMMON_NS_BEGIN

#ifndef DEFAULT_CHUNK_SIZE
# define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
# define MAX_SMALL_OBJECT_SIZE 64
#endif

namespace SmallObjPrivate {
    /*! Offers services for allocating fixed-sized objects
     */
    class AllocatorStat;
    class FixedAllocator {
    private:
        struct Chunk {
            void Init(size_t blockSize, unsigned char blocks);
            void* Allocate(size_t blockSize);
            void Deallocate(void* p, size_t blockSize);
            void Reset(size_t blockSize, unsigned char blocks);
            void Release();
            unsigned char* pData_;
            unsigned char
                firstAvailableBlock_,
                blocksAvailable_;
        };

        // Internal functions
        void DoDeallocate(void* p);
        Chunk* VicinityFind(void* p);

        // Data
        size_t blockSize_;
        unsigned char numBlocks_;
        typedef Vector<Chunk> Chunks;
        Chunks chunks_;
        Chunk* allocChunk_;
        Chunk* deallocChunk_;
        // For ensuring proper copy semantics
        mutable const FixedAllocator* prev_;
        mutable const FixedAllocator* next_;
        AllocatorStat* stat_;

    public:
        // Create a FixedAllocator able to manage blocks of 'blockSize' size
        explicit FixedAllocator(size_t blockSize = 0);
        FixedAllocator(const FixedAllocator&);
        FixedAllocator& operator=(const FixedAllocator&);
        ~FixedAllocator();

        void Swap(FixedAllocator& rhs);

        // Allocate a memory block
        void* Allocate();

        // Deallocate a memory block previously allocated with Allocate()
        // (if that's not the case, the behavior is undefined)
        void Deallocate(void* p);

        // Returns the block size with which the FixedAllocator was initialized
        size_t BlockSize() const { return blockSize_; }

        // Comparison operator for sorting
        bool operator<(size_t rhs) const { return BlockSize() < rhs; }
    };

    /*! Offers services for allocating small-sized objects
     */
    class SmallObjAllocator {
    public:
        SmallObjAllocator(size_t chunkSize, size_t maxObjectSize);

        void* Allocate(size_t numBytes);
        void Deallocate(void* p, size_t size);

    private:
        SmallObjAllocator(const SmallObjAllocator&);
        SmallObjAllocator& operator=(const SmallObjAllocator&);

        typedef std::vector<FixedAllocator> Pool;
        Pool pool_;
        FixedAllocator* pLastAlloc_;
        FixedAllocator* pLastDealloc_;
        size_t chunkSize_;
        size_t maxObjectSize_;
    };
} // namespace SmallObjPrivate

/*! Base class for small objects.
 */
template
<
    class ThreadingModel = DEFAULT_THREADING_CL<>,
    size_t chunkSize = DEFAULT_CHUNK_SIZE,
    size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE
>
class SmallObject : public ThreadingModel {
private:
    struct MySmallObjAllocator : public SmallObjPrivate::SmallObjAllocator
    {
        MySmallObjAllocator()
          : SmallObjAllocator(chunkSize, maxSmallObjectSize) {}
    };
public:
    typedef SingletonHolder<MySmallObjAllocator,
        CreateUsingNew<MySmallObjAllocator>,
        NoDestroy<MySmallObjAllocator> > AllocatorSingleton;

    typedef typename ThreadingModel::Lock Locktype;

    static void* operator new(size_t size)
    {
        using namespace SmallObjPrivate;
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
        Locktype lock;

        return AllocatorSingleton::instance().Allocate(size);
#else
        return ::operator new(size);
#endif
    }
    static void operator delete(void* p, size_t size)
    {
        using namespace SmallObjPrivate;
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
        Locktype lock;

        AllocatorSingleton::instance().Deallocate(p, size);
#else
        ::operator delete(p, size);
#endif
    }
};

COMMON_NS_END

#include "common/DefaultSmallObject.h"

#endif // SMALLOBJ_H_
