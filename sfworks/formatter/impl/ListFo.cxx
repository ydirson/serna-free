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
#include "XslMessages.h"
#include "grove/Nodes.h"

#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"

using namespace GroveLib;
using namespace Common;

namespace Formatter {

void ListBlockFo::calcProperties(const Allocation& alloc)
{
    BlockLevelFo::calcProperties(alloc);
    startsDist_ = getProperty<ProvisionalDistance>(alloc).value();
    separation_ = getProperty<ProvisionalSeparation>(alloc).value();
    if (separation_ >= startsDist_)
        separation_ = startsDist_ / 10.0;
}

Fo* ListBlockFo::makeChildFo(const Node* foNode)
{
    //! TODO: straiten the acceptable children to be list-fo
    switch (contentType(foNode)) {
        case BLOCK_CONTENT :
            return ReferencedFo::makeChildFo(foNode);
            break;
        default:
            switch (foName(foNode)) {
                case SECTION_CORNER :
                case CHOICE :
                case COMMENT :
                case PI :
                case FOLD :
                case UNKNOWN :
                    {
                        Fo* fo = ReferencedFo::makeChildFo(foNode);
                        SAFE_CAST(ExtensionFo*, fo)->
                            setContentType(BLOCK_CONTENT);
                        return fo;
                    }
                    break;
                default:
                    break;
            }
            break;
    }
    data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                   << foNode->nodeName() << node()->nodeName();
    return 0;
}

String ListBlockFo::name() const
{
    return "ListBlockFo";
}

String ListBlockFo::areaName() const
{
    return "ListBlockArea";
}

//////////////////////////////////////////////////////////////////////

CRect ListItemFo::getSpaceAfter(const Area*, bool isFirst,
                                const Area*, const CRect& space) const
{
    CType bord = 0;
    CType padd = 0;
    CType h = (isFirst) ? getDecoration(START_DECOR, bord, padd) : 0;
    return CRect(CPoint(-startIndent_, 0),
                 CRange(space.extent_.w_, space.extent_.h_ - h));
}

CType ListItemFo::childrenHeight(const Area* area) const
{
    CType max_height = 0;
    for (Area* a = area->firstChild(); a; a = a->nextSibling())
        if (max_height < a->allcH())
            max_height = a->allcH();
    return max_height;
}

Fo* ListItemFo::makeChildFo(const Node* foNode)
{
    //! TODO: straiten the acceptable children to be list-item-label(body)
    switch (contentType(foNode)) {
        case BLOCK_CONTENT :
            return ReferencedFo::makeChildFo(foNode);
            break;
        default:
            switch (foName(foNode)) {
                case SECTION_CORNER:
                case CHOICE:
                case COMMENT:
                case PI :
                case FOLD :
                case UNKNOWN:
                    {
                        Fo* fo = ReferencedFo::makeChildFo(foNode);
                        SAFE_CAST(ExtensionFo*, fo)->
                            setContentType(BLOCK_CONTENT);
                        return fo;
                    }
                    break;
                default:
                    break;
            }
            break;
    }
    data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                   << foNode->nodeName() << node()->nodeName();
    return 0;
}

String ListItemFo::name() const
{
    return "ListItemFo";
}

String ListItemFo::areaName() const
{
    return "ListItemArea";
}

//////////////////////////////////////////////////////////////////

void ListItemLabelFo::calcProperties(const Allocation& alloc)
{
    BlockFo::calcProperties(alloc);
    bord_ = padd_ = ORect();
    contColor_ = Rgb(0, 0, 0, true);
}

String ListItemLabelFo::name() const
{
    return "ListItemLabelFo";
}

String ListItemLabelFo::areaName() const
{
    return "ListItemLabelArea";
}

//////////////////////////////////////////////////////////////////

void ListItemBodyFo::calcProperties(const Allocation& alloc)
{
    BlockFo::calcProperties(alloc);
    bord_ = padd_ = ORect();
    contColor_ = Rgb(0, 0, 0, true);
}

String ListItemBodyFo::name() const
{
    return "ListItemBodyFo";
}

String ListItemBodyFo::areaName() const
{
    return "ListItemBodyArea";
}

} // namespace Formatter
