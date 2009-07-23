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
 *  Debug framework implementation
 *
 */

#include "common/common_defs.h"
#include "common/OwnerPtr.h"
#include "common/Debug.h"
#include "common/ThreadingPolicies.h"
#include "common/AssocVector.h"
#include "common/Singleton.h"

#include <QString>
#include <QByteArray>
#include <algorithm>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

COMMON_NS_BEGIN

typedef unsigned int size_type;
using namespace std;

struct TagCmp : public std::binary_function<const char*, const char*, bool> {

    inline bool operator()(const char* tag1, const char* tag2) const
    {
        return 0 > strcmp(tag1, tag2);
    }
};

typedef AssocVector<const char*, DebugTag*, TagCmp> TagMap;

struct TagCmpN : public std::binary_function<const TagMap::value_type&,
                                             const char*, bool> {

    TagCmpN(unsigned int size) : size_(size) {}

    inline bool operator()(const TagMap::value_type& val, const char* tag) const
    {
        return 0 > strncmp(val.first, tag, size_);
    }

    inline bool operator()(const char* tag, const TagMap::value_type& val) const
    {
        return 0 < strncmp(val.first, tag, size_);
    }
private:
    unsigned int size_;
};

class DebugPrivate : public DEFAULT_THREADING_CL<> {
public:
    DebugPrivate() : indent_(0), osp_(&cerr)
    {
        ThreadMutexClass::initialize(&os_lk_);
    }
    ~DebugPrivate()
    {
        ThreadMutexClass::destroy(&os_lk_);
    }

    typedef unsigned int size_type;

    inline size_type getIndent() const { return indent_; }
    inline size_type setIndent(size_type newind)
    {
        Lock lk;
        return indent_ = newind;
    }

    inline size_type incIndent(size_type delta)
    {
        Lock lk;
        return indent_ += delta;
    }
    inline size_type decIndent(size_type delta)
    {
        Lock lk;
        if (indent_ >= delta)
            indent_ -= delta;
        else
            indent_ = 0;
        return indent_;
    }

    inline void add_tag(DebugTag* tag)
    {
        Lock lk;
        map_.insert(TagMap::value_type(tag->getName(), tag));
    }

    inline void del_tag(const char* tagname)
    {
        Lock lk;
        map_.erase(tagname);
    }

    bool enable_tag(const char* tagname);
    bool disable_tag(const char* tagname);
    bool is_enabled(const char* tagname) const;
    bool set_tags(const char* taglist, bool val);

    inline std::ostream& getOutStream() const
    {
        Lock lk;
        if (0 != osp_)
            return *osp_;
        else
            return cerr;
    }
    inline void setOutStream(std::ostream& os)
    {
        Lock lk;
        ofsp_.clear();
        osp_ = &os;
    }

    inline bool setOutFile(const char* fname)
    {
        ofsp_ = new ofstream(fname);
        if (!ofsp_->good()) {
            ofsp_.clear();
            return false;
        }
        else
            osp_ = ofsp_.pointer();
        return true;
    }

    inline void os_lock() { ThreadMutexClass::lock(&os_lk_); }
    inline void os_unlock() { ThreadMutexClass::unlock(&os_lk_); }

private:
    typedef TagMap::value_type::second_type dtag_t;
    inline dtag_t find_tag(const char* tagname) const
    {
        TagMap::const_iterator it = map_.find(tagname);
        if (map_.end() != it)
            return it->second;
        return 0;
    }

    MutexType os_lk_;
    size_type indent_;
    ostream* osp_;
    TagMap map_;
    OwnerPtr<ofstream> ofsp_;
};

inline DebugPrivate& dbg_private()
{
    typedef SingletonHolder<DebugPrivate, CreateUsingNew<DebugPrivate>,
            PhoenixSingleton<DebugPrivate> > DbgSingleton;
    return DbgSingleton::instance();
}

inline bool
DebugPrivate::is_enabled(const char* tagname) const
{
    if (dtag_t dt = find_tag(tagname))
        return dt->is_enabled();
    return false;
}

inline bool
DebugPrivate::disable_tag(const char* tagname)
{
    Lock lk;
    if (dtag_t dt = find_tag(tagname)) {
        dt->disable();
        return true;
    }
    return false;
}

inline bool
DebugPrivate::enable_tag(const char* tagname)
{
    Lock lk;
    if (dtag_t dt = find_tag(tagname)) {
        dt->enable();
        return true;
    }
    return false;
}

struct TagValSet {
    TagValSet(bool b) : set_(b) {}
    inline void operator()(TagMap::value_type& val)
    {
        if (set_) val.second->enable();
        else val.second->disable();
    }
private:
    bool set_;
};

inline bool
set_tag(TagMap& tmap, const string& tag, bool b)
{
    TagMap::iterator it = tmap.find(tag.c_str());
    if (tmap.end() != it) {
        DebugTag& dtag = *(it->second);
        if (b) dtag.enable();
        else dtag.disable();
        return true;
    }
    return false;
}

