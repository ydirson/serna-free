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
#include "xslt/xslt_defs.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/debug.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "qstringlist.h"

USING_COMMON_NS;

namespace Xslt {

static bool must_strip(const GroveLib::Text* tn)
{
    const GroveLib::Node* ns = tn->nextSibling();
    if (ns && ns->nodeType() == GroveLib::Node::ENTITY_REF_START_NODE) {
        const GroveLib::Node* ps = tn->prevSibling();
        if (ps && ps->nodeType() == GroveLib::Node::ENTITY_REF_END_NODE)
            return false;
    }
    const Char* cp = tn->data().unicode();
    const Char* ce = cp + tn->data().length();
    for (; cp < ce; ++cp)
        if (!cp->isSpace() || *cp == Char::nbsp)
            return false;
    return true;
}

// travel tree recursively, and strip text nodes which are needs to be stripped
void
WhitespaceStripper::stripTree(GroveLib::Element* e,
                              const GroveLib::NodeWithNamespace* nsRes)
{
    if (0 == e)
        return;
    bool strip = check_strip(e, nsRes);
    DBG(XSLT.WSTRIP) << "Node " << e->nodeName() << ": s" << strip << std::endl;
    GroveLib::Node* cn = e->firstChild();
    GroveLib::Node::NodeType t;
    GroveLib::Text* tn = 0;
    while (cn) {
        t = cn->nodeType();
        //debug: if (cn->nodeName() == "title")
        //     GroveLib::Node::dumpSubtree(cn);
        if (t == GroveLib::Node::ELEMENT_NODE)
            stripTree(static_cast<GroveLib::Element*>(cn), nsRes);
        if (!strip || t != GroveLib::Node::TEXT_NODE) {
            cn = cn->nextSibling();
            continue;
        }
        tn = static_cast<GroveLib::Text*>(cn);
        if (!must_strip(tn)) {
            cn = cn->nextSibling();
            continue;
        }
        DBG(XSLT.WSTRIP) << " - Stripping text child " << cn->siblingIndex()
            << std::endl;
        if (leaveSS_) {
            RefCntPtr<GroveLib::SSepNode> sn = new GroveLib::SSepNode;
            sn->setData(tn->data());
            cn->insertBefore(sn.pointer());
            cn->remove();
            cn = sn->nextSibling();
        } else {
            cn = tn->nextSibling();
            tn->remove();
        }
    }
}

void WhitespaceStripper::stripTree(GroveLib::Element* e)
{
    stripTree(e, nsResolver_);
}

void WhitespaceStripper::strip(GroveLib::Node* n) const
{
    WhitespaceStripper& ws = const_cast<WhitespaceStripper&>(*this);
    if (n->nodeType() == GroveLib::Node::ELEMENT_NODE) {
        ws.stripTree(static_cast<GroveLib::Element*>(n));
        return;
    }
    GroveLib::Node* cn = n->firstChild();
    GroveLib::Node* nn = 0;
    while (cn) {
        switch (cn->nodeType()) {
            case GroveLib::Node::TEXT_NODE: {
                if (!must_strip(static_cast<const GroveLib::Text*>(cn))) 
                    cn = cn->nextSibling();
                else {
                    nn = cn->nextSibling();
                    cn->remove();
                    cn = nn;
                }
                break;
            }
            case GroveLib::Node::ELEMENT_NODE: {
                ws.stripTree(static_cast<GroveLib::Element*>(cn));
                cn = cn->nextSibling();
                break;
            }
            default:
                cn = cn->nextSibling();
                break;
        }
    }
}

bool WhitespaceStripper::checkStrip(const GroveLib::Element* elem) const
{
    return const_cast<WhitespaceStripper*>(this)->
        check_strip(elem, nsResolver_);
}

bool
WhitespaceStripper::check_strip(const GroveLib::Element* e,
                                const GroveLib::NodeWithNamespace* nsRes)
{
    // first, check whether node is in cache
    bool* strip = 0;
    if (lookup_cache(e, strip))
        return *strip;      // we already know - strip it or not
    if (has_preserve(e, strip))
        return false;
    int prioStrip = sl_find(e, stripList_, nsRes);
    int prioPres  = sl_find(e, preserveList_, nsRes);
    DBG(XSLT.WSTRIP) << "prios for " << e->nodeName() << ": sp" << prioStrip
        << " pp" << prioPres << std::endl;
    *strip = prioStrip > prioPres;
    return *strip;
}

bool WhitespaceStripper::has_preserve(const GroveLib::Element* e,
                                      bool* strip) const
{
    for (;;) {
        if (0 == e)
            return false;
        if (e->nodeType() != GroveLib::Node::ELEMENT_NODE)
            return false;
        const GroveLib::Attr* a = e->attrs().getAttribute("xml:space");
        if (a) {
            if (a->value() == "preserve") {
                *strip = false;
                return true;
            } else
                return false;
        }
        if (0 == e->parent())
            return false;
        e = static_cast<const GroveLib::Element*>(e->parent());
    }
    return false;
}

int WhitespaceStripper::sl_find(const GroveLib::Element* e, StripList& sl,
                                const GroveLib::NodeWithNamespace* nsRes)
{
    // we encode both precedence and priority into "combined" priority.
    // priority == 0 means there's no match.
    StripList::const_iterator sli = sl.begin();
    for (; sli != sl.end(); ++sli)
        if (sli->matches(e, nsRes))
            return sli->precedence() | sli->priority() << 18; // see below
    return 0;
}

bool WhitespaceStripper::lookup_cache(const GroveLib::Element* e,
                                      bool*& strip)
{
    String localname = e->localName();
    String uri = e->xmlNsUri();
    StripCacheItem* sc = &cache_[uri];
    StripCacheItem::iterator sci = sc->find(localname);
    if (sci != sc->end()) {
        strip = &sci->second;
        return true;
    }
    strip = &(*sc)[localname];
    return false;
}

WhitespaceStripper::StripTest::StripTest(const COMMON_NS::String& pattern,
                                         int precedence)
  : prec_(precedence)
{
    int colonpos = pattern.find(':');
    if (colonpos < 0) {
        localname_ = pattern;
        prefix_ = "";
    } else {
        localname_ = pattern.mid(colonpos + 1);
        prefix_    = pattern.left(colonpos);
    }
    DBG(XSLT.WSTRIP) << "StripTest: added " << prefix_ << ":"
        << localname_ << std::endl;
}

int WhitespaceStripper::StripTest::priority() const
{
    if ("*" == localname_)
        return 1;
    return 2;
}

bool WhitespaceStripper::StripTest::matches(const GroveLib::Element* node,
                                const GroveLib::NodeWithNamespace* ns) const
{
    DBG(XSLT.WSTRIP) << "ST match [" << localname_ << "/" << prefix_
        << "]: " << node->xmlNsUri() << "/" << node->localName() << std::endl;
    if ("*" == localname_) {
        if (prefix_.isEmpty())
            return true;
    } else {
        if (node->localName() != localname_)
            return false;
    }
    if (prefix_.isEmpty())
        return true;
    return ns->getXmlNsByPrefix(prefix_) == node->xmlNsUri();
}

void WhitespaceStripper::add_pattern(const String& tlist,
                                     int prec, StripList& sl)
{
    QStringList qsl = QStringList::split(' ', tlist.simplifyWhiteSpace());
    for (QStringList::iterator it = qsl.begin(); it != qsl.end(); ++it)
        sl.push_back(StripTest(*it, prec));
}

void WhitespaceStripper::addPreserve(const String& tlist, int prec)
{
    add_pattern(tlist, prec, preserveList_);
}

void WhitespaceStripper::addStrip(const String& tlist, int prec)
{
    add_pattern(tlist, prec, stripList_);
}

void WhitespaceStripper::stripStyle(GroveLib::Element* docElem)
{
    if (stripList_.empty()) {
        addStrip("*");
        addPreserve("xsl:text");
    }
    stripTree(docElem, docElem);
}

// Expression for match precedence is:
// precedence << 20 | priority << 18 | (100000 - sequence)
WhitespaceStripper::WhitespaceStripper(const Stylesheet* ts, bool pss)
    : nsResolver_(ts->element()), leaveSS_(pss)
{
    int prec = 0;
    int seq = 0;
    for (;;) {
        if (0 == ts)
            break;
        const Instruction* i = ts->lastChild();
        for (; i; i = i->prevSibling()) {
            switch (i->type()) {
                default:
                    continue;
                case Instruction::PRESERVE_SPACE:
                    addPreserve(static_cast<const PreserveSpace*>
                        (i)->elements(), prec << 20 | (100000 - seq));
                    ++seq;
                    continue;
                case Instruction::STRIP_SPACE:
                    addStrip(static_cast<const StripSpace*>
                        (i)->elements(), prec << 20 | (100000 - seq));
                    ++seq;
                continue;
            }
        }
        ts = ts->nextImported();
        ++prec;
    }
}

WhitespaceStripper::WhitespaceStripper(bool pss)
    : leaveSS_(pss)
{
}

}
