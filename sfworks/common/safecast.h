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
/*! \file
 *  Run-time type compatibility checker
 */
#ifndef SAFECAST_H_
#define SAFECAST_H_

#include "common/common_defs.h"
#include "common/asserts.h"


/**
 * SAFE_CAST(TYPE, PTR) may be used instead of just static_cast
 * to check type compatibility run-time in debugging version of the
 * program. It does the following:
 *
 * - in debug mode (NDEBUG is not set) it uses dynamic_cast and
 *   aborts the program if it returns 0. It also checks compile-time
 *   type compatibility (it generates compile-time errors if one is
 *   trying to cast from virtual base, for example).
 *
 * - in release mode, it is static_cast.
 */

#if !defined(NDEBUG) && !defined(__SUNPRO_CC)

COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

# define SAFE_CAST(TYPE, PTR) SAFE_CAST1(TYPE, PTR, __FILE__, __LINE__)
# define SAFE_CAST1(TYPE, PTR, F, L) \
  COMMON_NS::safecast_check<TYPE>(PTR, \
    "Cannot cast " #PTR " to " #TYPE, \
      static_cast<const char*>(F), static_cast<int>(L))

template <class Casted, class Initial>
  inline Casted safecast_check(Initial obj,
                               const char* message,
                               const char* file,
                               int line)
{
    // compile-time type compatibility check
    (void)static_cast<Casted>(obj);

    if (!obj)
        return 0;
    Casted casted = dynamic_cast<Casted>(obj);
    if (0 != casted)
        return casted;
    abort_with_message(file, line, message);
    return 0;                           // to keep compiler happy
}

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#else  // !defined(NDEBUG) && !defined(__SUNPRO_CC)
# define SAFE_CAST(TYPE, PTR) static_cast<TYPE>(PTR)
#endif // !defined(NDEBUG) && !defined(__SUNPRO_CC)

#endif // SAFECAST_H_
