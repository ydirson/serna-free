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
#ifndef XSLT_EXTERNAL_FUNCTION_H
#define XSLT_EXTERNAL_FUNCTION_H

#include "xslt/xslt_defs.h"
#include "xpath/ExternalFunction.h"
#include "xpath/ExprContext.h"
#include "xpath/Value.h"
#include "common/String.h"
#include "common/SernaApiRefCounted.h"

namespace Xslt {

class EngineImpl;

class XSLT_EXPIMP ExternalFunction : public Common::SernaApiRefCounted,
                                     public Xpath::ExternalFunctionBase {
public:
    ExternalFunction(const Common::String& localName,
                     const Common::String& ns)
        : localName_(localName), xmlns_(ns) {}
        
    const Common::String& localName() const { return localName_; }
    const Common::String& xmlns() const { return xmlns_; }
    
    virtual ~ExternalFunction();
    
private:
    friend class EngineImpl;
    friend class FunctionRegistry;
    Common::String  localName_;
    Common::String  xmlns_;
    EngineImpl*     xsltEngine_;
};

} // namespace Xslt

#endif // XSLT_EXTERNAL_FUNCTION_H
