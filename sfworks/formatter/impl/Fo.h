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
/*! \file
 */

#ifndef FORMATTER_FO_H
#define FORMATTER_FO_H

#include "common/XTreeNode.h"
#include "common/String.h"
#include "grove/Decls.h"

#include "formatter/decls.h"
#include "formatter/Area.h"
#include "formatter/impl/Property.h"
#include "formatter/XslFoExt.h"

namespace Formatter {

class Fo;
class AreaViewFactory;
class ModificationRegistry;

//! Following enumeration declares supported subset of XSL specification
enum FoName {
    FIRST_FO = 0,
    //! Pagination and Layout formatting objects
    ROOT = FIRST_FO,
    PAGE_SEQUENCE,
    PAGE_SEQUENCE_MASTER,
    SINGLE_PAGE_MASTER_REFERENCE,
    REPEATABLE_PAGE_MASTER_REFERENCE,
    LAYOUT_MASTER_SET,
    SIMPLE_PAGE_MASTER,
    REGION_BODY,
    DECLARATIONS,
    FLOW,

    //! Block formatting objects
    BLOCK,

    //! Inline formatting objects
    EXTERNAL_GRAPHIC,
    INLINE,
    LEADER,
    PAGE_NUMBER,
    PAGE_NUMBER_CITATION,

    //! Table formatting objects
    TABLE_AND_CAPTION,
    TABLE,
    TABLE_COLUMN,
    TABLE_CAPTION,
    TABLE_HEADER,
    TABLE_FOOTER,
    TABLE_BODY,
    TABLE_ROW,
    TABLE_CELL,

    //! List formatting objects
    LIST_BLOCK,
    LIST_ITEM,
    LIST_ITEM_LABEL,
    LIST_ITEM_BODY,

    //! Syntext extentions
    TEXT,
    LINE,
    SECTION_CORNER,
    CHOICE,
    COMMENT,
    PI,
    FOLD,
    COMBO_BOX,
    LINE_EDIT,
    INLINE_OBJECT,

    //! Unknown or unsupported formatting objects
    UNKNOWN,
    LAST_FO
};

/*! Following structure describes the fo content type according to
  section 6.2 of XSL specifications
*/
enum ContentType {
    BLOCK_CONTENT,
    TABLE_CONTENT,
    INLINE_CONTENT,
    OUT_OF_LINE_CONTENT,
    IGNORED
};

enum ModificationType {
    PREV_FO_MODIFIED,
    NEXT_FO_MODIFIED,
    THIS_FO_MODIFIED,
    NO_MODIFICATION
};

/*!
 */
class Allocation {
public:
    Allocation()
        : area_(0), maxw_(false), maxh_(false), 
          enoughW_(true), enoughH_(true) {}
    
