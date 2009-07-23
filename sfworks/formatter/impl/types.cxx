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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "debug.h"
#include "formatter/types.h"
#include "common/String.h"
#include "XslMessages.h"

#include <iostream>
#include <limits>

#if defined(_WIN32)
# undef max
#endif

namespace Formatter {
    
const CType CTYPE_MAX = std::numeric_limits<double>::max();

XslException::XslException(uint m, const COMMON_NS::String& str)
    : COMMON_NS::Exception(str)
{
    message_ = new COMMON_NS::UintIdMessage(m, XslMessages::getFacility());
    message_->appendArgInstance
              (new COMMON_NS::MessageArg<COMMON_NS::String>(str));
}

std::ostream& operator<<(std::ostream& os, const Rgb& color)
{
    return os << " r:" << color.r_ << " g:" << color.g_
              << " b:" << color.b_ << " tr:" << color.isTransparent_;
}

void CRange::dump() const
{
    DBG(XSL.TYPES) << "range w:" << w_ << " h:" << h_ << std::endl;
}

void CPoint::dump() const
{
    DBG(XSL.TYPES) << "point x:" << x_ << " y:" << y_ << std::endl;
}

void CRect::dump() const
{
    DBG(XSL.TYPES) << "rect x:" << origin_.x_ << " y:" << origin_.y_ 
                   << " w:" << extent_.w_ << " h:" << extent_.h_
                   << std::endl;
}

void Rgb::dump() const
{
    DBG(XSL.TYPES) << "Color r:" << r_ << " g:" << g_ << " b:" << b_
                   << " tr:" << isTransparent_ << std::endl;
}

void ORect::dump() const
{
    dump_oval(*this);
}

}
