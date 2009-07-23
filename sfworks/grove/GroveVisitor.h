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

#ifndef GROVE_VISITOR_H
#define GROVE_VISITOR_H

#ifdef _MSC_VER
# pragma warning( disable : 4251)
#endif

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "grove/Grove.h"
#include "grove/Decls.h"
#include "common/common_defs.h"
#include "common/RefCntPtr.h"
#include "common/prtti.h"

GROVE_NAMESPACE_BEGIN

class Attr;
class Text;
class Element;
class GroveVisitor;
class Node;
class GroveSectionRoot;

/*! This class represents abstract interface for grove change notifications.
 *  To receive these notifications, client must use Grove::registerVisitor()
 *  function to register a grove visitor. Grove visitor classes must
 *  inherit from GroveVisitorBase and reimplement those notification
 *  functions they desire (by default, GroveVisitorBase implements empty
 *  stubs which do nothing).
 */
class GROVE_EXPIMP GroveVisitorBase {
public:
    enum NotificationMask {
        NOTIFY_GENERIC            = 0x0001,
        NOTIFY_NODE_DESTROYED     = 0x0002,
        NOTIFY_CHILD_INSERTED     = 0x0004,
        NOTIFY_CHILD_REMOVED      = 0x0008,
        NOTIFY_ATTRIBUTE_CHANGED  = 0x0010,
        NOTIFY_ATTRIBUTE_ADDED    = 0x0020,
        NOTIFY_ATTRIBUTE_REMOVED  = 0x0040,
        NOTIFY_TEXT_CHANGED       = 0x0080,
        NOTIFY_NS_MAPPING_CHANGED = 0x0100,
        NOTIFY_FORCE_MODIFIED     = 0x0200,
        // custom bits
        NOTIFY_CSL_BIT            = 0x1000, // currently used by CSL
        NOTIFY_CUSTOM1            = 0x2000,
        NOTIFY_CUSTOM2            = 0x4000,
        NOTIFY_CUSTOM3            = 0x8000,
        // all standard notifications
        NOTIFY_ALL = 0x3FF
    };
    virtual ~GroveVisitorBase();

    /*! happens when node was destroyed. Actually this notification is
     *  called from node destructor, so be careful not to use node
     *  pointer for anything except synchronizing state (the pointer is OK,
     *  but node parent pointer is already zeroized etc.)
     */
    virtual void    nodeDestroyed(const Node* node) = 0;

    /// happens when this node is inserted into node->parent()
    virtual void    childInserted(const Node* node) = 0;

    /*! happens when \a child os removed from node children list.
     *  Note that \a child pointer is still valid, but parent pointer
     *  is already zeroized.
     */
    virtual void    childRemoved (const Node* node,
                                  const Node* child) = 0;
    /*! Attr-only event: called when value of attribute \a node is changed.
     * \a node always has an Element as a parent.
     */
    virtual void    attributeChanged(const Attr* node) = 0;

    /*! Attr-only event: called immediately BEFORE attribute \a node
     * is removed from it's parent element attribute list.
     */
    virtual void    attributeRemoved(const Element* elem, const Attr* node) = 0;

    /// Attr-only: called when attribute \a node is added to the parent element.
    virtual void    attributeAdded(const Attr* node) = 0;

    virtual void    textChanged(const Text* node) = 0;

    /// Generic notification. Second argument is used to determine notification
    /// type or for passing a parameter.
    virtual void    genericNotify(const GroveLib::Node*, void*) {}

    /// Used by dynamic XSLT for forced template reselection/update
    virtual void    forceModified(const Node*) {}

    /// Called when node namespace mappings were changed
    virtual void    nsMappingChanged(const NodeWithNamespace*,
                                     const Common::String&) {}

    PRTTI_DECL(GroveVisitor);
};

/*! Same as GroveVisitorBase, but this class automagically de-registers
    itself from grove on destruction.
 */
class GROVE_EXPIMP GroveVisitor : public GroveVisitorBase {
public:
    virtual ~GroveVisitor();

    PRTTI_DECL(GroveVisitor);

    GroveVisitor(GroveSectionRoot* gsr);

protected:
    GroveSectionRootPtr gsr_;

private:
    friend class GroveSectionRoot;
    GroveVisitor();
};

GROVE_NAMESPACE_END

#endif // GROVE_VISITOR_H
