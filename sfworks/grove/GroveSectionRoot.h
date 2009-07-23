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

#ifndef GROVE_SECTION_ROOT_
#define GROVE_SECTION_ROOT_

#include "grove/grove_defs.h"
#include "grove/SectionNodes.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"

GROVE_NAMESPACE_BEGIN

class EntityReferenceTable;
class EntityDecl;
class GroveVisitorBase;
class Element;

/*! This is a common class for nodes that are able to keep full subtrees
    of nodes along with entity reference tables and marked section tables,
    namely Document and DocumentFragment.
 */
class GROVE_EXPIMP GroveSectionRoot : public Node {
public:
    GroveSectionRoot(NodeType t);
    virtual ~GroveSectionRoot();

    /// Set new governing grove for GroveSectionRoot
    void    setGrove(Grove* g) { grove_ = g; }

    /// Get pointer to the governing grove
    Grove*  grove() const { return grove_; }

    /*! Register node visitor with this document section root. See
     * GroveVisitorBase class description for details.
     */
    void    registerVisitor(GroveVisitorBase* v);

    /// De-register grove visitor from this document section root.
    void    deregisterVisitor(GroveVisitorBase* v);

    /// Root of EntityReferenceStart tree of this GSR.
    EntityReferenceStart*   ers() const
    { 
        return static_cast<EntityReferenceStart*>(sections_[ENTITY_SECTION]);
    }
    /// Root of MarkedSectionStart tree of this GSR.
    MarkedSectionStart*     mss() const 
    { 
        return static_cast<MarkedSectionStart*>(sections_[MARKED_SECTION]); 
    }
    RedlineSectionStart*    rss() const
    {
        return static_cast<RedlineSectionStart*>(sections_[REDLINE_SECTION]); 
    }
    /// Access to entity reference table of this GSR.
    EntityReferenceTable*   ert() const;

    /*! Insert entity reference before \a beforeNode, taking content
     *  from \a master. If beforeNode is null, then append entity reference
     *  to the \a parent.
     */
    EntityReferenceStart* insertEntityRef(EntityReferenceStart* master,
                                          Node* beforeNode, Node* parent = 0);
    EntityReferenceStart* insertEntityRef(EntityDecl* decl, Node* data,
                                          Node* beforeNode, Node* parent = 0);
    /*! Update (synchronize) entity references to \a decl thorough whole grove,
     *  using entity reference \a master as a source of content.
     */
    void    updateEntityRefs(EntityReferenceStart* master);

    /////////////////////////////////////////////////////////////////////
    
    /*! Obtain a list of changed entity declarations in current GSR
     * relative to the entity references in gsr2.
     */
    bool    getChangedDecls(GroveSectionRoot* gsr2,
                            COMMON_NS::Vector<EntityDecl*>& v) const;

    PRTTI_DECL(GroveSectionRoot);
    
    /// internal
    void    releaseSectionData();

    static GroveSectionStart* makeSectionStart(GroveSectionType t);
    static GroveSectionEnd*   makeSectionEnd(GroveSectionType t);
    GroveSectionStart*        ensureSectionRoot(int t);

    GROVE_OALLOC(GSR);

private:
    // friends are used by Node(s) for sending notifications
    friend class Node;
    friend class Element;
    friend class Attr;
    friend class Text;
    friend class NodeWithNamespace;
    friend class GroveSectionStart;
    friend class GroveSectionEnd;

    void    gsr_nodeDestroyed(const Node* node) const;
    void    gsr_childInserted(const Node* n) const;
    void    gsr_childRemoved(const Node* node, const Node* child) const;
    void    gsr_genericNotify(const Node* node, void*) const;
    void    gsr_attributeChanged(const Attr* attr) const;
    void    gsr_attributeAdded(const Attr* attr) const;
    void    gsr_attributeRemoved(const Element* elem, const Attr* attr) const;
    void    gsr_textChanged(const Text* old) const;
    void    gsr_nsMappingChanged(const NodeWithNamespace*,
                                 const Common::String&) const;
    void    gsr_forceModified(const Node* node) const;

    GroveSectionRoot();
    virtual void dumpInherited() const;

    typedef COMMON_NS::Vector<GroveVisitorBase*> GvTable;

    Grove*                          grove_;
    GroveSectionStart*              sections_[MAX_SECTION_ID];
    COMMON_NS::OwnerPtr<GvTable>    vtab_;
};

////////////////////////////////////////////////////////////

GROVE_NAMESPACE_END

#endif // GROVE_SECTION_ROOT_
