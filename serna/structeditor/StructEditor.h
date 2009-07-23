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
#ifndef STRUCTEDITOR_H_
#define STRUCTEDITOR_H_

#include "structeditor/se_defs.h"
#include "docview/MessageViewHolder.h"
#include "common/PropertyTree.h"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/Exception.h"
#include "common/i18n_translate.h"

#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "utils/SernaUiItems.h"
#include "grove/Decls.h"
#include <qpointer.h>

class SelectionHistory;
class EditableView;
class EditPolicyImpl;
class Schema;
class SernaDoc;
class ColorSchemeImpl;
class ViewParam;
class LiveNodeLocator;
class StructDocumentActions;
class StructDocumentActionGroups;
class PositionWatcher;
class SchemaRequiredAttrs;
class DocUriMapper;
class HelpHandle;
class ElementList;
class AutoSaver;
class RecentElementsCache;
class StructEditor;
class SernaValidatorProvider;

namespace Common {
    class CommandExecutor;
    class Command;
}
namespace GroveEditor {
    class Editor;
    class GrovePos;
    class GroveSelection;
}
namespace GroveLib {
    class SectionSyncher;
    class XmlValidatorProvider;
    class ElementMatcher;
    class GroveBuilder;
}
namespace Xslt {
    class Engine;
}
namespace Formatter {
    class Chain;
    class AreaPos;
    class TreePos;
    class TreeSelection;
    class ChainSelection;
}

class STRUCTEDITOR_EXPIMP SourceDocument : public Common::CommandEventContext,
                                           public MessageTreeHolder {
public:
    DECLARE_I18N_TRANSLATE_FUNCTIONS(SourceDocument)

    SourceDocument(SernaDoc* doc);
    virtual ~SourceDocument();

    bool                createGroveFromTemplate(const Common::PropertyNode* dt,
                                  const Common::String& path,
                                  const Common::String& = Common::String());
    SernaDoc*           sernaDoc() const { return doc_; }
    Common::PropertyNode* getDsi() const;
    DocUriMapper*       uriMapper() const;
    GroveLib::Grove*    grove() const { return grove_.pointer(); }
    Schema*             schema() const { return schema_.pointer(); }
    const HelpHandle*   helpHandle() const { return helpHandle_.pointer(); }
    int                 isValidationOn() const { return isValidationOn_; }
    //
    SchemaRequiredAttrs* requiredAttrsProvider() const 
    {
        return requiredAttrsProvider_.pointer();
    }
    GroveLib::XmlValidatorProvider* validatorProvider() const;

    Common::MessageStream& messageStream() { return messageStream_; }

    static GroveLib::GroveBuilder* 
        makeGroveBuilder(const Common::PropertyNode* dsi);

protected:
    SourceDocument(const SourceDocument&);
    SourceDocument& operator=(const SourceDocument&);

    void    createGrove(Common::PropertyNode* dsi);
    bool    initGrove(Common::PropertyNode* dsi);
    void    firstValidate();
    void    processEntities();
    bool    makeRootElement(const Common::PropertyNode* dsi);
    bool    useSchema(Common::PropertyNode* dsi);
    void    setCatalogs(Common::PropertyNode* dsi);
    void    initElementHelp(Common::PropertyNode* dsi);

    SernaDoc* doc_;
    Common::MessageStream           messageStream_;
    Common::RefCntPtr<Schema>       schema_;
    Common::RefCntPtr<SernaValidatorProvider> validatorProvider_;
    Common::OwnerPtr<SchemaRequiredAttrs> requiredAttrsProvider_;
    Common::RefCntPtr<HelpHandle>   helpHandle_;
    GroveLib::GrovePtr              grove_;
    int                             isValidationOn_;
};
    
