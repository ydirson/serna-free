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
#ifndef EDITABLE_VIEW_H
#define EDITABLE_VIEW_H

#include "editableview_defs.h"
#include "editableview/EditPolicy.h"
#include "editableview/EditableViewMessages.h"
#include "common/RefCntPtr.h"
#include "common/MessageUtils.h"

#include "formatter/types.h"
#include "formatter/Formatter.h"

#include "common/OwnerPtr.h"

class QWidget;
class QPoint;
class ToolTipEventData;

class EditableViewException : public Common::Exception {
public:
    EDITABLEVIEW_OALLOC(EditableViewException);
    Common::Message* message() { return message_.pointer(); }
    EditableViewException(EditableViewMessages::Messages m,
                          const Common::String& str)
        : Common::Exception(str)
    {
        message_ = new Common::UintIdMessage(
            m, EditableViewMessages::getFacility());
        message_->appendArgInstance
            (new Common::MessageArg<Common::String>(str));
    }
    ~EditableViewException() throw() {}
protected:
    Common::RefCntPtr<Common::Message> message_;
};

/*
 */
class ViewParam {
public:
    ViewParam(Formatter::CType indent, bool showTags, bool showPaginated,
              const Formatter::ColorScheme* colorScheme,
              bool showNbsp)
        : dpi_(96),
          indent_(indent),
          colorScheme_(colorScheme),
          showTags_(showTags),
          showPaginated_(showPaginated),
          showNbsp_(showNbsp) {}

    int                             dpi_;
    Formatter::CType                indent_;
    const Formatter::ColorScheme*   colorScheme_;
    bool                            showTags_;
    bool                            showPaginated_;
    bool                            showNbsp_;
};

/*
 */
class QPoint;

class EDITABLEVIEW_EXPIMP EditContext {
public:
    const Formatter::AreaPos&   areaPos() const { return areaPos_; }
    const Formatter::AreaPos&   persistent() const {return persistentAreaPos_;}
    const Formatter::TreePos&   treePos() const { return treePos_; }

    const GroveEditor::GrovePos& foPos() const { return foPos_; }
    const GroveEditor::GrovePos& srcPos() const { return srcPos_; }

    void                        set(const Formatter::AreaPos& areaPos,
                                    const GroveEditor::GrovePos& srcPos);
    void                        lockPersistentPos(bool isLocked) const {
        persistentPosLocked_ = isLocked; }

    //! Get left/right shifted cursor position
    Formatter::AreaPos          getShiftedCursor(int shift) const;
    //! Get start/end line cursor position
    Formatter::AreaPos          getWordCursor(bool left) const;
    //! Get start/end line cursor position
    Formatter::AreaPos          getLineCursor(bool start) const;

    static GroveEditor::GrovePos  getFoPos(const Formatter::AreaPos& areaPos);
    static GroveEditor::GrovePos  getSrcPos(const Formatter::AreaPos& areaPos,
                                            bool preferPrevNode = false);

private:
    Formatter::AreaPos          areaPos_;
    Formatter::AreaPos          persistentAreaPos_;
    mutable bool                persistentPosLocked_;

    Formatter::TreePos          treePos_;
    GroveEditor::GrovePos       foPos_;
    GroveEditor::GrovePos       srcPos_;
};

class EDITABLEVIEW_EXPIMP Selection {
public:
    Selection()
        : tree_(), src_() {}
    Selection(const Formatter::TreeSelection& tree,
              const GroveEditor::GroveSelection& src)
        : tree_(tree), src_(src) {}

    void    clear() {
        tree_.clear();
        src_.clear();
    }
    Formatter::TreeSelection    tree_;
    GroveEditor::GroveSelection src_;
};

/*! \brief EditableView is an abstract interface to formatted Xml document
*/
class EDITABLEVIEW_EXPIMP EditableView : public Formatter::FormattedView {
public:
    class SelectionOp {
    public:
        virtual void operator()(const Formatter::Area* area,
                                long from, long to) = 0;
        virtual ~SelectionOp() {}
    };
    enum FormattingMode {
        VISIBLE, //! Format all areas that are visible
        IDLE,    //! Format until user event comes
        PAGE,    //! Format necessary page to make PageDown successfully
        WHOLE    //! Format the whole document
    };

    typedef Common::OwnerPtr<Formatter::Formatter>  FormatterOwner;
    typedef Formatter::TreeSelection                TreeSelection;
    typedef Formatter::AreaViewFactory              Avf;
    typedef Formatter::ImageProvider                ImProvider;
    typedef Formatter::MediaInfo                    MInfo;
    typedef Formatter::InlineObjectFactory          InlineObjFactory;
    typedef ::Formatter::Formatter::FoModList       FoModList;

