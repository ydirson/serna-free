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
#include "sapi/sapi_defs.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/SString.h"
#include "sapi/common/xtn_wrap.h"

#ifndef SAPI_PROPERTY_NODE_H_
#define SAPI_PROPERTY_NODE_H_

namespace SernaApi {

class PropertyNode;

/// Dynamic tracker of PropertyNode changes.
class SAPI_EXPIMP PropertyNodeWatcher : public RefCountedWrappedObject {
public:
    PropertyNodeWatcher();
    virtual ~PropertyNodeWatcher() {}
    /// Happens when property is changed
    virtual void propertyChanged(const PropertyNode& property) = 0;
};

/// The node of property tree.
class SAPI_EXPIMP PropertyNode : public RefCountedWrappedObject {
public:
    PropertyNode(SernaApiBase* = 0);
    PropertyNode(const SString& name, const SString& value = SString());
    PropertyNode(const SString& name, bool value);
    PropertyNode(const SString& name, int value);
    PropertyNode(const SString& name, double value);

    /// Get property name
    SString         name() const;

    /// Set (new) property name
    void            setName(const SString&);

    /// Get string value of property
    SString         getString() const;

    /// Get boolean property value
    bool            getBool(bool* ok = 0) const;

    /// Get integer property value
    int             getInt(bool* ok = 0) const;

    /// Get pointer property value
    void*           getPtr(bool* ok = 0) const;

    /// Get double property value
    double          getDouble(bool* ok = 0) const;

    /// Set (new) string value of property
    void            setString(const SString& value);

    /// Set boolean value of property
    void            setBool(bool);

    /// Set integer value of property
    void            setInt(int);

    /// Set pointer value of property
    void            setPtr(void*);

    /// Set double value of property
    void            setDouble(double);

    /// Merge values from other property tree into current property tree.
    /// Change notifications are currently not sent during merge.
    void            merge(const PropertyNode& other,
                          bool override = false);

    /// Make (possible recursive) copy of this property node.
    PropertyNode    copy(bool recursive = false) const;

    /// Get child property by slash-delimited path
    PropertyNode    getProperty(const SString& name) const;

    /// Safely get child property (never returns NULL, returns empty property)
    PropertyNode    getSafeProperty(const SString& name) const;

    /// Makes child property subtree (with path). Returns the leaf.
    PropertyNode    makeDescendant(const SString& name);
    /// Makes child property subtree (with path). Returns the leaf.
    PropertyNode    makeDescendant(const SString& name, 
                                   const SString& value,
                                   bool override = true);

    /// Register watcher for this PropertyNode
    void            addWatcher(PropertyNodeWatcher* watcher);

    /// Removes watcher
    void            removeWatcher(PropertyNodeWatcher* watcher);

    /// Removes all watchers
    void            removeAllWatchers();
    /// Dump PropertyNode
    void            dump() const;

    /// Save property tree to the given URL. Returns True on success.
    bool            save(const SString& url) const;

    /// Read property tree from the URL. If the \a merge attribute is false,
    /// then contents of the current property node are discarded; otherwise,
    /// existing property tree is merged with one which were read from file.
    bool            read(const SString& url, bool merge = false);  

#ifdef DOX_PYTHON_ONLY
    /// Returns the python list of children
    PY_List children();
#endif

    virtual ~PropertyNode();

    XTREENODE_WRAP_DECL(PropertyNode)
};

} // namespace PropertyNode

#endif // SAPI_PROPERTY_NODE_H
