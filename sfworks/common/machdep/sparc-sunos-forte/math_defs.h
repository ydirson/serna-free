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
/*! Definitions of sparc-specific point arithmetics things
 */
#ifndef SPARC_FORTE_MATH_DEFS_H_
#define SPARC_FORTE_MATH_DEFS_H_


#include <limits>
#include <ieeefp.h>

template <typename T> inline T
getNaN()
{
    return std::numeric_limits<T>::quiet_NaN();
}

// template <> inline float undef_value<float>(float*)   { return getNaN<float>(); }
// template <> inline double undef_value<double>(double*)  { return getNaN<double>(); }

// left for compatibility
#define NAN getNaN<float>()

inline bool is_undef(float v)  { return isnanf(v); }
inline bool is_undef(double v) { return isnand(v); }

inline void set_undef(float& v)  { v = getNaN<float>(); }
inline void set_undef(double& v) { v = getNaN<double>(); }

inline void set_undef(float* v, uint32 n)  { while(n--) *v++ = getNaN<float>(); }
inline void set_undef(double* v, uint32 n) { while(n--) *v++ = getNaN<double>(); }


#endif // SPARC_FORTE_MATH_DEFS_H_
