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

#include "grove/grove_defs.h"
#include "grove/grove_trace.h"
#include "grove/GroveSection.h"
#include "grove/EntityDeclSet.h"
#include "grove/Nodes.h"

GROVE_NAMESPACE_BEGIN

GroveSectionStart::~GroveSectionStart()
{
    if (sectEnd_)
        sectEnd_->setSectStart(0);
    sectEnd_ = 0;
}

Node* GroveSectionStart::copy(Node* newParent) const
{
    const Node::GroveSectionType sectId = 
        (Node::GroveSectionType)(nodeType() & SECT_TYPE_MASK);
    GroveSectionStart* n = GroveSectionRoot::makeSectionStart(sectId);
    n->assign(this, newParent);
    GroveSectionRoot* gsr = 0;
    if (newParent)
        gsr = newParent->getGSR();
    if (!gsr)
        return n;
    GroveSectionStart* sroot = gsr->ensureSectionRoot(sectId);
    n->copy_sect_start(this, newParent);
    sroot->appendSect(n);
    n->setSectParent(sroot);
    gsr->sections_[sectId] = n;
    return n;
}

Node* GroveSectionEnd::copy(Node* newParent) const
{
    const Node::GroveSectionType sectId = 
        (Node::GroveSectionType)(nodeType() & SECT_TYPE_MASK);
    GroveSectionEnd* n = GroveSectionRoot::makeSectionEnd(sectId);
    n->assign(this, newParent);
    GroveSectionRoot* gsr = 0;
    if (newParent)
        gsr = n->getGSR();
    if (!gsr)
        return n;
    n->setSectStart(gsr->sections_[sectId]);
    gsr->sections_[sectId]->setSectEnd(n);
    gsr->sections_[sectId] = gsr->sections_[sectId]->getSectParent();
    return n;
}

void GroveSectionStart::convertFromSection()
{
    NodePtr gssHolder(this);
    detachSection();
    GroveSectionStart* gs;
    while ((gs = getFirstSect())) {
        gs->sectRemove();
        insertSectBefore(gs);
    }
    sectRemove();
    remove();
    getSectEnd()->remove();
}

static void find_first_sect(Node::NodeType nodeType, Node* node, 
                            GroveSectionStart*& gss,
                            uint& level, uint& gsslevel)
{
    ++level;
    if (node->nodeType() == nodeType && level < gsslevel) {
        gss = static_cast<GroveSectionStart*>(node);
        gsslevel = level;
    }
    for (node = node->firstChild(); node; node = node->nextSibling())
        find_first_sect(nodeType, node, gss, level, gsslevel);
    --level;
}

GroveSectionStart* 
GroveSectionStart::convertToSection(GroveSectionType type, 
                                    Node* from, Node* to,
                                    GroveSectionStart* gss,
                                    GroveSectionEnd* gse)
{
    if (0 == to)
        to = from;
    if (!check_sections(from, to))
        return 0;
    if (0 == gss)
        gss = GroveSectionRoot::makeSectionStart(type);
    if (0 == gse)
        gse = GroveSectionRoot::makeSectionEnd(type);
    gss->setSectEnd(gse);
    gse->setSectStart(gss);
    GroveSectionStart* fgss = 0, *ngss = 0;
    uint level = 0, gsslevel = ~0;

    for (Node* node = from; node; node = node->nextSibling()) {
        find_first_sect(section_start_node_type_map[type],
                        node, fgss, level, gsslevel);
        if (node == to)
            break;
    }
    while (fgss) {
        ngss = static_cast<GroveSectionStart*>(fgss->getNextSect());
        if (ngss && ngss->parent() != fgss->parent())
            ngss = 0;
        fgss->sectRemove();
        gss->appendSect(fgss);
        fgss = ngss;
    }
    from->insertToEnclosingSection(gss);
    from->insertBefore(gss);
    to->insertAfter(gse);
    gss->attachSection();
    return gss;
}

GROVE_EXPIMP bool check_sections(const Node* n, const Node* endNode)
{
    if (n->parent() != endNode->parent())
        return false;
    int sectcnt = 0;
    for (;;) {
        if (0 == n)
            return false;
        if (n->nodeType() & Node::SECT_NODE_MASK) {
            if (n->nodeType() & Node::SECT_END_BIT)
                --sectcnt;
            else    
                ++sectcnt;
        }
        if (n == endNode)
            return !sectcnt;
        n = n->nextSibling();
    }
    return false;
}

#ifdef GROVE_DEBUG
using COMMON_NS::ind;
void GroveSectionStart::dumpSectionInfo() const
{
    uint nc = 0;
    GroveSectionStart* gss =
        const_cast<GroveSectionStart*>(this)->getFirstSect();
    for (; gss; gss = gss->getNextSect())
        ++nc;
    DDBG << "end:" << sectEnd_ << " parent:" << getSectParent()
         << " children:" << nc << " next: " << getNextSect() << std::endl;
}

void GroveSectionStart::dumpSectionTree(GroveSectionStart* n)
{
//    DINDENT(indent);
    n->dump();
    DDINDENT;
    GroveSectionStart* gss = n->getFirstSect();
    for (; gss; gss = gss->getNextSect())
        dumpSectionTree(gss);
}

void GroveSectionEnd::dumpSectionInfo() const
{
    DDBG << "start:" << sectStart_ << std::endl;
}

#else // GROVE_DEBUG

void GroveSectionStart::dumpSectionInfo() const
{
}

void GroveSectionStart::dumpSectionTree(GroveSectionStart*)
{
}

void GroveSectionEnd::dumpSectionInfo() const
{
}

#endif // GROVE_DEBUG

GROVE_NAMESPACE_END
