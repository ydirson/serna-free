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

#include "groveeditor/GrovePos.h"
#include "groveeditor/Exception.h"
#include "common/asserts.h"
#include "groveeditor/groveeditor_debug.h"
#include "grove/EntityReferenceTable.h"
#include "grove/SectionSyncher.h"
#include "grove/RedlineUtils.h"
#include "grove/Origin.h"

namespace GroveEditor
{

USING_COMMON_NS;
USING_GROVE_NAMESPACE;

///////////////////////////////////////////////////////////////////////////

static XTreeNodeCmpResult compare_pos(const GrovePos& p1, const GrovePos& p2)
{
    if (p1.node() == p2.node()) {
        if (GrovePos::TEXT_POS == p1.type()) {
            if (p1.idx() < p2.idx())
                return LESS;
            if (p1.idx() > p2.idx())
                return GREATER;
            return EQUAL;
        }
        if (p1.before() == p2.before()) 
            return EQUAL;
        if (p1.before() && p2.before())
            return p1.before()->comparePos(p2.before());
        if (p1.before()) //! p2.before() is NULL
            return LESS;
        return GREATER;
    }
    Node* aos = p1.node()->commonAos(p2.node());
    if (aos == p1.node()) {
        if (0 == p1.before())
            return GREATER;
        Node* n2 = p2.node();
        while (n2 && n2->parent() != aos)
            n2 = n2->parent();
        if (p1.before() == n2)
            return LESS;
        return p1.before()->comparePos(n2);
    }
    if (aos == p2.node()) {
        if (0 == p2.before())
            return LESS;
        Node* n1 = p1.node();
        while (n1 && n1->parent() != aos)
            n1 = n1->parent();
        if (p2.before() == n1)
            return GREATER;
        return n1->comparePos(p2.before());
    }
    return p1.node()->comparePos(p2.node());
}

///////////////////////////////////////////////////////////////////////////

GrovePos::GrovePos()
    : node_(0), before_(0), beforeIdx_(-1)
{
}

GrovePos::GrovePos(const GroveLib::Node* node,
                   const GroveLib::Node* before)
{
    makeElementPos(node, before);
}

GrovePos::GrovePos(const GroveLib::Node* node)
{
    makeElementPos(node, 0);
}

GrovePos::GrovePos(const GroveLib::Text* node, long idx)
{
    beforeIdx_ = idx;
    before_    = 0;
    node_      = const_cast<Text*>(node);
}

GrovePos::GrovePos(const GroveLib::Node* n, long idx)
{
    if (n->nodeType() == Node::TEXT_NODE)
        *this = GrovePos(static_cast<Text*>(const_cast<Node*>(n)), idx);
    else
        makeElementPos(n, n->getChild(idx));
}

GrovePos::Type GrovePos::type() const
{
    if (!node_)
        return UNKNOWN_POS;
    if (node_->nodeType() == Node::TEXT_NODE)
        return TEXT_POS;
    return ELEMENT_POS;
}

GrovePos::GrovePos(const COMMON_NS::String& sv,
                   const GroveLib::Node* root,
                   bool exact)
{
    const Char* cp = sv.unicode();
    const Char* ce = cp + sv.length();
    const Char *s = 0, *s1 = 0;
    makeElementPos(root, 0);
    Node* n;

    while (cp < ce && *cp == '/')
        ++cp;

    for (;;) {
        int  cnt = 1;
        if (cp >= ce)
            break;
        s = cp;
        while (cp < ce && *cp != '/' && *cp != '[')
            ++cp;
        if (cp >= ce || *cp == '/') {
            cnt = 0;
            s1 = cp;
            if (cp < ce)
                ++cp;
        } else {
            s1 = cp++;
            const Char* p = cp;
            while (p < ce  && p->isDigit())
                ++p;
            cnt = String(cp, p - cp).toInt();
            if (p < ce && *p == ']')
                ++p;
            if (p < ce && *p == '/')
                ++p;
            cp = p;
        }
        String ns(s, s1 - s);
        //std::cerr << "parsing token: " << ns << " cnt: " << cnt << std::endl;
        if (ns == NOTR("#text")) {
            if (cp < ce && *cp == '[') {
                const Char* p = ++cp;
                while (p < ce  && p->isDigit())
                    ++p;
                beforeIdx_ = String(cp, p - cp).toInt();
                n = node_->firstChild();
                for (; n; n = n->nextSibling())
                    if (Node::TEXT_NODE == n->nodeType())
                        --cnt;
                    if (cnt <= 0)
                        break;
                if (n)
                    node_ = n;
                else if (exact)
                    node_ = n;
            } 
            else {
                beforeIdx_ = cnt;
                for (n = node_->firstChild(); n; n = n->nextSibling())
                    if (n->nodeType() == Node::TEXT_NODE)
                        break;
                if (n)
                    node_ = n;
                else if (exact)
                    node_ = 0;
            }
            return;
        }
        n = node_->firstChild();
        if (0 == n) {
            if (exact)
                makeElementPos(0, 0);
            return;
        }
        do {
            for (; n && n->nodeName() != ns; n = n->nextSibling())
                ;
            if (0 == n)
                break;
            if (cnt > 1)
                n = n->nextSibling();
            --cnt;
        } while (cnt > 0);
        if (0 == n) {
            if (exact)
                makeElementPos(0, 0);
            return;
        }
        node_ = n;
    }
    makeElementPos(node_->parent(), node_);
}

COMMON_NS::String GrovePos::asString() const
{
    return nodePosToString(contextNode(), TEXT_POS == type() ? beforeIdx_ : -1);
}

void GrovePos::makeElementPos(const GroveLib::Node* parent,
                                 const GroveLib::Node* node)
{
    node_   = const_cast<Node*>(parent);
    before_ = const_cast<Node*>(node);
    beforeIdx_ = -1;
}

void GrovePos::dump() const
{
    if (isNull()) {
        DDBG << "GrovePos: NULL " << std::endl;
        return;
    }
    TreelocRep treeloc;
    if (before_)
        before_->treeLoc(treeloc);
    DDBG << "GrovePos: type " << (int)type() << " node_ " << node_
         << "<" << node_->nodeName() << "> "
         << " before " << before_
         << (before_ ? ("<" + before_->nodeName() + ">[" + 
                        treeloc.toString() + "]") : COMMON_NS::String())
         << " beforeIdx " << beforeIdx_ << std::endl;
}

Node* GrovePos::before() const
{
    if (!(ELEMENT_POS == type() || 
          (node() && Node::DOCUMENT_NODE == node()->nodeType()))) {
        //dump();
        //throw Exception(GroveEditorMessages::posError);
        return 0;
    }
    if (node() && 0 == before_ && -1 != beforeIdx_)
        return node()->getChild(beforeIdx_);
    return before_;
}
    
bool GrovePos::compareElemPos(const GrovePos& other) const
{
    return node_ == other.node_ && before_ == other.before_;    
}

Text* GrovePos::text() const
{
    RT_ASSERT(TEXT_POS == type() \
              && node() && Node::TEXT_NODE == node()->nodeType());
    return static_cast<Text*>(node_);
}

long GrovePos::idx() const
{
    if (0 == before_) {
        if (-1 != beforeIdx_)
            return beforeIdx_;
        else
            return node_->countChildren();
    }
    return before_->siblingIndex();
}

GroveLib::Node* GrovePos::contextNode() const
{
    if (ELEMENT_POS == type()) {
        Node* n = before();
        if (n)
            return n;
        else
            return node();
    }
    return node();
}

GroveLib::EntityReferenceStart* GrovePos::getErs() const
{
    switch (type()) {
        case ELEMENT_POS: {
            Node* n = before();
            if (n) {
                EntityReferenceStart* ers = get_ers(n);
                return (n->nodeType() == Node::ENTITY_REF_START_NODE) ?
                    ERS_CAST(ers->getSectParent()) : ers;
            } 
            return get_ers(node());
        }
        case TEXT_POS:
            return GroveLib::get_ers(node());
        default:
            return 0;
    }
}

GroveLib::RedlineSectionStart* GrovePos::getRedlineStart() const
{
    switch (type()) {
        case ELEMENT_POS: {
            Node* n = before();
            if (n) {
                RedlineSectionStart* rss = get_redline_section(n);
                if (n->nodeType() == Node::REDLINE_START_NODE) 
                    rss = static_cast<RedlineSectionStart*>
                        (rss->getSectParent());
                if (!rss)
                    return 0;
                return rss->getSectParent() ? rss : 0;
            } 
            return get_redline_section(node());
        }
        case TEXT_POS:
            return GroveLib::get_redline_section(node());
        default:
            return 0;
    }
}

GroveLib::Node* GrovePos::insert(GroveLib::Node* n) const
{
    Node* before_node = before();
    if (before_node)
        before_node->insertBefore(n);
    else
        node()->appendChild(n);
    return n;
}

GrovePos GrovePos::adjustBoundaryPos() const
{
    if (TEXT_POS != type()) 
        return *this;
    if (0 == idx())
        return GrovePos(text()->parent(), text());
    else if (text()->data().length() <= (uint)idx())
        return GrovePos(text()->parent(), text()->nextSibling());
    return *this;
}

GrovePos GrovePos::adjustChoice() const
{
    if (ELEMENT_POS != type() || !node())
        return *this;
    GrovePos adj_pos = node()->nodeType() == GroveLib::Node::CHOICE_NODE ?
        GrovePos(parentNode(node()), node()) : *this;
    Node* bn = adj_pos.before();
    if (!bn)
        return adj_pos;
    while (bn && bn->nodeType() == GroveLib::Node::CHOICE_NODE)
        bn = bn->nextSibling();
    return GrovePos(adj_pos.node(), bn);
}

bool GrovePos::operator==(const GrovePos& other) const
{
    return node_ == other.node_ &&
        before_ == other.before_ && beforeIdx_ == other.beforeIdx_;
}

bool GrovePos::operator!=(const GrovePos& other) const
{
    return (!operator==(other));
}

bool GrovePos::operator<(const GrovePos& other) const
{
    if (operator==(other))
        return false;
    return LESS == compare_pos(*this, other);
}

bool GrovePos::operator<=(const GrovePos& other) const
{
    if (operator==(other))
        return true;
    return LESS == compare_pos(*this, other);
}

bool GrovePos::operator>(const GrovePos& other) const
{
    if (operator==(other))
        return false;
    return GREATER == compare_pos(*this, other);
}

bool GrovePos::operator>=(const GrovePos& other) const
{
    if (operator==(other))
        return true;
    return GREATER == compare_pos(*this, other);
}

/////////////////////////////////////////////////////////////////

GroveSelection::GroveSelection()
    : start_(),
      end_()
{}

GroveSelection::GroveSelection(const GrovePos& pos)
    : start_(pos),
      end_(pos)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = GrovePos();
}

