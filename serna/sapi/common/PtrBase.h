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
#ifndef SAPI_PTR_BASE_H_
#define SAPI_PTR_BASE_H_

#include "sapi/sapi_defs.h"

namespace SernaApi {
///Smart pointer
/**
 * Base class for smart pointers
 *
 * Defines functions which are common for all smart Ptr's (comparsions etc).
 */
template <class T> class PtrBase {
public:
    typedef T OriginalType;

    /// Returns raw pointer to the object
    T* pointer() const { return p_; }
    T* getPtr()  const { return p_; } // for compatibility
    T* get() const { return p_; } // for compatibility w/auto_ptr

    /*! Extract pointer to the kept object and release it. */
    T *release()
    {
        T *tem = p_;
        p_ = 0;
        return tem;
    }
    /// Dereference operator ->
    T* operator->() const { return p_; }

    /// Dereference operator *
    T &operator*() const  { return *p_; }

        /// Check whether owned pointer is NULL
    bool isNull() const
    {
        return 0 == p_;
    }

    /// Synonymous to isNull()
    bool operator!() const
    {
        return 0 == p_;
    }

    /// Checks if pointer is non-NULL
    operator bool() const
    {
        return 0 != p_;
    }

    /// Checks that the current pointer is less than rhs
    bool operator<(const PtrBase& rhs) const
    {
        return p_ < rhs.pointer();
    }

    /// Checks that the current pointer is less than rhs
    bool operator<(const T* rhs) const
    {
        return p_ < rhs;
    }

    /// Checks that the current pointer is greater than rhs
    bool operator>(const PtrBase& rhs) const
    {
        return p_ > rhs.pointer();
    }

    /// Checks that the current pointer is greater than rhs
    bool operator>(const T* rhs) const
    {
        return p_ > rhs;
    }

    /// Checks that the current pointer is less or equal than rhs
    bool operator<=(const PtrBase& rhs) const
    {
        return p_ <= rhs.pointer();
    }

    /// Checks that the current pointer is less or equal than rhs
    bool operator<=(const T* rhs) const
    {
        return p_ <= rhs;
    }

    /// Checks that the current pointer is greater or equal than rhs
    bool operator>=(const PtrBase& rhs) const
    {
        return p_ >= rhs.pointer();
    }

    /// Checks that the current pointer is greater or equal than rhs
    bool operator>=(const T* rhs) const
    {
        return p_ >= rhs;
    }

    /// Construct null pointer
    PtrBase() : p_(0) {}

    /// Construct from a dumb pointer
    PtrBase(T* p) : p_(p) {}

protected:
    T*  p_;
};

/// Compare pointers for equality
template <class T, class T1>
  inline bool operator==(const PtrBase<T>& lhs, const T1* rhs)
{
    return lhs.pointer() == rhs;
}

/// Compare pointers for equality
template <class T, class T1>
  inline bool operator==(const T* lhs, const PtrBase<T1>& rhs)
{
    return rhs == lhs;
}

/// Compare pointers for inequality
template <class T, class T1>
  inline bool operator!=(const PtrBase<T>& lhs, const T1* rhs)
{
    return !(lhs == rhs);
}

/// Compare pointers for inequality
template <class T, class T1>
  inline  bool operator!=(const T* lhs, const PtrBase<T1>& rhs)
{
    return rhs != lhs;
}

/// Compare pointers for equality
template <class T, class T1>
  inline bool operator==(const PtrBase<T>& lhs, const PtrBase<T1>& rhs)
{
    return lhs.pointer() == rhs.pointer();
}

/// Compare pointers for inequality
template <class T, class T1>
  inline bool operator!=(const PtrBase<T>& lhs, const PtrBase<T1>& rhs)
{
    return !(lhs == rhs);
}

//
// The following ugly things are ambiguity busters for MSVC
// for comparsions like 0 == ptr, ptr == 0
//
template <class T>
  inline bool operator==(const int x, const PtrBase<T>& rhs)
{
    return (T*)x == rhs.pointer();
}

template <class T>
  inline bool operator!=(const int x, const PtrBase<T>& rhs)
{
    return (T*)x != rhs.pointer();
}

template <class T>
  inline bool operator==(const PtrBase<T>& lhs, const int x)
{
    return (T*)x == lhs.pointer();
}

template <class T>
  inline bool operator!=(const PtrBase<T>& lhs, const int x)
{
    return (T*)x != lhs.pointer();
}

} // namespace SernaApi

#endif // SAPI_PTR_BASE_H_

