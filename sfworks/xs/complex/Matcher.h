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

#ifndef XS_COMPLEX_MATCHER__H_
#define XS_COMPLEX_MATCHER__H_

#include "xs/xs_defs.h"
#include "common/Vector.h"
#include "grove/ChoiceNode.h"

class Schema;

XS_NAMESPACE_BEGIN

class Connector;
class InputTokenSequence;
class MatchState;
class XsElement;

/*! Interface to complex content matching engine (NFA/DFA/etc)
 */
class Matcher {
public:
    static Matcher* build(Schema* s, Connector* topParticle,
                          const COMMON_NS::String& name);

    /// Performs content matching.
    virtual bool    exactMatch(Schema* s,
                               InputTokenSequence& iseq,
                               MatchState& ms) const = 0;
    // List tokens which are possibly first in the content model
    virtual void    listFirstElems(Common::PropertyNode* el,
                        const GROVE_NAMESPACE::Element* node = 0) const = 0;
    virtual const XsElement*
                    getFirstElem(const GroveLib::ExpandedName& n) const = 0;

    virtual ~Matcher() {}
};

/*! This structure represents match context for match functions.
 */
class MatchState {
public:
    ulong  iseqStart;      // I: Where to start match in input sequence
    ulong  stopAt;         // I: at which offset in iseq stop matching
    ulong  matchedTokens;  // O: after match this contains # of matches
    bool   quickcheck;     // I: for test-matches - silent quick-check
    bool   editMode;       // I: edit mode: make choice elems, augment flags

    MatchState()
        : iseqStart(0), stopAt(~0), matchedTokens(0),
          quickcheck(false), editMode(false) {}
};

XS_NAMESPACE_END

#endif // XS_COMPLEX_MATCHER_H_
