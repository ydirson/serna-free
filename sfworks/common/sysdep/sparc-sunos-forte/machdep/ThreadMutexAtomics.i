// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *  
 */

#ifndef THREAD_MUTEX_ATOMICS_I_
#define THREAD_MUTEX_ATOMICS_I_

#if defined(__sun)
# if defined(__SUNPRO_CC) && (!defined(__sparcv8plus) && !defined(__sparcv9))
#    error SPARC processors supports atomic operators ONLY for Ultra Sparcs \
        (v8plus and v9); set up -xtarget=ultra and compile again
# endif
#endif

#if defined(_USE_SPARC_ATOMIC) || !defined(__SUNPRO_CC)
extern "C" int32 _sparc_atomic_increment(volatile int32 *v);
extern "C" int32 _sparc_atomic_decrement(volatile int32 *v);
#else
# include <rw/stdmutex.h>
#endif

namespace Common {

#if defined(_USE_SPARC_ATOMIC) && defined(__SUNPRO_CC)
// There is inline implementation of these methods
INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
        return _sparc_atomic_increment(&lval);
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
        return _sparc_atomic_decrement(&lval);
}

#elif defined(__GNUC__)

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
        return ++lval;
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
        return --lval;
}

#else // !_USE_SPARC_ATOMIC - use RW LIBRARY instead

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
        return ::__rwstd::InterlockedIncrement((long *)(&lval));
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
        return ::__rwstd::InterlockedDecrement((long *)(&lval));
}

#endif // _USE_SPARC_ATOMIC

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicRead(VolatileIntType& lval)
{
    return lval;
}

INLINE_ATOMICS void
ThreadMutex::atomicAssign(VolatileIntType& lval, IntType rval)
{
    lval = rval;
}

}

#endif // THREAD_MUTEX_ATOMICS_I_
