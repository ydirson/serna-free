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
// Copyright (c) 2002 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *
 *  pathname manipulation class
 */

#ifndef _PATH_NAME_H
#define _PATH_NAME_H

#include "common/common_defs.h"
#include "common/OwnerPtr.h"
#include "common/StringDecl.h"
#include "common/RangeIterator.h"
#include "common/FlexString.h"
#include <string>

namespace COMMON_NS {

class COMMON_EXPIMP PathName : private ustring {
public:
    typedef ustring base_type;

    PathName();
    PathName(const String& name);
    PathName(const std::string& name);
    PathName(const base_type& name);
    PathName(const char* name);
    PathName(const PathName& name);
    template<typename Iter> PathName(Iter begin, Iter end)
     :  base_type(begin, end), dirty_(true) {}
    ~PathName();
    PathName& operator=(const PathName&);

    const base_type& name() const { return *this; }

    //! returns everything between last dirSep() and the last dot in pathname
    base_type basename() const;
    //! returns everything between last dirSep() and the end
    base_type filename() const;
    //! returns everything after the last dot in pathname
    base_type extension() const;
    //! returns directory part of the pathname
    PathName dirname() const;
    //! if pathname is relative, prepends it with current directory
    PathName absolute() const;

    bool isRelative() const { return isRelative(name()); }
    bool exists() const;
    bool empty() const { return base_type::empty(); }

    PathName& assign(const PathName& p);
    PathName& assign(const base_type& p);
    PathName& assign(const char* p);

    PathName& append(const PathName& p);
    PathName& append(const base_type& p, bool appendDirSep = true);
    PathName& append(const char* p, bool appendDirSep = true);

    //! removes redundant dir separators, ..'s etc. on error returns empty()
    PathName normPath() const;
    //!
    static void normPath(base_type& s);
    //!
#if defined(_WIN32)
    static const char PATH_SEP  = ';';
    static const char DIR_SEP   = '\\';
#else
    static const char PATH_SEP  = ':';
    static const char DIR_SEP   = '/';
#endif
    //! returns current directory of the process
    static PathName getcwd();
    //! searches list of dirs represented by 'path' for the file named 'name'
    static PathName searchPath(const base_type& name, const base_type& path,
                               char sep = PATH_SEP);
    //!
    static bool     exists(const base_type& path);
    //!
    static bool     isRelative(const base_type& path);
    static bool     isRelative(const base_type::value_type* path, unsigned len);
    //!
    static bool     isSameFile(const PathName& p1, const PathName& p2);
    //!
    static bool isDirsep(Char c)
    {
        bool r(DIR_SEP == c);
#ifdef _WIN32
        r |= ('/' == c);
#endif
        return r;
    }
    static bool isDirsep(char c) { return isDirsep(Char(c)); }
private:
    PathName&                   set_dirty();
    class Buffer;
    mutable bool                dirty_; // whether cached cname_ is up-to-date
    mutable OwnerPtr<Buffer>    cname_; // system char representation buffer
};

COMMON_EXPIMP bool operator ==(const PathName& lhs, const PathName& rhs);

struct IsDirsep {
    template<typename CharType>
    bool operator()(CharType c) { return PathName::isDirsep(c); }
};

template <typename CharType, bool skipAdjSeps = true>
struct DirRangeIter
    : public RangeIterator<const CharType*,
                           IsDirsep, SepPolicy<const CharType*,
                                               IsDirsep,
                                               skipAdjSeps> > {
    typedef SepPolicy<const CharType*, IsDirsep, skipAdjSeps>
            SepPolicyType;
    typedef RangeIterator<const CharType*, IsDirsep, SepPolicyType>
            BaseType;
    typedef typename BaseType::IteratorType Iter;
    typedef typename BaseType::PredicateType PredicateType;
    typedef typename BaseType::RangeType RangeType;
    //!
    DirRangeIter(Iter begin, Iter end) : BaseType(begin, end) {}
    explicit DirRangeIter(const RangeType& r) : BaseType(r) {}
    DirRangeIter& operator=(const RangeType& r)
    {
        BaseType::operator=(r);
        return *this;
    }
};

} // end of namespace COMMON_NS

#endif // _PATH_NAME_H

