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
 *  Implementation of the parented, reference-counted double-linked list
 */

#ifndef _COMMON_XLIST_H_
#define _COMMON_XLIST_H_

#include "common/common_defs.h"
#include "common/common_types.h"
#include "common/XTreePolicies.h"

COMMON_NS_BEGIN

template <class T, class P> class XListItem;
template <class T, class P> class XTreeNode;

template <class T,
          class XtPolicy = XTreeNodeNoNotify<T> >
class XList {
    typedef XListItem<T, XtPolicy> XLTI;
public:

    /// Returns true if list has no items
    bool    empty() const { return (0 == headp_); }

    /// Get pointer to the first child
    T*      firstChild() const { return static_cast<T*>(headp_); }

    /// Get pointer to the last child
    T*      lastChild() const
    {
        if (headp_)
            return static_cast<T*>(headp_->prev_);
        return 0;
    }
    /// Get pointer to the n-th child
    T*      getChild(ulong n) const
    {
        XLTI* np = headp_;
        for (++n; --n && np; np = np->nextSibling())
            ;
        return static_cast<T*>(np);
    }
    /// Count children
    uint    countChildren() const
    {
        uint i = 0;
        for (const XLTI* p = firstChild(); p; p = p->nextSibling())
            ++i;
        return i;
    }
    /// Append new child to the end of children list. Current node is
    /// considered as parent node.
    void    appendChild(XLTI* child)
    {
        if (headp_) {
            child->next_ = 0;
            child->prev_ = headp_->prev_;
            headp_->prev_->next_ = child;
            headp_->prev_ = child;
        } else {
            headp_ = child;
            headp_->prev_ = child;
            headp_->next_ = 0;
        }
        child->parent_ = this;
        XtPolicy::xt_incRefCnt(static_cast<T*>(child));
        XtPolicy::notifyChildInserted(static_cast<T*>(child));
    }
    /// Remove all children
    void removeAllChildren()
    {
        while (headp_)
            headp_->remove();
    }
    XList()
        : headp_(0) {}
    ~XList()
    {
        removeAllChildren();
    }

private:
    friend class XListItem<T, XtPolicy>;
    friend class XTreeNode<T, XtPolicy>;

    XLTI*  headp_;     // pointer to the first children
};

/*! Implementation of the tree with refcounted children, which are kept
 *  in semi-circular double-linked list.
 */
