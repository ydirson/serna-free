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

#ifndef _COMMON_XTREENODE_DIFFUSESEARCH_H_
#define _COMMON_XTREENODE_DIFFUSESEARCH_H_

#include "common/common_defs.h"
#include "common/XTreeIterator.h"

namespace Common {

///////////////////////////////////////////////////////////////////////////

template <class T> 
class XTreeIterator {
public:
    XTreeIterator(const XTreeIterator<T>& i) 
        : node_(i.node()), before_(i.before()) {}
    XTreeIterator(T* node = 0, T* before = 0) 
        : node_(node), before_(before) {}

    virtual bool        isNull() const { return !node_; }
    T*                  node() const { return node_; }
    T*                  before() const { return before_; }
    
    XTreeIterator&      operator++();
    XTreeIterator&      operator--();    

    virtual ~XTreeIterator() {}

private:
    T*                  node_;
    T*                  before_;
};

template <class T> 
XTreeIterator<T>& XTreeIterator<T>::operator++()
{
    if (isNull())
        return *this;
    if (before_) {
        node_ = before_;
        before_ = node_->firstChild();
    }
    else {
        before_ = node_->nextSibling();
        node_ = node_->parent();
    }
    return *this;    
}

template <class T> 
XTreeIterator<T>& XTreeIterator<T>::operator--()
{
    if (isNull())
        return *this;
    if (before_ && before_->prevSibling()) {
        node_ = before_->prevSibling();
        before_ = node_->lastChild();
    }
    else {
        before_ = node_;
        node_ = node_->parent();
    }
    return *this;    
}

///////////////////////////////////////////////////////////////////////////

template <class T, class Pred>
XTreeIterator<T> find_diffuse(const XTreeIterator<T>& pos, Pred pred, 
                              bool preferPrevNode = false)
{
    if (pos.isNull())
        return XTreeIterator<T>();
    
    XTreeIterator<T> next(pos);
    XTreeIterator<T> prev(pos);

    while (!next.isNull() || !prev.isNull()) {
        if (preferPrevNode) {
            if (pred(prev))
                return prev;
            if (pred(next))
                return next;
        }
        else {
            if (pred(next))
                return next;
            if (pred(prev))
                return prev;
        }
        ++next;
        --prev;
    }
    return XTreeIterator<T>();
}

///////////////////////////////////////////////////////////////////////////

template <class T, class Pred>
XTreeIterator<T> find_forward(const XTreeIterator<T>& pos, Pred pred,
                              bool isForward = true)
{
    if (pos.isNull())
        return XTreeIterator<T>();
    
    for (XTreeIterator<T> next(pos); !next.isNull(); 
         (isForward) ? ++next : --next) {
        if (pred(next))
            return next;
    }
    return XTreeIterator<T>();
}

///////////////////////////////////////////////////////////////////////////

#define FIND_NEXT_NODE if (next.node() && next.node() != node) {\
    if (pred(next.node()))\
        return maker.make(next.node(), 1);\
    ++next;\
}

#define FIND_PREV_NODE if (prev.node() && prev.node() != node) {\
    if (pred(prev.node()))\
        return maker.make(prev.node(), -1);\
    --prev;\
}

template <class T, class Pred, class Ret, class RetMaker>
Ret find_diffuse(const T* start, const T* before, Pred pred, RetMaker maker,
                 bool preferPrevNode = false)
{
    if (0 == start)
        return 0;
    //std::cerr << "start:" << start << " before:" << before << std::endl;

    const T* node = (before) ? before->parent() : start;

    typedef XTreeWalkIterator<const T> Iterator;

    const T* next_start = node;
    bool next_down = true;
    const T* prev_start = node;
    bool prev_down = true;

    if (before) {
        next_start = before;
        prev_start = before->prevSibling();
        if (0 == prev_start) {
            prev_start = node;
            prev_down = false;
        }
    }
    else {
        prev_start = node->lastChild();
        if (0 == prev_start)
            prev_start = node;
        next_start = node;
        next_down = false;
    }
    //std::cerr << " next_start:" << next_start << " next_down:" << next_down
    //          << " prev_start:" << prev_start << " prev_down:" << prev_down
    //          << std::endl;

    Iterator next(next_start, next_down);
    Iterator prev(prev_start, prev_down);

    while (node) {
        //std::cerr << "node:" << std::endl;
        //node->dump();
        while ((next.node() && next.node() != node) ||
               (prev.node() && prev.node() != node)) {

            if (preferPrevNode) {
                FIND_PREV_NODE;
                FIND_NEXT_NODE;
            }
            else {
                FIND_NEXT_NODE;
                FIND_PREV_NODE;
            }
            
            
            /*
            if (next.node() && next.node() != node) {
                //std::cerr << "next:" << next.node() << std::endl;
                if (pred(next.node()))
                    return maker.make(next.node(), 1);
                ++next;
            }
            if (prev.node() && prev.node() != node) {
                //std::cerr << "prev:" << prev.node() << std::endl;
                if (pred(prev.node()))
                    return maker.make(prev.node(), -1);
                --prev;
            }
            */
        }
        if (next.node() && next.node() == node) {
            //std::cerr << "next == node:" << next.node() << std::endl;
            if (pred(next.node()))
                return maker.make(next.node(), 0);
        }
        else
            if (prev.node() && prev.node() == node) {
                //std::cerr << "prev == node:" << prev.node() << std::endl;
                if (pred(prev.node()))
                    return maker.make(prev.node(), 0);
            }
        node = node->parent();
    }
    return 0;
}

template <class T, class Pred>
const T* find_forward(const T* node, Pred pred)
{
    if (0 == node)
        return 0;
    while (node->firstChild())
        node = node->firstChild();

    typedef XTreeDfsIterator<const T> Iterator;

    Iterator next(node);

    while (node) {
        while ((next.node() && next.node() != node)) {
            if (next.node() && next.node() != node) {
                if (pred(next.node()))
                    return next.node();
                ++next;
            }
        }
        if (next.node() && next.node() == node) {
            if (pred(next.node()))
                return next.node();
        }
        node = node->parent();
    }
    return 0;
}

template <class T, class Pred>
const T* find_diffuse(const T* node, Pred pred)
{
    if (0 == node)
        return 0;
    while (node->firstChild())
        node = node->firstChild();

    typedef XTreeDfsIterator<const T> Iterator;

    Iterator next(node);
    Iterator prev(node);

    while (node) {
        while ((next.node() && next.node() != node) ||
               (prev.node() && prev.node() != node)) {

            if (next.node() && next.node() != node) {
                if (pred(next.node()))
                    return next.node();
                ++next;
            }
            if (prev.node() && prev.node() != node) {
                if (pred(prev.node()))
                    return prev.node();
                --prev;
            }
        }
        if (next.node() && next.node() == node) {
            if (pred(next.node()))
                return next.node();
        }
        else
            if (prev.node() && prev.node() == node) {
                if (pred(prev.node()))
                    return prev.node();
            }
        node = node->parent();
    }
    return 0;
}

}

#endif // COMMON_XTREENODE_DIFFUSESEARCH_H_

