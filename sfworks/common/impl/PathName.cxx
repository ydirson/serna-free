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
 */

#include "common/PathName.h"
#include "common/FlexString.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/StringCmp.h"
#include "common/RangeIterator.h"

#include <string>
#include <utility>
#include <algorithm>
#include <vector>
#include <iterator>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(_WIN32)
# include <io.h>
# include <direct.h>
# include "common/winlean.h"
# include <windows.h>
typedef wchar_t CharType;
static const size_t PATH_MAX = _MAX_DIR;
# define ACCESS _waccess
# define STRLEN wcslen
# define GETCWD _wgetcwd
# define F_OK 0
# define _ACCESS _access
#else
# include <sys/param.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# define ACCESS ::access
# define STRLEN strlen
# define GETCWD ::getcwd
typedef char CharType;
#endif

#if defined(_WIN32)
//TODO: move this function into machdep ?
BOOL isWin2000orLater()
{
   OSVERSIONINFOA osver;
   osver.dwOSVersionInfoSize = sizeof(osver);
   GetVersionExA(&osver);
   if (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
       return false;
   if (osver.dwMajorVersion < 5)
       return false;
   return true;
}
#endif

namespace COMMON_NS {

class PathName::Buffer : private std::vector<CharType> {
    typedef std::vector<CharType> base_type;
public:
    Buffer() {}
    Buffer(const CharType* p) : base_type(p, p + STRLEN(p) + 1) {}
    Buffer(const CharType* p, unsigned len) { assign (p, len); }

    template<typename Ch> void assign(const Ch* p, unsigned len);
    const CharType* c_str() { return &(*begin()); }
};

template<class Src, class Dst, int szdiff> class Inserter {
public:
    template<typename It> static void insert(const Src* b, const Src* e, It it)
    {
        std::copy(b, e, it);
    }
};

template<class Src, class Dst> class Inserter<Src, Dst, 0> {
public:
    template<typename It> static void insert(const Src* b, const Src* e, It it)
    {
        std::copy(reinterpret_cast<const Dst*>(b),
                  reinterpret_cast<const Dst*>(e), it);
    }
};

template<typename S, class C> void cont_assign(const S* b, const S* e, C& c)
{
    c.resize(0);
    typedef typename C::value_type D;
    Inserter<S, D, sizeof(S) - sizeof(D)>::insert(b, e, std::back_inserter(c));
}

template<typename Ch> void PathName::Buffer::assign(const Ch* p, unsigned len)
{
    bool nt = p[len-1] == Ch(0);
    reserve(nt ? len : len + 1);
    cont_assign(p, p + len, static_cast<base_type&>(*this));
    if (!nt)
        push_back('\0');
}

typedef PathName::base_type StrType;

PathName::PathName() : dirty_(true) {}

PathName::PathName(const base_type& name) : base_type(name), dirty_(true) {}

PathName::PathName(const String& name) : base_type(name), dirty_(true) {}

PathName::PathName(const std::string& name)
    : base_type(from_local_8bit(name.c_str())), dirty_(true) {}

PathName::PathName(const char* name)
    : base_type(from_local_8bit(name)), dirty_(true) {}

PathName::PathName(const PathName& name)
 :  base_type(name), dirty_(true)
{
}

PathName::~PathName() {}

PathName& PathName::operator=(const PathName& other)
{
    base_type::operator=(other);
    dirty_ = true;
    return *this;
}

bool operator ==(const PathName& lhs, const PathName& rhs)
{
    return lhs.name() == rhs.name();
}

PathName PathName::getcwd()
{
    PathName r;
    CharType* p = GETCWD(0, PATH_MAX);
    if (0 != p) {
        unsigned len = STRLEN(p);
        try { r.base_type::assign(p, p + len); }
        catch(...) {
            free(p);
            throw;
        }
        free(p);
    }
    return r;
}

bool PathName::exists(const base_type& path)
{
#ifndef _WIN32
    nstring tmp(local_8bit(path));
    return -1 != ACCESS(tmp.c_str(), F_OK);
#else
    if (isWin2000orLater()) {
        Buffer tmp;
        tmp.assign(path.data(), path.length());
        return -1 != ACCESS(tmp.c_str(), F_OK);
    }
    else {
        nstring tmp(local_8bit(path));
        return -1 != _ACCESS(tmp.c_str(), F_OK);
    }
#endif
}

bool PathName::exists() const
{
    if (empty())
        return false;
    if (dirty_) {
        if (cname_.isNull())
            cname_.reset(new Buffer);
#ifndef _WIN32
        nstring tmp(local_8bit(name()));
#else
        const base_type& tmp = name();
#endif
        cname_->assign(tmp.data(), tmp.size());
        dirty_ = false;
    }
#ifdef _WIN32
    if (!isWin2000orLater()) {
        nstring tmp(local_8bit(name()));
        return -1 != _ACCESS(tmp.c_str(), F_OK);
    }
#endif
    return -1 != ACCESS(cname_->c_str(), F_OK);
}

static size_t last_dirsep(const PathName::base_type& str)
{
    typedef StrType::value_type CharType;
    if (unsigned sz = str.size()) {
        const CharType* rend = str.data() - 1;
        for (const CharType* p = rend + sz; p != rend; --p)
            if (PathName::isDirsep(*p))
                return p - rend - 1;
    }
    return 0;
}

PathName PathName::absolute() const
{
    if (isRelative()) {
        PathName tmp(PathName::getcwd());
        if (!tmp.empty())
            return tmp.append(*this);
    }
    return *this;
}

PathName::base_type PathName::basename() const
{
    size_t sp = last_dirsep(*this);
    sp = sp ? sp + 1 : sp;
    size_t dp = find_last_of('.');
    if (npos == dp)
        dp = length();

    return substr(sp, dp - sp);
}

PathName::base_type PathName::filename() const
{
    size_t sp = last_dirsep(*this);
    sp = sp ? sp + 1 : sp;

    return substr(sp);
}

PathName PathName::dirname() const
{
    return PathName(substr(0, last_dirsep(*this)));
}

PathName::base_type PathName::extension() const
{
    size_t dp = find_last_of('.');
    if (npos == dp)
        return base_type();
    ++dp;

    return substr(dp, length() - dp);
}

template<typename Ch> unsigned win32_root_part_len(const Ch* p, unsigned sz)
{
    if (1 == sz && PathName::isDirsep(p[0]))
        return 1;
    if ((p[0] == '/' && p[1] == '/') || (p[0] == '\\' && p[1] == '\\')) {
        if (2 == sz || PathName::isDirsep(p[2]))
            return 1;
        for (unsigned i = 3; i < sz; ++i) {
            if (PathName::isDirsep(p[i]))
                return i + 1;
        }
        return sz;
    }
    if (PathName::isDirsep(p[0]))
        return 1;
    if (isalpha(to_int_type(p[0])) && ':' == p[1]) {
        if (sz > 2 && PathName::isDirsep(p[2]))
            return 3;
        return 2;
    }
    return 0;
}

template<typename Ch> unsigned root_part_len(const Ch* p, unsigned sz)
{
    if (0 == sz)
        return 0;
#if defined(_WIN32)
    return win32_root_part_len(p, sz);
#else
    return '/' == p[0] ? 1 : 0;
#endif
}

static inline unsigned root_part_len(const StrType& p)
{
    return root_part_len(p.data(), p.size());
}

bool PathName::isRelative(const base_type& path)
{
    return 0 == root_part_len(path);
}

bool PathName::isRelative(const base_type::value_type* path, unsigned len)
{
    return 0 == root_part_len(path, len);
}

inline StrType& append_dir_sep(StrType& s)
{
    typedef StrType::value_type CharType;
    const size_t sz = s.size();
    if (0 != sz) {
        CharType& c = s[sz - 1];
        if (!PathName::isDirsep(c))
            s.append(1, PathName::DIR_SEP);
#ifdef _WIN32
        else if (PathName::DIR_SEP != c)
            c = PathName::DIR_SEP;
#endif
    }
    return s;
}

inline PathName& PathName::set_dirty()
{
    dirty_ = true;
    return *this;
}

PathName& PathName::append(const char* name, bool appendDirSep)
{
    if (appendDirSep)
        append_dir_sep(*this).ustring::append(name, name + strlen(name));
    else
        ustring::append(name, name + strlen(name));
    return set_dirty();
}

PathName& PathName::append(const base_type& name, bool appendDirSep)
{
    if (appendDirSep)
        append_dir_sep(*this).append(name);
    else
        append(name);
    return set_dirty();
}

PathName& PathName::append(const PathName& name)
{
    append_dir_sep(*this).append(name.name());
    return set_dirty();
}

PathName& PathName::assign(const PathName& name)
{
    base_type::assign(name);
    return set_dirty();
}

PathName& PathName::assign(const base_type& name)
{
    base_type::assign(name);
    return set_dirty();
}

PathName& PathName::assign(const char* name)
{
    base_type::assign(name, name + strlen(name));
    return set_dirty();
}

PathName
PathName::searchPath(const base_type& nm, const base_type& path, char sep)
{
    typedef base_type::value_type ChType;
    typedef Range<const ChType*> CRange;
    if (!nm.empty() && !path.empty()) {
        IsSpace<ChType> ws;
        CRange path_range(find_longest_range_between(make_range(path), ws));
        SepRangeIter<ChType> path_iter(path_range, sep);
        for (PathName result; path_range.end() != path_iter; ++path_iter) {
            result.reserve(path_iter->size() + nm.size() + 1);
            result.base_type::assign(path_iter->begin(), path_iter->end());
            result.set_dirty().append(nm);
            if (result.exists())
                return result;
        }
    }
    return PathName();
}

template <typename CharType> int
norm_path(const Range<const CharType*>& range, CharType* dst, CharType dirSep,
          const NCRange& seps);

PathName PathName::normPath() const
{
    StrType result(*this);
    PathName::normPath(result);
    return PathName(result);
}

#ifdef _WIN32
static const char DIRSEPS[] = "\\/";
#else
static const char DIRSEPS[] = "/";
#endif

void PathName::normPath(StrType& s)
{
    unsigned dst_offset = root_part_len(s);
    if (0 < dst_offset)
        --dst_offset;
    typedef StrType::value_type CharType;
    Range<const CharType*> range(&s[dst_offset], &(*s.end()));
    int newsz = norm_path(range, &s[dst_offset], CharType(PathName::DIR_SEP),
                          make_range(DIRSEPS, sizeof(DIRSEPS) - 1));
#ifdef _WIN32
    if (0 < dst_offset)
        std::replace(&s[0], &s[dst_offset], '/', '\\');
#endif
    s.resize(newsz + dst_offset);
}

// returns true if caller should dump '..' to \a out
template <typename CharType> inline bool
rewind2(CharType*& out, CharType* dst, CharType dirSep)
{
    const int diff = out - dst;
    if (diff < 2) { // dst is like 'a' or ''
        out = dst;
        if (dirSep == *out)
            ++out;
        return true;
    }
    if ('.' == out[-1] && '.' == out[-2]) {
        if (diff == 2 || dirSep == out[-3]) {
            // dst is like '..' or '/..' or '[/..]/..'
            *out++ = PathName::DIR_SEP;
            return true;
        }
    }
    // dst is like 'foo/bar' or 'foo'
    for (--out; out != dst && dirSep != *out; --out);
    if (out == dst) { // dst is like 'foo' or '/foo'
        if (dirSep == *out)
            ++out;
        else
            return true;
    }
    // dst is like 'foo/bar'
    return false;
}

template <typename CharType> int
norm_path(const Range<const CharType*>& range, CharType* dst, CharType dirSep,
          const NCRange& seps)
{
    CharType* out = dst;
    typedef Range<const CharType*> CRange;
    IsSpace<CharType> wsp;
    CRange path_range(find_longest_range_between(range, wsp));
    if (path_range.empty())
        return 0;
    using std::find;
    const char* spos = find(seps.begin(), seps.end(), *(path_range.begin()));
    bool first_sep = (spos != seps.end());
    SetRangeIter<CharType> it(path_range, seps.begin(), seps.size());
    for (; range.end() != it; ++it) {
        if (*it == ".")
            continue;
        if (*it == "..") {
            if (rewind2(out, dst, dirSep)) {
                if (out == dst && first_sep)
                    *out++ = dirSep;
                *out++ = '.'; *out++ = '.';
            }
            continue;
        }
        bool at_begin = (out == dst);
        if ((at_begin && first_sep) || (!at_begin && dirSep != out[-1]))
            *out++ = dirSep;
        out = std::copy(it->begin(), it->end(), out);
    }
    if (out == dst && first_sep) {
        *out = dirSep;
        return 1;
    }
    return out - dst;
}

#ifdef _WIN32
static inline bool operator==(const FILETIME& lhs,
                              const FILETIME& rhs)
{
    return lhs.dwLowDateTime == rhs.dwLowDateTime &&
           lhs.dwHighDateTime == rhs.dwHighDateTime;
}
#endif

class FileId {
public:
    FileId(const PathName& p);
    //!
    bool operator ==(const FileId& other)
    {
        if (valid_ && other.valid_)
            return fuid_ == other.fuid_;
        return false;
    }
private:
    bool    valid_;
    struct fuid {
#ifdef _WIN32
        bool operator == (const fuid& o)
        {
            return
                fi_.dwFileAttributes == o.fi_.dwFileAttributes &&
                fi_.ftCreationTime == o.fi_.ftCreationTime &&
                fi_.ftLastAccessTime == o.fi_.ftLastAccessTime &&
                fi_.ftLastWriteTime == o.fi_.ftLastWriteTime &&
                fi_.dwVolumeSerialNumber == o.fi_.dwVolumeSerialNumber &&
                fi_.nFileSizeHigh == o.fi_.nFileSizeHigh &&
                fi_.nFileSizeLow == o.fi_.nFileSizeLow &&
                fi_.nNumberOfLinks == o.fi_.nNumberOfLinks &&
                fi_.nFileIndexHigh == o.fi_.nFileIndexHigh &&
                fi_.nFileIndexLow == o.fi_.nFileIndexLow;
        }
        BY_HANDLE_FILE_INFORMATION fi_;
#else
        bool operator == (const fuid& o)
        { return dev_ == o.dev_ && ino_ == o.ino_; }
        dev_t   dev_;
        ino_t   ino_;
#endif
    };
    fuid    fuid_;
};

FileId::FileId(const PathName& p)
 :  valid_(false)
{
    nstring name(local_8bit(p.name()));
#ifdef _WIN32
    HANDLE hFile = CreateFile(name.c_str(), GENERIC_READ, FILE_SHARE_READ, 0,
                              OPEN_EXISTING, 0, 0);
    if (INVALID_HANDLE_VALUE != hFile) {
        if (GetFileInformationByHandle(hFile, &fuid_.fi_))
            valid_ = true;
        CloseHandle(hFile);
    }
#else
    struct stat sb;
    if (0 == stat(name.c_str(), &sb)) {
        fuid_.dev_ = sb.st_dev;
        fuid_.ino_ = sb.st_ino;
        valid_ = true;
    }
#endif
}

bool PathName::isSameFile(const PathName& p1, const PathName& p2)
{
    return FileId(p1) == FileId(p2);
}

} // end of namespace COMMON_NS
