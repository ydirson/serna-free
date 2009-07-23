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

#ifndef FORMATTER_FO_CONTROLLER_H
#define FORMATTER_FO_CONTROLLER_H

#include "common/XList.h"

#include "formatter/formatter_defs.h"
#include "formatter/impl/Fo.h"
#include <map>

namespace Formatter {

typedef std::multimap<const Fo*, const FoController*>  FoControllerMap;
typedef FoControllerMap::const_iterator                FocMapIterator;
extern bool is_being_processed(const FoControllerMap* map, const Fo* fo);
    
    
/*! \brief FoController implements FSM to perform the formatting of given Fo
 */
class FoController : public COMMON_NS::RefCounted<>,
                     public COMMON_NS::XTreeNode<FoController,
                            COMMON_NS::XTreeNodeRefCounted<FoController> > {
public:
    typedef const AreaViewFactory           CViewFactory;

    FORMATTER_OALLOC(FoController);

    enum State {
        CHAIN_CHECK, AREA_PREP,  CHILD_MAKE,
        CHILD_BUILD, AREA_CLOSE, AREA_FAIL, FINAL
    };

    FoController(const AreaViewFactory* viewFactory,
                 const FoPtr& fo, const AreaPtr& lastArea,
                 uint& splitCount, FoControllerMap* foMap);
    virtual ~FoController();

    //!
    const FoPtr&        fo() const { return fo_; }
    //!
    const Area*         lastAreaMade() const { return lastAreaMade_; }
    //! Returns the state of the formatting process
    State               state() const { return state_; }
    //!
    void                resetFailedState() { state_ = CHAIN_CHECK; }
    //!
    FoController*       process();
    //!
    void                finalize();
    //!
    bool                isFinalizing() const { return isFinalizing_; }
    //! Returns Area being rendered
    const AreaPtr&      newArea() const { return newArea_; }
    //! Last area made by this or previous sibling fo (given in  constructor)
    const AreaPtr&      lastArea() const { return lastArea_; }
    //! Returns the last rendered Area (from this fo)
    Area*               lastChunk() const;
    //!
    Allocation          getSpaceAfter(const Area* childArea) const;
    //!
    CRect               getSpaceAfter(const Area* area, const Area* childArea, 
                                      const CRect&) const;
    //!
    CPoint              absContPoint() const;

protected:    
    //!
    void                makeChild();
    //!
    void                makeParallelChild();

    void                dump() const;
    void                dumpState(bool stateChanged = false) const;

protected:
    //!
    bool                prepareArea();
    //!
    void                prepareArea(Area* area, const Allocation& newAreaSpace,
                                    const CPoint& newAllcPoint);
    //!
    bool                closeArea(bool isContentExhausted);
    //!
    void                handleAffectedChildren() const;
    //!
    void                handleAffectedSiblings(const Area* child) const;
    //!
    void                discardChildrenAfter(const Area* area) const;
    //!
    CPoint              newAllcPoint() const { return newAllcPoint_; }
    //!
    PDirection          direction() const;

protected:
    //! Place newArea_ to it`s chain and to area tree
    void                placeArea();
    //! Place newArea_ to area tree
    void                placeAreaNode();
    //! Replace old area with new area
    void                replaceArea();
    //! Removes line area if some children does not fits vertically
    bool                removeLineArea();
    //! Moves created area above if possible
    void                checkSpaceAbove();
    //! Shifts created area if it was moved by "checkSpaceAbove"
    void                moveArea(Area* area, const CPoint& shift) const;
    //!
    void                setNewAreaSpace(const Area* area);
    //!
    void                discardChainAfter(const AreaPtr& area);

private:
    FoPtr               fo_;
    CViewFactory*       viewFactory_;
    State               state_;
    const Area*         lastAreaMade_;
    uint&               splitCount_;

    AreaPtr             oldArea_;
    AreaPtr             newArea_;
    AreaPtr             lastArea_;
    Allocation          newAreaSpace_;
    CPoint              newAllcPoint_;
    uint                failCount_;
    bool                isFinalizing_;
    FoControllerMap*    foMap_;
};

class BranchData : public Common::RefCounted<>,
                   public Common::XListItem<
                      BranchData, Common::XTreeNodeRefCounted<BranchData> > {
public:
    BranchData(FoController* foc, Area* area, Area* after) 
        : foc_(foc), area_(area), after_(after) {}
    
    FoController*   foc_;
    Area*           area_;
    Area*           after_;
    Allocation      newAreaSpace_;
    CPoint          newAllcPoint_;
};

typedef Common::XList<BranchData, 
                      Common::XTreeNodeRefCounted<BranchData> > BranchDataList;

extern void calc_branch_data(BranchDataList& data_list, FoController* foc, 
                             Area* newArea);


} // namespace Formatter

#endif // FORMATTER_FO_CONTROLLER_H
