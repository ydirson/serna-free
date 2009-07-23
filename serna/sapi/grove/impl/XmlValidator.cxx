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
#include "sapi/grove/XmlValidator.h"
#include "sapi/grove/Grove.h"
#include "sapi/grove/GroveNodes.h"
#include "grove/XmlValidator.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "common/safecast.h"

namespace SernaApi {

XmlValidatorProvider::XmlValidatorProvider(SernaApiBase* xvp)
    : RefCountedWrappedObject(xvp)
{
}

XmlValidator XmlValidatorProvider::getValidator(const Grove& grove) const
{
    GroveLib::XmlValidatorProvider* xvp = 
        SAFE_CAST(GroveLib::XmlValidatorProvider*, getRep());
    if (0 == xvp)
        return XmlValidator();
    bool ok = false;
    GroveLib::XmlValidatorPtr gxvp = 
        xvp->getValidator(SAFE_CAST(GroveLib::Grove*, grove.getRep()), &ok);
    return XmlValidator(const_cast<GroveLib::XmlValidator*>(gxvp.pointer()));
}

XmlValidator::XmlValidator(SernaApiBase* xv)
    : RefCountedWrappedObject(xv)
{
}

bool XmlValidator::validate(const GroveElement& elem, int vFlags) const
{
    GroveLib::XmlValidator* xv = SAFE_CAST(GroveLib::XmlValidator*, getRep());
    if (0 == xv)
        return false;
    return xv->validate(SAFE_CAST(GroveLib::Element*, elem.getRep()), vFlags);
}

} // namespace SernaApi
