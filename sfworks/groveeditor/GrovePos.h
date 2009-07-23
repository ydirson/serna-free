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
 *
 ***********************************************************************/

#ifndef GROVEEDITOR_GROVE_POS_H
#define GROVEEDITOR_GROVE_POS_H

#include "groveeditor/groveeditor_defs.h"
#include "grove/Nodes.h"
#include "grove/Decls.h"
#include "common/common_types.h"
#include "common/Vector.h"

namespace GroveEditor
{

//
// Meaning of GrovePos:
// if TEXT_POS, text() corresponds to text node, idx() to offset
// if ELEMENT_POS, node() corresponds to parent node, before() corresponds
// to target node.
//
class GROVEEDITOR_EXPIMP GrovePos {
public:
    enum Type { UNKNOWN_POS, ELEMENT_POS, TEXT_POS };

    // Construct NULL GrovePos
    GrovePos();

    // Constructs ELEMENT_POS
    GrovePos(const GroveLib::Node* node,
             const GroveLib::Node* before);
    GrovePos(const GroveLib::Node* node);

    // Constructs TEXT_POS
    GrovePos(const GroveLib::Text* text, long idx);

    // Constructs ELEMENT_POS or TEXT_POS, depending on node type
    GrovePos(const GroveLib::Node* node, long idx);

    // Constructs GrovePos from text representation
    GrovePos(const COMMON_NS::String&,
             const GroveLib::Node* root,
             bool exact = true);

    bool            isNull() const { return !node_ || type() == UNKNOWN_POS; }

    Type            type() const;

    // These are returned for ELEMENT_POS
    GroveLib::Node* node() const { return node_; }
    GroveLib::Node* before() const;

    // These are returned for TEXT_POS
    GroveLib::Text* text() const;
    long            idx() const;

    // Returns available most specific context node
    GroveLib::Node* contextNode() const;

    // Returns ERS for the current position
    GroveLib::EntityReferenceStart* getErs() const;
    GroveLib::RedlineSectionStart*  getRedlineStart() const;

    // Convert boundary text position to element position
    GrovePos        adjustBoundaryPos() const;

    // Returns text representation of GrovePos
    COMMON_NS::String asString() const;

    // Insert node at current position. Returns original node.
    GroveLib::Node* insert(GroveLib::Node*) const;

    // Compares element position only
    bool            compareElemPos(const GrovePos& other) const;

    // Test for choice node and adjust position, if necessary
    GrovePos        adjustChoice() const;

    void            dump() const;

    bool            operator==(const GrovePos& other) const;
    bool            operator!=(const GrovePos& other) const;
    bool            operator<(const GrovePos& other) const;
    bool            operator<=(const GrovePos& other) const;
    bool            operator>(const GrovePos& other) const;
    bool            operator>=(const GrovePos& other) const;

private:
    void            checkSectStart();
    void            makeElementPos(const GroveLib::Node*,
                                   const GroveLib::Node*);

    GroveLib::Node* node_;
    GroveLib::Node* before_;
    long            beforeIdx_;
};

///////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP GroveSelection {
public:
    enum Status {
        IS_VALID_SELECTION      = 0x00,
        IS_EMPTY_SELECTION      = 0x01,
        IS_NULL_SELECTION       = 0x02,
        IS_ATTRIBUTE_SELECTION  = 0x04,
        IS_CROSS_SECTION        = 0x08,
    };
    
    GroveSelection();
    GroveSelection(const GrovePos& pos);
    GroveSelection(const GrovePos& start, const GrovePos& end);
    GroveSelection(const GroveSelection& r);

    //!
    bool            isEmpty() const { return start_ == end_; }
    //!
    bool            isNull() const { return start_.isNull() || end_.isNull(); }
    //!
    Status          status() const;
    //!
    bool            operator==(const GroveSelection& s) const;
    //!
    bool            operator!=(const GroveSelection& s) const;
    //! 
    void            clear() { start_ = end_ = GrovePos(); }
    //! Updates the end_ corner of the selection
    void            extendTo(const GrovePos& pos) { 
        if (!start_.isNull()) 
            end_ = pos; 
    }
    //! Returns the begining of the region
    const GrovePos& start() const { return start_; }
    //! Returns the end of the region
    const GrovePos& end() const { return end_; }
    //! Returns the lower corner
    const GrovePos& minPos() const;
    //! Returns the higher corner
    const GrovePos& maxPos() const;
    //!
    void            balance();

    void            dump() const;
private:
    GrovePos        start_;
    GrovePos        end_;
};

///////////////////////////////////////////////////////////////////////////

class GrovePosList: public COMMON_NS::Vector<GrovePos> {};
class NodePosList:  public COMMON_NS::Vector<GroveLib::NodePtr> {};

// Obtain replicated position list
GROVEEDITOR_EXPIMP bool get_pos_list(const GrovePos& srcPos, 
                                     GrovePosList& resultList, 
                                     bool eext = 0);

GROVEEDITOR_EXPIMP bool get_pos_list(const GroveLib::NodePtr& srcNode,
                                     NodePosList& resultList,
                                     bool eext = 0);

GROVEEDITOR_EXPIMP bool is_within_range(const GrovePos& pos,
                                        const GrovePos& from,
                                        const GrovePos& to,
                                        bool checkReplication = false);

GROVEEDITOR_EXPIMP GroveLib::Grove* get_current_grove(const GroveLib::Node*);

} // namespace GroveEditor

typedef COMMON_NS::RefCntPtr<GroveEditor::GrovePos>      GrovePosPtr;

#endif // GROVEEDITOR_GROVE_POS_H
