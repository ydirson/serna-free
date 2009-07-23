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
    The doubly-linked circular list
 */
#ifndef CD_LIST_H_
#define CD_LIST_H_

#include "common/common_defs.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

/*! List node for type T, with next and prev pointers.
    T must inherit from ListItem<T>.
 */
template <class T> class CDListItem {
public:
    typedef CDListItem<T> CDLItem;

    CDListItem() { next_ = prev_ = this; }
    ~CDListItem() { remove(); }

    //! Returns pointer to the the next node
    T*      next() const { return static_cast<T*>(next_); }

    //! Returns the previous node
    T*      prev() const { return static_cast<T*>(prev_); }

    //! Insert given node before this node
    void    insertBefore(CDLItem* item)
    {
        prev_->next_ = item;
        item->prev_ = prev_;
        prev_ = item;
        item->next_ = this;
    }
    //! Insert given node after this node
    void    insertAfter(CDLItem* item)
    {
        item->next_ = next_;
        next_->prev_ = item;
        next_ = item;
        item->prev_ = this;
    }
    //! Removes current node from a list and makes it "shortcircuited"
    void    remove()
    {
        next_->prev_ = prev_;
        prev_->next_ = next_;
        next_ = prev_ = this;
    }

    //!TODO: Code revision is needed for following code
    //!      some methods are unnecessary

    //! "Shortcircuits" this node, i. e. makes it point to itself
    void    shortcircuit() { next_ = prev_ = this; }

    //! Returns true if the node does not belong to any structure
    bool    isByItself() const { return next_ == this; }

    //! Insert this node before node pointed by the \a next
    void    insert(CDLItem* next)
    {
        next_ = next;
        prev_ = next->prev_;
        prev_->next_ = this;
        next->prev_ = this;
    }
    //! Returns next list item
    CDLItem*       _next_() { return next_; }
    const CDLItem* _next_() const { return next_; }

    //! Returns previous list item
    CDLItem*       _prev_() { return prev_; }
    const CDLItem* _prev_() const { return prev_; }

    /// Used to append one list to another
    void        _append_(CDLItem& head2)
    {
        prev_->next_ = head2.next_;
        head2.next_->prev_ = prev_;
        prev_ = head2.prev_;
        prev_->next_ = this;
        head2.shortcircuit();
    }

private:
    CDLItem*   next_;
    CDLItem*   prev_;
};


/*! Circular double-linked list. \a T must inherit from ListItem<T>.
 */
template <class T> class CDList {
public:
    typedef CDListItem<T> CDLItem;
    typedef T OriginalType;

    struct iterator {
        iterator&   operator++() { p_ = p_->next(); return *this;}
        iterator&   operator--() { p_ = p_->prev(); return *this;}
        T*          operator->() const { return p_; }
        T&          operator*() const { return *p_; }
        T*          pointer() const { return p_; }
        operator    T*() const { return p_; }
        T*          current() const { return p_; }
        iterator(T* tp) : p_(tp) {}
        iterator() : p_(0) {}
        iterator(const iterator& it) { p_ = it.p_; }
        bool operator==(const iterator& it) const { return it.p_ == p_; }
        bool operator!=(const iterator& it) const { return !(it == *this); }
        operator    bool() const { return !p_; }
    protected:
        T* p_;
    };
    struct const_iterator : public iterator {
        using iterator::p_;

        const_iterator(const T* tp) : iterator(const_cast<T*>(tp)) {}
        const_iterator(const iterator& it) : iterator(it) {}
        const_iterator() {}

        const T*        operator->() const { return p_; }
        const T*        current() const { return p_; }
        const T&        operator*() const { return *p_; }
        const T*        pointer() const { return p_; }
        operator const  T*() const { return p_; }
    };

    CDList() {}
    CDList(T* elem) { head_.insert(elem); }

    //! Return true if the list is empty
    bool    isEmpty() const { return head_._next_() == &head_; }

    //! Returns pointer to the first element of the list or 0 if list is empty
    T*      first() const { return isEmpty() ? 0 : head_.next(); }

    //! Returns pointer to the last element of the list or 0 if list is empty
    T*      last() const { return isEmpty() ? 0 : head_.prev(); }

    //! Inserts the element \a elem at the beginning of the list
    void    push_front(CDLItem* elem) { elem->insert(head_._next_()); }

    //! Appends element \a to the end of the list
    void    push_back(CDLItem* elem) { elem->insert(&head_); }

    //! appends content of list <l> to this list. <l> becomes empty.
    void    append(CDList<T>& l) { head_._append_(l.head_); }

    /*! Returns pointer to the element at the beginning of the list and
        removes it from the list; returns 0 if list is empty */
    T*      pop_front()
    {
        CDLItem* a = head_._next_();
        if (a == &head_)
            return 0;
        a->remove();
        return static_cast<T*>(a);
    }
    /*! Returns pointer to the element at the end of the list and
        removes it fromthe list; returns 0 if list is empty */
    T*      pop_back()
    {
        CDLItem* a = head_._prev_();
        if (a == &head_)
            return 0;
        a->remove();
        return static_cast<T*>(a);
    }
    /*! Removes all elements from the list (this does not mean
        their deconstruction - CDList do not own it's elements). */
    void    clear()
    {
        CDLItem* a = head_._next_();
        CDLItem* a1;
        while(a != &head_) {
            a1 = a->_next_();
            a->shortcircuit();
            a = a1;
        }
        head_.shortcircuit();
    }
    //! Returns the first element of the list.
    iterator        begin() { return head_.next(); }
    const_iterator  begin() const { return head_.next(); }

    /*! Returns pointer to the terminating element of the list
        (this element is not a part of the list itself) */
    iterator        end() { return static_cast<T*>(&head_); }
    const_iterator  end() const { return static_cast<const T*>(&head_); }

    //! Deconstructs all elements in the list
    void    destruct()
    {
        T* a;
        while ((a = pop_front()))
            delete a;
    }
    void    swap(CDList<T>& other)
    {
        CDLItem temp = head_;
        head_ = other.head_;
        other.head_ = temp;
    }
private:
    CDList(const CDList<T>&);         // copy/assignment not allowed
    void    operator=(const CDList<T>&);
    CDLItem    head_; // Head of the CDList
};


/**
 * list that destructs its content
 */
template<class T>
class OwnerList : public CDList<T>
{
public:
    /// destructor
    ~OwnerList() { this->destruct(); }
};


# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // CD_LIST_H_
