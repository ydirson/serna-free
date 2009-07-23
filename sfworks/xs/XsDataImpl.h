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

#ifndef XS_DATA_IMPL_H_
#define XS_DATA_IMPL_H_

#include "xs/xs_defs.h"
#include "xs/XsData.h"
#include "xs/ComponentCred.h"
#include "xs/ComponentSpace.h"
#include "xs/ComponentRef.h"
#include "grove/Decls.h"
#include "grove/XNodePtr.h"
#include "common/String.h"
#include "common/OwnerPtr.h"
#include "common/ThreadingPolicies.h"
#include "grove/XmlName.h"

XS_NAMESPACE_BEGIN

class ExtPythonScript;
class SubstGroupMap;
class ImportMap;
class XsElementImpl;
class Particle;

/*! This is a part of XsData which is hidden from outside users.
 */
class XS_EXPIMP XsDataImpl : public XsData, public DEFAULT_THREADING<> {
public:
    typedef GROVE_NAMESPACE::NodeWithNamespace NodeWithNamespace;
    typedef GROVE_NAMESPACE::ExpandedName ExpandedName;
    typedef GROVE_NAMESPACE::Element Element;
    /*! Checks whether given node belongs to W3C Schema namespace
     */
    bool            isSchemaNode(const GROVE_NAMESPACE::NodeWithNamespace*) const;
    bool            isSchemaExtNode(const GROVE_NAMESPACE::NodeWithNamespace*) const;

    /*! Create general reference to qualified name \a qname for
        the component id \a cid.
     */
    ComponentRefBase*   makeRefBase(Schema* s,
                                    Element* elem,
                                    const COMMON_NS::String& qname,
                                    Component::ComponentType cid);

    void                parseSchema(GROVE_NAMESPACE::Element*, Schema* s);
    void                parse2(Schema* s);

    ExtPythonScript*    pyScript();

    void                insertSubstElement(ExpandedName name,
                                           XsElementImpl* elem);
    void                substGroup(ExpandedName name,
                                   COMMON_NS::Vector<XsElementImpl*>& grp);
    bool                putPieceIntoTable(Piece* p);
    bool                putComponentIntoTable(Component* c);

    ImportMap*          importMap() const { return importMap_; }
    NcnCred             makeCred(const COMMON_NS::String& qname,
                                 bool pvt = false) const;

    bool                processInclude(const GroveLib::Element* e, Schema*);

    ///////////////////////////////////////////////////////////

    void                resolveComponents();

    //!Find imported schema by uri
    Schema*             importedSchema(const String& uri);

#ifndef CXX_NO_MEMBER_TEMPLATES
    /*! Make component reference instance for a particular type
     */
    template <class T> void makeRefInst(T& rv,
                                        Schema* s,
                                        Element* elem,
                                        const COMMON_NS::String& qname,
                                        Component::ComponentType cid)
    {
        rv = makeRefBase(s, elem, qname, cid);
        references_.push_back(&rv);
    }
#else
    void makeRefInst(ComponentInstBase& rv,
                                        Schema* s,
                                        Element* elem,
                                        const COMMON_NS::String& qname,
                                        Component::ComponentType cid)
    {
            rv = makeRefBase(s, elem, qname, cid);
        references_.push_back(&rv);
    }
#endif
    ///////////////////////////////////////////////////////////

    virtual const COMMON_NS::String& targetNsUri() const;

    XsDataImpl(Schema *s);
    virtual ~XsDataImpl();

    XS_OALLOC(XsDataImpl);
    
    void                         joinSubstMap(XsDataImpl*);

private:
    friend class ::GroveAstParser;
    void                         initAtomicTypes();
    void                         init(Schema *s);
    void                         space_merge(ComponentSpaceBase* sp1,
                                             ComponentSpaceBase* sp2);

    ImportMap*                   importMap_;
    COMMON_NS::String                       schemaExtNs_;
    ExtPythonScript*             script_;
    SubstGroupMap*               substMap_;
    COMMON_NS::RefCntPtr<Particle>          rootParticle_;
    COMMON_NS::Vector<ComponentInstBase*> references_;
    COMMON_NS::Vector<Common::String> includes_;
    bool                         isParsed_;
    bool                         substParsed_;
};

#define XSD_IMPL(s) (s)->xsi()

XS_NAMESPACE_END

#endif // XS_DATA_IMPL_H_
