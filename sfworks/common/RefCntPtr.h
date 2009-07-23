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
    Implementation of reference-counting smart pointer (RefCntPtr)
 */
#ifndef REF_CNT_PTR_H
#define REF_CNT_PTR_H

#include "common/common_defs.h"
#include "common/PtrBase.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

template <class T> class RefCntPtr;
template <class T, class CT> class CastedRefCntPtr;

/*! Pointer to object T which maintains (intrusive) object reference count.
 *  T must have RefCounted class as it's base class (or at least maintain
 *  same interface).
 *  T may be an incomplete type.
 *  This implementation allows polymorphic assignments (e.g. RefCntPtr<X>
 *  to RefCntPtr<Y> if X is a base of Y).
 */
template <class T>
  class RefCntPtr : public PtrBase<T> {
//                             g++ 2.95.3 does not work with it, 2.96 does work
#if !defined(_MSC_VER) && !defined(__ICL)  && !defined(__GNUC__)
      template <class T1> friend class RefCntPtr;
#endif
    using PtrBase<T>::p_;

public:
    RefCntPtr() {}

    RefCntPtr(T* ptr)
        : PtrBase<T>(ptr)
    {
        if(p_)
            p_->incRefCnt();
    }

    template <class T1> RefCntPtr(const RefCntPtr<T1>& other)
        : PtrBase<T>(other.p_)
    {
        if (p_)
            p_->incRefCnt();
    }

    RefCntPtr(const RefCntPtr& other)
        : PtrBase<T>(other.p_)
    {
        if (p_)
            p_->incRefCnt();
    }

    template <class T1>
      RefCntPtr(const CastedRefCntPtr<T1, T>& other)
        : RefCntPtr(other.pointer()) {}

    template <class T1>
      RefCntPtr& operator=(const RefCntPtr<T1>& other)
    {
        return assign(other.pointer());
    }

    template <class T1>
      RefCntPtr& operator=(const CastedRefCntPtr<T1, T>& other)
    {
        return assign(other.pointer());
    }

    RefCntPtr& operator=(T* other)
    {
        return assign(other);
    }

    RefCntPtr& operator=(const RefCntPtr& other)
    {
        return assign(other.p_);
    }

    void swap(RefCntPtr& other)
    {
        RefCntPtr tmp = other;
        other = *this;
        assign(tmp.p_);
    }

    void clear()
    {
        assign(0);
    }

    int count()
    {
        return p_ ? p_->getRefCnt() : 0;
    }

    ~RefCntPtr()
    {
        if (p_ && p_->decRefCnt() <= 0)
            delete p_;
    }

private:
    RefCntPtr& assign(T* other)
    {
        if (other)
            other->incRefCnt();
        if (p_ && p_->decRefCnt() <= 0)
            delete p_;
        p_ = other;
        return *this;
    }
};


/*! Flavor of RefCntPtr for const objects. It isn't a specialization
 *  of RefCntPtr because MSVC doesn't support partial specialization.
 */
template <class T>
  class ConstRefCntPtr : public RefCntPtr<T> {
public:
    typedef const T OriginalType;

    ConstRefCntPtr() {}

    // allow initialization from const pointer
    ConstRefCntPtr(const T* ptr)
      : RefCntPtr<T>(const_cast<T*>(ptr)) {}

    template <class T1>
      ConstRefCntPtr(const RefCntPtr<T1>& other)
        : RefCntPtr<T>(other) {}

    ConstRefCntPtr(const RefCntPtr<T>& other)
      : RefCntPtr<T>(other) {}

    template <class T1>
      ConstRefCntPtr& operator=(const RefCntPtr<T1>& other)
    {
        RefCntPtr<T>::operator=(other);
        return *this;
    }

    ConstRefCntPtr& operator=(const RefCntPtr<T>& other)
    {
        RefCntPtr<T>::operator=(other);
        return *this;
    }

    ConstRefCntPtr& operator=(const T* other)
    {
        RefCntPtr<T>::operator=(const_cast<T*>(other));
        return *this;
    }
    const T* pointer() const { return RefCntPtr<T>::pointer(); }
    const T* getPtr() const  { return RefCntPtr<T>::getPtr(); }
    const T* operator->() const { return RefCntPtr<T>::operator->(); }
    const T& operator*() const { return RefCntPtr<T>::operator*(); }
};

/*! A "casted" RefCntPtr - a flavor of RefCntPtr which keeps base
 *  type \a T, but casts return value to \a CT.
 *  This is useful in situations like:
 *  \code
 *  class Attr : public Node { ... };
 *  typedef RefCntPtr<Node> NodePtr;
 *  typedef CastedRefCntPtr<Node, Attr> AttrPtr;
 *  ...
 *  void foo(NodePtr& n);
 *  AttrPtr ap;
 *  ...
 *  foo(ap);
 *  \endcode
 */
template <class T, class CT>
  class CastedRefCntPtr : public RefCntPtr<T> {
public:
    typedef RefCntPtr<T> BasePtr;
    typedef CT OriginalType;

    CastedRefCntPtr() {}
    CastedRefCntPtr(CT* ptr) : BasePtr(ptr) {}

    template <class T1>
      CastedRefCntPtr(const RefCntPtr<T1>& other)  : BasePtr(other) {}

    CastedRefCntPtr(const RefCntPtr<CT>& other) : BasePtr(other) {}

    template <class T1>
      CastedRefCntPtr& operator=(const RefCntPtr<T1>& other)
    {
        BasePtr::operator=(other);
        return *this;
    }

    CastedRefCntPtr& operator=(const RefCntPtr<CT>& other)
    {
        BasePtr::operator=(other);
        return *this;
    }

    CastedRefCntPtr& operator=(CT* other)
    {
        BasePtr::operator=(other);
        return *this;
    }

    CT* pointer() const
    {
        return static_cast<CT*>(BasePtr::pointer());
    }

    CT* getPtr() const
    {
        return static_cast<CT*>(BasePtr::getPtr());
    }

    CT* operator->() const
    {
        return static_cast<CT*>(BasePtr::operator->());
    }

    CT& operator*() const
    {
        return static_cast<CT*>(BasePtr::operator*());
    }
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // REF_CNT_PTR_H
