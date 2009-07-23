// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class XmlValidatorProvider : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/XmlValidator.h"
using namespace SernaApi;
%End

public:
    XmlValidatorProvider(const XmlValidatorProvider&);
    XmlValidator getValidator(const Grove&) const;
};

class XmlValidator : SimpleWrappedObject {
public:
    XmlValidator(const XmlValidator&);
    enum ValidationMode {
        OPEN_MODE, EDIT_MODE, PARTIAL, DONT_MAKE_ELEMS,    
        HIDE_PROGRESS, XINCLUDE, SILENT_MODE, DEFAULT_MODE
    };
    /// Validate content
    bool validate(const GroveElement&, int = DEFAULT_MODE) const;
};
