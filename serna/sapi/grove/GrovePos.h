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
#ifndef SAPI_GROVE_POS_H_
#define SAPI_GROVE_POS_H_

#ifdef BUILD_SAPI
# include "groveeditor/GrovePos.h"
#endif // BUILD_SAPI

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/grove/GroveDecls.h"

namespace SernaApi {

/// GrovePos denotes a position in the XML tree (grove). 
/*! There are two kinds
 *  of GrovePos: TEXT_POS and ELEMENT_POS. For TEXT_POS, position consists of
 *  reference to text node and offset in text; for ELEMENT_POS, position
 *  consists of parent node and "before" node (position in tree is assumed
 *  just before "before" node; of before node is NULL, then position is right
 *  after the last child of the parent node).
 */
class SAPI_EXPIMP GrovePos {
public:
    enum Type { UNKNOWN_POS, ELEMENT_POS, TEXT_POS };

    /// Construct NULL GrovePos
    GrovePos();

    /// Constructs ELEMENT_POS
    GrovePos(const GroveNode& node);
    /// Constructs ELEMENT_POS
    GrovePos(const GroveNode& node,
             const GroveNode& before);

    /// Constructs TEXT_POS (idx means offset in text, starting from 0)
    GrovePos(const GroveText& text, long idx);

    /// Constructs GrovePos from text representation (path starting at \a root)
    GrovePos(const SString&, const GroveNode& root);

    /// Returns text representation of GrovePos (like "xpath path")
    SString         asString() const;
    /// Returns true if position is not initialized
    bool            isNull() const;
    /// Return type of grove position, see Type enum
    Type            type() const;

    /// These are returned for ELEMENT_POS
    GroveNode       node() const;
    /// These are returned for ELEMENT_POS
    GroveNode       before() const;

    /// These are returned for TEXT_POS
    GroveText       text() const;
    /// These are returned for TEXT_POS
    long            idx() const;

    /// Get current entity context for this position
    GroveErs        getErs() const;

    /// Dump GrovePos
    void            dump() const;

    ////////////////////////////////////////////////////////////////

#ifdef BUILD_SAPI
    GrovePos(const ::GroveEditor::GrovePos&);
    operator ::GroveEditor::GrovePos() const;
#endif // BUILD_SAPI

private:
    char            pvt_[3 * sizeof(void*)];
};

} // namespace SernaApi

#endif // SAPI_GROVE_POS_H_

