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
#include "groveeditor/GroveCommands.h"
#include "groveeditor/RedlineCommands.h"
#include "grove/SectionSyncher.h"
#include "grove/Nodes.h"
#include "grove/udata.h"
#include "groveeditor/groveeditor_debug.h"

using namespace GroveLib;
using namespace Common;

#define RDBG DBG(GROVEEDITOR.REDLINE)

namespace GroveEditor {

#ifndef QT_TRANSLATE_NOOP
# define QT_TRANSLATE_NOOP(context, text) text
#endif // QT_TRANSLATE_NOOP

static void knock_xslt(Node* n, Node* last, bool needTick = false)
{
    if (n && needTick) {
        n->insertAfter(new Comment(""));
        n->nextSibling()->remove();
    }
    while (n) {
        set_node_modified(n, true);
        if (n == last)
            break;
        n = n->nextSibling();
    }
}

NewRedline::NewRedline(Node* first, Node* last, 
                       const GrovePos& origPos, RedlineSectionStart* rs)
    : first_(first), last_(last), origPos_(origPos), redlineSect_(rs)
{
}

void NewRedline::doExecute()
{
    DBG(GROVEEDITOR.SECT) << "ConvertToRedline: before:\n";
    DBG_IF(GROVEEDITOR.SECT) GroveSectionStart::dumpSectionTree
        (first_->getGSR()->rss());
    GroveSectionStart::convertToSection(Node::REDLINE_SECTION,
        first_.pointer(), last_.pointer(),
        redlineSect_.pointer(), savedGse_.pointer());
    savedGse_ = redlineSect_->getSectEnd();
    Node* next_pos = savedGse_->nextSibling();
    if (next_pos && next_pos->nodeType() == Node::TEXT_NODE)
        suggestedPos_ = GrovePos(static_cast<Text*>(next_pos), 0);
    else
        suggestedPos_ = GrovePos(savedGse_->parent(), next_pos);
    setEntityModified(origPos_);
    knock_xslt(first_.pointer(), last_.pointer());
    DBG(GROVEEDITOR.SECT) << "ConvertToRedline: after:\n";
    DBG_IF(GROVEEDITOR.SECT) GroveSectionStart::dumpSectionTree
        (redlineSect_->getGSR()->rss());
}
    
void NewRedline::doUnexecute()
{
    redlineSect_->convertFromSection();
    suggestedPos_ = origPos_;
    unsetEntityModified(origPos_);
    knock_xslt(first_.pointer(), last_.pointer());
}

String NewRedline::info(uint) const
{
    return String::null();
}

NewRedlineSelection::NewRedlineSelection(const GrovePos& first,
                                         const GrovePos& last,
                                         RedlineData* rd)
    : SplitSelectionCommand(first, last),
      posCmd_(0)
{
    first.node()->grove()->document()->ensureSectionRoot(Node::REDLINE_SECTION);
    redlineSect_ = new RedlineSectionStart(rd);
}

void NewRedlineSelection::makeSubcommand(GroveLib::Node* first,
                                         GroveLib::Node* last)
{
    if (redlineSect_)
        posCmd_ = add(new NewRedline(first, last, first_,
            redlineSect_.pointer()));
}

const GrovePos& NewRedlineSelection::pos() const
{
    if (posCmd_)
        return posCmd_->pos();
    else
        return GroveMacroCommand::pos();
}

String NewRedlineSelection::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Redline Selection");
        default: return String::null();
    }
}

//////////////////////////////////////////////////////////////////////////

static RedlineSectionStart* get_rss(const GrovePos& pos)
{
    return get_redline_section(pos.contextNode());
}

ConvertFromRedline::ConvertFromRedline(const GrovePos& pos)
    : pos_(pos)
{
    suggestedPos_ = pos_;
    RedlineSectionStart* rss = get_rss(pos);
    if (0 == rss)
        return;
    convFromSect_ = new ConvertFromSection(rss);
}

void ConvertFromRedline::doExecute()
{
    if (!convFromSect_)
        return;
    knock_xslt(convFromSect_->first(), convFromSect_->last());
    convFromSect_->execute();
    setEntityModified(pos_);
    suggestedPos_ = convFromSect_->pos();
}

void ConvertFromRedline::doUnexecute()
{
    if (!convFromSect_)
        return;
    convFromSect_->unexecute();
    knock_xslt(convFromSect_->first(), convFromSect_->last());
    suggestedPos_ = pos_;
    unsetEntityModified(pos_);
}

