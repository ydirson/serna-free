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

#include "formatter/impl/debug.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/Fo.h"
#include <iomanip>

#include "common/safecast.h"
#include "common/Singleton.h"

#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"

#include <map>

USING_GROVE_NAMESPACE;
USING_COMMON_NS;

namespace Formatter {

const char* fo_names[] = {
    //! Pagination and Layout formatting objects
    "root",
    "page-sequence",
    "page-sequence-master",
    "single-page-master-reference",
    "repeatable-page-master-reference",
    "layout-master-set",
    "simple-page-master",
    "region-body",
    "declarations",
    "flow",

    //! Block formatting objects
    "block",

    //! Inline formatting objects
    "external-graphic",
    "inline",
    "leader",
    "page-number",
    "page-number-citation",

    //! Table formatting objects
    "table-and-caption",
    "table",
    "table-column",
    "table-caption",
    "table-header",
    "table-footer",
    "table-body",
    "table-row",
    "table-cell",

    //! List formatting objects
    "list-block",
    "list-item",
    "list-item-label",
    "list-item-body",

    //! Syntext extentions
    "text",
    "line",
    "section", // groveSection start/end
    "choice",
    "comment",
    "pi",
    "fold",
    "combo-box",
    "line-edit",
    "inline-object",
    //! Unknown or unsupported formatting objects
    "unknown"
};

/*
 */
class FoNameMap {
public:
    FORMATTER_OALLOC(FoTable);
    typedef std::map<String, FoName> NameMap;

    FoNameMap()
    {
        const int length = sizeof(fo_names)/sizeof(char*);
        for (int i = 0; i < length; i++)
            nameMap_[fo_names[i]] = FoName(i);
    }

