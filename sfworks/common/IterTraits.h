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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *  defines InpIterTraits class to provide information about input iterator
 *  properties in absence of std conforming iterators
 */

#ifndef ITER_TRAITS_H_
#define ITER_TRAITS_H_

#include <iterator>

#if !defined(__GNUG__) && (defined(_MSC_VER) && (_MSC_VER < 1300))
//! defined when standard library doesn't have std conforming iterator traits
# define NO_ITERATOR_TRAITS
#endif

#ifdef NO_ITERATOR_TRAITS
# define REVERSE_ITERATOR(a,b) std::reverse_iterator<a,b>
#else
# define REVERSE_ITERATOR(a,b) std::reverse_iterator<a>
#endif

namespace StringPrivate {

typedef unsigned int size_type;

struct InpIterTag {};
struct RndIterTag {};
struct MBNIterTag : RndIterTag {};
struct PtrIterTag : MBNIterTag {};
struct IntIterTag : MBNIterTag {};

template<typename It> struct InpIterTraits {
#ifdef NO_ITERATOR_TRAITS
    typedef typename It::pointer    pointer;
    typedef typename It::value_type value_type;
    typedef std::input_iterator_tag iterator_tag;
#else
    typedef typename std::iterator_traits<It>::pointer           pointer;
    typedef typename std::iterator_traits<It>::value_type        value_type;
    typedef typename std::iterator_traits<It>::iterator_category iterator_tag;
#endif
};

template<typename Tag> struct IterTagSelect {
    typedef InpIterTag iterator_tag;
};

#ifdef NO_ITERATOR_TRAITS
template<typename T> struct InpIterTraits<T*> {
    typedef T*          pointer;
    typedef T           value_type;
    typedef PtrIterTag  iterator_tag;
};

template<typename T> struct InpIterTraits<const T*> {
    typedef const T*    pointer;
    typedef T           value_type;
    typedef PtrIterTag  iterator_tag;
};

template<> struct IterTagSelect<PtrIterTag> {
    typedef PtrIterTag iterator_tag;
};
#endif

template<> struct InpIterTraits<size_type> {
    typedef IntIterTag iterator_tag;
};

template<> struct InpIterTraits<unsigned long> {
    typedef IntIterTag iterator_tag;
};

template<> struct IterTagSelect<IntIterTag> {
    typedef IntIterTag iterator_tag;
};

template<> struct IterTagSelect<std::random_access_iterator_tag> {
    typedef RndIterTag iterator_tag;
};

template<typename It>
typename IterTagSelect<typename InpIterTraits<It>::iterator_tag>::iterator_tag
IterTag(const It&)
{
    typedef typename InpIterTraits<It>::iterator_tag Tag;
    typedef typename IterTagSelect<Tag>::iterator_tag IterTag;
    return IterTag();
}

} // end of namespace StringPrivate

#endif // ITER_TRAITS_H_
