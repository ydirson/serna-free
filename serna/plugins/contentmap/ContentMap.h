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
#ifndef CONTENT_MAP_H_
#define CONTENT_MAP_H_

#include "ui/LiquidItem.h"
#include "ui/UiAction.h"

#include "docview/DocumentPlugin.h"

#include "utils/SernaMessages.h"
#include "utils/IdleHandler.h"

#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/String.h"


#include "grove/Decls.h"
#include "grove/Node.h"

#include "groveeditor/GrovePos.h"

#include <QPointer>

class QWidget;
class ProfileSelector;
class ContentMapWidget;

extern const char* CONTENT_MAP;

//////////////////////////////////////////////////////////////////////

namespace Csl 
{
    class Engine;
};

class StructEditor;

class ContentMap : public Sui::LiquidItem,
                   public Common::CommandEventContext,
                   public IdleHandler {
public:
    typedef Common::DynamicEventFactory EventFactory;

    ContentMap(PropertyNode* properties);
    virtual ~ContentMap();

    virtual String      itemClass() const { return CONTENT_MAP; }
    virtual String      widgetClass() const { return Sui::TERMINAL_WIDGET; }
    virtual void        grabFocus() const;

    void                setPendingPos(const GroveEditor::GrovePos& src);
    void                updateSelection();
    void                removeSelection();

    void                sendSrcPos(const GroveEditor::GrovePos& pos);
    void                getSelection(GroveEditor::GrovePos& from,
                                     GroveEditor::GrovePos& to) const;
    void                setSelection(const GroveEditor::GrovePos& from,
                                     const GroveEditor::GrovePos& to);
    void                cutSelection(const GroveEditor::GrovePos& from,
                                     const GroveEditor::GrovePos& to);
    void                extendSelection(const GroveEditor::GrovePos& to);
    
    StructEditor*       structEditor() const { return structEditor_; }
    Csl::Engine*        engine() const;
    void                update();
    ContentMapWidget*   contentMapWidget() const;
    Sui::Action*         focusPolicyAction() const {return focusPolicyAction_;}
    void                languageChanged();
    
protected:
    void                init();
    virtual bool        processQuanta();
    virtual QWidget*    makeWidget(QWidget* parent, Type type);
    void                showContextMenu(const QPoint& pos);

protected:
    StructEditor*                   structEditor_;
    Common::OwnerPtr<Csl::Engine>   engine_;    
    QPointer<ProfileSelector>    widget_;
    QPointer<ContentMapWidget>   contentMapWidget_;
    GroveEditor::GrovePos           pendingPos_;
    Sui::Action*                     focusPolicyAction_;
};

//////////////////////////////////////////////////////////////////////

class ContentMapPlugin : public DocumentPlugin {
public:
    ContentMapPlugin(SernaApiBase* doc, SernaApiBase* properties, char**);
};

#endif // CONTENT_MAP_H_
