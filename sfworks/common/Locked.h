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
    Implementation of thread lock wrapper for smart pointers
 */
#ifndef LOCKED_H_
#define LOCKED_H_

#include "common/common_defs.h"
#include "common/ThreadingPolicies.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

/*! A wrapper class for all smart pointers which allows temporary
    locking of the object while object is being dereferenced.
    Example:
        Locked<OwnerPtr<Foo> > fooPtr;
        fooPtr->foo();  // call is locked with DEFAULT_THREADING policy
    Please note that the pointer() function is intentionally left
    raw - there is no lock for pointer(), so you can make explicitly
    non-locking calls as
        fooPtr.pointer()->foo();  // no lock
 */
template <class T, class TP = DEFAULT_THREADING<> >
  class Locked : public T {
public:
    typedef typename T::OriginalType OriginalType;    // original type

private:
    class PointerProxy {
    public:
        typedef typename TP::Lock Locktype;

        OriginalType* pointer() const    { return ptr_.pointer(); }
        OriginalType* operator->() const { return ptr_.pointer(); }
        OriginalType& operator*() const  { return *ptr_.pointer(); }

        // bypass constness - for object-level locking purposes only
        PointerProxy(const T& ptr)
          : ptr_(ptr), lock_(const_cast<OriginalType&>(*ptr.pointer())) {}

    private:
        PointerProxy();
        const T& ptr_;
        Locktype lock_;
    };

public:
    PointerProxy operator->() const { return PointerProxy(*this); }
    PointerProxy operator*() const  { return PointerProxy(*this); }
    // pointer() operator is still raw

    Locked() {}
    Locked(OriginalType* other) : T(other) {}
    Locked(const T& other) : T(other) {}
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // LOCKED_H_
