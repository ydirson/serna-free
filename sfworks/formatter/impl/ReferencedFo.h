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

#ifndef FORMATTER_REFERENCED_FO_H
#define FORMATTER_REFERENCED_FO_H

#include <list>
#include "common/safecast.h"
#include "grove/NodeVisitor.h"
#include "grove/Nodes.h"

#include "formatter/impl/FoImpl.h"
#include "formatter/impl/Properties.h"
#include "formatter/TagMetrixTable.h"

namespace Formatter {

template <class T, class Stop>
T* find_sibling_fo(const GroveLib::Node* fo_node, bool next)
{
    while (fo_node) {
        Fo* fo = getFoByNode(fo_node);
        if (fo) {
            if (typeid(Stop) == typeid(fo))
                return 0;
            if (typeid(T) == typeid(fo))
                return SAFE_CAST(T*, fo);
        }
        fo_node = (next) ? fo_node->nextSibling() : fo_node->prevSibling();
    }
    return 0;
}

template <class T> T* ancestor(Fo* fo)
{
    while (fo) {
        if (typeid(T) == typeid(fo))
            return static_cast<T*>(fo);
        fo = fo->parent();
    }
    return 0;
}

void    adjust_redline_props(const GroveLib::Node* foNode, 
                             int& decoration, Rgb& textColor);

Fo*     find_sibling_fo(const GroveLib::Node* fo_node, bool next);

/*! \brief Binds fo-node and it`s representation
 */
class FORMATTER_EXPIMP ReferencedFo : public FoImpl,
                                      public GroveLib::NodeVisitor {
public:
    FORMATTER_OALLOC(ReferencedFo);

    ReferencedFo(const FoInit& init);
    virtual ~ReferencedFo();

    //!
    CPoint      getAllcPoint(const CPoint& spaceAllc) const;
    //!
    bool        isEnoughSpace(const Allocation& alloc,
                              const Area* after) const;

    //! ReferencedFo belongs to only one foNode
    const GroveLib::Node*   headFoNode() const { return foNode_; }
    //! ReferencedFo belongs to only one foNode
    const GroveLib::Node*   tailFoNode() const { return foNode_; }
    //! Lazy construction of Fo subtree
    virtual void            buildChildren();

//FO DYNAMICS
    //! Fo does not remove itself
    virtual void        nodeDestroyed(const GroveLib::Node*) {};
    //! Insert child fo for the inserted fo node
    virtual void        childInserted(const GroveLib::Node* child);
    //! Remove child fo for the removed fo node
    virtual void        childRemoved (const GroveLib::Node* node,
                              const GroveLib::Node* child);
    //! TODO: recalculate affected properties
    virtual void        attributeChanged(const GroveLib::Attr*);
    //! TODO: recalculate affected properties
    virtual void        attributeRemoved(const GroveLib::Element*,
                                 const GroveLib::Attr*);
    //! TODO: recalculate affected properties
    virtual void        attributeAdded(const GroveLib::Attr*);
    //! This event is handled in TextFo
    virtual void        textChanged(const GroveLib::Text*) {};

    //!
    uint        level(bool physical = false) const {
        return (physical) ? physicalLevel_ : logicalLevel_;
    }
protected:
    //!
    virtual Fo* makeChildFo(const GroveLib::Node* foNode);
    //!
    const GroveLib::Node*   node() const { return foNode_; }
    //!
    bool        needsDecoration(Decoration) const { return false; }
    //!
    Rgb         getSchemeColor() const;

private:
    const GroveLib::Node*   foNode_;
protected:
    const GroveLib::Node*   foOrigin_;
    uint                    physicalLevel_;
    uint                    logicalLevel_;
    bool                    trackChildren_;
};

class PageMasterMap;

/*! \brief Creates layout-master-set for page generation
 */
class FORMATTER_EXPIMP RootFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(RootFo);

    typedef COMMON_NS::OwnerPtr<PageMasterMap>   PageMasterMapOwner;

    RootFo(const GroveLib::Node* foRoot, FoData& data);

    FoType      type() const { return ROOT_FO; }
    //!
    bool        isReference(CRange& contRange) const;
    //!
    void        detachRootArea() { rootArea_ = 0; }
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //! TODO: reimplement in TableFo, etc.
    PDirection  progression() const { return AMORPHOUS; }

    COMMON_NS::String   name() const;
private:
    PageMasterMapOwner  pageMasterMap_;
    mutable AreaPtr     rootArea_;
};

class PageMaster;
class PageSpecs;

/*! \brief Generates page sequence areas
 */
class PageFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(PageFo);

    typedef COMMON_NS::RefCntPtr<PageMaster> PageMasterPtr;

    PageFo(const GroveLib::Node* foPage, FoData& data,
           const PageMasterMap& pageMasterMap);

    FoType      type() const { return PAGE_FO; }
    //!
    bool        isReference(CRange& contRange) const;
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    void        updateGeometry(const AreaPtr&, const Allocation&,
                               const Area*, bool) const;
    //!
    void        expandGeometry(const AreaPtr& area,
                               const CRect& space) const;
    //!
    COMMON_NS::String   name() const;
    
    const PageSpecs*    pageSpecs() const { return pageSpecs_; }

private:
    PageMasterPtr       pageMaster_;
    const PageSpecs*    pageSpecs_;
};

class PageSpecs;
class RegionSpecs;

/*! \brief Generates region areas for the flow content
 */
class FlowFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(FlowFo);

    typedef COMMON_NS::RefCntPtr<RegionSpecs> RegionSpecsPtr;

    FlowFo(const GroveLib::Node* foFlow, FoData& data,
           const PageSpecs& pageSpecs);

    FoType      type() const { return FLOW_FO; }
    //!
    bool        isReference(CRange& contRange) const;
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    CPoint      getAllcPoint(const CPoint& spaceAllc) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    void        updateGeometry(const AreaPtr&, const Allocation&,
                               const Area*, bool) const;
    //!
    void        expandGeometry(const AreaPtr& area,
                               const CRect& space) const;
    const RegionSpecs*  regionSpecs() const;
    //!
    COMMON_NS::String   name() const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
private:
    RegionSpecsPtr      regionSpecs_;
};

///////////////////////////////////////////////////////////////////////

class BlockArea;
class BlockLevelFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(BlockLevelFo);

    BlockLevelFo(const FoInit& init);
    
    //!
    bool        isReference(CRange&) const { return false; }
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    bool        isEnoughSpace(const Allocation& alloc,
                              const Area* after) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    CPoint      getContPoint(const Area* area, bool isFirst) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //!
    void        expandGeometry(const AreaPtr& area,
                               const CRect& space) const;
    //!
    virtual COMMON_NS::String   areaName() const = 0;
    //!
    bool        bodyStart(const FunctionArgList& args, ValueTypePair&,
                          const PropertyContext*,
                          const Allocation& alloc) const;
    //!
    bool        labelEnd(const FunctionArgList& args, ValueTypePair&,
                         const PropertyContext*,
                         const Allocation& alloc) const;

    //!
    bool            isPreserveLinefeed() const;

protected:
    CType       contWidth(const CType& allocW) const;
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    virtual CType   childrenHeight(const Area* area) const;
    //!
    bool        needsDecoration(Decoration type) const;
    //!
    CType       getDecoration(Decoration type, CType& bord, CType& padd) const;
    //!
    void        settleEmptyBlock(BlockArea* block, const Area* prevChunk,
                                 bool& force_space, CType& space_left) const;
protected:
    CType           startIndent_;
    mutable CType   endIndent_;
    CType           contentWidth_;
    TagMetrix*      tagMetrix_;
    LfTreatment     lfTreatment_ : 4;
};

/*! \brief Generates block areas
 */
class BlockFo : public BlockLevelFo {
public:
    FORMATTER_OALLOC(BlockFo);

    BlockFo(const FoInit& init)
        : BlockLevelFo(init) {}

    FoType      type() const { return BLOCK_FO; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
//FO DYNAMICS
    //! Reimplemented from ReferencedFo to track LineFos
    void        childInserted(const GroveLib::Node* child);
    //! Reimplemented from ReferencedFo to track LineFos
    void        childRemoved (const GroveLib::Node* node,
                              const GroveLib::Node* child);
protected:
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
};

/*! \brief
 */
class ListBlockFo : public BlockLevelFo {
public:
    FORMATTER_OALLOC(ListBlockFo);

    ListBlockFo(const FoInit& init)
        : BlockLevelFo(init) {}
    //!
    FoType      type() const { return LIST_BLOCK_FO; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);

    friend class    BlockLevelFo;
private:
    CType       startsDist_;
    CType       separation_;
};

/*! \brief
 */
class ListItemFo : public BlockLevelFo {
public:
    FORMATTER_OALLOC(ListItemFo);

    ListItemFo(const FoInit& init)
        : BlockLevelFo(init) {}

    FoType      type() const { return LIST_ITEM_FO; }
    //!
    bool        isParallel() const { return true; }
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    PDirection  progression() const { return PARALLEL; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
protected:
    //!
    CType       childrenHeight(const Area* area) const;
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
};

/*! \brief
 */
class ListItemLabelFo : public BlockFo {
public:
    FORMATTER_OALLOC(ListItemLabelFo);

    ListItemLabelFo(const FoInit& init)
        : BlockFo(init) {}
    //!
    FoType      type() const { return LIST_ITEM_LABEL_FO; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
};

/*! \brief
 */
class ListItemBodyFo : public BlockFo {
public:
    FORMATTER_OALLOC(ListItemBodyFo);

