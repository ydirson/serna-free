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
#ifndef SAPI_REF_CNT_PTR_H
#define SAPI_REF_CNT_PTR_H

#include "sapi/sapi_defs.h"
#include "sapi/common/PtrBase.h"

namespace SernaApi {
/// Pointer to object T which maintains (intrusive) object reference count.
/*! Pointer to object T which maintains (intrusive) object reference count.
 *  T must have RefCounted class as it's base class (or at least maintain
 *  same interface).
 *  T may be an incomplete type.
 */
template <class T> class RefCntPtr : public PtrBase<T> {
    using PtrBase<T>::p_;
public:
    RefCntPtr() {}

    RefCntPtr(T* ptr)
        : PtrBase<T>(ptr)
    {
        if(p_)
            p_->incRefCnt();
    }

    template <class T1> RefCntPtr(const RefCntPtr<T1>& other)
        : PtrBase<T>(other.p_)
    {
        if (p_)
            p_->incRefCnt();
    }

    RefCntPtr(const RefCntPtr& other)
        : PtrBase<T>(other.p_)
    {
        if (p_)
            p_->incRefCnt();
    }

    RefCntPtr& operator=(T* other)
    {
        return assign(other);
    }

    RefCntPtr& operator=(const RefCntPtr& other)
    {
        return assign(other.p_);
    }

    void swap(RefCntPtr& other)
    {
        RefCntPtr tmp = other;
        other = *this;
        assign(tmp.p_);
    }

    void clear()
    {
        assign(0);
    }

    int count()
    {
        return p_ ? p_->getRefCnt() : 0;
    }

    ~RefCntPtr()
    {
        if (p_ && p_->decRefCnt() <= 0)
            delete p_;
    }

private:
    RefCntPtr& assign(T* other)
    {
        if (other)
            other->incRefCnt();
        if (p_ && p_->decRefCnt() <= 0)
            delete p_;
        p_ = other;
        return *this;
    }
};

} // namespace SernaApi

#endif // SAPI_REF_CNT_PTR_H
