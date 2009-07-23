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
#ifndef PLAINEDITOR_H_
#define PLAINEDITOR_H_

#include "plaineditor/pe_defs.h"

#include "common/PropertyTree.h"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/Exception.h"

#include "ui/UiProps.h"
#include "ui/UiItem.h"

#include "utils/SernaUiItems.h"
#include <QPointer>

class PlainDocument;
class PlainDocumentActions;
class PlainDocumentActionGroups;
class MessageView;
class SernaDoc;

class PLAINEDITOR_EXPIMP PlainEditor : public COMMON_NS::CommandEventContext,
                                       public Sui::Item {
public:
    typedef Common::PropertyNode PropertyNode;

    virtual ~PlainEditor();

    PlainDocument*          plainDoc() const { return doc_; }
    PlainDocumentActions&   uiActions();

    virtual bool            find(const String& text, bool matchCase,
                                 bool reverse, bool start) = 0;

    virtual void            cut() = 0;
    virtual void            copy() = 0;
    virtual void            insert(const String& text) = 0;
    virtual void            paste(const String& text) = 0;
    virtual String          selectedText() const = 0;
    virtual void            setCursor(int line, int column) = 0;

    virtual void            undo() = 0;
    virtual void            redo() = 0;

    virtual bool            canCloseDocument() = 0;
    virtual bool            openDocument(const Common::String& file) = 0;
    virtual bool            openSingleFile(const Common::String& file) = 0;
    virtual bool            switchToEntity(const Common::String&) = 0;
    virtual bool            saveDocument(const Common::String&) = 0;

    virtual String          itemClass() const { return Sui::PLAIN_EDITOR; }
    virtual String          widgetClass() const { return Sui::TERMINAL_WIDGET; }

protected:
    PlainEditor(PlainDocument* doc, Common::PropertyNode* dsi);

private:
    virtual const char*     selfTypeId() const
    {
        return typeid(PlainEditor).name();
    }

protected:
    PlainDocument*          doc_;
};

#endif // PLAINEDITOR_H_
