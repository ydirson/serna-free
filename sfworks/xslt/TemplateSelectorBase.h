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
#ifndef XSLT_TEMPLATE_SELECTOR_BASE_H
#define XSLT_TEMPLATE_SELECTOR_BASE_H

#include "xslt/xslt_defs.h"
#include "grove/NodeVisitor.h"
#include "xpath/ExprContext.h"
#include "common/SubscriberPtr.h"

namespace Xslt {
    
class XSLT_EXPIMP TemplateSelectorBase : public Xpath::ExprContext,
                                         public GroveLib::NodeVisitor,
                                         public Common::SubscriberPtrWatcher {
public:
    virtual void nodeDestroyed(const GroveLib::Node*) {}
    virtual void childInserted(const GroveLib::Node*) { notifyChanged(0); }
    virtual void childRemoved(const GroveLib::Node*,
                              const GroveLib::Node*) { notifyChanged(0); }
    virtual void attributeChanged(const GroveLib::Attr*) { notifyChanged(0); }
    virtual void attributeRemoved(const GroveLib::Element*,
                                  const GroveLib::Attr*) { notifyChanged(0); }
    virtual void attributeAdded(const GroveLib::Attr*) { notifyChanged(0); }
    virtual void textChanged(const GroveLib::Text*) { notifyChanged(0); }
    virtual void genericNotify(const GroveLib::Node*, void*) {}

    virtual void registerVisitor(const GroveLib::Node* node,
                                 GroveLib::NodeVisitor*,
                                 short mask) const 
    {
        const_cast<GroveLib::Node*>(node)->registerNodeVisitor(self(), mask);
    }
    virtual bool watchFor(Common::SubscriberPtrPublisher* p) const
    {
        self()->wlist_.push_front
            (new Common::SubscriberWatcherProxy(self(), p));
        return true;
    }
    bool    hasWatchers() const           
    {
       return !wlist_.isEmpty() || firstNodeLink();
    }
    void    deregisterWatchers()
    { 
        wlist_.destroyAll();
        deregisterFromAllNodes();
    }
private:
    TemplateSelectorBase* self() const 
    {
        return const_cast<TemplateSelectorBase*>(this);
    }
    Common::SubscriberWatcherProxyList wlist_;
};

} // namespace Xslt

#endif // XSLT_TEMPLATE_SELECTOR_BASE_H
