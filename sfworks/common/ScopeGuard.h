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

#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

#include "common/common_defs.h"

COMMON_NS_BEGIN

template <class T> class RefHolder {
    T& ref_;
public:
    RefHolder(T& ref) : ref_(ref) {}
    operator T& () const
    {
        return ref_;
    }
};

template <class T> inline RefHolder<T> ByRef(T& t)
{
    return RefHolder<T>(t);
}

class ScopeGuardImplBase {
    DEFAULT_ASSIGN_OP_DECL(ScopeGuardImplBase)
protected:
    ~ScopeGuardImplBase() {}
    ScopeGuardImplBase(const ScopeGuardImplBase& other) throw()
     :  dismissed_(other.dismissed_)
    {
        other.dismiss();
    }
    template <typename J> static void safeExecute(J& j) throw()
    {
        if (!j.dismissed_)
            try {
                j.execute();
            }
            catch(...) {}
    }
    //!
    mutable bool dismissed_;
    //!
    ScopeGuardImplBase() throw() : dismissed_(false) {}
public:
    void dismiss() const throw() { dismissed_ = true; }
};

typedef const ScopeGuardImplBase& ScopeGuard;

template <typename T, typename V>
class ValScopeGuard : public ScopeGuardImplBase {
public:
    ValScopeGuard(T& lvalue, const V& rvalue)
     :  lvalue_(lvalue), oldValue_(lvalue)
    {
        lvalue = rvalue;
    }
    ~ValScopeGuard() { safeExecute(*this); }
    void execute() { lvalue_ = oldValue_; }
private:
    T& lvalue_;
    T oldValue_;
};

template <typename T, typename V>
ValScopeGuard<T, V> makeValGuard(T& lvalue, const V& rvalue)
{
    return ValScopeGuard<T, V>(lvalue, rvalue);
}

template <typename F> class ScopeGuardImpl0 : public ScopeGuardImplBase {
public:
    static ScopeGuardImpl0<F> makeGuard(F fun)
    {
        return ScopeGuardImpl0<F>(fun);
    }
    ~ScopeGuardImpl0() throw() { safeExecute(*this); }
    //!
    void execute() { fun_(); }
protected:
    ScopeGuardImpl0(F fun) : fun_(fun) {}
    F fun_;
};

template <typename F> inline ScopeGuardImpl0<F> makeGuard(F fun)
{
    return ScopeGuardImpl0<F>::makeGuard(fun);
}

template <typename F, typename P1>
class ScopeGuardImpl1 : public ScopeGuardImplBase {
public:
    static ScopeGuardImpl1<F, P1> makeGuard(F fun, P1 p1)
    {
        return ScopeGuardImpl1<F, P1>(fun, p1);
    }
    ~ScopeGuardImpl1() throw() { safeExecute(*this); }
    //!
    void execute() { fun_(p1_); }
protected:
    ScopeGuardImpl1(F fun, P1 p1) : fun_(fun), p1_(p1) {}
    F fun_;
    const P1 p1_;
};

template <typename F, typename P1>
inline ScopeGuardImpl1<F, P1> makeGuard(F fun, P1 p1)
{
    return ScopeGuardImpl1<F, P1>::makeGuard(fun, p1);
}

template <typename F, typename P1, typename P2>
class ScopeGuardImpl2: public ScopeGuardImplBase {
public:
    static ScopeGuardImpl2<F, P1, P2> makeGuard(F fun, P1 p1, P2 p2)
    {
        return ScopeGuardImpl2<F, P1, P2>(fun, p1, p2);
    }
    ~ScopeGuardImpl2() throw() { safeExecute(*this); }
    //!
    void execute() { fun_(p1_, p2_); }
protected:
    ScopeGuardImpl2(F fun, P1 p1, P2 p2) : fun_(fun), p1_(p1), p2_(p2) {}
    F fun_;
    const P1 p1_;
    const P2 p2_;
};

template <typename F, typename P1, typename P2>
inline ScopeGuardImpl2<F, P1, P2> makeGuard(F fun, P1 p1, P2 p2)
{
    return ScopeGuardImpl2<F, P1, P2>::makeGuard(fun, p1, p2);
}

template <typename F, typename P1, typename P2, typename P3>
class ScopeGuardImpl3 : public ScopeGuardImplBase {
public:
    static ScopeGuardImpl3<F, P1, P2, P3> makeGuard(F fun, P1 p1, P2 p2, P3 p3)
    {
        return ScopeGuardImpl3<F, P1, P2, P3>(fun, p1, p2, p3);
    }
    ~ScopeGuardImpl3() throw() { safeExecute(*this); }
    //!
    void execute() { fun_(p1_, p2_, p3_); }
protected:
    ScopeGuardImpl3(F fun, P1 p1, P2 p2, P3 p3)
     :  fun_(fun), p1_(p1), p2_(p2), p3_(p3) {}
    F fun_;
    const P1 p1_;
    const P2 p2_;
    const P3 p3_;
};

