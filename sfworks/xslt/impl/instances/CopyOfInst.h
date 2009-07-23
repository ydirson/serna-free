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
#ifndef XSLT_COPY_OF_INSTANCE_H_
#define XSLT_COPY_OF_INSTANCE_H_

#include "xslt/xslt_defs.h"
#include "xslt/impl/Instance.h"
#include "xpath/ValueHolder.h"
#include "grove/Decls.h"
#include "grove/NodeVisitor.h"
#include "common/STQueue.h"

namespace Xslt {

class CopyOf;
class NodeCopier;

class CopyOfInstance : public Instance,
                       public COMMON_NS::SubscriberPtrWatcher,
                       public GroveLib::NodeVisitor {
public:
    enum NotificationType {
        CHILD_INSERTED, CHILD_REMOVED, TEXT_CHANGED, ATTRIBUTE_CHANGED,
        ATTRIBUTE_ADDED, ATTRIBUTE_REMOVED
    };
    struct ModQueueItem : public COMMON_NS::STQueueItem<ModQueueItem> {
        NotificationType        nt;     // notification type
        NodeCopier*             nc;     // affected copier
        COMMON_NS::RefCntPtr<const GroveLib::Node> aux; // aux node
    };
    typedef COMMON_NS::STQueue<ModQueueItem> ModQueue;

    XSLT_OALLOC(CopyOfInstance);

    CopyOfInstance(const CopyOf* copy, const InstanceInit& init, Instance* p);
    virtual ~CopyOfInstance();

    // Reimplemented from Instance
    const InstanceResult*   firstResult() const;
    const InstanceResult*   lastResult() const;
    const InstanceResult*   selfResult() const { return firstResult(); }
    void                    disposeResult();

    // Processes the notification from selectInst
    void                    notifyChanged(const COMMON_NS::SubscriberPtrBase*);

    // Process updates
    virtual void            update();
    virtual void            updateContext(int);

    /// Handles notifictions from document fragments
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*);
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) {}
    virtual void    attributeAdded(const GroveLib::Attr*) {}
    virtual void    textChanged(const GroveLib::Text*) {}

private:
    friend class NodeCopier;

    typedef COMMON_NS::CDList<NodeCopier> TopCopierList;
    void    set_modified(NotificationType ntype,
                         NodeCopier* nc, const GroveLib::Node*);
    void    setInstanceResults();
    void    makeCopiers(const ResultContext&);
    NodeCopier* makeCopier(const GroveLib::Node* n, const ResultContext&);

    Xpath::ValueHolderPtr   selectInst_;
    ModQueue                modQueue_;
    InstanceResult          firstResult_;
    InstanceResult          lastResult_;
    TopCopierList           topInstances_;
    bool                    selectInstChanged_;
};

} // namespace

#endif // XSLT_COPY_OF_INSTANCE_H_
