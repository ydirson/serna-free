// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveEditor : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/GroveEditor.h"
using namespace SernaApi;
%End

public:
    Command     insertText(const GrovePos&, const SString&);
    Command     removeText(const GrovePos&, unsigned int);
    Command     replaceText(const GrovePos&, unsigned int, const SString&);
    Command     insertElement(const GrovePos&, const SString&,
                              const PropertyNode& = PropertyNode());
    Command     removeNode(const GroveNode&);
    Command     splitElement(const GrovePos&);
    Command     joinElements(const GroveElement&);
    Command     renameElement(const GrovePos&, const SString&);
    Command     setAttribute(const GroveAttr&, const SString&);
    Command     addAttribute(const GroveElement&,
                             const PropertyNode& = PropertyNode());
    Command     removeAttribute(const GroveAttr&);
    Command     renameAttribute(const GroveAttr&,
                                const PropertyNode& = PropertyNode());
    Command     mapXmlNs(const GroveElement&, const SString&, const SString&);
    Command     insertEntity(const GrovePos&, const SString&);
    Command     cut(const GrovePos&, const GrovePos&);
    void        copy(const GrovePos&, const GrovePos&,
                     const GroveDocumentFragment&);
    Command     paste(const GroveDocumentFragment&, const GrovePos&);
    Command     tagRegion(const GrovePos&, const GrovePos&,
                          const SString&,const PropertyNode& = PropertyNode());
    Command     untag(const GrovePos&);
    Command     insertPi(const GrovePos&, const SString&, const SString&);
    Command     changePi(const GrovePos&, const SString&, const SString&);
    Command     insertComment(const GrovePos&, const SString&);
    Command     changeComment(const GrovePos&, const SString&);
    Command     convertToRedline(const GrovePos&, const GrovePos&,
                                 unsigned int, const SString&);
    Command     convertFromRedline(const GrovePos&);
    Command     changeRedline(const GrovePos&, unsigned int, const SString&);
    Command     insertRedline(const GrovePos&, unsigned int, const SString&);
    Command     insertXinclude(const GrovePos&,
                               const PropertyNode&,
                               const GroveStripInfo& = GroveStripInfo(),
                               bool = false);
    Command     declareEntity(const GrovePos&, const PropertyNode&);
    
    SString     errorMessage() const;
};


