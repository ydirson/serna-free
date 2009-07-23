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
#include "xs/components/XsKeyRefImpl.h"
#include "xs/Schema.h"
#include "xs/components/XsKeyImpl.h"
#include "xs/XsMessages.h"
#ifdef __SUNPRO_CC
# include "Schema.h"
# include "XsDataImpl.h"
#endif
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

XsKeyRefImpl::XsKeyRefImpl(const Origin& origin, const NcnCred& cred)
    :  XsIdentityConstraint( XsIdentityConstraint::keyRef, origin, cred)
{
}

bool XsKeyRefImpl::validate(Schema* schema,
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
                return false;
            }
            RefCntPtr<const XPATH_NAMESPACE::Value> v = evaluate(fld, ELEMENT_CAST(node));

            if (XPATH_NAMESPACE::Value::NODESET != v->type()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::notNodeset
                    << Message::L_ERROR << ("field:" + fld) << XS_ORIGIN;
                return false;
            }
            const XPATH_NAMESPACE::NodeSet& ns = v->getNodeSet();
            if (0 == ns.first()) {
                continue;
            }
            String key_str = TS_COPY(v->to(XPATH_NAMESPACE::Value::STRING)->getString());
            if(!key_.isNull()) {
                if(key_->asXsKeyImpl()) {
                    if(!key_->asXsKeyImpl()->checkConstraint(key_str)) {
                        if (0 != schema)
                            schema->mstream() << XsMessages::noCorrespondedKey
                            << Message::L_ERROR << XS_ORIGIN;
                        return false;
                    }
                }
                else {
                    if (0 != schema)
                        schema->mstream() << XsMessages::referenceToUndefined
                                          << Message::L_ERROR
                                          << key_->constCred()->format()
                                          << XS_ORIGIN;
                }
            }
        }
    }
#endif //NO_XPATH
    return true;
}

void XsKeyRefImpl::setReference(const XsIdentityConstraintInst& ic)
{
    key_ = ic;
}

XsKeyRefImpl::~XsKeyRefImpl()
{
}


void XsKeyRefImpl::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
        std::cerr << ' ';
    }
    std::cerr << NOTR("XsKeyRef:") << std::endl;
#endif // XS_DEBUG
}

PRTTI_IMPL(XsKeyRefImpl)

    XS_NAMESPACE_END
