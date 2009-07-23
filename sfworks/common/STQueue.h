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
    The single-linked tail queue
 */
#ifndef ST_QUEUE_H_
#define ST_QUEUE_H_

#include "common/common_defs.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

template<class T> class STQueue;

/*! Single-tail queue list item - objects which are to be kept
    in STQueue<T> must inherit from STQueueItem<T>.
 */
template <class T>
class STQueueItem {
public:
    STQueueItem() : stqueue_next_(0) {}
    /*! Get pointer to the next element in the list
     */
    T* next() const { return stqueue_next_; }

private:
   friend class STQueue<T>;
   T* stqueue_next_;     // chain link
};

/*! A single-linked tail queue; better than just a singly linked list
    because it allows constant time appends, and useful for implementing
    both FIFO and LIFO's.

    Objects of type T which are kept in STQueue must inherit from
    STQueueItem<T>.
 */
template <class T> class STQueue {
public:
    typedef T OriginalType;
    struct iterator {
        iterator& operator++() {p_ = p_->next(); return *this; }
        T* pointer() const { return p_; }
        T* operator->() const { return p_; }
        T& operator*() const { return *p_; }
        operator T*() const { return p_; }
        iterator(T* tp) : p_(tp) {}
        iterator() : p_(0) {}
        iterator(const iterator& it) { p_ = it.p_; }
        bool operator==(const iterator& it) const { return it.p_ == p_; }
        bool operator!=(const iterator& it) const { return !(it == *this); }
        operator bool() const { return !p_; }

    protected:
        T* p_;
    };
    struct const_iterator : public iterator {
        using iterator::p_;
        const T* pointer() const { return p_; }
        const T* operator->() const { return p_; }
        const T& operator*() const { return *p_; }
                operator const T*() const { return p_; }
                const_iterator(const T* tp) : iterator(const_cast<T*>(tp)) {}
        const_iterator(const iterator& it) : iterator(it) {}
        const_iterator() {}
    };

    bool empty() const { return !head_; }

    T* push_front(T* elem) {
        elem->stqueue_next_ = head_;
        if (!head_)
            tail_ = elem;
        head_ = elem;
        return elem;
    }

    T* push_back(T* elem) {
        elem->stqueue_next_ = 0;
        if (head_)
            tail_->stqueue_next_ = elem;
        else
            head_ = elem;
        tail_ = elem;
        return elem;
    }

    T* pop_front() {
        T* temp = head_;
        if (temp) {
            head_ = temp->stqueue_next_;
            temp->stqueue_next_ = 0;
        }
        return temp;
    }

    T* insertAfter(T* after, T* elem) {
        elem->stqueue_next_ = after->stqueue_next_;
        after->stqueue_next_ = elem;
        if (after == tail_)
            tail_ = elem;
        return elem;
    }

    T* removeAfter(T* after) {
        T* temp = after->stqueue_next_;
        if (temp) {
            after->stqueue_next_ = temp->stqueue_next_;
            if (temp == tail_)
                tail_ = after;
        }
        return temp;
    }

    T* remove(T* elem) {
        if (head_ == elem) {
            head_ = head_->stqueue_next_;
            if (!head_)
                tail_ = head_;
        } else {
            T* temp;
            for (temp = head_; temp && temp->stqueue_next_ != elem;
                 temp = temp->stqueue_next_)
                ;
            if (!temp)
                return 0;
            temp->stqueue_next_ = temp->stqueue_next_->stqueue_next_;
            if (!temp->stqueue_next_)
                tail_ = temp;
        }
        return elem;
    }

    void clear() { head_ = 0; tail_ = head_; }

    STQueue() : head_(0), tail_(head_) {}
    STQueue(T* elem)
        : head_(0), tail_(head_) { push_back(elem); }

    T* first() { return head_; }
    const T* first() const { return head_; }

    T* last() { return tail_; }
    const T* last() const { return tail_; }
    T* back() const { return tail_; }

    iterator begin() { return first(); }
    const_iterator begin() const { return first(); }

    iterator end() const { return 0; }

    /// Erase all elements in the list
    void destroyAll()
    {
        T* tn;
        T* t = head_;
        while (t) {
            tn = t->stqueue_next_;
            delete t;
            t = tn;
        }
        head_ = 0;
    }
    /// Count number of elements in the list
    unsigned long count() const
    {
        unsigned long cnt = 0;
        for (const T* t = head_; t; t = t->stqueue_next_)
            ++cnt;
        return cnt;
    }
    /// Append list l to the end of the current list; l is emptied
    void append(STQueue<T>& l)
    {
        if (l.head_) {
            if (head_ == 0)
                head_ = l.head_;
            else
                tail_->stqueue_next_ = l.head_;
            tail_ = l.tail_;
            l.head_ = l.tail_ = 0;
        }
    }
    /// Assign other list. This leaves other list empty.
    /// Swap two lists
    void assign(STQueue<T>& l)
    {
        head_ = l.head_;
        tail_ = l.tail_;
        l.head_ = l.tail_ = 0;
    }

private:
    /// Beginning of the list
    T*  head_;
    /// Tail of the list
    T*  tail_;
};

/*! A flavor of STQueue which owns all kept objects (deletes them
    in the destructor).
 */
template <class T> class OwnerSTQueue : public STQueue<T> {
public:
    OwnerSTQueue() {}
    OwnerSTQueue(T* elem) : STQueue<T>(elem) {}
    ~OwnerSTQueue() { this->destroyAll(); }

private:
    OwnerSTQueue(const STQueue<T>&);            // copy/assign not allowed
    OwnerSTQueue& operator=(const STQueue<T>&);
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // ST_QUEUE_H_
