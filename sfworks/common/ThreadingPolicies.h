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
    This file contains lightweight, policy-based threading library
    Windows part is derived from free Loki C++ library

    Template classes defined below are not intended for general use;
    rather they should be used for parametrization of other template
    classes who needs threading policies (like Singleton, Ptr, etc).
 */
#ifndef THREADING_POLICIES_H_
#define THREADING_POLICIES_H_

#include "common/common_defs.h"
#include "common/ThreadMutex.h"
#include <QAtomicInt>

// Specifies default threading model

#ifndef DEFAULT_THREADING
# ifdef MULTI_THREADED
#  define DEFAULT_THREADING    COMMON_NS::ObjectLevelLockable
#  define DEFAULT_THREADING_CL COMMON_NS::ClassLevelLockable
# else
#  define DEFAULT_THREADING    COMMON_NS::SingleThreaded
#  define DEFAULT_THREADING_CL COMMON_NS::SingleThreaded
# endif
#endif // DEFAULT_THREADING

COMMON_NS_BEGIN

class ThreadAtomicInt : public QAtomicInt {
public:
    ThreadAtomicInt(int v) : QAtomicInt(v) {}
    ThreadAtomicInt() {}
};

/*! Implementation of the SingleThreaded (no-synchronization) ThreadingModel
    policy used by various classes.
    It is desirable for all policies that class Lock should be constructed
    given a reference to the host class (a class which holds the lock
    instance). This reference is used only for ObjectLevelLockable,
    for all other threading policies it is ignored.
    Classes which do not initalize Lock with lock holder will not be
    able to use ObjectLevelLockable.

    T template argument is used for obtaining thread-specific type
    traits (e.g. volatile type).
 */
template <class M = NullThreadMutex>
  class SingleThreaded {
public:
    typedef ThreadAtomicInt AtomicInt; // no need for non-threaded now
    typedef typename M::MutexType MutexType;
    typedef M ThreadMutexClass;

    enum { multiThreaded = false }; // not really multithreaded

    /*! A lock guard primitive
     */
    class Lock
    {
    public:
        Lock() {}
        Lock(const SingleThreaded&) {}
        Lock(const Lock&) {}
        ~Lock() {} // GCC warning buster
    };
    // out-of-class traits use only
    typedef M VolatileType;
};

/*! An implementation of ObjectLevelLockable ThreadingModel policy.
 */
template <class M = ThreadMutex>
  class ObjectLevelLockable {
public:
    typedef ThreadAtomicInt AtomicInt;
    typedef typename M::MutexType MutexType;
    typedef M ThreadMutexClass;

    enum { multiThreaded = true };

    ObjectLevelLockable() {
        M::initialize(&mtx_);
    }

    ~ObjectLevelLockable() {
        M::destroy(&mtx_);
    }
    class Lock;
    friend class Lock;

    class Lock
    {
    private:
        ObjectLevelLockable& host_;

        Lock& operator=(const Lock&);
    public:
        Lock(ObjectLevelLockable& host) : host_(host)
        {
            M::lock(&host_.mtx_);
        }
        ~Lock()
        {
            M::unlock(&host_.mtx_);
        }
        Lock(const Lock& other) : host_(other.host_) {}
    };
    // out-of-class traits use only
    typedef volatile M VolatileType;

private:
    MutexType mtx_;
};

template <class M = ThreadMutex>
  class ClassLevelLockable {
public:
    typedef ThreadAtomicInt AtomicInt;
    typedef typename M::MutexType MutexType;
    typedef M ThreadMutexClass;
    
    enum { multiThreaded = true };

    class Lock;
    friend class Lock;

    class Lock
    {
        Lock& operator=(const Lock&);
    public:
        Lock()
        {
            if (!initialized_) {
                M::initialize(&mtx_);
                initialized_ = true;
            }
            M::lock(&mtx_);
        }
        Lock(ClassLevelLockable&)
        {
            if (!initialized_) {
                M::initialize(&mtx_);
                initialized_ = true;
            }
            M::lock(&mtx_);
        }
        ~Lock()
        {
            M::unlock(&mtx_);
        }
        Lock(const Lock&) {} // do nothing on copy
    };
    // out-of-class traits use only
    typedef volatile M VolatileType;

private:
    static MutexType mtx_;
    static bool initialized_;

    struct Initializer;
    friend struct Initializer;
    struct Initializer
    {
        Initializer()
        {
            if (!initialized_)
                M::initialize(&mtx_);
            initialized_ = true;
        }
        ~Initializer()
        {
            if (initialized_)
                M::destroy(&mtx_);
        }
    };
    static Initializer initializer_;
};

template <class M> typename ClassLevelLockable<M>::MutexType
    ClassLevelLockable<M>::mtx_;
template <class M> bool ClassLevelLockable<M>::initialized_ = false;
template <class M> typename ClassLevelLockable<M>::Initializer
    ClassLevelLockable<M>::initializer_;

COMMON_NS_END

#endif // THREADING_POLICIES_H_
