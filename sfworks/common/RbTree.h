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
    Implements a Red-Black Tree ADT, according to T. H. Corman, C. E. Leiserson,
    and R. L. Rivest, "Introduction to Algorithms" 1990, MIT, chapter 14.
 */
#ifndef RB_TREE_H_
# define RB_TREE_H_

#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251 4291)
#endif

#include "common/common_defs.h"
#include "common/DefaultSmallObject.h"
#include <assert.h>

COMMON_NS_BEGIN

namespace RbTreePrivate {
    template <class V, class K, class KF, class Alloc> class RbTreeNode;
}

template <class V, class K = V> class RbTreeDefaultKeyFunc {
public:
    static const K& key(const V& val) { return val; }
};

/*! Generic red-black tree template.
    \li V stands for ValueType;
    \li K stands for KeyType,
    \li KF stands for KeyFunction (which extracts const K& from const V&)
    \li Alloc is an allocator (class which defines operators new and delete)
 */
template <class V, class K = V,
          class KF = RbTreeDefaultKeyFunc<V, K>,
          class Alloc = DefaultObjectAllocator>
class RbTree {
public:
    typedef RbTreePrivate::RbTreeNode<V, K, KF, Alloc> RbTreeNodeType;
    typedef V value_type;

    class iterator {
        public:
        iterator& operator++() {
            node_ = tree_->tree_successor(node_);
            return *this;
        }
        iterator& operator--() {
            node_ = tree_->tree_predecessor(node_);
            return *this;
        }
        bool operator==(const iterator& it) const {
            return it.node_ == node_;
        }
        bool operator!=(const iterator& it) const { return !(it == *this); }
        operator bool() const { return !node_; }
        V&        operator*() const  { return node_->value_; }
        V*        operator->() const { return &node_->value_; }
        V*        pointer() const    { return &node_->value_; }
        operator V*() const { return &node_->value_; }
        iterator(const RbTree& t, RbTreeNodeType* n) : tree_(&t), node_(n) {}
        iterator() : tree_(0), node_(0) {}
        friend class RbTree<V, K, KF, Alloc>;
    private:
        const RbTree*   tree_;
        RbTreeNodeType* node_;
    };

    class const_iterator : public iterator {
    public:
        const V& operator*()  const { return iterator::operator*(); }
        const V* operator->() const { return iterator::operator->(); }
        const V* pointer()    const { return iterator::pointer(); }
        operator const V*()   const { return iterator::pointer(); }
        const_iterator(const RbTree& t, RbTreeNodeType* n) : iterator(t, n) {}
        const_iterator(const iterator& it) : iterator(it) {}
        const_iterator() {}
    };
    /*! Find item by it's exact value */
    iterator find(const K& key) const;

    /*! Find item closest (less) to \a key (in sort order) */
    iterator findClosest(const K& key) const;

    /*! Insert item into tree. If overwrite is false, value
        won't be overwritten if keys are equal.
     */
    iterator insert(const V& value, bool overwrite = true);

    ///*! Allows to insert multiple items with the same key. */
    //iterator insertEqual(const V& value);

    /*! Erase item. */
    bool     erase(const iterator&);

    /* Clear tree. */
    void     clear() { delete root_; root_ = 0; }

    /*! return first (minimum) element in tree */
    iterator first() const { return iterator(*this, tree_minimum(root_)); }

    /*! return last (maximum) element in tree */
    iterator last() const  { return iterator(*this, tree_maximum(root_)); }

    // STL compat
    iterator begin() const { return first(); }
    iterator end() const   { return iterator(*this, 0); }

    RbTree() : root_(0) {}
    RbTree(const RbTree& other) : root_(0) { assign(other); }
    RbTree& operator=(const RbTree& other) {
        clear(); assign(other); return *this;
    }
    ~RbTree() { clear(); }

    friend class iterator;

private:
    enum Color { RED, BLACK };
    enum FindResult { EXACT, LEFT, RIGHT };
    void rotate_right(RbTreeNodeType*);
    void rotate_left(RbTreeNodeType*);
    void delete_fixup(RbTreeNodeType*, RbTreeNodeType*);
    void rebalance(RbTreeNodeType*);
    RbTreeNodeType* tree_successor(RbTreeNodeType*) const;
    RbTreeNodeType* tree_predecessor(RbTreeNodeType*) const;
    RbTreeNodeType* tree_minimum(RbTreeNodeType*) const;
    RbTreeNodeType* tree_maximum(RbTreeNodeType*) const;
    RbTreeNodeType* find_node(const K& kv, FindResult& res) const;
    void assign(const RbTree& other);

    RbTreeNodeType*  root_;
};

namespace RbTreePrivate {

template <class V, class K, class KF, class Alloc>
  class RbTreeNode {
public:
    void        color(char c) { color_ = c; }
    char        color() const { return color_; }

