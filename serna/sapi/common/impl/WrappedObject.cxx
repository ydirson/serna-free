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
#include "sapi/common/WrappedObject.h"
#include "common/SernaApiRefCounted.h"
#include "common/safecast.h"

namespace SernaApi {

SimpleWrappedObject::SimpleWrappedObject(SernaApiBase* rep)
    : rep_(rep) {}

SimpleWrappedObject::~SimpleWrappedObject()
{
}

void SimpleWrappedObject::setRep(SernaApiBase* rep)
{
    rep_ = rep;
}

void SimpleWrappedObject::releaseRep()
{
    rep_ = 0;
}

/////////////////////////////////////////////////////////////////////

RefCountedWrappedObject::RefCountedWrappedObject(SernaApiBase* rep)
    : SimpleWrappedObject(rep)
{
    Common::SernaApiRefCounted* rcnt_rep =
        SAFE_CAST(Common::SernaApiRefCounted*, getRep());
    if (getRep())
        rcnt_rep->incRefCnt();
}

void RefCountedWrappedObject::decRepRefCnt()
{
    Common::SernaApiRefCounted* rcnt_rep =
        static_cast<Common::SernaApiRefCounted*>(getRep());
    if (rcnt_rep)
        rcnt_rep->decRefCnt();
}

RefCountedWrappedObject::~RefCountedWrappedObject()
{
    do_setrep(0);
}

void RefCountedWrappedObject::setRep(SernaApiBase* rep)
{
    do_setrep(rep);
}

void RefCountedWrappedObject::do_setrep(SernaApiBase* rep)
{
    Common::SernaApiRefCounted* rcnt_rep =
        static_cast<Common::SernaApiRefCounted*>(getRep());
    if (getRep() && rcnt_rep->decRefCnt() <= 0)
        delete rcnt_rep;
    SimpleWrappedObject::setRep(rep);
    rcnt_rep = SAFE_CAST(Common::SernaApiRefCounted*, rep);
    if (rep)
        rcnt_rep->incRefCnt();
}

RefCountedWrappedObject::RefCountedWrappedObject
    (const RefCountedWrappedObject& other)
    : SimpleWrappedObject()
{
    (void) operator=(other);
}

RefCountedWrappedObject&
RefCountedWrappedObject::operator=(const RefCountedWrappedObject& other)
{
    do_setrep(other.getRep());
    return *this;
}

///////////////////////////////////////////////////////////////////

OwnedWrappedObject::OwnedWrappedObject(SernaApiBase* rep)
    : SimpleWrappedObject(rep)
{
}

OwnedWrappedObject::~OwnedWrappedObject()
{
    do_setrep(0);
}

void OwnedWrappedObject::setRep(SernaApiBase* rep)
{
    do_setrep(rep);
}

void OwnedWrappedObject::do_setrep(SernaApiBase* rep)
{
    if (getRep())
        delete getRep();
    SimpleWrappedObject::setRep(rep);
}

OwnedWrappedObject::OwnedWrappedObject(const OwnedWrappedObject& other)
    : SimpleWrappedObject()
{
    (void) operator=(other);
}

OwnedWrappedObject&
OwnedWrappedObject::operator=(const OwnedWrappedObject& other)
{
    do_setrep(other.getRep());
    const_cast<OwnedWrappedObject&>(other).releaseRep();
    return *this;
}

} // namespace
