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

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "xs/xs_defs.h"
#include "xs/Piece.h"
#include "common/RefCntPtr.h"
#include "common/prtti.h"
#include "grove/Decls.h"
#include "common/PropertyTree.h"

class Schema;

XS_NAMESPACE_BEGIN

class InputTokenSequence;
class Piece;
class ElementParticle;
class WildcardParticle;
class Connector;
class XsElement;
class FixupSet;
class FsmMatcher;
class Nfa;
class ChoiceParticle;
class GeneratedParticle;
class ContentToken;

/*! A particle - either a connector, wildcard or element model
 */
class Particle : public Piece {
public:
    static const ulong unbounded;     // unbounded value for maxOccur

    ulong       minOccur() const { return minOccur_; }
    ulong       maxOccur() const { return maxOccur_; }
    void        setMinOccur(Schema* s, ulong n);
    void        setMaxOccur(Schema* s, ulong n);

    /*! Create skeleton of the particle
     */
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe, FixupSet* elemSet,
                                 ulong addOccurs = 0) const = 0;
    virtual void    makeElement(Schema*,
                                GROVE_NAMESPACE::ElementPtr& /* result */,
                                GROVE_NAMESPACE::Element* /* pe */) const {}
    virtual void    dump(int indent) const = 0;
    virtual bool    cdataAllowed() const { return true; }
    virtual Common::PropertyNode* makeElemProperty() const { return 0; }

    PRTTI_DECL(ElementParticle);
    PRTTI_DECL(WildcardParticle);
    PRTTI_DECL(ChoiceParticle);
    PRTTI_DECL(GeneratedParticle);
    PRTTI_DECL(Connector);

    /*! test whether tokName exists in the particle (or matches it)
     *  and it's maxOccur is > 1, taking care of connectors etc.
     *  Returns number of elements in subgroup whose occurence count
     *  is larger than 1.
     */
    virtual int     checkSplit(const COMMON_NS::String& tokName, XsElement*&) const = 0;

    Common::RefCntPtr<Nfa> buildTokenNfa(FsmMatcher*, ContentToken*);

    Particle(const Origin& o, const XsId& xsid = XsId())
        : Piece(o, xsid), minOccur_(1), maxOccur_(1) {}
    virtual ~Particle() {}

    XS_OALLOC(Particle);

    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher* top) = 0;
    COMMON_NS::RefCntPtr<Nfa> quantize_nfa(Schema*, FsmMatcher*, Particle*);

private:
    ulong          minOccur_;
    ulong          maxOccur_;
};

XS_NAMESPACE_END

#endif // PARTICLE_H_
