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

#ifndef INPUT_TOKEN_SEQUENCE_H_
#define INPUT_TOKEN_SEQUENCE_H_

#include "xs/xs_defs.h"
#include "common/Vector.h"
#include "common/RefCntPtr.h"
#include "xs/complex/ContentToken.h"
#include "grove/Decls.h"
#include "grove/XNodePtr.h"
#include "xs/complex/FsmMatcher.h"

class Schema;

namespace GroveLib {
    class ValidationCommandsMaker;
}

XS_NAMESPACE_BEGIN

class Particle;

class InputTokenSequence {
public:
    struct token_entry {
        COMMON_NS::RefCntPtr<QnameToken>   token;
        COMMON_NS::RefCntPtr<Particle>     matchedParticle; // returned by fsm
        GROVE_NAMESPACE::Element* element;          // if 0, then generated

        token_entry(QnameToken* tok, GROVE_NAMESPACE::Element* ep);

        XS_OALLOC(InpTokenSeq_entry);
    };
    enum buildFlags {
        noFlags = 00, cdataAllowed = 01, ignoreGhosts = 02
    };
    /*! Build input sequence from source Element children
     */
    bool            build(Schema* s,
                          GROVE_NAMESPACE::Element* parent,
                          int flags = noFlags, 
                          GroveLib::ValidationCommandsMaker* cm = 0);
    void            append(GROVE_NAMESPACE::Element* elem);
    void            insertBefore(Particle*, ulong idx,
                                 const FsmMatcher::DfaState* s = 0);
    void            dump() const;

    token_entry&    operator[](ulong index);
    ulong           size() const;

    GROVE_NAMESPACE::Element* parent() const;

    InputTokenSequence();
    ~InputTokenSequence();

    XS_OALLOC(InputTokenSeq);

private:
    COMMON_NS::Vector<token_entry*> entries_;
    GROVE_NAMESPACE::Element*       parent_;
    GroveLib::ChoiceNodeList        choiceList_;
    GroveLib::ValidationCommandsMaker* commandMaker_;
};

inline InputTokenSequence::token_entry&
  InputTokenSequence::operator[](ulong index)
{
    return *entries_[index];
}

inline ulong InputTokenSequence::size() const
{
    return entries_.size();
}

inline GROVE_NAMESPACE::Element* InputTokenSequence::parent() const
{
    return parent_;
}

XS_NAMESPACE_END

#endif // INPUT_TOKEN_SEQUENCE_H_
