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
    Delegator of RefCounted
 */
#ifndef REFCOUNTED_DELEGATOR_H_
#define REFCOUNTED_DELEGATOR_H_

#include "common/RefCounted.h"


/*! Delegates RefCounted interface to another class. This is useful
    to avoid diamond-shape virtual inheritance from the same RefCounted
    class. This implementation is also faster.

    You must choose which inheritance branch will be manipulated via
    RefCounted interface more often, and inherit it from normal
    RefCounted class. You must inherit other branch(es) from
    RefCountedDelegator class, which requires pointer to RefCounted instance
    which is to be used.

    Example:
    \code
    class A : public RefCounted<> {};               // often used branch
    class B : public RefCountedDelegator<> {        // less often used
    public:
        B(RefCounted<>* rp) : RefCountedDelegator(rp) {}
    };
    class C : public A, public B {
    public:
        C() : B(this) {}               // pass (this) as RefCounted*

        // specify native RefCounted inheritance branch
        DELEGATE_REFCOUNTED_INTERFACE(A)
    };
    \endcode
*/
template <class BT = EmptyStruct, class TP = DEFAULT_THREADING<> >
class RefCountedDelegator {
public:
    RefCountedDelegator(RefCounted<BT, TP>* r) : rp_(*r) {}

private:
    RefCountedDelegator();
    RefCounted<BT, TP>& rp_;
};

// MSVC barfs if there's no 'this->' before base::...
#define DELEGATE_REFCOUNTED_INTERFACE(base) \
    int32   decRefCnt() const { return this->base::decRefCnt(); } \
    void    incRefCnt() const { this->base::incRefCnt(); } \
    int32   getRefCnt() const { return this->base::getRefCnt(); }

#endif // REFCOUNTED_DELEGATOR_H_
