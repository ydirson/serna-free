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
//
// Machine-dependent definitions
//
// Solaris on SPARC, Sun Forte C++ compiler
//

#ifndef _MACHDEP_H_
#define _MACHDEP_H_


// define class prefixes empty
#define EIOCLASS
#define ETPCLASS
#define EUTILCLASS
#define ESLCLASS
#define EIOPCLASS


#define USE_UNIX_TIME
#define MUTEX_DEBUG
//#define MULTI_THREADED

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// used for file modes

#include <sys/types.h>
#include <sys/stat.h>

// #include <sys/wait.h>

// Max length of a host name
#include <sys/utsname.h>
#define MAX_HOST_NAME   SYS_NMLN

#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define STATIC_CONST_INT(type,name,value) static const type name = (value)
#define STATIC_CONST_STR(type,name,value) static const type name = (value)
#define STATIC_CONST_I64(type,name,value) static const type name = (value)

#include "OS.h"

// The Unix format strings for 64-bit integers

#define QD_FMT  "lld"
#define QU_FMT  "llu"
#define QX_FMT  "llx"

#include "common/common_types.h"
#include "common/math_defs.h"
#include "common/ThreadMutex.h"

//
// Alignment for different types
//
const uint32 ALIGN_BOOL   = 1;
const uint32 ALIGN_CHAR   = 1;
const uint32 ALIGN_SHORT  = 2;
const uint32 ALIGN_LONG   = 4;
const uint32 ALIGN_QUAD   = 4;
const uint32 ALIGN_FLOAT  = 4;
const uint32 ALIGN_DOUBLE = 8;
const uint32 ALIGN_PTR    = 4;
const uint32 ALIGN_ENUM   = 4;

const uint32 ALIGN_I8  = 1;
const uint32 ALIGN_I16 = 2;
const uint32 ALIGN_I32 = 4;
const uint32 ALIGN_I64 = 8;

/**
 * Align address to boundary
 * @arg addr address
 * @arg bdry boundary
 * @return aligned address
 */
inline uint32
align(uint32 addr, uint32 bdry)
{
    return ((addr + bdry-1) / bdry) * bdry;
}

// template for enums
template<class T>
inline bool is_undef_enum(const T& v)
{
    return v == (T) 0x80000000l;
}

// char* - special case because we must make delete first.
inline void un_set(char*& v)            { delete v; v = 0; }
inline void un_set(char* *pv, int n)    { while (n-- > 0) { delete *pv; *pv++ = 0; } }

// template for enums
template<class T>
inline void set_undef_enum(T& v)
{
    v = (T) 0x80000000l;
}

// template for enums
template<class T>
inline void set_undef_enum(T* pv, uint32 n)
{
    while(n--)
        *pv++ = (T) 0x80000000l;
}

//
// Convert from host byte order to EIOP network byte order and vice versa
// (the EIOP network order is LSB first, MSB last)
//

/** \attention Works only with even-sized integral types!
 *  It may be reasonable to change EIOP network byte order to big-endian
 *  since x86 instruction set has 'bswap' instruction while sparc, AFAIK, doesn't
 */

template <typename T> inline T
bswap(T v)
{
    char* p = reinterpret_cast<char*>(&v);
    const int sz  = sizeof(T) - 1;
    // swap bytes 0<->sz, 1<->sz-1, .., sz/2<->sz/2+1
    for (int i = 0; i < sz / 2; i++) {
        p[i] ^= p[sz-i]; p[sz-i] ^= p[i]; p[i] ^= p[sz-i];
    }
    return v;
}

inline int16 net_order(int16 v) { return bswap(v); }
inline int32 net_order(int32 v) { return bswap(v); }
inline int64 net_order(int64 v) { return bswap(v); }
inline uint16 net_order(uint16 v) { return bswap(v); }
inline uint32 net_order(uint32 v) { return bswap(v); }
inline uint64 net_order(uint64 v) { return bswap(v); }

// Mutex adaptor + atomic_xxx wrappers for new foundation

#ifdef MULTI_THREADED
typedef COMMON_NS::ThreadMutex MutexProvider;
#else
typedef COMMON_NS::NullThreadMutex MutexProvider;
#endif

class Mutex {

public:
    void lock()     { MutexProvider::lock(&mtx_); }
    void unlock()   { MutexProvider::unlock(&mtx_); }
private:
    MutexProvider::MutexType mtx_;
};

//
// System-specific types
//
typedef pid_t ProcessHandle;

#include <stdarg.h>
#include <fstream>
#include <ostream>

static inline char*
form_getbuffer()
{
    static const int cnt = 5;
    static const int sz = 1024;
    static int index;
    static char buf[cnt][sz];
    char* result = buf[(index++)%cnt];
    return result;
}

inline char*
form(const char *format,...) {
    char* buf = form_getbuffer();
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    return buf;
}

inline char*
vform(const char *format, va_list argptr) {
    char* buf = form_getbuffer();
    vsprintf(buf, format, argptr);
    return buf;
}

// these versions of form() should work via stream reference

inline std::ostream&
form(std::ostream& o, const char *format, ...) {
    char* buf = form_getbuffer();
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    o << buf;
    return o;
}

inline std::ostream&
vform(std::ostream& o, const char *format, va_list argptr) {
    char* buf = form_getbuffer();
    vsprintf(buf, format, argptr);
    o << buf;
    return o;
}



static const char* const ZONEINFO_DIR = "/usr/share/lib/zoneinfo";
inline const char* getTZdir() {
    return ZONEINFO_DIR;
}

#endif // _MACHDEP_H_
