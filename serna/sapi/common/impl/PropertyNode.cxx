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
#include "sapi/common/PropertyNode.h"
#include "sapi/common/impl/xtn_wrap_impl.h"
#include "proputils/PropertyTreeSaver.h"
#include "common/PropertyTree.h"
#include "common/safecast.h"

namespace SernaApi {

#define PN_IMPL(p) SAFE_CAST(Common::PropertyNode*, (p).getRep())
#define MY_IMPL SAFE_CAST(Common::PropertyNode*, getRep())

namespace {

class PropertyNodeWatcherProxy : public Common::SernaApiRefCounted,
                                 public Common::PropertyNodeWatcher {
public:
    PropertyNodeWatcherProxy(SernaApi::PropertyNodeWatcher* w)
        : sapi_watcher_(w) {}
    virtual void propertyChanged(Common::PropertyNode* property)
    {
        sapi_watcher_->propertyChanged(property);
    }
private:
    SernaApi::PropertyNodeWatcher* sapi_watcher_;
};

} // namespace

PropertyNodeWatcher::PropertyNodeWatcher()
    : RefCountedWrappedObject(new PropertyNodeWatcherProxy(this))
{
}

/////////////////////////////////////////////////////////////////////

PropertyNode::PropertyNode(SernaApiBase* rep)
    : RefCountedWrappedObject(rep)
{
}

PropertyNode::PropertyNode(const SString& name, const SString& value)
    : RefCountedWrappedObject(new Common::PropertyNode(name, value))
{
}

PropertyNode::PropertyNode(const SString& name, bool value)
    : RefCountedWrappedObject(new Common::PropertyNode(name, value))
{
}

PropertyNode::PropertyNode(const SString& name, int value)
    : RefCountedWrappedObject(new Common::PropertyNode(name, value))
{
}

PropertyNode::PropertyNode(const SString& name, double value)
    : RefCountedWrappedObject(new Common::PropertyNode(name, value))
{
}

void PropertyNode::addWatcher(PropertyNodeWatcher* w)
{
    if (getRep())
        MY_IMPL->addWatcher
            (static_cast<PropertyNodeWatcherProxy*>(w->getRep()));
}

void PropertyNode::removeWatcher(PropertyNodeWatcher* w)
{
    if (getRep())
        MY_IMPL->removeWatcher
            (static_cast<PropertyNodeWatcherProxy*>(w->getRep()));
}

void PropertyNode::removeAllWatchers()
{
    if (getRep())
        MY_IMPL->removeAllWatchers();
}

XTREENODE_WRAP_IMPL(PropertyNode, Common::PropertyNode)

PropertyNode PropertyNode::getProperty(const SString& name) const
{
    if (getRep())
        return MY_IMPL->getProperty(name);
    return 0;
}

PropertyNode PropertyNode::getSafeProperty(const SString& name) const
{
    if (getRep())
        return const_cast<Common::PropertyNode*>
            (MY_IMPL->getSafeProperty(name));
    return 0;
}

PropertyNode PropertyNode::makeDescendant(const SString& name)
{
    if (getRep())
        return MY_IMPL->makeDescendant(name);
    return 0;
}

PropertyNode PropertyNode::makeDescendant(const SString& name, 
                                          const SString& value,
                                          bool override)
{
    if (getRep())
        return MY_IMPL->makeDescendant(name, value, override);
    return 0;
}

SString PropertyNode::name() const
{
    if (isNull())
        return SString();
    return MY_IMPL->name();
}

void PropertyNode::setName(const SString& s)
{
    if (getRep())
        MY_IMPL->setName(s);
}

SString PropertyNode::getString() const
{
    if (isNull())
        return SString();
    return MY_IMPL->getString();
}

bool PropertyNode::getBool(bool* ok) const
{
    if (isNull())
        return false;
    return MY_IMPL->getBool(ok);
}

int PropertyNode::getInt(bool* ok) const
{
    if (isNull())
        return 0;
    return MY_IMPL->getInt(ok);
}

void* PropertyNode::getPtr(bool* ok) const
{
    if (isNull())
        return 0;
    return MY_IMPL->getPtr(ok);
}

double PropertyNode::getDouble(bool* ok) const
{
    if (isNull())
        return 0;
    return MY_IMPL->getDouble(ok);
}

void PropertyNode::setString(const SString& value)
{
    if (getRep())
        MY_IMPL->setString(value);
}

void PropertyNode::setBool(bool v)
{
    if (getRep())
        MY_IMPL->setBool(v);
}

void PropertyNode::setInt(int v)
{
    if (getRep())
        MY_IMPL->setInt(v);
}

void PropertyNode::setPtr(void* v)
{
    if (getRep())
        MY_IMPL->setPtr(v);
}

void PropertyNode::setDouble(double v)
{
    if (getRep())
        MY_IMPL->setDouble(v);
}

void PropertyNode::dump() const
{
    if (getRep())
        MY_IMPL->dump();
}

void PropertyNode::merge(const PropertyNode& other, bool override)
{
    if (getRep())
        MY_IMPL->merge(PN_IMPL(other), override);
}

PropertyNode PropertyNode::copy(bool recursive) const
{
    if (isNull())
        return 0;
    return MY_IMPL->copy(recursive);
}

bool PropertyNode::save(const SString& url) const
{
    if (isNull())
        return false;
    PropUtils::PropertyTreeSaver pts(MY_IMPL, MY_IMPL->name());
    return pts.savePropertyTree(url);
}

bool PropertyNode::read(const SString& url, bool merge)
{
    if (merge && getRep()) {
        PropUtils::PropertyTreeSaver pts(MY_IMPL, "");
        return pts.mergePropertyTree(url);
    }
    setRep(new Common::PropertyNode);
    PropUtils::PropertyTreeSaver pts(MY_IMPL, "");
    bool ok = pts.readPropertyTree(url);
    MY_IMPL->setName(pts.rootTag());
    return ok;
}

PropertyNode::~PropertyNode()
{
}

} // namespace SernaApi
