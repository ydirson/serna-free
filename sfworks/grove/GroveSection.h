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
#ifndef GROVE_SECTION_H_
#define GROVE_SECTION_H_

#include "grove/Decls.h"
#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "grove/Node.h"

GROVE_NAMESPACE_BEGIN

class Grove;
class GroveSectionEnd;
class GroveSectionStart;

/*! GroveSectionStart represents base types for all nodes that represent
 *  "start-tag" nodes for all kinds of grove sections, such as marked
 *  sections and entities. The idea is that grove sections form a separate
 *  tree overlaid over it's grove, and provides shortcuts to grove nodes
 *  which represent grove sections.
 */
class GROVE_EXPIMP GroveSectionStart : public Node,
                        public COMMON_NS::XTreeNode<GroveSectionStart> {
    typedef COMMON_NS::XTreeNode<GroveSectionStart> GSN;
    friend class COMMON_NS::XTreeNode<GroveSectionStart>;
public:
    GroveSectionStart(NodeType t)
       : Node(t), sectEnd_(0) {}

    virtual ~GroveSectionStart();

    /// Get pointer to the first child section
    GroveSectionStart* getFirstSect() const { return GSN::firstChild(); }

    /// Get pointer to the next sibling section
    GroveSectionStart* getNextSect() const { return GSN::nextSibling(); }

    /// Get parent section pointer for a current section
    GroveSectionStart* getSectParent() const { return GSN::parent(); }

    /// Set new section parent
    void               setSectParent(GroveSectionStart* p)
    {
        GSN::setParent(p);
    }
    /// Remove current section node from it's parent
    void                sectRemove() { GSN::remove(); }

    /// Append new child section
    void                appendSect(GroveSectionStart* sect)
    {
        GSN::appendChild(sect);
    }
    /// Insert new child section \a sect before this one
    void                insertSectBefore(GroveSectionStart* sect)
    {
        GSN::insertBefore(sect);
    }
    /// Insert new child section \a sect after this one
    void                insertSectAfter(GroveSectionStart* sect)
    {
        GSN::insertAfter(sect);
    }
    /// Get "section end" pointer
    GroveSectionEnd*    getSectEnd() const { return sectEnd_; }

    /// Sets "end-tag" node for a current GroveSection
    void                setSectEnd(GroveSectionEnd* e) { sectEnd_= e; }
    
    /// Expand section (convert from section)
    void                convertFromSection();

    /// Convert range to section of type t
    static GroveSectionStart* convertToSection(GroveSectionType t,
                                               Node* from, Node* to,
                                               GroveSectionStart* gss = 0,
                                               GroveSectionEnd* gse = 0);
    /*! Recursively destroy subsection tree */
    void                destroySectSubtree()
    {
        GroveSectionStart* s;
        while ((s = getFirstSect())) {
            s->destroySectSubtree();
            s->sectRemove();
        }
    }
    /*! Debug-print current section data */
    void                dumpSectionInfo() const;

    /*! Recursively debug-print GroveSections subtree*/
    static void         dumpSectionTree(GroveSectionStart* root);
    
    virtual void    processTakeAsFragment(GroveSectionStart*) {}
    virtual void    processInsertFragment(GroveSectionStart*) {}
    virtual void    initTables() {}
    virtual void    attachSection() {}
    virtual void    detachSection() {}

    REDECLARE_XTREENODE_INTERFACE(Node);
    GROVE_OALLOC(GSS);

protected:
    virtual Node*       copy(Node*) const;
    virtual void        copy_sect_start(const GroveSectionStart*,
                                        Node* newParent) = 0;

    void                reconnectSectChildren(GroveSectionStart* s);
    GroveSectionEnd*    sectEnd_;
private:
    GroveSectionStart(const GroveSectionStart&);
};

/*
*/
class GROVE_EXPIMP GroveSectionEnd : public Node {
public:
    GroveSectionStart*  getSectStart() const { return sectStart_; }
    void                setSectStart(GroveSectionStart* s) { sectStart_ = s; }

    void                dumpSectionInfo() const;

    GroveSectionEnd(NodeType t)
        : Node(t), sectStart_(0) {}

    virtual ~GroveSectionEnd()
    {
        if (sectStart_)
            sectStart_->setSectEnd(0);
    }

    GROVE_OALLOC(GSE);

protected:
    virtual Node*       copy(Node*) const;
    GroveSectionStart*  sectStart_;
};
    
/// Check whether grove sections between \a n and \a endNode are balanced
GROVE_EXPIMP bool check_sections(const Node* n, const Node* endNode);

GROVE_NAMESPACE_END

#endif // GROVE_SECTION_H_
