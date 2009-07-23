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
#include "xs/XsDataImpl.h"
#include "xs/XsIdTable.h"
#include "xs/Schema.h"
#include "xs/Piece.h"
#include "xs/Component.h"
#include "xs/ComponentSpace.h"
#include "common/asserts.h"
#include "grove/Nodes.h"
#include "xs/complex/Particle.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

const Component* XsData::getComponentById(const String& id) const
{
    XsIdTable<Component>::iterator it = componentIdt_.find(id);
    if (componentIdt_.end() != it)
        return it->pointer();
    return 0;
}

const Piece* XsData::getPieceById(const String& id) const
{
    XsIdTable<Piece>::iterator it = pieceIdt_.find(id);
    if (pieceIdt_.end() != it)
        return it->pointer();
    return 0;
}

ComponentSpaceBase* XsData::getOspaceByType(Component::ComponentType id)
{
    switch (id) {
        case Component::type:
            return typeSpace_.pointer();
        case Component::attribute:
            return attributeSpace_.pointer();
        case Component::element:
            return elementSpace_.pointer();
        case Component::group:
            return groupSpace_.pointer();
        case Component::attributeGroup:
            return attributeGroupSpace_.pointer();
        case Component::notation:
            return notationSpace_.pointer();
        case Component::identityConstraint:
            return identityConstraintSpace_.pointer();
        default:
            RT_MSG_ABORT("getOspaceByType: invalid OspaceType requested");
    }
    return 0;
}

XsData::~XsData()
{
}

XsData::XsData()
    : elemForm_(NcnCred::UNQUALIFIED),
      attrForm_(NcnCred::UNQUALIFIED)
{
}

XS_NAMESPACE_END
