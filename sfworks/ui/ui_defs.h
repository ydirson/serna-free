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
 */

#ifndef UI_DEFS_H_
#define UI_DEFS_H_

#define UI_OALLOC(n) USE_OALLOC(Sui::n)

#include "common/oalloc.h"
#include "common/common_defs.h"

#if defined(UI_THROW) || defined(__SUNPRO_CC)
# define UI_THROWS_NONE throw()
#else
# define UI_THROWS_NONE
#endif

#if defined(_MSC_VER)

# if (defined(SFWORKS_STATIC) || defined(UI_STATIC))
#  define UI_EXPIMP
#  define UI_EXTERN
#  define UI_EXPORT
# elif defined(BUILD_UI) || defined(SFWORKS_DLL)
#  if defined(UI_API)
#   define UI_EXPIMP __declspec(dllexport)
#   define UI_EXTERN
#  else
#   define UI_EXPIMP
#   define UI_EXTERN
#  endif
#  define UI_EXPORT __declspec(dllexport)
# else
#  define UI_EXPIMP __declspec(dllimport)
#  define UI_EXTERN extern
#  define UI_EXPORT __declspec(dllexport)
# endif // STATIC...
#else
# define UI_EXPIMP
# define UI_EXTERN
# define UI_EXPORT
#endif

#endif // UI_DEFS_H_
