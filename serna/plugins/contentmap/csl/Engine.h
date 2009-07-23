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
#ifndef CSL_ENGINE_H_
#define CSL_ENGINE_H_

#include "common/String.h"

namespace Common {
    class Messenger;
}

namespace CatMgr {
    class CatalogManager;
}

namespace Xslt {
    class NumberCache;
}

namespace GroveLib {
    class Node;
    class Document;
}

namespace Csl {

class RootInstance;
class Stylesheet;
class Instance;

class InstanceInit {
public:
    RootInstance*        rootInst_;
    GroveLib::Node*      node_;
    Instance*            parent_;

    InstanceInit(RootInstance* root, GroveLib::Node* node, Instance* parent)
        : rootInst_(root), 
          node_(node),
          parent_(parent) {}
};

class InstanceWatcher {
public:
    virtual void notifyInstanceInserted(Instance*) {}
    virtual void notifyInstanceRemoved(Instance* /*parent*/,
                                       Instance* /*child*/) {}
    virtual ~InstanceWatcher() {}
};

typedef Instance* (*InstanceMaker)(const InstanceInit&);

class Engine {
public:
    // create engine object
    static Engine*      make(Common::Messenger* m = 0,
                             const CatMgr::CatalogManager* cat = 0);
    // set stylesheet
    virtual void        setStyle(const Common::String& url) = 0;
    
    // set item factory (must be called BEFORE transform)
    virtual void        setInstanceMaker(const InstanceMaker) = 0;
    
    // Set watcher for instance insert/remove. Note that lifetime
    // of instance watcher must be not less than of Engine.
    virtual void        setInstanceWatcher(InstanceWatcher*) = 0;
    
    // Set shared number cache
    virtual void        setNumberCache(Xslt::NumberCache*) = 0;
    
    // transform the source
    virtual Instance*   transform(const GroveLib::Document* document,
                                  const Common::String& profileName 
                                        = Common::String()) = 0;
    
    // process change updates
    virtual void        update() = 0;
    
    // for debug purposes
    virtual const Stylesheet* stylesheet() const = 0;
    virtual const Instance*   rootInstance() const = 0;
    virtual ~Engine() {}
};

} // namespace

#endif // CSL_ENGINE_H_