    virtual ~EditableView();

//! Formatting area tree
    //!
    bool                isModified() const { return formatter_->isModified(); }
    //!
    void                setNotModified() { formatter_->setNotModified(); }
    //!
    bool                isFormatted() const {return formatter_->isFinished();}
    //!
    bool                ensureFormatted(const GroveLib::Node* foNode);

    FoModList& textFoModList() const { return formatter_->textFoModList(); }
    //!
    virtual void        format(const ViewParam& viewParam);
    const Formatter::Area*  rootArea() const { return rootArea_; }
    //! Returns true if actual update has occured
    virtual bool        update();
    //!
    virtual double      getZoom() const = 0;
    //!
    virtual void        setZoom(double zoom) = 0;
    //!
    virtual void        setContextHint(bool enabled, bool persistent,
                                       bool showParent, uint delay) = 0;
    //! Detaches formatted areas from formatter
    void                detachAreaTree();
    //!
    virtual bool        isToPostpone(const Formatter::Area* lastAreaMade);

//! Operating with cursor
    const EditContext&  context() const { return context_; }

    void                setCursor(const Formatter::AreaPos& pos,
                                  const GroveEditor::GrovePos& srcPos,
                                  bool isTop);
    //! Makes cursor visible
    virtual bool        showCursor() = 0;
    //! Hides the cursor
    virtual void        hideCursor() = 0;
    //! Makes cursor blinking
    virtual void        startCursor() = 0;
    //! Stops blinking
    virtual void        stopCursor() = 0;
    //! Set focus
    virtual void        grabFocus() = 0;
    //! Set focus
    virtual void        print() = 0;

    virtual void        setMicroFocusHint() = 0;
    virtual void        resetIM() = 0;

    //! Returns true if cursor exists and visible othervise false
    bool                isCursorVisible() const { return cursorVisible_; }
    //!
    Formatter::AreaPos  getInitialCursor() const;
    //! Get up/down shifted cursor position
    Formatter::AreaPos  getUpCursor(bool up) const;
    //! Get page-up/page-down shifted cursor position
    Formatter::AreaPos  getPageCursor(bool up, bool toCorner);

//! Selecting the parts of the document
    //! Use "raw" to get nonbalanced selection (raw user input
    const Selection&    getSelection(bool raw = false) const {
        return (raw || !isSelectionBalancing_)
            ? selection_ : balancedSelection_;
    }
    void                setSelection(const Selection& raw,
                                     const Selection& balanced,
                                     bool force = false);
    void                setSelectionBalancing(bool isOn);
    void                removeSelection();
    virtual void        repaintView() = 0;
    virtual void        adjustView(const Formatter::AreaPos& pos,
                                   bool isTop) = 0;
    //!
    static  Formatter::FontMgr* fontManager();

    //! Progress stream is for handling progress messages.
    virtual void        setProgressStream(Common::MessageStream* pstream) = 0;
    //!
    void                inSelectionDo(const TreeSelection& r,
                                      SelectionOp&) const;

    void                deleteFormatter() { formatter_ = 0; }

    virtual QWidget*    widget() = 0;
    virtual void        setParentWidget(QWidget*) = 0;
    virtual void        scrollByPage(bool isUp) = 0;
    virtual QPoint      mapToGlobal(const Formatter::CPoint&) const = 0;
    virtual void        setWatermark(const Common::String&) = 0;

protected:
    virtual Common::MessageStream*   pstream() const = 0;
    //!
    virtual Formatter::TagMetrixTable&  tagMetrixTable() = 0;
    //!
    virtual ImProvider*         imageProvider() = 0;
    virtual InlineObjFactory*   inlineObjFactory() = 0;
    //!
    virtual const Avf*  areaViewFactory() const = 0;

    //!
    void                continuePostponedFormatting();
    //!
    virtual bool        isBelowVisibleRange(const Formatter::Area* area) = 0;
    //! Redraws the (boolean) differense between two regions
    void                repaintSelection(const TreeSelection& r1,
                                         const TreeSelection& r2) const;
    //!
    void                inSelectionDo(const Formatter::Chain* chain,
                                      ulong level,
                                      bool isStartCorner, bool isEndCorner,
                                      const Formatter::TreePos& start,
                                      const Formatter::TreePos& end,
                                      SelectionOp& op) const;

    EditableView(Common::Messenger* messenger, const GroveLib::GrovePtr& fot,
                 EditPolicy* editPolicy);

protected:
    Common::MessengerPtr    messenger_;
    GroveLib::GrovePtr      fot_;
    EditPolicy*             editPolicy_;

    FormatterOwner          formatter_;
    const Formatter::Area*  rootArea_;

    EditContext             context_;
    Selection               selection_;
    Selection               balancedSelection_;
    bool                    isSelectionBalancing_;
    bool                    cursorVisible_;
private:
    FormattingMode          formattingMode_;
};

#endif  // EDITABLE_VIEW_H