GroveSelection::GroveSelection(const GrovePos& start, const GrovePos& end)
    : start_(start),
      end_(end)
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = GrovePos();
}

GroveSelection::GroveSelection(const GroveSelection& sel)
    : start_(sel.start()),
      end_(sel.end())
{
    if (start_.isNull() || end_.isNull())
        start_ = end_ = GrovePos();
}

bool GroveSelection::operator==(const GroveSelection& other) const
{
    return (start_ == other.start() && end_ == other.end());
}

bool GroveSelection::operator!=(const GroveSelection& other) const
{
    return !operator==(other);
}

static bool adjust_text_pos(GrovePos& pos)
{
    if (GrovePos::TEXT_POS == pos.type()) {
        if (0 == pos.idx()) 
            pos = GrovePos(pos.node()->parent(), pos.node());
        else
            if (uint(pos.idx()) == pos.text()->data().length())
                pos = GrovePos(pos.node()->parent(), 
                               pos.node()->nextSibling());
        return true;
    }
    return false;
}

typedef GroveSectionStart GSS;
static const GSS* common_aos_gss(const GSS* gss1, const GSS* gss2)
{
    if (gss1 == gss2)
        return gss1;
    std::list<const GSS*> gss_list_1, gss_list_2;
    for (; gss1; gss1 = gss1->getSectParent())
        gss_list_1.push_front(gss1);
    for (; gss2; gss2 = gss2->getSectParent())
        gss_list_2.push_front(gss2);
    const GSS* common_gss = 0;
    while (!gss_list_1.empty() && !gss_list_2.empty()) {
        if (gss_list_1.front() != gss_list_2.front())
            break;
        common_gss = gss_list_1.front();
        gss_list_1.pop_front();
        gss_list_2.pop_front();
    }
    return common_gss;
}

