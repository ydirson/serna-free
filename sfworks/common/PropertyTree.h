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
#ifndef _COMMON_PROPERTY_TREE_H_
#define _COMMON_PROPERTY_TREE_H_

#include "common/common_defs.h"
#include "common/XTreeNode.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include "common/PackedPtrSet.h"
#include "common/String.h"
#include "common/SernaApiRefCounted.h"

namespace Common {

class PropertyNode;
typedef COMMON_NS::RefCntPtr<PropertyNode>          PropertyNodePtr;
typedef COMMON_NS::RefCntPtr<const PropertyNode>    CPropertyNodePtr;

/////////////////////////////////////////////////////////////////////////

class COMMON_EXPIMP PropertyNodeWatcher {
public:
    typedef Common::PackedPointerSet<PropertyNode>  PropertyPtrSet;

    virtual ~PropertyNodeWatcher();

    const PropertyPtrSet&   propertyPtrSet() const;
    bool                    isRegisteredIn(const PropertyNode* prop) const;

protected:
    virtual void            propertyChanged(PropertyNode* property) = 0;
    void                    deregister();

private:
    friend class PropertyNode;
    PropertyPtrSet          properties_;
};

/////////////////////////////////////////////////////////////////////////

class COMMON_EXPIMP PropertyNode : public SernaApiRefCounted,
                                   public XTreeNode<PropertyNode,
                                                    PropertyNode> {

public:
    PropertyNode(const String& name = String())
        : name_(name) {}
    PropertyNode(const String& name, const String& value)
        : name_(name), value_(value) {}
    PropertyNode(const String& name, const char* value)
        : name_(name), value_(value) {}
    PropertyNode(const String& name, double value);
    PropertyNode(const String& name, int value);
    PropertyNode(const String& name, bool value);

    virtual ~PropertyNode();


    /// Property name
    const String&           name() const { return name_; }
    void                    setName(const String&);

    /// Get child property by slash-delimited path
    PropertyNode*           getProperty(const String& propertyPath) const;
    PropertyNode*           getProperty(const String& propertyPath1,
                                        const String& propertyPath2) const;
    PropertyNode*           getProperty(const String& propertyPath1,
                                        const String& propertyPath2,
                                        const String& propertyPath3) const;

    /// Safely get property (never returns NULL, returns empty property)
    const PropertyNode*     getSafeProperty(const String& proppath) const;

    /// Get value of this node
    bool                    getBool(bool* ok = 0) const;
    int                     getInt(bool* ok = 0) const;
    void*                   getPtr(bool* ok = 0) const;
    double                  getDouble(bool* ok = 0) const;
    const String&           getString(bool* ok = 0) const;

    // Convinience accessors (equal to getSafePropety(path)->getString())
    const String&           getString(const String& path) const;
    const String&           getString(const String& path1,
                                      const String& path2) const;
    const String&           getString(const String& path1,
                                      const String& path2,
                                      const String& path3) const;
    /// Set value of this node
    void                    setBool(bool);
    void                    setInt(int);
    void                    setPtr(void*);
    void                    setDouble(double);
    void                    setString(const String&);

    /// Merge values from other property tree into current property tree.
    /// Change notifications are currently not sent during merge.
    void                    merge(const PropertyNode* other,
                                  bool override = false);
    /// Copy a property tree
    virtual PropertyNode*   copy(bool recursive = false) const;

    /// Makes child property subtree (with path). Returns the leaf.
    PropertyNode*           makeDescendant(const String& path);

    /// Makes child property subtree (with path). Returns the leaf.
    PropertyNode*           makeDescendant(const String& path,
                                           const String& value,
                                           bool override = true);

    /// Set value as in other PTN (returns true if value was changed)
    virtual bool            setValue(const PropertyNode* other);

    //! Management of ItemWatchers
    void                    addWatcher(PropertyNodeWatcher* watcher);
    void                    removeWatcher(PropertyNodeWatcher* watcher);
    void                    removeAllWatchers();

    void                    dump(int indent = 0) const;

private:
    friend class XListItem<PropertyNode, PropertyNode>;
    friend class XList<PropertyNode, PropertyNode>;
    typedef XList<PropertyNode, PropertyNode>   XLPT;

    static void             xt_incRefCnt(PropertyNode* n) { n->incRefCnt(); }
    static int              xt_decRefCnt(PropertyNode* n)
        { return n->decRefCnt(); }

    // Notifications
    static void             notifyNodeDestroyed(PropertyNode*) {}
    static void             notifyChildInserted(PropertyNode*);
    static void             notifyChildRemoved(XLPT*, PropertyNode*);

    void                    notifyChanged(PropertyNode* property);

private:
    String                                          name_;
    String                                          value_;
    Common::PackedPointerSet<PropertyNodeWatcher>   watchers_;
};

/////////////////////////////////////////////////////////////////////////

/// Base class for classes which contain instance of the property tree
class COMMON_EXPIMP PropertyTree {
public:
    PropertyTree()
        : root_(new PropertyNode) {};
    PropertyTree(COMMON_NS::PropertyNode* root)
        : root_(root) {};

    virtual ~PropertyTree() {};

    PropertyNode*   root() const { return root_.pointer(); }
    void            setRoot(PropertyNode* root) { root_ = root; }

protected:
    PropertyNodePtr root_;
};

}

#endif // COMMON_PROPERTY_TREE_H_
