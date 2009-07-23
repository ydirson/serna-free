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
    Various asserts and aborts
 */
#ifndef ASSERTS_H_
#define ASSERTS_H_

#include "common/common_defs.h"


COMMON_NS_BEGIN

COMMON_EXPIMP void abort_now(const char* msg);

COMMON_EXPIMP void abort_with_message(const char* file,
                                      int line,
                                      const char* msg);
COMMON_EXPIMP void debug_break();

//
// Helper structures for the CT_ASSERT macro - do all the work
///
template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> { typedef int _Type; };

#define CT_ASSERT_CONCAT__(A,B) CT_ASSERT_JOIN__(A,B)
#define CT_ASSERT_JOIN__(A,B) A##B

#if !defined(__GNUG__)
# define CT_ASSERT_IMPL__(cond, msg) \
  { typedef COMMON_NS::CompileTimeError<(bool)(cond)>::_Type \
    CT_ASSERT_CONCAT__(CT_ASSERT_CONCAT__ \
      (CompileTimeAssertion_Failed_At_Line_, __LINE__), msg); }
#else
# define CT_ASSERT_IMPL__(cond, msg) \
  {  typedef COMMON_NS::CompileTimeError<(bool)(cond)> \
    CT_ASSERT_CONCAT__(CT_ASSERT_CONCAT__ \
      (CompileTimeAssertion_Failed_At_Line_, __LINE__), msg); \
    CT_ASSERT_CONCAT__(CT_ASSERT_CONCAT__ \
      (CompileTimeAssertion_Failed_At_Line_, __LINE__), msg) _t; \
    (void) _t; \
  }
#endif

/*! Macro CT_MSG_ASSERT(expr, id)
 *   \li expr is a compile-time integral or pointer expression;
 *   \li msg  is a C++ identifier that does not need to be defined.
 *
 *  If expr is zero, id will appear in a compile-time error message.
 */
#define CT_MSG_ASSERT(cond, msg) \
    CT_ASSERT_IMPL__(cond, __##msg)

/*! Compile-time assert: compiler aborts with error message if
 *  expr is false.
 */
# define CT_ASSERT(cond) \
  CT_ASSERT_IMPL__(cond, /**/)

/*! Print message and abort.
 */
#define RT_MSG_ABORT(msg) \
    COMMON_NS::abort_with_message(__FILE__, __LINE__, msg)

#ifndef NDEBUG

/*! Run-time assert: similar to usual assert
 */
# define RT_ASSERT(cond) \
    if (!(cond)) COMMON_NS::abort_with_message(__FILE__, __LINE__, \
        "Assertion failed: " #cond);

/*! Run-time assert which throws instance of class initialized with
 *  stringized condition instead of calling abort()
 */
# define RT_THROW_ASSERT(cond, obj) \
    if (!cond) throw obj(#cond);

/*! Run-time assert which prints message instead of stringized condition
 */
# define RT_MSG_ASSERT(cond, msg) \
    if (!cond) COMMON_NS::abort_with_message(__FILE__, __LINE__, msg);

/*! Debug break: sends STOP signal to itself under Unix,
 *  calls DebugBreak under Windows
 */
# define DEBUG_BREAK debug_break();

#else // NDEBUG

# define RT_ASSERT(cond)
# define RT_THROW_ASSERT(cond, obj)
# define RT_MSG_ASSERT(cond, msg)
# define DEBUG_BREAK /**/

#endif // NDEBUG

#if !defined(NDEBUG) && !defined(NO_INVCHECK)

struct InvCheckDefaultPolicy {
    static inline void react() { abort_now("Invariant check failed"); }
};

template<class T, bool CHECK = true, class CheckPolicy = InvCheckDefaultPolicy>
class InvariantGuard {
public:
    typedef bool (T::*InvChecker)() const;
    InvariantGuard(const T* const ths, InvChecker ic)
    : this_(*ths), invCheck_(ic) { check(); }
    ~InvariantGuard() { check(); }
    typedef T Type;
private:
    inline void check()
    {
        if (CHECK && !(this_.*invCheck_)())
            CheckPolicy::react();
    }
    const T& this_;
    InvChecker invCheck_;
};

# define INVARIANT_GUARD_DECLARE(type) \
    friend class COMMON_NS::InvariantGuard<type>; \
    typedef COMMON_NS::InvariantGuard<type> InvGuard

# define INVARIANT_GUARD(pm) InvGuard \
    CT_ASSERT_CONCAT__(invguard_,__LINE__)(this, &InvGuard::Type:: pm)

# define INVARIANT_CHECK(pm) if (!pm()) InvCheckDefaultPolicy::react()
#else
# define INVARIANT_GUARD_DECLARE(type)
# define INVARIANT_GUARD(x)
# define INVARIANT_CHECK(pm)
#endif

COMMON_NS_END

#endif // ASSERTS_H_