class STRUCTEDITOR_EXPIMP StructEditor : public Sui::Item,
                                         public SourceDocument {
public:
    DECLARE_I18N_TRANSLATE_FUNCTIONS(StructEditor)

    enum CursorAdjustment {
        ADJUST_FORWARD,
        ADJUST_BACKWARD,
        DO_NOT_ADJUST,
        ADJUST_AUTO
    };
    enum ChangeType {
        VIEW_FORMAT     = 0x01,
        CONTEXT_HINTS   = 0x02,
        TAG_COLORS      = 0x04
    };
    enum OpClass {
        STRUCT_OP       = 0x01, //! Structured operation
        TEXT_OP         = 0x02, //! Text-only operation
        SILENT_OP       = 0x04, //! Be silent
        CFE_OP          = 0x10, //! Convert-from-entity operation
        ANY_OP          = (STRUCT_OP|TEXT_OP),
        ALLOW_RDONLY    = 0x40, //! allow readonly position
        PARENT_OP       = 0x80,  //! modifies pos.node()
        NOROOT_OP       = 0x100 //! operation cannot be performed on root elem
    };
    enum PositionStatus {
        POS_OK = 0, POS_FAIL = 1, POS_RDONLY = 2
    };
    typedef Common::RefCntPtr<Common::Command>  CommandPtr;

    StructEditor(Sui::Action* action, SernaDoc* doc);
    virtual ~StructEditor();

    bool                newDocument(Common::PropertyNode* dsi);
    bool                openDocument(Common::PropertyNode* dsi);
    bool                saveDocument(Common::PropertyNode* dsi);
    void                saveContextToDsi();

    ///////////////////////////////////////////////////////////////////////
    /// Accessors ops
    
    EditableView&       editableView() const;
    EditPolicyImpl*     editPolicy() const { return editPolicy_.pointer(); }
    GroveEditor::Editor* groveEditor() {return groveEditor_.pointer();}
    GroveLib::Grove*    fot() const { return fot_.pointer(); }
    Xslt::Engine*       xsltEngine() const { return xsltEngine_.pointer(); }
    const GroveLib::StripInfo*  stripInfo() const;
    Common::CommandExecutor* executor() const {return cmdExecutor_.pointer();}

    GroveLib::ElementMatcher& elementMatcher();
    GroveLib::ElementMatcher& foldListMatcher();
    RecentElementsCache& recentElementsCache();

    /// Accessors to UI
    StructDocumentActions&      uiActions();
    void                        enableActions();
    StructDocumentActionGroups& actionGroups();

    ///////////////////////////////////////////////////////////////////////
    /// Cursor ops
    
    bool                setCursor(const Formatter::AreaPos& pos,
                                  bool isTop = false);

    bool                setCursor(const GroveEditor::GrovePos& srcPos,
                                  const Formatter::AreaPos& pos,
                                  bool isTop = false);

    GroveEditor::GrovePos   toResultPos(const GroveEditor::GrovePos& srcPos,
                                        const GroveLib::Node* foHint,
                                        bool diffuse = true) const;
    Formatter::AreaPos      toAreaPos(const GroveEditor::GrovePos& resultPos,
                                      CursorAdjustment adj= ADJUST_AUTO) const;

    bool                getCheckedPos(GroveEditor::GrovePos& pos,
                                      int opClass = ANY_OP) const;

    void                showContextInfo();
    void                showCursorInfo();

    ///////////////////////////////////////////////////////////////////////
    /// Command execution ops

    PositionStatus      isEditableEntity(const GroveEditor::GrovePos&,
                                         int opClass = ANY_OP) const;
    bool                executeAndUpdate(const CommandPtr& command);
    bool                updateView(Common::Command* command,
                                   const Formatter::TreePos& old_cursor,
                                   const LiveNodeLocator& foHint,
                                   int undoDepth,
                                   bool dontFormat);
    void                doUntil(int depth, bool dontFormat = false);
    void                setNotModified();
    bool                isModified() const;
    ElementList&        elementList() const { return *elementList_; }

    ///////////////////////////////////////////////////////////////////////
    /// Selection ops
    SelectionHistory&   selectionHistory() { return *selectionHistory_; }
    PositionStatus      getSelection(GroveEditor::GrovePos& from,
                                     GroveEditor::GrovePos& to,
                                     int opClass = ANY_OP);
    void                setSelection(const Formatter::ChainSelection& chain,
                                     const GroveEditor::GroveSelection& src,
                                     bool setCursor = true, 
                                     bool setCursorToStart = false,
                                     bool clearSelectionHistory = true);
    void                removeSelection();
    void                extendSelectionTo(const GroveEditor::GrovePos& srcPos,
                                          const Formatter::AreaPos& areaPos);

    ///////////////////////////////////////////////////////////////////////


    void            setValidationMode();

    void            setLastPosNode(GroveLib::Node* n) { lastSetPosNode_ = n; }
    GroveLib::Node* lastSetPosNode() const { return lastSetPosNode_; }

    ///////////////////////////////////////////////////////////////////////
    //! Reimplemented from UiItem

    virtual String      itemClass() const { return Sui::STRUCT_EDITOR; }
    virtual String      widgetClass() const { return Sui::TERMINAL_WIDGET; }
    virtual void        grabFocus() const;
    virtual QWidget*    widget(const Sui::Item* child = 0) const;
    virtual void        showContextMenu(const QPoint& pos);

    ////////////////////////////////////////////////////////////////////////

    /// Event factories (for notifications)
    Common::DynamicEventFactory&    elementContextChange();
    Common::DynamicEventFactory&    viewUpdate();
    Common::DynamicEventFactory&    selectionChange();
    Common::DynamicEventFactory&    historyChange();
    Common::DynamicEventFactory&    doubleClick();
    Common::DynamicEventFactory&    tripleClick();
    Common::DynamicEventFactory&    maybeTooltip();

public:  // Used by command-events only
    void                notifyPositionChange();
    void                notifySelectionChange(
                            const Formatter::ChainSelection& chainSel, 
                            const GroveEditor::GroveSelection& srcSel);
    void                notifyUndoStateChange();

    void                format();

    const GroveEditor::GrovePos&    editViewSrcPos() const;
    const GroveEditor::GrovePos&    editViewFoPos() const;

    virtual bool        doAttach();
    bool                update(bool dontFormat = false);

    void                updateView();
    void                notifyChanged(int type) { changeType_ |= type; }
    void                showCmdExecutionError() const;
    
    Common::String      generateId(const Common::String& fmt = 
                                   Common::String()) const;

    void                postInit();

private:
    virtual bool        executeEvent(COMMON_NS::CommandEventBase* event,
                                     COMMON_NS::EventData* ed);

    void                init();

    void                showPageInfo();

    void                processUndeclaredEntities();
    void                setCursorFromLineInfo();
    void                setCursorFromTreeloc();
    void                enableActions(const GroveEditor::GrovePos& pos);
    void                initViewParam();
    void                abortSession(const COMMON_NS::String&);
    
private:
    StructEditor(const StructEditor&);
    StructEditor& operator=(const StructEditor&);

    typedef Common::OwnerPtr<Common::DynamicEventFactory>
            DynamicEventFactoryOwner;

    GroveLib::GrovePtr                              fot_;

    COMMON_NS::OwnerPtr<GroveEditor::Editor>        groveEditor_;
    COMMON_NS::OwnerPtr<GroveLib::SectionSyncher>   sectionSyncher_;

    COMMON_NS::OwnerPtr<Xslt::Engine>               xsltEngine_;
    COMMON_NS::OwnerPtr<EditPolicyImpl>             editPolicy_;
    COMMON_NS::OwnerPtr<ViewParam>                  viewParam_;
    COMMON_NS::OwnerPtr<ColorSchemeImpl>            colorSchemeImpl_;
    EditableView*                                   editableView_;

    COMMON_NS::OwnerPtr<SelectionHistory>           selectionHistory_;
    COMMON_NS::OwnerPtr<Common::CommandExecutor>    cmdExecutor_;
    COMMON_NS::OwnerPtr<PositionWatcher>            positionWatcher_;

    //! Event factories
    DynamicEventFactoryOwner                        elemContextChangeFactory_;
    DynamicEventFactoryOwner                        viewUpdateFactory_;
    DynamicEventFactoryOwner                        selectionChangeFactory_;
    DynamicEventFactoryOwner                        doubleClickFactory_;
    DynamicEventFactoryOwner                        tripleClickFactory_;
    DynamicEventFactoryOwner                        maybeTooltipFactory_;

    GroveLib::Node*                                 lastSetPosNode_;
    DynamicEventFactoryOwner                        historyChangeFactory_;
    Common::OwnerPtr<ElementList>                   elementList_;
    Common::OwnerPtr<GroveLib::ElementMatcher>      elementMatcher_;
    Common::OwnerPtr<GroveLib::ElementMatcher>      foldListMatcher_;
    Common::OwnerPtr<RecentElementsCache>           recentElementsCache_;
    Common::OwnerPtr<AutoSaver>                     autoSaver_;

    bool                                            forceModified_;
    bool                                            isInitialized_;
    int                                             changeType_;
};

/////////////////////////////////////////////////////////////////////

class STRUCTEDITOR_EXPIMP StructEditorException : public COMMON_NS::Exception {
public:
    StructEditorException(const COMMON_NS::String& s)
        : COMMON_NS::Exception(s) {}
    StructEditorException(const char *s) : COMMON_NS::Exception(s) {}
};

#endif
