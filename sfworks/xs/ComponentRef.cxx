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
#include "xs/ComponentRef.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/ImportMap.h"
#include "common/ThreadingPolicies.h"
#include "xs/complex/Particle.h"

XS_NAMESPACE_BEGIN

void ComponentRefBase::component_base(RefCntPtr<Component>& ic) const
{
    if (component_) {
        ic = component_;
        return;
    }
    XsDataImpl* xsi = schema_->xsi();
    Schema* ischema = xsi->importMap()->ischema(cred_.xmlns());
    if (0 == ischema) {
        ic = 0;
        return;
    }
    XsDataImpl::Lock(*ischema->xsi());
    ComponentSpaceBase* cs =
        ischema->xsi()->getOspaceByType((Component::ComponentType)type_);
    if (withinRedef_ && cred_.xmlns() == xsi->targetNsUri())
        ic = cs->lookupBase(NcnCred(cred_.name(),
            xsi->makeCred(String(), true).xmlns()));
    if (ic)
        return;
    ic = cs->lookupBase(cred_);
}

XS_NAMESPACE_END
