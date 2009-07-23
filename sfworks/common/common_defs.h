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
    Common definitions for foundation library
 */
#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

// Pragmas - open COMMON declaration, close COMMON declaration.
#ifdef _MSC_VER
# define COMMON_PRAGMA_BEGIN pack(push,4)
# define COMMON_PRAGMA_END   pack(pop)
#endif

#if defined(_MSC_VER)

# if (defined(SFWORKS_STATIC) || defined(COMMON_STATIC))
#  define COMMON_IMPORT
#  define COMMON_EXPIMP
#  if defined(BUILD_COMMON)
#   define COMMON_EXTERN
#  else
#   define COMMON_EXTERN extern
#  endif
#  define COMMON_EXPORT
# elif defined(BUILD_COMMON) || defined(SFWORKS_DLL)
#  define COMMON_EXPIMP __declspec(dllexport)
#  define COMMON_IMPORT
#  define COMMON_EXTERN
#  define COMMON_EXPORT __declspec(dllexport)
# else
#  define COMMON_IMPORT __declspec(dllimport)
#  define COMMON_EXPIMP __declspec(dllimport)
#  define COMMON_EXTERN extern
#  define COMMON_EXPORT __declspec(dllexport)
# endif // STATIC...
#else
# define COMMON_IMPORT
# define COMMON_EXPIMP
# define COMMON_EXTERN
# define COMMON_EXPORT
#endif // MSC_VER

#if defined(_WIN32) && !defined (MULTI_THREADED)
# define MULTI_THREADED
// #error Under win32 you should define MULTI_THREADED!!!
#endif

#if defined(NOCVSID)
# define CVSID(n,s)
#else
# if !defined(CVSID)
#  if defined(__GNUC__)
#   if defined(i386) || defined(sparc)
// const char[] are optimized away by GCC, so __asm__
#    define CVSID(n, s) __asm__(".ident \"" s "\"")
#   elif defined(__APPLE__)
#    define CVSID(n, s) __asm__(".asciz \"" s "\"")
#   endif
#  else
#   define CVSID(n, s) static const char n##_cvsid[] = s
#  endif
# endif // CVSID
#endif


#define NEED_STRING2STL

#ifndef COMMON_NS
# define COMMON_NS Common
#endif

#ifdef COMMON_NS
# define COMMON_NS_PREFIX(x) COMMON_NS::x
# define COMMON_NS_BEGIN     namespace COMMON_NS {
# define COMMON_NS_END       }
# define USING_COMMON_NS     using namespace COMMON_NS;
namespace COMMON_NS {}
#else // COMMON_NS
# define COMMON_NS
# define COMMON_NS_PREFIX(x) x
# define COMMON_NS_BEGIN     // empty
# define COMMON_NS_END       // empty
# define USING_COMMON_NS     // empty
#endif // COMMON_NS

#define DEFAULT_COPY_CTOR_DECL(c) c(const c&);
#define DEFAULT_ASSIGN_OP_DECL(c) c& operator=(const c&);

#define DEFAULT_COPY_CTOR_IMPL(c) c::c(const c&) {}
#define DEFAULT_ASSIGN_OP_IMPL(c) c& c::operator=(const c&) {}

// Null translation macro
#define NOTR(x) x

#endif // COMMON_DEFS_H_
