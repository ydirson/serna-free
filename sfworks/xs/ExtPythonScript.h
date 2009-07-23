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

#ifndef EXT_PYTHON_SCRIPT_H_
#define EXT_PYTHON_SCRIPT_H_

#include "xs/xs_defs.h"

#ifdef USE_PYTHON

#include "common/OwnerPtr.h"
#include "common/StringDecl.h"

class Schema;

XS_NAMESPACE_BEGIN
using COMMON_NS::String;

class DvOrigin;
class Piece;

class ExtValidationCallback {
public:
    virtual bool  validate(Schema*,
                           const DvOrigin&,
                           const Piece*,
                           const String&,
                           String&) const
        { return true; }

    virtual bool  validateComplex(Schema*,
                                  const DvOrigin&,
                                  const Piece*,
                                  const String&,
                                  String&) const
        { return true; }
};

class XS_EXPIMP ExtPythonScript {
public:
    /*! Parse python Script and put it into dict.
     */
    virtual void  parsePyScript(Schema* schema, String& script) = 0;
    virtual bool  pyValidate(Schema* schema,
                             const DvOrigin& o,
                             const Piece* pc,
                             const String& funcName,
                             String& result) const = 0;

    virtual bool  pyValidateComplex(Schema* schema,
                                    const DvOrigin& o,
                                    const Piece* pc,
                                    const String& funcName) const = 0;

    void          setExtCallback(ExtValidationCallback* method);

    static ExtPythonScript* make();

    ExtPythonScript();
    virtual ~ExtPythonScript();
protected:
    COMMON_NS::OwnerPtr<ExtValidationCallback> method_;
};

XS_NAMESPACE_END

#endif // USE_PYTHON

#endif //  EXT_PYTHON_SCRIPT_H_
