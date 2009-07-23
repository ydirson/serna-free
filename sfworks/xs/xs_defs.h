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

#ifndef XS_DEFS_H_
#define XS_DEFS_H_

#include "common/oalloc.h"
#define XS_OALLOC(name) USE_OALLOC(Xs:name)

//#define XS_THREADS  // Enable thread support
#define NO_XPATH
//#define XS_DEBUG

#ifndef NO_NS
#define XS_NAMESPACE Xs
#define XPATH_NAMESPACE Xpath
#endif

#ifdef XS_NAMESPACE
# define XS_NAMESPACE_BEGIN namespace XS_NAMESPACE {
# define XS_NAMESPACE_END   }
# define USING_XS_NAMESPACE using namespace XS_NAMESPACE;
#else
# define XS_NAMESPACE
# define XS_NAMESPACE_BEGIN
# define XS_NAMESPACE_END
# define USING_XS_NAMESPACE
#endif // XS_NAMESPACE

#ifdef _MSC_VER
# pragma warning (disable : 4251 4786)

# if (defined(SFWORKS_STATIC) || defined(XS_STATIC))
#  define XS_EXPIMP
#  define XS_EXTERN
# elif defined(BUILD_XS) || defined(SFWORKS_DLL)
#  if defined(XS_API)
#   define XS_EXPIMP __declspec(dllexport)
#   define XS_EXTERN
#  else
#   define XS_EXPIMP
#   define XS_EXTERN
#  endif
# else
#  define XS_EXPIMP __declspec(dllimport)
#  define XS_EXTERN extern
# endif // STATIC...
#else
# define XS_EXPIMP
# define XS_EXTERN
#endif // _MSC_VER

#ifdef XS_THREADS
# ifndef USE_THREADS
# define USE_THREADS
# endif // USE_THREADS
# define TS_COPY(s) STRING_CONS(s)
#else  // XS_THREADS
# define TS_COPY(s) (s)
#endif // XS_THREADS

#endif // XS_DEFS_H_
