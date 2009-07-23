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
 *  $RCSfile: FpMake.cxx,v $
 *
 ***********************************************************************/

#include "xpath/impl3/FpMake.h"
#include <math.h>

#ifdef _MSC_VER
# include <float.h>
#endif

#ifdef __FreeBSD__
# include <floatingpoint.h>
#endif

#if !(defined(__GNUC__) && (__GNUC__ < 3))
# define CXX_HAS_STD_NUMERIC_LIMITS
# include <limits>
#endif

namespace {
#ifndef CXX_HAS_STD_NUMERIC_LIMITS
class FpGuard {
public:
    FpGuard()
        {
#if !defined(CXX_HAS_STD_NUMERIC_LIMITS) && !defined(linux)
            fpsetmask(0);
#endif  //linux os
        }
    ~FpGuard()
        {
#if !defined(CXX_HAS_STD_NUMERIC_LIMITS) && !defined(linux)
            fpresetsticky(FP_X_INV & FP_X_DZ);
            fpsetmask(FP_X_INV & FP_X_DZ);
#endif  //linux os
        }
};
/*! FpMake class makes from floating point exceptions.
  Before exception occurs it saves mask and then clear floating point mask.
  After exception, put saved mask back.
 */

double makeNaN()
{
    FpGuard guard;
    double zero = 0;
    return zero / zero;
}

double makeInf()
{
    FpGuard guard;
    double zero = 0;
    double positive = 1;
    return positive / zero;
}

double makeNegInf()
{
    FpGuard guard;
    double zero = 0;
    double negative = -1;
    return negative / zero;
}

double Nan = makeNaN();
double inf = makeInf();
double negInf = makeNegInf();
#endif
};

namespace Xpath
{
bool isNaN(const double& number)
{
#ifdef _MSC_VER
    return _isnan(number);
#else
    return isnan(number);
#endif
}

double getNaN()
{
#ifdef CXX_HAS_STD_NUMERIC_LIMITS
        static double Nan = std::numeric_limits<double>::quiet_NaN();
#endif
    return Nan;
}

double getInf()
{
#ifdef CXX_HAS_STD_NUMERIC_LIMITS
        static double inf = std::numeric_limits<double>::infinity();
#endif
    return inf;
}

double getNegInf()
{
#ifdef CXX_HAS_STD_NUMERIC_LIMITS
        static double negInf = -std::numeric_limits<double>::infinity();
#endif
    return negInf;
}

long truncate(double num)
{
    if (0 < num)
        return long(floor(num));
    return long(ceil(num));
}

}   //namespace XPath
