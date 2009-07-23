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
#include "sapi/grove/GroveWatcher.h"
#include "sapi/grove/GroveNodes.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/GroveVisitor.h"
#include "grove/NodeVisitor.h"
#include "common/safecast.h"
#include "common/asserts.h"

namespace {

using namespace GroveLib;

class GroveNodeWatcherProxy : public Common::SernaApiRefCounted,
                              public GroveLib::NodeVisitor {
public:
    GroveNodeWatcherProxy(SernaApi::GroveWatcherBase* v)
        : sapi_visitor_(v) {}

    virtual void    nodeDestroyed(const GroveLib::Node* node)
    {
        //currently fall Serna to coredump
        //sapi_visitor_->nodeDestroyed(const_cast<Node*>(node));
    }
    virtual void    childInserted(const GroveLib::Node* node)
    {
        sapi_visitor_->childInserted(const_cast<Node*>(node));
    }

    virtual void    childRemoved(const GroveLib::Node* node,
                                 const GroveLib::Node* child)
    {
        sapi_visitor_->childRemoved(const_cast<Node*>(node),
                                    const_cast<Node*>(child));
    }
    virtual void    attributeChanged(const GroveLib::Attr* node)
    {
        sapi_visitor_->attributeChanged(const_cast<Attr*>(node));
    }
    virtual void    attributeRemoved(const GroveLib::Element* elem,
                                     const GroveLib::Attr* node)
    {
        sapi_visitor_->attributeRemoved(const_cast<Element*>(elem),
                                        const_cast<Attr*>(node));
    }
    virtual void    attributeAdded(const GroveLib::Attr* node)
    {
        sapi_visitor_->attributeAdded(const_cast<Attr*>(node));
    }

    virtual void    textChanged(const GroveLib::Text* node)
    {
        sapi_visitor_->textChanged(const_cast<Text*>(node));
    }

    virtual void    genericNotify(const GroveLib::Node* node, void* obj)
    {
        sapi_visitor_->genericNotify(const_cast<Node*>(node), obj);
    }

    virtual void    nsMappingChanged(const GroveLib::NodeWithNamespace* node,
                                     const Common::String& str)
    {
        sapi_visitor_->nsMappingChanged(const_cast<NodeWithNamespace*>(node),
                                        str);
    }

private:
    SernaApi::GroveWatcherBase* sapi_visitor_;
};

} // namespace


namespace SernaApi {

#define MY_IMPL   SAFE_CAST(GroveLib::Node*, getRep())
#define GSR_IMPL   SAFE_CAST(GroveLib::GroveSectionRoot*, getRep())
#define GNVP_IMPL   SAFE_CAST(GroveNodeWatcherProxy*, getRep())

GroveWatcher::GroveWatcher()
    : RefCountedWrappedObject(new GroveNodeWatcherProxy(this))
{
}

GroveWatcher::~GroveWatcher()
{
}


GroveNodeWatcher::GroveNodeWatcher()
    : RefCountedWrappedObject(new GroveNodeWatcherProxy(this))
{
}

GroveNodeWatcher::~GroveNodeWatcher()
{
}

bool GroveNodeWatcher::isRegisteredIn(const GroveNode& node) const
{
    if (getRep())
        return GNVP_IMPL->isRegisteredIn(
            static_cast<GroveLib::Node*>(node.getRep()));
    return false;
}

void GroveNodeWatcher::deregisterFromAllNodes() 
{
    if (getRep())
        GNVP_IMPL->deregisterFromAllNodes();
}

void GroveNode::registerNodeWatcher(GroveNodeWatcher* nv, short bitmask)
{
    MY_IMPL->registerNodeVisitor(
        static_cast<GroveNodeWatcherProxy*>(nv->getRep()), bitmask);
}
void GroveNode::deregisterNodeWatcher(GroveNodeWatcher* nv)
{
    MY_IMPL->deregisterNodeVisitor(
        static_cast<GroveNodeWatcherProxy*>(nv->getRep()));
}

void GroveSectionRoot::registerWatcher(GroveWatcher* v)
{
    GSR_IMPL->registerVisitor(
        static_cast<GroveNodeWatcherProxy*>(v->getRep()));
}

void GroveSectionRoot::deregisterWatcher(GroveWatcher* v)
{
    GSR_IMPL->deregisterVisitor(
        static_cast<GroveNodeWatcherProxy*>(v->getRep()));
}


} //SernaApi
