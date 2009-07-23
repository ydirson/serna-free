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
#ifndef DEBUG_H_
#define DEBUG_H_ 1

#ifdef _MSC_VER
# pragma warning (disable : 4251)
#endif

#include "common/common_defs.h"
#include <iostream>

class QString;

COMMON_NS_BEGIN

#if defined(SFWORKS_DLL) || defined(NDEBUG)
# define COMMON_DEBUG_EXPIMP
#else
# define COMMON_DEBUG_EXPIMP COMMON_EXPIMP
#endif

/**
 * An individual debug tag
 */
class COMMON_DEBUG_EXPIMP DebugTag {
public:
    /// Tag constructor
    DebugTag(const char* tagname,
             bool is_enabled = false,
             bool is_automatic = false);
    ~DebugTag();

    /// Returns true if tracing is enabled for this flag
    bool is_enabled() const { return enabled_; }

    /// Enable this tag
    void enable(bool v = true)  { enabled_ = v; }

    /// Disable this tag
    void disable() { enabled_ = false; }

    /// Return the name of this tag
    const char* getName() const { return id_; }

private:
    bool        enabled_;   ///< Is printing enabled for this tag?
    bool        isAutomatic_; ///< Is this instance allocated on stack?
    const char* id_;        ///< The tag's identifier
};

class COMMON_DEBUG_EXPIMP DebugModuleBase {
public:
    /// enable \a tag
    void enable(const char* tag, bool v = true);
    /// query status of \a tag
    bool is_enabled(const char* tag) const;
protected:
    DebugModuleBase();
    ~DebugModuleBase();
private:
    DebugModuleBase(const DebugModuleBase&);
    class Impl;
    Impl* impl_;
};

class DebugOutStream;

class COMMON_DEBUG_EXPIMP Debug {
public:
    typedef unsigned int size_type;

    enum { INDENT = 2 };

    static size_type getIndent();
    static size_type setIndent(size_type newind);

    static size_type incIndent(size_type delta);
    static size_type decIndent(size_type delta);

    static bool enable_tag(const char* tagname);
    static bool disable_tag(const char* tagname);
    static bool is_enabled(const char* tagname);

    static void set_tags(const char* taglist, bool val);

    static std::ostream& getOutStream();
    static void setOutStream(std::ostream& os);
    static void setOutFile(const char* fname);

    class COMMON_DEBUG_EXPIMP OSLock {
        friend class DebugOutStream;
        OSLock();
        ~OSLock();
    };
protected:
    Debug();
private:
    Debug& operator=(const Debug&);
};

class COMMON_DEBUG_EXPIMP DebugOutStream : private Debug::OSLock {
public:
    explicit DebugOutStream(std::ostream& os);
    explicit DebugOutStream();
    ~DebugOutStream();

    const DebugOutStream&
    operator << (std::ostream& (*manip)(std::ostream&)) const;

    const DebugOutStream&
    operator << (const DebugOutStream& (*manip)(const DebugOutStream&)) const;

    const DebugOutStream& indent(unsigned int ind) const
    {
        if (0 < ind) {
            std::streambuf* sbp = os_.rdbuf();
            while (ind--)
                sbp->sputc(' ');
        }
        endl_was_last_ = false;
        return *this;
    }
    const DebugOutStream& indent() const { return indent(indent_); }

    inline std::ostream& get_os() const { return os_; }
    inline void notify() const
    {
        if (endl_was_last_) { // if new line starts, print indent
            indent();
            // TODO: output time, threadid etc.
        }
    }

    inline const DebugOutStream& write(const char* buf, unsigned sz) const
    {
        notify();
        if (0 < sz)
            get_os().write(&buf[0], sz);
        return *this;
    }
protected:
    unsigned int setIndent(unsigned int ind) const;
private:
    mutable unsigned int indent_;
    mutable bool endl_was_last_;
    mutable std::ostream& os_;
};

class COMMON_DEBUG_EXPIMP DebugTrace : public DebugOutStream {
public:
    explicit DebugTrace(std::ostream& os, const DebugTag& dt);
    ~DebugTrace();
private:
     const DebugTag& dtag_;
     unsigned save_ind_;
};

