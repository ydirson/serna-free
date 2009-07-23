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

#ifndef XPATH_VALUE_HOLDER_H
#define XPATH_VALUE_HOLDER_H

#include "xpath/xpath_defs.h"
#include "xpath/Value.h"
#include "common/SubscriberPtr.h"
#include "common/SernaApiRefCounted.h"

namespace Xpath {

class ExprInst;

/*! \brief ValueHolder keeps the xpath value and notifies on it`s changes
 */

class ValueHolder;
typedef COMMON_NS::SubscriberPtr<ValueHolder> ValueHolderPtr;

class XPATH_EXPIMP ValueHolder : public COMMON_NS::SernaApiRefCounted,
                                 public COMMON_NS::SubscriberPtrPublisher {
public:
    XPATH_OALLOC(ValueHolder);
    ValueHolder() {};
    virtual ~ValueHolder() {};
    //! Returns xpath value
    virtual ConstValuePtr   value() const = 0;
    virtual ExprInst*       exprInst() const { return 0; }
};

} // namespace Xpath

#endif // XPATH_VALUE_HOLDER_H
