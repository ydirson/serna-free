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

#if defined(__ICL)
# pragma warning (disable: 1011)
#endif

static inline unsigned long xadd(ThreadMutex::IntType& val, int amt)
{
    __asm mov edx, val __asm mov ecx, amt __asm mov eax, ecx
    __asm lock xadd [edx], eax __asm add eax, ecx
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
    return xadd(const_cast<IntType&>(lval), 1);
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
    return xadd(const_cast<IntType&>(lval), -1);
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicExchange(VolatileIntType& lval, IntType rval)
{
    __asm mov ebx, lval __asm mov eax, [ebx] __asm mov edx, rval
    __asm lock cmpxchg    [ebx], edx __asm jnz $-4
}

#if defined(__ICL) && 0
# pragma warning (default: 1011)
#endif

COMMON_NS_END

#endif // THREAD_MUTEX_ATOMICS_I_
