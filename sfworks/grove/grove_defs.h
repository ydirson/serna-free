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

#ifndef GROVE_DEFS_H_
#define GROVE_DEFS_H_

#include "common/common_defs.h"
#include "common/asserts.h"
#include "common/oalloc.h"

//#define GROVE_DEBUG

#define THREAD_SAFE_GROVE

#ifndef NO_NS
#define GROVE_NAMESPACE GroveLib
#endif

#ifdef GROVE_NAMESPACE
# define GROVE_NAMESPACE_BEGIN namespace GROVE_NAMESPACE {
# define GROVE_NAMESPACE_END   }
# define USING_GROVE_NAMESPACE using namespace GROVE_NAMESPACE;
namespace GROVE_NAMESPACE {}
//namespace GROVE_NAMESPACE {}
#else
# define GROVE_NAMESPACE
# define GROVE_NAMESPACE_BEGIN
# define GROVE_NAMESPACE_END
# define USING_GROVE_NAMESPACE
#endif // GROVE_NAMESPACE

#ifdef GROVE_DEBUG
# include <iostream>
#endif // GROVE_DEBUG

#include "grove/grove_exports.h"

#define GROVE_OALLOC(n) USE_OALLOC(Grove:n)

#endif // GROVE_DEFS_H_