template <class T, class XtPolicy = XTreeNodeNoNotify<T> > class XListItem {
    typedef XListItem<T, XtPolicy> XLTI;
    typedef XList<T, XtPolicy> XLT;
public:
    /// Returns parent node.
    XLT*      parent() const { return parent_; }

    /// Re-set parent. Dangerous, use with caution.
    void    setParent(XLT* p) { parent_ = p; }

    /// Get pointer to the previous sibling, 0 if none
    T*      prevSibling() const {
        if (!prev_->next_)
            return 0;
        return static_cast<T*>(prev_);
    }
    /// Get pointer to the next sibling, 0 if none
    T*      nextSibling() const { return static_cast<T*>(next_); }

    /// Index among siblings (counting from leftmost sibling)
    uint    siblingIndex() const
    {
        uint cnt = 0;
        for (const XLTI* n = prevSibling(); n; n = n->prevSibling())
            ++cnt;
        return cnt;
    }
    /// Insert child before this node (as left sibling)
    void    insertBefore(XLTI* child)
    {
        child->next_  = this;
        child->prev_  = prev_;
        if (parent_->headp_ == this)
            parent_->headp_ = child;
        else
            prev_->next_ = child;
        prev_ = child;
        child->parent_ = parent_;
        XtPolicy::xt_incRefCnt(static_cast<T*>(child));
        XtPolicy::notifyChildInserted(static_cast<T*>(child));
    }
    /// Insert child after this node (as right sibling)
    void    insertAfter(XLTI* child)
    {
        child->next_ = next_;
        child->prev_ = this;
        if (next_)
            next_->prev_ = child;
        else
            parent_->headp_->prev_ = child;
        next_ = child;
        child->parent_ = parent_;
        XtPolicy::xt_incRefCnt(static_cast<T*>(child));
        XtPolicy::notifyChildInserted(static_cast<T*>(child));
    }
    /// Remove child (myself as a child of my parent)
    void    remove()
    {
        if (!parent_)
            return;
        XLTI*& hp = parent_->headp_;
        if (!hp)
            return;
        if (hp == this) {   // first element or the one element
            hp = next_;
            if (next_)
                next_->prev_ = prev_;
        } else {
            prev_->next_ = next_;
            if (next_)
                next_->prev_ = prev_;
            else
                hp->prev_ = prev_; // if last element
        }
        XLT* oldp = parent_;
        parent_ = 0;
        XtPolicy::notifyChildRemoved(oldp, static_cast<T*>(this));
        if (!XtPolicy::xt_decRefCnt(static_cast<T*>(this)))
            delete static_cast<T*>(this);
    }
    /*! Remove group of children, starting from current tree node
     *  and until \a endNode, inclusive.
     */
    void removeGroup(XLTI* endNode)
    {
        XLTI *ns, *n = this;
        do {
            ns = n->nextSibling();
            n->remove();
            if (n == endNode)
                break;
            n = ns;
        } while(n);
    }
    XListItem()
        : parent_(0), prev_(this), next_(0) {}

    ~XListItem()
    {
        remove();
    }

protected:
    friend class XList<T, XtPolicy>;
    friend class XTreeNode<T, XtPolicy>;

    XLT* parent_;    // pointer to the parent
    XLTI*  prev_;      // pointer to the previous sibling
    XLTI*  next_;      // pointer to the next sibling
};

template <class T> class OwnedXList : public XList<T> {
public:
    void    destroyAll()
    {
        while (XList<T>::firstChild())
            delete XList<T>::firstChild();
    }
    ~OwnedXList() { destroyAll(); }
};

/*! This macro is used with multiple-inheritance scenario to redeclare
 *  interface of XListItem with respect to the specfied base class in order
 *  to avoid ambiguites.
 */
#define REDECLARE_XLISTITEM_INTERFACE_BASE(B, T, P) \
    P*      parent() const { return static_cast<P*>(B::parent()); } \
    T*      prevSibling() const { return static_cast<T*>(B::prevSibling()); } \
    T*      nextSibling() const { return static_cast<T*>(B::nextSibling()); } \
    uint    siblingIndex() const { return B::siblingIndex(); } \
    void    insertBefore(B* child) { B::insertBefore(child); } \
    void    insertAfter(B* child) { B::insertAfter(child); } \
    void    remove() { B::remove(); } \
    void    removeGroup(B* endNode) { B::removeGroup(endNode); }

#define REDECLARE_XLISTITEM_INTERFACE(T) \
    REDECLARE_XLISTITEM_INTERFACE_BASE(T, T, T)

#define REDECLARE_XLIST_INTERFACE_BASE(B, T, TN) \
    bool    empty() const { return B::empty(); } \
    T*      firstChild() const { return static_cast<T*>(B::firstChild()); } \
    T*      lastChild() const { return static_cast<T*>(B::lastChild()); } \
    T*  getChild(ulong n) const { return static_cast<T*>(B::getChild(n)); } \
    uint    countChildren() const { return B::countChildren(); } \
    void    appendChild(TN* child) { B::appendChild(child); } \
    void    removeAllChildren() { B::removeAllChildren(); }

#define REDECLARE_XLIST_INTERFACE(T) \
    REDECLARE_XLIST_INTERFACE_BASE(T, T, T)

#define XLIST_CONTAINING_PARENT_CLASS(T, M) \
    (T*)((char*)(parent()) - (((size_t)&((T*)16)->M) - 16))

#define XLIST_CONTAINING_PARENT_CLASS2(T, M, B) \
    (T*)((char*)(B::parent()) - (((size_t)&((T*)16)->M) - 16))

COMMON_NS_END

#endif // COMMON_XLIST_H_

