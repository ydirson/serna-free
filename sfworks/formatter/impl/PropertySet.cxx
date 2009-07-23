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

#include "grove/Nodes.h"

#include "formatter/Area.h"
#include "formatter/impl/PropertySet.h"
#include "formatter/impl/Properties.h"
#include "formatter/impl/Fo.h"

USING_COMMON_NS;
USING_GROVE_NAMESPACE;

namespace Formatter {

static void dump_as_chain(const PropertySet* propertySet)
{
    const Chain* chain = dynamic_cast<const Chain*>(propertySet);
    if (chain)
        chain->dump(0);
}

/*
 */
PropertyBase::PropertyBase()
    : percentBase_(0),
      isExplicit_(false),
      isModified_(false)
{
}

/*
 */
double PropertySet::fontSize(const String& propertyName) const
{
    Allocation alloc;
    if (propertyName == FontSize::name()) {
        /*! relative font-size should be calculated using parent font-size */
        RT_MSG_ASSERT(parentSet_, "No parent fo to get font-size");
        return parentSet_->getProperty<FontSize>(alloc).pixelSize();
    }
    return const_cast<PropertySet*>(this)->
        getProperty<FontSize>(alloc).pixelSize();
}

void PropertySet::recalcProperties(const Allocation& alloc)
{
    if (isModified_) {
        DBG(XSL.PROPDYN) << "Recalculating properties: " << this << std::endl;
        bool changed = false;
        PropertyMap::iterator i = propertyMap_.begin();
        for (; i != propertyMap_.end(); i++) {
            PropertyBase* property = (*i).second.pointer();
            if (property->isModified()) {
                CString attr(getAttr(property->propertyName()));
                if (!attr.isNull())
                    property->calculate(attr, this, alloc, -1);
                else
                    property->init(this, alloc, property->percentBase_);
                changed = true;
            }
        }
        if (!changed) DBG(XSL.PROPDYN) << "  No property changed" << std::endl;
    }
}

void PropertySet::checkProperties(const Allocation& alloc)
{
    if (isModified_) {
        DBG(XSL.PROPDYN) << "Checking properties: " << this << std::endl;
        recalcProperties(alloc);
        calcProperties(alloc);
        isModified_ = false;
    }
}

void PropertySet::setModified()
{
    if (!isModified_) {
        registerPropertyModification();
        isModified_ = true;
    }
}

void PropertySet::notifyChildren(const CString& propertyName)
{
    for (PropertySet* c = firstChildSet(); c; c = c->nextSiblingSet())
        c->notifyPropertyChanged(propertyName);
}

void PropertySet::useProperty(const CString& propertyName,
                              const PropertyBase* dependent)
{
    DBG(XSL.PROPDYN) << "Property: " << propertyName
                     << " used in:" << this << std::endl;

    usedPropertyMap_[propertyName] = dependent;
    DBG_IF(XSL.PROPDYN) dump_as_chain(this);
}

bool PropertySet::isModifiedExists(const CString& propertyName,
                                   bool removeExisting)
{
    PropertyMap::iterator i = propertyMap_.find(propertyName);
    if (propertyMap_.end() != i) {
        DBG(XSL.PROPDYN) << "Property: " << propertyName
                         << " instantiated in:" << this << std::endl;
        if (removeExisting)
            propertyMap_.erase(i);
        else
            (*i).second->setModified();
        setModified();
        return true;
    }
    return false;
}

void PropertySet::changeProperty(const CString& propertyName)
{
    DBG(XSL.PROPDYN) << "Property: " << propertyName
                     << " changed in:" << this << std::endl;
    DBG_IF(XSL.PROPDYN) dump_as_chain(this);
    //! Return if unused property has changed
    if (!isModifiedExists(propertyName))
        return;
    notifyChildren(propertyName);
}

void PropertySet::addProperty(const CString& propertyName)
{
    DBG(XSL.PROPDYN) << "Property added: " << propertyName
                     << " to:" << this << std::endl;
    DBG_IF(XSL.PROPDYN) dump_as_chain(this);

    if (!isModifiedExists(propertyName)) {
        PropertyUsingMap::iterator i = usedPropertyMap_.find(propertyName);
        if (usedPropertyMap_.end() != i) {
            DBG(XSL.PROPDYN) << "Used property found: " << propertyName
                             << " in:" << this << std::endl;

            usedPropertyMap_.erase(i);
            setModified();
        }
    }
    notifyChildren(propertyName);
}

void PropertySet::removeProperty(const CString& propertyName)
{
    DBG(XSL.PROPDYN) << "Property removed: " << propertyName
                     << " from:" << this << std::endl;
    DBG_IF(XSL.PROPDYN) dump_as_chain(this);
    //! Return if unused property has been removed
    if (!isModifiedExists(propertyName, true))
        return;
    notifyChildren(propertyName);
}

void PropertySet::notifyPropertyChanged(const CString& propertyName)
{
    DBG(XSL.PROPDYN) << "Notified property changed: " << propertyName
                         << " at:" << this << std::endl;
    DBG_IF(XSL.PROPDYN) dump_as_chain(this);

    PropertyMap::iterator i = propertyMap_.find(propertyName);
    if (propertyMap_.end() != i) {
        if ((*i).second->isExplicit_) {
            //TODO: || !property->isInheritable())
            DBG(XSL.PROPDYN) << "Explicit property found: " << propertyName
                             << " in:" << this << std::endl;
            return;
        }
        propertyMap_.erase(i);
        DBG(XSL.PROPDYN) << "Implicit property removed: " << propertyName
                         << " from:" << this << std::endl;
        setModified();
    }
    else {
        PropertyUsingMap::iterator i = usedPropertyMap_.find(propertyName);
        if (usedPropertyMap_.end() != i) {
            DBG(XSL.PROPDYN) << "Used property found: " << propertyName
                             << " in:" << this << std::endl;
            if ((*i).second)
                const_cast<PropertyBase*>((*i).second)->setModified();
            usedPropertyMap_.erase(i);
            setModified();
        }
    }
    DBG(XSL.PROPDYN) << "Notifying children:" << std::endl;
    notifyChildren(propertyName);
}

void PropertySet::setParentSet(PropertySet* parentSet)
{
    parentSet_ = parentSet;
}

PropertyBase* PropertySet::getSpecified(const String& name) const
{
    PropertyMap::const_iterator i = propertyMap_.find(name);
    if (propertyMap_.end() != i)
        return (*i).second.pointer();
    return 0;
}

const String& PropertySet::getAttr(const String& name) const
{
    if (Node::ELEMENT_NODE != node()->nodeType())
        return String::null();
    const AttrList& attrs = static_cast<const Element*>(node())->attrs();
    for (Attr* a = attrs.firstChild(); a; a = a->nextSibling())
        //! Ignore namespaces for performance issues
        if (a->localName() == name)
            return a->value();
    return String::null();
}

void PropertySet::dump(int indent) const
{
    DINDENT(indent);

    PropertyMap::const_iterator i = propertyMap_.begin();
    if (i != propertyMap_.end())
        DBG(XSL.PROP) << "Instantiated properties:\n";
    else
        DBG(XSL.PROP) << "Instantiated properties: none\n";
    uint num = 1;
    for (; i != propertyMap_.end(); i++, num++)
        DBG(XSL.PROP)
            << "  " << num << ": "<< (*i).second->propertyName()
            << (((*i).second->isExplicit_) ? ("") : ("(implicit)"))
            << std::endl;

    PropertyUsingMap::const_iterator u = usedPropertyMap_.begin();
    if (u != usedPropertyMap_.end())
        DBG(XSL.PROP) << "Used properties:\n";
    else
        DBG(XSL.PROP) << "Used properties: none\n";
    num = 1;
    for (; u != usedPropertyMap_.end(); u++, num++) {
        const String& name = (*u).first;
        String used_by;
        if ((*u).second)
            used_by = " used by: " + (*u).second->propertyName();
        DBG(XSL.PROP)
            << "  " << num << ": "<< name << used_by << std::endl;
    }
}

}
