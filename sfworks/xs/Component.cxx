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
#include "xs/Appinfo.h"
#include "xs/Origin.h"
#include "xs/NcnCred.h"
#include "xs/Schema.h"
#include "xs/Component.h"
#include "xs/XsMessages.h"
#include "xs/XsDataImpl.h"
#include "xs/ComponentSpace.h"
#include "grove/Nodes.h"
#include "xs/complex/Particle.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

Component::Component(ComponentType t, const Origin& origin, const NcnCred& cr)
    : Piece(origin), ComponentCred(cr), componentType_(t)
{
}

Component::~Component()
{
}

bool Component::refToUndef(Schema* s) const
{
    if (s)
        s->mstream() << XsMessages::referenceToUndefined << Message::L_ERROR
            << constCred()->format();
    return false;
}

void Component::putIntoTable(Schema* schema, const String& id) const
{
    return;
    Component* casted = const_cast<Component*>(this);
    casted->setId(id);
    bool ok = schema->xsi()->putComponentIntoTable(casted);
    if (!ok)
        schema->mstream() << XsMessages::idConflict
            << Message::L_ERROR << XS_ORIGIN;
    Piece::putIntoTable(schema, id);
}

// Redefinition. Idea is to switch cred's between redefined and original
// component (redefined components are placed to private namespace by the
// schema parser). Also, ComponentRef checks whether it resolves reference
// from within the redefine, and if so, it redirects ref to the pvt namespace.

void Component::processRedefine(Schema* schema, Component* origComponent)
{
    ComponentSpaceBase* cs = schema->xsi()->getOspaceByType(componentType());
    RefCntPtr<Component> origComponentHolder(origComponent);
    RefCntPtr<Component> selfHolder(this);
    NcnCred cred = *constCred();
    cs->erase(origComponent);
    cs->erase(this);
    setCred(*origComponent->constCred());
    origComponent->setCred(cred);
    cs->insert(this);
    cs->insert(origComponent);
}

PRTTI_BASE_STUB(Component, XsType)
PRTTI_BASE_STUB(Component, XsAttribute)
PRTTI_BASE_STUB(Component, XsGroup)
PRTTI_BASE_STUB(Component, XsAttributeGroup)
PRTTI_BASE_STUB(Component, XsElement)
PRTTI_BASE_STUB(Component, XsNotation)
PRTTI_BASE_STUB(Component, XsIdentityConstraint)

XS_NAMESPACE_END

