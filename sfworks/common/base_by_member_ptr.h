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
  The macro definition for computing pointer to the base
  class or structure from the pointer to a member.
 */
#ifndef BASE_BY_MEMBER_PTR_H_
#define BASE_BY_MEMBER_PTR_H_

#include "common/common_defs.h"


#ifdef __GNUC__
/**
 * The type-checking version of function for computing pointer to
 * base class from pointer to a member.
 *
 * @arg member_ptr pointer to a member of a base class
 * @arg base_type type of the base class or structure
 * @arg member_name the name of the referred member
 * @return pointer to the base class
 */
 #define BASE_BY_MEMBER_PTR(member_ptr, base_type, member_name) ({ \
        typeof (((base_type *)0)->member_name) *p = (member_ptr); \
        const int off = (int) &((base_type *)0)->member_name; \
        (base_type *)((char *)p - off); })

#else // not a GNU compiler
/**
 * The non-type-checking version of function for computing pointer to
 * base class from pointer to a member
 *
 * @arg member_ptr pointer to a member of a base class
 * @arg base_type type of the base class or structure
 * @arg member_name the name of the referred member
 * @return pointer to the base class
 */
#define BASE_BY_MEMBER_PTR(member_ptr, base_type, member_name) \
        ((base_type *)(((char *) (member_ptr)) - \
                       (int) &((base_type *)0)->member_name))

#endif // __GNUC__

#endif // BASE_BY_MEMBER_PTR_H_
