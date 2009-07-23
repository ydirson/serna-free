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
#include "grove/grove_trace.h"
#include "grove/grove_defs.h"
#include "grove/RedlineUtils.h"
#include "grove/Nodes.h"
#include "grove/udata.h"
#include "grove/NodeExt.h"
#include "grove/impl/gsutils.h"
#include "common/Singleton.h"
#include <map>

#define REDLINE_START_PITARGET "serna-redline-start"
#define REDLINE_END_PITARGET   "serna-redline-end"

using namespace Common;

namespace GroveLib {

String RedlineData::asPiString() const
{
    String res = "<?" REDLINE_START_PITARGET " ";
    res += String::number(redlineMask(), 8);
    res += " ";
    res += annotation();
    res += "?>";
    return res;
}
    
void RedlineData::dump() const
{
#ifdef GROVE_DEBUG
    String res;
    if (redlineMask_ & (1 << REDLINE_UNDERLINE))
        res += " UNDERLINE";
    if (redlineMask_ & (1 << REDLINE_STRIKE_THRU))
        res += " STRIKE-THRU";
    DDBG << " RC: " << getRefCnt() 
        << " FONT-C: " << (int)(redlineMask_ & 07)
        << " HIGH-C: " << (int)((redlineMask_ >> REDLINE_HIGHLIGHT_COLOR) & 07)
        << res << " ANN: " << annotation_ << std::endl;
#endif
}

void RedlineData::setRedlineMask(uint v) 
{ 
    redlineMask_ = v; 
    notifyChanged();
}

void RedlineData::setAnnotation(const Common::String& s) 
{
    annotation_ = s; 
    notifyChanged();
}

void* RedlineData::redlineNotifyId()
{
    static int id_stub = 0;
    return &id_stub;
}

void RedlineData::notifyChanged()
{
    RedlineSectionList::iterator rsi = replicas_.begin();
    for (; rsi != replicas_.end(); ++rsi)
        rsi->genericNotify(redlineNotifyId());
}

RedlineData::~RedlineData()
{
}

/////////////////////////////////////////////////////////////////////////

class RedlineIdMap : public std::map<uint, RedlineSectionStart*> {
public:
    RedlineIdMap() :
        lastId_(0) {}
    uint    lastId_;
};    

typedef Common::SingletonHolder<RedlineIdMap,
                                Common::CreateUsingNew<RedlineIdMap>,
                                Common::PhoenixSingleton<RedlineIdMap> > RID; 

RedlineSectionStart* RedlineSectionStart::find(uint redlineId)
{
    RedlineIdMap& rm = RID::instance();
    RedlineIdMap::const_iterator it = rm.find(redlineId);
    return (it != rm.end()) ? it->second : 0;
}

static RedlineSectionStart* make_from_pi(const ProcessingInstruction* pi)
{
    // so, it is serna-redline-start
    uint mask = 0;
    const Char* cp = pi->data().unicode();
    const Char* ce = cp + pi->data().length();
    while (cp < ce && cp->isSpace())
        ++cp;
    const Char* cs = cp;
    while (cp < ce && cp->isDigit())
        ++cp;
    if (cs != cp)
        mask = String(cs, cp - cs).toUInt(0, 8);
    while (cp < ce && cp->isSpace())
        ++cp;
    String text(cp, ce - cp);
    return new RedlineSectionStart(new RedlineData(mask, text));
}

//////////////////////////////////////////////////////////////////////

GROVE_EXPIMP RedlineSectionStart* get_redline_section(const Node* n)
{
    return RedlineSectionStart::find(n->udata() & REDLINE_MASK);
}

RedlineData* RedlineSectionStart::redlineData() const 
{ 
    return redlineData_.pointer(); 
}

int RedlineSectionStart::memSize() const
{
    return node_size() + sizeof(*this);
}

//////////////////////////////////////////////////////////////////////

RedlineParserTool::RedlineParserTool(RedlineSectionStart* rs)
    : rss_(rs)
{
}

void RedlineParserTool::processPi(ProcessingInstruction* pi)
{
    if (pi->target() == REDLINE_START_PITARGET) {
        RedlineSectionStart* rs = make_from_pi(pi);
        rss_->appendSect(rs);
        rss_ = rs;
        pi->insertBefore(rs);
        pi->remove();
    } else if (pi->target() == REDLINE_END_PITARGET && 
        rss_->getSectParent() && parentNode(rss_) == parentNode(pi)) {
            RedlineSectionEnd* re = new RedlineSectionEnd;
            rss_->setSectEnd(re);
            re->setSectStart(rss_);
            rss_ = static_cast<RedlineSectionStart*>(rss_->getSectParent());
            pi->insertBefore(re);
            pi->remove();
    }
}

void RedlineParserTool::processElementEnd(Node* n)
{
    while (rss_->getSectParent() && 
        parentNode(rss_) == n && !rss_->getSectEnd()) {
            RedlineSectionEnd* re = new RedlineSectionEnd;
            re->setSectStart(rss_);
            rss_->setSectEnd(re);
            n->appendChild(re);
            rss_ = static_cast<RedlineSectionStart*>(rss_->getSectParent());
    }
}

void RedlineParserTool::finalize()
{
}

//////////////////////////////////////////////////////////////////////
    
RedlineSectionStart::RedlineSectionStart(RedlineData* rd)
    : GroveSectionStart(REDLINE_START_NODE), 
      redlineData_(rd)
{
    if (rd) {
        rd->replicas().push_back(this);
        assignRedlineId();
    } else
        redlineId_ = 0;
}

void RedlineSectionStart::assignRedlineId()
{
    RedlineIdMap& rm = RID::instance();
    redlineId_ = ++rm.lastId_;
    rm[redlineId_] = this;
}

void RedlineSectionStart::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << asString();
}

const String& RedlineSectionStart::nodeName() const
{
    static String node_name("#redline-start");
    return node_name;
}

void RedlineSectionStart::setRedlineData(RedlineData* rd)
{
    redlineData_ = rd;
}

void RedlineSectionStart::copy_sect_start(const GroveSectionStart* other,
                                          Node*)
{
    const RedlineSectionStart* n =
        static_cast<const RedlineSectionStart*>(other);
    if (n->redlineData()) 
        n->redlineData()->replicas().push_back(this);
    redlineData_ = n->redlineData();
    assignRedlineId();
}

void RedlineSectionStart::dumpInherited() const
{
#ifdef GROVE_DEBUG
    GroveSectionStart::dumpSectionInfo();
    DDBG << "RID=" << redlineId_ << " ";
    if (redlineData())
        redlineData()->dump();
#endif // GROVE_DEBUG
}

String RedlineSectionStart::asString() const
{
    return redlineData()->asPiString();
}

RedlineSectionStart::~RedlineSectionStart()
{
    if (redlineId_ > 0)
        RID::instance().erase(redlineId_);
}

///////////////////////

void RedlineSectionEnd::saveAsXml(GroveSaverContext& gsc, int) const
{
    gsc.os() << asString();
}

const String& RedlineSectionEnd::nodeName() const
{
    static String node_name("#redline-end");
    return node_name;
}

String RedlineSectionEnd::asString() const
{
    return "<?" REDLINE_END_PITARGET "?>";
}

int RedlineSectionEnd::memSize() const
{
    return node_size() + sizeof(*this);
}

void RedlineSectionEnd::dumpInherited() const
{
#ifdef GROVE_DEBUG
    GroveSectionEnd::dumpSectionInfo();
#endif // GROVE_DEBUG
}

} // namespace GroveLib
