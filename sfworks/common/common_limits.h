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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef COMMON_LIMITS_H_
#define COMMON_LIMITS_H_

#include "common/common_defs.h"
#include "common/common_types.h"


#if defined(__linux__) && defined(__GNUG__)
# ifndef __STDC_VERSION__
#  define STDC_VERSION_DEFINED_
#  define __STDC_VERSION__ 199901L
# endif
#endif

#if defined(_MSC_VER) || (defined(__GNUG__) && !(__GNUG__ < 3))
# include <limits>
# include <limits.h>
# if defined(max) && defined(min)
#  undef max
#  undef min
# endif

# if defined(_MSC_VER) && (_MSC_VER < 1300)

# define ULLONG_MAX    _UI64_MAX
# define LLONG_MAX     _I64_MAX
# define LLONG_MIN     _I64_MIN

namespace std {

template<> struct numeric_limits<int64> {
    static int64 max() throw() { return LLONG_MAX; }
    static int64 min() throw() { return LLONG_MIN; }
};

template<> struct numeric_limits<uint64> {
    static uint64 max() throw() { return ULLONG_MAX; }
    static uint64 min() throw() { return 0; }
};

}

# endif /* _MSC_VER */

#else /* Compilers w/o std::numeric_limits.h */

# include "common/math_defs.h"
# include <limits.h>
# include <float.h>

namespace std {

template <typename T> struct numeric_limits {
    static T max() throw();
    static T min() throw();
    static T quiet_NaN() throw();
};

template<> struct numeric_limits<char> {
    static char max() throw() { return CHAR_MAX; }
    static char min() throw() { return CHAR_MIN; }
};

template<> struct numeric_limits<short> {
    static short max() throw() { return SHRT_MAX; }
    static short min() throw() { return SHRT_MIN; }
};

template<> struct numeric_limits<int> {
    static int max() throw() { return INT_MAX; }
    static int min() throw() { return INT_MIN; }
};

template<> struct numeric_limits<long> {
    static long max() throw() { return LONG_MAX; }
    static long min() throw() { return LONG_MIN; }
};

template<> struct numeric_limits<int64> {
    static int64 max() throw() { return LLONG_MAX; }
    static int64 min() throw() { return LLONG_MIN; }
};

template<> struct numeric_limits<unsigned char> {
    static uint64 max() throw() { return UCHAR_MAX; }
    static uint64 min() throw() { return 0; }
};

template<> struct numeric_limits<unsigned short> {
    static unsigned short max() throw() { return USHRT_MAX; }
    static unsigned short min() throw() { return 0; }
};

template<> struct numeric_limits<unsigned int> {
    static unsigned int max() throw() { return UINT_MAX; }
    static unsigned int min() throw() { return 0; }
};

template<> struct numeric_limits<unsigned long> {
    static unsigned long max() throw() { return ULONG_MAX; }
    static unsigned long min() throw() { return 0; }
};

template<> struct numeric_limits<uint64> {
    static uint64 max() throw() { return ULLONG_MAX; }
    static uint64 min() throw() { return 0; }
};

template<> struct numeric_limits<float> {
    static float max() throw() { return FLT_MAX; }
    static float min() throw() { return FLT_MIN; }
    static float quiet_NaN() throw() { return NAN; }
    static const int max_exponent10 = FLT_MAX_10_EXP;
    static const int min_exponent10 = FLT_MIN_10_EXP;
};

template<> struct numeric_limits<double> {
    static double max() throw() { return DBL_MAX; }
    static double min() throw() { return DBL_MIN; }
    static double quiet_NaN() throw() { return NAN; }
    static const int max_exponent10 = DBL_MAX_10_EXP;
    static const int min_exponent10 = DBL_MIN_10_EXP;
};

}

#endif

#if defined(__linux__) && defined(__GNUG__) && defined(STDC_VERSION_DEFINED_)
# undef STDC_VERSION_DEFINED_
# undef __STDC_VERSION__
#endif

COMMON_NS_BEGIN

template<typename T> inline T Max() { return std::numeric_limits<T>::max(); }
template<typename T> inline T Min() { return std::numeric_limits<T>::min(); }
template<typename T> inline T Nil() { return T(); }

COMMON_NS_END

#endif // COMMON_LIMITS_H_
