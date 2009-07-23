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

#ifndef XSLT_TEMPLATE_H
#define XSLT_TEMPLATE_H

#include "xslt/xslt_defs.h"
#include "xslt/impl/PatternExprImpl.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "common/OwnerPtr.h"

namespace Xslt {

/*! \brief Template
 */

class Template : public Instruction {
public:
    XSLT_OALLOC(Template);

    Template(const GroveLib::Element* element,
             const Stylesheet* context,
             Instruction* p,
             bool  isBuiltin = false);
    virtual ~Template() {}

    Type                        type() const { return TEMPLATE; }
    const COMMON_NS::String&    mode() const { return mode_; }
    const COMMON_NS::String&    name() const { return name_; }
    const COMMON_NS::String&    match() const { return match_; }
    const PatternExprImplPtr&   matchExpr() const { return matchPattern_; }
    bool                        isDefaultPriority() const
    {
        return priority_.isNull();
    }
    double                      priority() const { return priorityNum_; }

    //! Reimplemented from Instruction
    Instance*                   makeInst(const InstanceInit& init,
                                         Instance* p) const;
    //! Reimplemented from Instruction
    void                        dump() const;

    // check whether this template can be possibly matched to nsi node
    bool    isApplicable(const Xpath::NodeSetItem&) const;

private:
    virtual bool                allowsChild(Type type) const;

    PatternExprImplPtr          matchPattern_;
    COMMON_NS::String           match_;
    COMMON_NS::String           name_;
    COMMON_NS::String           priority_;
    COMMON_NS::String           mode_;
    double                      priorityNum_;
};

} // namespace Xslt

#endif // XSLT_TEMPLATE_H