template <typename F, typename P1, typename P2, typename P3>
inline ScopeGuardImpl3<F, P1, P2, P3> makeGuard(F fun, P1 p1, P2 p2, P3 p3)
{
    return ScopeGuardImpl3<F, P1, P2, P3>::makeGuard(fun, p1, p2, p3);
}

//************************************************************

template <class Obj, typename MemFun>
class ObjScopeGuardImpl0 : public ScopeGuardImplBase {
public:
    static ObjScopeGuardImpl0<Obj, MemFun> makeObjGuard(Obj& obj, MemFun memFun)
    {
        return ObjScopeGuardImpl0<Obj, MemFun>(obj, memFun);
    }
    ~ObjScopeGuardImpl0() throw() { safeExecute(*this); }
    //!
    void execute() { if (&obj_) (obj_.*memFun_)(); }
protected:
    ObjScopeGuardImpl0(Obj& obj, MemFun memFun) : obj_(obj), memFun_(memFun) {}
    Obj& obj_;
    MemFun memFun_;
};

template <class Obj, typename MemFun>
inline ObjScopeGuardImpl0<Obj, MemFun> makeObjGuard(Obj& obj, MemFun memFun)
{
    return ObjScopeGuardImpl0<Obj, MemFun>::makeObjGuard(obj, memFun);
}

template <class Obj, typename MemFun, typename P1>
class ObjScopeGuardImpl1 : public ScopeGuardImplBase {
public:
    static ObjScopeGuardImpl1<Obj, MemFun, P1>
    makeObjGuard(Obj& obj, MemFun memFun, P1 p1)
    {
        return ObjScopeGuardImpl1<Obj, MemFun, P1>(obj, memFun, p1);
    }
    ~ObjScopeGuardImpl1() throw() { safeExecute(*this); }
    //!
    void execute() { if (&obj_) (obj_.*memFun_)(p1_); }
protected:
    ObjScopeGuardImpl1(Obj& obj, MemFun memFun, P1 p1)
        : obj_(obj), memFun_(memFun), p1_(p1) {}
    Obj& obj_;
    MemFun memFun_;
    const P1 p1_;
};

template <class Obj, typename MemFun, typename P1>
inline ObjScopeGuardImpl1<Obj, MemFun, P1>
makeObjGuard(Obj& obj, MemFun memFun, P1 p1)
{
    return ObjScopeGuardImpl1<Obj, MemFun, P1>::makeObjGuard(obj, memFun, p1);
}

template <class Obj, typename MemFun, typename P1, typename P2>
class ObjScopeGuardImpl2 : public ScopeGuardImplBase {
public:
    static ObjScopeGuardImpl2<Obj, MemFun, P1, P2>
    makeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
    {
        return ObjScopeGuardImpl2<Obj, MemFun, P1, P2>(obj, memFun, p1, p2);
    }
    ~ObjScopeGuardImpl2() throw() { safeExecute(*this); }
    //!
    void execute() { if (&obj_) (obj_.*memFun_)(p1_, p2_); }
protected:
    ObjScopeGuardImpl2(Obj& obj, MemFun memFun, P1 p1, P2 p2)
        : obj_(obj), memFun_(memFun), p1_(p1), p2_(p2) {}
    Obj& obj_;
    MemFun memFun_;
    const P1 p1_;
    const P2 p2_;
};

template <class Obj, typename MemFun, typename P1, typename P2>
inline ObjScopeGuardImpl2<Obj, MemFun, P1, P2>
makeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
{
    return ObjScopeGuardImpl2<Obj, MemFun, P1, P2>::makeObjGuard(obj, memFun,
                                                                 p1, p2);
}

COMMON_NS_END

#ifdef __GNUC__
# define UNUSED_VAR __attribute__ ((__unused__))
#else
# define UNUSED_VAR
#endif

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANON_VARIABLE(str) CONCATENATE(str, __LINE__) UNUSED_VAR

#define ON_BLOCK_EXIT ScopeGuard ANON_VARIABLE(scopeGuard) = makeGuard
#define ON_BLOCK_EXIT_OBJ ScopeGuard ANON_VARIABLE(scopeGuard) = makeObjGuard
#define ON_BLOCK_EXIT_VAL ScopeGuard ANON_VARIABLE(scopeGuard) \
    = makeValGuard

#endif //SCOPEGUARD_H_
