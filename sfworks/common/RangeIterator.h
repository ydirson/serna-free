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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef RANGE_ITERATOR_H_
#define RANGE_ITERATOR_H_

#include "common/StringUtils.h"

namespace Common {

template <typename Iter, class Pred, bool noEmptyRanges> struct SepPolicy;

template <typename Iter, class Pred> struct SepPolicy<Iter, Pred, true> {
    SepPolicy(Pred pr) : pr_(pr) {}
    typedef typename Common::Range<Iter> RangeType;
    RangeType operator()(const RangeType& r)
    {
        return find_first_range_not_of(r, pr_);
    }
    RangeType operator()(Iter begin, Iter end)
    {
        return find_first_range_not_of(begin, end, pr_);
    }
private:
    Pred pr_;
};

template <typename Iter, class Pred> struct SepPolicy<Iter, Pred, false> {
    SepPolicy(Pred pr) : pr_(pr) {}
    typedef typename Common::Range<Iter> RangeType;
    RangeType operator()(const RangeType& r)
    {
        return find_first_range_between(r, pr_);
    }
    RangeType operator()(Iter begin, Iter end)
    {
        return find_first_range_between(begin, end, pr_);
    }
private:
    Pred pr_;
};

template
<
    typename Iter,
    class Pred,
    class SplitPolicy = SepPolicy<Iter, Pred, true>
>
class RangeIterator {
public:
    typedef Pred PredicateType;
    typedef Iter IteratorType;
    typedef typename Common::Range<Iter> RangeType;
    typedef SplitPolicy SplitPolicyType;
    //!
    RangeIterator(Iter begin, Iter end)
     :  sp_(Pred()), range_(sp_(begin, end)), end_(end) {}
    RangeIterator(Iter begin, Iter end, Pred pr)
     :  sp_(pr), range_(sp_(begin, end)), end_(end) {}
    explicit RangeIterator(const RangeType& range)
     :  sp_(Pred()), range_(sp_(range)), end_(range.end()) {}
    RangeIterator(const RangeType& range, Pred pr)
     :  sp_(pr), range_(sp_(range)), end_(range.end()) {}

    template<typename OtherIter> bool operator == (OtherIter end) const
    {
        return &(*range_.begin()) == &*end;
    }
    template<typename OtherIter> bool operator != (OtherIter end) const
    {
        return &(*range_.begin()) != &*end;
    }

    const RangeType* operator->() const { return &range_; }
    const RangeType& operator*() const { return range_; }

    RangeIterator& operator++()
    {
        if (range_.second != end_)
            range_ = sp_(++range_.second, end_);
        else
            range_.first = end_;
        return *this;
    }
    //!
    RangeIterator& operator=(const RangeType& r)
    {
        range_ = sp_(r);
        end_ = r.end();
        return *this;
    }
private:
    SplitPolicyType sp_;
    RangeType       range_;
    Iter            end_;
};

template<typename Iter, typename OtherIter, class Pred, class SP> inline bool
operator==(OtherIter lhs, const RangeIterator<Iter, Pred, SP>& rhs)
{
    return rhs == lhs;
}

template<typename Iter, typename OtherIter, class Pred, class SP> inline bool
operator!=(OtherIter lhs, const RangeIterator<Iter, Pred, SP>& rhs)
{
    return rhs != lhs;
}

template <
    typename CharType,
    bool skipAdjSeps = true,
    class Pred = IsChar<CharType>
>
struct SepRangeIter
    : public RangeIterator<const CharType*,
                           Pred,
                           SepPolicy<const CharType*, Pred, skipAdjSeps> > {
    typedef SepPolicy<const CharType*, Pred, skipAdjSeps> SepPolicyType;
    typedef RangeIterator<const CharType*, Pred, SepPolicyType> BaseType;
    typedef typename BaseType::IteratorType Iter;
    typedef typename BaseType::RangeType RangeType;
    //!
    SepRangeIter(Iter begin, Iter end) : BaseType(begin, end, Pred(' ')) {}
    SepRangeIter(Iter begin, Iter end, Pred pr) : BaseType(begin, end, pr) {}
    SepRangeIter(Iter begin, Iter end, CharType sep)
     :  BaseType(begin, end, Pred(sep)) {}
    explicit SepRangeIter(const RangeType& r) : BaseType(r, Pred(' ')) {}
    SepRangeIter(const RangeType& r, CharType sep)
     :  BaseType(r, Pred(sep)) {}
    SepRangeIter& operator=(const RangeType& r)
    {
        BaseType::operator=(r);
        return *this;
    }
};

template <
    typename CharType,
    bool skipAdjSeps = true,
    class Pred = InChars<CharType>
>
struct SetRangeIter
    : public RangeIterator<const CharType*,
                           Pred,
                           SepPolicy<const CharType*, Pred, skipAdjSeps> > {
    typedef SepPolicy<const CharType*, Pred, skipAdjSeps> SepPolicyType;
    typedef RangeIterator<const CharType*, Pred, SepPolicyType> BaseType;
    typedef typename BaseType::IteratorType Iter;
    typedef Pred PredicateType;
    typedef typename PredicateType::CharSetType CharSetType;
    typedef typename BaseType::RangeType RangeType;
    //!
    SetRangeIter(Iter begin, Iter end) : BaseType(begin, end) {}
    SetRangeIter(Iter begin, Iter end, Pred pr) : BaseType(begin, end, pr) {}
    SetRangeIter(Iter begin, Iter end, const typename Pred::CharSetType* pset,
                 unsigned len) : BaseType(begin, end, Pred(pset, len)) {}
    template <unsigned N>
    SetRangeIter(Iter begin, Iter end, const CharSetType (&l)[N])
     :  BaseType(begin, end, PredicateType(l)) {}
    //!
    explicit SetRangeIter(const RangeType& r) : BaseType(r) {}
    SetRangeIter(const RangeType& r, Pred pr) : BaseType(r, pr) {}
    SetRangeIter(const RangeType& r, const typename Pred::CharSetType* pset,
                 unsigned len) : BaseType(r, Pred(pset, len)) {}
    template <unsigned N>
    SetRangeIter(const RangeType& r, const CharSetType (&l)[N])
     :  BaseType(r, PredicateType(l)) {}
    SetRangeIter& operator=(const RangeType& r)
    {
        BaseType::operator=(r);
        return *this;
    }
};

template <typename CharType, bool skipAdjSeps = true>
struct WSRangeIter
    : public RangeIterator<const CharType*,
                           IsSpace<CharType>,
                           SepPolicy<const CharType*,
                                     IsSpace<CharType>,
                                     skipAdjSeps> > {
    typedef SepPolicy<const CharType*, IsSpace<CharType>, skipAdjSeps>
            SepPolicyType;
    typedef RangeIterator<const CharType*, IsSpace<CharType>, SepPolicyType>
            BaseType;
    typedef typename BaseType::IteratorType Iter;
    typedef typename BaseType::PredicateType PredicateType;
    typedef typename BaseType::RangeType RangeType;
    //!
    WSRangeIter(Iter begin, Iter end) : BaseType(begin, end) {}
    explicit WSRangeIter(const RangeType& r) : BaseType(r) {}
    WSRangeIter& operator=(const RangeType& r)
    {
        BaseType::operator=(r);
        return *this;
    }
};

}

#endif

