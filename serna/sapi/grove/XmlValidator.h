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
#ifndef SAPI_XML_VALIDATOR_H_
#define SAPI_XML_VALIDATOR_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/WrappedObject.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/grove/GroveDecls.h"

namespace SernaApi {

class XmlValidator;

/// Provides XML validator instance for the specified grove. Usually
/// validator provider matches grove against document templates, checks
/// for xsi:noNamespaceSchemaLocation etc.
class SAPI_EXPIMP XmlValidatorProvider : public RefCountedWrappedObject {
public:
    XmlValidatorProvider(SernaApiBase* = 0);

    /// Returns XML validator instance
    XmlValidator getValidator(const Grove& grove) const;
};

class SAPI_EXPIMP XmlValidator : public RefCountedWrappedObject {
public:
    XmlValidator(SernaApiBase* = 0);
    
    enum ValidationMode {
        OPEN_MODE = 001,            ///! after-parse validation mode
        EDIT_MODE = 002,            ///! edit-in-progress validation mode
        PARTIAL   = 004,            ///! partial validate
        DONT_MAKE_ELEMS = 010,      ///! do not create elements in edit mode
        HIDE_PROGRESS = 020,        ///! do not write to progress stream
        XINCLUDE = 040,             ///! used when validating XIncluded section
        SILENT_MODE = 0100,         ///! suppress error messages
        DEFAULT_MODE = (OPEN_MODE|DONT_MAKE_ELEMS|HIDE_PROGRESS)
    };
    /// Validate content
    bool validate(const GroveElement& elem, int vFlags = DEFAULT_MODE) const;
};

} // namespace SernaApi

#endif // SAPI_XML_VALIDATOR_H_
