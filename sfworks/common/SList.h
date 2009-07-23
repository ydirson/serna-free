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

#ifndef COMMON_SLIST_H_
#define COMMON_SLIST_H_

#include "common/common_defs.h"

COMMON_NS_BEGIN

template <class> class SList;

/*! Single-linked list item. List items must inherit from this template.
 */
template <class T> class SListItem {
public:
    T*  next() const { return slist_next_; }
    T*  removeNext()
    {
        T* temp = slist_next_;
        if (slist_next_)
            slist_next_ = slist_next_->slist_next_;
        return temp;
    }

    SListItem()
        : slist_next_(0) {}

private:
    T*  slist_next_;
    friend class SList<T>;
};

/*! A Single-linked list.
 */
template <class T> class SList {
public:
    /// Construct empty list
    SList()
        : head_(0) {};
    /// Construct list with single element
    SList(T *p)
        : head_(p) {}
    ~SList() { clear(); }

    /// Returns TRUE if list is empty
    bool  isEmpty() const { return !head_; }

    /// Pushes new element in front of the list
    void  push_front(T *p) { p->slist_next_ = head_; head_ = p; }

    /// Returns pointer to the beginning (head) of the list
    T*    first() const { return head_; }

    /// Pops element from the beginning of the list
    T*    pop_front()
    {
        T* t = head_;
        head_ = head_->slist_next_;
        t->slist_next_ = 0;
        return t;
    }

    /// Counts number of element in the list. This is expensive.
    unsigned int count() const
    {
        unsigned int sz = 0;
        for (const T* p = head_; p; p = p->slist_next_)
            ++sz;
        return sz;
    }
    /// Removes single element from anywhere within the list. This
    /// operation is expensive.
    void  removeSingleItem(T* p)
    {
        for (T **pp = &head_; *pp; pp = &(*pp)->slist_next_)
            if (*pp == p) {
                *pp = p->slist_next_;
                p->slist_next_ = 0;
                return;
            }
    }
    /// Clears up the list
    void  clear()
    {
        while (head_)
            pop_front();
    }
    /// Deletes all elements in the list and clears the list
    void  destroyAll()
    {
        while (head_)
            delete pop_front();
    }

private:
    T*    head_;
};

COMMON_NS_END

#endif // COMMON_SLIST_H_
