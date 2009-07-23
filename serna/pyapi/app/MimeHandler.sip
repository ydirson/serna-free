// (c) 2008 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class MimeCallback : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/MimeHandler.h"
#include <QDropEvent>
using namespace SernaApi;
%End
public:
     enum CallbackClass {
        CREATE_CONTENT = 0, CREATE_FRAGMENT, TRANSFORM,
        WRAP_CONTENT, PASTE_CONTENT, VALIDATE_CONTENT,
        DROP_CONTENT, DROP_COPY_CONTENT, DROP_MOVE_CONTENT, 
        DROP_LINK_CONTENT, MAX_CALLBACK_ID
    };
    virtual ~MimeCallback();

    double                  priority() const;
    void                    setPriority(double p);
    
    virtual SString         mimeType() const;
    virtual CallbackClass   callbackClass() const = 0;
    virtual bool            execute(MimeHandler&, QDropEvent*) = 0;           
};

class MimeHandler : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/app/MimeHandler.h"
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
using namespace SernaApi;
%End
public:
    void        registerCallback(MimeCallback&, double = 0);
    bool        callback(MimeCallback::CallbackClass, QDropEvent* = 0);

    QDrag*      makeDrag(QWidget*) const;
    void        dragEnter(QDragEnterEvent*);

    QMimeData*  mimeData() const;
};

class SernaDocFragment : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/app/MimeHandler.h"
using namespace SernaApi;
%End
public:
    void                    setFragment(const GroveDocumentFragment&);
    GroveDocumentFragment   fragment() const;
    
    static SString          fragMimeType(const PropertyNode&);
    static SernaDocFragment newFragment(MimeHandler&, const SString&);
    static SernaDocFragment getFragment(const MimeHandler&, const SString&);
    static SernaDocFragment getSeFragment(const SernaDoc&);
    static SernaDocFragment newSeFragment(const SernaDoc&);
};