    void            dump() const;

public:
    const Area*     area_;
    CRect           space_;
    bool            maxw_;
    bool            maxh_;
    mutable bool    enoughW_;
    mutable bool    enoughH_;
};

class FoController;
    
/*! \brief Fo produces Areas and keeps them in single chain.

  Fo also keeps all properties necessary for area generation.
*/
class FORMATTER_EXPIMP Fo : public Chain,
                            public COMMON_NS::RefCounted<>,
                            public COMMON_NS::XTreeNode<Fo,
                                   COMMON_NS::XTreeNodeRefCounted<Fo> >,
                            public PropertySet {
public:
    FORMATTER_OALLOC(Fo);
    typedef COMMON_NS::XTreeNode<Fo, COMMON_NS::XTreeNodeRefCounted<Fo> > FTN;

    REDECLARE_XTREENODE_INTERFACE_BASE(FTN, Fo);
    Fo(PropertySet* parentSet, bool hasParentOrigin)
        : Chain(hasParentOrigin),
          PropertySet(parentSet),
          mType_(NO_MODIFICATION),
          formattedBy_(0),
          lineHeight_(0) {};
    virtual ~Fo();

    //!
    virtual ContentType contentType() const { return BLOCK_CONTENT; }
    //!
    virtual bool        isEnoughSpace(const Allocation& alloc,
                                      const Area* after) const = 0;
    //!
    virtual Area*       makeArea(const Allocation& alloc, const Area* after,
                                 bool forceToMake) const = 0;
    //! Removes this fo with its Areas
    void                removeFo();
    //! Insert child fo and registers modification
    void                insertChildFo(Fo* childFo,
                                      const GroveLib::Node* childNode);
    //! Removes child fo and registers its siblings as modified
    void                removeChildFo(Fo* child);
    //! Reimplemented to make public
    virtual void        checkProperties(const Allocation& alloc);
    const CType&        lineHeight() const { return lineHeight_; }

//! For AreaPos
    //! Returns next sibling fo
    Chain*              nextChain() const { return nextSibling(); }
    //! Returns next sibling fo
    Chain*              prevChain() const { return prevSibling(); }
    //!
    Chain*              parentChain() const { return parent(); }
    //!
    Chain*              commonAos(const Chain* chain) const;
    //! Returns the chain by it`s sequential number
    Chain*              chainAt(ulong chainPos) const;
    //! Returns true if there are child fos
    bool                hasChildChains() const { return !empty(); }
    //! Returns the count of available positions
    ulong               chainPosCount() const;
    //! Returns position of the child chain inside this one
    ulong               chainPos() const { return siblingIndex(); }
    //!
    void                treeloc(COMMON_NS::TreelocRep& tloc) const;
    //!
    Chain*              byTreeloc(COMMON_NS::TreelocRep& tloc) const;

    //! This is DIFFERENT treeloc - it is treeloc in XSLT instance tree
    void                getTreeloc(COMMON_NS::TreelocRep& tloc) const;

    //! Returns area allocation point within given space
    virtual CPoint      getAllcPoint(const CPoint& spaceAllc) const = 0;
    //!
    virtual CPoint      getContPoint(const Area* area, bool isFirst) const;
    //! Returns the maximum space available  after 'child' area in given space
    virtual CRect       getSpaceAfter(const Area* area, bool isFirst,
                                      const Area* child,
                                      const CRect& space) const = 0;
    //! Sets area geometry after the placing of all children
    virtual void        updateGeometry(const AreaPtr& area,
                                       const Allocation& alloc,
                                       const Area* prevChunk,
                                       bool isLast) const = 0;
    //! Sets area geometry to maximum possible size according given space
    virtual void        expandGeometry(const AreaPtr&, const CRect&) const {};
//! Formatting process
    //! Lazy construction of Fo subtree
    virtual void        buildChildren() = 0;
    //!
    bool                isFormatting() const { return 0 != formattedBy_; }
    //! Increments branch counter
    void                startFormatting() { ++formattedBy_; }
    //!
    void                stopFormatting() { --formattedBy_; }
    //! Forces formatter to remove Branches which are processing this Fo
    virtual void        detachFromFormatter(const FoController* 
                                            preserveBranch) = 0;
    //! If was not registered then register
    ModificationType    modificationType() const { return mType_; }
    //! If was not registered then register
    virtual void        registerModification(ModificationType mType) = 0;
    //! If was registered then deregister
    virtual void        deregisterModification() = 0;
    //! Determines by content & decoration whether the area is the last one
    virtual bool        isChainFinishedAt(const AreaPtr& area) const = 0;
    //! Returns true if area has children or other visual data
    virtual bool        hasContent(const AreaPtr& area) const;
    //! Returns true if this Fo produces terminal children.
    virtual bool        isTerminal() const { return false; }
    //! Returns true if this Fo treats children nonsequentially
    virtual bool        isParallel() const { return false; }

//! Debugging purposes
    //! Prints the debugging info
    virtual void        dump(int indent = 0) const;

public:
    static FoName       foName(const GroveLib::Node* node);
    static ContentType  contentType(const GroveLib::Node* node);

private:
    PropertySet*        firstChildSet() const { return firstChild(); }
    PropertySet*        nextSiblingSet() const { return nextSibling(); }
protected:
    //! Registers property modification
    virtual void        registerPropertyModification();

protected:
    ModificationType    mType_;
    int                 formattedBy_;
    CType               lineHeight_;
};

inline Fo* getFoByNode(const GroveLib::Node* foNode)
{
    if (foNode) {
        Chain* chain = XslFoExt::areaChain(foNode);
        if (chain)
            return static_cast<Fo*>(chain);
    }
    return 0;
}

} // namespace Formatter

#endif // FORMATTER_FO_H
