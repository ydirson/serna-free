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

#ifndef COMPONENT_REF_H_
#define COMPONENT_REF_H_

#include "xs/xs_defs.h"
#include "xs/XsType.h"
#include "xs/XsGroup.h"
#include "xs/Component.h"
#include "xs/NcnCred.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "common/ThreadingPolicies.h"

class Schema;

XS_NAMESPACE_BEGIN

/*! Reference to the component
 */
class ComponentRefBase : public COMMON_NS::RefCounted<> {
public:
    Schema*             schema() const;
    const NcnCred&      cred() const;

    void                component_base(COMMON_NS::RefCntPtr<Component>&) const;

    ComponentRefBase(Schema* s,
                     const COMMON_NS::String& name,
                     const COMMON_NS::String& targetUri,
                     Component::ComponentType t,
                     bool withinRedef)
        : schema_(s), cred_(name, targetUri),
          component_(0), type_(t), withinRedef_(withinRedef) {}

    XS_OALLOC(ComponentRefBase);

private:
    Schema*                 schema_;
    NcnCred                 cred_;
    mutable Component*      component_;
    char                    type_;
    bool                    withinRedef_;
};

inline Schema* ComponentRefBase::schema() const
{
    return schema_;
}

inline const NcnCred& ComponentRefBase::cred() const
{
    return cred_;
}

class ComponentInstBase {
public:
    virtual void    resolve() = 0;
    virtual ~ComponentInstBase() {}
    virtual ComponentInstBase& operator=(ComponentRefBase*) = 0;
};

//
// TODO: mutex locking at the time of component resolution
//
template <class T> class ComponentInst : public ComponentInstBase {
public:
    T* pointer() const {
        if (!component_.isNull())
            return static_cast<T*>(component_.pointer());
        ref_->component_base(component_);
        if (component_.isNull())
            component_ = T::makeEmptyStub(ref_->cred());
        else {
            ref_ = 0;
            component_->decRefCnt(); // because it is already in comp. table
            release_ = true;
        }
        return static_cast<T*>(component_.pointer());
    }

    virtual void resolve() { (void)pointer(); }
    T* operator->() const { return pointer(); }

    bool isNull() const { return (component_.isNull() && ref_.isNull()); }

    ComponentInst(ComponentRefBase* ref)
        : ref_(ref), component_(0), release_(false) {}

    ComponentInst(Component* c)
        : component_(c), release_(false) {}

    ComponentInst()
        : release_(false) {}

    virtual ~ComponentInst()
    {
        if (release_)
            component_.release();
    }

    virtual ComponentInstBase& operator=(ComponentRefBase* refbase)
    {
        if (ref_.pointer() != refbase) {
            ref_ = refbase;
            component_ = 0;
        }
        return *this;
    }
private:
    // uses default copy/assign operators
    mutable COMMON_NS::RefCntPtr<ComponentRefBase> ref_;
    mutable COMMON_NS::RefCntPtr<Component> component_;
    mutable bool release_;
};

XS_NAMESPACE_END

#endif // COMPONENT_REF_H_