class COMMON_DEBUG_EXPIMP DebugTagSet {
public:
    DebugTagSet(DebugTag& dt, bool is_enabled)
     :  dtag_(dt), wasEnabled_(dt.is_enabled())
    {
        if (is_enabled != wasEnabled_)
            dt.enable(is_enabled);
    }
    ~DebugTagSet()
    {
        dtag_.enable(wasEnabled_);
    }
private:
     DebugTag&  dtag_;
     bool       wasEnabled_;
};

/*
inline const Common::DebugOutStream&
operator << (const Common::DebugOutStream& ds, const char* s)
{
#ifndef NDEBUG
    ds.notify();
    ds.get_os() << s;
#endif
    (void) s;
    return ds;
}
*/

template<typename T> inline
const DebugOutStream& operator << (const DebugOutStream& ds, const T& obj)
{
#ifndef NDEBUG
    ds.notify();
    ds.get_os() << obj;
#endif
    (void) obj;
    return ds;
}

COMMON_EXPIMP const DebugOutStream&
operator << (const DebugOutStream& ds, const QString& qs);

template<typename T, unsigned sz> inline const DebugOutStream&
operator << (const DebugOutStream& ds, const T (&l)[sz])
{
#ifndef NDEBUG
    ds.write(&l[0], (0 == l[sz - 1]) ? sz - 1 : sz);
#endif
    (void) l;
    return ds;
}

struct DebugIndent {
    DebugIndent(unsigned int ind = Debug::INDENT) : ind_(ind)
    {
        Debug::incIndent(ind_);
    }
    ~DebugIndent() { Debug::decIndent(ind_); }
private:
    const unsigned int ind_;
};

struct ind {
    ind(unsigned int indval = Debug::INDENT) : ind_(indval) {}
    const unsigned int ind_;
};

#ifndef NDEBUG
COMMON_DEBUG_EXPIMP
#else
inline
#endif
const DebugOutStream& operator <<(const DebugOutStream& ds, const ind&)
#ifdef NDEBUG
{
    return ds;
}
#else
;
#endif

#ifndef NDEBUG
COMMON_DEBUG_EXPIMP
#else
inline
#endif
const DebugOutStream& noind(const DebugOutStream& ds)
#ifdef NDEBUG
{
    return ds;
}
#else
;
#endif

#if !defined(NDEBUG) && !defined(DEBUG_TRACE)
# define DEBUG_TRACE 1
#endif

#define DDBG DBG(DBG_DEFAULT_TAG)

inline DebugOutStream getdos() { return DebugOutStream(Debug::getOutStream()); }

#if defined(DEBUG_TRACE)

# define DEBUG_FLAG (true)

# define DBG(tag) if (DEBUG_FLAG && tag.is_enabled()) COMMON_NS::getdos()

# define DBG_CONCAT(x,y) x ## y
# define DBG_UNIVAR(x,y) DBG_CONCAT(x,y)

# define DDINDENT    auto COMMON_NS::DebugIndent DBG_UNIVAR(dbgi_,__LINE__);

# define DINDENT(di) auto COMMON_NS::DebugIndent DBG_UNIVAR(dbgi_,__LINE__)(di);

# define DBG_TRACE(tag) \
    COMMON_NS::DebugTrace \
    DBG_UNIVAR(dbgt_,__LINE__)(COMMON_NS::Debug::getOutStream(), tag); \
    if (DEBUG_FLAG && tag.is_enabled()) DBG_UNIVAR(dbgt_,__LINE__)

# define DBG_SET_TAGS(taglist, v) COMMON_NS::Debug::set_tags(taglist, v)

# define DBG_ENABLE(tag) COMMON_NS::Debug::enable_tag(tag)

# define DBG_DISABLE(tag) COMMON_NS::Debug::disable_tag(tag)

# define DBG_EXEC(tag, x) if (tag.is_enabled()) { x; }

# define DBG_IF(tag) if(tag.is_enabled())

# define DBG_OUTPUT(os) COMMON_NS::Debug::setOutStream(os)

# define DBG_OUTFILE(file) COMMON_NS::Debug::setOutFile(file)

# define DBG_DEF(modname) DebugTags_ ## modname modname

# define DBG_DEF_DLL(mod, expimp) expimp DebugTags_ ## mod mod

