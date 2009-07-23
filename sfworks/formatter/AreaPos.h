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

#ifndef FORMATTER_AREA_POS_H
#define FORMATTER_AREA_POS_H

#include "common/common_types.h"
#include "common/TreelocRep.h"
#include "formatter/formatter_defs.h"

namespace Formatter {

class Area;
class TreePos;
class Chain;
class ChainPos;

/*
*/
class FORMATTER_EXPIMP AreaPos {
public:
    FORMATTER_OALLOC(AreaPos);
    AreaPos();
    AreaPos(const Area* area, ulong pos);
    AreaPos(const AreaPos& areaPos);

    bool        isNull() const;
    bool        isValid() const;
    bool        validate();
    template <class Pred>
    bool        validate(bool fwd, Pred pred)
        {
            RT_ASSERT(!isNull());
            // Search in given direction
            AreaPos old(*this);
            while (!isNull() && (!isValid() || !pred(this))) {
                if (fwd)
                    (*this)++;
                else
                    (*this)--;
            }
            return !isNull();
        }
    //! Tries to find the nearest allowed position in given direction
    void        findAllowed(bool fwd, bool lookOpposite);
    //! Returns true if this position lays on the corner
    bool        isCorner() const { return (isFrontCorner() || isBackCorner());}
    //! Returns true if this position lays on the front corner
    bool        isFrontCorner() const;
    //! Returns true if this position lays on the back corner
    bool        isBackCorner() const;
    //! Returns the area if is valid, 0 otherwise
    const Area* area() const { return area_; }
    //! Returns the local position in the area
    ulong       pos() const { return pos_; }

    void        getPagePos(uint& page, uint& max_page) const;

    //!
    static AreaPos  getPosBeforeArea(const Area* area);
    //!
    static AreaPos  getPosAfterArea(const Area* area);
    //!
    AreaPos     getPosBeforeArea() const;
    //!
    AreaPos     getPosAfterArea() const;

    //! Overloaded for iteration through area tree
    AreaPos&    operator++(int) { return (*this)+=1; }
    //! Overloaded for iteration through area tree
    AreaPos&    operator--(int) { return (*this)-=1; }
    //! Overloaded for iteration through area tree
    AreaPos&    operator+=(ulong shift);
    //! Overloaded for iteration through area tree
    AreaPos&    operator-=(ulong shift);
    //! Returns true if this position is less than given one
    bool        operator<(const AreaPos& p) const;
    //!
    bool        operator==(const AreaPos& p) const;
    //!
    bool        operator!=(const AreaPos& p) const;
    //!
    void        dump() const;
private:
    const Area* area_;
    ulong       pos_;
};

/*
 */
class FORMATTER_EXPIMP ChainPos {
public:
    FORMATTER_OALLOC(ChainPos);
    ChainPos();
    ChainPos(const AreaPos& areaPos);
    ChainPos(const Chain* chain, ulong pos);
    //!
    bool            isNull() const;
    //! Returns true if this position lays on the corner
    bool    isCorner() const { return (isFrontCorner() || isBackCorner()); }
    //! Returns true if this position lays on the front corner
    bool            isFrontCorner() const;
    //! Returns true if this position lays on the back corner
    bool            isBackCorner() const;
    //! Returns the area if is valid, 0 otherwise
    const Chain*    chain() const { return chain_; }
    //! Returns the local position in the area
    ulong           pos() const { return pos_; }
    //! Returns corresponding AreaPos
    AreaPos         toAreaPos() const;
    //! Returns true if this position is less than given one
    bool            operator<(const ChainPos& p) const;
    //! Returns true if this position is greater than given one
    bool            operator>(const ChainPos& p) const;
    //!
    bool            operator==(const ChainPos& p) const;
    //!
    bool            operator!=(const ChainPos& p) const;
    //!
    void            dump() const;
private:
    const Chain*    chain_;
    ulong           pos_;
};

/*! \brief ChainSelection
 */
class FORMATTER_EXPIMP ChainSelection {
public:
    FORMATTER_OALLOC(ChainSelection);
    ChainSelection();
    ChainSelection(const ChainPos& chainPos);
    ChainSelection(const ChainPos& start, const ChainPos& end);
    ChainSelection(const AreaPos& start, const AreaPos& end);
    ChainSelection(const ChainSelection& r);