inline bool
DebugPrivate::set_tags(const char* tagname, bool val)
{
    if (0 == tagname)
        return false;

    bool retval = true;
    static const char ws[] = " \t\n\r\f\v,";
    string tag;
    const char* begp = tagname + strspn(tagname, ws);
    const char* endp = begp + strcspn(begp, ws);
    while ('\0' != *begp) {
        tag.assign(begp, endp);
        if (1 == tag.size() && '*' == tag[0]) {
            for_each(map_.begin(), map_.end(), TagValSet(val));
            return true;
        }
        string::size_type starpos = tag.npos;
        string::size_type dotpos = tag.find('.');
        if (tag.npos != dotpos) { // tag is "module.xxx"
            const string::size_type sz = tag.size() - 1;
            if (dotpos == sz || (dotpos == sz - 1 && '*' == tag[sz])) {
                // tag is "module." or "module.*"
                starpos = dotpos + 1;
            }
            else // tag is "module.tagname"
                retval &= set_tag(map_, tag, val);
        }
        else {  // tag is "module"
            tag.append(1, '.');
            starpos = tag.size();
        }
        if (starpos != tag.npos) { // wildcard tag
            // find range of tags for "module"
            pair<TagMap::iterator, TagMap::iterator> range =
                equal_range(map_.begin(), map_.end(), tag.data(),
                            TagCmpN(starpos));
            // set each tag to val
            for_each(range.first, range.second, TagValSet(val));
        }
        begp = endp + strspn(endp, ws);
        endp = begp + strcspn(begp, ws);
    }
    return retval;
}

// DebugOutStream

DebugOutStream::DebugOutStream(ostream& os)
    : indent_(Debug::getIndent()), endl_was_last_(true), os_(os)
{
}

DebugOutStream::DebugOutStream()
    : indent_(Debug::getIndent()), endl_was_last_(true),
      os_(Debug::getOutStream())
{
}

DebugOutStream::~DebugOutStream()
{
    try {
        os_ << std::flush;
    }
    catch(...) {}
}

#if 0
const DebugOutStream&
DebugOutStream::indent(unsigned int ind) const
{
    if (0 < ind) {
        std::streambuf* sbp = os_.rdbuf();
        while (ind--)
            sbp->sputc(' ');
    }
    endl_was_last_ = false;
    return *this;
}

void
DebugOutStream::notify() const
{
    if (endl_was_last_) { // if new line starts, print indent
        indent();
        // TODO: output time, threadid etc.
    }
}

const DebugOutStream& DebugOutStream::write(const char* buf, unsigned sz) const
{
    notify();
    if (0 < sz)
        get_os().write(&buf[0], sz);
    return *this;
}
#endif

unsigned int
DebugOutStream::setIndent(unsigned int ind) const
{
    return indent_ = ind;
}

// DebugTrace
DebugTrace::DebugTrace(std::ostream& os, const DebugTag& dt)
 :  DebugOutStream(os), dtag_(dt), save_ind_(Debug::getIndent())
{
    if (dt.is_enabled()) {
        const DebugOutStream& ds = *this;
        ds << "===>" << std::endl;
        setIndent(Debug::incIndent(2));
    }
}

DebugTrace::~DebugTrace()
{
    try {
        setIndent(Debug::setIndent(save_ind_));
        if (dtag_.is_enabled()) {
            const DebugOutStream& ds = *this;
            ds << "<===" << std::endl;
        }
    }
    catch(...) {}
}

// Debug::OSLock

Debug::OSLock::OSLock()
{
    dbg_private().os_lock();
}

Debug::OSLock::~OSLock()
{
    dbg_private().os_unlock();
}

// Debug static methods

void
Debug::set_tags(const char* taglist, bool val)
{
    dbg_private().set_tags(taglist, val);
}

size_type Debug::getIndent() { return dbg_private().getIndent(); }
size_type Debug::setIndent(size_type newind)
{
    return dbg_private().setIndent(newind);
}

size_type
Debug::incIndent(size_type d)
{
    return dbg_private().incIndent(d);
}

size_type
Debug::decIndent(size_type d)
{
    return dbg_private().decIndent(d);
}

bool
Debug::enable_tag(const char* tagname)
{
    return dbg_private().enable_tag(tagname);
}

bool
Debug::disable_tag(const char* tagname)
{
    return dbg_private().disable_tag(tagname);
}

bool
Debug::is_enabled(const char* tagname)
{
    return dbg_private().is_enabled(tagname);
}

std::ostream&
Debug::getOutStream()
{
    return dbg_private().getOutStream();
}

void
Debug::setOutStream(std::ostream& os)
{
    dbg_private().setOutStream(os);
}

void
Debug::setOutFile(const char* file)
{
    dbg_private().setOutFile(file);
}

DebugTag::DebugTag(const char* tagname, bool is_enabled, bool is_automatic)
 :  enabled_(is_enabled), isAutomatic_(is_automatic), id_(tagname)
{
    dbg_private().add_tag(this);
}

DebugTag::~DebugTag()
{
    if (isAutomatic_)
        dbg_private().del_tag(getName());
}

#ifndef NDEBUG
const DebugOutStream& noind(const DebugOutStream& ds)
{
    return ds.indent(0);
}

const DebugOutStream& operator <<(const DebugOutStream& ds, const ind& indval)
{
    ds.notify();
    return ds.indent(indval.ind_);
}
#endif

const DebugOutStream&
DebugOutStream::operator << (std::ostream& (*manip)(std::ostream&)) const
{
    typedef std::ostream& (*manip_t)(std::ostream&);
    manip(get_os());
    if ((manip_t)std::endl == manip)
        endl_was_last_ = true;
    return *this;
}

const DebugOutStream& DebugOutStream::operator
<< (const DebugOutStream& (*manip)(const DebugOutStream&)) const
{
    return manip(*this);
}

const DebugOutStream&
operator << (const DebugOutStream& dos, const QString& qs)
{
    const QByteArray qba(qs.toLatin1());
    return dos.write(qba.data(), qba.size());
}

COMMON_NS_END
