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
    A simple small object allocator implementation.
    Code mostly derived from the Loki library.
 */
#include <iostream>

#include "common/common_defs.h"
#include "common/SmallObject.h"
#include "common/AssocVector.h"
#include "SmallObjStats.h"
#include <cassert>
#include <algorithm>
#include <limits.h>


#define ASSERTS_ON

#ifdef ASSERTS_ON
#define DEBUG_ASSERT(x) assert(x);
#else
#define DEBUG_ASSERT(x)
#endif

COMMON_NS_BEGIN

namespace SmallObjPrivate
{
/*! Initializes a chunk object
 */
void FixedAllocator::Chunk::Init(size_t blockSize, unsigned char blocks)
{
    DEBUG_ASSERT(blockSize > 0);
    DEBUG_ASSERT(blocks > 0);
    // Overflow check
    DEBUG_ASSERT((blockSize * blocks) / blockSize == blocks);

    pData_ = new unsigned char[blockSize * blocks];
    Reset(blockSize, blocks);
}

/*! Clears an already allocated chunk
 */
void FixedAllocator::Chunk::Reset(size_t blockSize, unsigned char blocks)
{
    DEBUG_ASSERT(blockSize > 0);
    DEBUG_ASSERT(blocks > 0);
    // Overflow check
    DEBUG_ASSERT((blockSize * blocks) / blockSize == blocks);

    firstAvailableBlock_ = 0;
    blocksAvailable_ = blocks;

    unsigned char i = 0;
    unsigned char* p = pData_;
    for (; i != blocks; p += blockSize)
        *p = ++i;
}

/*! Releases the data managed by a chunk
 */
void FixedAllocator::Chunk::Release()
{
    delete[] pData_;
}

/*! Allocates a block from a chunk
 */
void* FixedAllocator::Chunk::Allocate(size_t blockSize)
{
    if (!blocksAvailable_)
        return 0;

    DEBUG_ASSERT((firstAvailableBlock_ * blockSize) / blockSize ==
        firstAvailableBlock_);

    unsigned char* pResult =
        pData_ + (firstAvailableBlock_ * blockSize);
    firstAvailableBlock_ = *pResult;
    --blocksAvailable_;

    return pResult;
}

/*! Dellocates a block from a chunk
 */
void FixedAllocator::Chunk::Deallocate(void* p, size_t blockSize)
{
    DEBUG_ASSERT(p >= pData_);

    unsigned char* toRelease = static_cast<unsigned char*>(p);
    // Alignment check
    DEBUG_ASSERT((toRelease - pData_) % blockSize == 0);

    *toRelease = firstAvailableBlock_;
    firstAvailableBlock_ =
        static_cast<unsigned char>((toRelease - pData_) / blockSize);

    // Truncation check
    DEBUG_ASSERT(firstAvailableBlock_ == (toRelease - pData_) / blockSize);

    ++blocksAvailable_;
}

/*! Creates a FixedAllocator object of a fixed block size
 */
FixedAllocator::FixedAllocator(size_t blockSize)
    : blockSize_(blockSize), allocChunk_(0), deallocChunk_(0), stat_(0)
{
    DEBUG_ASSERT(blockSize_ > 0);

    prev_ = next_ = this;

    size_t numBlocks = DEFAULT_CHUNK_SIZE / blockSize;

    if (numBlocks > UCHAR_MAX)
        numBlocks = UCHAR_MAX;
    else if (numBlocks == 0)
        numBlocks = 8 * blockSize;
    numBlocks_ = static_cast<unsigned char>(numBlocks);
    DEBUG_ASSERT(numBlocks_ == numBlocks);
    REGISTER_ALLOCATOR(stat_, this)
}

/*! Creates a FixedAllocator object of a fixed block size
 */
FixedAllocator::FixedAllocator(const FixedAllocator& rhs)
    : blockSize_(rhs.blockSize_), numBlocks_(rhs.numBlocks_),
      chunks_(rhs.chunks_)
{
    prev_ = &rhs;
    next_ = rhs.next_;
    rhs.next_->prev_ = this;
    rhs.next_ = this;

    allocChunk_ = rhs.allocChunk_
        ? chunks_.begin() + (rhs.allocChunk_ - rhs.chunks_.begin())
        : 0;

    deallocChunk_ = rhs.deallocChunk_
        ? chunks_.begin() + (rhs.deallocChunk_ - rhs.chunks_.begin())
        : 0;
    REGISTER_ALLOCATOR(stat_, this)
}

FixedAllocator& FixedAllocator::operator=(const FixedAllocator& rhs)
{
    FixedAllocator copy(rhs);
    copy.Swap(*this);
    return *this;
}

FixedAllocator::~FixedAllocator()
{
    if (prev_ != this)
    {
        prev_->next_ = next_;
        next_->prev_ = prev_;
    }
    else {

        DEBUG_ASSERT(prev_ == next_);
        Chunks::iterator i = chunks_.begin();
        for (; i != chunks_.end(); ++i) {
    //for debug if (i->blocksAvailable_ != numBlocks_)
    //            cerr << "BlockSize = " << (unsigned)blockSize_ << " i->blocksAvailable_="
    //                      << (int)i->blocksAvailable_
    //                      << "; numBlocks_="
    //                      << (int)numBlocks_
    //                      << endl;
            DEBUG_ASSERT(i->blocksAvailable_ == numBlocks_);
            i->Release();
        }
    }
    UNREGISTER_ALLOCATOR(this)
}

void FixedAllocator::Swap(FixedAllocator& rhs)
{
    std::swap(blockSize_, rhs.blockSize_);
    std::swap(numBlocks_, rhs.numBlocks_);
    chunks_.swap(rhs.chunks_);
    std::swap(allocChunk_, rhs.allocChunk_);
    std::swap(deallocChunk_, rhs.deallocChunk_);
}

/*! Allocates a block of fixed size
 */
void* FixedAllocator::Allocate()
{
    if (allocChunk_ == 0 || allocChunk_->blocksAvailable_ == 0)
    {
        Chunks::iterator i = chunks_.begin();
        for (;; ++i) {
            if (i == chunks_.end()) {
                // Initialize
                chunks_.reserve(chunks_.size() + 1);
                Chunk newChunk;
                newChunk.Init(blockSize_, numBlocks_);
                chunks_.push_back(newChunk);
                allocChunk_ = &chunks_.back();
                deallocChunk_ = chunks_.begin();
                break;
            }
            if (i->blocksAvailable_ > 0) {
                allocChunk_ = &*i;
                break;
            }
        }
    }
    DEBUG_ASSERT(allocChunk_ != 0);
    DEBUG_ASSERT(allocChunk_->blocksAvailable_ > 0);

    REGISTER_ALLOCATION(stat_)
    return allocChunk_->Allocate(blockSize_);
}

/*! Deallocates a block previously allocated with Allocate
    (undefined behavior if called with the wrong pointer)
 */
void FixedAllocator::Deallocate(void* p)
{
    DEBUG_ASSERT(chunks_.size());
    DEBUG_ASSERT(chunks_.begin() <= deallocChunk_);
    DEBUG_ASSERT(&chunks_.back() >= deallocChunk_);

    deallocChunk_  = VicinityFind(p);
    DEBUG_ASSERT(deallocChunk_);

    DoDeallocate(p);
    REGISTER_DEALLOCATION(stat_)
}

/*! \internal Finds the chunk corresponding to a pointer, using an efficient search
 */
FixedAllocator::Chunk* FixedAllocator::VicinityFind(void* p)
{
    DEBUG_ASSERT(chunks_.size());
    DEBUG_ASSERT(deallocChunk_);

    const size_t chunkLength = numBlocks_ * blockSize_;

    Chunk* loBound = chunks_.begin();
    Chunk* hiBound = &chunks_.back() + 1;
    Chunk* lo = deallocChunk_;
    Chunk* hi = deallocChunk_ + 1;
    if (hi == hiBound)
        hi = 0;

    for (;;) {
        DEBUG_ASSERT(hi || lo);
        if (lo) {
            if (p >= lo->pData_ && p < lo->pData_ + chunkLength)
                return lo;
            if (lo == loBound)
                lo = 0;
            else
                --lo;
        }
        if (hi) {
            if (p >= hi->pData_ && p < hi->pData_ + chunkLength)
                return hi;
            if (++hi == hiBound)
                hi = 0;
        }
    }
    DEBUG_ASSERT(false);
    return 0;
}

/*! \internal Performs deallocation. Assumes deallocChunk_ points to
     the correct chunk
 */
void FixedAllocator::DoDeallocate(void* p)
{
    DEBUG_ASSERT(deallocChunk_->pData_ <= p);
    DEBUG_ASSERT(deallocChunk_->pData_ + numBlocks_ * blockSize_ > p);

    // call into the chunk, will adjust the inner list but won't release memory
    deallocChunk_->Deallocate(p, blockSize_);

    if (deallocChunk_->blocksAvailable_ == numBlocks_) {
        // deallocChunk_ is completely free, should we release it?

        Chunk& lastChunk = chunks_.back();

        if (&lastChunk == deallocChunk_) {
            // check if we have two last chunks empty

            if (chunks_.size() > 1 &&
                deallocChunk_[-1].blocksAvailable_ == numBlocks_) {
                    // Two free chunks, discard the last one
                    lastChunk.Release();
                    chunks_.pop_back();
                    allocChunk_ = deallocChunk_ = chunks_.begin();
            }
            return;
        }

        if (lastChunk.blocksAvailable_ == numBlocks_) {
            // Two free blocks, discard one
            lastChunk.Release();
            chunks_.pop_back();
            allocChunk_ = deallocChunk_;
        } else {
            // move the empty chunk to the end
            std::swap(*deallocChunk_, lastChunk);
            allocChunk_ = &chunks_.back();
        }
    }
}

/*! Creates an allocator for small objects given chunk size and maximum 'small'
    object size
 */
SmallObjAllocator::SmallObjAllocator(size_t chunkSize,
                                     size_t maxObjectSize)
  : pLastAlloc_(0), pLastDealloc_(0),
    chunkSize_(chunkSize), maxObjectSize_(maxObjectSize)
{
}

/*! Allocates 'numBytes' memory;
    uses an internal pool of FixedAllocator objects for small objects
 */
void* SmallObjAllocator::Allocate(size_t numBytes)
{
    if (numBytes > maxObjectSize_)
        return operator new(numBytes);

    if (pLastAlloc_ && pLastAlloc_->BlockSize() == numBytes)
        return pLastAlloc_->Allocate();

    Pool::iterator i = std::lower_bound(pool_.begin(), pool_.end(), numBytes);
    if (i == pool_.end() || i->BlockSize() != numBytes) {
        i = pool_.insert(i, FixedAllocator(numBytes));
        pLastDealloc_ = &*pool_.begin();
    }
    pLastAlloc_ = &*i;
    return pLastAlloc_->Allocate();
}

/*! Deallocates memory previously allocated with Allocate
    (undefined behavior if you pass any other pointer)
 */
void SmallObjAllocator::Deallocate(void* p, size_t numBytes)
{
    if (numBytes > maxObjectSize_) {
        operator delete(p);
        return;
    }
    if (pLastDealloc_ && pLastDealloc_->BlockSize() == numBytes) {
        pLastDealloc_->Deallocate(p);
        return;
    }
    Pool::iterator i = std::lower_bound(pool_.begin(), pool_.end(), numBytes);
    DEBUG_ASSERT(i != pool_.end());
    DEBUG_ASSERT(i->BlockSize() == numBytes);
    pLastDealloc_ = &*i;
    pLastDealloc_->Deallocate(p);
}

} // end of namespace SmallObjPrivate

COMMON_NS_END
