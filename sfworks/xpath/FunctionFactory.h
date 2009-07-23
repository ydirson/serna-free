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

#ifndef XPATH_FUNCTION_FACTORY_H
#define XPATH_FUNCTION_FACTORY_H

#include "xpath/xpath_defs.h"
#include "common/String.h"

namespace Xpath {

class FunctionExpr;
class FunctionArgExpr;

/*! An abstract interface to the FunctionFactory.
 */
class XPATH_EXPIMP FunctionFactory {
public:
    typedef FunctionExpr* (*FunctionMaker)
        (const Common::String& funcName, const FunctionArgExpr* alist);
    struct FunctionMakerInfo {
        const char*     name;
        const char*     uri;
        FunctionMaker   maker;
        uint            minArgs;
        uint            maxArgs;
    };
    virtual const FunctionMakerInfo*
        findFunction(const Common::String& name,
                     const Common::String& uri) const = 0;
    FunctionExpr*         makeFunction(const Common::String& funcName,
                                const FunctionArgExpr* alist,
                                const GroveLib::NodeWithNamespace*) const;

    enum OptimizationFlags { OPT_NONE = 0, OPT_NORMAL = 01, OPT_DITA = 02 };
    virtual int optimizationFlags() const { return OPT_NORMAL; }

protected:
    virtual ~FunctionFactory() {}
};

#define SERNA_XPATH_EXTENSIONS_URI "http://www.syntext.com/Extensions/XSLT-1.0" 

} // namespace Xpath

#endif // XPATH_FUNCTION_FACTORY_H
