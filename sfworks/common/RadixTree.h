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
    The Radix tree
 */
#ifndef RADIX_TREE_H_
#define RADIX_TREE_H_

#include "common/common_defs.h"
#include "common/common_types.h"


namespace RadixTreePrivate {

/**
 * The Radix Tree node
 *
 * This is an internal class for Radix Tree template
 */
class RTNode {
public:
    enum {
        BPL = 4,          ///< Radix tree bits per level
        EPL = 1 << BPL,   ///< Elements per level
        LMASK = EPL - 1   ///< Bitmask for element index
    };

    /// The empty node constructor
    RTNode() { zeroTreeNode(); }

    /// Zero pointers in a tree node
    void zeroTreeNode()
    {
        for (int32 i = EPL; i--;)
            p[i] = 0;
    }

    /// Clean the sub-trees recursively
    void cleanTreeNode(int32 bits)
    {
        bits -= BPL;
        if (bits <= 0)
            return;

        for (int32 i = EPL; i--;) {
            RTNode* n = p[i];
            if (n) {
                n->cleanTreeNode(bits);
                delete n;
            }
        }
    }

    /// Erase the field idx, and count non-zero pointers
    int32 erase(int32 idx)
    {
        p[idx] = 0;
        int32 cnt = 0;
        for (int32 i = EPL; i--;) {
            if (p[i])
                cnt++;
        }
        return cnt;
    }

    /// Add another level
    void addLevel()
    {
        RTNode* t = new RTNode(*this);

        p[0] = t;
        for (int32 i = EPL-1; i--;)
            p[i+1] = 0;
    }
    /// Pointers to sub-nodes or leaves
    RTNode* p[1 << BPL];

#ifndef RADIX_TREE_DEBUG
private:
#endif // RADIX_TREE_DEBUG

    /// Print tree node
    void printTreeNode(int32 bits);

    /// Count tree nodes
    int32 countTreeNodes(int32 bits);
};

} // namespace RadixTreePrivate

COMMON_NS_BEGIN
/**
 * The radix tree of elements T
 * indexed by a 32-bit integer number
 */
template <class T> class RadixTree : public RadixTreePrivate::RTNode {
public:
    typedef RadixTreePrivate::RTNode Node;
    typedef RadixTree<T> Tree;

    //// The tree iterator
    class iterator {
    public:
        /// Get pointer to the element
        T* get() const { return cur_; }

        /// Returns pointer to the element
        T* operator->() const { return cur_; }

        /// Returns reference to the element
        T& operator*() const { return *cur_; }

        /// Prefix ++
        iterator& operator++()
        {
            cur_ = tree_->advance(index_);
            return *this;
        }

        /// Return the curent index
        int32 getIndex() const
        {
            return index_;
        }

        /// Replace the current element
        void set(T* elem)
        {
            if (cur_) {
                tree_->set(elem, index_);
                cur_ = elem;
            }
        }

        /// Delete the current element
        void remove()
        {
            tree_->remove(index_);
            cur_ = tree_->advance(index_);
        }

        /// Compare iterators for equality
        bool operator==(const iterator& x) const
        {
            return tree_ == x.tree_ && index_ == x.index_;
        }

        /// Compare iterators for inequality
        bool operator!=(const iterator& x) const
        {
            return !(*this == x);
        }
        bool operator!() const { return !cur_; }

        iterator() {}
        iterator(const Tree* tp, int32 i, T* cur)
          : tree_(const_cast<Tree*>(tp)), index_(i), cur_(cur) {}

        friend class RadixTree<T>;

    protected:
        Tree*   tree_;          ///< The tree
        int32   index_;         ///< The current index in the tree
        T*      cur_;           ///< The current element
    };

    /// Const iterator
    class const_iterator : public iterator {
        using iterator::cur_;
    public:
        const T* get() const { return cur_; }
        const T* operator->() const { return cur_; }
        const T& operator*() const { return *cur_; }
        const_iterator() {}
        const_iterator(const iterator& it) : iterator(it) {}

    private:
        void set(T*);
        void remove();
    };

    /// Constructor
    RadixTree() : bits_(BPL), lastkey_(-1) {}

    /// Destructor (DOES NOT destruct elements!)
    ~RadixTree()
    {
        cleanTreeNode(bits_);
    }

    /// Returns pointer to the tree element
    T* get(int32 idx) const;

    /// Set element with the given index
    void set(T* elem, int32 idx);

    /// Returns an unused key (do not touch first "reserved" keys)
    int32 unique(int32 reserved = 0)
    {
        do {
            if (++lastkey_ < reserved)
                lastkey_ = reserved;
        } while (get(lastkey_));
        return lastkey_;
    }

    /// Remove element with the given index
    void remove(int32 idx);

    /// The max number of elements in the tree
    int32 maxElements() const
    {
        return 1L << bits_;
    }

    /// Get iterator at the first element of the tree
    iterator begin() const;

    /// Get iterator past the last element of the tree
    iterator end() const;

    /// Tests for emptiness
    bool isEmpty() const { return begin() == end(); }

    /// Advance index to the next element of the tree
    T* advance(int32& idx) const;

    /// Clean the radix tree
    void clean()
    {
        cleanTreeNode(bits_);
        zeroTreeNode();
        bits_ = BPL;
    }

#ifndef RADIX_TREE_DEBUG
private:
#endif // RADIX_TREE_DEBUG

    /// Print out the tree
    void printTree();

    /// Count tree nodes
    int32 countNodes()  { return countTreeNodes(bits_); };

private:
    int32   bits_;          ///< Number of key bits (always multiple of BPL)
    int32   lastkey_;       ///< The last allocated unique key
};

