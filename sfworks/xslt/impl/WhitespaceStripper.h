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
#ifndef XSLT_IMPL_WS_STRIPPER_H_
#define XSLT_IMPL_WS_STRIPPER_H_

#include "xslt/xslt_defs.h"
#include "common/String.h"
#include "grove/StripInfo.h"
#include "grove/Decls.h"
#include <list>
#include <map>

namespace Xslt {

class Stylesheet;

class WhitespaceStripper : public GroveLib::StripInfo {
public:
    /// constructs stripper from the instruction tree
    WhitespaceStripper(const Stylesheet* topStylesheet, bool leaveSS = true);
    /// constructs (empty) stripper
    WhitespaceStripper(bool leaveSS = false);

    /// Strip node tree according to stripping policy.
    void    stripTree(GroveLib::Element* top);

    /// Strip xsl style tree
    void    stripStyle(GroveLib::Element* docElem);

    /// Add "strip-whitespace" item
    void    addStrip(const COMMON_NS::String& nameTest, int prec = 0);

    /// Add "preserve-whitespace" item
    void    addPreserve(const COMMON_NS::String& nameTest, int prec = 0);

private:
    virtual void strip(GroveLib::Node*) const;

    class StripTest {
    public:
        StripTest(const COMMON_NS::String& pattern, int precedence);
        int     priority() const;
        bool    matches(const GroveLib::Element* e,
                        const GroveLib::NodeWithNamespace* nsResolver) const;
        int     precedence() const { return prec_; }

    private:
        int               prec_;
        COMMON_NS::String prefix_;
        COMMON_NS::String localname_;
    };
    typedef std::list<StripTest> StripList;
    typedef std::map<COMMON_NS::String, bool> StripCacheItem;
    typedef std::map<COMMON_NS::String, StripCacheItem> StripCache;

    virtual bool checkStrip(const GroveLib::Element* elem) const;
    void    stripTree(GroveLib::Element* top,
                      const GroveLib::NodeWithNamespace* nsResolver);
    void    add_pattern(const COMMON_NS::String& pattern, int, StripList&);
    int     sl_find(const GroveLib::Element* e, StripList& sl,
                    const GroveLib::NodeWithNamespace*);
    bool    check_strip(const GroveLib::Element* e,
                        const GroveLib::NodeWithNamespace* nsRes);
    bool    lookup_cache(const GroveLib::Element* e, bool*& strip);
    bool    has_preserve(const GroveLib::Element* e, bool* strip) const;

    const GroveLib::NodeWithNamespace* nsResolver_;
    StripList   preserveList_;
    StripList   stripList_;
    StripCache  cache_;
    bool        leaveSS_;
};

} // namespace Xslt

#endif  // XSLT_IMPL_WS_STRIPPER_H_

