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

#include "SpExtIdResolver.h"
#include "SpUtils.h"
#include "SpCatalogDtd.h"
#include "sp_debug.h"

#include "common/common_defs.h"
#include "common/String.h"
#include "common/StringUtils.h"
#include "common/StringCvt.h"
#include "common/MessageUtils.h"
#include "common/PathName.h"
#include "catmgr/CatalogManager.h"

#include "sp/config.h"
#include "sp/ExternalIdResolver.h"
#include "sp/StringC.h"

#include <algorithm>
#include <iostream>

#ifdef DBG_DEFAULT_TAG
# undef DBG_DEFAULT_TAG
#endif

#define DBG_DEFAULT_TAG SPGB.RESOLV

GROVE_NAMESPACE_BEGIN

using CatMgr::CatalogManager;
using namespace Common;
using namespace std;

SpExtIdResolver::SpExtIdResolver(const CatalogManager& catmgr, Messenger* msgr)
 : catMgr_(catmgr), messenger_(msgr)
{
}

SpExtIdResolver::~SpExtIdResolver()
{
}

static inline bool isxdigit(const Char& c) 
{
    return c.isDigit() || (c.unicode() >= 'A' && c.unicode() <= 'F') ||
        (c.unicode() >= 'a' && c.unicode() <= 'f');
}

static String get_path(const Char* p, const Char* e);

static String unescape(const Char* p, const Char* end)
{
    String result;
    result.reserve(end - p);
    for (; p < end; ++p) {
        if ('%' == *p && end - p > 2 && isxdigit(p[1]) && isxdigit(p[2])) {
            const char buf[] = { p[1].toLatin1(), p[2].toLatin1(), '\0' };
            char* eptr(0);
            int ch = strtoul(buf, &eptr, 16);
            if (eptr && '\0' == *eptr) {
                result += Char(ch);
                p += 2;
                continue;
            }
        }
        result += *p;
    }
    return result;
}

static const char OSFILE[] = "<OSFILE";
static const char SMCRD[] = " SMCRD='^'>";

static PathName makeSysIdPath(const String& spSysid)
{
    String sysPath;
    const Char* p(spSysid.begin()), *end(spSysid.end());

    if (starts_with(spSysid, "file://"))
        sysPath = get_path(p, end);
    else {
        if (end <= (p + sizeof(OSFILE))
            || compare(p, sizeof(OSFILE) - 1, OSFILE))
            return sysPath;
        p += sizeof(OSFILE) - 1;
        if (p >= end)
            return sysPath;
        if ('>' == *p)
            sysPath.assign(ustring(++p, end));
        else if (end > p + sizeof(SMCRD)
                 && !compare(p, sizeof(SMCRD) - 1, SMCRD)) {
            p += sizeof(SMCRD) - 1;
            ustring path;
            path.reserve(end - p);
            for (; p < end; ++p) {
                if ('^' == *p) {
                    const Char* psmc = std::find(p, end, ';');
                    if (psmc != end) {
                        bool ok = false;
                        int code = stoi(p + 1, psmc - p - 1, 10, &ok);
                        if (ok) {
                            path += Char(code);
                            p = psmc;
                            continue;
                        }
                    }
                }
                path += *p;
            }
            sysPath.assign(path);
        }
    }
#ifdef _WIN32
    replace(sysPath.begin(), sysPath.end(), '\\', '/');
#endif
    return sysPath;
}

static const char* CAT_PUBIDS[] = {
    "-//OASIS//DTD XML Catalogs V1.0//EN",
    "-//OASIS//DTD Entity Resolution XML Catalog V1.0//EN",
    "urn:publicid:-:OASIS:DTD+XML+Catalogs+V1.0:EN",
    0
};

template<typename T, unsigned N> unsigned array_sz(const T (&array)[N])
{
    return sizeof(array)/sizeof(T);
}

static bool is_cat_id(const String& idStr)
{
    return 0 != *find(CAT_PUBIDS, CAT_PUBIDS + array_sz(CAT_PUBIDS) - 1, idStr);
}

bool SpExtIdResolver::resolveExternalId(const Sp::StringC& pubid,
                                        const Sp::StringC& sysid,
                                        Sp::StringC& result) const
{
    String pid(&pubid ? SpUtils::makeString(pubid) : String::null());
    String sid(&sysid ? SpUtils::makeString(sysid) : String::null());
    DDBG << "SpExtIdResolver got pubid: " << pid << ", sysid: " << sid << endl;
    if (is_cat_id(sid) || is_cat_id(pid)) {
        ustring t;
        PathName sysIdPath(makeSysIdPath(SpUtils::makeString(result)));
        if (sysIdPath.exists()) {
            DDBG << "Catalog sysid '" << sysIdPath.name() << "' exists" << endl;
            str_assign(t, "<OSFILE>");
            t.append(sysIdPath.name().begin(), sysIdPath.name().size());
        }
        else
            t.assign(CATALOG_DTD_data, CATALOG_DTD_data + CATALOG_DTD_size);
        result.assign(reinterpret_cast<const Sp::Char*>(t.data()), t.size());
        return true;
    }
    String extid;
    if (0 == &catMgr_) {
        DDBG << "Catalog manager is not set" << endl;
    }
    else {
        extid = catMgr_.resolveExternalId(pid, sid, &*messenger_);
        DDBG << "Catalog manager returned '" << extid << '\'' << endl;
    }
    if (extid.empty())
        extid = sid;
    if (!extid.empty()) {
        ustring buf;
        if (starts_with(extid, "http://"))
            str_assign(buf, "<URL>");
        else if (starts_with(extid, "file://")) {
            extid = get_path(extid.begin(), extid.end());
            str_assign(buf, "<OSFILE>");
        }
        else
            return false;
        buf += extid;
        result.assign(reinterpret_cast<Sp::Char*>(&buf[0]), buf.size());
        return true;
    }
    return false;
}

static String get_path(const Char* p, const Char* e)
{
    p += sizeof("file://") - 1;
    String path(unescape(p, e));
    p = path.begin();
    e = path.end();
    if (e > p) {
#if defined(_WIN32)
        if ('/' == p[0]) {
            if (e - p > 2 && isalpha(p[1].unicode()) && ':' == p[2])
                ++p;
        }
        else
            p -= 2;
#elif defined(__APPLE__)
        static const char MACHD[] = "/Macintosh HD/";
        const unsigned MACHD_LEN(sizeof(MACHD) - 1);
        if ('/' == p[0] && e > (p + MACHD_LEN)) {
            if (!compare(MACHD, MACHD_LEN, p, MACHD_LEN))
                p += MACHD_LEN - 1;
        }
#endif
    }
    return String(p, e - p);
}

GROVE_NAMESPACE_END
