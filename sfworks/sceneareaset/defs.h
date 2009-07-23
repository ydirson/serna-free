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
/* \file This file contains namespace and type definitions for URI
   manager classes.
 */

#ifndef SCENEAREASET_DEFS_H_
#define SCENEAREASET_DEFS_H_

#include "common/common_types.h"

#if (OALLOC_TYPE > 0)
# include "common/oalloc.h"
# define SCENEAREASET_OALLOC(name) USE_OALLOC(SCENEAREASET:name)
#else
# define SCENEAREASET_OALLOC(name) ;
#endif // OALLOC_TYPE


#if defined(_MSC_VER)
# if (defined(SFWORKS_STATIC) || defined(SCENEAREASET_STATIC))
#  define SCENEAREASET_EXPIMP
#  define SCENEAREASET_EXTERN
# elif defined(BUILD_SCENEAREASET) || defined(SFWORKS_DLL)
#  if defined(SCENEAREASET_API)
#   define SCENEAREASET_EXPIMP __declspec(dllexport)
#   if !defined(QT_QTMMLWIDGET_EXPORT)
#    define QT_QTMMLWIDGET_EXPORT
#   endif
#   define SCENEAREASET_EXTERN
#  else
#   define SCENEAREASET_EXPIMP
#   define SCENEAREASET_EXTERN
#  endif
# else
#  define SCENEAREASET_EXPIMP __declspec(dllimport)
#  define SCENEAREASET_EXTERN extern
# endif // STATIC...
#else
# define SCENEAREASET_EXPIMP
# define SCENEAREASET_EXTERN
#endif //_MSC_VER

const double TAG_Z = 10000;
const double TEXT_Z_SHIFT = 0.1;

const double SELECTION_Z = 10100;

const double CURSOR_TIP_RECT_Z = 10200;
const double CURSOR_TIP_LABEL_Z = 10201;
const double CURSOR_Z = 10202;

static const int CURSOR_WIDTH = 1;


#endif // SCENEAREASET_DEFS_H_
