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
/*! Definitions of i386-specific point arithmetics things
 */
#ifndef I386_GCC_MATH_DEFS_H_
#define I386_GCC_MATH_DEFS_H_


#include <math.h>

#ifdef __linux__
# include <bits/nan.h>
#else // __linux__
# ifdef __FreeBSD__
#  define NAN \
     (__extension__(( \
      union { unsigned __l __attribute__((__mode__(__SI__))); float __d; }) \
        { __l: 0x7fc00000UL }).__d)
# else // __FreeBSD__
#  error "NaN is not defined for your platform"
# endif // __FreeBSD__
#endif // __linux__

inline bool is_undef(float v)  { return isnan(v); }
inline bool is_undef(double v) { return isnan(v); }

inline void set_undef(float& v)  { v = NAN; }
inline void set_undef(double& v) { v = NAN; }

inline void set_undef(float* v, unsigned n)  { while(n--) *v++ = NAN; }
inline void set_undef(double* v, unsigned n) { while(n--) *v++ = NAN; }

#endif // I386_GCC_MATH_DEFS_H_
