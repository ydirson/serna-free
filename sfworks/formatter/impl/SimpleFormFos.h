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

#ifndef FORMATTER_FORM_FOS_H
#define FORMATTER_FORM_FOS_H

#include "formatter/impl/TerminalFos.h"

namespace Formatter {

/////////////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP SimpleFormFo : public TerminalFo {
public:
    SimpleFormFo(const FoInit& init)
        : TerminalFo(init) {}

    bool            isChainFinishedAt(const AreaPtr& area) const;
    bool            hasContent(const AreaPtr& area) const;

protected:
    //!
    void            calcProperties(const Allocation& alloc);

//FO DYNAMICS
    virtual void    attributeChanged(const GroveLib::Attr*);
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*);
    virtual void    attributeAdded(const GroveLib::Attr*);

protected:
    CType           width_;
    bool            isEnabled_;
    Common::String  text_;
};

/////////////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP ComboBoxFo : public SimpleFormFo {
public:
    FORMATTER_OALLOC(ComboBoxFo);

    ComboBoxFo(const FoInit& init);

    FoType          type() const { return COMBO_BOX_FO; }
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    Common::String  name() const;

protected:
    //!
    void            calcProperties(const Allocation& alloc);

//FO DYNAMICS
    void            childInserted(const GroveLib::Node*);
    void            childRemoved(const GroveLib::Node*, const GroveLib::Node*);

protected:
    CType           accender_;
    CType           descender_;
    bool            isEditable_;
    Common::String  text_;
    Common::Vector<Common::String>  valueList_;
};

/////////////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP LineEditFo : public SimpleFormFo {
public:
    FORMATTER_OALLOC(LineEditFo);

    LineEditFo(const FoInit& init);

    FoType          type() const { return LINE_EDIT_FO; }
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    Common::String  name() const;

protected:
    //!
    void            calcProperties(const Allocation& alloc);

protected:
    CType           accender_;
    CType           descender_;
    bool            isEditable_;
    Common::String  text_;
};

} // namespace Formatter

#endif // FORMATTER_FORM_FO_H
