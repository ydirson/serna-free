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
/*! Elementary types for i386/GCC
 */
#ifndef MACHDEP_I386_GNUC_TYPES_H_
#define MACHDEP_I386_GNUC_TYPES_H_

#include "common/common_defs.h"


typedef char           int8;
typedef unsigned char  uint8;
typedef short          int16;
typedef unsigned short uint16;
typedef long            int32;
typedef unsigned long   uint32;
typedef long long      int64;
typedef unsigned long long uint64;

#define UINT64_VALUE(x) x##ULL
#define INT64_VALUE(x)  x##LL

//
// Generate scalar & vector is_undef()s and set_undef()s
//
#define _UNDEF_(t, uval) \
    inline bool is_undef(t v)              { return v == t(uval); } \
    inline void set_undef(t& v)            { v = t(uval); } \
    inline void set_undef(t* pv, uint32 n) { while (n--) *pv++ = t(uval); }

    _UNDEF_(int8,   0x80)
    _UNDEF_(int16,  0x8000)
    _UNDEF_(int32,  0x80000000L)
    _UNDEF_(int64,  0x8000000000000000LL)
    _UNDEF_(uint8,  0x80)
    _UNDEF_(uint16, 0x8000)
    _UNDEF_(uint32, 0x80000000L)
    _UNDEF_(uint64, 0x8000000000000000LL)

#undef _UNDEF_

// char* - special case because we must make delete first.
inline void un_set(char*& v)            { delete v; v = 0; }
inline void un_set(char* *pv, uint32 n) { while (n--) { delete *pv; *pv++ = 0; } }
inline bool is_empty(const char*& v)    {return (v == 0 || *v == 0); }

//
// Template "undef" functions for enums
//
template <class T>
inline bool is_undef_enum(const T& v)
{
    return v == (T) 0x80000000l;
}

template <class T>
inline void set_undef_enum(T& v)
{
    v = (T) 0x80000000l;
}

template <class T>
inline void set_undef_enum(T* pv, uint32 n)
{
    while (n--)
        *pv++ = (T) 0x80000000l;
}

//
// Template "undef" functions for pointers
//
template <class T>
inline bool is_undef(const T* v)
{
    return v == 0;
}

template <class T>
inline void set_undef(T*& v)
{
    v = 0;
}

template <class T>
inline void set_undef(T** pv, uint32 n)
{
    while (n--)
        *pv++ = 0;
}

// "Undef" functions for bool
inline bool is_undef(bool v)              { return (int8&) v == int8(0x80); }
inline void set_undef(bool& v)            { (int8&) v = 0x80; }
inline void set_undef(bool* pv, uint32 n) { while (n--) (int8&) (*pv++) = 0x80; }
inline bool is_true(bool v)  { return !is_undef(v) && v;  }
inline bool is_false(bool v) { return !is_undef(v) && !v; }

#endif // MACHDEP_I386_GNUC_TYPES_H_
