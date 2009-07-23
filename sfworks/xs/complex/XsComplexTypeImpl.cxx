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
 *  $RCSfile: XsComplexTypeImpl.cxx,v $
 *
 ***********************************************************************/

#include "xs/xs_defs.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/XsContent.h"
#include "xs/XsMessages.h"
#include "grove/Nodes.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/ExtPythonScript.h"
#include "xs/complex/Particle.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

XsComplexTypeImpl::XsComplexTypeImpl(const Origin& origin,
                                     const NcnCred& cred)
  : XsType(XsType::complexType, origin, cred),
    block_(NONE), final_(NONE), abstract_(false),
    mixed_(false), pyFunc_(String::null())
{
}

XsComplexTypeImpl::~XsComplexTypeImpl()
{
}

void XsComplexTypeImpl::setContent(RefCntPtr<XsContent>& content)
{
    content_ = content;
}

XsContent* XsComplexTypeImpl::content() const
{
    return content_.pointer();
}

bool XsComplexTypeImpl::cdataAllowed() const
{
    if (isMixed())
        return true;
    if (content() && content()->cdataAllowed())
        return true;
    return false;
}

bool XsComplexTypeImpl::check_abstract(Schema* schema) const
{
    if (abstract_) {
        if (schema)
            schema->mstream() << XsMessages::cannotValidateAbstract
            << Message::L_ERROR  << XS_ORIGIN;
        return true;
    }
    return false;
}

bool XsComplexTypeImpl::validate(Schema* schema,
                                 GROVE_NAMESPACE::Element* elem,
                                 bool recursive) const
{
    bool valid = false;
    if (!check_abstract(schema))
        if (0 != content_)
                valid = content_->validate(schema, elem, mixed_, recursive);

#ifdef USE_PYTHON
    if ((!pyFunc().isNull()) && (valid)) {
        valid = schema->xsi()->pyScript()->
            pyValidateComplex(schema, elem, this, pyFunc());
        if (!valid)
            if (schema)
                schema->mstream() << XsMessages::scriptReturn
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
    }
#endif // USE_PYTHON
    return valid;
}

bool XsComplexTypeImpl::validate(Schema* schema,
                                 const String& source,
                                 String* result) const
{
    bool valid = false;
    if (!check_abstract(schema)) {
        if (!mixed_) {
            valid = content_->validate(schema, 0, source, result);
        }
        else
            if (schema)
                schema->mstream() << XsMessages::cannotDataValidateMixed
                << Message::L_ERROR  << XS_ORIGIN;
    }
#ifdef USE_PYTHON
    if ((!pyFunc().isNull()) && (valid)) {
        valid = schema->xsi()->pyScript()->
            pyValidateComplex(schema, 0, this, pyFunc());
        if (!valid)
            if (schema)
                schema->mstream() << XsMessages::scriptReturn
                                  << Message::L_ERROR
                                  << XS_ORIGIN;
    }
#endif // USE_PYTHON
    return valid;
}

bool XsComplexTypeImpl::makeContent(Schema* s,
                                    GROVE_NAMESPACE::Element* elem,
                                    FixupSet* elemSet,
                                    const String* defv) const
{
    if (content_.isNull())
        return true;
    return content_->makeContent(s, elem, elemSet, defv);
}

void XsComplexTypeImpl::makeAttrs(Schema* s, GROVE_NAMESPACE::Element* elem) const
{
    if (content_.isNull())
        return;
    content_->makeAttrs(s, elem);
}

const String& XsComplexTypeImpl::pyFunc() const
{
    return pyFunc_;
}

void XsComplexTypeImpl::setPyFunc(String& funcName)
{
    pyFunc_ = funcName;
}

void XsComplexTypeImpl::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    std::cerr << NOTR("XsComplexType:") << std::endl;
    content_->dump(indent+1);
#endif // XS_DEBUG
}

PRTTI_IMPL(XsComplexTypeImpl)

XS_NAMESPACE_END
