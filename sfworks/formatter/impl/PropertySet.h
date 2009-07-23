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

#ifndef FORMATTER_PROPERTY_SET_H
#define FORMATTER_PROPERTY_SET_H

#include <map>
#include <typeinfo>
#include "grove/Decls.h"
#include "common/RefCntPtr.h"
#include "common/String.h"

#include "formatter/impl/ParserContext.h"
#include "formatter/impl/PropertyParser.h"
#include "formatter/formatter_defs.h"

#ifdef BUILD_FORMATTER
# include "formatter/impl/debug.h"
#endif // BUILD_FORMATTER

namespace Formatter {

class PropertySet;

/*! \brief TODO: remove inheritance from COMMON_NS::RefCounted<>
 */
class FORMATTER_EXPIMP PropertyBase : public COMMON_NS::RefCounted<>,
                                      public PropertyContext {
public:
    FORMATTER_OALLOC(PropertyBase);
    PropertyBase();

    virtual void    calculate(const COMMON_NS::String& specified,
                              ParserContext* context, const Allocation& alloc,
                              const CType& percentBase) = 0;
    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase) = 0;

    bool            isModified() const { return isModified_; }
    bool            isExplicit() const { return isExplicit_; }
protected:
    void            setModified() { isModified_ = true; }

    friend class PropertySet;

protected:
    CType           percentBase_;
    bool            isExplicit_;
    bool            isModified_;
};

/*! \brief
 */
class FORMATTER_EXPIMP PropertySet : public ParserContext {
public:
    typedef COMMON_NS::String                           CString;
    typedef COMMON_NS::RefCntPtr<PropertyBase>          PropertyPtr;
    typedef std::map<CString, PropertyPtr>              PropertyMap;
    typedef std::map<CString, const PropertyBase*>      PropertyUsingMap;

    typedef COMMON_NS::RefCntPtr<const GroveLib::Node>  ConstNodePtr;

    PropertySet(PropertySet* parentSet)
        : parentSet_(parentSet), isModified_(true) {};
    virtual ~PropertySet() {};

    //!
    virtual double          fontSize(const CString& propertyName) const;
    //! Returns specified property or ask parent
    template <class T> T&   getProperty(const Allocation& alloc,
                                        const CType percentBase = -1, 
                                        bool disableInheritance = false);
    //! Recalculates modified properties
    void                    checkProperties(const Allocation& alloc);
    //!
    void                    setModified();
    //!
    void                    useProperty(const CString& propertyName,
                                        const PropertyBase* dependent);
protected:
    //! Recalculates modified properties
    void                    recalcProperties(const Allocation& alloc);
    //! Returns explicitely specified property (asking parent if necessary)
    template <class T> T*   getSpecifiedProperty(const CString& name,
                                                 const Allocation& alloc,
                                                 const CType& percentBase,
                                                 const PropertySet*& used_set,
                                            bool disableInheritance = false);
    //! Returns property specified and already instantiated in this set
    PropertyBase*           getSpecified(const CString& name) const;
    //!
    const CString&          getAttr(const CString& name) const;
    //!
    void                    setParentSet(PropertySet* parentSet);
    //! Recalculates data if some properties were modidfied
    virtual void            calcProperties(const Allocation&) {};
    //!
    void                    changeProperty(const CString& propertyName);
    //! Marks property with given name as modified and registers modification
    void                    addProperty(const CString& propertyName);
    //!
    void                    removeProperty(const CString& propertyName);
    //! Notifies child sets that property has changed
    void                    notifyChildren(const CString& propertyName);
    //! Registers property modification
    void                    notifyPropertyChanged(const CString& propertyName);
    //!
    void                    dump(int indent) const;
private:
    //! Returns node to take attributes from
    virtual const GroveLib::Node*   node() const = 0;
    virtual PropertySet*    firstChildSet() const { return 0; }
    virtual PropertySet*    nextSiblingSet() const { return 0; }

    bool                    isModifiedExists(const CString& propertyName,
                                             bool removeExisting = false);
    virtual void            registerPropertyModification() {};

    friend class CellAllocator;

protected:
    PropertySet*            parentSet_;
//private:
    PropertyMap             propertyMap_;
    PropertyUsingMap        usedPropertyMap_;
    bool                    isModified_;
};

template <class T> T& PropertySet::getProperty(const Allocation& alloc,
                                               const CType percentBase,
                                               bool disableInheritance)
{
    const CString& name = T::name();
    //! If property was explicitely specified here or inherited from ancestor
#ifdef BUILD_FORMATTER
    DBG(XSL.PROP) << "Looking for <" << name << ">" << std::endl;
#endif // BUILD_FORMATTER
    const PropertySet* used_set = 0;
    T* specified = getSpecifiedProperty<T>(
        name, alloc, percentBase, used_set, disableInheritance);
    if (specified) {
        if (this != used_set)
            useProperty(name, 0);
        return *specified;
    }
    //! Return property set to it`s initial value
#ifdef BUILD_FORMATTER
    DBG(XSL.PROP) << "Implicit <" << name << "> returned" << std::endl;
#endif // BUILD_FORMATTER
    T* initial = new T;
    initial->init(this, alloc, percentBase);
    propertyMap_[name] = initial;
    return *initial;
}

template <class T> T*
PropertySet::getSpecifiedProperty(const CString& name,
                                  const Allocation& alloc,
                                  const CType& percentBase,
                                  const PropertySet*& used_set,
                                  bool disableInheritance)
{
    //! If property was already instantiated, return it
    PropertyBase* existing = getSpecified(name);
    if (existing) {
#ifdef BUILD_FORMATTER
        DBG(XSL.PROP) << "Existing property:" << name << std::endl;
#endif // BUILD_FORMATTER
        used_set = this;
        if (-1 != percentBase && percentBase != existing->percentBase_) {
            CString attr(getAttr(existing->propertyName()));
            if (!attr.isNull())
                existing->calculate(attr, this, alloc, percentBase);
            else
                existing->init(this, alloc, percentBase);
        }
        return static_cast<T*>(existing);
    }
    //! If property was explicitly specified, instantiate and return it
    CString attr(getAttr(name));
    if (!attr.isNull()) {
#ifdef BUILD_FORMATTER
        DBG(XSL.PROP)
            << "Explicit property:" << name << " = " << attr << std::endl;
#endif // BUILD_FORMATTER
        T* specified = new T;
        specified->calculate(attr, this, alloc, percentBase);
#ifdef BUILD_FORMATTER
        DBG(XSL.PROP)
            << "Typecheck: prop<" << name << "> type <"
            << typeid(T).name() << "> addr:" << specified << std::endl;
#endif // BUILD_FORMATTER
        propertyMap_[name] = specified;
        used_set = this;
        return specified;
    }
    //! Return inherited property if any
    if (!disableInheritance && parentSet_ && 
        (T::isInheritable() || "inherit" == attr)) {
#ifdef BUILD_FORMATTER
        DBG(XSL.PROP) << "Asking parent for <" << name << ">" << std::endl;
#endif // BUILD_FORMATTER
        T* inherited = parentSet_->getSpecifiedProperty<T>(
            name, alloc, -1, used_set);
        if (inherited)
            return inherited;
    }
    return 0;
}

}

#endif // FORMATTER_PROPERTY_SET_H
