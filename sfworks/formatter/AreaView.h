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

#ifndef FORMATTER_AREA_VIEW_H
#define FORMATTER_AREA_VIEW_H

#include "formatter/formatter_defs.h"
#include "formatter/types.h"

namespace Formatter {

class Area;

class FORMATTER_EXPIMP AreaSelection {
public:
    FORMATTER_OALLOC(AreaSelection);
    AreaSelection();
    //!
    ulong   from() const {return from_;}
    //!
    ulong   to() const {return to_;}
    //!
    bool    extendsToLeft() {return extendsToLeft_;}
    //!
    bool    extendsToRight() {return extendsToRight_;}
    //!
    bool    isEmpty() const;
    //!
    bool    isNull() const;
    //!
    void    set(long from, long to, bool extendsToLeft, bool extendsToRight);
    //!
    void    clear();
private:
    long    from_;
    long    to_;
    bool    extendsToLeft_;
    bool    extendsToRight_;
};

/*! \brief Base class for visual representation of Area
 */
class FORMATTER_EXPIMP AreaView {
public:
    FORMATTER_OALLOC(AreaView);

    AreaView(const Area* area)
       : area_(area) {}
    virtual ~AreaView() {}

    const Area*     area() const { return area_; }
    //!
    virtual void    updateGeometry() = 0;
    //!
    virtual void    updateDecoration() {};
    //!
    virtual void    setSelection(long from, long to);
    //!
    virtual void    removeSelection();
    //!
    virtual void    repaintSelection() {};

    const AreaSelection& selection() const { return selection_; }

private:
    const Area*     area_;
protected:
    AreaSelection   selection_;
};

class FORMATTER_EXPIMP AreaViewFactory {
public:
    FORMATTER_OALLOC(AreaViewFactory);
    virtual ~AreaViewFactory() {}

    virtual AreaView*   makeAreaView(const Area* area) const = 0;
    //!
    virtual void        disablePerPageResizing() = 0;
};

}   // namespace Formatter

#endif      // FORMATTER_AREA_VIEW_H
