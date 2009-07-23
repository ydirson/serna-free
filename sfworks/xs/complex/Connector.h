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

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "xs/xs_defs.h"
#include "xs/complex/Particle.h"
#include "xs/Piece.h"
#include "common/RefCntPtr.h"
#include "common/Vector.h"
#include "common/prtti.h"
#include "xs/Component.h"

class Schema;

XS_NAMESPACE_BEGIN

class AllConnector;
class ChoiceConnector;
class SequenceConnector;
class GroupConnector;
class ComplexContent;

/*! A model group connector. ALL connector may appear only on top
    level. GROUPREF is a special case of the connector - this
    means that if group reference was specified instead of connector,
    it should resolve thyself to SEQUENCE.
 */

class Connector : public Particle {
public:
    enum ConnectorType {
        UNKNOWN, ALL, CHOICE, SEQUENCE, GROUPREF
    };

    /*! Obtain a type for the connector.
     */
    ConnectorType       type() const;

    Connector(ConnectorType type, const Origin& o, const XsId& id = XsId())
        : Particle(o, id), type_(type), substDone_(false) {}
    virtual ~Connector() {}
    bool          substDone() { return substDone_; }
    virtual void dump(int indent) const = 0;
    void dumpChildren(int indent) const;
    virtual void makeSubst(Schema* schema);

    PRTTI_DECL(AllConnector);
    PRTTI_DECL(ChoiceConnector);
    PRTTI_DECL(SequenceConnector);
    PRTTI_DECL(GroupConnector);
    PRTTI_DECL(Connector);
    XS_OALLOC(Connector);

private:
    friend class ::GroveAstParser;
    friend class ComplexContent;

    Connector*          parent();
    void                copyContent(Connector* con);
    ConnectorType       type_;

protected:
    void                addChild(Particle* child);
    bool                check(Schema* schema, const GroveLib::Node* o,
                              const Connector* base) const;
    void                setParent(Connector* parent);
    
    Connector*          parent_;
    COMMON_NS::Vector<COMMON_NS::RefCntPtr<Particle> > children_;
    bool                substDone_;
};

XS_NAMESPACE_END

#endif // CONNECTOR_H_
