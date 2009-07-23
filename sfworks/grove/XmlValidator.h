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
#ifndef GROVE_XML_VALIDATOR_H_
#define GROVE_XML_VALIDATOR_H_

#include "grove/grove_defs.h"
#include "common/String.h"
#include "common/SernaApiRefCounted.h"
#include "common/RefCntPtr.h"

namespace Common {
    class Messenger;
    class PropertyNode;
}

namespace GroveLib {

class Element;
class ValidationCommandsMaker;
class XmlValidator;
class Grove;

typedef Common::RefCntPtr<const XmlValidator> XmlValidatorPtr;

class GROVE_EXPIMP XmlValidatorProvider : public Common::SernaApiRefCounted {
public:
    /// Get the validator for the specified grove. This is necessary to
    /// process xsi:schemaLocation correctly.
    virtual XmlValidatorPtr getValidator(const Grove*,
                                         bool* found = 0) const = 0;
    virtual ~XmlValidatorProvider() {}
};

/*! A prototype generic validator interface 
 */
class GROVE_EXPIMP XmlValidator : public Common::SernaApiRefCounted {
public:
    enum ValidationMode {
        OPEN_MODE = 001,            ///! after-parse validation mode
        EDIT_MODE = 002,            ///! edit-in-progress validation mode
        PARTIAL   = 004,            ///! partial validate
        DONT_MAKE_ELEMS = 010,      ///! do not create elements in edit mode
        HIDE_PROGRESS = 020,        ///! do not write to progress stream
        XINCLUDE = 040,             ///! validating XIncluded section
        SILENT_MODE = 0100          ///! be silent about error messages
    };
    //! Validate content
    virtual bool validate(Element* elem, int vFlags) const = 0;
    
    //! Get the list of possible root elements
    virtual void getRootElementList(Common::PropertyNode*) const = 0;
    
    //! Get attribute specs for the root element with given name/uri
    virtual void getAttrSpecsForTopElement(const Common::String& localName,
        const Common::String& uri, Common::PropertyNode* specList,
        const GroveLib::Element* = 0) const = 0;

    //! Set/get command makers
    virtual void setCommandMaker(ValidationCommandsMaker*) = 0;
    virtual ValidationCommandsMaker* commandMaker() const = 0;

    //! Set/get messenger
    virtual void        setMessenger(Common::Messenger*) = 0;
    virtual Common::Messenger* getMessenger() const = 0;
    
    virtual ~XmlValidator() {}
};

} // namespace GroveLib

#endif // GROVE_XML_VALIDATOR_H_
