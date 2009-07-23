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
#include "formatter/Area.h"
#include "formatter/AreaView.h"

namespace Formatter {

AreaSelection::AreaSelection()
    : from_(-1),
      to_(-1),
      extendsToLeft_(false),
      extendsToRight_(false)
{
}

bool AreaSelection::isEmpty() const
{
    return (from_ == to_);
}

bool AreaSelection::isNull() const
{
    return (from_ == -1 && to_ == -1);
}

void AreaSelection::set(long from, long to,
                        bool extendsToLeft, bool extendsToRight)
{
    from_ = from;
    to_ = to;
    extendsToLeft_ = extendsToLeft;
    extendsToRight_ = extendsToRight;
}

void AreaSelection::clear()
{
    from_ = -1;
    to_ = -1;
    extendsToLeft_ = false;
    extendsToRight_ = false;
}

void AreaView::setSelection(long from, long to)
{
    DBG(XSL.AREA) << "Selection area:" << area_ << " from:" << from
                  << " to:" << to << std::endl;
    long max = area()->chunkPosCount();
    selection_.set((from >= 0) ? from : 0,
                   (to < max) ? to : max,
                   from < 0,
                   to >= max);
}

void AreaView::removeSelection()
{
    selection_.clear();
}

}   // namespace Formatter

