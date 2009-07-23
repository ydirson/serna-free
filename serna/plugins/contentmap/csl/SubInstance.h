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
#ifndef CSL_SUBINST_H_
#define CSL_SUBINST_H_

#include "csl/csl_defs.h"
#include "common/XList.h"
#include "common/SubscriberPtr.h"
#include "common/String.h"
#include "xpath/ExprContext.h"

namespace Csl {

class Instance;

class SubInstance : public Common::XListItem<SubInstance>,
                    public Common::SubscriberPtrWatcher,
                    public Xpath::ExprContext {
public:
    SubInstance()
        : isModified_(false) {}
        
    const Common::String& result() const { return result_; }
    Instance*             parentInst() const;
    virtual void          dump() const = 0;
    
    void            setModified();
    void            update() 
    { 
        if (isModified_) {
            doUpdate();
            isModified_ = false; 
        }
    }
    void notifyChanged(const Common::SubscriberPtrBase*) { setModified(); }
    
protected:
    virtual void    doUpdate() = 0;
    
    Common::String  result_;
    bool            isModified_;
    
private:
    SubInstance(const SubInstance&);
    SubInstance& operator=(const SubInstance&);
};

class SubInstanceList : public Common::OwnedXList<SubInstance> {};

} // namespace Csl

#endif // CSL_SUBINST_H_

