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
// Copyright (c) 2008 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef SAPI_MIME_HANDLER_H_
#define SAPI_MIME_HANDLER_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/WrappedObject.h"
#include "sapi/common/SString.h"
#include "sapi/common/xtn_wrap.h"

class QDropEvent;
class QDragEnterEvent;
class QWidget;
class QMimeData;
class QDrag;

namespace SernaApi {

class GroveDocumentFragment;
class MimeHandler;
class PropertyNode;
class SernaDoc;

class SAPI_EXPIMP MimeCallback : public RefCountedWrappedObject {
public:
    enum CallbackClass {
        CREATE_CONTENT = 0, CREATE_FRAGMENT, TRANSFORM,
        WRAP_CONTENT, PASTE_CONTENT, VALIDATE_CONTENT,
        DROP_CONTENT, DROP_COPY_CONTENT, DROP_MOVE_CONTENT, 
        DROP_LINK_CONTENT, MAX_CALLBACK_ID
    };
    MimeCallback(SernaApiBase*);
    MimeCallback();
    virtual ~MimeCallback();

    double                  priority() const;
    void                    setPriority(double p);
    
    virtual SString         mimeType() const;
    virtual CallbackClass   callbackClass() const = 0;
    virtual bool            execute(MimeHandler&, QDropEvent*) = 0;
};

class SAPI_EXPIMP MimeHandler : public SimpleWrappedObject {
public:
    MimeHandler(SernaApiBase* = 0);
    ~MimeHandler();

    void        registerCallback(MimeCallback&, double prio = 0);
    bool        callback(MimeCallback::CallbackClass, QDropEvent* = 0);

    QDrag*      makeDrag(QWidget* dragSource) const;
    void        dragEnter(QDragEnterEvent*);

    QMimeData*  mimeData() const;
};

class SAPI_EXPIMP SernaDocFragment : public SimpleWrappedObject {
public:
    SernaDocFragment(SernaApiBase* = 0);
    ~SernaDocFragment();

    void                    setFragment(const GroveDocumentFragment&);
    GroveDocumentFragment   fragment() const;

    /// Generate SernaDocFragment MIME type from the document template
    static SString          fragMimeType(const PropertyNode&);

    /// Create new SernaDocFragment with given MIME type, and add it to
    /// QMimeData instance
    static SernaDocFragment newFragment(MimeHandler&, 
                                        const SString& mimeType);
    
    /// Retrieve SernaDocFragment from QMimeData
    static SernaDocFragment getFragment(const MimeHandler&, 
                                        const SString& mimeType);
    
    /// Convenience function, equivalent to calling getFragment with
    /// document template information from provided SernaDoc
    static SernaDocFragment getSeFragment(const SernaDoc&);

    /// Convenience function, equivalent to calling newFragment with
    /// document template information from provided SernaDoc
    static SernaDocFragment newSeFragment(const SernaDoc&);
};

} // namespace

#endif // SAPI_MIME_HANDLER_H_