typedef const GSS* (* GetGssFunc) (const GrovePos&);

static void balance_grove_section(GrovePos& min, GrovePos& max,
                                  GetGssFunc getGss)
{
    const GSS* common_gss = common_aos_gss(getGss(min), getGss(max));
    if (!common_gss) {
        const GSS* gss = 0;
        while ((gss = getGss(min))) {
            if (!gss->parent())
                break;
            min = GrovePos(gss->parent(), gss);
        }
        while ((gss = getGss(max))) {
            if (!gss->parent())
                break;
            max = GrovePos(gss->getSectEnd()->parent(), 
                           gss->getSectEnd()->nextSibling());
        }
        return;
    }
    while (!min.isNull()) {
        const GSS* min_gss = getGss(min);
        if (!min_gss || min_gss == common_gss)
            break;
        min = GrovePos(min_gss->parent(), min_gss);
    }
    while (!max.isNull()) {
        const GSS* max_gss = getGss(max);
        if (!max_gss || max_gss == common_gss)
            break;
        max = GrovePos(max_gss->getSectEnd()->parent(), 
                       max_gss->getSectEnd()->nextSibling());
    }
}

static const GSS* get_ers(const GrovePos& pos)
{
    return pos.getErs();
}

static const GSS* get_redline_start(const GrovePos& pos)
{
    return pos.getRedlineStart();
}

