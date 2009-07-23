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
#include "structeditor/ElementList.h"
#include "structeditor/impl/XsUtils.h"
#include "groveeditor/GrovePos.h"
#include "utils/GrovePosEventData.h"
#include "editableview/EditableView.h"
#include "xs/XsNodeExt.h"
#include <iostream>

using namespace Common;

ElementList::ElementList(StructEditor* se)
    : se_(se), modified_(false), isAny_(false)
{
}

bool ElementList::reload(const GroveEditor::GrovePos* newPos)
{
    if (!se_->schema())
        return true;
    if (updatePos(newPos) || modified_)
        return get_list(*this, false);
    return false;
}

bool ElementList::getRenameSet(PtnSet& ps)
{
    updatePos(0);
    return get_list(ps, true);
}

bool ElementList::updatePos(const GroveEditor::GrovePos* newPos)
{
    GroveEditor::GrovePos from, to;
    if (newPos)
        from = *newPos;
    else {
        if (se_->editableView().getSelection().src_.isEmpty())
            from = se_->editViewSrcPos();
        else
            se_->getSelection(from, to, StructEditor::SILENT_OP);
    }
    bool need_update = !from.compareElemPos(pos_);
    if (!from.isNull() && 
        (from.node()->nodeType() & GroveLib::Node::SECT_NODE_MASK))
            pos_ = GroveEditor::GrovePos();
    else
        pos_ = from;
    return need_update;
}

bool ElementList::get_list(PtnSet& to, bool rename)
{
    GroveLib::Element* elem = traverse_to_element(pos_);
    GroveLib::Node* node = 0;
    to.clear();
    modified_ = false;
    if (pos_.isNull())
        return false;
    if (rename || pos_.type() == GroveEditor::GrovePos::TEXT_POS)
        node = pos_.node();
    else
        node = pos_.before();
    PropertyTree raw_elem_list;
    bool is_any = 
        XsNodeExt::getList(raw_elem_list.root(), node, pos_.node(), rename);
    PropertyNode* eprop;
    while ((eprop = raw_elem_list.root()->firstChild())) {
        String name = eprop->name();
        if (elem && !eprop->getString().isEmpty()) { // (getString() == URI)
            String prefix = elem->getPrefixByXmlNs(eprop->getString());
            if (!prefix.isEmpty())
                eprop->setName(prefix + ':' + name);
        }
        to.insert(eprop);
        eprop->remove();
    }
    return is_any;
}

bool ElementList::canInsert(const Common::String& elemName,
                            const Common::String& ns,
                            const GroveEditor::GrovePos* newPos)
{
    if (reload(newPos))
        return true;
    PropertyNodePtr pn = new PropertyNode(elemName, ns);
    return find(pn) != end();
}

bool ElementList::fillElemList(PropertyNode* attachTo)
{
    bool is_any = reload(0);
    for (const_iterator it = begin(); it != end(); ++it)
        attachTo->appendChild((*it)->copy());
    return is_any;
}

ElementList::~ElementList()
{
}
