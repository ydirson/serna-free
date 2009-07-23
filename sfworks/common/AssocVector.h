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
    Implementation of associative vector (a vector of pairs with binary search by key
    value), which is more effective than std::map in situations when lookups are more
    frequent than inserts. Implementation is derived from Loki library.
 */
#ifndef ASSOCVECTOR_H_
#define ASSOCVECTOR_H_

#include "common/common_defs.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <utility>

COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN


namespace AssocVectorPrivate
{
    template <class Value, class C> class AssocVectorCompare : public C {
    private:
        typedef std::pair<typename C::first_argument_type, Value> Data;
        typedef typename C::first_argument_type first_argument_type;

    public:
        AssocVectorCompare() {}
        AssocVectorCompare(const C& src) : C(src) {}

        bool operator()(const first_argument_type& lhs,
                        const first_argument_type& rhs) const
            { return C::operator()(lhs, rhs); }

        bool operator()(const Data& lhs, const Data& rhs) const
            { return operator()(lhs.first, rhs.first); }

        bool operator()(const Data& lhs,
                        const first_argument_type& rhs) const
            { return operator()(lhs.first, rhs); }

        bool operator()(const first_argument_type& lhs,
                        const Data& rhs) const
            { return operator()(lhs, rhs.first); }
    };
} // namespace AssocVectorPrivate

/*! An associative vector built as a syntactic drop-in replacement
    for std::map. BEWARE: AssocVector doesn't respect all map's guarantees,
    the most important being:
    \li iterators are invalidated by insert and erase operations
    \li the complexity of insert/erase is O(N) not O(log N)
    \li value_type is std::pair<K, V> not std::pair<const K, V>
    \li iterators are random
*/
template
<
    class K,
    class V,
    class C = std::less<K>,
    class A = std::allocator< std::pair<K, V> >
