// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *  
 */

#ifndef UNIVERSAL_DARWIN_MACHDEP_THREAD_MUTEX_ATOMICS_I_
#define UNIVERSAL_DARWIN_MACHDEP_THREAD_MUTEX_ATOMICS_I_

#if defined(__ppc__)
# include "../../powerpc-darwin-g++/machdep/ThreadMutexAtomics.i"
#else
# include "../../i386-darwin-g++/machdep/ThreadMutexAtomics.i"
#endif

#endif // UNIVERSAL_DARWIN_MACHDEP_THREAD_MUTEX_ATOMICS_I_
