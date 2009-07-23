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

#ifndef ALL_CONNECTOR_H_
#define ALL_CONNECTOR_H_

#include "xs/xs_defs.h"
#include "xs/complex/Connector.h"
#include "xs/ComponentRef.h"

XS_NAMESPACE_BEGIN

class XsElement;
class Matcher;

/*! Application-specific information associated with particular
    schema construct.
 */
class AllConnector :  public Connector {
public:
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe,
                                 FixupSet* elemSet,
                                 ulong occurs = 0) const;
    virtual void dump(int indent) const;

    Matcher*     buildAllMatcher(Schema* s);

    AllConnector(ConnectorType type, const Origin& o, const XsId& id = XsId())
        : Connector(type, o, id) {}
    virtual ~AllConnector() {}

    PRTTI_DECL(AllConnector);
    XS_OALLOC(AllConnector);

private:
    friend class ::GroveAstParser;

    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher* top);
    virtual int     checkSplit(const String& tokName, XsElement*&) const;
};

////////////////////////////////////////////////////////////////////////

/*! Choice of particles (OR in terms of regexps)
 */
class ChoiceConnector :  public Connector {
public:
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe,
                                 FixupSet* elemSet,
                                 ulong occurs = 0) const;

    void            makeChoiceElem(Schema* s,
                                   GROVE_NAMESPACE::Node* attachTo,
                                   GROVE_NAMESPACE::Element* pe) const;

    virtual void dump(int indent) const;

    ChoiceConnector(ConnectorType t, const Origin& o, const XsId& id = XsId())
        : Connector(t, o, id) {}
    virtual ~ChoiceConnector() {}

    PRTTI_DECL(ChoiceConnector);
    XS_OALLOC(ChoiceConnector);

private:
    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher* top);
    virtual int checkSplit(const String& tokName, XsElement*&) const;
    COMMON_NS::RefCntPtr<Nfa> build_single_nfa(Schema* s, FsmMatcher* top);
};

//////////////////////////////////////////////////////////////

/*! A connector to model group reference
 */
class GroupConnector :  public Connector {
public:
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe,
                                 FixupSet* elemSet,
                                 ulong occurs = 0) const;

    virtual void    dump(int indent) const;

    GroupConnector(ConnectorType type, const Origin& o, const XsId& x = XsId())
        : Connector(type, o, x) {}
    virtual ~GroupConnector() {}
    Connector*  connector();
    virtual void makeSubst(Schema*);
    PRTTI_DECL(GroupConnector);
    XS_OALLOC(GroupConnector);

private:
    friend class ::GroveAstParser;

    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher* top);
    virtual int         checkSplit(const String& tokName, XsElement*&) const;
    void                setGroupRef(const XsGroupInst& grp);

    XsGroupInst         grp_;
};

//////////////////////////////////////////////////////////////

/*! Connector denoting sequence of particles
 */
class SequenceConnector :  public Connector {
public:
    virtual bool    makeSkeleton(Schema* s,
                                 GROVE_NAMESPACE::Node* attachTo,
                                 GROVE_NAMESPACE::Element* pe,
                                 FixupSet* elemSet,
                                 ulong occurs = 0) const;

    virtual void dump(int indent) const;

    SequenceConnector(ConnectorType type,
                      const Origin& o,
                      const XsId& xsid = XsId())
        : Connector(type, o, xsid) {}
    virtual ~SequenceConnector() {}

    PRTTI_DECL(SequenceConnector);
    XS_OALLOC(SeqConnector);

private:
    virtual COMMON_NS::RefCntPtr<Nfa> build_nfa(Schema*, FsmMatcher* top);
    virtual int checkSplit(const String& tokName, XsElement*& e) const;
    COMMON_NS::RefCntPtr<Nfa> build_single_nfa(Schema* s, FsmMatcher* top);
};

XS_NAMESPACE_END

#endif // XS_COMPLEX_CONNECTORS_H_
