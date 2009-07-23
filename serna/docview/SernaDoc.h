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
#ifndef SERNA_DOCUMENT_H_
#define SERNA_DOCUMENT_H_

#include "docview/dv_defs.h"
#include "ui/UiDocument.h"
#include "ui/UiAction.h"

#include "common/CommandEvent.h"

class QWidget;
class BuiltinUiActions;
class BuiltinActionGroups;
class EventTranslator;

namespace Common {
    class MessageStream;
    class Messenger;
    class PropertyNode;
}

namespace GroveEditor {
    class GrovePos;
}

class DocBuilder {
public:
    virtual ~DocBuilder() {};

    virtual void    buildActions(Sui::ActionDispatcher* dispatcher,
                                 Sui::ActionSet* actionSet) const = 0;
    virtual void    buildInterface(Common::PropertyNode* property) const = 0;
};

//////////////////////////////////////////////////////////////////////////////

class DOCVIEW_EXPIMP ActionExecutor {
public:
    ActionExecutor(Sui::Action* action)
        : eventTranslator_(0), 
          uiAction_(action) {}
    virtual ~ActionExecutor();

    Sui::Action*         uiAction() const { return uiAction_.pointer(); }

    virtual void        execute() = 0;

private:
    friend class EventTranslator;
    EventTranslator*    eventTranslator_;
    Sui::ActionPtr       uiAction_;
};

/* 
   SernaDoc is a base class for a instance-views (= tabs).
   It implements interface which is used by all views (progress and
   context info, etc.)
*/
class DOCVIEW_EXPIMP SernaDoc : public Sui::Document,
                                public COMMON_NS::CommandEventContext {
public:
    DECLARE_I18N_TRANSLATE_FUNCTIONS(SernaDoc)

    typedef COMMON_NS::PropertyNode     Ptn;

    enum Level { RECENT_DOCUMENT_LEVEL,
                 TEMPLATE_LEVEL,
                 ORIGINAL_TEMPLATE_LEVEL,
                 BUILTIN_LEVEL
    };

    SernaDoc(const DocBuilder* builder, Sui::Item* prevItem = 0);
    virtual ~SernaDoc();

    void                registerActionExecutor(ActionExecutor*);
    void                deregisterActionExecutor(const ActionExecutor*);

    virtual void        updateTooltip() {}

    BuiltinUiActions&      uiActions() const;
    BuiltinActionGroups&   actionGroups() const;

    void                showStageInfo(const String& = "") const;
    void                showPageInfo(const String&) const;
    void                showContextInfo(const String&) const;

    Common::DynamicEventFactory& stateChangeFactory();

    /// This function is called for post-initialization
    void                postInit();

    COMMON_NS::MessageStream* progressStream() const;

    enum MessageBoxSeverity {
        MB_INFO, MB_WARNING, MB_CRITICAL, CHECKED_WARNING
    };
    int                 showMessageBox(MessageBoxSeverity,
                                       const String& caption,
                                       const String& message,
                                       const String& button0,
                                       const String& button1 = String(),
                                       const String& button2 =
                                             String()) const;

    bool                checkDsi(const Ptn* propertyNode) const;
    // Obtain pointer to DocSrcInfo property set
    virtual Ptn*        getDsi() const { return dsi_.pointer(); }
    void                setDsi(Ptn*);

    virtual Common::String  getLevelFile(Level level,
                                Common::String* comment = 0) const = 0;

    bool                saveDocument(Common::PropertyNode* dsi = 0);
    bool                closeDocument();
    virtual bool        canCloseDocument() const { return false; }

    virtual bool        isToMakeItem(const Common::PropertyNode*) const;

    void                buildInterface();
    void                buildInterface(Common::PropertyNode* iface, 
                                       bool& isUpdated);
    void                saveView(Common::String path = Common::String());
    virtual bool        restoreView(const Common::String& restoreFrom);
    
    virtual Common::CommandEventPtr makeUiEventExecutor(ActionExecutor*) = 0;

    static SernaDoc*    activeDocument(); 


protected:
    virtual const char* selfTypeId() const { return typeid(SernaDoc).name(); }

    virtual Common::CommandEventPtr makeSaveEvent(PropertyNode* = 0) const = 0;

protected:
    Sui::ActionPtr                                   contextAction_;

private:
    Common::OwnerPtr<const DocBuilder>              docBuilder_;
    COMMON_NS::RefCntPtr<COMMON_NS::Messenger>      progressMessenger_;
    COMMON_NS::OwnerPtr<COMMON_NS::MessageStream>   progressStream_;
    Common::DynamicEventFactory                     stateChangeFactory_;
    Common::PropertyNodePtr                         dsi_;
};

////////////////////////////////////////////////////////////////////////


#endif // SERNA_DOCUMENT_H_
