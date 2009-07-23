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
#ifndef CSL_INSTANCE_H_
#define CSL_INSTANCE_H_

#include "csl/SubInstance.h"
#include "xpath/NodeSet.h"
#include "grove/NodeVisitor.h"
#include "grove/udata.h"
#include "common/XTreeNode.h"
#include "common/RefCounted.h"
#include "common/OwnerPtr.h"

namespace Csl {

class InstanceInit;
class Template;
class Instance;
class Instruction;
class RootInstance;
class TemplateSelector;

typedef Common::XTreeNode<Instance, Instance> InstanceTreeBase;

/*! CslInstance is a instance of template instantiated for some
 *  particular node.
 */
class Instance : public Common::RefCounted<>,
                 public InstanceTreeBase,
                 public GroveLib::NodeVisitor {
    friend class COMMON_NS::XListItem<Instance, Instance>;
    friend class COMMON_NS::XList<Instance, Instance>;
public:
    typedef Common::XList<Instance, Instance> XLPT;
    class ModItem;
    
    Instance(const InstanceInit&);
    virtual ~Instance();
    
    enum CursorPlacement {
        CURSOR_INSIDE_END,      // after last child, or text_pos = text_len-1
        CURSOR_INSIDE_BEGIN,    // before first child, or text_pos = 0
        CURSOR_BEFORE,
        CURSOR_AFTER
    };
    CursorPlacement cursorPlacement() const;
    const GroveLib::Node*   cursorPosition() const;
    
    //! item text, if any
    const Common::String&   text() const { return text_; }
    const Template*         selectedTemplate() const { 
        return selectedTemplate_; }
    
    //! true if processing had been stopped at this level AND this
    //! instance has potential children
    bool            isOpen() const { return !(firstChild() && is_stopped()); }
    void            setOpen(bool v);
    bool            isStub() const { return isStub_; }
    //! origin in the source grove
    const GroveLib::Node* origin() const { return nsi_.node(); }

    //! current depth
    uint            depth() const { return depth_; }

    ////////////////////////////////////////////////////////////////

    virtual void    dump() const;

    const Xpath::NodeSetItem& nsi() const { return nsi_; }
    RootInstance*   rootInstance() const { return rootInstance_; }
    void            setDepth(uint new_depth);
    
protected:
    Instance(GroveLib::Node* node, RootInstance* ri);
    Instance*       newInstance(GroveLib::Node*);
    
    // called when text or children had changed
    virtual void    resultChanged() {}

private:
    Instance(const Instance&);
    Instance& operator=(const Instance&);
  
    static void     notifyNodeDestroyed(Instance*) {}
    static void     notifyChildInserted(Instance*) {}
    static void     notifyChildRemoved(XLPT*, Instance*);
    static void     xt_incRefCnt(Instance* n) { n->incRefCnt(); }
    static int      xt_decRefCnt(Instance* n) { return n->decRefCnt(); }
                                              
    // implementation of node visitor functions
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node* node);
    virtual void    childRemoved(const GroveLib::Node* node,
                                 const GroveLib::Node* child);
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}
    virtual void    textChanged(const GroveLib::Text*) {}
    virtual void    genericNotify(const GroveLib::Node*, void*) {}
    virtual void    forceModified(const GroveLib::Node*);
    
    void            subinstChanged();
    void            addModification(ModItem*);
    void            update(const ModItem&);
    void            instantiate_children();
    void            evaluate_subinsts();
    void            setTemplateSelector(TemplateSelector*);
    void            do_apply(int pass_type, bool is_top);
    void            reconnect(Instance* new_parent, Instance* after = 0);
    bool            is_stopped() const
    { return nsi_.node()->udata() & GroveLib::CM_FOLD_BIT; }
    void            set_stopped(bool v);
  
    friend class SubInstance;
    friend class TemplateSelector;
    friend class RootInstance;
    
    RootInstance*           rootInstance_;
    Xpath::NodeSetItem      nsi_;
    Common::String          text_;
    SubInstanceList         subInstances_;
    Common::OwnerPtr<TemplateSelector> templateSelector_;
    const Template*         selectedTemplate_;
    uint                    depth_ : 24;
    uint                    subinstChanged_ : 1;
    uint                    isStub_ : 1;
};    

Instance* csl_instance_origin(const GroveLib::Node* n);

} // namespace Csl

#endif // CSL_INSTANCE_H_
