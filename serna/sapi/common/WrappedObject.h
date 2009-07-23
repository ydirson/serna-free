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
#ifndef COMMON_WRAPPED_OBJECT_H_
#define COMMON_WRAPPED_OBJECT_H_

#include "sapi/sapi_defs.h"
#include "sapi/SernaApiBase.h"

namespace SernaApi {

/// Base class for wrapping the internal Serna classes
class SAPI_EXPIMP SimpleWrappedObject {
public:
    SimpleWrappedObject(SernaApiBase* rep = 0);
    virtual ~SimpleWrappedObject();

    virtual void    setRep(SernaApiBase*);
    SernaApiBase*   getRep() const { return rep_; }

    bool            isNull() const { return !rep_; }
    operator        bool() const { return !isNull(); }
    void            releaseRep();

    bool operator==(const SimpleWrappedObject& o) const 
    { 
        return rep_ == o.rep_;
    }
    bool operator!=(const SimpleWrappedObject& o) const 
    {
        return !(*this == o);
    }
    bool operator<(const SimpleWrappedObject& o) const
    {
        return rep_ < o.rep_;
    }

private:
    SernaApiBase*   rep_;
};

/// Reference counted base class for wrapping the internal Serna classes
class SAPI_EXPIMP RefCountedWrappedObject : public SimpleWrappedObject {
public:
    RefCountedWrappedObject(SernaApiBase* rep = 0);
    virtual ~RefCountedWrappedObject();

    virtual void    setRep(SernaApiBase*);
    void            decRepRefCnt();

    RefCountedWrappedObject(const RefCountedWrappedObject&);
    RefCountedWrappedObject& operator=(const RefCountedWrappedObject&);

private:
    void    do_setrep(SernaApiBase*);
};

/// Interface to the owned objects wrapping internal Serna classes
class SAPI_EXPIMP OwnedWrappedObject : public SimpleWrappedObject {
public:
    OwnedWrappedObject(SernaApiBase* rep = 0);
    virtual ~OwnedWrappedObject();

    virtual void    setRep(SernaApiBase*);

    OwnedWrappedObject(const OwnedWrappedObject&);
    OwnedWrappedObject& operator=(const OwnedWrappedObject&);

private:
    void    do_setrep(SernaApiBase*);
};

#define SAPI_PVT_CONTAINER(n) \
    char*       getData() { return udata_.data_; } \
    const char* getData() const { return udata_.data_; } \
    union { char data_[sizeof(void*) * n]; int align_; } udata_;

} // namespace SernaApi

#endif // COMMON_WRAPPED_OBJECT_H_