    FoName  foName(const Node* const node) const
    {
        const Node::NodeType type = node->nodeType();
        if (Node::TEXT_NODE == type)
            return TEXT;
        if (Node::CHOICE_NODE == type)
            return CHOICE;
        if (Node::ELEMENT_NODE != type)
            return UNKNOWN;
        //! TODO: check namespace (to be XSL Fo)
        const Element* const e = static_cast<const Element*>(node);
        NameMap::const_iterator i = nameMap_.find(e->localName().lower());
        DBG(XSL.FO) << e->localName() << '=' << (*i).second << std::endl;
        if (nameMap_.end() == i)
            return UNKNOWN;
        return (*i).second;
    }

private:
    NameMap nameMap_;
};

/*
 */
void Allocation::dump() const
{
    DBG(XSL.LAYOUT)
        << "space x:" << fix_fmt(space_.origin_.x_)
        << " y:" << fix_fmt(space_.origin_.y_)
        << " dx:" << fix_fmt(space_.extent_.w_);
    if (maxw_)
        DBG(XSL.LAYOUT) << "(max)";

    if (CTYPE_MAX == space_.extent_.h_)
        DBG(XSL.LAYOUT) << " dy:CTYPE_MAX";
    else
        DBG(XSL.LAYOUT) << " dy:" << fix_fmt(space_.extent_.h_);

    if (maxh_)
        DBG(XSL.LAYOUT) << "(max)";
    if (!enoughW_)
        DBG(XSL.LAYOUT) << " <not enough W>";
    if (!enoughH_)
        DBG(XSL.LAYOUT) << " <not enough H>";
    DBG(XSL.LAYOUT) << std::endl;
}

/*
 */
FoName Fo::foName(const Node* node)
{
    return SingletonHolder<FoNameMap>::instance().foName(node);
}

ContentType Fo::contentType(const Node* node)
{
    // NOTE: ALL THE FALLTHROUGHS ARE INTENTIONAL
    switch (foName(node)) {

        case EXTERNAL_GRAPHIC:
        case INLINE:
        case LEADER:
        case PAGE_NUMBER:
        case PAGE_NUMBER_CITATION:
        case TEXT:
        case SECTION_CORNER:
        case CHOICE:
        case COMMENT:
        case PI:
        case FOLD:
        case COMBO_BOX:
        case LINE_EDIT:
        case INLINE_OBJECT:
        case UNKNOWN:
            return INLINE_CONTENT;
            break;

        case BLOCK:
        case LINE:
        case TABLE_AND_CAPTION:
        case TABLE:
        case TABLE_CAPTION:
        case LIST_BLOCK:
        case LIST_ITEM:
        case LIST_ITEM_LABEL:
        case LIST_ITEM_BODY:
            return BLOCK_CONTENT;
            break;

        case TABLE_COLUMN:
        case TABLE_HEADER:
        case TABLE_FOOTER:
        case TABLE_BODY:
        case TABLE_ROW:
        case TABLE_CELL:
            return TABLE_CONTENT;
            break;

        case ROOT:
        case DECLARATIONS:
        case PAGE_SEQUENCE:
        case LAYOUT_MASTER_SET:
        case PAGE_SEQUENCE_MASTER:
        case SINGLE_PAGE_MASTER_REFERENCE:
        case REPEATABLE_PAGE_MASTER_REFERENCE:
        case SIMPLE_PAGE_MASTER:
        case REGION_BODY:
        case FLOW:
        //! Not yet supported
        default:
            return IGNORED;
    }
    return IGNORED;
}

Fo::~Fo()
{
    for (Area* area = firstChunk(); area; area = firstChunk())
        area->remove();
}

void Fo::registerPropertyModification()
{
    DBG(XSL.PROPDYN)
        << "Registering property modification: " << this << std::endl;
    registerModification(THIS_FO_MODIFIED);
}

void Fo::removeFo()
{
    DBG(XSL.CHAIN) << "Removing: " << name() << " :" << this << std::endl;
    deregisterModification();
    detachFromFormatter(0);
    for (Area* area = firstChunk(); area; area = firstChunk()) {
        /*TODO: check whether this is hack or not,
          maybe this line should present in LayoutImpl in chain disposal,
          maybe we should disambeguate remove scope */
        area->dispose();
    }
    remove();
}

void Fo::insertChildFo(Fo* childFo, const Node* childNode)
{
    const Node* next = childNode->nextSibling();
    Fo* next_fo = 0;
    while (next) {
        next_fo = getFoByNode(next);
        if (0 == next_fo)
            next = next->nextSibling();
        else
            break;
    }
    if (next_fo)
        next_fo->insertBefore(childFo);
    else
        appendChild(childFo);
    childFo->registerModification(THIS_FO_MODIFIED);
}

void Fo::removeChildFo(Fo* child)
{
    RT_MSG_ASSERT((this == child->parent()),
                  "Removed Fo is not immediate child of this Fo");
    if (child->nextSibling())
        child->nextSibling()->registerModification(PREV_FO_MODIFIED);
    else
        if (child->prevSibling()) {
            /*! TODO: register child->prevSibling() to construct its
             FoController with the state "FINAL" and lastArea().

             or use prevSibling()->registerModification(NEXT_FO_MODIFIED);

             This will make it possible to update the geometry of the
             area without remaking the child->prevSibling() itself
            */
            Fo* fo = child->prevSibling();
            while (fo->lastChild())
                fo = fo->lastChild();
            fo->registerModification(THIS_FO_MODIFIED);
        }
        else
            registerModification(THIS_FO_MODIFIED);
    child->removeFo();
}

void Fo::checkProperties(const Allocation& alloc)
{
    DBG(XSL.CHAIN) << "Fo: Checking Properties: " << name()
                   << " :" << this << std::endl;
    PropertySet::checkProperties(alloc);
}

bool Fo::hasContent(const AreaPtr& area) const
{
    return !area->empty();
}

CPoint Fo::getContPoint(const Area* area, bool) const
{
    return area->contPoint();
}

ulong Fo::chainPosCount() const
{
    if (!hasChildChains()) {
        uint count = 1;
        for (const Area* chunk = firstChunk(); chunk;
             chunk = chunk->nextChunk())
            count += chunk->chunkPosCount() - 1;
        return count;
    }
    return countChildren() + 1;
}

Chain* Fo::commonAos(const Chain* chain) const
{
    const Fo* fo = static_cast<const Fo*>(chain);
    if (this == fo)
        return const_cast<Fo*>(fo);
    return FTN::commonAos(fo);
}

Chain* Fo::chainAt(ulong chainPos) const
{
    return getChild(chainPos);
}

void Fo::treeloc(COMMON_NS::TreelocRep& tloc) const
{
    treeLoc(tloc);
}

Chain* Fo::byTreeloc(COMMON_NS::TreelocRep& tloc) const
{
    return getByTreeloc(tloc);
}

void Fo::getTreeloc(COMMON_NS::TreelocRep& tloc) const
{
    get_fo_node_treeloc(headFoNode(), tloc);
}

void Fo::dump(int indent) const
{
    DINDENT(indent);
    DBG(XSL.CHAIN)
        << " " << name() << "<" << headFoNode()->nodeName() << ">:"
        << this << " p:" << parent() << " node:" << headFoNode()
        << " origin:" << XslFoExt::origin(headFoNode())
        << ((hasParentOrigin()) ? ("(same as parent)") : (" ")) << std::endl;
    Chain::dump(4);
    PropertySet::dump(4);
    for (Fo* child = firstChild(); child; child = child->nextSibling())
        DBG_IF(XSL.CHAIN) child->dump(4);
}

} // namespace Formatter
