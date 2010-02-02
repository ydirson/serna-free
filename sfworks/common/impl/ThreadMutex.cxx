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
/** \file
 *  Implementation file for ThreadMutex.h
 */

#if !defined(__i386__) && !defined(__sun) && \
    !defined(_WIN32) && !defined(__APPLE__) && !defined(__amd64__)
# error Atomic increment/decrement is not defined for your architecture
#endif

#include "common/common_defs.h"

#ifdef MULTI_THREADED
# ifdef _WIN32
#  define VC_EXTRALEAN
#  include <windows.h>
# else
/// \attention kludge to compile in pthread_mutexattr_settype prototype
#  ifdef linux
#   define _XOPEN_SOURCE 500
#  endif
#  include <pthread.h>
# endif // _WIN32
#endif // MULTI_THREADED

// For memset(), see pthread-based ThreadMutex::initialize
#include <string.h>
#include "common/ThreadMutex.h"


COMMON_NS_BEGIN

#ifdef MULTI_THREADED

#ifdef _WIN32

void ThreadMutex::initialize(MutexType* m)
{
    ::InitializeCriticalSection(m);
}

void ThreadMutex::destroy(MutexType* m)
{
    ::DeleteCriticalSection(m);
}

void ThreadMutex::lock(MutexType* m)
{
    ::EnterCriticalSection(m);
}

void ThreadMutex::unlock(MutexType* m)
{
    ::LeaveCriticalSection(m);
}

#else // _WIN32

void ThreadMutex::initialize(MutexType* m)
{
    ::pthread_mutexattr_t mutex_attr;
    ::pthread_mutexattr_init(&mutex_attr);
    ::pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
// Solaris 8 MT programming guide states that mutex memory _must_ be
// initialized to zero. It can't hurt other pthreads implementations.
    memset(m, 0, sizeof(*m));
    ::pthread_mutex_init(m, &mutex_attr);
    ::pthread_mutexattr_destroy(&mutex_attr);
}

void ThreadMutex::destroy(MutexType* m)
{
    ::pthread_mutex_destroy(m);
}

void ThreadMutex::lock(MutexType* m)
{
    ::pthread_mutex_lock(m);
}

void ThreadMutex::unlock(MutexType* m)
{
    ::pthread_mutex_unlock(m);
}

#endif // _WIN32

#endif // MULTI_THREADED

COMMON_NS_END

#if !defined(INLINE_ATOMICS)
# define INLINE_ATOMICS
# include "machdep/ThreadMutexAtomics.i"
#endif

