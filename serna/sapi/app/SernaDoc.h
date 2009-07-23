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
#ifndef SAPI_SERNA_DOC_H_
#define SAPI_SERNA_DOC_H_

#include "sapi/app/UiAction.h"
#include "sapi/app/UiDocument.h"
#include "sapi/app/MimeHandler.h"
#include "sapi/common/SString.h"

namespace SernaApi {

class PropertyNode;
class StructEditor;
class DocumentPlugin;
class MessageView;
class Grove;
    
/// Document components.
/*! Provides access to the StructEditor and the DSI tree 
 *  of a document opened in Serna.
 */
class SAPI_EXPIMP SernaDoc : public UiDocument {
public:
    SernaDoc(SernaApiBase* = 0, DocumentPlugin* = 0);
    virtual ~SernaDoc();

    enum MessageBoxSeverity {
        MB_INFO, MB_WARNING, MB_CRITICAL, CHECKED_WARNING
    };

    /// Shows message window with \i caption,
    ///  \i message and \i buttons defined.
    /// The \i severity is an enumerated value.In python use as
    /// SernaDoc class member, e.g:  \a self.MB_INFO .
    int showMessageBox(MessageBoxSeverity severity,
                       const SString& caption,
                       const SString& message,
                       const SString& button0,
                       const SString& button1 = SString(),
                       const SString& button2 = SString()) const;

    /// Obtain Document Source Information property node. DSI is a property
    /// tree which contain various information about opened document 
    /// (current document path, stylesheet paths, settings, etc).
    PropertyNode    getDsi() const;

    /// Returns reference to the StructEditor instance.
    StructEditor    structEditor() const;

    /// Returns reference to the Serna messages window.
    MessageView     messageView() const;

    /// Builds UI item from an XML representation. \a uiTree is a property
    /// tree which represents UI item(s), and and \a childName is an optional
    /// name of the (child) UI item which can be specified if we want to build
    /// only particular subtree. 
    UiItem          buildUiItem(const PropertyNode& uiTree,
                                const SString& childName = SString()) const;

    void            showHelp(const SString& ref, 
                             const SString& adp = SString()) const;

    /// Returns currently active document (active tab in the active window)
    static SernaDoc activeDocument();                             
    
    /// Makes this document active (active tab in the active window)
    void            setActive();

    /// Returns MIME callback registry for current document
    MimeHandler     mimeHandler() const;

    /// Build new grove as specified in document template.
    Grove           groveFromTemplate(const PropertyNode& docTemplate,
                                      const SString& url,
                                      const SString& skel = SString()) const;
    
    /////////////////////////////////////////////////////////////

    virtual void        setRep(SernaApiBase*);
    
    /// Returns associated DocumentPlugin instance
    DocumentPlugin*     plugin() const;

    SernaDoc(const SernaDoc&);

    SernaDoc& operator=(const SernaDoc&);
private:
    DocumentPlugin* dp_;
};

} // namespace SernaApi

#endif // SAPI_SERNA_DOC_H_
