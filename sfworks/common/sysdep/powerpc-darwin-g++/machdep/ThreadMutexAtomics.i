// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *  
 */

#ifndef THREAD_MUTEX_ATOMICS_I_
#define THREAD_MUTEX_ATOMICS_I_

COMMON_NS_BEGIN

//    __attribute__((always_inline))
INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicRead(VolatileIntType& lval)
{
    register IntType ret;
    __asm__ __volatile__("\n\
1:        lwarx   %[ret],0,%[lval]\n\
          stwcx.  %[ret],0,%[lval]\n\
          bne-    1b"
          : [ret] "=&r" (ret)
          : [lval] "r" (&lval)
          : "cc"
    );
    return ret;
}

//    __attribute__((always_inline))
INLINE_ATOMICS void
ThreadMutex::atomicAssign(VolatileIntType& lval, IntType rval)
{
    register IntType tmp __attribute__((unused));
    __asm__ __volatile__("\n\
1:        lwarx   %[tmp],0,%[lval]\n\
          stwcx.  %[rval],0,%[lval]\n\
          bne-    1b"
          : [tmp] "=&r" (tmp)
          : [lval] "r" (&lval), [rval] "r" (rval)
          : "cc", "memory"
    );
}

//    __attribute__((always_inline))
INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
    register IntType ret;
    __asm__ __volatile__("\n\
1:        lwarx   %[ret],0,%[lval]\n\
          addic   %[ret],%[ret],1\n\
          stwcx.  %[ret],0,%[lval]\n\
          bne-    1b"
          : [ret] "=&r" (ret)
          : [lval] "r" (&lval)
          : "cc", "memory"
    );
    return ret;
}

//    __attribute__((always_inline))
INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
    register IntType ret;
    __asm__ __volatile__("\n\
1:        lwarx   %[ret],0,%[lval]\n\
          addic   %[ret],%[ret],-1\n\
          stwcx.  %[ret],0,%[lval]\n\
          bne-    1b"
          : [ret] "=&r" (ret)
          : [lval] "r" (&lval)
          : "cc", "memory"
    );
    return ret;
}

//    __attribute__((always_inline))
INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicExchange(VolatileIntType& lval, IntType rval)
{
    register IntType ret;
    __asm__ __volatile__("\n\
1:        lwarx   %[ret],0,%[lval]\n\
          stwcx.  %[rval],0,%[lval]\n\
          bne-    1b"
          : [ret] "=&r" (ret)
          : [lval] "r" (&lval), [rval] "r" (rval)
          : "cc", "memory"
    );
    return ret;
}

COMMON_NS_END

#endif // THREAD_MUTEX_ATOMICS_I_
