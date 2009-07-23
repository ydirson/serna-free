// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class DocTemplateHolder : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/DocTemplateHolder.h"
using namespace SernaApi;
%End

public:
    static DocTemplateHolder instance();

    bool            getDocProperties(const SString&, PropertyNode&);
    PropertyNode    getDocTemplates();
    bool            findMatchedTemplates(PropertyNode&, const PropertyNode&,
                                         const SString&) const;
};
