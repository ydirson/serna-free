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

#include <iostream>
#include "grove/grove_defs.h"
#include "grove/Nodes.h"
#include "grove/DebugGroveVisitor.h"

GROVE_NAMESPACE_BEGIN

//
// All the functions defined in this file used only for debug printouts
// of various kinds of GroveVisitor events.
//
void DebugGroveVisitor::nodeDestroyed(const Node* node)
{
    if (!(nmask_ & NOTIFY_NODE_DESTROYED))
        return;
    std::cerr << "Node destroyed: " << node << '\n';
    if (verbose_) {
        std::cerr << "    ";
        node->dump();
    }
}

void DebugGroveVisitor::childInserted(const Node* node)
{
    if (!(nmask_ & NOTIFY_CHILD_INSERTED))
        return;
    std::cerr << "Child inserted at node " << node->parent() << "/"
              << node->parent()->nodeName().latin1()
              << ", new child: " << node << "/"
              << node->nodeName().latin1() << '\n';
    if (verbose_) {
        std::cerr << "  ";
        node->parent()->dump();
        std::cerr << "  + ";
        node->dump();
    }
}

void DebugGroveVisitor::childRemoved(const Node* node,
                                     const Node* child)
{
    if (!(nmask_ & NOTIFY_CHILD_REMOVED))
        return;
    std::cerr << "Child removed at node " << node << "/"
              << node->nodeName().latin1()
              << ", removed child: " << child << "/"
              << child->nodeName().latin1()
              << std::endl;
    if (verbose_) {
        std::cerr << "  ";
        node->dump();
        std::cerr << "  - ";
        child->dump();
    }
}

void DebugGroveVisitor::genericNotify(const Node* node, void* p)
{
    if (!(nmask_ & NOTIFY_GENERIC))
        return;
    std::cerr << "Generic Notify: node " << node << "/"
              << node->nodeName().latin1() << " P: " << p
              << std::endl;
    if (verbose_) {
        std::cerr << "    ";
        node->dump();
    }
}

void DebugGroveVisitor::textChanged(const Text* t)
{
    if (!(nmask_ & NOTIFY_TEXT_CHANGED))
        return;
    std::cerr << "Text node changed: " << t << ": new text "
              << t->data() << std::endl;
    if (verbose_) {
        std::cerr << "    ";
        t->dump();
    }
}

void DebugGroveVisitor::attributeChanged(const Attr* node)
{
    if (!(nmask_ & NOTIFY_ATTRIBUTE_CHANGED))
        return;
    std::cerr << "Attribute node changed: node " << node << "/"
              << node->nodeName().latin1()
              << std::endl;
    if (verbose_) {
        std::cerr << "  ";
        node->dump();
    }
}

void DebugGroveVisitor::attributeRemoved(const Element* elem, const Attr* node)
{
    if (!(nmask_ & NOTIFY_ATTRIBUTE_REMOVED))
        return;
    std::cerr << "Attribute node removed: " << node << " P"
              << elem << " name "
              << node->nodeName().latin1()
              << std::endl;
}

void DebugGroveVisitor::attributeAdded(const Attr* node)
{
    if (!(nmask_ & NOTIFY_ATTRIBUTE_ADDED))
        return;
    std::cerr << "Attribute node added: ";
    node->dump();
}

GROVE_NAMESPACE_END
