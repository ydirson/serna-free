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
#include "common/PropertyTree.h"
#include "common/StringTokenizer.h"
#include <map>
#include <iostream>


namespace Common {

////////////////////////////////////////////////////////////////////////

PropertyNodeWatcher::~PropertyNodeWatcher()
{
    deregister();
}

void PropertyNodeWatcher::deregister()
{
    PropertyNode** property = properties_.list();
    if (0 == property)
        return;
    for (; *property; ++property) {
        if (PropertyPtrSet::isValid(*property))
            (*property)->removeWatcher(this);
    }
}

const PropertyNodeWatcher::PropertyPtrSet&
PropertyNodeWatcher::propertyPtrSet() const
{
    return properties_;
}

bool PropertyNodeWatcher::isRegisteredIn(const PropertyNode* prop) const
{
    return properties_.check(prop);
}

////////////////////////////////////////////////////////////////////////

PropertyNode::PropertyNode(const String& name, double value)
    : name_(name)
{
    setDouble(value);
}

PropertyNode::PropertyNode(const String& name, int value)
    : name_(name)
{
    setInt(value);
}

PropertyNode::PropertyNode(const String& name, bool value)
    : name_(name)
{
    setBool(value);
}

PropertyNode::~PropertyNode()
{
    removeAllWatchers();
}

/* Watcher notifications
 */
void PropertyNode::addWatcher(PropertyNodeWatcher* watcher)
{
    if (!watchers_.check(watcher))
        watchers_.insert(watcher);
    if (!watcher->properties_.check(this))
        watcher->properties_.insert(this);
}

void PropertyNode::removeWatcher(PropertyNodeWatcher* watcher)
{
    watchers_.remove(watcher);
    watcher->properties_.remove(this);
}

void PropertyNode::removeAllWatchers()
{
    PropertyNodeWatcher** watcher = watchers_.list();
    if (0 == watcher)
        return;
    for (; *watcher; ++watcher) {
        if (PackedPointerSet<PropertyNodeWatcher>::isValid(*watcher))
            (*watcher)->properties_.remove(this);
    }
    watchers_.clear();
}

void PropertyNode::notifyChanged(PropertyNode* property)
{
    PropertyNode* root_node = root();
    if (root_node && this != root_node)
        root_node->notifyChanged(property);

    if (!watchers_.isNull()) {
        PropertyNodeWatcher** watcher = watchers_.list();
        for (; *watcher; ++watcher)
            if (PackedPointerSet<PropertyNodeWatcher>::isValid(*watcher))
                (*watcher)->propertyChanged(property);
    }
}

void PropertyNode::setName(const String& newName)
{
    name_ = newName;
    notifyChanged(this);
}

PropertyNode* PropertyNode::getProperty(const String& propertyPath) const
{
    if (propertyPath.isEmpty())
        return 0;
    PropertyNode* ptn = firstChild();
    for (StringTokenizer st(propertyPath, "/"); st; ) {
        String tok = st.next();
        if (tok.isEmpty())
            ptn = root();
        for (; ptn; ptn = ptn->nextSibling())
            if (ptn->name() == tok)
                break;
        if (0 == ptn)
            return 0;
        if (st)
            ptn = ptn->firstChild();
    }
    return ptn;
}

PropertyNode* PropertyNode::getProperty(const String& propertyPath1,
                                        const String& propertyPath2) const
{
    PropertyNode* pn = getProperty(propertyPath1);
    return pn ? pn->getProperty(propertyPath2) : 0;
}

PropertyNode* PropertyNode::getProperty(const String& propertyPath1,
                                        const String& propertyPath2,
                                        const String& propertyPath3) const
{
    PropertyNode* pn = getProperty(propertyPath1, propertyPath2);
    return pn ? pn->getProperty(propertyPath3) : 0;
}

static PropertyNode *empty_node = 0;

static PropertyNode* empty_node_inst()
{
    if (!empty_node) {
        empty_node = new PropertyNode;
        empty_node->incRefCnt();
    }
    return empty_node;
}

static const String& empty_node_string()
{
    return empty_node_inst()->getString();
}

const PropertyNode* PropertyNode::getSafeProperty(const String& proppath) const
{
    const PropertyNode* ptn = getProperty(proppath);
    return ptn ? ptn : empty_node_inst();
}
    
const String& PropertyNode::getString(const String& path) const
{
    return getSafeProperty(path)->getString();
}

const String& PropertyNode::getString(const String& path1,
                                      const String& path2) const
{
    const PropertyNode* pn = getProperty(path1, path2);
    return pn ? pn->getString() : empty_node_string();
}

const String& PropertyNode::getString(const String& path1,
                                      const String& path2,
                                      const String& path3) const
{
    const PropertyNode* pn = getProperty(path1, path2, path3);
    return pn ? pn->getString() : empty_node_string();
}

void PropertyNode::merge(const PropertyNode* other, bool override)
{
    typedef std::multimap<String, PropertyNode*> ToMap;
    ToMap to_map;
    ToMap::iterator to_iter;
    bool changed = false;
    PropertyNode* pn = other->getProperty("##additive##");
    if (pn) {
        pn->remove();
        if (!other->getString().isEmpty()) {
            value_ = value_ + other->value_;
            changed = true;
        }
    } else
        changed = (override) ? setValue(other) : false;
    pn = firstChild();
    for (; pn; pn = pn->nextSibling())
        to_map.insert(ToMap::value_type(pn->name(), pn));
    for (pn = other->firstChild(); pn; pn = pn->nextSibling()) {
        to_iter = to_map.find(pn->name());
        if (to_iter == to_map.end()) {      // not found - append
            appendChild(pn->copy(true));
            continue;
        }
        to_iter->second->merge(pn, override);
        to_map.erase(to_iter);
    }
    if (changed)
        notifyChanged(this);
}

bool PropertyNode::setValue(const PropertyNode* other)
{
    if (value_ == other->value_)
        return false;
    value_ = other->value_;
    return true;
}

static inline PropertyNode* make_descendant(PropertyNode* node,
                                            const String& path, bool& created)
{
    created = false;
    PropertyNode* ptn = node;
    for (StringTokenizer st(path, "/"); st; ) {
        String tok = st.next();
        PropertyNode* cn = ptn->getProperty(tok);
        if (cn)
            ptn = cn;
        else {
            ptn->appendChild(new PropertyNode(tok));
            ptn = ptn->lastChild();
            created = true;
        }
    }
    return ptn;
}

PropertyNode* PropertyNode::makeDescendant(const String& path)
{
    bool created = false;
    return make_descendant(this, path, created);
}

PropertyNode* PropertyNode::makeDescendant(const String& path,
                                           const String& value,
                                           bool override)
{
    bool created = false;
    PropertyNode* descendant = make_descendant(this, path, created);
    if (descendant && (override || created))
        descendant->setString(value);
    return descendant;
}

PropertyNode* PropertyNode::copy(bool recursive) const
{
    PropertyNode* n = new PropertyNode(name_, value_);
    if (false == recursive)
        return n;
    const PropertyNode* cn = firstChild();
    for (; cn; cn = cn->nextSibling())
        n->appendChild(cn->copy(true));
    return n;
}

bool PropertyNode::getBool(bool* ok) const
{
    String lv = value_.lower();
    if (lv == "true" || lv == "yes" || lv == "1") {
        if (ok)
            *ok = true;
        return true;
    }
    if (lv == "false" || lv == "no" || lv == "0") {
        if (ok)
            *ok = true;
        return false;
    }
    if (ok)
        *ok = false;
    return false;
}

int PropertyNode::getInt(bool* ok) const
{
    return value_.toInt(ok);
}

double PropertyNode::getDouble(bool* ok) const
{
    return value_.toDouble(ok);
}

const String& PropertyNode::getString(bool* ok) const
{
    if (ok)
        *ok = true;
    return value_;
}

///////////////////

void PropertyNode::setBool(bool v)
{
    setString((v) ? "true" : "false");
}

void PropertyNode::setDouble(double v)
{
    setString(String::number(v));
}

void PropertyNode::setInt(int v)
{
    setString(String::number(v));
}

void PropertyNode::setString(const String& sv)
{
    if (value_ == sv)
        return;
    value_ = sv;
    notifyChanged(this);
}

void PropertyNode::notifyChildInserted(PropertyNode* n)
{
    n->parent()->notifyChanged(n->parent());
    n->notifyChanged(n);
}

void PropertyNode::notifyChildRemoved(XLPT* p, PropertyNode*)
{
    PropertyNode* parent = static_cast<PropertyNode*>(p);
    parent->notifyChanged(parent);
}

void PropertyNode::dump(int indent) const
{
#ifndef NDEBUG
    for (int i = 0; i < indent; ++i)
        std::cerr << ' ';
    std::cerr << "PTN <" << name_ << ">";
    if (!value_.isNull())
        std::cerr << " = <" << value_ << ">";
    std::cerr << std::endl;
    indent += 2;
    for (PropertyNode* n = firstChild(); n; n = n->nextSibling())
        n->dump(indent);
#endif // NDEBUG
}

}