void GroveSelection::balance()
{
    if (isEmpty())
        return;
    //! Balance grove node positions
    const Node* common_aos = start_.node()->commonAos(end_.node());
    GrovePos min = minPos();
    while (min.node() && min.node() != common_aos) {
        if (min.node()->parent() == common_aos)
            if (adjust_text_pos(min))
                break;
        min = GrovePos(min.node()->parent(), min.node());
    }
    GrovePos max = maxPos();
    while (max.node() && max.node() != common_aos) {
        if (max.node()->parent() == common_aos)
            if (adjust_text_pos(max))
                break;
        max = GrovePos(max.node()->parent(), max.node()->nextSibling());
    }
    //! Balance grove section positions
    balance_grove_section(min, max, &get_ers);
    //! Balance redline positions
    balance_grove_section(min, max, &get_redline_start);
    //! Assign new borders
    if (start_ > end_) {
        start_ = max;
        end_ = min;
    }
    else {
        start_ = min;
        end_ = max;
    }
}

static inline bool is_attribute_pos(const GrovePos& gp)
{
    const Node* node = gp.contextNode();
    for (uint i = 0; node && i < 2; ++i, node = parentNode(node))
        if (Node::ATTRIBUTE_NODE == node->nodeType())
            return true;
    return false;
}

GroveSelection::Status GroveSelection::status() const
{
    if (isEmpty())
        return IS_EMPTY_SELECTION;
    if (isNull())
        return IS_NULL_SELECTION;
    if (is_attribute_pos(start_) || is_attribute_pos(end_))
        return IS_ATTRIBUTE_SELECTION;
    if (start_.getErs() != end_.getErs() ||
        start_.getRedlineStart() != end_.getRedlineStart())
        return IS_CROSS_SECTION;
    return IS_VALID_SELECTION;
}

const GrovePos& GroveSelection::minPos() const
{
    if (start_ < end_)
        return start_;
    else
        return end_;
}

const GrovePos& GroveSelection::maxPos() const
{
    if (end_ > start_)
        return end_;
    else
        return start_;
}

void GroveSelection::dump() const
{
    DBG(GROVEEDITOR.TEST) << "GroveSelection:" << std::endl;
    DBG_IF(GROVEEDITOR.TEST) minPos().dump();
    DBG_IF(GROVEEDITOR.TEST) maxPos().dump();
    DBG(GROVEEDITOR.TEST) << "--------------" << std::endl;
}

////////////////////////////////////////////////////////////////////////////

// Position replication

typedef Common::Vector<Node*> NodeList;

static bool get_node_list(const GroveLib::Node* src,
                          EntityReferenceStart* ers,
                          const EntityReferenceTable::ErtEntry* ert,
                          NodeList& nl)
{
    DBG(GROVEEDITOR.ESYNC) << "Replicating: ";
    DBG_EXEC(GROVEEDITOR.ESYNC, src->dump());
    nl.clear();
    nl.push_back(const_cast<Node*>(src));
    const Node* n = src;
    const Node* pn = n;
    COMMON_NS::Vector<ulong> loc;
    loc.reserve(64);
    for (;;) {
        uint pos = 0;
        pn = n->parent();
        while (n != ers) {
            n = n->prevSibling();
            if (0 == n)
                break;
            ++pos;
        }
        loc.push_back(pos);
        if (n)
            break;
        n = pn;
        if (0 == pn) {
            DBG(GROVEEDITOR.ESYNC) << "ESYNC error: can't reget ERS\n";
            return false;
        }
    }
    DBG_IF(GROVEEDITOR.ESYNC) {
        DBG(GROVEEDITOR.ESYNC) << "ESYNC: replicating ERS-TLOC: ";
        for (uint i = 0; i < loc.size(); ++i)
            DBG(GROVEEDITOR.ESYNC) << loc[i] << ' ';
        DBG(GROVEEDITOR.ESYNC) << std::endl;
    }
    EntityReferenceStart* es = 0;
    for (uint i = 0; i < ert->numOfRefs(); ++i) {
        es = ert->node(i);
        if (es == ers)
            continue;
        n = es;
        for (int l = loc.size() - 1; l >= 0; --l) {
            int steps = loc[l];
            while (steps-- > 0 && n)
                n = n->nextSibling();
            if (0 == n)
                break;
            if (l)
                n = n->firstChild();
            if (0 == n)
                break;
        }
        if (n && n->parent()->nodeType() != GroveLib::Node::ATTRIBUTE_NODE)
            nl.push_back(const_cast<Node*>(n));
    }
    // Verification
    for (uint i = 1; i < nl.size(); ++i) {
        n = nl[i];
        if (!n || src->nodeType() != n->nodeType() ||
            n->nodeName() != src->nodeName()) {
                nl.resize(i);
                return false;
        }
    }
    DBG(GROVEEDITOR.ESYNC) << "ESYNC: returning " << nl.size()
        << " positions" << std::endl;
    return true;
}

