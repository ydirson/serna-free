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

#ifndef XSLT_APPLICATING_INSTANCE_H
#define XSLT_APPLICATING_INSTANCE_H

#include "xslt/xslt_defs.h"
#include "xslt/impl/Instance.h"
#include "xpath/NodeSet.h"
#include "common/CDList.h"

namespace Xslt {

class TemplateSelector;
class TemplateProvider;
class ApplicatingInstruction;

/*! \brief
 */
class ApplicatingInstance : public Instance,
                                        public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(ApplicatingInstance);

    struct UpdateListItem : public COMMON_NS::CDListItem<UpdateListItem> {
        COMMON_NS::RefCntPtr<TemplateSelector> ts;
    };
    typedef COMMON_NS::CDList<UpdateListItem> UpdateList;

    ApplicatingInstance(const Instruction* instruction,
                        const ApplicatingInstruction* appInstr,
                        const InstanceInit& init,
                        Instance* parentInst,
                        bool importsOnly);
    virtual ~ApplicatingInstance();

    // Processes the notification about resoure`s change
    void                notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);

    //! Reimplemented from Instance
    //!
    void                    update();
    void                    updateList();
    //
    bool                    importsOnly() const { return importsOnly_; }
    //
    virtual const Common::String& mode() const;
    //
    const TemplateProvider* templateProvider() const;
    //!
    void                    apply(const ResultContext& applyBefore);
    //!
    void                    dump() const;

    bool                    canFold() const { return canFold_; }

protected:
    virtual bool            mayHaveWithParams() const { return true; }
    virtual const ApplicatingInstance* asConstApplicatingInstance() const;

    //! Reimplemented from Instance
    virtual void            updateContext(int contextUpdateType);
    //!
    void                    clear(bool disposeResult = false);

private:
    void                        append_instance(const Xpath::NodeSetItem*,
                                                const ResultContext& rctx);
    void                        reselect();
    void                        remove_update(Instance*);

    UpdateList                  updateList_;
    Xpath::ValueHolderPtr       selectInst_;
    Xpath::NodeSet              nodeSet_;
    bool                        selectInstChanged_;
    const bool                  importsOnly_;
    char                        sectProcType_;
    bool                        canFold_;
};

} // namespace Xslt

#endif // XSLT_APPLICATING_INSTANCE_H
