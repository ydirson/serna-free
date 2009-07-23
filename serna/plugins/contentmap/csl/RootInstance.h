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
#ifndef CSL_ROOT_INSTANCE_H_
#define CSL_ROOT_INSTANCE_H_

#include "csl/Instance.h"
#include "csl/Engine.h"

namespace Xslt {
    class NumberCache;
}

namespace Csl {

class Stylesheet;
class Profile;
class InstanceWatcher;
    
class Instance::ModItem : public Common::XListItem<Instance::ModItem> {
public:
    USE_SUBALLOCATOR
    
    enum ModChangeType {
        CHILD_INSERTED = 0,  CHILD_REMOVED = 1, 
        SUBINST_CHANGED = 2, TEMPLATE_CHANGED = 3,
        RESULT_CHANGED = 4
    };
    ModChangeType   type_;
    Common::RefCntPtr<Instance> instance_;
    Common::RefCntPtr<GroveLib::Node> node_;
    ModItem(ModChangeType t, Instance* inst, GroveLib::Node* node = 0)
        : type_(t), instance_(inst), node_(node) {}
};
    
class RootInstance : public Instance {
public:
    enum PassType {
        OPEN_PASS = 0, FIRST_PASS = 1, UPDATE_PASS = 2
    };
    void                update();
    const Stylesheet*   stylesheet() const { return stylesheet_; }
    const Profile*      profile() const { return profile_; }
    virtual void        dump() const;

    InstanceWatcher*    instWatcher() const { return instWatcher_; }
    void                setInstanceWatcher(InstanceWatcher* w)
    { 
        instWatcher_ = w; 
    }
    const InstanceMaker& instanceMaker() const { return instanceMaker_; }
    Xslt::NumberCache*   numberCache() const { return numberCache_; }
  
    RootInstance(GroveLib::Node* docNode, 
                 const InstanceMaker instanceMaker,
                 const Stylesheet* stylesheet,
                 const Profile* profile,
                 Xslt::NumberCache*,
                 PassType passType);
    ~RootInstance();

protected:
    void    addModification(ModItem*, uint depth);
    void    removeModification(const Instance*, uint depth);

private:
    friend class Instance;
    class ModificationSet;

    InstanceMaker     instanceMaker_;
    const Stylesheet* stylesheet_;
    const Profile*    profile_;
    Common::OwnerPtr<ModificationSet> modSet_;
    InstanceWatcher*    instWatcher_;
    Xslt::NumberCache*  numberCache_;
};

} // namespace Csl

#endif // CSL_ROOT_INSTANCE_H_
