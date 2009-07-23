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

#include "grove/grove_defs.h"
#include "grove/GroveVisitorMux.h"
#include "grove/Nodes.h"

GROVE_NAMESPACE_BEGIN

#define NOTIFY_X(t, what) \
    if (!(mask_ & t)) \
        return; \
    const Node* cn = node; \
    while (cn) {         \
        int32 hv = phash(cn); \
        gvListItem* t = gvtab_.get(hv); \
        t->gv_-> what ; \
        cn = cn->parent(); \
    }

void GroveVisitorMux::nodeDestroyed(const Node* node)
{
    NOTIFY_X(node_destroyed, nodeDestroyed(node))
}

void GroveVisitorMux::childInserted(const Node* node)
{
    NOTIFY_X(child_inserted, childInserted(node))
}

void GroveVisitorMux::childRemoved (const Node* node, const Node* child)
{
    NOTIFY_X(child_removed, childRemoved(node, child))
}

void GroveVisitorMux::genericNotify(const Node* node, void* vp)
{
    NOTIFY_X(generic_notify,  genericNotify(node, vp))
}

void GroveVisitorMux::textChanged(const Text* node)
{
    NOTIFY_X(text_changed, textChanged(node));
}

void GroveVisitorMux::attributeChanged(const Attr* node)
{
    NOTIFY_X(attribute_changed, attributeChanged(node))
}

void GroveVisitorMux::attributeRemoved(const Element* elem, const Attr* node)
{
    NOTIFY_X(attribute_removed, attributeRemoved(elem, node))
}

void GroveVisitorMux::attributeAdded(const Attr* node)
{
    NOTIFY_X(attribute_added, attributeAdded(node))
}

void GroveVisitorMux::registerVisitor(const Node* n, GroveVisitorBase* v)
{
    int32 hv = phash(n);
    gvtab_.set(new gvListItem(v, n), hv);
}

void GroveVisitorMux::deregisterVisitor(const Node* n)
{
    int32 hv = phash(n);
    gvListItem* t = gvtab_.get(hv);
    gvtab_.remove(hv);
    delete t;
}

void GroveVisitorMux::deregisterVisitor(const GroveVisitorBase* v)
{
    for (int hv = 0; hv < htsz; ++hv) {
        gvListItem* t = gvtab_.get(hv);
        if (t->gv_ == v) {
            gvtab_.remove(hv);
            delete t;
            break;
        }
    }
}

GROVE_NAMESPACE_END
