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

#ifdef _MSC_VER
#define SS_TEMPLATE_INST
#endif

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/RefCntStorage.h"
#include "common/StringUtils.h"
#include "common/FlexString.h"
#include <QString>


COMMON_NS_BEGIN

#if defined(_MSC_VER) || 1

template <typename E, class T, class A> RefCntStorage<E, T, A>&
RefCntStorage<E, T, A>::operator=(const RefCntStorage<E, T, A>&)
{
    return *this;
}

template class COMMON_EXPIMP RefCntStorage<Char>;
template class COMMON_EXPIMP RefCntStorage<char>;

template<class StrType> struct null_checker : public StrType {
    static inline bool is_null_(const StrType& s)
    {
        const null_checker& r(reinterpret_cast<const null_checker&>(s));
        return r.is_null();
    }
};

template <class S> bool is_null_string(const S& s)
{
    return null_checker<S>::is_null_(s);
}

#define FSTRING(c,s) flex_string<c, CHAR_TRAITS_TYPE(c), std::allocator<c>, \
                       s<c, CHAR_TRAITS_TYPE(c), std::allocator<c> > >

template<> bool is_null(const FSTRING(Char, RefCntStorage)& s)
{
    return is_null_string(s);
}

template<> bool is_null(const FSTRING(char, RefCntStorage)& s)
{
    return is_null_string(s);
}

#endif

COMMON_NS_END
