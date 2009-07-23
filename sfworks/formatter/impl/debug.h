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

#ifndef FORMATTER_DEBUG_H
#define FORMATTER_DEBUG_H

#include "formatter/formatter_defs.h"

#define DBG_EXPIMP FORMATTER_EXPIMP
#define DBG_TAGS    DTAG(TEST) DTAG(FODYN) DTAG(FO) DTAG(CHAIN) DTAG(AREA) \
                    DTAG(LAYOUT) DTAG(PROP) DTAG(PROPDYN) DTAG(TYPES) \
        DTAG(BRANCH) DTAG(FORMATTER) DTAG(DEV) DTAG(SHIFT) DTAG(CURSOR) \
        DTAG(SAFE) DTAG(FSM)
#define DBG_MODULE DMOD(XSL)
#define DBG_DEFAULT_TAG XSL.TEST

#include "common/Debug.h"
#include <iostream>
#include <iomanip>

template<class T> struct hex_val {
    hex_val(const T& t) : t_(t) {}
    const T& t_;
};

template<class T>
inline hex_val<T> pntr(const T& t)
{
    return hex_val<T>(t);
}

template<class T> inline std::ostream&
operator << (std::ostream& os, const hex_val<T>& hval)
{
    if (0 == hval.t_)
        return os << "0x0";
    return os << "0x" << hval.t_;
}

template<class T> struct fixed_val {
    fixed_val(const T& t) : t_(t) {}
    const T& t_;
};

template<class T>
inline fixed_val<T> fix_fmt(const T& t)
{
    return fixed_val<T>(t);
}

struct save_flags {
    save_flags(std::ostream& os)
     :  os_(os), flags_(os.flags()), width_(os.width()),
        prec_(os.precision()), fill_(os.fill()) {}
    ~save_flags()
    {
        os_.flags(flags_);
        os_.width(width_);
        os_.precision(prec_);
        os_.fill(fill_);
    }
private:
    std::ostream& os_;
    std::ostream::fmtflags   flags_;
    unsigned                 width_;
    unsigned                 prec_;
    char                     fill_;
};

template<class T> inline std::ostream&
operator << (std::ostream& os, const fixed_val<T>& fval)
{
    save_flags sflags(os);
    return os << std::setiosflags(std::ios::fixed) << std::setprecision(3)
              << std::setfill('0') << fval.t_;
}

namespace Formatter {

template<typename T> class OValue;

template<typename T> void dump_oval(const OValue<T>& ov)
{
    DBG(XSL.TYPES)
        << " top:" << ov.top_ << " bottom:" << ov.bottom_ << " left:"
        << ov.left_ << " right:" << ov.right_ << std::endl;
}

}

#endif
