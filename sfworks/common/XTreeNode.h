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

#ifndef _XTREENODE_H_
#define _XTREENODE_H_

#include "common/common_defs.h"
#include "common/XList.h"
#include "common/TreelocRep.h"

COMMON_NS_BEGIN

/* XTreeNode comparsion result. NONCOMP means that nodes are in different
   trees and cannot be compared. */
enum XTreeNodeCmpResult { NONCOMP, EQUAL, LESS, GREATER };

/*! Implementation of the tree with refcounted children, which are kept
 *  in semi-circular double-linked list.
 */
template <class T, class XtPolicy = XTreeNodeNoNotify<T> >
class XTreeNode : public XList<T, XtPolicy>,
                  public XListItem<T, XtPolicy> {
public:
    typedef XTreeNode<T, XtPolicy> TN;
    typedef XList<T, XtPolicy> XLT;
    typedef XListItem<T, XtPolicy> XLTI;
public:
    REDECLARE_XLIST_INTERFACE_BASE(XLT, T, XLTI);
    REDECLARE_XLISTITEM_INTERFACE_BASE(XLTI, T, T);

    /// Return root of the current tree
    T*      root() const
    {
        TN* pn = const_cast<TN*>(this);
        while (pn->parent_)
            pn = static_cast<TN*>(pn->parent_);
        return static_cast<T*>(pn);
    }
    /// Get tree location of the node, relative to \a topNode
    void treeLoc(COMMON_NS::TreelocRep& tloc, const T* topNode = 0) const
    {
        const T* n = static_cast<const T*>(this);
        tloc.resize(0);
        for (;;) {
            if (n == topNode)
                break;
            if (0 == n->TN::parent_)
                break;
            tloc += n->siblingIndex();
            n = static_cast<const T*>(n->TN::parent_);
        }
    }
    T* getByTreeloc(const COMMON_NS::TreelocRep& tloc) const
    {
        const T* p = 0;
        const T* n = static_cast<const T*>(this);
        ulong i = 0;

        for (; i < tloc.length(); ++i) {
            p = static_cast<T*>(n->getChild(tloc[i]));
            if (!p)
                return 0;
            n = p;
        }
        return const_cast<T*>(n);
    }
    /* Returns nodes which are children of the common parent of this and
       otherNode, such as that *this are in subtree of r1, and otherNode
       is in subtree of r2. EQUAL here has the semantics that r1 and r2
       are on the same level of the tree (children of one parent). */
    XTreeNodeCmpResult  commonChildren(const T* otherNode,
                                       T*& r1, T*& r2) const;

    //! Returns common parent of current node and otherNode, if any
    T*                  commonParent(const T* otherNode) const;
    T*                  commonAos(const T* otherNode) const;

    //! Document-order comparsion of current node and otherNode.
    XTreeNodeCmpResult  comparePos(const T* otherNode) const;

    ~XTreeNode() {}
};

//
// Apparent complexity of the code above is for avoiding vector
// allocations if possible for reasonable tree depths
//
template <class T, class XtPolicy>
XTreeNodeCmpResult XTreeNode<T, XtPolicy>::commonChildren(const T* otherNode,
                                                          T*& r1, T*& r2) const
{
    const int psz = 256;
    T *path1_buf[psz], *path2_buf[psz];
    T **p1 = path1_buf, **p2 = path2_buf;
    T **p1s = p1, **p2s = p2;
    T *cn1 = const_cast<T*>(static_cast<const T*>(this));
    T *cn2 = const_cast<T*>(static_cast<const T*>(otherNode));
    Vector<T*> nv1, nv2;
    if (!otherNode)
        return NONCOMP;
    for (;;) {
        *p1++ = cn1;
        cn1 = static_cast<T*>(cn1->parent());
        if (!cn1)
            break;
        if (p1 >= (path1_buf + psz)) {
            nv1.reserve(psz * 2);
            for (T** p = path1_buf; p < p1; ++p)
                nv1.push_back(*p);
            for (;;) {
                nv1.push_back(cn1);
                cn1 = static_cast<T*>(cn1->parent());
                if (!cn1)
                    break;
            }
            p1  = nv1.end();
            p1s = nv1.begin();
            break;
        }
    }
    for (;;) {
        *p2++ = cn2;
        cn2 = static_cast<T*>(cn2->parent());
        if (!cn2)
            break;
        if (p2 >= (path2_buf + psz)) {
            nv2.reserve(psz * 2);
            for (T** p = path2_buf; p < p2; ++p)
                nv2.push_back(*p);
            for (;;) {
                nv2.push_back(cn2);
                cn2 = static_cast<T*>(cn2->parent());
                if (!cn2)
                    break;
            }
            p2  = nv2.end();
            p2s = nv2.begin();
            break;
        }
    }
    if (*--p1 != *--p2) {
        r1 = r2 = 0;
        return NONCOMP;       // no common parent
    }
    for (;;) {
        if (*p1 != *p2) {
            r1 = *p1; r2 = *p2;
            return EQUAL;
        }
        if (p1 == p1s) {
            r1 = *p1; r2 = *p1;
            return (p2 == p2s) ? EQUAL : LESS;
        }
        if (p2 == p2s) {
            r1 = *p2; r2 = *p2;
            return GREATER;
        }
        --p1; --p2;
    }
}

template <class T, class XtPolicy>
T* XTreeNode<T, XtPolicy>::commonParent(const T* otherNode) const
{
    T *r1, *r2;
    if (commonChildren(otherNode, r1, r2) == NONCOMP)
        return 0;
    return static_cast<T*>(r1->parent());
}

template <class T, class XtPolicy>
T* XTreeNode<T, XtPolicy>::commonAos(const T* otherNode) const
{
    T *r1, *r2;
    XTreeNodeCmpResult res = commonChildren(otherNode, r1, r2);
    if (NONCOMP == res)
        return 0;
    if (r1 != r2)
        return static_cast<T*>(r1->parent());
    return static_cast<T*>(r1);
}

template <class T, class XtPolicy>
XTreeNodeCmpResult XTreeNode<T, XtPolicy>::comparePos(const T* otherNode) const
{
    T *r1, *r2;
    XTreeNodeCmpResult res = commonChildren(otherNode, r1, r2);
    if (res != EQUAL)
        return res;
    if (r1 == r2)
        return EQUAL; // comparsion with *this
    for (const T* c = static_cast<T*>(r1->parent()->firstChild()); c;
         c = static_cast<T*>(c->nextSibling())) {
        if (c == r1)
            return LESS;
        if (c == r2)
            return GREATER;
    }
    return NONCOMP; // none children not found?
}

/*! This macro is used with multiple-inheritance scenario to redeclare
 *  interface of XTreeNode with respect to the specfied base class in order
 *  to avoid ambiguites.
 */
#define REDECLARE_XTREENODE_INTERFACE_BASE(B, T) \
    REDECLARE_XLIST_INTERFACE_BASE(B, T, T) \
    REDECLARE_XLISTITEM_INTERFACE_BASE(B, T, T) \
    T*      root() const { return B::root(); } \
    void    treeLoc(COMMON_NS::TreelocRep& tl, const T* tn = 0) const \
        { B::treeLoc(tl, tn); }                            \
    T*      getByTreeloc(const COMMON_NS::TreelocRep& tl) const       \
        { return B::getByTreeloc(tl); }

#define REDECLARE_XTREENODE_INTERFACE(T) \
    REDECLARE_XTREENODE_INTERFACE_BASE(T, T)

COMMON_NS_END

#endif // XTREENODE_H_

