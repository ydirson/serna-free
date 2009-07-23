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

#ifndef COMPONENT_SPACE_H_
#define COMPONENT_SPACE_H_

#include "xs/xs_defs.h"
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/Vector.h"

class Schema;

namespace GroveLib {
    class ElemNameList;
}

COMMON_NS_BEGIN
class PropertyNode;
COMMON_NS_END

XS_NAMESPACE_BEGIN

using COMMON_NS::String;
using COMMON_NS::RefCntPtr;

class Component;
class NcnCred;

/*! A common base for Component spaces
 */
class ComponentSpaceBase {
public:
    class NsTableItem;

    /*! Lookup component from component table by name and ns
     */
    XS_EXPIMP Component*  lookupBase(const String& name,
                                   const String& xmlns) const;

    /*! Lookup component from component table by NcnCred
     */
    Component*  lookupBase(const NcnCred& cred) const;

    /*! Return first component from component table by ns
     */
    Component*  getFirst(const String& xmlns) const;

    /*! Erase component from component table
     */
    void        erase(const Component*);

    /*! Insert new component into table
     */
    void        insert(Component*, bool override = true);

    /*! Set local namespace
     */
    void        setLns(const String& lns);

    /*! Merge second space into first
     */
    void        merge(Schema* s, ComponentSpaceBase* sp2);

    /// Obtain a list of all components in the space
    void        getAllComponents(Common::Vector<Component*>&) const;
    void        processRedefine(Schema* s);

    void        dump() const;

    ComponentSpaceBase();
    ~ComponentSpaceBase();

    XS_OALLOC(ComponentSpaceBase);

private:
    NsTableItem*        getTable(const String& uri) const;
    String              lns_;
    COMMON_NS::Vector<COMMON_NS::RefCntPtr<NsTableItem> > table_;
};

template <class T> class ComponentSpace : public ComponentSpaceBase {
public:
    T*      lookup(const String& name,
                   const String& xmlns = String()) const {
        return static_cast<T*>(ComponentSpaceBase::lookupBase(name, xmlns));
    }
    T*      lookup(const NcnCred& cred) const {
        return static_cast<T*>(ComponentSpaceBase::lookupBase(cred));
    }
    T*      getFirst(const String& xmlns = String()) const {
        return static_cast<T*>(ComponentSpaceBase::getFirst(xmlns));
    };
};

XS_NAMESPACE_END

#endif // COMPONENT_SPACE_H_