    ListItemBodyFo(const FoInit& init)
        : BlockFo(init) {}
    
    FoType      type() const { return LIST_ITEM_BODY_FO; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
};

/*! \brief Keeps inline fos together within block fo
 */
class LineFo : public FoImpl {
public:
    FORMATTER_OALLOC(LineFo);

    LineFo(PropertySet* parentSet, const GroveLib::Node* foInline,
           FoData& data, uint physicalLevel, uint logicalLevel);

    FoType      type() const { return LINE_FO; }
    //!
    bool        isReference(CRange&) const { return false; }
    //!
    bool        isEnoughSpace(const Allocation& alloc,
                              const Area* after) const;
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    CPoint      getAllcPoint(const CPoint& spaceAllc) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //!
    uint        level(bool physical = false) const {
        return (physical) ? physicalLevel_ : logicalLevel_;
    }
    //! LineFo belongs to nodes of it`s child fos
    const GroveLib::Node*   headFoNode() const;
    //! LineFo belongs to nodes of it`s child fos
    const GroveLib::Node*   tailFoNode() const;

    //! LineFo buids it`s subtree in construction
    void        buildChildren() {};
    //!
    PDirection  progression() const { return HORIZONTAL; }
    //! Prints the debugging info
    void        dump(int indent) const;
    //! Uses parent fo to obtain properties
    template <class T> T&   getProperty(const Allocation& alloc,
                                        const CType percentBase = -1) {
        T& property = parentSet_->getProperty<T>(alloc, percentBase);
        usedPropertyMap_[T::name()] = 0;
        return property;
    }
    //!
    COMMON_NS::String       name() const;
    //!
    bool        isPreserveLinefeed() const {
        return parent()->isPreserveLinefeed();
    }

protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    bool        needsDecoration(Decoration) const { return false; }
    //!
    void        updateBaseline(const Area* area) const;

private:
    //! LineFo has no origin node
    const GroveLib::Node*   node() const { return 0; }
private:
    CType       accender_;
    CType       descender_;
    CType       startIndent_;
    CType       endIndent_;
    uint        physicalLevel_;
    uint        logicalLevel_;
    TAlign      textAlign_;
};

/*! \brief Generates inline areas
 */
class InlineFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(InlineFo);

    InlineFo(const FoInit& init);

    FoType      type() const { return INLINE_FO; }
    //!
    ContentType contentType() const { return INLINE_CONTENT; }
    //!
    bool        isReference(CRange&) const { return false; }
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    bool        isEnoughSpace(const Allocation& alloc,
                              const Area* after) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    CPoint      getContPoint(const Area* area, bool isFirst) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //!
    PDirection  progression() const { return HORIZONTAL; }
    //!
    COMMON_NS::String   name() const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    void        checkProperties(const Allocation& alloc);
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
    //!
    bool        needsDecoration(Decoration type) const;
    //!
    CType       getDecoration(Decoration type, CType& bord, CType& padd) const;
private:
    CType       accender_;
    CType       descender_;
    TagMetrix*  tagMetrix_;
    CType       baselineShift_;
};

/*! \brief Generates no children
 */
class TerminalFo : public ReferencedFo {
public:
    FORMATTER_OALLOC(TerminalFo);

    TerminalFo(const FoInit& init)
        : ReferencedFo(init) {}
    //!
    ContentType contentType() const { return INLINE_CONTENT; }

    //! Prevents child allocation for terminal fos
    CRect       getSpaceAfter(const Area*, bool, const Area*,
                              const CRect&) const
    {
        RT_ASSERT("Terminal Fos don`t give space for children");
        return CRect();
    };
    //! Terminals don`t change its geometry
    void        updateGeometry(const AreaPtr&, const Allocation&,
                               const Area*, bool) const {};
    //! To be reimplemented in concrete terminl fos
    bool        isChainFinishedAt(const AreaPtr& area) const
    {
        return (!area.isNull());
    }
    //! To be reimplemented in concrete terminl fos
    bool        hasContent(const AreaPtr& area) const
    {
        return (!area.isNull());
    }
    //!
    bool        isReference(CRange&) const { return false; }
    //!
    bool        isTerminal() const { return true; }
    //!
    PDirection  progression() const { return HORIZONTAL; }

//FO DYNAMICS
    //! Terminal Fo has no children
    void        childInserted(const GroveLib::Node*) {};
    //! Terminal Fo has no children
    void        childRemoved (const GroveLib::Node*, const GroveLib::Node*) {};
protected:
    //! Makes no children
    Fo*         makeChildFo(const GroveLib::Node*) { return 0; }
};

} // namespace Formatter

#endif // FORMATTER_REFERENCED_FO_H