String ConvertFromRedline::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Remove Redlining");
        default: return String::null();
    }
}

ConvertFromRedline::~ConvertFromRedline()
{
}

//////////////////////////////////////////////////////////////////////////

static bool is_mine_rss(const RedlineSectionStart* my_rss,
                        const RedlineSectionStart* rss)
{
    if (rss->getGSR()->nodeType() == Node::DOCUMENT_FRAGMENT_NODE ||
        my_rss == rss) 
            return true;    // don't touch those in frags
    if (my_rss->grove() != rss->grove())  
        return false;
    const EntityReferenceStart* my_ers = get_ers(my_rss);
    const EntityReferenceStart* his_ers = get_ers(rss);
    if (his_ers != my_ers && his_ers->entityDecl() == my_ers->entityDecl())
        return true;    // replicated
    return false;
}

ChangeRedline::ChangeRedline(const GrovePos& pos, uint redlineMask,
                             const Common::String& annotation)
    : redlineMask_(redlineMask), 
      annotation_(annotation)
{
    suggestedPos_ = pos;
    RedlineSectionStart* rss = get_rss(pos);
    if (0 == rss)
        return;
    // split off redline data, if it is copied (not replicated)
    // Note that this needs not to be undoable.
    Vector<RedlineSectionStart*> rsvec;
    RedlineSectionList::iterator rit = rss->redlineData()->replicas().begin();
    for (; rit != rss->redlineData()->replicas().end(); ++rit) 
        if (!is_mine_rss(rss, rit))
            rsvec.push_back(rit);
    if (rsvec.size()) {
        RedlineData* split_rd = rss->redlineData()->copy();
        for (uint i = 0; i < rsvec.size(); ++i) {
            rss = rsvec[i];
            rss->CDLItem::remove();
            split_rd->replicas().push_back(rss);
            rss->setRedlineData(split_rd);
        }
    }
}

void ChangeRedline::doExecute()
{
    change_rinfo(true);
}

void ChangeRedline::doUnexecute()
{
    change_rinfo(false);
}

void ChangeRedline::change_rinfo(bool isDo)
{
    RedlineSectionStart* rss = get_rss(suggestedPos_);
    if (0 == rss)
        return;
    if (isDo)
        setEntityModified(suggestedPos_);    
    else
        unsetEntityModified(suggestedPos_);   
    uint rmask = rss->redlineData()->redlineMask();
    Common::String ann = rss->redlineData()->annotation();
    rss->redlineData()->setRedlineMask(redlineMask_);
    rss->redlineData()->setAnnotation(annotation_);
    redlineMask_ = rmask;
    annotation_  = ann;
    if (rss->getSectEnd() != rss->nextSibling())
        knock_xslt(rss, rss->getSectEnd(), true);
}           

String ChangeRedline::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Change Redlining");
        default: return String::null();
    }
}
    
ChangeRedline::~ChangeRedline()
{
}

/////////////////////////////////////////////////////////////////
    
InsertRedline::InsertRedline(const GrovePos& pos, GroveLib::RedlineData* rd)
    : pos_(pos)
{
    savedGss_ = new RedlineSectionStart(rd);
    savedGse_ = new RedlineSectionEnd;
    suggestedPos_ = pos;
}

void InsertRedline::doExecute()
{
    DBG(GROVEEDITOR.SECT) << "InsertRedline: before:\n";
    DBG_IF(GROVEEDITOR.SECT) GroveSectionStart::dumpSectionTree
        (pos_.node()->getGSR()->rss());
    NodePtr n = pos_.insert(new Comment(""));
    GroveSectionStart::convertToSection(Node::REDLINE_SECTION,
        n.pointer(), n.pointer(), savedGss_.pointer(), savedGse_.pointer());
    n->remove();
    setEntityModified(pos_);
    suggestedPos_ = GrovePos(savedGse_->parent(), savedGse_.pointer());
    DBG(GROVEEDITOR.SECT) << "InsertRedline: before:\n";
    DBG_IF(GROVEEDITOR.SECT) GroveSectionStart::dumpSectionTree
        (pos_.node()->getGSR()->rss());
}

void InsertRedline::doUnexecute()
{
    savedGss_->convertFromSection();
    unsetEntityModified(pos_);
    suggestedPos_ = pos_;
}

String InsertRedline::info(uint idx) const
{
    switch (idx) {
        case 0:  return QT_TRANSLATE_NOOP("GroveEditor", "Insert Redlining");
        default: return String::null();
    }
}

InsertRedline::~InsertRedline()
{
}

} // namespace GroveEditor
