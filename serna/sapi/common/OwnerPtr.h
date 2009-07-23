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
#ifndef SAPI_OWNER_PTR_H_
#define SAPI_OWNER_PTR_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/PtrBase.h"

namespace SernaApi {

/// A pointer that owns the object pointed to.
/*! Destroys the owned object when being destroyed by itself.
    T must not be of array type.
    Behaviour of this class mimics std::auto_ptr, the main difference
    is common interface of PtrBase<T>.
 */
template <class T> class OwnerPtr : public PtrBase<T> {
    using PtrBase<T>::p_;
public:
    OwnerPtr() { }

    explicit OwnerPtr(T *p) : PtrBase<T>(p) {}

    ~OwnerPtr() { clear(); }


    void operator=(T *p) { reset(p); }

    void reset(T *p)
    {
        delete p_;
        p_ = p;
    }

    /*! Implicitly destroy object. */
    void clear() { reset(0); }

    template<class T1>
        OwnerPtr(OwnerPtr<T1>& other) : PtrBase<T>(other.release()) {}

    template<class T1> OwnerPtr& operator=(OwnerPtr<T1>& other)
    {
        if (p_ != other.release())
            reset(other.p_);
        return *this;
    }

    /// Allows correct passing of OwnerPtr by value as a temporary object.
    /** The following code allows correct passing of OwnerPtr
     * by value as a temporary (const) object.
     */
    template <class T1> struct OwnerPtrRef {
        T1* p_;
        OwnerPtrRef(T1* p) : p_(p) {}
    };

    OwnerPtr(OwnerPtrRef<T> ref) : PtrBase<T>(ref.p_) {}

    template <class T1>
        operator OwnerPtrRef<T1>() { return OwnerPtrRef<T>(this->release()); }
    template <class T1>
        operator OwnerPtr<T1>() { return OwnerPtr<T1>(this->release()); }
};

} // namespace SernaApi

#endif // SAPI_OWNER_PTR_H_