GROVEEDITOR_EXPIMP
bool get_pos_list(const GrovePos& p, GrovePosList& pl, bool eext)
{
    NodeList nl;
    pl.clear();
    bool isText = (p.type() == GrovePos::TEXT_POS);
    Node* src = isText ? p.text() : p.before();
    bool noBefore = false;
    if (!isText && !src) {
        src = p.node();
        noBefore = true;
    }
    if (0 == src) {
        pl.push_back(p);
        return false;
    }
    EntityReferenceStart* ers = get_ers(src);
    DBG(GROVEEDITOR.ESYNC) << "RPL: SRC: ";
    DBG_EXEC(GROVEEDITOR.ESYNC, src->dump());
    DBG(GROVEEDITOR.ESYNC) << "RPL: ERS: ";
    DBG_EXEC(GROVEEDITOR.ESYNC, ers->dump());
    if (src->nodeType() == Node::ENTITY_REF_START_NODE ||
        (eext && (src->nodeType() == Node::ENTITY_REF_END_NODE)))
            ers = static_cast<EntityReferenceStart*>(ers->getSectParent());
    if (0 == src->getGSR()->ert()) {
        pl.push_back(p);
        return false;
    }
    const EntityReferenceTable::ErtEntry* ert =
        src->getGSR()->ert()->lookup(ers->entityDecl());
    if (0 == ert || ert->numOfRefs() < 2) {
        pl.push_back(p);
        return false;
    }
    nl.reserve(ert->numOfRefs());
    if (!get_node_list(src, ers, ert, nl)) {
        pl.push_back(p);
        return false;
    }
    for (uint i = 0; i < nl.size(); ++i) {
        if (isText) {
            RT_ASSERT(nl[i]->nodeType() == Node::TEXT_NODE);
            pl.push_back(GrovePos(static_cast<Text*>(nl[i]), p.idx()));
        } else {
            if (noBefore)
                pl.push_back(GrovePos(nl[i]));
            else
                pl.push_back(GrovePos(nl[i]->parent(), nl[i]));
        }
    }
    return true;
}

GROVEEDITOR_EXPIMP
bool get_pos_list(const NodePtr& p, NodePosList& npl, bool eext)
{
    GrovePosList gpl;
    get_pos_list(GrovePos(p.pointer()), gpl, eext);
    npl.reserve(gpl.size());
    for (uint i = 0; i < gpl.size(); ++i)
        npl.push_back(gpl[i].node());
    return npl.size() > 1;
}

GROVEEDITOR_EXPIMP bool is_within_range(const GrovePos& pos,
                                        const GrovePos& from,
                                        const GrovePos& to,
                                        bool checkReplication)
{
    if (pos.isNull() || from.isNull() || to.isNull())
        return false;
    if (pos.getErs() == from.getErs() ||
        pos.getErs()->entityDecl() != from.getErs()->entityDecl())
            checkReplication = false;
    if (!checkReplication)
        return !(pos < from || pos > to);
    GrovePosList pl;
    if (!get_pos_list(pos, pl, true))
        return false;
    // find appropriate replicated pos
    uint i = 0;
    for (; i < pl.size(); ++i) 
        if (pl[i].getErs() == from.getErs())
            break;
    if (i >= pl.size())
        return false;
    return !(pl[i] < from || pl[i] > to);
}


} // namespace GroveEditor