>
  class AssocVector
    : private std::vector< std::pair<K, V>, A >,
      private AssocVectorPrivate::AssocVectorCompare<V, C>
{
    typedef std::vector<std::pair<K, V>, A> Base;
    typedef AssocVectorPrivate::AssocVectorCompare<V, C> MyCompare;

public:
    typedef K key_type;
    typedef V mapped_type;
    typedef typename Base::value_type value_type;

    typedef C key_compare;
    typedef A allocator_type;
    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::size_type size_type;
    typedef typename Base::difference_type difference_type;
    typedef typename A::pointer pointer;
    typedef typename A::const_pointer const_pointer;
    typedef typename Base::reverse_iterator reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;

    class value_compare
        : public std::binary_function<value_type, value_type, bool>,
          private key_compare
    {
        friend class AssocVector;

    protected:
        value_compare(key_compare pred) : key_compare(pred) {}

    public:
        bool operator()(const value_type& lhs, const value_type& rhs) const
        { return key_compare::operator()(lhs.first, rhs.first); }
    };

    // 23.3.1.1 construct/copy/destroy

    explicit AssocVector(const key_compare& comp = key_compare(),
        const A& alloc = A())
      : Base(alloc), MyCompare(comp) {}

    template <class InputIterator>
      AssocVector(InputIterator first, InputIterator last,
                  const key_compare& comp = key_compare(),
                  const A& alloc = A())
        : Base(first, last, alloc), MyCompare(comp)
    {
        MyCompare& me = *this;
        std::sort(begin(), end(), me);
    }

    AssocVector& operator=(const AssocVector& rhs)
        { AssocVector(rhs).swap(*this); return *this; }

    // iterators:
    // The following are here because MWCW gets 'using' wrong
    iterator begin() { return Base::begin(); }
    const_iterator begin() const { return Base::begin(); }
    iterator end() { return Base::end(); }
    const_iterator end() const { return Base::end(); }
    reverse_iterator rbegin() { return Base::rbegin(); }
    const_reverse_iterator rbegin() const { return Base::rbegin(); }
    reverse_iterator rend() { return Base::rend(); }
    const_reverse_iterator rend() const { return Base::rend(); }

    // capacity:
    bool empty() const { return Base::empty(); }
    size_type size() const { return Base::size(); }
    size_type max_size() { return Base::max_size(); }

    // 23.3.1.2 element access:
    mapped_type& operator[](const key_type& key)
        { return insert(value_type(key, mapped_type())).first->second; }

    // modifiers:
    std::pair<iterator, bool> insert(const value_type& val)
    {
        bool found(true);
        iterator i(lower_bound(val.first));

        if (i == end() || operator()(val.first, i->first)) {
            i = Base::insert(i, val);
            found = false;
        }
        return std::make_pair(i, !found);
    }
    iterator insert(iterator pos, const value_type& val)
    {
        if (pos != end() && operator()(*pos, val) &&
            (pos == end() - 1 ||
                !operator()(val, pos[1]) &&
                    operator()(pos[1], val))) {
            return Base::insert(pos, val);
        }
        return insert(val).first;
    }

    template <class InputIterator>
      iterator insert(InputIterator first, InputIterator last)
        { for (; first != last; ++first) insert(*first); }

    void erase(iterator pos) { Base::erase(pos); }

    size_type erase(const key_type& k)
    {
        iterator i(find(k));
        if (i == end())
            return 0;
        erase(i);
        return 1;
    }

    void erase(iterator first, iterator last)
        { Base::erase(first, last); }

    void swap(AssocVector& other)
    {
        using namespace std;
        Base::swap(other);
        MyCompare& me = *this;
        MyCompare& rhs = other;
        std::swap(me, rhs);
    }

    void clear() { Base::clear(); }

    // observers:
    key_compare key_comp() const { return *this; }

    value_compare value_comp() const
    {
        const key_compare& comp = *this;
        return value_compare(comp);
    }

    // 23.3.1.3 map operations:
    iterator find(const key_type& k)
    {
        iterator i(lower_bound(k));
        if (i != end() && operator()(k, i->first))
            i = end();
        return i;
    }

    const_iterator find(const key_type& k) const
    {
        const_iterator i(lower_bound(k));
        if (i != end() && operator()(k, i->first))
            i = end();
        return i;
    }

    size_type count(const key_type& k) const
        { return find(k) != end(); }

    iterator lower_bound(const key_type& k)
    {
        MyCompare& me = *this;
        return std::lower_bound(begin(), end(), k, me);
    }

    const_iterator lower_bound(const key_type& k) const
    {
        const MyCompare& me = *this;
        return std::lower_bound(begin(), end(), k, me);
    }

    iterator upper_bound(const key_type& k)
    {
        MyCompare& me = *this;
        return std::upper_bound(begin(), end(), k, me);
    }

    const_iterator upper_bound(const key_type& k) const
    {
        const MyCompare& me = *this;
        return std::upper_bound(begin(), end(), k, me);
    }

    std::pair<iterator, iterator> equal_range(const key_type& k)
    {
        MyCompare& me = *this;
        return std::equal_range(begin(), end(), k, me);
    }

    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& k) const
    {
        const MyCompare& me = *this;
        return std::equal_range(begin(), end(), k, me);
    }

    friend bool operator==(const AssocVector& lhs, const AssocVector& rhs)
    {
        const Base& me = lhs;
        return me == rhs;
    }

    bool operator<(const AssocVector& rhs) const
    {
        const Base& me = *this;
        const Base& yo = rhs;
        return me < yo;
    }

    friend bool operator!=(const AssocVector& lhs, const AssocVector& rhs)
        { return !(lhs == rhs); }

    friend bool operator>(const AssocVector& lhs, const AssocVector& rhs)
        { return rhs < lhs; }

    friend bool operator>=(const AssocVector& lhs, const AssocVector& rhs)
        { return !(lhs < rhs); }

    friend bool operator<=(const AssocVector& lhs, const AssocVector& rhs)
        { return !(rhs < lhs); }
};

// specialized algorithms:
template <class K, class V, class C, class A>
  void swap(AssocVector<K, V, C, A>& lhs, AssocVector<K, V, C, A>& rhs)
    { lhs.swap(rhs); }

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // ASSOCVECTOR_H_