/**
 * Get pointer to an element with the given index
 *
 * @arg idx element's index
 * @return pointer to the lement
 */
template <class T>
  inline T* RadixTree<T>::get(int32 idx) const
{
    if (idx >> bits_)   // more bits than in the table
        return 0;

    const Node* t = this;
    int32 rembits = bits_;
    do {
        rembits -= BPL;
        t = t->p[(idx >> rembits) & LMASK];
        if (rembits <= 0)
            return (T*) t;
    } while (t);
    return 0;
}

/**
 * Set an element in the tree
 *
 * Replaces existing element with the same index
 *
 * @arg elem pointer to the element
 * @arg idx the index
 */
template <class T>
  inline void RadixTree<T>::set(T* elem, int32 idx)
{
    // Add more levels to the tree if necessary
    while (idx >> bits_ && bits_ < 32) {
        addLevel();
        bits_ += BPL;
    }

    // Walk down the tree creating nodes if necessary
    Node* t = this;
    int32 rembits = bits_;
    for(;;) {
        rembits -= BPL;
        Node** tp = &t->p[(idx >> rembits) & LMASK];
        if (rembits <= 0) {
            *tp = (Node*)(elem);
            return;
        }
        t = *tp;
        if (t == 0)
            *tp = t = new Node;
    }
}

/**
 * Remove element from the tree
 * Does not reduce number of levels, but destroys extraneous nodes
 *
 * @arg idx the element's index
 */
template <class T>
  inline void RadixTree<T>::remove(int32 idx)
{
    if (idx >> bits_ && bits_ < 32)   // more bits than in the table
        return;

    //
    // Locate reference to the element
    //
    Node* t = this;
    int32 rembits = bits_;
    int32 i;
    for(;;) {
        rembits -= BPL;
        i = (idx >> rembits) & LMASK;
        if (t->p[i] == 0)
            return;         // element didn't exist, do nothing
        if (rembits <= 0)
            break;
        t = t->p[i];
    }

    //
    // Destroy node t and (possibly) its ancestors by
    // repeatedly descending the tree and zeroing pointers
    // until a node with more than one pointer is encountered
    //
    while (t->erase(i) == 0 && t != this) {
        Node* u = this;
        rembits = bits_;
        for(;;) {
            rembits -= BPL;
            i = (idx >> rembits) & LMASK;
            if (u->p[i] == t)
                break;
            u = u->p[i];
        }
        delete t;
        t = u;
    }
}

/**
 * Create iterator poiting to the first element of the tree
 *
 * @return iterator
 */
template <class T>
  inline typename RadixTree<T>::iterator RadixTree<T>::begin() const
{
    iterator i(this, 0, get(0));
    if (i.cur_ == 0)
        i.cur_ = advance(i.index_);
    return i;
}

/**
 * Create iterator pointing past the last element of the tree
 *
 * @return iterator
 */
template <class T>
  inline typename RadixTree<T>::iterator RadixTree<T>::end() const
{
    return iterator(this, -1, 0);
}

/**
 * Advance to the next tree element, and update index
 *
 * @arg idx the index
 * @return pointer to the new element, or 0 if not found
 */
template <class T>
  inline T* RadixTree<T>::advance(int32& idx) const
{
    // Increment index (-1 is reserved for terminator)
    if (idx == -1)
        return 0;
    idx++;

    // Try to get to the incremented index
next_idx:
    while (idx < maxElements()) {
        const Node* t = this;
        int32 rembits = bits_;

        do {
            rembits -= BPL;
            if (rembits <= 0) {
                // Scan the last block linearly
                do {
                    T* res = (T*)(t->p[idx & LMASK]);

                    if (res)
                        return res;
                    idx++;
                } while (idx & LMASK);
                goto next_idx;
            }

            t = t->p[(idx >> rembits) & LMASK];
        } while (t);

        // Intermediate node is missing, increment by the number
        // of the missing leaves
        rembits = 1L << rembits;
        idx &= ~(rembits - 1);
        idx += rembits;
    }

    // ran out of range
    idx = -1;
    return 0;
}

COMMON_NS_END

#endif // RADIX_TREE_H_
