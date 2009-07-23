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

#ifndef NODE_VISITOR_H_
#define NODE_VISITOR_H_

#include "grove/grove_defs.h"
#include "grove/GroveVisitor.h"
#include "grove/NodeVisitorLink.h"

namespace GroveLib {

class GROVE_EXPIMP NodeVisitor : public GroveVisitorBase {
public:
    bool    isRegisteredIn(const Node* node) const;
    void    deregisterFromAllNodes();

    VisitedNodeLink* firstNodeLink() const { return nodes_.firstChild(); }

    virtual ~NodeVisitor();

private:
    friend class Node;
    friend class NodeVisitorLink;

    static NodeVisitor* calculateNlistParent(VisitedNodeList* p);

    VisitedNodeList nodes_;
};

inline NodeVisitor*
NodeVisitor::calculateNlistParent(VisitedNodeList* p)
{
    // 16 is to kill gcc warning
    static const size_t offs = ((size_t)&((NodeVisitor*)16)->nodes_) - 16;
    return (NodeVisitor*)((char*)p - offs);
}

inline NodeVisitor* NodeVisitorLink::nodeVisitor() const
{
    return NodeVisitor::calculateNlistParent(VisitedNodeLink::parent());
}

} // namespace GroveLib

#endif // NODE_VISITOR_H_
