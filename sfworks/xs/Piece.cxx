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

#include "xs/Piece.h"
#include "grove/Nodes.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/XsMessages.h"
#include "xs/complex/Particle.h"

USING_COMMON_NS
XS_NAMESPACE_BEGIN

Piece::Piece(const Origin& o, const XsId& xsid)
    : origin_(o), parent_(0), errmsg_(String::null()), noUserMsg_(false)
{
    // TODO: put id into table
    (void) xsid;
}

Piece::~Piece()
{
}

const GroveLib::Element* Piece::origin() const
{
    return origin_;
}

void Piece::setParentPiece(Piece* parent)
{
    parent_ = parent;
}

void  Piece::setAppinfo(RefCntPtr<Appinfo>&  appinfo)
{
    appinfo_ = appinfo;
}

const Appinfo* Piece::appinfo() const
{
    return appinfo_.pointer();
}

void Piece::setCustomMessage(String& errmsg)
{
    errmsg_ = errmsg;
}

String Piece::customMessage() const
{
    return errmsg_;
}

bool Piece::checkCustomMessage() const
{
    if (!customMessage().isNull())
        return true;
    Piece* p = parent();
    while ((0 != p) && (p->customMessage().isNull()))
        p = p->parent();
    return (0 != p);
}

String Piece::format() const
{
    if (!noUserMsg_) {
        if (!customMessage().isNull())
            return customMessage();
        Piece* p = parent();
        while ((0 != p) && (p->customMessage().isNull()))
            p = p->parent();
        if (0 != p)
            return TS_COPY(p->customMessage());
    }
    return String::null();
}

void Piece::putIntoTable(Schema* schema, const String& id) const
{
    return;
    Piece* casted = const_cast<Piece*>(this);
    casted->setId(id);
    bool ok = schema->xsi()->putPieceIntoTable(casted);
    if (!ok)
        schema->mstream() << XsMessages::idConflict
            << XS_ORIGIN << Message::L_ERROR;
}

XS_NAMESPACE_END
