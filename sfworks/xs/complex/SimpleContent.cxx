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
#include "xs/XsType.h"
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "xs/complex/SimpleContent.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/components/AttributeSet.h"
#include "xs/datatypes/DerivationTypes.h"
#include "xs/SchemaNamespaces.h"
#include "grove/ValidationCommandsMaker.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

SimpleContent::SimpleContent(DerivationType type, const Origin& o, const XsId& xsid)
    : XsContent(type, o, xsid), attrsJoined_(false)
{
}

SimpleContent::~SimpleContent()
{
}

static void join_attribute_sets(XsType* type, AttributeSet* aset)
{
    while (type) {
        const XsComplexTypeImpl* xcti = type->asConstXsComplexTypeImpl();
        if (0 == xcti) 
            return;
        SimpleContent* sc = xcti->content()->asSimpleContent();
        if (0 == sc)
            break;
        if (sc->attributeSet())
            aset->join(sc->attributeSet().pointer());
        if (sc->restriction())
            type = sc->restriction()->baseType().pointer();
        else
            break;
    }
}

void SimpleContent::joinAttributeSets()
{
    if (restriction()) {
        XsType* type = restriction()->baseType().pointer();
        if (type && type->typeClass() != XsType::unknown)
            join_attribute_sets(type, attributeSet_.pointer());
    }
    attrsJoined_ = true;
}

bool SimpleContent::validate(Schema* schema,
                             GROVE_NAMESPACE::Element* elem,
                             bool /*mixed*/,
                             bool /*recursive*/) const
{
    Vector<GROVE_NAMESPACE::Attr*> attrs;
    ulong i;
    if (!attrsJoined_ && derivationType() == EXTENSION) 
        const_cast<SimpleContent*>(this)->joinAttributeSets();
    for (Attr* a = elem->attrs().firstChild(); a; a = a->nextSibling())
        attrs.push_back(a);
    bool ok = true;
    if (attributeSet_)
        ok = attributeSet_->validate(schema, elem, attrs) && ok;
    for (i = 0; i < attrs.size(); ++i) {
        if (schema && XSI_NAMESPACE != attrs[i]->expandedName().uri())
            schema->mstream() << XsMessages::attrExtra << elem->nodeName()
                              << attrs[i]->nodeName()
                              << Message::L_ERROR
                              << DV_ORIGIN(attrs[0]);
        ok = false;
    }
    return validate_simple_data(schema, rst_.pointer(), elem) && ok;
}

bool SimpleContent::validate(Schema* schema,
                             const Node* o,
                             const String& source,
                             String* result) const
{
    if (rst_.isNull()) {
        if (source.isEmpty())
            return true;
        if (schema)
            schema->mstream() << XsMessages::simpleContentEmpty
                              << Message::L_ERROR
                              << DV_ORIGIN(o);
        return false;
    }
    return rst_->validate(schema, o, source, result);
}

bool SimpleContent::makeContent(Schema* schema,
                                GROVE_NAMESPACE::Element* pe,
                                FixupSet* fs,
                                const String* defv) const
{
    if (rst_.isNull())
        return true;
    return rst_->makeContent(schema, pe, fs, defv);
}

void SimpleContent::setRestriction(RefCntPtr<Restriction>& rst)
{
    rst_ = rst;
}

void SimpleContent::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
        std::cerr << ' ';
    std::cerr << NOTR("SimpleContent:") << std::endl;
    rst_->dump(indent+1);
    XsContent::dumpAttrs(indent+1);
#endif // XS_DEBUG
}

PRTTI_IMPL(SimpleContent);

XS_NAMESPACE_END
