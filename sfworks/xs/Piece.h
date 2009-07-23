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

#ifndef _PIECE_H_
#define _PIECE_H_

#include "xs/xs_defs.h"
#include "xs/Origin.h"
#include "xs/Appinfo.h"
#include "xs/XsId.h"
#include "common/RefCounted.h"
#include "common/Message.h"

class Schema;

XS_NAMESPACE_BEGIN

class List;
class Restriction;
class SetNoMsg;
/*! An unnamed piece of a complex schema construct
 */
class XS_EXPIMP Piece : public COMMON_NS::RefCounted<>, public XsId {
public:
    const GroveLib::Element* origin() const;

    /*! Extracted <appinfo> element
     */
    const Appinfo*  appinfo() const;

    Piece* parent() const { return parent_; }

    COMMON_NS::String customMessage() const;
    COMMON_NS::
    String format() const;

    Piece(const Origin& o, const XsId& xsid = XsId());
    virtual ~Piece();

    virtual void dump(int) const{}

    virtual void putIntoTable(Schema* schema,
                              const COMMON_NS::String& id) const;

    XS_OALLOC(Piece);

private:
    friend class ::GroveAstParser;
    friend class List;
    friend class Restriction;
    friend class SetNoMsg;
    Piece();
    void setAppinfo(COMMON_NS::RefCntPtr<Appinfo>&  appinfo);
    void setParentPiece(Piece* parent);
    void setCustomMessage(COMMON_NS::String& errmsg);
    bool checkCustomMessage() const;
    const GroveLib::Element*          origin_;
    COMMON_NS::RefCntPtr<Appinfo>     appinfo_;
    Piece*                            parent_;
    COMMON_NS::String                 errmsg_;
    bool                              noUserMsg_;
};

class SetNoMsg {
public:
    SetNoMsg(Piece* p)
        : p_(p)
        {p_->noUserMsg_ = true;}
    ~SetNoMsg(){if (p_) p_->noUserMsg_ = false;}
private:
    Piece* p_;
};

XS_NAMESPACE_END

#endif // _PIECE_H_
