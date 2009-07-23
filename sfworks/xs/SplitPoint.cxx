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
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/Origin.h"
#include "xs/XsMessages.h"
#include "xs/XsElement.h"
#include "xs/components/XsElementImpl.h"
#include "xs/XsType.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/Particle.h"
#include "xs/complex/Connector.h"
#include "xs/complex/ComplexContent.h"

USING_COMMON_NS

bool Schema::splitPoint(const Vector<String>& path,
                        Vector<int>& splitPoints) const
{
    splitPoints.clear();
    if (path.size() < 2)
        return false;
    XS_NAMESPACE::XsElement* elem =
        xsd_->elementSpace().lookup(path[0]);
    if (0 == elem)
        return false;
    ulong level = 1;
    for (;;) {
        if (0 == elem)
            break;
        if (level >= path.size())
            return true;
        const XS_NAMESPACE::XsType* baseType =
            static_cast<const XS_NAMESPACE::XsElementImpl*>(elem)->xstype();
        const XS_NAMESPACE::XsComplexTypeImpl* xcti =
            baseType->asConstXsComplexTypeImpl();
        if (0 == xcti)  // parent cannot be of a simple type
            break;
        const XS_NAMESPACE::XsContent* xscontent = xcti->content();
        if (0 == xscontent)
            break;
        const XS_NAMESPACE::ComplexContent* cc =
            xscontent->asConstComplexContent();
        if (0 == cc)
            break;
        const XS_NAMESPACE::Connector* connector = cc->connector();
        if (0 == connector)
            break;
        elem = 0;
        int maxocc = connector->checkSplit(path[level], elem);
        if (maxocc > 0)
            splitPoints.push_back(level);
        ++level;
    }
    splitPoints.clear();
    return false;
}
