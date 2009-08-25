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

#include "xslt/impl/debug.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/TopStylesheet.h"

USING_COMMON_NS;

namespace Xslt {

TemplateProvider::TemplateProvider(const Stylesheet* s)
    : stylesheet_(s)
{
}

TemplateProvider::~TemplateProvider()
{
}

static TemplateProvider::TemplateList empty_template_list;

TemplateProvider::TemplateIterator::TemplateIterator(const TemplateProvider* t,
                                   const TemplateList& list,
                                   const COMMON_NS::String& mode,
                                   bool  importsOnly)
    : tp_(t),
      startStylesheet_(t ? t->styleContext() : 0),
      current_(list.begin()),
      end_(list.end()),
      mode_(mode),
      importsOnly_(importsOnly)
{
}

static bool checkParent(const Stylesheet* start, const Stylesheet* s)
{
    do {
        if (start == s)
            return true;
        s = s->parentStylesheet();
    } while(s);
    return false;
}

const Template* TemplateProvider::TemplateIterator::nextTemplate()
{
    if (current_ == end_) {
        do {
            if (0 == tp_)
                return 0;
            const Stylesheet* st = tp_->styleContext()->nextImported();
            if (0 == st)
                return 0;
            if (importsOnly_ && !checkParent(startStylesheet_, st))
                return 0;
            tp_ = st->templateProvider();
            const TemplateList& tl = tp_->get_first(mode_);
            current_ = tl.begin();
            end_     = tl.end();
        } while(current_ == end_);
    }
    return *current_++;
}

const Template*
TemplateProvider::getTemplate(const COMMON_NS::String& name) const
{
    const TemplateProvider* tp = this;
    for (;;) {
        NamedTemplateMap::const_iterator it = tp->namedMap_.find(name);
        if (it != tp->namedMap_.end())
            return it->second;
        const Stylesheet* st = tp->styleContext()->nextImported();
        if (0 == st)
            break;
        tp = st->templateProvider();
    }
    return 0;
}

TemplateProvider::TemplateIterator
TemplateProvider::getFirstTemplate(const COMMON_NS::String& mode,
                                     bool  importsOnly) const
{
    const TemplateProvider* tp = 0;
    if (importsOnly) {
            const Stylesheet* s = styleContext()->nextImported();
            if (s)
                tp = s->templateProvider();
    } else
        tp = styleContext()->topStylesheet()->templateProvider();
    return TemplateIterator(tp, tp ? tp->get_first(mode) : empty_template_list,
        mode, importsOnly);
}

const TemplateProvider::TemplateList&
TemplateProvider::get_first(const COMMON_NS::String& mode) const
{
    ModeMap::const_iterator it = modeMap_.find(mode);
    if (it != modeMap_.end())
        return it->second;
    return empty_template_list;
}

// Needs optimization.
TemplateProvider::TemplateIterator
TemplateProvider::iteratorForTemplate(const Template* t, bool imp) const
{
    TemplateIterator ti = getFirstTemplate(t->mode(), imp);
    for (;;) {
        const Template* t2 = ti.nextTemplate();
        if (0 == t2 || t2 == t)
            break;
    }
    return ti;
}

bool TemplateProvider::registerTemplate(Template* tmpl)
{
    if (!tmpl->name().isEmpty()) {
        if (namedMap_.find(tmpl->name()) != namedMap_.end())
            return false;
        namedMap_[tmpl->name()] = tmpl;
        if (tmpl->matchExpr().isNull())
            return true;
    }
    TemplateList& tl = modeMap_[tmpl->mode()];
    if (tl.empty()) {
        tl.push_back(tmpl);
        return true;
    }
    TemplateList::iterator iPos(tl.end()), ti(tl.end());
    bool conflict = false;
    do {
        --ti;
        if (tmpl->priority() > (*ti)->priority())
            iPos = ti;
        if ((*ti)->matchExpr().isNull() || tmpl->matchExpr().isNull())
            continue;
        if (*(*ti)->matchExpr() == *tmpl->matchExpr()
            && (*ti)->mode() == tmpl->mode()) {
                conflict = true;
                break;
        }
    } while (ti != tl.begin());
    if (conflict)
        return false;
    if (iPos != tl.end())
        tl.insert(iPos, tmpl);
    else
        tl.push_back(tmpl);
    return true;
}

void TemplateProvider::dump() const
{
    DDINDENT;
    DDBG << "Template provider <" << styleContext()->origin()
        << ">:" << std::endl;
    DDBG << " NamedList: ";
    NamedTemplateMap::const_iterator ni = namedMap_.begin();
    for (; ni != namedMap_.end(); ++ni)
        DDBG << noind << ni->second->name() << ' ';
    DDBG << std::endl;
    ModeMap::const_iterator mi = modeMap_.begin();
    for (; mi != modeMap_.end(); ++mi) {
        DDBG << "ModeList <" << mi->first << ">: ";
        TemplateList::const_iterator ti = mi->second.begin();
        for (; ti != mi->second.end(); ++ti)
            DDBG << noind << (*ti)->name() << '/' << (*ti)->match()
                << '/' << (*ti)->priority() << ' ';
        DDBG << std::endl;
    }
}

} // namespace Xslt
