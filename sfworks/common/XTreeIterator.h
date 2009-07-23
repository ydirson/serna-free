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
#ifndef COMMON_XTREE_ITERATOR_H_
#define COMMON_XTREE_ITERATOR_H_

#include "common/common_defs.h"

COMMON_NS_BEGIN

template <class T>
class XTreeUpwardIterator {
public:
    XTreeUpwardIterator(T* node)
        : node_(node) {}
    T*  node() const { return node_; }
    T*  operator->() const { return node_; }

    XTreeUpwardIterator& operator++()       // go to right
    {
        if (0 == node_)
            return *this;
        if (node_->nextSibling())
            node_ = node->nextSibling();
        else
            node_ = node_->parent();
        return *this;
    }
    XTreeUpwardIterator& operator--()      // go to left
    {
        if (0 == node_)
            return *this;
        T* ps = node_->prevSibling();
        if (ps)
            node_ = ps;
        else
            node_ = node_->parent();
        return *this;
    }
    XTreeUpwardIterator& operator=(T* other)
    {
        node_ = other;
        return *this;
    }
    XTreeUpwardIterator& operator=(const XTreeUpwardIterator<T>& other)
    {
        operator=(other.node());
    }

private:
    T*  node_;
};

template <class T>
class XTreeDfsIterator {
public:
    XTreeDfsIterator(T* node = 0)
        : node_(node) {}

    T*    node() const { return node_; }
    T*    operator->() const { return node_; }
    T&    operator*() const { return *node_; }

    XTreeDfsIterator& operator++() {
        if (0 == node_)
            return *this;
        if (node_->nextSibling()) {
            node_ = node_->nextSibling();
            while (node_->firstChild())
                node_ = node_->firstChild();
        }
        else
            node_ = node_->parent();
        return *this;
    }
    XTreeDfsIterator& operator--() {
        if (0 == node_)
            return *this;
        if (node_->prevSibling()) {
            node_ = node_->prevSibling();
            while (node_->lastChild())
                node_ = node_->lastChild();
        }
        else
            node_ = node_->parent();
        return *this;
    }
    bool operator==(const XTreeDfsIterator& o) { return node_ == o.node_; }
    bool operator!=(const XTreeDfsIterator& o) { return node_ != o.node_; }
private:
    T* node_;
};

// Iterator for walking the tree in document order, 
// starting upwards or downwards. This differs from DocOrderIterator in a way
// that parent nodes are also visited during walk (so they may be visited
// twice)
template <class T> class XTreeWalkIterator {
public:
    XTreeWalkIterator(T* node, bool down)
        : node_(node), down_(down) {}

    T*    node() const { return node_; }
    T*    operator->() const { return node_; }

    XTreeWalkIterator& operator++() {
        if (0 == node_)
            return *this;
        if (down_ && node_->firstChild())
            node_ = node_->firstChild();
        else {
            if (node_->nextSibling()) {
                down_ = true;
                node_ = node_->nextSibling();
            }
            else {
                down_ = false;
                node_ = node_->parent();
            }
        }
        return *this;
    }
    XTreeWalkIterator& operator--() {
        if (0 == node_)
            return *this;
        if (down_ && node_->lastChild())
            node_ = node_->lastChild();
        else {
            if (node_->prevSibling()) {
                down_ = true;
                node_ = node_->prevSibling();
            }
            else {
                down_ = false;
                node_ = node_->parent();
            }
        }
        return *this;
    }
private:
    T*      node_;
    bool    down_;
};

// Document order iterator.
template <class T> class XTreeDocOrderIterator {
public:
    XTreeDocOrderIterator(T* node)
        : node_(node) {}
    
    T*    node() const { return node_; }
    T*    operator->() const { return node_; }

    XTreeDocOrderIterator& operator++()   
    {
        if (!node_)
            return *this;
        if (node_->firstChild()) {
            node_ = node_->firstChild();
            return *this;
        }
        while (node_) {
            if (node_->nextSibling()) {
                node_ = node_->nextSibling();
                return *this;
            }
            node_ = node_->parent();
        }
        return *this;
    }
    XTreeDocOrderIterator& operator--()      
    {
        if (!node_)
            return *this;
        if (node_->lastChild()) {
            node_ = node_->lastChild();
            return *this;
        }
        while (node_) {
            if (node_->prevSibling()) {
                node_ = node_->prevSibling();
                return *this;
            }
            node_ = node_->parent();
        }
        return *this;    
    }
private:
    T*  node_;
};

COMMON_NS_END

#endif // COMMON_XTREE_ITERATOR_H_
