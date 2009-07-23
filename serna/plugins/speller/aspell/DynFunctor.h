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

#ifndef DYN_FUNCTOR_H_
#define DYN_FUNCTOR_H_

#include "common/asserts.h"
#include "FunTraits.h"

template<typename T = void> struct DefaultResolver {
    static void* resolve()
    {
        CT_MSG_ASSERT(false, Please_Provide_Appropriate_Resolver__);
        return 0;
    }
    static bool need_reload() { return false; }
};

template<const char* name, class L = DefaultResolver<> > struct DynSym {
protected:
    DynSym()
    {
        if (0 == sym_ || L::need_reload())
            sym_ = L::resolve(name);
    }
    void* getSym() const { return sym_; }
private:
    static void* sym_;
};

template<const char* name, class L> void* DynSym<name, L>::sym_ = 0;

template<typename T, const char* name, class L> struct DynFunctor;

template<typename R, const char* name, class L>
struct DynFunctor<R(), name, L> : private DynSym<name, L>,
                                  private FunTraits<R()> {
    using DynSym<name, L>::getSym;
    typedef typename FunTraits<R()>::PtrType PtrType;
    R operator()() { return ((PtrType)(getSym()))(); }
    DynFunctor() {}
};

template<typename R, typename A, const char* name, class L>
struct DynFunctor<R(A), name, L> : private DynSym<name, L>,
                                   private FunTraits<R(A)> {
    using DynSym<name, L>::getSym;
    typedef typename FunTraits<R(A)>::PtrType PtrType;
    R operator()(A a) { return ((PtrType)(getSym()))(a); }
    DynFunctor() {}
};

template<typename R, typename A1, typename A2, const char* name, class L>
struct DynFunctor<R(A1,A2), name, L> : private DynSym<name, L>,
                                       private FunTraits<R(A1,A2)> {
    using DynSym<name, L>::getSym;
    typedef typename FunTraits<R(A1,A2)>::PtrType PtrType;
    R operator()(A1 a1, A2 a2) { return ((PtrType)(getSym()))(a1, a2); }
    DynFunctor() {}
};

template<typename R, typename A1, typename A2, typename A3, const char* name,
         class L>
struct DynFunctor<R(A1,A2,A3), name, L> : private DynSym<name, L>,
                                          private FunTraits<R(A1,A2,A3)> {
    using DynSym<name, L>::getSym;
    typedef typename FunTraits<R(A1,A2,A3)>::PtrType PtrType;
    R operator()(A1 a1, A2 a2, A3 a3)
    {
        return ((PtrType)(getSym()))(a1, a2, a3);
    }
    DynFunctor() {}
};

#endif // DYN_FUNCTOR_H_
