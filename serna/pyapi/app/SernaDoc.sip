// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class SernaDoc : UiDocument {

%TypeHeaderCode
#include "sapi/app/SernaDoc.h"
#include "sapi/app/StructEditor.h"
using namespace SernaApi;
%End

public:
    enum MessageBoxSeverity {
        MB_INFO, MB_WARNING, MB_CRITICAL, CHECKED_WARNING
    };
    
    UiItem          buildUiItem(const PropertyNode&,
                                const SString& = SString()) const;
    
    int showMessageBox(MessageBoxSeverity,
                       const SString&,
                       const SString&,
                       const SString&,
                       const SString& = SString(),
                       const SString& = SString()) const;
    
    PropertyNode    getDsi() const;
    StructEditor    structEditor() const;
    MessageView     messageView() const;
    void            showHelp(const SString&, 
                             const SString& = SString()) const;
    static SernaDoc activeDocument();                             
    void            setActive();
    MimeHandler     mimeHandler() const;
    Grove           groveFromTemplate(const PropertyNode&, 
                                      const SString&,
                                      const SString& = SString()) const;

    SernaDoc(const SernaDoc&);
    bool operator==(const SernaDoc&) const;
    bool operator!=(const SernaDoc&) const;
};
