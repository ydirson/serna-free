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
#ifndef SAPI_DEFS_H_
#define SAPI_DEFS_H_

#define DISABLE_COPY_CTORS(x) private: c(const c&); c& operator=(const c&);

#if defined(_MSC_VER)
# if (defined(SFWORKS_STATIC) || defined(SAPI_STATIC))
#  define SAPI_IMPORT
#  define SAPI_EXPIMP
#  if defined(BUILD_SAPI)
#   define SAPI_EXTERN
#  else
#   define SAPI_EXTERN extern
#  endif
#  define SAPI_EXPORT
# elif defined(BUILD_SAPI)
#  define SAPI_EXPIMP __declspec(dllexport)
#  define SAPI_IMPORT
#  define SAPI_EXTERN
#  define SAPI_EXPORT __declspec(dllexport)
# else
#  define SAPI_IMPORT __declspec(dllimport)
#  define SAPI_EXPIMP __declspec(dllimport)
#  define SAPI_EXTERN extern
#  define SAPI_EXPORT __declspec(dllexport)
# endif // STATIC...
#else
# define SAPI_IMPORT
# define SAPI_EXPIMP
# define SAPI_EXTERN
# define SAPI_EXPORT
#endif // MSC_VER

namespace SernaApi {
    typedef long long int64;
    typedef unsigned long long uint64;
}

#endif // SAPI_DEFS_H_
