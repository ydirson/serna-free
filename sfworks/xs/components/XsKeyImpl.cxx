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

#include "xs/xs_defs.h"
#include "xs/components/XsKeyImpl.h"
#include "xs/Schema.h"
#include "common/PtrSet.h"
#include "common/RefCounted.h"
#include "xs/XsMessages.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"

#ifndef NO_XPATH
#include "xpath/Value.h"
#include "xpath/NodeSet.h"
#endif  //NO_XPATH
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

template <class V, class K> class KeyHolderFunction {
public:
    typedef K KeyType;
    static const K& key(const PtrBase<V>& kh) { return kh->name_; }
    static const K& key(const V* kh) { return kh->name_; }
};

class KeyHolder : public RefCounted<> {
public:
    KeyHolder(const String& name, GROVE_NAMESPACE::Node* n)
        : name_(name), node_(n){}
    String name_;
    GROVE_NAMESPACE::Node* node_;
};

class XsKeyImpl::KeyTable : public SmartPtrSet<RefCntPtr<KeyHolder>, KeyHolderFunction<KeyHolder, String> > {};

XsKeyImpl::XsKeyImpl(const Origin& origin, const NcnCred& cred)
    :  XsIdentityConstraint( XsIdentityConstraint::key, origin, cred),
       nodeTable_(new KeyTable())
{
}

bool XsKeyImpl::validate(Schema* schema,
                         GROVE_NAMESPACE::Element* elem) const
{
#ifndef NO_XPATH
    String sel = TS_COPY(selector());
    RefCntPtr<const XPATH_NAMESPACE::Value> val = evaluate(sel, elem);
    if (XPATH_NAMESPACE::Value::NODESET != val->type()) {
        if (0 != schema)
            schema->mstream() << XsMessages::notNodeset
            << Message::L_ERROR << ("selector:" + sel) << XS_ORIGIN;
        return false;
    }
    const XPATH_NAMESPACE::NodeSet& nset = val->getNodeSet();
    if (0 == nset.first()) {
        if (0 != schema)
            schema->mstream() << XsMessages::emptyNodeset
            << Message::L_ERROR << ("selector:" + sel) << XS_ORIGIN;
        return false;
    }
    for(uint j = 0; j < fields_.size(); j++) {
        const XPATH_NAMESPACE::NodeSetItem* ns_item = nset.first();
        for(; ns_item;ns_item = ns_item->next()) {
            GROVE_NAMESPACE::Node* node = ns_item->node();
            String fld = TS_COPY(field(j));
            if (GROVE_NAMESPACE::Node::ELEMENT_NODE != node->nodeType()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::notElement
                                      << Message::L_ERROR << XS_ORIGIN;
                nodeTable_->clear();
                return false;
            }
            RefCntPtr<const XPATH_NAMESPACE::Value> v = evaluate(fld, ELEMENT_CAST(node));
            if (XPATH_NAMESPACE::Value::NODESET != v->type()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::notNodeset
                    << Message::L_ERROR  << ("field:" + fld) << XS_ORIGIN;
                nodeTable_->clear();
                return false;
            }
            const XPATH_NAMESPACE::NodeSet& ns = v->getNodeSet();
            if (0 == ns.first()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::emptyNodeset
                    << Message::L_ERROR << ("field:" + fld) << XS_ORIGIN;
                nodeTable_->clear();
                return false;
            }
            GROVE_NAMESPACE::Node* top = ns.first()->node();
            String key_str = TS_COPY(v->to(XPATH_NAMESPACE::Value::STRING)->getString());
            if (nodeTable_->end() != nodeTable_->find(key_str)) {
                if (0 != schema)
                    schema->mstream() << XsMessages::keyViolation
                    << Message::L_ERROR << ("key:" + key_str)  << XS_ORIGIN;
                nodeTable_->clear();
                return false;
            }
            nodeTable_->insert(new KeyHolder(key_str, top));
        }
    }
#endif  //NO_XPATH
    return true;
}

bool XsKeyImpl::checkConstraint(const String& key) const
{
    return  (nodeTable_->end() != nodeTable_->find(key));
}


XsKeyImpl::~XsKeyImpl()
{
}


void XsKeyImpl::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    std::cerr << NOTR("XsKey:") << std::endl;
#endif // XS_DEBUG
}

PRTTI_IMPL(XsKeyImpl)

XS_NAMESPACE_END
