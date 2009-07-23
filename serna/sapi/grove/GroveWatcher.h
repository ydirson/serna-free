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
#include "sapi/common/xtn_wrap.h"
#include "sapi/common/SString.h"
#include "sapi/grove/GroveDecls.h"

#ifndef SAPI_GROVE_VISITOR_H_
#define SAPI_GROVE_VISITOR_H_

namespace SernaApi {

/// Base class for dynamic tracking of the changes in the grove
class SAPI_EXPIMP GroveWatcherBase {
public:
    enum NotificationMask {
        NOTIFY_GENERIC        = 001,
        NOTIFY_NODE_DESTROYED = 002,
        NOTIFY_CHILD_INSERTED = 004,
        NOTIFY_CHILD_REMOVED  = 010,
        NOTIFY_ATTRIBUTE_CHANGED = 0100,
        NOTIFY_ATTRIBUTE_ADDED   = 0200,
        NOTIFY_ATTRIBUTE_REMOVED = 0400,
        NOTIFY_TEXT_CHANGED = 01000,
        NOTIFY_NS_MAPPING_CHANGED = 02000,
        NOTIFY_ALL = 0xFFFF
    };

    virtual ~GroveWatcherBase() {}

    /*! Happens when node was destroyed. Actually this notification is
     *  called from node destructor, so be careful not to use node
     *  pointer for anything except synchronizing state (the pointer is OK,
     *  but node parent pointer is already zeroized etc.)
     */
    virtual void    nodeDestroyed(const GroveNode&) {}

    /// Happens when this node is inserted into node->parent()
    virtual void    childInserted(const GroveNode&) {}

    /*! Happens when \a child os removed from node children list.
     *  Note that \a child pointer is still valid, but parent pointer
     *  is already zeroized.
     */
    virtual void    childRemoved (const GroveNode&,
                                  const GroveNode&) {}
    /*! Attr-only event: called when value of attribute \a node is changed.
     * \a node always has an Element as a parent.
     */
    virtual void    attributeChanged(const GroveAttr&) {}

    /*! Attr-only event: called immediately BEFORE attribute \a node
     * is removed from it's parent element attribute list.
     */
    virtual void    attributeRemoved(const GroveElement&,
                                     const GroveAttr&) {}

    /// Attr-only: called when attribute \a node is added
    /// to the parent element.
    virtual void    attributeAdded(const GroveAttr&) {}
    /// Happens when text node is changed
    virtual void    textChanged(const GroveText&) {}
    /// Generic notification
    virtual void    genericNotify(const GroveNode&, void*) {}
    /// Happens when namespace maping is changed
    virtual void    nsMappingChanged(const GroveNodeWithNamespace&,
                                     const SString&) {}
};

/// Dynamic tracker of the changes of the grove as a whole.
class SAPI_EXPIMP GroveWatcher : public GroveWatcherBase,
                                 public RefCountedWrappedObject {
public:
    GroveWatcher();
    virtual ~GroveWatcher();
};

/// Dynamic tracker of a grove node changes.
class SAPI_EXPIMP GroveNodeWatcher : public GroveWatcherBase,
                                     public RefCountedWrappedObject {
public:
    GroveNodeWatcher();
    virtual ~GroveNodeWatcher();
    /// checks if this visitor is already in use by node.
    bool    isRegisteredIn(const GroveNode& node) const;
    /// unhook visitor from all nodes.
    void    deregisterFromAllNodes();
};

} // namespace SernaApi

#endif // SAPI_GROVE_VISITOR_H_

