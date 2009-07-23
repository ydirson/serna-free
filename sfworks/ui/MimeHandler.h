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
#ifndef UI_MIME_HANDLER_H_
#define UI_MIME_HANDLER_H_

#include "ui/ui_defs.h"
#include "common/SernaApiBase.h"
#include "common/Vector.h"
#include "common/String.h"
#include "common/XList.h"

class QDrag;
class QMimeData;
class QDropEvent;
class QWidget;
class QDragEnterEvent;

namespace Sui {

class MimeHandler;

class UI_EXPIMP MimeCallback : public Common::XListItem<MimeCallback> {
public:
    enum CallbackClass {
        CREATE_CONTENT = 0, CREATE_FRAGMENT, TRANSFORM,
        WRAP_CONTENT, PASTE_CONTENT, VALIDATE_CONTENT,
        DROP_CONTENT, DROP_COPY_CONTENT, DROP_MOVE_CONTENT, DROP_LINK_CONTENT,
        MAX_CALLBACK_ID
    };
    virtual Common::String  mimeType() const { return Common::String(); }
    virtual CallbackClass   callbackClass() const = 0;
    double                  priority() const { return priority_; }
    void                    setPriority(double p) { priority_ = p; }

    virtual bool            execute(MimeHandler&, QDropEvent*) = 0;

    virtual ~MimeCallback() {}

private:
    double  priority_;
};

///////////////////////////////////////////////////////////////

class UI_EXPIMP CustomMimetypeCallback : public MimeCallback {
public:
    CustomMimetypeCallback(const Common::String& mimeType)
        : mimeType_(mimeType) {}
    virtual Common::String  mimeType() const { return mimeType_; }
private:
    Common::String mimeType_;
};

///////////////////////////////////////////////////////////////

class UI_EXPIMP MimeHandler : public SernaApiBase {
public:
    typedef MimeCallback::CallbackClass CC;

    virtual void            registerCallback(MimeCallback*, 
                                             double priority = 0) = 0;
    virtual bool            callback(const CC, QDropEvent* = 0) = 0;

    // get callback with highest prio for the mimetype
    virtual MimeCallback*   getCallback(const CC,
                                        const Common::String&) const = 0;
    
    virtual void            dragEnter(QDragEnterEvent*) = 0;

    virtual void            addObject(SernaApiBase*, const Common::String&) = 0;
    virtual SernaApiBase*   getObject(const Common::String&, 
                                      bool usePrefix = false) const = 0;

    virtual QDrag*          makeDrag(QWidget* dragSource) const = 0;
    virtual QMimeData*      mimeData() const = 0;

    static Common::String   sernaObjectMimetype(const Common::String& name);

    static  MimeHandler*    make();
    virtual ~MimeHandler() {}
};

} // namespace Sui

/////////////////////////////////////////////////////////////////

#define MIMETYPE_CALLBACK_DECL(name, type, mtt, extra) \
class name : public MimeCallback { \
public: \
    virtual bool            execute(MimeHandler&, QDropEvent*); \
    virtual CallbackClass   callbackClass() const { return type; } \
    virtual Common::String  mimeType() const { return mtt; } \
    extra \
};

#define CUSTOM_MIMETYPE_CALLBACK_DECL(name, type, extra) \
class name : public CustomMimetypeCallback { \
public: \
    name(const Common::String& mt) : CustomMimetypeCallback(mt) {} \
    virtual bool            execute(MimeHandler&, QDropEvent*); \
    virtual CallbackClass   callbackClass() const { return type; } \
    extra \
};

#endif // UI_MIME_HANDLER_H_
