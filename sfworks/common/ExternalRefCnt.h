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
    Implementation of external reference counting policy
 */
#ifndef EXTERNAL_REF_CNT_H_
#define EXTERNAL_REF_CNT_H_

#include "common/common_defs.h"
#include "common/common_types.h"
#include "common/ThreadingPolicies.h"
#include "common/DefaultSmallObject.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

/**
 * The external (non-intrusive) reference counting pointer
 */
template <class T, class TP = DEFAULT_THREADING<> >
class ExternalRefCnt : public PtrBase<T> {
public:
    /// Construct empty pointer
    ExternalRefCnt() {}

    /// Construct from a dumb pointer
    ExternalRefCnt(T* p)
    {
        p_ = p;
        counter_ = 0;
        initCounter();
    }

    /// Copy constructor
    ExternalRefCnt(const ExternalRefCnt<T>& p)
    {
        p_ = p.p_;
        counter_ = p.counter_;
        if (p.counter_)
            TP::atomicIncrement(*counter_);
    }

    /// Assignment from a dumb pointer
    ExternalRefCnt& operator=(T* p)
    {
        clear();
        p_ = p;
        initCounter();
        return *this;
    }

    /// Assignment from a refcounted pointer
    ExternalRefCnt& operator=(const ExternalRefCnt<T>& p)
    {
        if (p.counter_)
            TP::atomicIncrement(*p.counter_);
        clear();
        p_ = p;
        counter_ = p.counter_;
    }

    /// Clearing the reference-counted pointer
    void clear()
    {
        if (counter_ && TP::atomicDecrement(*counter_) <= 0) {
            delete p_;

            // For some reason operator delete does not accept pointers
            // to volatile locations; hence, const_cast required
            DefaultSmallObject::operator delete
                (const_cast<typename TP::IntType*>(counter_),
                sizeof(CounterType));
        }
        p_ = 0;
        counter_ = 0;
    }

    /// Returns the reference count
    int32 count()
    {
        return counter_ ? TP::atomicRead(*counter_) : 0;
    }

    /// Swap two smart pointers
    void swap(ExternalRefCnt<T>& p)
    {
        const T* tmp = p_;
        CounterType* tmpc = counter_;
        p_ = p.p_;
        counter_ = p.counter_;
        p.p_ = tmp;
        p.counter_ = tmpc;
    }

    /// The destructor
    ~ExternalRefCnt()
    {
        clear();
    }

private:
    typedef typename TP::VolatileIntType CounterType;

    /// Initialize counter to 1
    void initCounter()
    {
        if (p_) {
            counter_ = static_cast<CounterType*>(
                          DefaultSmallObject::operator new(sizeof(CounterType)));
            TP::atomicAssign(*counter_, 1);
        }
    }

    CounterType* counter_;      ///< The external counter
};

/**
 * The external (non-intrusive) reference counted pointer to constants
 */
template<class T, class TP = DEFAULT_THREADING<> >
class ConstExternalRefCnt : public ExternalRefCnt<const T, TP>
{
public:
    /// Null constructor
    ConstExternalRefCnt() {}

    /// Constructor from dumb pointers
    ConstExternalRefCnt(const T* p) : ExternalRefCnt<const T, TP>(p) {}

    /// Copy constructor
    ConstExternalRefCnt(const ExternalRefCnt<const T>& p) : ExternalRefCnt<const T, TP>(p) {}

    /// Constructor from non-const version
    ConstExternalRefCnt(const ExternalRefCnt<T>& p) : ExternalRefCnt<const T, TP>(p.pointer()) {}

    /// Assignment from non-const version
    ConstExternalRefCnt& operator=(const ExternalRefCnt<T, TP>& p)
    {
        this->ExternalRefCnt<const T, TP>::operator=(p.pointer());
        return *this;
    }

    /// Assignment from a const pointer
    ConstExternalRefCnt& operator=(const T* p)
    {
        this->ExternalRefCnt<const T, TP>::operator=(p);
        return *this;
    }
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif //  EXTERNAL_REF_CNT_H_