    RbTreeNode* parent() const { return parent_; }
    void        parent(RbTreeNode* n) { parent_ = n; }
    RbTreeNode* left() const { return left_; }
    void        left(RbTreeNode* n) { left_ = n; }
    RbTreeNode* right() const { return right_; }
    void        right(RbTreeNode* n)  { right_ = n; }
    const V&    value() const { return value_; }
    void        setValue(const V& v) { value_ = v; }

    RbTreeNode(const V& value)
      : parent_(0), left_(0), right_(0), value_(value), color_(0) {}
    ~RbTreeNode() { delete left_; delete right_; }

    USE_SMALLOBJECT_ALLOCATOR(Alloc);

    friend class RbTree<V, K, KF, Alloc>::iterator;

private:
    RbTreeNode();
    RbTreeNode*      parent_;
    RbTreeNode*      left_;
    RbTreeNode*      right_;
    V                value_;
    char             color_;
};

} // namespace RbTreePrivate

/////////////////////////////////////////////////////////////////

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::iterator
    RbTree<V, K, KF, Alloc>::find(const K& key) const
{
    FindResult result;
    RbTreeNodeType* current = find_node(key, result);
    if (current && result == EXACT)
        return iterator(*this, current);
    return end();
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::iterator
    RbTree<V, K, KF, Alloc>::findClosest(const K& key) const
{
    FindResult result;
    RbTreeNodeType* current = find_node(key, result);
    if (current)
        return iterator(*this, current);
    return end();
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::iterator
    RbTree<V, K, KF, Alloc>::insert(const V& value, bool ov)
{
    FindResult result;
    RbTreeNodeType* current = find_node(KF::key(value), result);
    if (current) {
        if (result == LEFT) {
            assert(current->right() == 0);
            current->right(new RbTreeNodeType(value));
            current->right()->parent(current);
            rebalance(current->right());
            root_->color(BLACK);
            return iterator(*this, current->right());
        } else if (result == RIGHT) {
            assert(current->left() == 0);
            current->left(new RbTreeNodeType(value));
            current->left()->parent(current);
            rebalance(current->left());
            root_->color(BLACK);
            return iterator(*this, current->left());
        } else { // EXACT
            if (ov)
                current->setValue(value);
            return iterator(*this, current);
        }
    } else {
        root_ = new RbTreeNodeType(value);
        root_->color(BLACK);
        return iterator(*this, root_);
    }
}

template <class V, class K, class KF, class Alloc>
  bool RbTree<V, K, KF, Alloc>::erase(const iterator& iter)
{
    RbTreeNodeType* x;
    RbTreeNodeType* z = iter.node_;
    if (0 == z)
        return false;
    RbTreeNodeType* y;
    if (z->left() && z->right())
        y = tree_successor(z);
    else
        y = z;
    x = (y->left()) ? y->left() : y->right();
    RbTreeNodeType* parent = y->parent();
    if (x)
        x->parent(parent);
    if (parent) {
        if (y == parent->left())
            parent->left(x);
        else
            parent->right(x);
    } else
        root_ = x;
    if (y != z)
        z->setValue(y->value());
    if (!y || y->color() == BLACK)
        delete_fixup(x, parent);
    y->parent(0);
    y->right(0);
    y->left(0);
    delete y;
    return true;
}

template <class V, class K, class KF, class Alloc>
  void RbTree<V, K, KF, Alloc>::rotate_right(RbTreeNodeType* x)
{
    assert(x && x->left());
    RbTreeNodeType* y = x->left();
    x->left(y->right());
    if (y->right())
        y->right()->parent(x);
    y->parent(x->parent());
    if (x->parent()) {
        if (x == x->parent()->right())
            x->parent()->right(y);
        else
            x->parent()->left(y);
    } else
        root_ = y;
    y->right(x);
    x->parent(y);
}

template <class V, class K, class KF, class Alloc>
  void RbTree<V, K, KF, Alloc>::rotate_left(RbTreeNodeType* x)
{
    assert(x && x->right());
    RbTreeNodeType* y = x->right();
    x->right(y->left());
    if (y->left())
        y->left()->parent(x);
    y->parent(x->parent());
    if (x->parent()) {
        if (x == x->parent()->left())
            x->parent()->left(y);
        else
            x->parent()->right(y);
    } else
        root_ = y;
    y->left(x);
    x->parent(y);
}

template <class V, class K, class KF, class Alloc>
  void RbTree<V, K, KF, Alloc>::delete_fixup(RbTreeNodeType* x,
                                             RbTreeNodeType* parent)
{
    while (x != this->root_ && (!x || x->color() == BLACK)) {
        if (x == parent->left ()) {
            RbTreeNodeType* w = parent->right();
            if (w && w->color() == RED) {
                w->color(BLACK);
                parent->color(RED);
                rotate_left(parent);
                w = parent->right();
            }
            if (w && (!w->left() || w->left()->color() == BLACK)
                && (!w->right() || w->right()->color() == BLACK)) {
                    w->color(RED);
                    x = parent;
                    parent = x->parent();
            } else {
                if (w && (!w->right() || w->right()->color() == BLACK)) {
                    if (w->left())
                        w->left()->color(BLACK);
                    w->color(RED);
                    rotate_right(w);
                    w = parent->right();
                }
                if (w) {
                    w->color (parent->color());
                    if (w->right())
                        w->right()->color(BLACK);
                }
                parent->color(BLACK);
                rotate_left(parent);
                x = root_;
            }
        } else {
            RbTreeNodeType* w = parent->left();
            if (w && w->color() == RED) {
                w->color(BLACK);
                parent->color(RED);
                rotate_right(parent);
                w = parent->left();
            }
            if (w && (!w->left() || w->left()->color() == BLACK)
                && (!w->right() || w->right()->color() == BLACK)) {
                    w->color(RED);
                    x = parent;
                    parent = x->parent ();
            } else {
                if (w && (!w->left() || w->left()->color() == BLACK)) {
                    w->color(RED);
                    if (w->right())
                        w->right()->color (BLACK);
                    rotate_left(w);
                    w = parent->left();
                }
                if (w) {
                  w->color(parent->color());
                  if (w->left())
                    w->left()->color(BLACK);
                }
                parent->color(BLACK);
                rotate_right(parent);
                x = root_;
            }
        }
    }
    if (x)
        x->color(BLACK);
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::RbTreeNodeType*
    RbTree<V, K, KF, Alloc>::find_node(const K& key, FindResult& result) const
{
    RbTreeNodeType* current = root_;
    while (current) {
        if (KF::key(current->value()) < key) {
            if (current->right()) {
                current = current->right();
            } else {
                result = LEFT;
                break;
            }
        } else if (key < KF::key(current->value())) {
            if (current->left()) {
                current = current->left();
            } else {
                result = RIGHT;
                break;
            }
        } else {
            result = EXACT;
            break;
        }
    }
    return current;
}

template <class V, class K, class KF, class Alloc>
  void RbTree<V, K, KF, Alloc>::rebalance(RbTreeNodeType* x)
{
    RbTreeNodeType* y = 0;
    while (x && x->parent() && x->parent()->color() == RED) {
        assert(x->parent()->parent());
        if (x->parent() == x->parent()->parent()->left()) {
            y = x->parent()->parent()->right();
            if (y && y->color() == RED) {
                x->parent()->color(BLACK);
                y->color(BLACK);
                x->parent()->parent()->color(RED);
                x = x->parent()->parent();
            } else {
                if (x == x->parent()->right()) {
                    x = x->parent();
                    rotate_left(x);
                }
                x->parent()->color(BLACK);
                x->parent()->parent()->color(RED);
                rotate_right(x->parent()->parent());
            }
        } else {
            y = x->parent()->parent()->left();
            if (y && y->color() == RED) {
                x->parent()->color(BLACK);
                y->color(BLACK);
                x->parent()->parent()->color(RED);
                x = x->parent()->parent();
            } else {
                if (x == x->parent()->left()) {
                    x = x->parent();
                    rotate_right(x);
                }
                x->parent()->color(BLACK);
                x->parent()->parent()->color(RED);
                rotate_left(x->parent()->parent());
            }
        }
    }
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::RbTreeNodeType*
    RbTree<V, K, KF, Alloc>::tree_successor(RbTreeNodeType* x) const
{
    if (x->right())
        return tree_minimum(x->right());
    RbTreeNodeType* y = x->parent();
    while (y && (x == y->right())) {
        x = y;
        y = y->parent();
    }
    return y;
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::RbTreeNodeType*
    RbTree<V, K, KF, Alloc>::tree_predecessor(RbTreeNodeType* x) const
{
    if (x->left())
        return tree_maximum(x->left());
    RbTreeNodeType* y = x->parent();
    while (y && (x == y->left())) {
        x = y;
        y = y->parent();
    }
    return y;
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::RbTreeNodeType*
    RbTree<V, K, KF, Alloc>::tree_minimum(RbTreeNodeType* x) const
{
    while (x && x->left())
        x = x->left();
    return x;
}

template <class V, class K, class KF, class Alloc>
  typename RbTree<V, K, KF, Alloc>::RbTreeNodeType*
    RbTree<V, K, KF, Alloc>::tree_maximum(RbTreeNodeType* x) const
{
    while (x && x->right())
        x = x->right();
    return x;
}

template <class V, class K, class KF, class Alloc>
  void RbTree<V, K, KF, Alloc>::assign(const RbTree& other)
{
    for(const_iterator iter = other.first(); iter; ++iter)
        insert(*iter);
}

COMMON_NS_END

#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // RB_TREE_H_