    //!
    bool            isEmpty() const { return (start_ == end_); }
    //!
    bool            operator==(const ChainSelection& s) const;
    //!
    bool            operator!=(const ChainSelection& s) const;
    //! Clears the selection
    void            clear() { start_ = end_ = ChainPos(); }
    //! Updates the end_ corner of the region
    void            extendTo(const ChainPos& pos) {
        if (!start_.isNull()) 
            end_ = pos; 
    }
    //! Returns intersection of the given regions
    ChainSelection  intersection(const ChainSelection& selection) const;
    //! Returns the begining of the region
    const ChainPos& start() const { return start_; }
    //! Returns the end of the region
    const ChainPos& end() const { return end_; }
    //! Returns the lower corner
    const ChainPos& minPos() const;
    //! Returns the higher corner
    const ChainPos& maxPos() const;

    const Chain*    commonChain() const;
    void            balance();

    void            dump() const;
private:
    ChainPos        start_;
    ChainPos        end_;
};

/* \!brief TreePos
 */
class FORMATTER_EXPIMP TreePos {
public:
    typedef COMMON_NS::TreelocRep Treeloc;

    FORMATTER_OALLOC(TreePos);
    TreePos() {};
    TreePos(const AreaPos& areaPos);
    TreePos(const ChainPos& chain_pos);
    TreePos(const COMMON_NS::TreelocRep& treeloc);

    //! Returns the treeloc for the position
    const Treeloc&  treeloc() const { return treeloc_; }
    //!
    bool            isNull() const;
    //! Returns corresponding AreaPos
    AreaPos         toAreaPos(const Area* root) const;
    //! Returns corresponding ChainPos position
    ChainPos        toChainPos(const Area* root) const;
    //! Returns true if this position is less than given one
    bool            operator<(const TreePos& p) const;
    //!
    bool            operator<=(const TreePos& p) const;
    //!
    bool            operator>(const TreePos& p) const;
    //!
    bool            operator>=(const TreePos& p) const;
    //!
    bool            operator==(const TreePos& p) const;
    //!
    bool            operator!=(const TreePos& p) const;
    //!
    void            clear() {treeloc_.clear();}
    //!
    void            dump() const;
private:
    Treeloc         treeloc_;
};

/*! \brief TreeSelection
 */
class FORMATTER_EXPIMP TreeSelection {
public:
    FORMATTER_OALLOC(TreeSelection);
    TreeSelection();
    TreeSelection(const TreePos& t);
    TreeSelection(const TreePos& start, const TreePos& end);
    TreeSelection(const AreaPos& start, const AreaPos& end);
    TreeSelection(const TreeSelection& r);

    //!
    bool            isEmpty() const { return (start_ == end_); }
    //!
    bool            operator==(const TreeSelection& s) const;
    //!
    bool            operator!=(const TreeSelection& s) const;
    //! Clears the selection
    void            clear() { start_.clear(); end_.clear();}
    //! Updates the end_ corner of the region
    void            extendTo(const TreePos& pos) {
        if (!start_.isNull()) 
            end_ = pos; 
    }
    //! Returns intersection of the given regions
    TreeSelection   intersection(const TreeSelection& r) const;
    //! Returns the begining of the region
    const TreePos&  start() const { return start_; }
    //! Returns the end of the region
    const TreePos&  end() const { return end_; }
    //! Returns the lower corner
    const TreePos&  minLoc() const;
    //! Returns the higher corner
    const TreePos&  maxLoc() const;

    TreePos         lastCommon() const;
    bool            isBalanced() const;

    void            dump() const;
private:
    TreePos         start_;
    TreePos         end_;
};

}

#endif  // FORMATTER_AREA_POS_H
