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
#ifndef REF_COUNTED_H
#define REF_COUNTED_H

#include "common/common_defs.h"
#include "common/ThreadingPolicies.h"

namespace Common {

/*! Objects eligible for intrusive refcounting should inherit from
    this class.
    RefCntPtr uses interface of this class.
 */
template <class TP = DEFAULT_THREADING<> >
class RefCounted {
public:
    /// Decrement reference counter & return new value
    int32 decRefCnt() const { count_.deref(); return count_; }

    /// Increment reference counter
    void incRefCnt() const { count_.ref(); }

    /// Return ref counter's value
    int32 getRefCnt() const { return count_; }

    RefCounted(const RefCounted&) : count_(0) {}
    RefCounted() : count_(0) {}
    
private:
    typedef typename TP::AtomicInt AtomicInt;
    mutable AtomicInt count_;
};

#ifdef _WIN32
template class COMMON_EXPIMP RefCounted<>;
#endif

/*! This template should be used when we want to have reference
 *  counter located _after_ certain base class BT. Disadvantage of this
 *  feature is that it defeats the non-default constructors of BT.
 *
 *  We use separate template (not just another template argument to
 *  RefCounted) because certain compilers don't do empty base optimization.
 */
template <class BT, class TP = DEFAULT_THREADING<> >
  class RefCountedBase : public BT, public RefCounted<TP> {};

} // namespace Common

#endif // REF_COUNTED_H
