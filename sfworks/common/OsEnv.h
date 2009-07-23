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
// Copyright (c) 2007 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef OS_ENV_H_
#define OS_ENV_H_

#include "common/common_defs.h"
#include "common/String.h"

namespace Common {

namespace os_env_private {

template<unsigned int CHAR_SZ>
    Common::String get_env(const void* name, unsigned len);

template<> COMMON_EXPIMP Common::String
    get_env<sizeof(Common::Char)>(const void* name, unsigned len);

template<> COMMON_EXPIMP Common::String
    get_env<sizeof(char)>(const void* name, unsigned len);

}

template<typename T> static String get_env(const T* name, unsigned len = ~0U)
{
    if (0 == name)
        return String();
    if (~0U == len)
        len = CHAR_TRAITS_TYPE(T)::length(name);
    typedef typename CHAR_TRAITS_TYPE(T)::char_type CharType;
    return os_env_private::get_env<sizeof(CharType)>(name, len);
}

template<typename T> static String get_env(const T& name)
{
    return get_env(name.data(), name.length());
}

}

#endif // OS_ENV_H_
