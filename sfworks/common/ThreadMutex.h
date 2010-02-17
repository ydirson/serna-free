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
     This file contains (machine-dependent) definitions of
     primitive mutex locks. They are not intended for general
     use, and used for parametrization of threading policies
     defined in ThreadingPolicies.h
 */
#ifndef THREAD_MUTEX_H_
#define THREAD_MUTEX_H_

#if defined(_WIN32) && !defined (MULTI_THREADED)
# error Under win32 you must define MULTI_THREADED!!!
#endif

#include "common/common_defs.h"
#include "common/common_types.h"

#include <QBasicAtomicInt>

#ifdef MULTI_THREADED
# ifdef _WIN32
#  define VC_EXTRALEAN
#  include <windows.h>
# else // _WIN32
#  include <pthread.h>
# endif // _WIN32
#endif // MULTI_THREADED

#if defined(MUTEX_DEBUG) && defined(MULTI_THREADED)
# error MUTEX_DEBUG with MULTI_THREADED makes no sense
#endif

COMMON_NS_BEGIN


/*! A null mutex (no multithreading)
 */
class COMMON_EXPIMP NullThreadMutex {
public:
#ifndef MUTEX_DEBUG
    typedef EmptyStruct MutexType;
#else
    typedef NullThreadMutex MutexType;
#endif

    typedef int32 IntType;
    typedef int32 VolatileIntType;

#ifndef MUTEX_DEBUG
    static void initialize(MutexType*) {}
    static void destroy(MutexType*) {}
    static void lock(MutexType*) {}
    static void unlock(MutexType*) {}
#else // MUTEX_DEBUG
    static void initialize(MutexType* m)
    {
        m->locked_ = false; m->initialized_ = true;
    }
    static void destroy(MutexType* m)
    {
        assert(m->initialized_); m->initialized_ = false;
    }
    static void lock(MutexType* m)
    {
       assert(m->initialized_); assert(!m->locked_); m->locked_ = true;
    }
    static void unlock(MutexType* m)
    {
        assert(m->initialized_); assert(m->locked_); m->locked_ = false;
    }
#endif // MUTEX_DEBUG
#ifdef MUTEX_DEBUG
private:
    bool locked_;
    bool initialized_;
#endif // MUTEX_DEBUG
};

#ifdef MULTI_THREADED
/*! Non-recursive thread mutex
 */
class COMMON_EXPIMP ThreadMutex {
public:
    typedef int32 IntType;
# ifdef _WIN32
    // Win32 critical sections
    typedef CRITICAL_SECTION MutexType;
# else
    // posix threads mutexes
    typedef ::pthread_mutex_t MutexType;
# endif

    static void initialize(MutexType*);
    static void destroy(MutexType*);
    static void lock(MutexType*);
    static void unlock(MutexType*);
};

#else // MULTI_THREADED

class COMMON_EXPIMP ThreadMutex : public NullThreadMutex {};

#endif // MULTI_THREADED

COMMON_NS_END

#endif // THREAD_MUTEX_H_