# define DBG_TAG(module, tag, is_enabled) \
    COMMON_NS::DebugTag module ## _ ## tag \
        (#module "." #tag, is_enabled, true)

# define DBG_SCOPED_ENABLE(tag, is_enabled) \
    COMMON_NS::DebugTagSet \
    DBG_UNIVAR(dbg_tag_set_,__LINE__)(tag, is_enabled)

#else

# define DEBUG_FLAG (false)

# if defined(__GNUG__) && (__GNUG__ <= 3)
#  define DBG(tag) if (false) COMMON_NS::getdos()
# else
#  define DBG(tag) \
    if (false) \
        COMMON_NS::DebugOutStream(COMMON_NS::Debug::getOutStream())
# endif

# define DBG_TRACE(tag) DBG(tag)
# define DDINDENT
# define DINDENT(di)
# define DBG_SET_TAGS(taglist, v)
# define DBG_ENABLE(tag)
# define DBG_DISABLE(tag)
# define DBG_EXEC(tag, x)
# define DBG_IF(tag) if (false)
# define DBG_OUTPUT(os)
# define DBG_OUTFILE(file)
# define DBG_DEF(modname)
# define DBG_DEF_DLL(modname, expimp)
# define DBG_TAG(module, tag, is_enabled)
# define DBG_SCOPED_ENABLE(tag, is_enabled)
//struct DBG_DEF_ENBL {
//    bool is_enabled() {return false;}
//} x_enbl;
//# define DBG_DEFAULT_TAG
//x_enbl
#endif

#define DBG_SCOPED_SET(tag, is_enabled) DBG_SCOPED_ENABLE(tag, is_enabled)

COMMON_NS_END

template<typename T> struct brktd {
    brktd(const T& t, char l, char r) : t_(t), l_(l), r_(r) {}
    const T& t_;
    char l_, r_;
};

template<class T> inline std::ostream&
operator <<(std::ostream& os, const brktd<T>& brval)
{
    return os << brval.l_ << brval.t_ << brval.r_;
}

template<class T> inline brktd<T>
gbr(const T& t, char l, char r) { return brktd<T>(t, l, r); }

template<class T> inline brktd<T> abr(const T& t) { return gbr(t, '<', '>'); }
template<class T> inline brktd<T> rbr(const T& t) { return gbr(t, '(', ')'); }
template<class T> inline brktd<T> sbr(const T& t) { return gbr(t, '[', ']'); }
template<class T> inline brktd<T> sqt(const T& t) { return gbr(t, '\'', '\''); }
template<class T> inline brktd<T> dqt(const T& t) { return gbr(t, '"', '"'); }
template<class T> inline brktd<T> bqt(const T& t) { return gbr(t, '`', '`'); }

#endif // DEBUG_H_

#ifdef _DEBUG
//------------------------------------------------------------------------------
//
// NOTE: The following code uses DBG_MODULE and DBG_TAGS
//       to generate trace module declaration
//
#if defined(DBG_MODULE) || defined(DBG_TAGS)

#ifndef DBG_TAGS
# error "DBG_TAGS is not defined!"
# define DBG_TAGS
#endif

#ifndef DBG_MODULE
# error "DBG_MODULE is not defined!"
# define DBG_MODULE __dummy_trace__
#endif

#ifndef DBG_DEFAULT_TAG
# error "DBG_DEFAULT_TAG is not defined!"
#endif

#ifndef DBG_EXPIMP
# define DBG_EXPIMP
#endif

#define DMOD(modname) DebugTags_ ## modname
class DBG_EXPIMP DBG_MODULE {
public:
    /// Constructor
    DBG_MODULE() :
#undef DMOD
#define DMOD(modname) #modname
#define DTAG(tag_id)  tag_id(DBG_MODULE "." #tag_id),
        DBG_TAGS
#undef DTAG
#undef DMOD
        dummy_(0)
    { }

    // Tag fields
#define DTAG(tag_id) COMMON_NS::DebugTag tag_id;
    DBG_TAGS
#undef DTAG

private:
    int dummy_;     ///< A dummy field, to get around a problem with closing comma
};

//
// The trace module singleton declaration
//
#define DMOD(modname) extern DBG_EXPIMP DebugTags_ ## modname modname;
DBG_MODULE
#undef DMOD

#undef DBG_EXPIMP
#undef DBG_MODULE
#undef DBG_TAGS

#endif

#endif // defined(DBG_MODULE) || defined(DBG_TAGS)
