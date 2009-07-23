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

#ifndef CHOICEELEMPARTICLE_H_
#define CHOICEELEMPARTICLE_H_

#include "xs/xs_defs.h"
#include "xs/complex/Particle.h"
#include "grove/ChoiceNode.h"

XS_NAMESPACE_BEGIN

class XsElement;

class ChoiceParticle : public Particle {
public:
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe,
                                 FixupSet* elemSet,
                                 ulong occurs = 0) const;
    virtual void    makeElement(Schema* s,
                                GROVE_NAMESPACE::ElementPtr& result,
                                GROVE_NAMESPACE::Element* pe) const;

    void            addElem(Common::PropertyNode* newProp);
    const Common::PropertyNode* elist() const { return elist_.root(); }

    virtual void    dump(int indent) const;

    ChoiceParticle(const Origin& o)
        : Particle(o) {}
    virtual ~ChoiceParticle();

    PRTTI_DECL(ChoiceParticle);
    XS_OALLOC(ChoiceParticle);

private:
    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher*);
    COMMON_NS::RefCntPtr<Nfa>  quantize_nfa(Schema*, FsmMatcher*, Particle*);
    virtual int     checkSplit(const String& tokName, XsElement*&) const;

    Common::PropertyTree elist_;
};

XS_NAMESPACE_END

#endif // CHOICEELEMPARTICLE_H_
