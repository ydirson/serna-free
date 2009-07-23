// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *  
 */

#ifndef IX86_GCC_THREADMUTEX_I_
#define IX86_GCC_THREADMUTEX_I_

COMMON_NS_BEGIN

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicRead(VolatileIntType& lval)
{
    return lval;
}

INLINE_ATOMICS void ThreadMutex::atomicAssign(VolatileIntType& lval,
                                              IntType rval)
{
    lval = rval;
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicIncrement(VolatileIntType& lval)
{
    __asm__ __volatile__("lock ; incl %0"
        : "=m" (lval) : "m" (lval));
    return lval;
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicDecrement(VolatileIntType& lval)
{
    __asm__ __volatile__("lock ; decl %0"
        : "=m" (lval) : "m" (lval));
    return lval;
}

INLINE_ATOMICS ThreadMutex::IntType
ThreadMutex::atomicExchange(VolatileIntType& lval, IntType rval)
{
    int ret;
    __asm__ __volatile__ ("\n"
        "movl       %0, %%eax\n"
"     1:     lock; cmpxchg    %1, %0\n"
        "jnz        1b\n"
        "movl       %%eax, %1\n"
        : "=&g" (lval), "=r" (ret)
        :  "g" (rval)
        : "eax"
    );
    return ret;
}

COMMON_NS_END

#endif // IX86_GCC_THREADMUTEX_I_
