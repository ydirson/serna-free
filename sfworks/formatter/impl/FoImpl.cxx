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

#include "formatter/impl/debug.h"
#include "formatter/impl/FoImpl.h"

namespace Formatter {

FoImpl::FoImpl(PropertySet* parentSet, bool hasParentOrigin, FoData& data)
    : Fo(parentSet, hasParentOrigin),
      data_(data),
      isRegistered_(false)
{}

FoImpl::~FoImpl()
{
    deregisterModification();
    detachFromFormatter(0);
}

bool FoImpl::isChainFinishedAt(const AreaPtr& area) const
{
    if (needsDecoration(END_DECOR))
        return area->hasDecoration(END_DECOR);
    if (needsDecoration(EMPTY_DECOR))
        return area->hasDecoration(EMPTY_DECOR);
    return true;
}

#include "common/TreelocRep.h"
void FoImpl::registerModification(ModificationType mType)
{
    if (!isRegistered_) {
        DBG(XSL.FODYN)
            << name() << ':' << this << " register modification" << std::endl;
        data_.foRegistry_.add(this);
        isRegistered_ = true;
        mType_ = mType;
    }
}

void FoImpl::deregisterModification()
{
    if (isRegistered_) {
        DBG(XSL.FO)
            << "Fo:" << this << " deregister modification" << std::endl;
        data_.foRegistry_.deregisterInstance(this);
        isRegistered_ = false;
        mType_ = NO_MODIFICATION;
    }
    isGeometryModified_ = false;
}

void FoImpl::detachFromFormatter(const FoController* preserveBranch)
{
    if (0 != formattedBy_)
        data_.foRegistry_.detachFromFormatter(this, preserveBranch);
}

static void calc_baseline(const Area* area, CType& maxAccender,
                          CType& maxDescender, const CType baselineShift)
{
    for (Area* a = area->firstChild(); a; a = a->nextSibling()) {
        const CType a_accend = a->base() + baselineShift;
        if (a_accend > maxAccender)
            maxAccender = a_accend;
        const CType a_descend = a->allcH() - a->base() - baselineShift;
        if (a_descend > maxDescender)
            maxDescender = a_descend;
        calc_baseline(a, maxAccender, maxDescender, 
                      a->baselineShift() + baselineShift);
    }
}

void FoImpl::layoutChildrenOnBaseline(Area* area, const CType& accender,
                                      const CType& descender) const
{
    CType max_accender = accender;
    CType max_descender = descender;

    calc_baseline(area, max_accender, max_descender, 0);
    area->contRange_.h_ = max_accender + max_descender;
    area->base_ = max_accender;
    // Place children the right way around.
    for (Area* a = area->firstChild(); a; a = a->nextSibling())
        a->allcPoint_.y_ = area->base_ - a->base();
}

} // namespace Formatter
