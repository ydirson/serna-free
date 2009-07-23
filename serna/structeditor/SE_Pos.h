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
#ifndef SE_POS_H_
#define SE_POS_H_

#include "structeditor/se_defs.h"
#include "common/DiffuseSearch.h"

namespace GroveLib {
    class Node;
}
namespace GroveEditor {
    class GrovePos;
    class GroveSelection;
}
namespace Formatter {
    class Chain;
}

STRUCTEDITOR_EXPIMP const GroveLib::Node*   
find_result(const GroveLib::Node* src, const GroveLib::Node* foHint, 
            bool diffuse);

STRUCTEDITOR_EXPIMP const GroveLib::Node*   
fo_hint(const GroveEditor::GrovePos& foPos);

STRUCTEDITOR_EXPIMP Formatter::Chain*   
get_formatted_chain(const GroveLib::Node* foNode, EditableView& view);

STRUCTEDITOR_EXPIMP GroveEditor::GrovePos 
to_result_pos(const GroveEditor::GrovePos& srcPos, 
              const GroveEditor::GrovePos& foHint, 
              bool diffuse, bool isForward = true);

STRUCTEDITOR_EXPIMP GroveEditor::GrovePos 
to_result_pos(const GroveEditor::GrovePos& srcPos, 
              const Common::XTreeIterator<GroveLib::Node>& foIter, 
              bool diffuse, bool isForward = true);

STRUCTEDITOR_EXPIMP Common::XTreeIterator<GroveLib::Node>
xtree_iterator(const GroveEditor::GrovePos& pos);

STRUCTEDITOR_EXPIMP Formatter::ChainSelection
get_chain_selection(const GroveEditor::GroveSelection& src, 
                    const GroveEditor::GrovePos& foHint, 
                    const StructEditor* se);

STRUCTEDITOR_EXPIMP GroveEditor::GrovePos 
adjust_to_text_pos(const GroveEditor::GrovePos& srcPos);

#endif
