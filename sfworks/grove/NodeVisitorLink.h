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
#ifndef GROVE_NODE_VISITOR_LINK_H_
#define GROVE_NODE_VISITOR_LINK_H_

#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "common/XList.h"

namespace GroveLib {

class NodeVisitor;

class VisitedNodeLink : public Common::XListItem<VisitedNodeLink> {};
class VisitorLink : public Common::XListItem<VisitorLink> {};

class NodeVisitorLink : public VisitedNodeLink,
                        public VisitorLink {
public:
    NodeVisitorLink(short bitmask = 0xFFFF)
        : visitorMask_(bitmask) {}

    Node*           visitedNode() const;
    NodeVisitor*    nodeVisitor() const;

    short           visitorMask() const { return visitorMask_; }
    void            addVisitorMask(short v) { visitorMask_ |= v; }
    void            setVisitorMask(short v) { visitorMask_ = v; }
    
    // user-defined data
    short&          udata() { return udata_; }

    GROVE_OALLOC(NodeVisitorLink);

private:
    short           visitorMask_;
    short           udata_;
};

typedef Common::XList<VisitorLink>     NodeVisitorList;
typedef Common::XList<VisitedNodeLink> VisitedNodeList;

} // namespace GroveLib

#endif // GROVE_NODE_VISITOR_LINK_H_

