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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#if !defined(USE_PCH)
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "grove/Nodes.h"
# include "grove/XmlName.h"

# include "common/PathName.h"
# include "common/String.h"
# include "common/StringUtils.h"
# include "common/StringCvt.h"

# include <algorithm>
# include <cstring>
# include <time.h>
# include <sys/stat.h>
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

extern const char CAT_NS_URI[] = "urn:oasis:names:tc:entity:xmlns:xml:catalog";

using namespace Common;
using namespace GroveLib;

const String& get_attr(const Element* ep, const String& name)
{
    const Attr* attr = ep->attrs().firstChild();
    if (name.find(':') < 0) {
        for (; attr; attr = attr->nextSibling())
            if (attr->localName() == name && attr->xmlNsUri().isEmpty())
                return attr->value();
    } else {
        attr = ep->attrs().getAttribute(name);
        if (attr)
            return attr->value();
    }
    return String::null();
}

bool in_catalog_ns(const Element* ep)
{
    return ep->isSameNs(from_latin1(CAT_NS_URI, sizeof(CAT_NS_URI) - 1));
}

bool elem_has_name(const GroveLib::Element* ep, const char* name)
{
    return ep->expandedName() == ExpandedName(from_latin1(name), CAT_NS_URI);
}

static const char URI_ROOT_START[] = "://";

String uri_from_path(const String& path)
{
    ustring abspath(PathName(path).absolute().name());
#ifdef _WIN32
    std::replace(abspath.begin(), abspath.end(), '\\', '/');
    if (':' == abspath[1])
        abspath.insert(0U, 1U, Char('/'));
    else
        abspath.erase(0, 2);
#endif
    return str_insert(abspath, 0, "file://");
}

typedef Range<const Char*> CRange;

static CRange get_scheme(const String& uri)
{
    unsigned urs = uri.find(URI_ROOT_START);
    if (ustring::npos == urs)
        return make_range(uri.begin(), 0U);
    return make_range(uri.begin(), urs + sizeof(URI_ROOT_START) - 1);
}

static CRange get_host(const String& uri)
{
    CRange r(get_scheme(uri));
    if (!r.empty() && r.end() < uri.end() && *r.end() != '/') {
        CRange host(find_first_range_not_of(r.end(), uri.end(),
                                            IsChar<Char>('/')));
        if (!host.empty())
            return host;
    }
    return make_range(r.end(), 0U);
}

static CRange get_root(const String& uri)
{
    CRange host(get_host(uri));
    return CRange(uri.begin(), host.end());
}

static CRange get_path(const String& uri)
{
    const char FSEP[] = { '?', '#' };
    CRange r(get_host(uri));
    const Char* lb = r.end();
#ifdef _WIN32
    if (uri.end() - lb > 2 && '/' == lb[0] && isalpha(lb[1].unicode())
        && ':' == lb[2])
        ++lb;
#endif
    const Char* rb = std::find_first_of(lb, uri.end(),
                                        FSEP, FSEP + sizeof(FSEP));
    if (rb != lb)
        return make_range(lb, rb);
    return make_range(r.end(), 0U);
}

String get_base_uri(const String& uri)
{
    CRange path(get_path(uri));
    if (!path.empty()) {
        for (--path.second; !path.empty(); --path.second) {
            if ('/' == *path.second)
                return String(uri.begin(), path.end() - uri.begin());
        }
    }
    return range_ctor<ustring>(get_root(uri));
}

static bool is_relative(const String& uriRef)
{
    CRange scheme(get_scheme(uriRef));
    if (scheme.empty())
        return true;
    scheme.second -= sizeof(URI_ROOT_START) - 1;
    for (; !scheme.empty(); ++scheme.first) {
        if (!isalpha(to_int_type(*(scheme.begin()))))
            return true;
    }
    return false;
}

String make_abs_uri(const String& uriRef)
{
    if (!is_relative(uriRef))
        return uriRef;
    return uri_from_path(uriRef);
}

template<typename RndIter, class StrType> inline StrType&
operator+=(StrType& s, const Range<RndIter>& r)
{
    return s.append(r.begin(), r.end());
}

String rebase_uri(const String& base, const String& uriRef)
{
    if (uriRef.empty())
        return base;
    if (base.empty() || !is_relative(uriRef))
        return uriRef;
    CRange path(get_path(uriRef));
    if (path.empty())
        return base;
    ustring rv;
    if ('/' == *path.begin()) {
        CRange root(get_root(base));
        rv.reserve(root.size() + path.size());
        rv += root;
    }
    else {
        rv.reserve(base.size() + path.size() + 1);
        rv += base;
        if ('/' != base[base.size() - 1])
            rv += '/';
    }
    return rv += path;
}

bool is_file_uri(const String& uri)
{
    CRange scheme(get_scheme(uri));
    return scheme.empty() || "file://" == to_lower<nstring>(scheme);
}

String get_filepath(const String& uri)
{
    return range_ctor<ustring>(get_path(uri));
}

static const char RES[] = "\"<>\\^`{}|\x7F";

static inline bool must_escape(unsigned char c)
{
    return c >= 0x80 || c < 0x20 || *std::find(RES, RES + sizeof(RES) - 1, c);
}

String norm_uri(const String& uri)
{
    String result, digits;
    nstring uriUtf8(utf8(uri));
    result.reserve(uriUtf8.size());
    for (const char* p = uriUtf8.begin(); p < uriUtf8.end(); ++p) {
        if (must_escape(*p)) {
            result.push_back('%');
            set_integer(digits, *p, 16);
            result += to_upper<String>(digits);
        }
        else
            result.push_back(*p);
    }
    return result;
}

String norm_pubid(const String& pubid)
{
    return simplify_white_space(pubid);
}

static const char URN_START[] = "urn:publicid:";

bool in_urn_ns(const String& s)
{
    return URN_START == to_lower<nstring>(s.substr(0, sizeof(URN_START) - 1));
}

String unwrap_id(const String& idStr)
{
    static const unsigned char UNWRAP[] = "+:/;'?#%";
    String result;
    result.reserve(idStr.size());
    Range<const Char*> r(idStr.begin() + sizeof(URN_START) - 1, idStr.end());
    for (; !r.empty(); ++r.first) {
        Char srcCh(*r.begin());
        switch (to_int_type(srcCh)) {
            case '+':
                result += ' ';
                continue;
            case ':':
                result += "//";
                continue;
            case ';':
                result += "::";
                continue;
            case '%': {
                if (r.size() < 3) {
                    result.append(r.begin(), r.size());
                    return result;
                }
                bool ok = true;
                unsigned char c = stoi(r.begin() + 1,
                                       static_cast<unsigned char>(2), 16, &ok);
                if (ok && *std::find(UNWRAP, UNWRAP + sizeof(UNWRAP) - 1, c)) {
                    result += c;
                    r.first += 3;
                    continue;
                }
                break;
            }
            default:
                break;
        };
        result += srcCh;
    }
    return result;
}

#ifdef _WIN32
# define _stat stat
#endif

unsigned get_modtime(const String& uri)
{
    if (!is_file_uri(uri))
        return 0;
    nstring filepath(range_ctor<nstring>(get_path(uri)));
    struct stat s;
    int err = stat(filepath.c_str(), &s);
    return 0 == err ? s.st_mtime : 0;
}

}
