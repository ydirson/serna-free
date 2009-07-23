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

#ifndef FORMATTER_TYPES_H
#define FORMATTER_TYPES_H

#include "common/common_types.h"
#include "common/String.h"
#include "common/Exception.h"
#include "common/Message.h"

#include "formatter/formatter_defs.h"

namespace Formatter {

class FORMATTER_EXPIMP XslException : public COMMON_NS::Exception {
public:
    FORMATTER_OALLOC(Exception);
    COMMON_NS::Message* message() { return message_.pointer(); }
    XslException(uint m, const COMMON_NS::String& str);
    ~XslException() throw() {}

protected:
    COMMON_NS::RefCntPtr<COMMON_NS::Message> message_;
};

typedef double CType;
FORMATTER_EXPIMP extern const CType CTYPE_MAX;

/*! \brief Coordinate range
 */

class FORMATTER_EXPIMP CRange {
public:
    FORMATTER_OALLOC(CRange);

    CRange(CType w = 0, CType h = 0)
        : w_(w), h_(h) {}

    bool    operator==(const CRange& r) const
    {
        return (r.w_ == w_ && r.h_ == h_);
    }
    bool    operator!=(const CRange& r) const { return !(*this == r); }

    void    dump() const;

public:
    CType   w_;
    CType   h_;
};

/*! \brief Coordinate point
 */
class FORMATTER_EXPIMP CPoint {
public:
    FORMATTER_OALLOC(CPoint);

    CPoint()
        : x_(0), y_(0) {}
    CPoint(CType x, CType y)
        : x_(x), y_(y) {}

    //! Shifts point by given value
    CPoint& operator+=(const CPoint& p)
        {
            x_ += p.x_;
            y_ += p.y_;
            return *this;
        }
    CPoint& operator-=(const CPoint& p)
        {
            x_ -= p.x_;
            y_ -= p.y_;
            return *this;
        }
    bool    operator==(const CPoint& p) const
        {
            return (p.x_ == x_ && p.y_ == y_);
        }

    bool    operator!=(const CPoint& p) const
        {
            return (p.x_ != x_ || p.y_ != y_);
        }

    CPoint  operator+(const CPoint& p) const
        {
            return CPoint(x_ + p.x_, y_ + p.y_);
        }
    CPoint  operator-(const CPoint& p) const
        {
            return CPoint(x_ - p.x_, y_ - p.y_);
        }

    CPoint  operator+(const CRange& r) const
        {
            return CPoint(x_ + r.w_, y_ + r.h_);
        }
    CPoint  operator-(const CRange& r) const
        {
            return CPoint(x_ - r.w_, y_ - r.h_);
        }
    void    dump() const;

public:
    CType   x_;
    CType   y_;
};

/*! \brief
 */
class FORMATTER_EXPIMP CRect {
public:
    FORMATTER_OALLOC(CRect);

    CRect()
        : origin_(0, 0),
          extent_(0, 0) {}
    CRect(CPoint origin, CRange extent)
        : origin_(origin),
          extent_(extent) {}

    CRect(CPoint topLeft, CPoint bottRight)
        : origin_(topLeft),
          extent_(CRange(bottRight.x_ - topLeft.x_,
                         bottRight.y_ - topLeft.y_)) {}

    CPoint  bottomRight() const
        {
            return origin_ + CPoint(extent_.w_, extent_.h_);
        }
    bool    contains(const CPoint& p) const
        {
            CPoint bott_right(bottomRight());
            return ((origin_.x_ <= p.x_) && (origin_.y_ <= p.y_) &&
                    (bott_right.x_ >= p.x_) && (bott_right.y_ >= p.y_));
        }
    bool    operator==(const CRect& r) const
        {
            return (r.origin_ == origin_ && r.extent_ == extent_);
        }
    void    dump() const;

public:
    CPoint  origin_;
    CRange  extent_;
};

/*! \brief
 */
template <class T>
class OValue {
public:
    FORMATTER_OALLOC(OValue);

    OValue()
        : top_(),
          bottom_(),
          left_(),
          right_() {};
    OValue(const T& all)
        : top_(all),
          bottom_(all),
          left_(all),
          right_(all) {};
    OValue(const T& t, const T& b, const T& l, const T& r)
        : top_(t),
          bottom_(b),
          left_(l),
          right_(r) {};

    bool    operator==(const OValue& v) const {
        return (v.top_ == top_ && v.bottom_ == bottom_ &&
                v.left_ == left_ && v.right_ == right_);
    }

public:
    T   top_;
    T   bottom_;
    T   left_;
    T   right_;
};

/*! \brief
 */
class FORMATTER_EXPIMP ORect : public OValue<CType> {
public:
    FORMATTER_OALLOC(ORect);

    ORect()
        : OValue<CType>() {};
    ORect(const ORect& o)
        : OValue<CType>(o.top_, o.bottom_, o.left_, o.right_) {};
    ORect(CType top, CType bottom, CType left, CType right)
        : OValue<CType>(top, bottom, left, right) {};
    void dump() const;
};

/*! \brief
 */
class FORMATTER_EXPIMP Rgb {
public:
    FORMATTER_OALLOC(Rgb);

    Rgb(uint r_val = 0, uint g_val = 0, uint b_val = 0,
        bool transparent = false)
        : r_(r_val),
          g_(g_val),
          b_(b_val),
          isTransparent_(transparent) {};

    bool    operator==(const Rgb& c) const
    {
        return (c.r_ == r_ && c.g_ == g_ && c.b_ == b_);
    }

    void    dump() const;

public:
    uint    r_;
    uint    g_;
    uint    b_;
    bool    isTransparent_;
};

    COMMON_EXPIMP std::ostream& operator<<(std::ostream& os, const Rgb& color);

}

#endif // FORMATTER_TYPES_H
